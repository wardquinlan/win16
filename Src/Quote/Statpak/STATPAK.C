//////////////////////////////////////////////////////////////////////////////
// STATPAK.C
//
// Statistics and memory management tool functions for use with the
// Quote package.
//
// Copyright (c) Ward Quinlan, 1996
//
//////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "statpak.h"
#include "resource.h"

#define TASKLIST    do
#define ENDTASKLIST while ( 0 );
#define QUIT        break

#define cbBufferMax ( 80 )
typedef char CHAR;

// Trading position, short or long
typedef enum 
    {
    posShort,
    posLong
    } Position;

static  HANDLE _hInstance;
int FAR PASCAL LibMain( HANDLE hInstance, 
                        WORD wDataSeg, 
                        WORD wHeapSize, 
                        LPSTR lpszCmdLine );
int FAR PASCAL _export WEP( int );

//////////////////////////////////////////////////////////////////////////////
// LibMain( )
//
// Library entrance procedure.
//////////////////////////////////////////////////////////////////////////////
int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine )
    {
    if ( wHeapSize > 0 )
        {
        UnlockData( 0 );
        }
    _hInstance = hInstance;
    return 1;
    }

//////////////////////////////////////////////////////////////////////////////
// WEP( )
//
// Windows Exit Procedure.
//////////////////////////////////////////////////////////////////////////////
int FAR PASCAL _export WEP( int nParam )
    {
    return 1;
    }

//////////////////////////////////////////////////////////////////////////////
// GAlloc( )
//
// General-purpose memory allocation routine.
//////////////////////////////////////////////////////////////////////////////
void FAR *FAR PASCAL _export GAlloc( UINT cb )
    {
    return (void FAR *) GlobalAllocPtr( GHND, cb );
    }

//////////////////////////////////////////////////////////////////////////////
// GFree( )
//
// General-purpose memory free routine.  It is safe to call this routine
// with NULL pointers; these requests result in no operation.
//////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export GFree( void FAR *lp )
    {
    if ( lp )
        {
        GlobalFreePtr( lp );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// DataSetZero( )
//
// Sets a dataset structure to zero.
//////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export DataSetZero( LPDATASET lpdataset )
    {
    _fmemset( lpdataset, 0, sizeof( DATASET ) );
    }

//////////////////////////////////////////////////////////////////////////////
// DataSetAlloc( )
//
// Allocates a dataset
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DataSetAlloc( LPDATASET lpdataset, WORD nPoints, WORD nOffset )
    {
    UINT cb;
    
    DataSetZero( lpdataset );
    cb = nPoints * sizeof( NUM );
    if ( cb )
        {
        lpdataset->lpnum = GAlloc( cb );
        if ( !lpdataset->lpnum )
            {
            return FALSE;
            }
        _fmemset( lpdataset->lpnum, 0, cb );
        }
    lpdataset->nPoints = nPoints;
    lpdataset->nOffset = nOffset;
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// DataSetFree( )
//
// Frees any memory allocated in the dataset, and then zeros it.
//////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export DataSetFree( LPDATASET lpdataset )
    {
    GFree( lpdataset->lpnum );
    DataSetZero( lpdataset );
    }

//////////////////////////////////////////////////////////////////////////////
// DataSetCopy( )
//
// Copies a dataset from Src to Dst.  The destination dataset
// is *not* freed before the copy is performed.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DataSetCopy( LPDATASET lpdatasetDst, LPDATASET lpdatasetSrc )
    {
    BOOL fRet = FALSE;
    UINT cb;

    TASKLIST
        {
        if ( !DataSetAlloc( lpdatasetDst, lpdatasetSrc->nPoints, lpdatasetSrc->nOffset ) )
            {
            QUIT;
            }

        cb = lpdatasetDst->nPoints * sizeof( NUM );
        _fmemcpy( lpdatasetDst->lpnum, lpdatasetSrc->lpnum, cb );
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// OptionsZero( )
//
// Sets options data to zero.
//////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export OptionsZero( LPOPTIONS lpoptions )
    {
    _fmemset( lpoptions, 0, sizeof( OPTIONS ) );
    }

//////////////////////////////////////////////////////////////////////////////
// OptionsAlloc( )
//
// Allocates options data
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export OptionsAlloc( LPOPTIONS lpoptions, HWND hwnd, WORD cbOptions, DWORD lOptions )
    {
    BOOL fRet = FALSE;
    
    OptionsZero( lpoptions );
    if ( cbOptions )
        {
        lpoptions->lOptions = (DWORD)(LPBYTE) GAlloc( cbOptions );
        if ( lpoptions->lOptions )
            {
            if ( lOptions )
                _fmemcpy( (LPBYTE) lpoptions->lOptions, (LPBYTE) lOptions, cbOptions );
            else
                _fmemset( (LPBYTE) lpoptions->lOptions, 0, cbOptions );
            lpoptions->cbOptions = cbOptions;
            lpoptions->hwndParent = hwnd;
            fRet = TRUE;
            }
        }
    else
        {
        lpoptions->cbOptions = 0;
        lpoptions->lOptions  = lOptions;
        lpoptions->hwndParent = hwnd;
        fRet = TRUE;
        }
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// OptionsFree( )
//
// Frees any memory associated with the dataset, then zeros it.
//////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export OptionsFree( LPOPTIONS lpoptions )
    {
    if ( lpoptions->cbOptions )
        {
        GFree( (LPBYTE) lpoptions->lOptions );
        }
    OptionsZero( lpoptions );
    }

//////////////////////////////////////////////////////////////////////////////
// OptionsCopy( )
//
// Copies options data from Src to Dst.  This routine will not
// free any data in Dst prior to use.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export OptionsCopy( LPOPTIONS lpoptionsDst, LPOPTIONS lpoptionsSrc )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {
        if ( !OptionsAlloc( lpoptionsDst, lpoptionsSrc->hwndParent, lpoptionsSrc->cbOptions, lpoptionsSrc->lOptions ) )
            {
            QUIT;
            }

        if ( lpoptionsSrc->cbOptions )
            {
            _fmemcpy( (LPBYTE) lpoptionsDst->lOptions, 
                      (LPBYTE) lpoptionsSrc->lOptions, 
                      lpoptionsSrc->cbOptions );
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// PeriodOptions( )
//
// Users call this routine to obtain a standard period length.
// If this routine returns TRUE, the period length will be
// in the LOWORD( lOptions ).  lOptions is used as data, not
// a pointer.  The HIWORD( lOptions ) is not used.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export PeriodOptions( LPOPTIONS lpopts )
    {
    return DialogBoxParam( _hInstance, MAKEINTRESOURCE( IDD_PERIODDLGPROC ), 
                           lpopts->hwndParent, PeriodDlgProc, (LPARAM) lpopts );
    }

//////////////////////////////////////////////////////////////////////////////
// PeriodDlgProc( )
//
// This is the dialog procedure to get the standard period options.
// The routine uses lOptions as data itself: LOWORD( lOptions ) ==
// the period length.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export PeriodDlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static LPOPTIONS lpopts;
    static HWND      hwndPeriod;
    static CHAR      szBuffer[ cbBufferMax + 1 ];
    int              nPeriod;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndPeriod = GetDlgItem( hDlg, IDC_PERIOD );
        SendMessage( hwndPeriod, EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );
        lpopts = (LPOPTIONS) lParam;
        nPeriod = (int) LOWORD( lpopts->lOptions );
        wsprintf( szBuffer, "%d", nPeriod );
        SetWindowText( hwndPeriod, szBuffer );
        SendMessage( hwndPeriod, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            SendMessage( hwndPeriod, WM_GETTEXT, sizeof( szBuffer ), (LPARAM)(LPCSTR) szBuffer );
            nPeriod = atoi( szBuffer );
            if ( nPeriod <= 0 || nPeriod >= 1024 )
                {
                MessageBox( hDlg, "Period must be in [1 .. 1023]", "Indicator Period", MB_ICONEXCLAMATION );
                SendMessage( hwndPeriod, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
                SetFocus( hwndPeriod );
                break;
                }
            lpopts->lOptions = MAKELONG( (WORD) nPeriod, 0u );
            EndDialog( hDlg, TRUE );
            break;
        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            break;
        case IDC_PERIOD:
            break;
            }
        return TRUE;
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// Options2( )
//
// Users call this routine to obtain 2 word-sized options from
// the user.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Options2( LPOPTIONS lpopts )
    {
    return DialogBoxParam( _hInstance, 
                           MAKEINTRESOURCE( IDD_PERIOD2DLGPROC ), 
                           lpopts->hwndParent, 
                           Options2DlgProc, 
                           (LPARAM) lpopts );
    }

//////////////////////////////////////////////////////////////////////////////
// Options2DlgProc( )
//
// This is the dialog procedure to obtain 2 word-sized options from
// the user.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Options2DlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static LPOPTIONS lpopts;
    static HWND      hwndPeriod;
    static HWND      hwndPeriod2;
    static CHAR      szBuffer[ cbBufferMax + 1 ];
    static CHAR      szBuffer2[ cbBufferMax + 1 ];
    int              nPeriod;
    int              nPeriod2;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndPeriod  = GetDlgItem( hDlg, IDC_PERIOD );
        hwndPeriod2 = GetDlgItem( hDlg, IDC_PERIOD2 );
        SendMessage( hwndPeriod,  EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );
        SendMessage( hwndPeriod2, EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );

        lpopts = (LPOPTIONS) lParam;
        
        nPeriod = (int) LOWORD( lpopts->lOptions );
        wsprintf( szBuffer, "%d", nPeriod );
        SetWindowText( hwndPeriod, szBuffer );
        SendMessage( hwndPeriod, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
        
        nPeriod2 = (int) HIWORD( lpopts->lOptions );
        wsprintf( szBuffer2, "%d", nPeriod2 );
        SetWindowText( hwndPeriod2, szBuffer2 );
        SendMessage( hwndPeriod2, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            SendMessage( hwndPeriod, WM_GETTEXT, sizeof( szBuffer ), (LPARAM)(LPCSTR) szBuffer );
            nPeriod = atoi( szBuffer );
            if ( nPeriod <= 0 || nPeriod >= 1024 )
                {
                MessageBox( hDlg, "Period must be in [1 .. 1023]", "Indicator Period", MB_ICONEXCLAMATION );
                SendMessage( hwndPeriod, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
                SetFocus( hwndPeriod );
                break;
                }
            
            SendMessage( hwndPeriod2, WM_GETTEXT, sizeof( szBuffer2 ), (LPARAM)(LPCSTR) szBuffer2 );
            nPeriod2 = atoi( szBuffer2 );
            if ( nPeriod2 <= -1 || nPeriod2 >= 1024 )
                {
                MessageBox( hDlg, "Param1 must be in [0 .. 1023]", "Param1", MB_ICONEXCLAMATION );
                SendMessage( hwndPeriod2, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
                SetFocus( hwndPeriod2 );
                break;
                }
            
            lpopts->lOptions = MAKELONG( (WORD) nPeriod, nPeriod2 );
            EndDialog( hDlg, TRUE );
            break;
        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            break;
        case IDC_PERIOD:
            break;
            }
        return TRUE;
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// Options3( )
//
// Users call this routine to obtain 3 word-sized options from
// the user.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Options3( LPOPTIONS lpopts )
    {
    return DialogBoxParam( _hInstance, 
                           MAKEINTRESOURCE( IDD_PERIOD3DLGPROC ), 
                           lpopts->hwndParent, 
                           Options3DlgProc, 
                           (LPARAM) lpopts );
    }

//////////////////////////////////////////////////////////////////////////////
// Options3DlgProc( )
//
// This is the dialog procedure to obtain 3 word-sized options from
// the user.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Options3DlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static LPOPTIONS lpopts;
    static HWND      hwndPeriod;
    static HWND      hwndPeriod2;
    static HWND      hwndPeriod3;
    static CHAR      szBuffer [ cbBufferMax + 1 ];
    static CHAR      szBuffer2[ cbBufferMax + 1 ];
    static CHAR      szBuffer3[ cbBufferMax + 1 ];
    WORD3            w3;
    LPWORD3          lpw3;
    int              nPeriod;
    int              nPeriod2;
    int              nPeriod3;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndPeriod  = GetDlgItem( hDlg, IDC_PERIOD );
        hwndPeriod2 = GetDlgItem( hDlg, IDC_PERIOD2 );
        hwndPeriod3 = GetDlgItem( hDlg, IDC_PERIOD3 );
        
        SendMessage( hwndPeriod,  EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );
        SendMessage( hwndPeriod2, EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );
        SendMessage( hwndPeriod3, EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );
        
        lpopts = (LPOPTIONS) lParam;
        if ( !lpopts->lOptions )
            {
            w3.w1 = 1u;
            w3.w2 = 1u;
            w3.w3 = 1u;
            if ( !OptionsAlloc( lpopts, hDlg, sizeof( WORD3 ), (DWORD) (LPWORD3) &w3 ) )
                {
                MessageBox( hDlg, "Statpak.dll - Out of Memory.", "Quote", MB_ICONEXCLAMATION );
                EndDialog( hDlg, FALSE );
                return TRUE;
                }
            }

        lpw3 = (LPWORD3) lpopts->lOptions;
        
        nPeriod  = (int) lpw3->w1;
        wsprintf( szBuffer, "%d", nPeriod );
        SetWindowText( hwndPeriod, szBuffer );
        SendMessage( hwndPeriod, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
        
        nPeriod2 = (int) lpw3->w2;
        wsprintf( szBuffer2, "%d", nPeriod2 );
        SetWindowText( hwndPeriod2, szBuffer2 );
        SendMessage( hwndPeriod2, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
        
        nPeriod3 = (int) lpw3->w3;
        wsprintf( szBuffer3, "%d", nPeriod3 );
        SetWindowText( hwndPeriod3, szBuffer3 );
        SendMessage( hwndPeriod3, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            SendMessage( hwndPeriod, WM_GETTEXT, sizeof( szBuffer ), (LPARAM)(LPCSTR) szBuffer );
            nPeriod = atoi( szBuffer );
            if ( nPeriod <= 0 || nPeriod >= 1024 )
                {
                MessageBox( hDlg, "Period must be in [1 .. 1023]", "Indicator Period", MB_ICONEXCLAMATION );
                SendMessage( hwndPeriod, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
                SetFocus( hwndPeriod );
                break;
                }
            SendMessage( hwndPeriod2, WM_GETTEXT, sizeof( szBuffer ), (LPARAM)(LPCSTR) szBuffer2 );
            nPeriod2 = atoi( szBuffer2 );
            if ( nPeriod2 <= -1 || nPeriod2 >= 1024 )
                {
                MessageBox( hDlg, "Param1 must be in [0 .. 1023]", "Param1", MB_ICONEXCLAMATION );
                SendMessage( hwndPeriod2, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
                SetFocus( hwndPeriod2 );
                break;
                }
            SendMessage( hwndPeriod3, WM_GETTEXT, sizeof( szBuffer ), (LPARAM)(LPCSTR) szBuffer3 );
            nPeriod3 = atoi( szBuffer3 );
            if ( nPeriod3 <= -1 || nPeriod3 >= 1024 )
                {
                MessageBox( hDlg, "Param2 must be in [0 .. 1023]", "Param2", MB_ICONEXCLAMATION );
                SendMessage( hwndPeriod3, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
                SetFocus( hwndPeriod3 );
                break;
                }
            
            lpw3 = (LPWORD3) lpopts->lOptions;
            lpw3->w1 = (WORD) nPeriod;
            lpw3->w2 = (WORD) nPeriod2;
            lpw3->w3 = (WORD) nPeriod3;
            EndDialog( hDlg, TRUE );
            break;
        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            break;
        case IDC_PERIOD:
            break;
            }
        return TRUE;
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ManageCtlColor( )
//
// This function is used to make the PeriodDlgBox be gray.
//////////////////////////////////////////////////////////////////////////////
HBRUSH FAR PASCAL _export ManageCtlColor( WPARAM wParam, LPARAM lParam )
    {
    WORD w = HIWORD( lParam );
    
    if ( w != CTLCOLOR_EDIT && w != CTLCOLOR_LISTBOX )
        {
        SetBkMode( (HDC) wParam, TRANSPARENT );
        return GetStockObject( LTGRAY_BRUSH );
        }
    return NULL;
    }

//////////////////////////////////////////////////////////////////////////////
// Rate1( )
//
// Arithmetic rate of change.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Rate1( LPDATASET lpdatasetResult, 
                               LPDATASET lpdatasetData,
                               WORD      nPeriod )
    {
    WORD iData;
    WORD iRes;
    BOOL fRet = FALSE;
    BOOL fError = FALSE;
    WORD nPoints;
    WORD nOffset;

    DataSetZero( lpdatasetResult );
    TASKLIST
        {
        if ( nPeriod < 1 || nPeriod >= lpdatasetData->nPoints )
            {
            QUIT;
            }
        
        nPoints = lpdatasetData->nPoints - nPeriod;
        nOffset = lpdatasetData->nOffset + nPeriod;
        if ( !DataSetAlloc( lpdatasetResult, nPoints, nOffset ) )
            {
            QUIT;
            }

        for ( iRes = 0; iRes < nPoints; iRes++ )
            {
            iData = iRes;

            lpdatasetResult->lpnum[ iRes ] =
                lpdatasetData->lpnum[ iData + nPeriod ] -
                lpdatasetData->lpnum[ iData ];
            }
        
        if ( fError )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST     
    
    if ( !fRet )
        {
        DataSetFree( lpdatasetResult );
        }

    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Rate2( )
//
// Geometric rate of change.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Rate2( LPDATASET lpdatasetResult, 
                               LPDATASET lpdatasetData,
                               WORD      nPeriod )
    {
    WORD iData;
    WORD iRes;
    BOOL fRet = FALSE;
    BOOL fError = FALSE;
    WORD nPoints;
    WORD nOffset;

    DataSetZero( lpdatasetResult );
    TASKLIST
        {
        if ( nPeriod < 1 || nPeriod >= lpdatasetData->nPoints )
            {
            QUIT;
            }
        
        nPoints = lpdatasetData->nPoints - nPeriod;
        nOffset = lpdatasetData->nOffset + nPeriod;
        if ( !DataSetAlloc( lpdatasetResult, nPoints, nOffset ) )
            {
            QUIT;
            }

        for ( iRes = 0; iRes < nPoints; iRes++ )
            {
            iData = iRes;
            if ( lpdatasetData->lpnum[ iData ] == (NUM) 0.0 )
                {
                fError = TRUE;
                break;
                }

            lpdatasetResult->lpnum[ iRes ] =
                lpdatasetData->lpnum[ iData + nPeriod ] /
                lpdatasetData->lpnum[ iData ];
            }
        
        if ( fError )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST     
    
    if ( !fRet )
        {
        DataSetFree( lpdatasetResult );
        }

    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// NewMinimum( )
//
// Returns 1 if point is a new minimum; 0 otherwise.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export NewMinimum( LPDATASET lpdatasetResult, 
                                    LPDATASET lpdatasetData,
                                    WORD      nPeriod )
    {
    BOOL    fRet = FALSE;
    BOOL    fNewLo;
    WORD    iRes;
    WORD    iData;
    WORD    iCur;
    WORD    nPoints;
    WORD    nOffset;

    TASKLIST
        {
        if ( nPeriod < 1 || nPeriod > lpdatasetData->nPoints )
            {
            QUIT;
            }

        nPoints = lpdatasetData->nPoints - nPeriod + 1;
        nOffset = lpdatasetData->nOffset + nPeriod - 1;
        if ( !DataSetAlloc( lpdatasetResult, nPoints, nOffset ) )
            {
            QUIT;
            }
        
        for ( iRes = 0; iRes < nPoints; iRes++ )
            {                              
            fNewLo = TRUE;
            iCur = iRes + nPeriod - 1;
            for ( iData = iRes; iData < iCur; iData++ )
                {
                // if any previous values are at least as low as the current,
                // then we aren't the lowest
                if ( lpdatasetData->lpnum[ iData ] <=
                     lpdatasetData->lpnum[ iCur ] )
                    {
                    fNewLo = FALSE;
                    break;
                    }
                }
            lpdatasetResult->lpnum[ iRes ] = fNewLo ? (NUM) 1.0 : (NUM) 0.0;
            }
        fRet = TRUE;
        }
    ENDTASKLIST     
    
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// NewMaximum( )
//
// Returns 1 if point is a new maximum; 0 otherwise.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export NewMaximum( LPDATASET lpdatasetResult, 
                                    LPDATASET lpdatasetData,
                                    WORD      nPeriod )
    {
    BOOL    fRet = FALSE;
    BOOL    fNewHi;
    WORD    iRes;
    WORD    iData;
    WORD    iCur;
    WORD    nPoints;
    WORD    nOffset;

    TASKLIST
        {
        if ( nPeriod < 1 || nPeriod > lpdatasetData->nPoints )
            {
            QUIT;
            }

        nPoints = lpdatasetData->nPoints - nPeriod + 1;
        nOffset = lpdatasetData->nOffset + nPeriod - 1;
        if ( !DataSetAlloc( lpdatasetResult, nPoints, nOffset ) )
            {
            QUIT;
            }
        
        for ( iRes = 0; iRes < nPoints; iRes++ )
            {                              
            fNewHi = TRUE;
            iCur = iRes + nPeriod - 1;
            for ( iData = iRes; iData < iCur; iData++ )
                {
                // if any previous values are at least as high as the current,
                // then we aren't the highest
                if ( lpdatasetData->lpnum[ iData ] >= 
                     lpdatasetData->lpnum[ iCur ] )
                    {
                    fNewHi = FALSE;
                    break;
                    }
                }
            lpdatasetResult->lpnum[ iRes ] = fNewHi ? (NUM) 1.0 : (NUM) 0.0;
            }
        fRet = TRUE;
        }
    ENDTASKLIST     
    
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// KFromPeriod( )
//
// Exponentional smoothing.  Given a period nPeriod, this routine
// finds the exponential-smoothing constant k:
//
//           2
// k  =  ----------
//       period + 1
//
//////////////////////////////////////////////////////////////////////////////
NUM FAR PASCAL _export KFromPeriod( WORD nPeriod )
    {
    return ( (NUM) 2.0 / ( (NUM) nPeriod + (NUM) 1.0 ) );
    }

//////////////////////////////////////////////////////////////////////////////
// PeriodFromK( )
//
// Exponential smoothing.  Given a k constant k, this routine finds
// the period length:
//
//           2
// period =  - - 1
//           k
//
//////////////////////////////////////////////////////////////////////////////
WORD FAR PASCAL _export PeriodFromK( NUM numK )
    {
    return (WORD) ( ( ( (NUM) 2.0 ) / numK ) - (NUM) 1.0 );
    }

//////////////////////////////////////////////////////////////////////////////
// Absolute( )
//
// Computes the absolute value of a dataset
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Absolute( LPDATASET lpdatasetResult,
                                  LPDATASET lpdatasetData )
    {                              
    WORD i;
    BOOL fRet = FALSE;
    TASKLIST
        {
        if ( !DataSetCopy( lpdatasetResult, lpdatasetData ) )
            break;

        for ( i = 0; i < lpdatasetResult->nPoints; i++ )
            {
            if ( lpdatasetResult->lpnum[ i ] < (NUM) 0.0 )
                lpdatasetResult->lpnum[ i ] = -lpdatasetResult->lpnum[ i ];
            }

        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Shift( )
//
// Shifts a dataset by n positions.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Shift( LPDATASET lpdatasetResult,
                               LPDATASET lpdatasetData,
                               WORD      nPeriod )
    {
    WORD nPoints;
    WORD nOffset;
    WORD iData;
    WORD iRes;
    BOOL fRet = FALSE;
    TASKLIST
        {
        if ( nPeriod < 1 || nPeriod >= lpdatasetData->nPoints )
            break;

        nPoints = lpdatasetData->nPoints - nPeriod;
        nOffset = lpdatasetData->nOffset + nPeriod;
        if ( !DataSetAlloc( lpdatasetResult, nPoints, nOffset ) )
            break;

        for ( iRes = 0; iRes < lpdatasetResult->nPoints; iRes++ )
            {
            iData = iRes;
            lpdatasetResult->lpnum[ iRes ] = lpdatasetData->lpnum[ iData ];
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// ExpSmooth( )
//
// Given a dataset and a smoothing constant k, this routine
// will find the resultant exponentially-smoothed dataset.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ExpSmooth( LPDATASET lpdatasetResult,
                                   LPDATASET lpdatasetData,
                                   WORD      nPeriod )
    {
    BOOL  fRet = FALSE;
    WORD  iData;
    WORD  iResult;
    WORD  nPointsResult;
    WORD  nOffsetResult;
    NUM   num;
    NUM   numPrev;
    NUM   numK;

    TASKLIST
        {
        if ( nPeriod == 0 )
            {
            return FALSE;
            }

        if ( nPeriod > lpdatasetData->nPoints )
            {
            return FALSE;
            }
        
        nPointsResult = lpdatasetData->nPoints - nPeriod + 1;
        nOffsetResult = lpdatasetData->nOffset + nPeriod - 1;
        if ( !DataSetAlloc( lpdatasetResult, nPointsResult, nOffsetResult ) )
            {
            QUIT;
            }

        for ( iData = 0; iData < nPeriod; iData++ )
            {
            num += lpdatasetData->lpnum[ iData ];
            }
        num /= nPeriod;
        lpdatasetResult->lpnum[ 0 ] = num;
        numPrev = num;
        numK = KFromPeriod( nPeriod );
        for ( iResult = 1; iResult < nPointsResult; iResult++ )
            {
            iData = iResult + nPeriod - 1;
            lpdatasetResult->lpnum[ iResult ] = ( lpdatasetData->lpnum[ iData ] - numPrev ) * numK +
                                          numPrev;
            numPrev = lpdatasetResult->lpnum[ iResult ];
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Mean( )
//
// Computes a dataset-mean for a period of nPeriod.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Mean( LPDATASET lpdatasetResult,
                              LPDATASET lpdatasetData,
                              WORD      nPeriod )
    {
    BOOL fRet = FALSE;
    WORD nPointsResult;
    WORD nOffsetResult;
    WORD iData;
    WORD iResult;
    NUM  num = (NUM) 0.0;

    TASKLIST
        {
        if ( nPeriod == 0 )
            {
            return FALSE;
            }

        if ( nPeriod > lpdatasetData->nPoints )
            {
            return FALSE;
            }
        
        nPointsResult = lpdatasetData->nPoints - nPeriod + 1;
        nOffsetResult = lpdatasetData->nOffset + nPeriod - 1;
        if ( !DataSetAlloc( lpdatasetResult, nPointsResult, nOffsetResult ) )
            {
            QUIT;
            }

        for ( iData = 0; iData < nPeriod; iData++ )
            {
            num += lpdatasetData->lpnum[ iData ];
            }
        num /= nPeriod;

        lpdatasetResult->lpnum[ 0 ] = num;
        for ( iResult = 1; iResult < nPointsResult; iResult++ )
            {
            iData = iResult + nPeriod - 1;
            lpdatasetResult->lpnum[ iResult ] = 
                lpdatasetResult->lpnum[ iResult - 1 ] +
                    ( lpdatasetData->lpnum[ iData ] - 
                      lpdatasetData->lpnum[ iData - nPeriod ] ) / nPeriod;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// StdDev( )
//
// Computes the standard deviation.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export StdDev( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod )
    {
    BOOL fRet = FALSE;
    WORD nPointsResult;
    WORD nOffsetResult;
    WORD iRes;
    WORD iData;
    WORD iMean;
    NUM  numSS;
    NUM  num;
    DATASET dsMean;
    
    TASKLIST
        {
        DataSetZero( &dsMean );
        if ( nPeriod < 2u )
            break;

        if ( nPeriod > lpdatasetData->nPoints )
            break;

        if ( !Mean( &dsMean, lpdatasetData, nPeriod ) )
            break;

        nPointsResult = dsMean.nPoints;
        nOffsetResult = dsMean.nOffset;
        if ( !DataSetAlloc( lpdatasetResult, nPointsResult, nOffsetResult ) )
            break;

        for ( iRes = 0; iRes < nPointsResult; iRes++ )
            {
            iMean = iRes;
            numSS = 0;
            for ( iData = iRes; iData < iRes + nPeriod; iData++ )
                {
                num    = lpdatasetData->lpnum[ iData ] - dsMean.lpnum[ iMean ];
                numSS += num * num;
                }
            lpdatasetResult->lpnum[ iRes ] = (NUM) sqrt( numSS / (NUM) ( nPeriod - 1 ) );
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    DataSetFree( &dsMean );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Log( )
//
// Computes the natural logarithm
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Log( LPDATASET lpdsResult, LPDATASET lpdsData )
    {
    WORD i;
    BOOL fRet   = FALSE;
    BOOL fError = FALSE;
    TASKLIST
        {
        if ( !DataSetAlloc( lpdsResult, lpdsData->nPoints, lpdsData->nOffset ) )
            break;
        for ( i = 0; i < lpdsData->nPoints; ++i )
            {
            if ( lpdsData->lpnum[ i ] <= (NUM) 0 )
                {
                fError = TRUE;
                break; // for
                }
            else
                {
                lpdsResult->lpnum[ i ] = (NUM) log( (double) lpdsData->lpnum[ i ] );
                }
            }
        if ( fError )
            break;
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// MinMax( )
//
// Computes the minimum or maximum between two values
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export MinMax( LPDATASET lpdsResult, LPDATASET lpdsA, LPDATASET lpdsB, BOOL fMin )
    {
    WORD iA;
    WORD iB;
    WORD iRes;
    WORD deltaA;
    WORD deltaB;
    WORD nPoints;
    WORD nOffset;
    BOOL fRet = FALSE;
    TASKLIST
        {
        nPoints = min( lpdsA->nPoints, lpdsB->nPoints );
        nOffset = max( lpdsA->nOffset, lpdsB->nOffset );
        if ( !DataSetAlloc( lpdsResult, nPoints, nOffset ) )
            break;

        deltaA = max( lpdsA->nOffset, lpdsB->nOffset ) - lpdsA->nOffset;
        deltaB = max( lpdsA->nOffset, lpdsB->nOffset ) - lpdsB->nOffset;
        
        for ( iRes = 0; iRes < lpdsResult->nPoints; ++iRes )
            {
            iA = iRes + deltaA,
            iB = iRes + deltaB;

            if ( fMin )
                lpdsResult->lpnum[ iRes ] = min( lpdsA->lpnum[ iA ], lpdsB->lpnum[ iB ] );
            else
                lpdsResult->lpnum[ iRes ] = max( lpdsA->lpnum[ iA ], lpdsB->lpnum[ iB ] );
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Power( )
//
// Computes A raised to the power of B
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Power( LPDATASET lpdsResult, LPDATASET lpdsA, 
                               LPDATASET lpdsB )
    {
    WORD iA;
    WORD iB;
    WORD iRes;
    WORD deltaA;
    WORD deltaB;
    WORD nPoints;
    WORD nOffset;
    BOOL fRet = FALSE;
    TASKLIST
        {
        nPoints = min( lpdsA->nPoints, lpdsB->nPoints );
        nOffset = max( lpdsA->nOffset, lpdsB->nOffset );
        if ( !DataSetAlloc( lpdsResult, nPoints, nOffset ) )
            break;

        deltaA = max( lpdsA->nOffset, lpdsB->nOffset ) - lpdsA->nOffset;
        deltaB = max( lpdsA->nOffset, lpdsB->nOffset ) - lpdsB->nOffset;
        
        for ( iRes = 0; iRes < lpdsResult->nPoints; ++iRes )
            {
            iA = iRes + deltaA,
            iB = iRes + deltaB;

            lpdsResult->lpnum[ iRes ] = (NUM) pow( (double) lpdsA->lpnum[ iA ], 
                                                   (double) lpdsB->lpnum[ iB ] );
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Exp( )
//
// Computes the exponential
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Exp( LPDATASET lpdsResult, LPDATASET lpdsData )
    {
    WORD i;
    BOOL fRet = FALSE;
    BOOL fError = FALSE;
    TASKLIST
        {
        if ( !DataSetAlloc( lpdsResult, lpdsData->nPoints, lpdsData->nOffset ) )
            break;
        for ( i = 0; i < lpdsData->nPoints; ++i )
            {
            if ( lpdsData->lpnum[ i ] > (NUM) 64.0 )
                {
                fError = TRUE;
                break;
                }
            else
                lpdsResult->lpnum[ i ] = (NUM) exp( (double) lpdsData->lpnum[ i ] );
            }
        if ( fError )
            break;
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Lowest( )
//
// Computes the lowest value for a given period.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Lowest( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod )
    {
    BOOL fRet = FALSE;
    WORD nPointsResult;
    WORD nOffsetResult;
    WORD iRes;
    WORD iData;
    NUM  numMin;

    TASKLIST
        {
        if ( nPeriod == 0 )
            break;

        if ( nPeriod > lpdatasetData->nPoints )
            break;

        nPointsResult = lpdatasetData->nPoints - nPeriod + 1;
        nOffsetResult = lpdatasetData->nOffset + nPeriod - 1;
        if ( !DataSetAlloc( lpdatasetResult, nPointsResult, nOffsetResult ) )
            break;

        for ( iRes = 0; iRes < nPointsResult; iRes++ )
            {                              
            iData = iRes;
            numMin = lpdatasetData->lpnum[ iData ];
            for ( iData = iRes + 1; iData < iRes + nPeriod; iData++ )
                {
                if ( lpdatasetData->lpnum[ iData ] < numMin )
                    numMin = lpdatasetData->lpnum[ iData ];
                }
            lpdatasetResult->lpnum[ iRes ] = numMin;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Highest( )
//
// Computes the highest value for a given period.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Highest( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod )
    {
    BOOL fRet = FALSE;
    WORD nPointsResult;
    WORD nOffsetResult;
    WORD iRes;
    WORD iData;
    NUM  numMax;

    TASKLIST
        {
        if ( nPeriod == 0 )
            break;

        if ( nPeriod > lpdatasetData->nPoints )
            break;

        nPointsResult = lpdatasetData->nPoints - nPeriod + 1;
        nOffsetResult = lpdatasetData->nOffset + nPeriod - 1;
        if ( !DataSetAlloc( lpdatasetResult, nPointsResult, nOffsetResult ) )
            break;

        for ( iRes = 0; iRes < nPointsResult; iRes++ )
            {                              
            iData = iRes;
            numMax = lpdatasetData->lpnum[ iData ];
            for ( iData = iRes + 1; iData < iRes + nPeriod; iData++ )
                {
                if ( lpdatasetData->lpnum[ iData ] > numMax )
                    numMax = lpdatasetData->lpnum[ iData ];
                }
            lpdatasetResult->lpnum[ iRes ] = numMax;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// LinReg( )
//
// This routine performs the linear regression calculation.
//
// Parameters:
//     lpdatasetResult - The resulting dataset of standard deviation.
//     lpdatasetData   - The dataset to use to calculate.
//     nPeriod         - The period length.
//
// Returns:
//     TRUE on success.
//     FALSE otherwise.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export LinReg( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod )
    {
    WORD nPoints;
    WORD nOffset;
    NUM  numxLg;
    NUM  numB0;
    NUM  numB1;
    NUM  numSx;  // E x
    NUM  numSy;  // E y
    NUM  numSxx; // E ( x * x )
    NUM  numSxy; // E ( x * y )
    NUM  numX;
    NUM  numY;
    NUM  numDen;
    WORD iData;
    WORD iLg;
    DATASET dsLG;
    BOOL fRet   = FALSE;
    BOOL fError = FALSE;

    TASKLIST
        {
        DataSetZero( &dsLG );
        
        if ( lpdatasetData->nPoints < nPeriod )
            break;

        if ( nPeriod < 2 )
            break;

        nPoints = lpdatasetData->nPoints - nPeriod + 1;
        nOffset = lpdatasetData->nOffset + nPeriod - 1;

        if ( !DataSetAlloc( &dsLG, nPoints, nOffset ) )
            break;

        for ( iLg = 0; iLg < dsLG.nPoints; iLg++ )
            {
            // We first calculate numB1
            numSx = numSy = numSxx = numSxy = (NUM) 0.0;
            numX = 0;
            for ( iData = iLg; iData < iLg + nPeriod; iData++ )
                {
                numY    = lpdatasetData->lpnum[ iData ];

                numSx  += numX;
                numSy  += numY;
                numSxx += numX * numX;
                numSxy += numX * numY;
                numX   += (NUM) 1.0;
                }
            numDen = ( numSxx - ( numSx * numSx ) / (NUM) nPeriod );
            if ( numDen == (NUM) 0.0 )
                {
                fError = TRUE;
                break;
                }

            numB1 = ( numSxy - ( numSx * numSy ) / (NUM) nPeriod ) / numDen;
            numB0 = ( numSy / nPeriod ) - ( numB1 * numSx / nPeriod );

            numxLg = (NUM) nPeriod - 1;
            dsLG.lpnum[ iLg ] = numB0 + ( numB1 * numxLg );
            }
        
        if ( fError )
            break;

        if ( !DataSetCopy( lpdatasetResult, &dsLG ) )
            break;
        
        fRet = TRUE;
        }
    ENDTASKLIST
    DataSetFree( &dsLG );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Correl( )
//
// This routine calculates the correlation coefficient between X and Y.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Correl( LPDATASET lpdatasetResult,
                                LPDATASET lpdsX,
                                LPDATASET lpdsY,
                                WORD      nPeriod )
    {
    NUM  numNum;
    NUM  numDen;
    NUM  numSS;
    WORD iRes;
    WORD iX;
    WORD iY;
    WORD iMeanX;
    WORD iMeanY;
    WORD nPoints;
    WORD nOffset;
    WORD nCount;
    WORD deltaX;
    WORD deltaY;
    DATASET dsMeanX;
    DATASET dsMeanY;
    DATASET dsStdevX;
    DATASET dsStdevY;
    BOOL fRet   = FALSE;
    BOOL fError = FALSE;
    
    TASKLIST
        {
        DataSetZero( &dsMeanX );
        DataSetZero( &dsMeanY );
        DataSetZero( &dsStdevX );
        DataSetZero( &dsStdevY );

        if ( nPeriod < 2u )
            break;

        if ( nPeriod > lpdsX->nPoints || nPeriod > lpdsY->nPoints )
            break;

        if ( !Mean( &dsMeanX, lpdsX, nPeriod ) )
            break;
        
        if ( !Mean( &dsMeanY, lpdsY, nPeriod ) )
            break;

        if ( !StdDev( &dsStdevX, lpdsX, nPeriod ) )
            break;

        if ( !StdDev( &dsStdevY, lpdsY, nPeriod ) )
            break;

        nPoints = min( dsMeanX.nPoints, dsMeanY.nPoints );
        nOffset = max( dsMeanX.nOffset, dsMeanY.nOffset );
        if ( !DataSetAlloc( lpdatasetResult, nPoints, nOffset ) )
            break;

        deltaX = max( lpdsX->nOffset, lpdsY->nOffset ) - lpdsX->nOffset;
        deltaY = max( lpdsX->nOffset, lpdsY->nOffset ) - lpdsY->nOffset;

        for ( iRes = 0; iRes < nPoints; iRes++ )
            {
            iMeanX = iRes + deltaX,
            iMeanY = iRes + deltaY;
            numSS = 0;
            for ( nCount = 0, 
                  iX     = iRes + deltaX, 
                  iY     = iRes + deltaY;
                  
                  nCount < nPeriod; 

                  nCount++,
                  iX++, 
                  iY++ )
                {
                numSS += ( lpdsX->lpnum[ iX ] - dsMeanX.lpnum[ iMeanX ] ) *
                         ( lpdsY->lpnum[ iY ] - dsMeanY.lpnum[ iMeanY ] );
                }
            numNum = numSS / (NUM) ( nPeriod - 1 );
            numDen = dsStdevX.lpnum[ iMeanX ] * dsStdevY.lpnum[ iMeanY ];
            if ( numDen == (NUM) 0.0 )
                {
                fError = TRUE;
                break;
                }
            lpdatasetResult->lpnum[ iRes ] = numNum / numDen;
            }
        if ( fError )
            break;
        fRet = TRUE;
        }
    ENDTASKLIST
    DataSetFree( &dsMeanX );
    DataSetFree( &dsMeanY );
    DataSetFree( &dsStdevX );
    DataSetFree( &dsStdevY );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Rsi
//
// Relative Strength Index calculation
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Rsi( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod )
    {
    BOOL    fError = FALSE;
    BOOL    fRet = FALSE;
    WORD    nPointsData;
    WORD    i;
    NUM     numMean;
    DATASET datasetPC;
    DATASET datasetNC;
    DATASET datasetP;
    DATASET datasetN;
    DATASET datasetRSI;

    TASKLIST
        {
        DataSetZero( &datasetPC  );
        DataSetZero( &datasetNC  );
        DataSetZero( &datasetP   );
        DataSetZero( &datasetN   );
        DataSetZero( &datasetRSI );

        // 1 <= nPeriod <= 1023
        if ( nPeriod < 1 || nPeriod > 1023 )
            {
            QUIT;
            }

        if ( nPeriod >= lpdatasetData->nPoints )
            {
            QUIT;
            }

        nPointsData = lpdatasetData->nPoints;
        if ( !DataSetAlloc( &datasetPC, nPointsData - 1, 1 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetNC, nPointsData - 1, 1 ) )
            {
            QUIT;
            }
        
        if ( !DataSetAlloc( &datasetP, nPointsData - nPeriod, nPeriod  + 1 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetN, nPointsData - nPeriod, nPeriod + 1 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetRSI, nPointsData - nPeriod, nPeriod ) )
            {
            QUIT;
            }

        for ( i = 0; i < datasetPC.nPoints; i++ )
            {
            datasetPC.lpnum[ i ] = max( lpdatasetData->lpnum[ i + 1 ] - lpdatasetData->lpnum[ i ], (NUM) 0.0 );
            datasetNC.lpnum[ i ] = max( lpdatasetData->lpnum[ i ] - lpdatasetData->lpnum[ i + 1 ], (NUM) 0.0 );
            }

        numMean = (NUM) 0.0;
        for ( i = 0; i < nPeriod; i++ )
            {
            numMean += datasetPC.lpnum[ i ];
            }
        numMean /= (NUM) nPeriod;

        datasetP.lpnum[ 0 ] = numMean;
        for ( i = 1; i < datasetP.nPoints; i++ )
            {
            NUM num;
            num  = datasetP.lpnum[ i - 1 ];
            num *= ( nPeriod - 1 );
            num += datasetPC.lpnum[ i + nPeriod - 1 ]; // PC and P are offset by nPeriod - 1
            num /= (NUM) nPeriod;
            
            datasetP.lpnum[ i ] = num;
            }

        numMean = (NUM) 0.0;
        for ( i = 0; i < nPeriod; i++ )
            {
            numMean += datasetNC.lpnum[ i ];
            }
        numMean /= (NUM) nPeriod;

        datasetN.lpnum[ 0 ] = numMean;
        for ( i = 1; i < datasetN.nPoints; i++ )
            {
            NUM num;
            num  = datasetN.lpnum[ i - 1 ];
            num *= ( nPeriod - 1 );
            num += datasetNC.lpnum[ i + nPeriod - 1 ]; // NC and N are offset by nPeriod - 1
            num /= (NUM) nPeriod;

            datasetN.lpnum[ i ] = num;
            }

        for ( i = 0; i < datasetRSI.nPoints && !fError; i++ )
            {
            NUM numRS;
            NUM numRSI;

            if ( datasetN.lpnum[ i ] == 0.0 )
                {
                // catch divide by zero
                fError = TRUE;
                break;
                }
            numRS  = datasetP.lpnum[ i ] / datasetN.lpnum[ i ];
            numRSI = (NUM) 1.0 + numRS;
            numRSI = (NUM) 100.0 / numRSI;
            numRSI = (NUM) 100.0 - numRSI;
            datasetRSI.lpnum[ i ] = numRSI;
            }

        if ( fError )
            {
            QUIT;
            }

        if ( !DataSetCopy( lpdatasetResult, &datasetRSI ) )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST

    DataSetFree( &datasetPC  );
    DataSetFree( &datasetNC  );
    DataSetFree( &datasetP   );
    DataSetFree( &datasetN   );
    DataSetFree( &datasetRSI );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Vi
//
// Volatility Index calculation
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Vi( LPDATASET lpdsResult, 
                LPDATASET lpdsHi, 
                LPDATASET lpdsLo,
                LPDATASET lpdsCl,
                WORD      nPeriod )
    {
    DATASET dsTR;
    BOOL fRet = FALSE;
    
    TASKLIST
        {
        WORD iCl;

        // 1 <= nPeriod <= 1023
        if ( nPeriod < 1 || nPeriod > 1023 )
            {
            QUIT;
            }

        if ( lpdsCl->nPoints <= 1 )
            break;
        
        if ( lpdsCl->nPoints != lpdsHi->nPoints ||
             lpdsCl->nPoints != lpdsLo->nPoints )
            break;
        
        if ( !DataSetAlloc( &dsTR, lpdsCl->nPoints - 1, 1 ) )
            break;

        for ( iCl = 1; iCl < lpdsCl->nPoints; ++iCl )
            {
            const WORD iHi = iCl;
            const WORD iLo = iCl;
            const WORD iTr = iCl - 1;
            
            NUM num = (NUM) fabs( (double) ( lpdsHi->lpnum[ iHi ] - lpdsLo->lpnum[ iLo ] ) );
            num = max( (NUM) fabs( (double) ( lpdsHi->lpnum[ iHi ] - lpdsCl->lpnum[ iCl - 1 ] ) ), num );
            num = max( (NUM) fabs( (double) ( lpdsLo->lpnum[ iLo ] - lpdsCl->lpnum[ iCl - 1 ] ) ), num );            
            
            dsTR.lpnum[ iTr ] = num;
            }

        fRet = Mean( lpdsResult, &dsTR, nPeriod );
        DataSetFree( &dsTR );
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Sar( )
//
// SAR calculation routine
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Sar( LPDATASET lpdatasetResult, 
                             LPDATASET lpdatasetHi,
                             LPDATASET lpdatasetLo,     
                             NUM numAccel,
                             NUM numAccelMax )
    {
    BOOL         fRet = FALSE;
    BOOL         fFirst;
    Position     pos;
    WORD         iHiLo;
    DATASET      dsRes;
    WORD         iRes;
    NUM          numExtreme;
    NUM          numSar;
    NUM          numHi;
    NUM          numLo;
    NUM          numAccelCur;
    
    TASKLIST
        {
        // initialize our result dataset
        DataSetZero( &dsRes );

        // 0 < numAccel < numAccelMax
        if ( numAccel <= 0 )
            break;
        if ( numAccel >= numAccelMax )
            break;

        // do some sanity checks
        if ( lpdatasetLo->nPoints != lpdatasetHi->nPoints )
            break;
        if ( lpdatasetHi->nPoints <= 2 )
            break;

        // allocate our result dataset
        if ( !DataSetAlloc( &dsRes, lpdatasetHi->nPoints - 2, 2 ) )
            break;
        
        // we start out 'short', just having switched positions
        // into a long position
        numExtreme  = min( lpdatasetLo->lpnum[ 0 ], lpdatasetLo->lpnum[ 1 ] );
        pos         = posLong;
        fFirst      = TRUE;
        
        // main loop
        for ( iHiLo = 2; iHiLo < lpdatasetHi->nPoints; iHiLo++ )
            {
            iRes  = iHiLo - 2;
            numHi = lpdatasetHi->lpnum[ iHiLo ];
            numLo = lpdatasetLo->lpnum[ iHiLo ];
            
            if ( fFirst )
                {
                // SAR is previous extreme price
                numSar = numExtreme;
                
                // calculate tomorrow's acceleration
                numAccelCur = numAccel;
                
                if ( pos == posLong )
                    numExtreme = numHi;
                else
                    numExtreme = numLo;
                    
                fFirst = FALSE;
                }
            else
                {
                if ( pos == posLong )
                    {
                    BOOL f = ( numHi > numExtreme );
                    if ( f )
                        numExtreme = numHi;
                    
                    numSar = numSar + numAccelCur * ( numExtreme - numSar );
                    
                    // calculate tomorrow's acceleration
                    if ( f )
                        numAccelCur = min( numAccelCur + numAccel, numAccelMax );
                    }
                else
                    {
                    BOOL f = ( numLo < numExtreme );
                    if ( f )
                        numExtreme = numLo;

                    numSar = numSar + numAccelCur * ( numExtreme - numSar );
                    
                    // calculate tomorrow's acceleration
                    if ( f )
                        numAccelCur = min( numAccelCur + numAccel, numAccelMax );
                    }
                }
            
            if ( pos == posLong )
                {
                numSar = min( numSar, lpdatasetLo->lpnum[ iHiLo - 2 ] );
                numSar = min( numSar, lpdatasetLo->lpnum[ iHiLo - 1 ] );
                }
            else
                {
                numSar = max( numSar, lpdatasetHi->lpnum[ iHiLo - 2 ] );
                numSar = max( numSar, lpdatasetHi->lpnum[ iHiLo - 1 ] );
                }
                
            dsRes.lpnum[ iRes ] = numSar;
            
            // check for price penetration
            if ( pos == posLong ) 
                {
                if ( numSar >= numLo )
                    {
                    pos    = posShort;
                    fFirst = TRUE;
                    }
                }
            else
                {
                if ( numSar <= numHi )
                    {
                    pos    = posLong;
                    fFirst = TRUE;
                    }
                }
            }
        
        // copy the results
        if ( !DataSetCopy( lpdatasetResult, &dsRes ) )
            break;
        fRet = TRUE;
        }
    ENDTASKLIST
    DataSetFree( &dsRes );

    return fRet;
    }


