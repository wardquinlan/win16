//////////////////////////////////////////////////////////////////////////////
// SAR.CPP
//
// Parabolic-SAR Indicator
//
// This indicator is for use with the Quote package.
//
// This is an implementation of the Parabolic-SAR
// indicator, developed by J. Welles Wilder.
//////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <statpak.h>
#include "sar.hxx"

#define _cbBufferMax 128
#define TASKLIST do
#define ENDTASKLIST while ( 0 );
#define QUIT break

#define NUM_MAX FLT_MAX
const int cbBufferMax = 80;

static HANDLE _hInstance = 0;

// Trading position, short or long
enum Position
    {
    posShort,
    posLong
    };

// SAR options structure
struct tagSAR
    {
    NUM numAccel;
    NUM numAccelMax;
    };
typedef struct tagSAR SAR;
typedef struct tagSAR FAR *LPSAR;

//////////////////////////////////////////////////////////////////////////////
// LibMain( )
//
// Library entrance routine.
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
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// VerString( )
//
// Version routine.
//////////////////////////////////////////////////////////////////////////////
LPCSTR FAR PASCAL _export VerString( void )
    {
    return (LPCSTR) "Parabolic SAR V 1.30";
    }

//////////////////////////////////////////////////////////////////////////////
// ParamString( )
//
// Returns SAR parameters formatted as a string
//////////////////////////////////////////////////////////////////////////////
LPCSTR FAR PASCAL _export ParamString( LPOPTIONS lpopts )
    {
    static char szBuffer[ _cbBufferMax + 1 ];
    LPSAR lpsar = (LPSAR) lpopts->lOptions;
    memset( szBuffer, 0, sizeof( szBuffer ) );
    sprintf( szBuffer, "%f,%f", lpsar->numAccel, lpsar->numAccelMax );
    return (LPCSTR) szBuffer;
    }

//////////////////////////////////////////////////////////////////////////////
// Options( )
//
// Options routine.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Options( LPOPTIONS lpopts )
    {
    return DialogBoxParam( _hInstance, 
                           MAKEINTRESOURCE( IDD_SAR ),
                           lpopts->hwndParent, 
                           (DLGPROC) SarOptionsDlgProc, 
                           (LPARAM) lpopts );
    }

//////////////////////////////////////////////////////////////////////////////
// SarOptionsDlgProc( )
//
// This is the dialog procedure to obtain the options from
// the user.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export SarOptionsDlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static    LPOPTIONS lpOpts;
    static    HWND      hwndAccel;
    static    HWND      hwndAccelMax;
    static    char      szText[ cbBufferMax + 1 ];
    SAR                 sar;
    LPSAR               lpsar;
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndAccel    = GetDlgItem( hDlg, IDC_ACCEL );
        hwndAccelMax = GetDlgItem( hDlg, IDC_ACCELMAX );
        SendMessage( hwndAccel,    EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );
        SendMessage( hwndAccelMax, EM_LIMITTEXT, (WPARAM) cbBufferMax, 0L );

        lpOpts = (LPOPTIONS) lParam;
        if ( !lpOpts->lOptions )
            {
            sar.numAccel    = 0.02;
            sar.numAccelMax = 0.20;
            if ( !OptionsAlloc( lpOpts, hDlg, sizeof( SAR ), (DWORD) (LPSAR) &sar ) )
                {
                MessageBox( hDlg, "SAR - Out of Memory", "SAR", MB_ICONEXCLAMATION );
                EndDialog( hDlg, FALSE );
                return TRUE;
                }
            }
        
        lpsar = (LPSAR) lpOpts->lOptions;
        sprintf( szText, "%.4f", lpsar->numAccel );
        SetWindowText( hwndAccel, szText );
        sprintf( szText, "%.4f", lpsar->numAccelMax );
        SetWindowText( hwndAccelMax, szText );

        SendMessage( hwndAccel, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) );
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            GetWindowText( hwndAccel, szText, sizeof( szText ) );
            sar.numAccel    = (NUM) atof( szText );
            GetWindowText( hwndAccelMax, szText, sizeof( szText ) );
            sar.numAccelMax = (NUM) atof( szText );
            if ( sar.numAccel <= 0 || sar.numAccelMax <= sar.numAccel )
                {
                MessageBox( hDlg, "SAR: Invalid Parameters", "SAR", MB_ICONEXCLAMATION );
                break;
                }
            lpsar = (LPSAR) lpOpts->lOptions;
            lpsar->numAccel    = sar.numAccel;
            lpsar->numAccelMax = sar.numAccelMax;
            EndDialog( hDlg, TRUE );
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            break;
            }
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
        }
    
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// Calculate( )
//
// Calculate routine.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Calculate( LPCALCSTRUCT lpcalc )
    {
    BOOL         fRet        = FALSE;
    LPSAR        lpsar       = (LPSAR) lpcalc->opts.lOptions;
    const NUM    numAccel    = lpsar->numAccel;
    const NUM    numAccelMax = lpsar->numAccelMax;
    DATASET FAR &dsHi        = lpcalc->datasetHi;
    DATASET FAR &dsLo        = lpcalc->datasetLo;
    BOOL         fFirst;
    Position     pos;
    WORD         iHi;
    WORD FAR    &iLo = iHi;
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

        // do some sanity checks
        if ( lpcalc->datasetLo.nPoints != lpcalc->datasetHi.nPoints )
            break;
        if ( lpcalc->datasetHi.nPoints <= 2 )
            break;

        // allocate our result dataset
        if ( !DataSetAlloc( &dsRes, dsHi.nPoints - 2, 2 ) )
            break;
        
        // we start out 'short', just having switched positions
        // into a long position
        numExtreme  = min( dsLo.lpnum[ 0 ], dsLo.lpnum[ 1 ] );
        pos         = posLong;
        fFirst      = TRUE;
        
        // main loop
        for ( iHi = 2; iHi < dsHi.nPoints; iHi++ )
            {
            iRes  = iHi - 2;
            numHi = dsHi.lpnum[ iHi ];
            numLo = dsLo.lpnum[ iLo ];
            
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
                numSar = min( numSar, dsLo.lpnum[ iLo - 2 ] );
                numSar = min( numSar, dsLo.lpnum[ iLo - 1 ] );
                }
            else
                {
                numSar = max( numSar, dsHi.lpnum[ iHi - 2 ] );
                numSar = max( numSar, dsHi.lpnum[ iHi - 1 ] );
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
        if ( !DataSetCopy( &lpcalc->datasetCalc, &dsRes ) )
            break;
        fRet = TRUE;
        }
    ENDTASKLIST
    DataSetFree( &dsRes );

    return fRet;
    }

