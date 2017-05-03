//////////////////////////////////////////////////////////////////////////////
// DLG.CPP
//
// This file contains dialog box procedures.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <stdlib.h>
#include <windowsx.h>
#include <boxctrl.h>
#include <statpak.h>
#include "resource.hxx"
#include "types.hxx"
#include "template.hxx"
#include "tools.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "dlg.hxx"
#include "rpt.hxx"
#include "perfeng.hxx"
#include "simeng.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

static BOOL ParseDate( CHAR *szDate, int *pmonth, int *pday, int *pyear );
static VOID Adjust8( CHAR *sz );
static PerformanceRequest::SortKey MapSortKey( const CHAR *pszKey );

//////////////////////////////////////////////////////////////////////////////
// IndValProc( )
//
// Drives the Indicator Value Dialog
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export IndValProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    int    month, day, year;
    static HWND hwndName;
    static HWND hwndDate;
    CHAR szName  [ _cbNameMax   + 1 ];
    CHAR szBuffer[ _cbBufferMax + 1 ];
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndName = GetDlgItem( hDlg, IDC_TEMPLATE );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        hwndDate = GetDlgItem( hDlg, IDC_DATE );
        SendMessage( hwndDate, EM_LIMITTEXT, _cbBufferMax, 0L );

        ComboAddTemplates( hwndName );
        if ( _pdoc->GetCount( ) )
            {
            QTime  dt;
            dt = _pdoc->GetLast( )->GetDate( );
            dt.Format( 32, "%m/%d/%Y");
            SetWindowText( hwndDate, (const CHAR *)(QString) dt );
            }
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                TASKLIST
                    {
                    GetText( hwndDate, szBuffer, sizeof( szBuffer ) );
                    if ( !ParseDate( szBuffer, &month, &day, &year ) )
                        {
                        MsgBox( hDlg, IDS_BADDATE );
                        SetFocus( hwndDate );
                        SelectEdit( hwndDate );
                        break;
                        }
                    
                    QTime dt;
                    dt.Set( year, month, day, 12, 0, 0 );
                    
                    GetText( hwndName, szName, sizeof( szName ) );
                    IndicatorValue( hDlg, szName, dt );
                    SetFocus( hwndName );
                    }
                ENDTASKLIST
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                SetFocus( hwndDate );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                SetFocus( hwndDate );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ReportIndicatorValueResultsProc( )
//
// Reports indicator value results
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ReportIndicatorValueResultsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    CHAR  sz[ 80 + 1 ];
    LPNUM lpnum;

    switch( message )
        {
    case WM_INITDIALOG:
        lpnum = (LPNUM) lParam;
        sprintf( sz, "The value is: %.4f", *lpnum );
        SetWindowText( GetDlgItem( hDlg, IDC_RESULTS ), sz );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ReportScanResultsProc( )
//
// This routine will report the results of the Chart Scanner.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ReportScanResultsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static  ScanResults FAR *lpResults;
    FARPROC lpfn;
    CHAR    sz[ 80 + 1 ];

    switch( message )
        {
    case WM_INITDIALOG:
        lpResults = (ScanResults FAR *) lParam;
        wsprintf( sz, "Signals found: %u", lpResults->m_scanlist.GetCount( ) );
        if ( lpResults->m_scanlist.corruptfiles( ).GetCount( ) )
            lstrcat( sz, "*" );
        SetWindowText( GetDlgItem( hDlg, IDC_RESULTS ), sz );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            lpfn = MakeProcInstance( (FARPROC) SaveScanResultsProc, _hInstance );
            DialogBoxParam( _hInstance, MAKEINTRESOURCE( IDD_SAVE_SCAN_RESULTS ),
                            hDlg, (DLGPROC) lpfn, (LPARAM) lpResults );
            FreeProcInstance( lpfn );
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// SaveScanResultsProc( )
//
// This routine will save the scan results into a file.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export SaveScanResultsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static ScanResults FAR *lpResults;
    static HWND hwndPath;
    CHAR   szPath[ _cbPathMax + 1 ];

    switch( message )
        {
    case WM_INITDIALOG:
        lpResults = (ScanResults FAR *) lParam;
        hwndPath  = GetDlgItem( hDlg, IDC_PATH );
        SendMessage( hwndPath, EM_LIMITTEXT, _cbPathMax, 0L );
        SetWindowText( hwndPath, "SCAN.RPT" );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
    
    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                GetText( hwndPath, szPath, sizeof( szPath ) );
                SaveScanResults( hDlg, *lpResults, szPath );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                }
            END_CATCH
            EndDialog( hDlg, TRUE );
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// AboutProc( )
//
// Manages the About dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export AboutProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QIndicatorTemplate *pi;
    QString sVersion;
    static  HWND hwndVersion;
    static  HWND hwndName;
    static  HWND hwndA;
    static  HWND hwndB;
    static  HWND hwndC;
    CHAR    szName[ _cbNameMax + 1 ];
    HDC     hdc;
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndVersion = GetDlgItem( hDlg, IDC_VERSION );
        hwndName    = GetDlgItem( hDlg, IDC_TEMPLATE );
        hwndA       = GetDlgItem( hDlg, IDC_A );
        hwndB       = GetDlgItem( hDlg, IDC_B );
        hwndC       = GetDlgItem( hDlg, IDC_C );

        hdc = GetDC( hwndVersion );
        SetBoxFontHeight( hwndVersion, LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) ) );
        ReleaseDC( hwndVersion, hdc );
        
        hdc = GetDC( hwndA );
        SetBoxFontHeight( hwndA, LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) ) );
        ReleaseDC( hwndA, hdc );

        hdc = GetDC( hwndB );
        SetBoxFontHeight( hwndB, LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) ) );
        ReleaseDC( hwndB, hdc );

        hdc = GetDC( hwndC );
        SetBoxFontHeight( hwndC, LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) ) );
        ReleaseDC( hwndC, hdc );
        
        TRY
            {
            sVersion = "Quote Version ";
            sVersion += _vApp;
#ifdef DEBUG
            sVersion += "-db";
#endif

#ifdef T_LM
            sVersion += "-lm";
#endif

#ifdef T_BG
            sVersion += "-bg";
#endif

#ifdef T_TX
            sVersion += "-tx";
#endif
            SetWindowText( hwndVersion, sVersion );
            }
        CATCH_ALL
            {
            sVersion.Cleanup( );
            MsgBox( hDlg, IDS_MEMORY );
            EndDialog( hDlg, FALSE );
            }
        END_CATCH
        ComboAddTemplates( hwndName );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDC_TEMPLATE:
                PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
                return TRUE;

            case IDC_RESERVED:
                TRY
                    {
                    GetText( hwndName, szName, sizeof( szName ) );
                    pi = _ptpl->m_itlist.Find( szName );
                    if ( pi )
                        {
                        SetWindowText( hwndA, szName );
                        SetWindowText( hwndB, pi->GetLibrary( ) );
                        LPCSTR lpsz = pi->VerString( );
                        SetWindowText( hwndC, lpsz );
                        }
                    else
                        {
                        SetWindowText( hwndA, GetString( IDS_UNAVAIL ) );
                        SetWindowText( hwndB, GetString( IDS_UNAVAIL ) );
                        SetWindowText( hwndC, GetString( IDS_UNAVAIL ) );
                        }
                    InvalidateRect( hwndA, NULL, TRUE );
                    InvalidateRect( hwndB, NULL, TRUE );
                    InvalidateRect( hwndC, NULL, TRUE );
                    UpdateWindow( hwndA );
                    UpdateWindow( hwndB );
                    UpdateWindow( hwndC );
                    }
                CATCH_ALL
                    {
                    EndDialog( hDlg, FALSE );
                    }
                END_CATCH
                return TRUE;
                    
            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// PrintProc( )
//
// This is the dialog box to print a window layout.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export PrintProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND hwndName;
    static HWND hwndTop;
    static HWND hwndLeft;
    static HWND hwndBottom;
    static HWND hwndRight;
    static HWND hwndLastPageOnly;
    HWND        hwndChild;
    CHAR   sz[ _cbBufferMax + 1 ]; 
    NUM    numTop;
    NUM    numLeft;
    NUM    numBottom;
    NUM    numRight;
    BOOL   fRet;
    BOOL   fLastPageOnly;
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndName         = GetDlgItem( hDlg, IDC_LAYOUT );
        hwndTop          = GetDlgItem( hDlg, IDC_TOP );
        hwndLeft         = GetDlgItem( hDlg, IDC_LEFT );
        hwndBottom       = GetDlgItem( hDlg, IDC_BOTTOM );
        hwndRight        = GetDlgItem( hDlg, IDC_RIGHT );
        hwndLastPageOnly = GetDlgItem( hDlg, IDC_LASTPAGEONLY );
        ComboAddWLs( hwndName );
        lstrcpy( sz, "0" );
        SetWindowText( hwndTop,    sz );
        SetWindowText( hwndLeft,   sz );
        SetWindowText( hwndBottom, sz );
        SetWindowText( hwndRight,  sz );
        
        TRY
            {
            // Attempt to select the active MDI window
            hwndChild = (HWND) LOWORD( SendMessage( _hwndClient, WM_MDIGETACTIVE, 0, 0L ) );
            if ( hwndChild != NULL )
                {
                CHAR szTextChild[ _cbNameMax + 1 ];
                GetWindowText( hwndChild, szTextChild, sizeof( szTextChild ) );
                ComboSelectString( hwndName, szTextChild );
                }
            }
        CATCH_ALL
            {
            // ignore the exception
            }
        END_CATCH
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            fRet = FALSE;
            TRY
                {
                GetText( hwndTop,    sz, sizeof( sz ) );
                numTop    = (NUM) atof( sz );

                GetText( hwndLeft,   sz, sizeof( sz ) );
                numLeft   = (NUM) atof( sz );

                GetText( hwndBottom, sz, sizeof( sz ) );
                numBottom = (NUM) atof( sz );

                GetText( hwndRight,  sz, sizeof( sz ) );
                numRight  = (NUM) atof( sz );

                GetText( hwndName,   sz, sizeof( sz ) );

                fLastPageOnly = (BOOL) SendMessage( hwndLastPageOnly, BM_GETCHECK, 0, 0L );
                PrintQuotes( hDlg, sz, numTop, numLeft, numBottom, numRight, fLastPageOnly );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                }
            END_CATCH
            EndDialog( hDlg, fRet );
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// CreateRuleProc( )
//
// Creates a trading rule.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export CreateRuleProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QRule  *prule;
    BOOL   fAllocated;
    BOOL   fAdded;
    static HWND hwndName;
    static HWND hwndBuy;
    static HWND hwndSell;
    static HWND hwndFilter;
    static HWND hwndFilter2;
    static HWND hwndOk;
    static HWND hwndCurrentClose;
    static HWND hwndNextOpen;
    static BOOL fModified;
    CHAR   szName[ _cbNameMax + 1 ];
    CHAR   szBuy [ _cbTextMax + 1 ];
    CHAR   szSell[ _cbTextMax + 1 ];
    CHAR   szFilter[ _cbTextMax + 1 ];
    CHAR   szFilter2[ _cbTextMax + 1 ];

    switch( message )
        {
    case WM_INITDIALOG:   
        fModified = FALSE;
        hwndOk   = GetDlgItem( hDlg, IDOK );
        hwndName = GetDlgItem( hDlg, IDC_RULE );
        hwndBuy  = GetDlgItem( hDlg, IDC_BUY  );
        hwndSell = GetDlgItem( hDlg, IDC_SELL );
        hwndFilter = GetDlgItem( hDlg, IDC_FILTER );
        hwndFilter2 = GetDlgItem( hDlg, IDC_FILTER2 );
        hwndCurrentClose = GetDlgItem( hDlg, IDC_CURRENT_CLOSE );
        hwndNextOpen = GetDlgItem( hDlg, IDC_NEXT_OPEN );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax,   0L );
        SendMessage( hwndBuy,  EM_LIMITTEXT, _cbTextMax, 0L );
        SendMessage( hwndSell, EM_LIMITTEXT, _cbTextMax, 0L );
        SendMessage( hwndFilter, EM_LIMITTEXT, _cbTextMax, 0L );
        SendMessage( hwndFilter2, EM_LIMITTEXT, _cbTextMax, 0L );
        ComboAddRules( hwndName );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
    
    case WM_COMMAND:
        switch( wParam )
            {
        case IDC_RULE:
            PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
            return TRUE;

        case IDC_RESERVED:
            GetText( hwndName, szName, sizeof( szName ) );
            if ( *szName && ( prule = _ptpl->m_rulelist.Find( szName ) ) )
                {
                if ( prule->GetBuyString( ) )
                    {
                    ASSERT( lstrlen( prule->GetBuyString( ) )  <= _cbTextMax );
                    SetText( hwndBuy,  prule->GetBuyString( ) );
                    }
                else
                    {
                    SetText( hwndBuy, GetString( IDS_EMPTY ) );
                    }
                if ( prule->GetSellString( ) )
                    {
                    ASSERT( lstrlen( prule->GetSellString( ) ) <= _cbTextMax );
                    SetText( hwndSell, prule->GetSellString( ) );
                    }
                else                                          
                    {
                    SetText( hwndBuy, GetString( IDS_EMPTY ) );
                    }
                if ( prule->GetFilterString( ) )
                    {
                    ASSERT( lstrlen( prule->GetFilterString( ) ) <= _cbTextMax );
                    SetText( hwndFilter, prule->GetFilterString( ) );
                    }
                else                                          
                    {
                    SetText( hwndFilter, GetString( IDS_EMPTY ) );
                    }
                if ( prule->GetFilter2String( ) )
                    {
                    ASSERT( lstrlen( prule->GetFilter2String( ) ) <= _cbTextMax );
                    SetText( hwndFilter2, prule->GetFilter2String( ) );
                    }
                else                                          
                    {
                    SetText( hwndFilter2, GetString( IDS_EMPTY ) );
                    }

                // Set the current close/next open buttons
                BOOL fNextOpen = prule->GetNextOpenFlag( );
                SendMessage( hwndCurrentClose, BM_SETCHECK, !fNextOpen, 0L );
                SendMessage( hwndNextOpen, BM_SETCHECK, fNextOpen, 0L );
                }
            else
                {
                SetText( hwndBuy,    GetString( IDS_EMPTY ) );
                SetText( hwndSell,   GetString( IDS_EMPTY ) );
                SetText( hwndFilter, GetString( IDS_EMPTY ) );
                SetText( hwndFilter2, GetString( IDS_EMPTY ) );

                // Default to 'next open'
                SendMessage( hwndCurrentClose, BM_SETCHECK, FALSE, 0L );
                SendMessage( hwndNextOpen, BM_SETCHECK, TRUE, 0L );
                }
            EnableWindow( hwndOk, (BOOL) ( *szName ) );
            return TRUE;

        case IDOK:
            TRY
                {                          
                fAllocated = FALSE;
                fAdded = FALSE;
                GetText( hwndName, szName, sizeof( szName ) );
                if ( !ValidName( szName ) )
                    {
                    MsgBox( hDlg, IDS_BADNAME );
                    SetFocus( hwndName );
                    SelectCBEdit( hwndName );
                    return TRUE;
                    }
                GetText( hwndBuy,    szBuy,    sizeof( szBuy ) );
                GetText( hwndSell,   szSell,   sizeof( szSell ) );
                GetText( hwndFilter, szFilter, sizeof( szFilter ) );
                GetText( hwndFilter2, szFilter2, sizeof( szFilter2 ) );
                prule = _ptpl->m_rulelist.Find( szName );
                if ( !prule )
                    {
                    prule = new QRule;
                    if ( !prule )
                        {
                        THROW_EXCEPTION( exMemory );
                        }
                    fAllocated = TRUE;
                    }
                prule->SetName( szName );
                prule->SetBuyString( szBuy );
                prule->SetSellString( szSell );
                prule->SetFilterString( szFilter );
                prule->SetFilter2String( szFilter2 );
                prule->SetNextOpenFlag( (BOOL) SendMessage( hwndNextOpen, BM_GETCHECK, 0, 0L ) );
                if ( fAllocated )
                    {
                    _ptpl->m_rulelist.AddTail( prule );
                    fAdded = TRUE;
                    SendMessage( hwndName, CB_ADDSTRING, 0, (LPARAM)(LPSTR) szName );
                    }
                gcache.Cleanup( );
                _ptpl->SetModifiedFlag( TRUE );
                UpdateMenu( );
                SetFocus( hwndName );
                fModified = TRUE;
                }
            CATCH( exMemory )
                {
                gcache.Cleanup( );
                MsgBox( hDlg, IDS_MEMORY );
                if ( fAdded )
                    {
                    BOOL f = _ptpl->m_rulelist.Remove( prule );
                    ASSERT( f );
                    }
                if ( prule )
                    {
                    prule->SetBuyString( NULL );
                    prule->SetSellString( NULL );
                    prule->SetFilterString( NULL );
                    prule->SetFilter2String( NULL );
                    }
                if ( fAllocated )
                    {
                    delete prule;
                    }
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                gcache.Cleanup( );
                MsgBox( hDlg, IDS_GENERALERROR );
                if ( fAdded )
                    {
                    BOOL f = _ptpl->m_rulelist.Remove( prule );
                    ASSERT( f );
                    }
                if ( prule )
                    {
                    prule->SetBuyString( NULL );
                    prule->SetSellString( NULL );
                    prule->SetFilterString( NULL );
                    prule->SetFilter2String( NULL );
                    }
                if ( fAllocated )
                    {
                    delete prule;
                    }
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            if ( fModified )
                {
                UpdateAllViews( TRUE );
                }
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// DeleteRuleProc( )
//
// Deletes a trading rule.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DeleteRuleProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {               
    QRule *prule;
    static HWND hwndName;
    CHAR   szName[ _cbNameMax + 1 ];
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndName = GetDlgItem( hDlg, IDC_RULE );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        ComboAddRules( hwndName );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
    
    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                TASKLIST
                    {
                    GetText( hwndName, szName, sizeof( szName ) );
                    prule = _ptpl->m_rulelist.Find( szName );
                    ASSERT( prule != NULL );
                    if ( _ptpl->RuleInUse( szName ) )
                        {
                        MsgBox( hDlg, IDS_RULEINUSE );
                        QUIT;
                        }
                    BOOL f = _ptpl->m_rulelist.Remove( prule );
                    ASSERT( f );
                    delete prule;
                    gcache.Cleanup( );
                    ComboDeleteString( hwndName, szName );
                    UpdateMenu( );
                    _ptpl->SetModifiedFlag( TRUE );
                    if ( SendMessage( hwndName, CB_GETCOUNT, 0, 0L ) != 0 )
                        {
                        QUIT;
                        }
                    UpdateAllViews( TRUE );
                    EndDialog( hDlg, TRUE );
                    }
                ENDTASKLIST
                }
            CATCH( exMemory )
                {
                gcache.Cleanup( );
                MsgBox( hDlg, IDS_MEMORY );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                gcache.Cleanup( );
                MsgBox( hDlg, IDS_GENERALERROR );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ScannerProc( )
//
// This dialogproc manages requests for chart scanning.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ScannerProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static const CHAR *szFilter[ ] = { "*.QT", "*.TXT", "*.*" };
    static HWND hwndName;
    static HWND hwndDays;
    static HWND hwndExt;
    static HWND hwndBuys;
    static HWND hwndSells;
    static HWND hwndBoth;

    const CHAR *pszFilter;
    CHAR   szName[ _cbNameMax + 1 ];
    CHAR   szDays[ _cbNameMax + 1 ];
    int    nDays;
    int    n;
    BOOL   fBuys;
    BOOL   fSells;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndName  = GetDlgItem( hDlg, IDC_RULE );
        hwndDays  = GetDlgItem( hDlg, IDC_DAYS );
        hwndExt   = GetDlgItem( hDlg, IDC_FILE_EXT );
        hwndBuys  = GetDlgItem( hDlg, IDC_BUYS );
        hwndSells = GetDlgItem( hDlg, IDC_SELLS );
        hwndBoth  = GetDlgItem( hDlg, IDC_BOTH );

        ComboAddRules( hwndName );
        SendMessage( hwndDays, EM_LIMITTEXT, _cbNameMax, 0L );
        SetWindowText( hwndDays, "14" );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter1 );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter2 );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter3 );
        SendMessage( hwndExt, CB_SELECTSTRING, -1, (LPARAM)(LPCSTR) _pszFilter2 );
        SendMessage( hwndBuys,  BM_SETCHECK, FALSE, 0L );
        SendMessage( hwndSells, BM_SETCHECK, FALSE, 0L );
        SendMessage( hwndBoth,  BM_SETCHECK, TRUE,  0L );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
    
    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                GetText( hwndName, szName, sizeof( szName ) );
                GetText( hwndDays, szDays, sizeof( szDays ) );
                nDays = atoi( szDays );
                
                n = (int) SendMessage( hwndExt, CB_GETCURSEL, 0, 0L );
                ASSERT( n >= 0 && n < 3 );
                pszFilter = szFilter[ n ];

                if ( (BOOL) SendMessage( hwndBoth, BM_GETCHECK, 0, 0L ) )
                    {
                    fBuys = fSells = TRUE;
                    }
                else
                    {
                    fBuys  = (BOOL) SendMessage( hwndBuys,  BM_GETCHECK, 0, 0L );
                    fSells = (BOOL) SendMessage( hwndSells, BM_GETCHECK, 0, 0L );
#ifdef DEBUG                
                    ASSERT( fBuys || fSells );
                    if ( fBuys )
                        {
                        ASSERT( !fSells );
                        }
                    if ( fSells )
                        {
                        ASSERT( !fBuys );
                        }
#endif
                    }
                if ( nDays >= 0 )
                    {
                    ChartScanner( hDlg, szName, nDays, pszFilter, fBuys, fSells );
                    }
                else
                    {
                    MsgBox( hDlg, IDS_POSITIVE );
                    SetFocus( hwndDays );
                    SelectEdit( hwndDays );
                    }
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// MapSortKey( )
//
// Helper method to map sort keys
//////////////////////////////////////////////////////////////////////////////
PerformanceRequest::SortKey MapSortKey( const CHAR *pszKey )
    {
    if ( lstrcmp( pszKey, GetString( IDS_NET_PROFIT ) ) == 0 )
        return PerformanceRequest::NetProfit;
    else if ( lstrcmp( pszKey, GetString( IDS_ROA ) ) == 0 )
        return PerformanceRequest::ROA;
    else if ( lstrcmp( pszKey, GetString( IDS_PROFIT_FACTOR ) ) == 0 )
        return PerformanceRequest::ProfitFactor;
    else if ( lstrcmp( pszKey, GetString( IDS_AVG_TRADE ) ) == 0 )
        return PerformanceRequest::AvgTrade;
    else if ( lstrcmp( pszKey, GetString( IDS_PCT_PROFITABLE ) ) == 0 )
        return PerformanceRequest::PctProfitable;
    else if ( lstrcmp( pszKey, GetString( IDS_RATIO_AVGWIN_AVGLOSS ) ) == 0 )
        return PerformanceRequest::RatioAvgWinAvgLoss;
    else if ( lstrcmp( pszKey, GetString( IDS_ACCOUNT_SIZE ) ) == 0 )
        return PerformanceRequest::AccountSize;
    else if ( lstrcmp( pszKey, GetString( IDS_N_TRADES ) ) == 0 )
        return PerformanceRequest::NTrades;
    else if ( lstrcmp( pszKey, GetString( IDS_NONE ) ) == 0 )
        return PerformanceRequest::None;

    ASSERT( FALSE );
    return PerformanceRequest::None;
    }

//////////////////////////////////////////////////////////////////////////////
// PerformanceProc( )
//
// This dialogproc manages requests for performance tests.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export PerformanceProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    PerformanceRequest req;
    static HWND hwndName;
    static HWND hwndLoadedCompany;
    static HWND hwndAllCompanies;
    static HWND hwndPosition;
    static HWND hwndComm;
    static HWND hwndExt;

    static HWND hwndConst1;
    static HWND hwndConst2;
    static HWND hwndConst3;
    static HWND hwndStart1;
    static HWND hwndStart2;
    static HWND hwndStart3;
    static HWND hwndStep1;
    static HWND hwndStep2;
    static HWND hwndStep3;
    static HWND hwndN1;
    static HWND hwndN2;
    static HWND hwndN3;
    static HWND hwndSortBy;
    
    CHAR   szName[ _cbNameMax + 1 ];
    CHAR   szConst[ _cbNameMax + 1 ];
    CHAR   szFloat[ _cbNameMax + 1 ];
    CHAR   szKey[ _cbBufferMax + 1 ];
    static CHAR szPos [ _cbNameMax + 1 ];
    static CHAR szComm[ _cbNameMax + 1 ];
    static BOOL fLoadedCompany = TRUE;
    static BOOL fFirst = TRUE;
    static CHAR *szFilter[ ] = { "*.QT", "*.TXT", "*.*" };
    QString sNone = GetString( IDS_NONE );
    int    n;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndName             = GetDlgItem( hDlg, IDC_RULE );
        hwndLoadedCompany    = GetDlgItem( hDlg, IDC_LOADED_COMPANY );
        hwndAllCompanies     = GetDlgItem( hDlg, IDC_ALL_COMPANIES );
        hwndPosition         = GetDlgItem( hDlg, IDC_POSITION );
        hwndComm             = GetDlgItem( hDlg, IDC_COMMISSION );
        hwndExt              = GetDlgItem( hDlg, IDC_FILE_EXT );
        hwndConst1           = GetDlgItem( hDlg, IDC_CONST1 );
        hwndConst2           = GetDlgItem( hDlg, IDC_CONST2 );
        hwndConst3           = GetDlgItem( hDlg, IDC_CONST3 );
        hwndStart1           = GetDlgItem( hDlg, IDC_START1 );
        hwndStart2           = GetDlgItem( hDlg, IDC_START2 );
        hwndStart3           = GetDlgItem( hDlg, IDC_START3 );
        hwndStep1            = GetDlgItem( hDlg, IDC_STEP1 );
        hwndStep2            = GetDlgItem( hDlg, IDC_STEP2 );
        hwndStep3            = GetDlgItem( hDlg, IDC_STEP3 );
        hwndN1               = GetDlgItem( hDlg, IDC_N1 );
        hwndN2               = GetDlgItem( hDlg, IDC_N2 );
        hwndN3               = GetDlgItem( hDlg, IDC_N3 );
        hwndSortBy           = GetDlgItem( hDlg, IDC_SORT_BY );
        SendMessage( hwndPosition,  EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndComm,      EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndStart1,    EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndStart2,    EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndStart3,    EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndStep1,     EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndStep2,     EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndStep3,     EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndN1,        EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndN2,        EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndN3,        EM_LIMITTEXT, _cbNameMax, 0L );
        if ( fFirst )
            {
            fFirst = FALSE;
            lstrcpy( szPos,  "2000.00" );
            lstrcpy( szComm, "25.00" );
            }

        ComboAddRules( hwndName );
        SendMessage( hwndLoadedCompany, BM_SETCHECK, fLoadedCompany, 0L );
        SendMessage( hwndAllCompanies, BM_SETCHECK, !fLoadedCompany, 0L );
        SetWindowText( hwndPosition, szPos );
        SetWindowText( hwndComm, szComm );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter1 );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter2 );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter3 );
        SendMessage( hwndExt, CB_SELECTSTRING, -1, (LPARAM)(LPCSTR) _pszFilter2 );
        ComboAddConstants( hwndConst1, TRUE );
        ComboAddConstants( hwndConst2, TRUE );
        ComboAddConstants( hwndConst3, TRUE );

        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_NONE ) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_NET_PROFIT ) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_ROA ) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_AVG_TRADE ) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_PROFIT_FACTOR ) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_PCT_PROFITABLE ) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_RATIO_AVGWIN_AVGLOSS) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_ACCOUNT_SIZE ) );
        SendMessage( hwndSortBy, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) GetString( IDS_N_TRADES ) );
        SendMessage( hwndSortBy, CB_SELECTSTRING, -1, (LPARAM)(LPCSTR) GetString( IDS_ROA ) );
        return TRUE;
    
    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                req.hwndParent = hDlg;

                GetText( hwndName, szName, sizeof( szName ) );
                req.sRuleName  = szName;

                fLoadedCompany = (BOOL) SendMessage( hwndLoadedCompany, BM_GETCHECK, 0, 0L );
                if ( fLoadedCompany )
                    req.type = PerformanceRequest::eLoadedCompany;
                else
                    req.type = PerformanceRequest::eAllCompanies;

                GetText( hwndPosition, szPos, sizeof( szPos ) );
                req.numPosition = (NUM) atof( szPos );

                GetText( hwndComm, szComm, sizeof( szComm ) );
                req.numComm = (NUM) atof( szComm );

                n = (int) SendMessage( hwndExt, CB_GETCURSEL, 0, 0L );
                ASSERT( n >= 0 && n < 3 );
                req.sFilter = szFilter[ n ];

                GetText( hwndConst1, szConst, sizeof( szConst ) );
                req.optRequests[ 0 ].sConstName = szConst;
                GetText( hwndStart1, szFloat, sizeof( szFloat ) );
                req.optRequests[ 0 ].numStart = (NUM) atof( szFloat );
                GetText( hwndStep1, szFloat, sizeof( szFloat ) );
                req.optRequests[ 0 ].numStep = (NUM) atof( szFloat );
                GetText( hwndN1, szFloat, sizeof( szFloat ) );
                req.optRequests[ 0 ].n = atoi( szFloat );
                
                GetText( hwndConst2, szConst, sizeof( szConst ) );
                req.optRequests[ 1 ].sConstName = szConst;
                GetText( hwndStart2, szFloat, sizeof( szFloat ) );
                req.optRequests[ 1 ].numStart = (NUM) atof( szFloat );
                GetText( hwndStep2, szFloat, sizeof( szFloat ) );
                req.optRequests[ 1 ].numStep = (NUM) atof( szFloat );
                GetText( hwndN2, szFloat, sizeof( szFloat ) );
                req.optRequests[ 1 ].n = atoi( szFloat );

                GetText( hwndConst3, szConst, sizeof( szConst ) );
                req.optRequests[ 2 ].sConstName = szConst;
                GetText( hwndStart3, szFloat, sizeof( szFloat ) );
                req.optRequests[ 2 ].numStart = (NUM) atof( szFloat );
                GetText( hwndStep3, szFloat, sizeof( szFloat ) );
                req.optRequests[ 2 ].numStep = (NUM) atof( szFloat );
                GetText( hwndN3, szFloat, sizeof( szFloat ) );
                req.optRequests[ 2 ].n = atoi( szFloat );

                GetText( hwndSortBy, szKey, sizeof( szKey ) );
                req.sortKey = MapSortKey( szKey );

                for ( int i = 0; i < OPT_REQUESTS_MAX; i++ )
                    {
                    if ( req.optRequests[ i ].sConstName != sNone )
                        req.optRequests[ i ].inUse = TRUE;
                    }
                long perfRuns = req.performanceRuns( );

                if ( req.optRequests[ 2 ].inUse &&
                     ( !req.optRequests[ 0 ].inUse || 
                       !req.optRequests[ 1 ].inUse ) )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( req.optRequests[ 1 ].inUse && !req.optRequests[ 0 ].inUse )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( req.optRequests[ 2 ].inUse &&
                          ( req.optRequests[ 2 ].sConstName == req.optRequests[ 1 ].sConstName ||
                            req.optRequests[ 2 ].sConstName == req.optRequests[ 0 ].sConstName ) )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( req.optRequests[ 1 ].inUse &&
                          req.optRequests[ 1 ].sConstName == req.optRequests[ 0 ].sConstName )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( req.optRequests[ 0 ].inUse &&
                          ( req.optRequests[ 0 ].numStep == (NUM) 0 ||
                            req.optRequests[ 0 ].n <= 0 ) )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( req.optRequests[ 1 ].inUse &&
                          ( req.optRequests[ 1 ].numStep == (NUM) 0 ||
                            req.optRequests[ 1 ].n <= 0 ) )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( req.optRequests[ 2 ].inUse &&
                          ( req.optRequests[ 2 ].numStep == (NUM) 0 ||
                            req.optRequests[ 2 ].n <= 0 ) )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( perfRuns > (long) PERF_RUNS_MAX )
                    {
                    MessageBox( hDlg, "Invalid optimization request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else if ( req.numPosition <= 0.0 || req.numComm < 0.0 )
                    {
                    MessageBox( hDlg, "Invalid position or commission size", 
                        GetString( IDS_APPNAME ), MB_ICONEXCLAMATION );
                    }
                else if ( perfRuns > 1 && MessageBox( hDlg, "Optimizations may require "
                          "a significant amount of time.  Proceed?", GetString( IDS_APPNAME ),
                          MB_YESNO ) != IDYES )
                    {
                    // empty
                    }
                else
                    {
                    PerformanceTester( req );
                    EndDialog( hDlg, TRUE );
                    }
                }
            CATCH( exMemory )
                {
                req.Cleanup( );
                MsgBox( hDlg, IDS_MEMORY );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                req.Cleanup( );
                MsgBox( hDlg, IDS_GENERALERROR );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;
        
        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// AddQuoteProc( )
//
// Manages the add quote dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export AddQuoteProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    int    month, day, year;
    static HWND hwndDate;
    static HWND hwndOp;
    static HWND hwndHi;
    static HWND hwndLo;
    static HWND hwndCl;
    static HWND hwndVl;
    static BOOL fModified;
    NUM    numOp;
    NUM    numHi;
    NUM    numLo;
    NUM    numCl;
    NUM    numVl;
    CHAR   szBuffer[ _cbBufferMax + 1 ];
    QTime  date;
    QQt    qt;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndDate = GetDlgItem( hDlg, IDC_DATE );
        hwndOp   = GetDlgItem( hDlg, IDC_OP );
        hwndHi   = GetDlgItem( hDlg, IDC_HI );
        hwndLo   = GetDlgItem( hDlg, IDC_LO );
        hwndCl   = GetDlgItem( hDlg, IDC_CL );
        hwndVl   = GetDlgItem( hDlg, IDC_VL );
        SendMessage( hwndDate, EM_LIMITTEXT, _cbBufferMax, 0L );
        SendMessage( hwndOp,   EM_LIMITTEXT, _cbBufferMax, 0L );
        SendMessage( hwndHi,   EM_LIMITTEXT, _cbBufferMax, 0L );
        SendMessage( hwndLo,   EM_LIMITTEXT, _cbBufferMax, 0L );
        SendMessage( hwndCl,   EM_LIMITTEXT, _cbBufferMax, 0L );
        SendMessage( hwndVl,   EM_LIMITTEXT, _cbBufferMax, 0L );
        fModified = FALSE;
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                TRY
                    {
                    TASKLIST
                        {
                        GetText( hwndDate, szBuffer, sizeof( szBuffer ) );
                        if ( !ParseDate( szBuffer, &month, &day, &year ) )
                            {
                            MsgBox( hDlg, IDS_BADDATE );
                            SetFocus( hwndDate );
                            SelectEdit( hwndDate );
                            QUIT;
                            }
                        date.Set( year, month, day, 12, 0, 0 );

                        GetText( hwndOp, szBuffer, sizeof( szBuffer ) );
                        Adjust8( szBuffer );
                        numOp = (NUM) atof( szBuffer );
                        
                        GetText( hwndHi, szBuffer, sizeof( szBuffer ) );
                        Adjust8( szBuffer );
                        numHi = (NUM) atof( szBuffer );

                        GetText( hwndLo, szBuffer, sizeof( szBuffer ) );
                        Adjust8( szBuffer );
                        numLo = (NUM) atof( szBuffer );

                        GetText( hwndCl, szBuffer, sizeof( szBuffer ) );
                        Adjust8( szBuffer );
                        numCl = (NUM) atof( szBuffer );

                        GetText( hwndVl, szBuffer, sizeof( szBuffer ) );
                        Adjust8( szBuffer );
                        numVl = (NUM) atof( szBuffer );

                        qt.SetDate( date );
                        qt.SetOp( numOp );
                        qt.SetHi( numHi );
                        qt.SetLo( numLo );
                        qt.SetCl( numCl );
                        qt.SetVl( numVl );
                        if ( !qt.IsValid( ) )
                            {
                            MsgBox( hDlg, IDS_BADQUOTE );
                            SetFocus( hwndOp );
                            SelectEdit( hwndOp );
                            QUIT;
                            }
                        _pdoc->Add( qt, TRUE );
                        _pdoc->SetModifiedFlag( TRUE );
                        fModified = TRUE;
                        ClearEdit( hwndDate );
                        ClearEdit( hwndOp );
                        ClearEdit( hwndHi );
                        ClearEdit( hwndLo );
                        ClearEdit( hwndCl );
                        ClearEdit( hwndVl );
                        SetFocus( hwndDate );
                        }
                    ENDTASKLIST
                    }
                CATCH( exMemory )
                    {
                    gcache.Cleanup( );
                    MsgBox( hDlg, IDS_MEMORY );
                    ClearEdit( hwndDate );
                    ClearEdit( hwndOp );
                    ClearEdit( hwndHi );
                    ClearEdit( hwndLo );
                    ClearEdit( hwndCl );
                    ClearEdit( hwndVl );
                    SetFocus( hwndDate );
                    }
                CATCH_ALL
                    {
                    gcache.Cleanup( );
                    MsgBox( hDlg, IDS_GENERALERROR );
                    ClearEdit( hwndDate );
                    ClearEdit( hwndOp );
                    ClearEdit( hwndHi );
                    ClearEdit( hwndLo );
                    ClearEdit( hwndCl );
                    ClearEdit( hwndVl );
                    SetFocus( hwndDate );
                    }
                END_CATCH
                return TRUE;

            case IDCANCEL:
                if ( fModified )
                    {
                    UpdateAllViews( TRUE );
                    }
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// DeleteQuoteProc( )
//
// Manages the delete quote dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DeleteQuoteProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static BOOL fModified;
    int    month, day, year;
    static HWND hwndDate;
    CHAR   szBuffer[ _cbBufferMax + 1 ];
    QTime  date;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndDate = GetDlgItem( hDlg, IDC_DATE );
        SendMessage( hwndDate, EM_LIMITTEXT, _cbBufferMax, 0L );
        fModified = FALSE;
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                TRY
                    {
                    TASKLIST
                        {
                        GetText( hwndDate, szBuffer, sizeof( szBuffer ) );
                        if ( !ParseDate( szBuffer, &month, &day, &year ) )
                            {
                            MsgBox( hDlg, IDS_BADDATE );
                            SetFocus( hwndDate );
                            SelectEdit( hwndDate );
                            QUIT;
                            }
                        date.Set( year, month, day, 12, 0, 0 );
                        if ( !_pdoc->Remove( date ) )
                            {
                            MsgBox( hDlg, IDS_REMOVE );
                            SetFocus( hwndDate );
                            SelectEdit( hwndDate );
                            QUIT;
                            }
                        _pdoc->SetModifiedFlag( TRUE );
                        fModified = TRUE;
                        ClearEdit( hwndDate );
                        SetFocus( hwndDate );
                        }
                    ENDTASKLIST
                    }
                CATCH( exMemory )
                    {
                    gcache.Cleanup( );
                    MsgBox( hDlg, IDS_MEMORY );
                    ClearEdit( hwndDate );
                    SetFocus( hwndDate );
                    }
                CATCH_ALL
                    {
                    gcache.Cleanup( );
                    MsgBox( hDlg, IDS_GENERALERROR );
                    ClearEdit( hwndDate );
                    SetFocus( hwndDate );
                    }
                END_CATCH
                return TRUE;

            case IDCANCEL:
                if ( fModified )
                    {
                    UpdateAllViews( TRUE );
                    }
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// TextFormatProc( )
//
// Manages the text file format dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export TextFormatProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    BOOL   fEnd;
    static HWND hwndLoadString;
    static HWND hwndLoadDelimiters;
    static HWND hwndSaveString;
    CHAR szBuffer1[ _cbTextMax + 1 ];
    CHAR szBuffer2[ _cbTextMax + 1 ];
    CHAR szBuffer3[ _cbTextMax + 1 ];
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndLoadString     = GetDlgItem( hDlg, IDC_LOAD_STRING );
        hwndLoadDelimiters = GetDlgItem( hDlg, IDC_LOAD_DELIMITERS );
        hwndSaveString     = GetDlgItem( hDlg, IDC_SAVE_STRING );
        SendMessage( hwndLoadString,     EM_LIMITTEXT, _cbTextMax, 0L );
        SendMessage( hwndLoadDelimiters, EM_LIMITTEXT, _cbTextMax, 0L );
        SendMessage( hwndSaveString,     EM_LIMITTEXT, _cbTextMax, 0L );
        SetWindowText( hwndLoadString,     (LPSTR) _ptpl->m_format.GetLoadString( ) );
        SetWindowText( hwndLoadDelimiters, (LPSTR) _ptpl->m_format.GetLoadDelimiters( ) );
        SetWindowText( hwndSaveString,     (LPSTR) _ptpl->m_format.GetSaveString( ) );
        SelectEdit( hwndLoadString );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                TRY
                    {
                    TASKLIST
                        {
                        fEnd = FALSE;
                        
                        GetText( hwndLoadString,     szBuffer1, sizeof( szBuffer1 ) );
                        GetText( hwndLoadDelimiters, szBuffer2, sizeof( szBuffer2 ) );
                        GetText( hwndSaveString,     szBuffer3, sizeof( szBuffer3 ) );

                        if ( !*szBuffer1 || !*szBuffer2 || !*szBuffer3 )
                            {
                            MsgBox( hDlg, IDS_FILLIN );
                            QUIT;
                            }
                        _ptpl->m_format.SetLoadString( szBuffer1 );
                        _ptpl->m_format.SetLoadDelimiters( szBuffer2 );
                        _ptpl->m_format.SetSaveString( szBuffer3 );
                        _ptpl->SetModifiedFlag( TRUE );
                        fEnd = TRUE;
                        }
                    ENDTASKLIST
                    }
                CATCH( exMemory )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    _ptpl->m_format.Cleanup( );
                    }
                CATCH_ALL
                    {
                    MsgBox( hDlg, IDS_GENERALERROR );
                    _ptpl->m_format.Cleanup( );
                    }
                END_CATCH
                if ( fEnd )
                    {
                    EndDialog( hDlg, TRUE );
                    }
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ExternalIndicatorFileProc( )
//
// Manages the external indicator file dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ExternalIndicatorFileProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND hwndFile;
    CHAR   szBuffer[ _cbPathMax + 1 ];
    HCURSOR hCursor = NULL;
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndFile = GetDlgItem( hDlg, IDC_EXTERNAL_INDICATOR_FILE );
        SendMessage( hwndFile, EM_LIMITTEXT, _cbPathMax, 0L );
        SetWindowText( hwndFile, _ptpl->m_genopts.GetExternalFileName( ) );
        SelectEdit( hwndFile );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                GetText( hwndFile, szBuffer, sizeof( szBuffer ) );
                TRY
                    {
                    hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                    _pExtDoc->RetrieveText( szBuffer );
                    _ptpl->m_genopts.SetExternalFileName( szBuffer );
                    _ptpl->SetModifiedFlag( TRUE );
                    gcache.Cleanup( );
                    UpdateAllViews( TRUE );
                    EndDialog( hDlg, TRUE );
                    }
                CATCH( exFileCorrupt )
                    {
                    SetCursor( hCursor );
                    hCursor = NULL;
                    MsgBox( hDlg, IDS_EXTINDFILE_CORRUPT );
                    SetFocus( hwndFile );
                    SelectEdit( hwndFile );
                    }
                CATCH( exMemory )
                    {
                    SetCursor( hCursor );
                    hCursor = NULL;
                    MsgBox( hDlg, IDS_MEMORY );
                    SetFocus( hwndFile );
                    SelectEdit( hwndFile );
                    }
                CATCH_ALL
                    {
                    SetCursor( hCursor );
                    hCursor = NULL;
                    MsgBox( hDlg, IDS_EXTINDFILE_ERROR );
                    SetFocus( hwndFile );
                    SelectEdit( hwndFile );
                    }
                END_CATCH
                if ( hCursor )
                    {
                    SetCursor( hCursor );
                    hCursor = NULL;
                    }
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// BrowserProc( )
//
// Manages the file browser dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export BrowserProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND hwndBrowser;
    CHAR   szBuffer[ _cbPathMax + 1 ];
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndBrowser = GetDlgItem( hDlg, IDC_BROWSER );
        SendMessage( hwndBrowser, EM_LIMITTEXT, _cbPathMax, 0L );
        SetWindowText( hwndBrowser, _ptpl->m_genopts.GetBrowserName( ) );
        SelectEdit( hwndBrowser );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                GetText( hwndBrowser, szBuffer, sizeof( szBuffer ) );
                _ptpl->m_genopts.SetBrowserName( szBuffer );
                _ptpl->SetModifiedFlag( TRUE );
                EndDialog( hDlg, TRUE );
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }
    
//////////////////////////////////////////////////////////////////////////////
// FontSizeProc( )
//
// Manages the font size dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export FontSizeProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND hwndFontSize;
    CHAR   szBuffer[ _cbBufferMax + 1 ];
    int    dyFont;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndFontSize = GetDlgItem( hDlg, IDC_FONT_SIZE );
        SendMessage( hwndFontSize, EM_LIMITTEXT, _cbBufferMax, 0L );
        wsprintf( szBuffer, "%u", _ptpl->m_genopts.GetFontSize( ) );
        SetWindowText( hwndFontSize, szBuffer );
        SelectEdit( hwndFontSize );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                GetText( hwndFontSize, szBuffer, sizeof( szBuffer ) );
                dyFont = atoi( szBuffer );
                if ( dyFont <= 0 )
                    {
                    MsgBox( hDlg, IDS_BADVALUE );
                    SetFocus( hwndFontSize );
                    SelectEdit( hwndFontSize );
                    }
                else
                    {
                    _ptpl->m_genopts.SetFontSize( (USHORT) dyFont );
                    _ptpl->SetModifiedFlag( TRUE );
                    UpdateAllViews( TRUE );
                    EndDialog( hDlg, TRUE );
                    }
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// LineWidthProc( )
//
// Manages the line widths dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export LineWidthProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND hwndLineWidth;
    CHAR   szBuffer[ _cbBufferMax + 1 ];
    int    dxLine;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndLineWidth = GetDlgItem( hDlg, IDC_LINE_WIDTH );
        SendMessage( hwndLineWidth, EM_LIMITTEXT, _cbBufferMax, 0L );
        wsprintf( szBuffer, "%u", _ptpl->m_genopts.GetLineWidths( ) );
        SetWindowText( hwndLineWidth, szBuffer );
        SelectEdit( hwndLineWidth );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                GetText( hwndLineWidth, szBuffer, sizeof( szBuffer ) );
                dxLine = atoi( szBuffer );
                if ( dxLine < 0 )
                    {
                    MsgBox( hDlg, IDS_BADVALUE );
                    SetFocus( hwndLineWidth );
                    SelectEdit( hwndLineWidth );
                    }
                else
                    {
                    _ptpl->m_genopts.SetLineWidths( (USHORT) dxLine );
                    _ptpl->SetModifiedFlag( TRUE );
                    UpdateAllViews( );
                    EndDialog( hDlg, TRUE );
                    }
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// NewWindowProc( )
//
// Manages the new window dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export NewWindowProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    CHAR szName[ _cbNameMax + 1 ];
    BOOL   fMaximized;
    static HWND hwndName;
    static HWND hwndMax;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndName = GetDlgItem( hDlg, IDC_LAYOUT );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        hwndMax  = GetDlgItem( hDlg, IDC_MAXIMIZED );
        ComboAddWLs( hwndName );
        SendMessage( hwndMax, BM_SETCHECK, (WPARAM) TRUE, 0L );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                TRY
                    {
                    GetText( hwndName, szName, sizeof( szName ) );
                    fMaximized = (BOOL) SendMessage( hwndMax, BM_GETCHECK, 0, 0L );
                    OpenWindow( szName, fMaximized );
                    UpdateAllViews( );
                    EndDialog( hDlg, TRUE );
                    }
                CATCH( exMemory )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    EndDialog( hDlg, FALSE );
                    }
                CATCH_ALL
                    {
                    MsgBox( hDlg, IDS_GENERALERROR );
                    EndDialog( hDlg, FALSE );
                    }
                END_CATCH
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ColorProc( )
//
// Manages the color dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ColorProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND   hwndAttribute;
    static HWND   hwndColor;
    static USHORT nIdxClrWindowBackground;
    static USHORT nIdxClrTextFace;
    static USHORT nIdxClrHLC;
    static USHORT nIdxClrVolume;
    static USHORT nIdxClrGrids;
    static USHORT nIdxClrBuy;
    static USHORT nIdxClrSell;
    USHORT        nIdxClr;
    CHAR          szBuffer[ _cbBufferMax + 1 ];
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndAttribute = GetDlgItem( hDlg, IDC_ATTRIBUTE );
        hwndColor = GetDlgItem( hDlg, IDC_COLOR );
        SendMessage( hwndAttribute, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_WINDOW_BACKGROUND ) );
        SendMessage( hwndAttribute, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_TEXT_FACE ) );
        SendMessage( hwndAttribute, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_HI_LO_CLOSE_BAR ) );
        SendMessage( hwndAttribute, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_VOLUME_BAR ) );
        SendMessage( hwndAttribute, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_GRIDS ) );
        SendMessage( hwndAttribute, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_BUY_SIGNAL ) );
        SendMessage( hwndAttribute, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_SELL_SIGNAL ) );
        SendMessage( hwndAttribute, CB_SETCURSEL, 0, 0L );
        ComboAddColors( hwndColor );
        nIdxClrWindowBackground = _ptpl->m_genopts.GetBackgroundColor( );
        nIdxClrTextFace         = _ptpl->m_genopts.GetTextColor( );
        nIdxClrHLC              = _ptpl->m_genopts.GetHLCColor( );
        nIdxClrVolume           = _ptpl->m_genopts.GetVolumeColor( );
        nIdxClrGrids            = _ptpl->m_genopts.GetGridsColor( );
        nIdxClrBuy              = _ptpl->m_genopts.GetBuyColor( );
        nIdxClrSell             = _ptpl->m_genopts.GetSellColor( );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDC_ATTRIBUTE:
                GetText( hwndAttribute, szBuffer, sizeof( szBuffer ) );
                if ( lstrcmp( szBuffer, GetString( IDS_WINDOW_BACKGROUND ) ) == 0 )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, (WPARAM) nIdxClrWindowBackground, 0L );
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_TEXT_FACE ) ) == 0 )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, (WPARAM) nIdxClrTextFace, 0L );
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_HI_LO_CLOSE_BAR ) ) == 0 )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, (WPARAM) nIdxClrHLC, 0L );
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_VOLUME_BAR ) ) == 0 )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, (WPARAM) nIdxClrVolume, 0L );
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_GRIDS ) ) == 0 )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, (WPARAM) nIdxClrGrids, 0L );
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_BUY_SIGNAL ) ) == 0 )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, (WPARAM) nIdxClrBuy, 0L );
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_SELL_SIGNAL ) ) == 0 )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, (WPARAM) nIdxClrSell, 0L );
                    }
                else
                    {
                    ASSERT( FALSE );
                    }
                return TRUE;

            case IDC_COLOR:
                nIdxClr = (USHORT) SendMessage( hwndColor, CB_GETCURSEL, 0, 0L );
                ASSERT( nIdxClr < RGCLRSIZE );
                GetText( hwndAttribute, szBuffer, sizeof( szBuffer ) );
                if ( lstrcmp( szBuffer, GetString( IDS_WINDOW_BACKGROUND ) ) == 0 )
                    {
                    nIdxClrWindowBackground = nIdxClr;
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_TEXT_FACE ) ) == 0 )
                    {
                    nIdxClrTextFace = nIdxClr;
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_HI_LO_CLOSE_BAR ) ) == 0 )
                    {
                    nIdxClrHLC = nIdxClr;
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_VOLUME_BAR ) ) == 0 )
                    {
                    nIdxClrVolume = nIdxClr;
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_GRIDS ) ) == 0 )
                    {
                    nIdxClrGrids = nIdxClr;
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_BUY_SIGNAL ) ) == 0 )
                    {
                    nIdxClrBuy = nIdxClr;
                    }
                else if ( lstrcmp( szBuffer, GetString( IDS_SELL_SIGNAL ) ) == 0 )
                    {
                    nIdxClrSell = nIdxClr;
                    }
                else
                    {
                    ASSERT( FALSE );
                    }
                return TRUE;

            case IDOK:
                _ptpl->m_genopts.SetBackgroundColor( nIdxClrWindowBackground );
                _ptpl->m_genopts.SetTextColor( nIdxClrTextFace );
                _ptpl->m_genopts.SetHLCColor( nIdxClrHLC );
                _ptpl->m_genopts.SetVolumeColor( nIdxClrVolume );
                _ptpl->m_genopts.SetGridsColor( nIdxClrGrids );
                _ptpl->m_genopts.SetBuyColor( nIdxClrBuy );
                _ptpl->m_genopts.SetSellColor( nIdxClrSell );
                _ptpl->SetModifiedFlag( TRUE );
                UpdateAllViews( );
                EndDialog( hDlg, TRUE );
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// CreateCondProc( )
//
// Manages Conditions.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export CreateCondProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    BOOL        fAllocated;
    BOOL        fAdded;
    BOOL        f;
    UINT        nIdxClr;
    static BOOL fModified;
    static HWND hwndName;
    static HWND hwndColor;
    static HWND hwndDefn;
    static HWND hwndOK;
    CHAR        szName[ _cbNameMax + 1 ];
    CHAR        szText[ _cbTextMax + 1 ];
    QCondition *pcond;

    switch( message )
        {
    case WM_INITDIALOG:
        fModified = FALSE;
        hwndName  = GetDlgItem( hDlg, IDC_CONDITION );
        hwndColor = GetDlgItem( hDlg, IDC_COLOR );
        hwndDefn  = GetDlgItem( hDlg, IDC_DEFN );
        hwndOK    = GetDlgItem( hDlg, IDOK );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndDefn, EM_LIMITTEXT, _cbTextMax, 0L );
        ComboAddColors( hwndColor );
        ComboAddConditions( hwndName );
        SendMessage( hwndColor, CB_SETCURSEL, 0, 0L );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
        
    case WM_COMMAND:
        switch( wParam )
            {
        case IDC_CONDITION:
            PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
            return TRUE;

        case IDC_RESERVED:
            GetText( hwndName, szName, sizeof( szName ) );
            if ( ( *szName ) && ( pcond = _ptpl->m_condlist.Find( szName ) ) != NULL )
            {
                SendMessage( hwndColor, CB_SETCURSEL, pcond->GetIdxClr( ), 0L );
                SetText( hwndDefn, pcond->GetDefinition( ) );
            }
            else
            {
                SendMessage( hwndColor, CB_SETCURSEL, 0, 0L );
                ClearEdit( hwndDefn );
            }
            EnableWindow( hwndOK, *szName );
            return TRUE;

        case IDOK:
            GetText( hwndName, szName, sizeof( szName ) );
            ASSERT( *szName );
            if ( !ValidName( szName ) )
                {
                MsgBox( hDlg, IDS_BADNAME );
                SetFocus( hwndName );
                SelectCBEdit( hwndName );
                return TRUE;
                }
            
            fAllocated = FALSE;
            fAdded     = FALSE;
            nIdxClr = (USHORT) SendMessage( hwndColor, CB_GETCURSEL, 0, 0L );
            ASSERT( nIdxClr < RGCLRSIZE );

            GetText( hwndDefn, szText, sizeof( szText ) );
            TRY
                {
                pcond = _ptpl->m_condlist.Find( szName );
                if ( !pcond )
                    {
                    pcond = new QCondition;
                    if ( !pcond )
                        {
                        THROW_EXCEPTION( exMemory );
                        }    
                    fAllocated = TRUE;
                    }
                pcond->SetName( szName );
                pcond->SetDefinition( szText );
                pcond->SetIdxClr( nIdxClr );
                if ( fAllocated )
                    {
                    _ptpl->m_condlist.AddTail( pcond );
                    fAdded = TRUE;
                    SendMessage( hwndName, CB_ADDSTRING, 0, (LPARAM)(LPSTR) szName );
                    }
                _ptpl->SetModifiedFlag( TRUE );
                UpdateMenu( );
                gcache.Cleanup( );
                fModified = TRUE;
                SetFocus( hwndName );
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                if ( fAdded )
                    {
                    f = _ptpl->m_condlist.Remove( pcond );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete pcond;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                if ( fAdded )
                    {
                    f = _ptpl->m_condlist.Remove( pcond );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete pcond;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            if ( fModified )
                UpdateAllViews( TRUE );
                
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ExternalITProc( )
//
// Manages external indicator templates.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export ExternalITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    CHAR        szName[ _cbNameMax + 1 ];
    CHAR        szText[ _cbTextMax + 1 ];
    CHAR        szHorz[ _cbTextMax + 1 ];
    UINT        nIdxClr;
    GraphType   gt;
    BOOL        f;
    BOOL        fAdded;
    BOOL        fAllocated;
    static BOOL fModified;
    static HWND hwndName;
    static HWND hwndColor;
    static HWND hwndLine;
    static HWND hwndBar;
    static HWND hwndPoint;
    static HWND hwndOK;
    static HWND hwndDefn;
    static HWND hwndHorz;
    QIndicatorTemplate *pi;
    QIndicatorTemplateExternal *piExternal;

    switch( message )
        {
    case WM_INITDIALOG:
        fModified = FALSE;
        hwndName  = GetDlgItem( hDlg, IDC_TEMPLATE );
        hwndColor = GetDlgItem( hDlg, IDC_COLOR );
        hwndLine  = GetDlgItem( hDlg, IDC_LINE );
        hwndBar   = GetDlgItem( hDlg, IDC_BAR );
        hwndPoint = GetDlgItem( hDlg, IDC_POINT );
        hwndOK    = GetDlgItem( hDlg, IDOK );
        hwndDefn  = GetDlgItem( hDlg, IDC_DEFN );
        hwndHorz  = GetDlgItem( hDlg, IDC_HORZ );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndDefn, EM_LIMITTEXT, _cbTextMax, 0L );
        SendMessage( hwndHorz, EM_LIMITTEXT, _cbTextMax, 0L );
        ComboAddColors( hwndColor );
        ComboAddTemplates( hwndName );
        SendMessage( hwndColor, CB_SETCURSEL, 0, 0L );
        SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
        SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
        SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND: 
        switch( wParam )
            {
        case IDC_TEMPLATE:
            PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
            return TRUE;

        case IDC_RESERVED:
            GetText( hwndName, szName, sizeof( szName ) );
            if ( ( *szName ) && ( pi = _ptpl->m_itlist.Find( szName ) ) != NULL )
                {
                SendMessage( hwndColor, CB_SETCURSEL, pi->GetIdxClr( ), 0L );
                if ( pi->GetGraphType( ) == gtLine )
                    {
                    SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
                    SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                    }
                else if ( pi->GetGraphType( ) == gtBar )
                    {
                    SendMessage( hwndLine,  BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndBar,   BM_SETCHECK, TRUE,  0L );
                    SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                    }
                else
                    {
                    ASSERT( pi->GetGraphType( ) == gtPoint );
                    SendMessage( hwndLine,  BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndPoint, BM_SETCHECK, TRUE,  0L );
                    }
                if ( pi->rtti( ) == QIndicatorTemplate::rtExternal )
                    {
                    piExternal = (QIndicatorTemplateExternal *) pi;
                    SetText( hwndDefn, piExternal->GetExternalName( ) );
                    SetText( hwndHorz, piExternal->GetHorz( ) );
                    EnableWindow( hwndOK,    TRUE );
                    EnableWindow( hwndColor, TRUE );
                    EnableWindow( hwndLine,  TRUE );
                    EnableWindow( hwndBar,   TRUE );
                    EnableWindow( hwndPoint, TRUE );
                    EnableWindow( hwndDefn,  TRUE );
                    EnableWindow( hwndHorz,  TRUE );
                    }
                else
                    {
                    EnableWindow( hwndOK,    FALSE );
                    EnableWindow( hwndColor, FALSE );
                    EnableWindow( hwndLine,  FALSE );
                    EnableWindow( hwndBar,   FALSE );
                    EnableWindow( hwndPoint, FALSE );
                    EnableWindow( hwndDefn,  FALSE );
                    EnableWindow( hwndHorz,  FALSE );
                    ClearEdit( hwndDefn );
                    ClearEdit( hwndHorz );
                    }
                }
            else
                {
                SendMessage( hwndColor, CB_SETCURSEL, 0, 0L );
                SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
                SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                EnableWindow( hwndOK,    *szName );
                EnableWindow( hwndColor, TRUE );
                EnableWindow( hwndLine,  TRUE );
                EnableWindow( hwndBar,   TRUE );
                EnableWindow( hwndPoint, TRUE );
                EnableWindow( hwndDefn,  TRUE );
                EnableWindow( hwndHorz,  TRUE );
                }
            return TRUE;

        case IDOK:
            GetText( hwndName, szName, sizeof( szName ) );
            ASSERT( *szName );
            if ( !ValidName( szName ) )
                {
                MsgBox( hDlg, IDS_BADNAME );
                SetFocus( hwndName );
                SelectCBEdit( hwndName );
                return TRUE;
                }
            
            fAllocated = FALSE;
            fAdded     = FALSE;
            if ( (BOOL) SendMessage( hwndLine, BM_GETCHECK, 0, 0L ) )
                gt = gtLine;
            else if ( (BOOL) SendMessage( hwndBar, BM_GETCHECK, 0, 0L ) )
                gt = gtBar;
            else
                gt = gtPoint;

            nIdxClr = (USHORT) SendMessage( hwndColor, CB_GETCURSEL, 0, 0L );
            ASSERT( nIdxClr < RGCLRSIZE );

            GetText( hwndDefn, szText, sizeof( szText ) );
            GetText( hwndHorz, szHorz, sizeof( szHorz ) );
            TRY
                {
                pi = _ptpl->m_itlist.Find( szName );
                if ( pi )
                    {
                    ASSERT( pi->rtti( ) == QIndicatorTemplate::rtExternal );
                    piExternal = (QIndicatorTemplateExternal *) pi;
                    }
                else
                    {
                    piExternal = new QIndicatorTemplateExternal;
                    if ( !piExternal )
                        {
                        THROW_EXCEPTION( exMemory );
                        }
                    fAllocated = TRUE;
                    }
                piExternal->SetName( szName );
                piExternal->SetExternalName( szText );
                piExternal->SetHorz( szHorz );
                piExternal->SetIdxClr( nIdxClr );
                piExternal->SetGraphType( gt );
                if ( fAllocated )
                    {
                    _ptpl->m_itlist.AddTail( piExternal );
                    fAdded = TRUE;
                    SendMessage( hwndName, CB_ADDSTRING, 0, (LPARAM)(LPSTR) szName );
                    }
                _ptpl->SetModifiedFlag( TRUE );
                UpdateMenu( );
                gcache.Cleanup( );
                fModified = TRUE;
                SetFocus( hwndName );
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                if ( fAdded )
                    {
                    f = _ptpl->m_itlist.Remove( piExternal );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete piExternal;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                if ( fAdded )
                    {
                    f = _ptpl->m_itlist.Remove( piExternal );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete piExternal;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            if ( fModified )
                {
                UpdateAllViews( TRUE );
                }
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// CustomITProc( )
//
// Manages custom indicator templates.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export CustomITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    CHAR        szName[ _cbNameMax + 1 ];
    CHAR        szText[ _cbTextMax + 1 ];
    CHAR        szHorz[ _cbTextMax + 1 ];
    UINT        nIdxClr;
    GraphType   gt;
    BOOL        f;
    BOOL        fAdded;
    BOOL        fAllocated;
    static BOOL fModified;
    static HWND hwndName;
    static HWND hwndColor;
    static HWND hwndLine;
    static HWND hwndBar;
    static HWND hwndPoint;
    static HWND hwndOK;
    static HWND hwndDefn;
    static HWND hwndHorz;
    QIndicatorTemplate *pi;
    QIndicatorTemplateCustom *piCustom;

    switch( message )
        {
    case WM_INITDIALOG:
        fModified = FALSE;
        hwndName  = GetDlgItem( hDlg, IDC_TEMPLATE );
        hwndColor = GetDlgItem( hDlg, IDC_COLOR );
        hwndLine  = GetDlgItem( hDlg, IDC_LINE );
        hwndBar   = GetDlgItem( hDlg, IDC_BAR );
        hwndPoint = GetDlgItem( hDlg, IDC_POINT );
        hwndOK    = GetDlgItem( hDlg, IDOK );
        hwndDefn  = GetDlgItem( hDlg, IDC_DEFN );
        hwndHorz  = GetDlgItem( hDlg, IDC_HORZ );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndDefn, EM_LIMITTEXT, _cbTextMax, 0L );
        SendMessage( hwndHorz, EM_LIMITTEXT, _cbTextMax, 0L );
        ComboAddColors( hwndColor );
        ComboAddTemplates( hwndName );
        SendMessage( hwndColor, CB_SETCURSEL, 0, 0L );
        SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
        SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
        SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND: 
        switch( wParam )
            {
        case IDC_TEMPLATE:
            PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
            return TRUE;

        case IDC_RESERVED:
            GetText( hwndName, szName, sizeof( szName ) );
            if ( ( *szName ) && ( pi = _ptpl->m_itlist.Find( szName ) ) != NULL )
                {
                SendMessage( hwndColor, CB_SETCURSEL, pi->GetIdxClr( ), 0L );
                if ( pi->GetGraphType( ) == gtLine )
                    {
                    SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
                    SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                    }
                else if ( pi->GetGraphType( ) == gtBar )
                    {
                    SendMessage( hwndLine,  BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndBar,   BM_SETCHECK, TRUE,  0L );
                    SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                    }
                else
                    {
                    ASSERT( pi->GetGraphType( ) == gtPoint );
                    SendMessage( hwndLine,  BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                    SendMessage( hwndPoint, BM_SETCHECK, TRUE,  0L );
                    }
                if ( pi->rtti( ) == QIndicatorTemplate::rtCustom )
                    {
                    piCustom = (QIndicatorTemplateCustom *) pi;
                    SetText( hwndDefn, piCustom->GetDefn( ) );
                    SetText( hwndHorz, piCustom->GetHorz( ) );
                    EnableWindow( hwndOK,    TRUE );
                    EnableWindow( hwndColor, TRUE );
                    EnableWindow( hwndLine,  TRUE );
                    EnableWindow( hwndBar,   TRUE );
                    EnableWindow( hwndPoint, TRUE );
                    EnableWindow( hwndDefn,  TRUE );
                    EnableWindow( hwndHorz,  TRUE );
                    }
                else
                    {
                    EnableWindow( hwndOK,    FALSE );
                    EnableWindow( hwndColor, FALSE );
                    EnableWindow( hwndLine,  FALSE );
                    EnableWindow( hwndBar,   FALSE );
                    EnableWindow( hwndPoint, FALSE );
                    EnableWindow( hwndDefn,  FALSE );
                    EnableWindow( hwndHorz,  FALSE );
                    ClearEdit( hwndDefn );
                    ClearEdit( hwndHorz );
                    }
                }
            else
                {
                SendMessage( hwndColor, CB_SETCURSEL, 0, 0L );
                SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
                SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                EnableWindow( hwndOK,    *szName );
                EnableWindow( hwndColor, TRUE );
                EnableWindow( hwndLine,  TRUE );
                EnableWindow( hwndBar,   TRUE );
                EnableWindow( hwndPoint, TRUE );
                EnableWindow( hwndDefn,  TRUE );
                EnableWindow( hwndHorz,  TRUE );
                }
            return TRUE;

        case IDOK:
            GetText( hwndName, szName, sizeof( szName ) );
            ASSERT( *szName );
            if ( !ValidName( szName ) )
                {
                MsgBox( hDlg, IDS_BADNAME );
                SetFocus( hwndName );
                SelectCBEdit( hwndName );
                return TRUE;
                }
            
            fAllocated = FALSE;
            fAdded     = FALSE;
            if ( (BOOL) SendMessage( hwndLine, BM_GETCHECK, 0, 0L ) )
                gt = gtLine;
            else if ( (BOOL) SendMessage( hwndBar, BM_GETCHECK, 0, 0L ) )
                gt = gtBar;
            else
                gt = gtPoint;

            nIdxClr = (USHORT) SendMessage( hwndColor, CB_GETCURSEL, 0, 0L );
            ASSERT( nIdxClr < RGCLRSIZE );

            GetText( hwndDefn, szText, sizeof( szText ) );
            GetText( hwndHorz, szHorz, sizeof( szHorz ) );
            TRY
                {
                pi = _ptpl->m_itlist.Find( szName );
                if ( pi )
                    {
                    ASSERT( pi->rtti( ) == QIndicatorTemplate::rtCustom );
                    piCustom = (QIndicatorTemplateCustom *) pi;
                    }
                else
                    {
                    piCustom = new QIndicatorTemplateCustom;
                    if ( !piCustom )
                        {
                        THROW_EXCEPTION( exMemory );
                        }
                    fAllocated = TRUE;
                    }
                piCustom->SetName( szName );
                piCustom->SetDefn( szText );
                piCustom->SetHorz( szHorz );
                piCustom->SetIdxClr( nIdxClr );
                piCustom->SetGraphType( gt );
                if ( fAllocated )
                    {
                    _ptpl->m_itlist.AddTail( piCustom );
                    fAdded = TRUE;
                    SendMessage( hwndName, CB_ADDSTRING, 0, (LPARAM)(LPSTR) szName );
                    }
                _ptpl->SetModifiedFlag( TRUE );
                UpdateMenu( );
                gcache.Cleanup( );
                fModified = TRUE;
                SetFocus( hwndName );
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                if ( fAdded )
                    {
                    f = _ptpl->m_itlist.Remove( piCustom );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete piCustom;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                if ( fAdded )
                    {
                    f = _ptpl->m_itlist.Remove( piCustom );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete piCustom;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            if ( fModified )
                {
                UpdateAllViews( TRUE );
                }
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// CreateITProc( )
//
// Manages the create indicator template dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export CreateITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static BOOL fModified;
    static HWND hwndName;
    static HWND hwndPath;
    static HWND hwndColor;
    static HWND hwndLine;
    static HWND hwndBar;
    static HWND hwndPoint;
    static HWND hwndOK;
    static HWND hwndOptions;
    static OPTIONS opts;
    CHAR   szName[ _cbNameMax + 1 ];
    CHAR   szPath[ _cbPathMax + 1 ];
    USHORT nIdxClr;
    GraphType gt;
    QIndicatorTemplate *pi;
    QIndicatorTemplateNormal *piNormal;
    LPOPTIONSPROC    lpfnOptions;
    BOOL f;
    HANDLE hLibrary;
    BOOL fAllocated;
    BOOL fAdded;
    
    switch( message )
        {
    case WM_INITDIALOG:
        fModified = FALSE;
        hwndName  = GetDlgItem( hDlg, IDC_TEMPLATE );
        hwndPath  = GetDlgItem( hDlg, IDC_PATH );
        hwndColor = GetDlgItem( hDlg, IDC_COLOR );
        hwndLine  = GetDlgItem( hDlg, IDC_LINE );
        hwndBar   = GetDlgItem( hDlg, IDC_BAR );
        hwndPoint = GetDlgItem( hDlg, IDC_POINT );
        hwndOK    = GetDlgItem( hDlg, IDOK );
        hwndOptions = GetDlgItem( hDlg, IDC_OPTIONS );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndPath, EM_LIMITTEXT, _cbPathMax, 0L );
        ComboAddColors( hwndColor );
        ComboAddTemplates( hwndName );
        OptionsZero( &opts );
        SendMessage( hwndColor, CB_SETCURSEL, 0, 0L );
        SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
        SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
        SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDC_TEMPLATE:
                // This work-around gets around the editbox bug.
                // This bug notifies you before it updates its
                // window name.  When you get the window name,
                // you get the old name, not the new name.
                // That's no good.
                PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
                return TRUE;

            case IDC_RESERVED:
                OptionsFree( &opts );
                GetText( hwndName, szName, sizeof( szName ) );
                if ( ( *szName ) && ( pi = _ptpl->m_itlist.Find( szName ) ) != NULL )
                    {
                    SendMessage( hwndColor, CB_SETCURSEL, pi->GetIdxClr( ), 0L );
                    if ( pi->GetGraphType( ) == gtLine )
                        {
                        SendMessage( hwndLine,  BM_SETCHECK, TRUE,  0L );
                        SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                        SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                        }
                    else if ( pi->GetGraphType( ) == gtBar )
                        {
                        SendMessage( hwndLine,  BM_SETCHECK, FALSE, 0L );
                        SendMessage( hwndBar,   BM_SETCHECK, TRUE,  0L );
                        SendMessage( hwndPoint, BM_SETCHECK, FALSE, 0L );
                        }
                    else
                        {
                        ASSERT( pi->GetGraphType( ) == gtPoint );
                        SendMessage( hwndLine,  BM_SETCHECK, FALSE, 0L );
                        SendMessage( hwndBar,   BM_SETCHECK, FALSE, 0L );
                        SendMessage( hwndPoint, BM_SETCHECK, TRUE,  0L );
                        }

                    if ( pi->rtti( ) == QIndicatorTemplate::rtNormal )
                        {
                        // Normal indicators are OK
                        piNormal = (QIndicatorTemplateNormal *) pi;
                        ASSERT( piNormal->GetLibrary( ) != NULL );
                        ASSERT( *piNormal->GetLibrary( ) );
                        SetWindowText( hwndPath, piNormal->GetLibrary( ) );
                        if ( !OptionsCopy( &opts, piNormal->GetOptions( ) ) )
                            {
                            MsgBox( hDlg, IDS_MEMORY );
                            EndDialog( hDlg, FALSE );
                            return TRUE;
                            }
                        EnableWindow( hwndOptions, TRUE );
                        EnableWindow( hwndOK,      TRUE );
                        EnableWindow( hwndPath,    TRUE );
                        EnableWindow( hwndColor,   TRUE );
                        EnableWindow( hwndLine,    TRUE );
                        EnableWindow( hwndBar,     TRUE );
                        EnableWindow( hwndPoint,   TRUE );
                        }
                    else
                        {
                        // Custom indicators are no good.
                        EnableWindow( hwndOptions, FALSE );
                        EnableWindow( hwndOK,      FALSE );
                        EnableWindow( hwndPath,    FALSE );
                        EnableWindow( hwndColor,   FALSE );
                        EnableWindow( hwndLine,    FALSE );
                        EnableWindow( hwndBar,     FALSE );
                        EnableWindow( hwndPoint,   FALSE );
                        ClearEdit( hwndPath );
                        }
                    }
                else
                    {
                    EnableWindow( hwndOK,      FALSE );
                    EnableWindow( hwndOptions, TRUE );
                    EnableWindow( hwndPath,    TRUE );
                    EnableWindow( hwndColor,   TRUE );
                    EnableWindow( hwndLine,    TRUE );
                    EnableWindow( hwndBar,     TRUE );
                    EnableWindow( hwndPoint,   TRUE );
                    }
                return TRUE;

            case IDC_PATH:
                OptionsFree( &opts );
                EnableWindow( hwndOK, FALSE );
                return TRUE;

            case IDC_OPTIONS:
                GetText( hwndName, szName, sizeof( szName ) );
                if ( !ValidName( szName ) )
                    {
                    MsgBox( hDlg, IDS_BADNAME );
                    SetFocus( hwndName );
                    SelectCBEdit( hwndName );
                    return TRUE;
                    }
#ifdef DEBUG
                pi = _ptpl->m_itlist.Find( szName );
                if ( pi != NULL )
                    {
                    ASSERT( pi->rtti( ) == QIndicatorTemplate::rtNormal );
                    }
#endif
                GetText( hwndPath, szPath, sizeof( szPath ) );
                if ( !*szPath )
                    {
                    MsgBox( hDlg, IDS_NOPATH );
                    SetFocus( hwndPath );
                    SelectEdit( hwndPath );
                    return TRUE;
                    }
                hLibrary = LoadLibrary( szPath );
                if ( hLibrary < 32 )
                    {
                    MsgBox( hDlg, IDS_BADINDICATOR );
                    SetFocus( hwndPath );
                    SelectEdit( hwndPath );
                    return TRUE;
                    }
                lpfnOptions = (LPOPTIONSPROC) GetProcAddress( hLibrary, GetString( IDS_ITPROC_OPTIONS ) );    
                if ( !lpfnOptions )
                    {
                    MsgBox( hDlg, IDS_BADINDICATOR );
                    SetFocus( hwndPath );
                    SelectEdit( hwndPath );
                    FreeLibrary( hLibrary );
                    return TRUE;
                    }
                opts.hwndParent = hDlg;
                f = (*lpfnOptions)( &opts );
                FreeLibrary( hLibrary );
                if ( f )
                    {
                    EnableWindow( hwndOK, TRUE );
                    }
                return TRUE;

            case IDOK:
                fAllocated = FALSE;
                fAdded     = FALSE;
                TRY
                    {
                    if ( (BOOL) SendMessage( hwndLine, BM_GETCHECK, 0, 0L ) )
                        gt = gtLine;
                    else if ( (BOOL) SendMessage( hwndBar, BM_GETCHECK, 0, 0L ) )
                        gt = gtBar;
                    else
                        gt = gtPoint;

                    nIdxClr = (USHORT) SendMessage( hwndColor, CB_GETCURSEL, 0, 0L );
                    ASSERT( nIdxClr < RGCLRSIZE );
                    GetText( hwndName, szName, sizeof( szName ) );
                    ASSERT( *szName );
                    GetText( hwndPath, szPath, sizeof( szPath ) );
                    ASSERT( *szPath );
                    pi = _ptpl->m_itlist.Find( szName );
                    if ( pi )
                        {
                        ASSERT( pi->rtti( ) == QIndicatorTemplate::rtNormal );
                        piNormal = (QIndicatorTemplateNormal *) pi;
                        }
                    else
                        {
                        piNormal = new QIndicatorTemplateNormal;
                        if ( !piNormal )
                            {
                            THROW_EXCEPTION( exMemory );
                            }
                        fAllocated = TRUE;
                        }
                    piNormal->SetName( szName );
                    piNormal->SetLibrary( szPath );
                    piNormal->SetIdxClr( nIdxClr );
                    piNormal->SetGraphType( gt );
                    piNormal->SetOptions( &opts );
                    if ( fAllocated )
                        {
                        _ptpl->m_itlist.AddTail( piNormal );
                        fAdded = TRUE;
                        SendMessage( hwndName, CB_ADDSTRING, 0, (LPARAM)(LPSTR) szName );
                        }
                    _ptpl->SetModifiedFlag( TRUE );
                    UpdateMenu( );
                    gcache.Cleanup( );
                    fModified = TRUE;
                    SetFocus( hwndName );
                    }
                CATCH( exMemory )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    if ( fAdded )
                        {
                        f = _ptpl->m_itlist.Remove( piNormal );
                        ASSERT( f );
                        }
                    if ( fAllocated )
                        {
                        delete piNormal;
                        }
                    gcache.Cleanup( );
                    EndDialog( hDlg, FALSE );
                    }
                CATCH_ALL
                    {
                    MsgBox( hDlg, IDS_GENERALERROR );
                    if ( fAdded )
                        {
                        f = _ptpl->m_itlist.Remove( piNormal );
                        ASSERT( f );
                        }
                    if ( fAllocated )
                        {
                        delete piNormal;
                        }
                    gcache.Cleanup( );
                    EndDialog( hDlg, FALSE );
                    }
                END_CATCH
                return TRUE;

            case IDCANCEL:
                OptionsFree( &opts );
                if ( fModified )
                    {
                    UpdateAllViews( TRUE );
                    }
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// DeleteCondProc( )
//
// Manages Conditions.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DeleteCondProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    BOOL f;
    QCondition *pcond;
    CHAR szName[ _cbNameMax + 1 ];
    static HWND hwndName;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndName = GetDlgItem( hDlg, IDC_CONDITION );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        ComboAddConditions( hwndName );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
        
    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                TASKLIST
                    {
                    GetText( hwndName, szName, sizeof( szName ) );
                    pcond = _ptpl->m_condlist.Find( szName );
                    ASSERT( pcond != NULL );
                    if ( _ptpl->ConditionInUse( szName ) )
                        {
                        MsgBox( hDlg, IDS_CONDITIONINUSE );
                        QUIT;
                        }
                    f = _ptpl->m_condlist.Remove( pcond );
                    ASSERT( f );
                    delete pcond;
                    ComboDeleteString( hwndName, szName );
                    UpdateMenu( );
                    gcache.Cleanup( );
                    _ptpl->SetModifiedFlag( TRUE );
                    if ( SendMessage( hwndName, CB_GETCOUNT, 0, 0L ) != 0 )
                        {
                        QUIT;
                        }
                    UpdateAllViews( TRUE );
                    EndDialog( hDlg, TRUE );
                    }
                ENDTASKLIST
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;
            
        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// CreateConstProc( )
//
// Manages Constants.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export CreateConstProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    BOOL        fAllocated;
    BOOL        fAdded;
    BOOL        f;
    static BOOL fModified;
    static HWND hwndName;
    static HWND hwndDefn;
    static HWND hwndOK;
    CHAR        szName[ _cbNameMax + 1 ];
    CHAR        szText[ _cbTextMax + 1 ];
    QConstant  *pconst;
    
    switch( message )
        {
    case WM_INITDIALOG:
        fModified = FALSE;
        hwndName  = GetDlgItem( hDlg, IDC_CONST );
        hwndDefn  = GetDlgItem( hDlg, IDC_DEFN );
        hwndOK    = GetDlgItem( hDlg, IDOK );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndDefn, EM_LIMITTEXT, _cbTextMax, 0L );
        ComboAddConstants( hwndName );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
        
    case WM_COMMAND:
        switch( wParam )
            {
        case IDC_CONST:
            PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
            return TRUE;

        case IDC_RESERVED:
            GetText( hwndName, szName, sizeof( szName ) );
            if ( ( *szName ) && ( pconst = _ptpl->m_constlist.Find( szName ) ) != NULL )
                {
                sprintf( szText, "%f", (NUM) pconst->GetValue( ) );
                SetText( hwndDefn, szText );
                }
            else
                {
                ClearEdit( hwndDefn );
                }
            EnableWindow( hwndOK, *szName );
            return TRUE;

        case IDOK:
            GetText( hwndName, szName, sizeof( szName ) );
            ASSERT( *szName );
            if ( !ValidName( szName ) )
                {
                MsgBox( hDlg, IDS_BADNAME );
                SetFocus( hwndName );
                SelectCBEdit( hwndName );
                return TRUE;
                }
            
            fAllocated = FALSE;
            fAdded     = FALSE;
            
            GetText( hwndDefn, szText, sizeof( szText ) );
            TRY
                {
                pconst = _ptpl->m_constlist.Find( szName );
                if ( !pconst )
                    {
                    pconst = new QConstant;
                    if ( !pconst )
                        {
                        THROW_EXCEPTION( exMemory );
                        }    
                    fAllocated = TRUE;
                    }
                pconst->SetName( szName );
                pconst->SetValue( (NUM) atof( szText ) );
                if ( fAllocated )
                    {
                    _ptpl->m_constlist.AddTail( pconst );
                    fAdded = TRUE;
                    SendMessage( hwndName, CB_ADDSTRING, 0, (LPARAM)(LPSTR) szName );
                    }
                _ptpl->SetModifiedFlag( TRUE );
                UpdateMenu( );
                gcache.Cleanup( );
                fModified = TRUE;
                SetFocus( hwndName );
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                if ( fAdded )
                    {
                    f = _ptpl->m_constlist.Remove( pconst );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete pconst;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                if ( fAdded )
                    {
                    f = _ptpl->m_constlist.Remove( pconst );
                    ASSERT( f );
                    }
                if ( fAllocated )
                    {
                    delete pconst;
                    }
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            if ( fModified )
                UpdateAllViews( TRUE );
                
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// DeleteConstProc( )
//
// Manages Constants.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DeleteConstProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    BOOL f;
    QConstant *pconst;
    CHAR szName[ _cbNameMax + 1 ];
    static HWND hwndName;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndName = GetDlgItem( hDlg, IDC_CONST );
        ComboAddConstants( hwndName );
        return TRUE;

    case WM_DRAWITEM:
        DrawColors( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );
        
    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                TASKLIST
                    {
                    GetText( hwndName, szName, sizeof( szName ) );
                    pconst = _ptpl->m_constlist.Find( szName );
                    ASSERT( pconst != NULL );
                    f = _ptpl->m_constlist.Remove( pconst );
                    ASSERT( f );
                    delete pconst;
                    ComboDeleteString( hwndName, szName );
                    UpdateMenu( );
                    gcache.Cleanup( );
                    _ptpl->SetModifiedFlag( TRUE );
                    if ( SendMessage( hwndName, CB_GETCOUNT, 0, 0L ) != 0 )
                        {
                        QUIT;
                        }
                    UpdateAllViews( TRUE );
                    EndDialog( hDlg, TRUE );
                    }
                ENDTASKLIST
                }
            CATCH( exMemory )
                {
                MsgBox( hDlg, IDS_MEMORY );
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                MsgBox( hDlg, IDS_GENERALERROR );
                gcache.Cleanup( );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;
            
        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// DeleteITProc( )
//
// Manages the delete indicator template dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DeleteITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    BOOL   f;
    static HWND hwndName;
    QIndicatorTemplate *pi;
    CHAR szName[ _cbNameMax + 1 ];
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndName = GetDlgItem( hDlg, IDC_TEMPLATE );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        ComboAddTemplates( hwndName );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                TRY
                    {
                    TASKLIST
                        {
                        GetText( hwndName, szName, sizeof( szName ) );
                        pi = _ptpl->m_itlist.Find( szName );
                        ASSERT( pi != NULL );
                        if ( _ptpl->IndicatorInUse( szName ) )
                            {
                            MsgBox( hDlg, IDS_INDICATORINUSE );
                            QUIT;
                            }
                        f = _ptpl->m_itlist.Remove( pi );
                        ASSERT( f );
                        delete pi;
                        ComboDeleteString( hwndName, szName );
                        UpdateMenu( );
                        gcache.Cleanup( );
                        _ptpl->SetModifiedFlag( TRUE );
                        if ( SendMessage( hwndName, CB_GETCOUNT, 0, 0L ) != 0 )
                            {
                            QUIT;
                            }
                        UpdateAllViews( TRUE );
                        EndDialog( hDlg, TRUE );
                        }
                    ENDTASKLIST
                    }
                CATCH( exMemory )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    gcache.Cleanup( );
                    EndDialog( hDlg, FALSE );
                    }
                CATCH_ALL
                    {
                    MsgBox( hDlg, IDS_GENERALERROR );
                    gcache.Cleanup( );
                    EndDialog( hDlg, FALSE );
                    }
                END_CATCH
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// CreateWLProc( )
//
// Manages the create window layout dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export CreateWLProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static BOOL fModified;
    static HWND hwndName;
    static HWND hwndDxIncr;
    static HWND hwndOK;
    static HWND hwndViewA;
    static HWND hwndViewB;
    static HWND hwndViewC;
    static HWND hwndViewD;
    static HWND hwndViewTitle;
    static HWND hwndAutoOpen;
    static HWND hwndExclude;
    static HWND hwndInclude;
    static HWND hwndAdd;
    static HWND hwndDelete;
    static HWND hwndHLCV;
    static HWND hwndHorz;
    static HWND hwndLogScaling;
    static HWND hwndRoundScaling;
    static HWND hwndGrids;
    static HWND hwndPercent;
    static HWND hwndRule;
    static HWND hwndCond;
    CHAR   szName[ _cbNameMax + 1 ];
    CHAR   szRule[ _cbNameMax + 1 ];
    CHAR   szCond[ _cbNameMax + 1 ];
    CHAR   szTmp[ _cbBufferMax + 1 ];
    QWindowLayout *pwl;
    QIndicatorName *pName;
    BOOL   fAutoOpen;
    BOOL   fAllocated;
    BOOL   fAdded;
    BOOL   f;
    BOOL   fError;
    int    nSum;
    int    i;

    static BOOL   fShow;
    static int    dxincr;
    static USHORT nIndex;
    static CHAR   rgszViewTitle[ _cViewLayouts ][ _cbTitleMax + 1 ];
    static int    rgnRuleIndex[ _cViewLayouts ];
    static int    rgnCondIndex[ _cViewLayouts ];
    static BOOL   rgfHLCV[ _cViewLayouts ];
    static BOOL   rgfHorz[ _cViewLayouts ];
    static BOOL   rgfLogScaling[ _cViewLayouts ];
    static BOOL   rgfRoundScaling[ _cViewLayouts ];
    static int    rgnGrids[ _cViewLayouts ];
    static int    rgnPercent[ _cViewLayouts ];
    static QIndicatorDictionary rgDictInc[ _cViewLayouts ];

    switch( message )
        {
    case WM_INITDIALOG:
        fModified        = FALSE;
        hwndName         = GetDlgItem( hDlg, IDC_LAYOUT );
        hwndDxIncr       = GetDlgItem( hDlg, IDC_DXINCR );
        hwndOK           = GetDlgItem( hDlg, IDOK );
        hwndViewA        = GetDlgItem( hDlg, IDC_A );
        hwndViewB        = GetDlgItem( hDlg, IDC_B );
        hwndViewC        = GetDlgItem( hDlg, IDC_C );
        hwndViewD        = GetDlgItem( hDlg, IDC_D );
        hwndViewTitle    = GetDlgItem( hDlg, IDC_VIEW_TITLE );
        hwndAutoOpen     = GetDlgItem( hDlg, IDC_AUTO_OPEN );
        hwndExclude      = GetDlgItem( hDlg, IDC_EXCLUDE );
        hwndInclude      = GetDlgItem( hDlg, IDC_INCLUDE );
        hwndAdd          = GetDlgItem( hDlg, IDC_INCLUDE_PUSH );
        hwndDelete       = GetDlgItem( hDlg, IDC_EXCLUDE_PUSH );
        hwndHLCV         = GetDlgItem( hDlg, IDC_SHOW_HLC );
        hwndHorz         = GetDlgItem( hDlg, IDC_SHOW_HG );
        hwndLogScaling   = GetDlgItem( hDlg, IDC_LOGSCALING );
        hwndRoundScaling = GetDlgItem( hDlg, IDC_ROUNDSCALING );
        hwndGrids        = GetDlgItem( hDlg, IDC_GRIDSPERAXIS );
        hwndPercent      = GetDlgItem( hDlg, IDC_REALESTATE );
        hwndRule         = GetDlgItem( hDlg, IDC_RULE );
        hwndCond         = GetDlgItem( hDlg, IDC_CONDITION );
        SendMessage( hwndName,      CB_LIMITTEXT, _cbNameMax,   0L );
        SendMessage( hwndDxIncr,    EM_LIMITTEXT, _cbBufferMax, 0L );
        SendMessage( hwndViewTitle, EM_LIMITTEXT, _cbTitleMax,  0L );
        SendMessage( hwndPercent,   EM_LIMITTEXT, _cbBufferMax, 0L );
        SendMessage( hwndGrids,     EM_LIMITTEXT, _cbBufferMax, 0L );
        ComboAddWLs( hwndName );
        ComboAddRules( hwndRule, TRUE );
        ComboAddConditions( hwndCond, TRUE );
        fShow = TRUE;
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDC_LAYOUT:
                PostMessage( hDlg, WM_COMMAND, IDC_RESERVED, lParam );
                return TRUE;

            case IDC_RESERVED:
                TRY
                    {
                    GetText( hwndName, szName, sizeof( szName ) );
                    // Optimization: *szName
                    if ( ( *szName ) && 
                         ( ( pwl = _ptpl->m_wllist.Find( szName ) ) != NULL ) ) 
                        {
                        dxincr = (int) pwl->GetDxIncr( );
                        wsprintf( szTmp, "%d", dxincr );
                        SetWindowText( hwndDxIncr, szTmp );

                        fAutoOpen = pwl->GetAutoOpen( );
                        SendMessage( hwndAutoOpen, BM_SETCHECK, fAutoOpen, 0L );
                    
                        nIndex = 0;
                        SendMessage( hwndViewA, BM_SETCHECK, TRUE, 0L );
                        SendMessage( hwndViewB, BM_SETCHECK, FALSE, 0L );
                        SendMessage( hwndViewC, BM_SETCHECK, FALSE, 0L );
                        SendMessage( hwndViewD, BM_SETCHECK, FALSE, 0L );
                                                
                        for ( i = 0; i < _cViewLayouts; i++ )
                            {
                            ASSERT( pwl->m_rgvl[ i ].GetTitle( ) != NULL );
                            lstrcpy( rgszViewTitle[ i ], pwl->m_rgvl[ i ].GetTitle( ) );
                            rgfHLCV[ i ]         = pwl->m_rgvl[ i ].GetHLCV( );
                            rgfHorz[ i ]         = pwl->m_rgvl[ i ].GetHorz( );
                            rgfLogScaling[ i ]   = pwl->m_rgvl[ i ].GetLogScale( );
                            rgfRoundScaling[ i ] = pwl->m_rgvl[ i ].GetRoundScale( );
                            rgnGrids[ i ]        = pwl->m_rgvl[ i ].GetGrids( );
                            rgnPercent[ i ]      = pwl->m_rgvl[ i ].GetPercent( );
                            rgDictInc[ i ]       = pwl->m_rgvl[ i ].m_dict;
                            const CHAR *pszRuleName = pwl->m_rgvl[ i ].GetRule( );
                            if ( pszRuleName )
                                {      
                                rgnRuleIndex[ i ] = ComboFindString( hwndRule, pszRuleName );
                                ASSERT( rgnRuleIndex[ i ] >= 0 );
                                }
                            else
                                {
                                rgnRuleIndex[ i ] = 0;
                                }
                            
                            const CHAR *pszCondName = pwl->m_rgvl[ i ].GetCondition( );
                            if ( pszCondName )
                                {
                                rgnCondIndex[ i ] = ComboFindString( hwndCond, pszCondName );
                                ASSERT( rgnCondIndex[ i ] >= 0 );
                                }
                            else
                                {
                                rgnCondIndex[ i ] = 0;
                                }
                            }
                        SendMessage( hwndRule, CB_SETCURSEL, (WPARAM) rgnRuleIndex[ nIndex ],  0L );
                        SendMessage( hwndCond, CB_SETCURSEL, (WPARAM) rgnCondIndex[ nIndex ],  0L );
                        
                        SetWindowText( hwndViewTitle, rgszViewTitle[ nIndex ] );
                        SendMessage( hwndHLCV,         BM_SETCHECK, rgfHLCV[ nIndex ],         0L );
                        SendMessage( hwndHorz,         BM_SETCHECK, rgfHorz[ nIndex ],         0L );
                        SendMessage( hwndLogScaling,   BM_SETCHECK, rgfLogScaling[ nIndex ],   0L );
                        SendMessage( hwndRoundScaling, BM_SETCHECK, rgfRoundScaling[ nIndex ], 0L );

                        wsprintf( szTmp, "%d", rgnGrids[ nIndex ] );
                        SetWindowText( hwndGrids, szTmp );

                        wsprintf( szTmp, "%d", rgnPercent[ nIndex ] );
                        SetWindowText( hwndPercent, szTmp );

                        ComboAddExclude( hwndExclude, rgDictInc[ nIndex ] );
                        ComboAddInclude( hwndInclude, rgDictInc[ nIndex ] );
                        EnableWindow( hwndOK, TRUE );
                        fShow = TRUE;
                        }
                    else
                        {
                        // TODO: get the defaults by constructing
                        // empty template objects.
                        // Use defaults
                        nIndex = 0;

                        lmemset( rgszViewTitle,   0, sizeof( rgszViewTitle ) );
                        lmemset( rgfHLCV,         0, sizeof( rgfHLCV ) );
                        lmemset( rgfHorz,         0, sizeof( rgfHorz ) );
                        lmemset( rgfRoundScaling, 0, sizeof( rgfRoundScaling ) );
                        lmemset( rgfLogScaling,   0, sizeof( rgfLogScaling ) );
                        lmemset( rgnGrids,        0, sizeof( rgnGrids ) );
                        lmemset( rgnPercent,      0, sizeof( rgnPercent ) );
                        lmemset( rgnRuleIndex,    0, sizeof( rgnRuleIndex ) );
                        lmemset( rgnCondIndex,    0, sizeof( rgnCondIndex ) );

                        lstrcpy( rgszViewTitle[ 0 ], GetString( IDS_VIEWA ) );
                        lstrcpy( rgszViewTitle[ 1 ], GetString( IDS_VIEWB ) );
                        lstrcpy( rgszViewTitle[ 2 ], GetString( IDS_VIEWC ) );
                        lstrcpy( rgszViewTitle[ 3 ], GetString( IDS_VIEWD ) );
                        dxincr = 4;

                        rgnGrids[ 0 ] = rgnGrids[ 1 ] = rgnGrids[ 2 ] = rgnGrids[ 3 ] = 8;
                        rgfHorz [ 0 ] = rgfHorz [ 1 ] = rgfHorz [ 2 ] = rgfHorz [ 3 ] = TRUE; 
                    
                        rgnPercent[ 0 ] = 100;

                        for( i = 0; i < _cViewLayouts; i++ )
                            {
                            rgDictInc[ i ].Cleanup( );
                            }

                        rgfHLCV[ nIndex ] = TRUE;

                        // This is used to prevent 'flashing' because
                        // we get a lot of IDC_LAYOUT messages
                        if ( fShow )
                            {
                            wsprintf( szTmp, "%d", dxincr );
                            SetWindowText( hwndDxIncr, szTmp );
                            SendMessage( hwndAutoOpen, BM_SETCHECK, FALSE, 0L );

                            SendMessage( hwndViewA, BM_SETCHECK, TRUE, 0L );
                            SendMessage( hwndViewB, BM_SETCHECK, FALSE, 0L );
                            SendMessage( hwndViewC, BM_SETCHECK, FALSE, 0L );
                            SendMessage( hwndViewD, BM_SETCHECK, FALSE, 0L );
                            SendMessage( hwndRule, CB_SETCURSEL, (WPARAM) rgnRuleIndex[ nIndex ],  0L );
                            SendMessage( hwndCond, CB_SETCURSEL, (WPARAM) rgnCondIndex[ nIndex ],  0L );

                            SetWindowText( hwndViewTitle, rgszViewTitle[ nIndex ] );

                            wsprintf( szTmp, "%d", rgnGrids[ nIndex ] );
                            SetWindowText( hwndGrids, szTmp );

                            wsprintf( szTmp, "%d", rgnPercent[ nIndex ] );
                            SetWindowText( hwndPercent, szTmp );
                        
                            SendMessage( hwndHLCV,         BM_SETCHECK, rgfHLCV[ nIndex ],         0L );
                            SendMessage( hwndHorz,         BM_SETCHECK, rgfHorz[ nIndex ],         0L );
                            SendMessage( hwndLogScaling,   BM_SETCHECK, rgfLogScaling[ nIndex ],   0L );
                            SendMessage( hwndRoundScaling, BM_SETCHECK, rgfRoundScaling[ nIndex ], 0L );
                    
                            ComboAddExclude( hwndExclude, rgDictInc[ nIndex ] );
                            ComboAddInclude( hwndInclude, rgDictInc[ nIndex ] );
                            fShow = FALSE;
                            }
                        EnableWindow( hwndOK, (BOOL) ( *szName ) );
                        }
                    }
                CATCH( exMemory )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    for ( i = 0; i < _cViewLayouts; i++ )
                        {
                        rgDictInc[ i ].Cleanup( );
                        }
                    EndDialog( hDlg, FALSE );
                    }
                CATCH_ALL
                    {
                    MsgBox( hDlg, IDS_GENERALERROR );
                    for ( i = 0; i < _cViewLayouts; i++ )
                        {
                        rgDictInc[ i ].Cleanup( );
                        }
                    EndDialog( hDlg, FALSE );
                    }
                END_CATCH
                return TRUE;
            
            case IDC_RULE:
                rgnRuleIndex[ nIndex ] = (int) SendMessage( hwndRule, CB_GETCURSEL, 0, 0L );
                ASSERT( rgnRuleIndex[ nIndex ] >= 0 );
                return TRUE;
                
            case IDC_CONDITION:
                rgnCondIndex[ nIndex ] = (int) SendMessage( hwndCond, CB_GETCURSEL, 0, 0L );
                ASSERT( rgnCondIndex[ nIndex ] >= 0 );
                return TRUE;

            case IDC_VIEW_TITLE:
                GetText( hwndViewTitle, rgszViewTitle[ nIndex ], 
                         sizeof( rgszViewTitle[ nIndex ] ) );
                return TRUE;

            case IDC_SHOW_HG:
                rgfHorz[ nIndex ] = (BOOL) SendMessage( hwndHorz, BM_GETCHECK, 0, 0L );
                return TRUE;

            case IDC_SHOW_HLC:
                rgfHLCV[ nIndex ] = (BOOL) SendMessage( hwndHLCV, BM_GETCHECK, 0, 0L );
                return TRUE;

            case IDC_GRIDSPERAXIS:
                GetText( hwndGrids, szTmp, sizeof( szTmp ) );
                rgnGrids[ nIndex ] = atoi( szTmp );
                return TRUE;

            case IDC_REALESTATE:
                GetText( hwndPercent, szTmp, sizeof( szTmp ) );
                rgnPercent[ nIndex ] = atoi( szTmp );
                return TRUE;

            case IDC_LOGSCALING:
                rgfLogScaling[ nIndex ] = (BOOL) SendMessage( hwndLogScaling, BM_GETCHECK, 0, 0L );
                return TRUE;

            case IDC_ROUNDSCALING:
                rgfRoundScaling[ nIndex ] = (BOOL) SendMessage( hwndRoundScaling, BM_GETCHECK, 0, 0L );
                return TRUE;

            case IDC_EXCLUDE_PUSH:
                GetText( hwndInclude, szName, sizeof( szName ) );
                if ( !*szName )
                    {
                    return TRUE;
                    }
                pName = rgDictInc[ nIndex ].Find( szName );
                ASSERT( pName != NULL );
                f = rgDictInc[ nIndex ].Remove( pName );
                ASSERT( f );
                delete pName;
                ComboAddExclude( hwndExclude, rgDictInc[ nIndex ] );
                ComboAddInclude( hwndInclude, rgDictInc[ nIndex ] );
                return TRUE;

            case IDC_INCLUDE_PUSH:
                GetText( hwndExclude, szName, sizeof( szName ) );
                if ( !*szName )
                    {
                    return TRUE;
                    }
                pName = new QIndicatorName( szName );
                if ( !pName )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    return TRUE;
                    }
                rgDictInc[ nIndex ].AddTail( pName );
                ComboAddExclude( hwndExclude, rgDictInc[ nIndex ] );
                ComboAddInclude( hwndInclude, rgDictInc[ nIndex ] );
                return TRUE;

            case IDC_A:
            case IDC_B:
            case IDC_C:
            case IDC_D:
                if ( wParam == IDC_A )
                    nIndex = 0;
                else if ( wParam == IDC_B )
                    nIndex = 1;
                else if ( wParam == IDC_C )
                    nIndex = 2;
                else if ( wParam == IDC_D )
                    nIndex = 3;
                else
                    {
                    ASSERT( FALSE );
                    nIndex = 0;
                    }

                SetWindowText( hwndViewTitle,  rgszViewTitle[ nIndex ] );
                SendMessage( hwndRule, CB_SETCURSEL, (WPARAM) rgnRuleIndex[ nIndex ],  0L );
                SendMessage( hwndCond, CB_SETCURSEL, (WPARAM) rgnCondIndex[ nIndex ],  0L );
                SendMessage( hwndHLCV,         BM_SETCHECK, rgfHLCV[ nIndex ],         0L );
                SendMessage( hwndHorz,         BM_SETCHECK, rgfHorz[ nIndex ],         0L );
                SendMessage( hwndLogScaling,   BM_SETCHECK, rgfLogScaling[ nIndex ],   0L );
                SendMessage( hwndRoundScaling, BM_SETCHECK, rgfRoundScaling[ nIndex ], 0L );

                wsprintf( szTmp, "%d", rgnGrids[ nIndex ] );
                SetWindowText( hwndGrids, szTmp );

                wsprintf( szTmp, "%d", rgnPercent[ nIndex ] );
                SetWindowText( hwndPercent, szTmp );

                ComboAddExclude( hwndExclude, rgDictInc[ nIndex ] );
                ComboAddInclude( hwndInclude, rgDictInc[ nIndex ] );
                return TRUE;

            case IDOK:
                fAllocated = FALSE;
                fAdded     = FALSE;
                TRY
                    {
                    TASKLIST
                        {
                        GetText( hwndName, szName, sizeof( szName ) );
                        ASSERT( *szName );
                        if ( !ValidName( szName ) )
                            {
                            MsgBox( hDlg, IDS_BADNAME );
                            SetFocus( hwndName );
                            SelectCBEdit( hwndName );
                            QUIT;
                            }
                        fAutoOpen = (BOOL) SendMessage( hwndAutoOpen, BM_GETCHECK, 0, 0L );
                        GetText( hwndDxIncr, szTmp, sizeof( szTmp ) );
                        dxincr = atoi( szTmp );
                        if ( dxincr <= 0 )
                            {
                            MsgBox( hDlg, IDS_BADDXINCR );
                            SetFocus( hwndDxIncr );
                            SelectEdit( hwndDxIncr );
                            QUIT;
                            }
                        fError = FALSE;
                        nSum = 0;
                        for ( i = 0; i < _cViewLayouts; i++ )
                            {
                            if ( rgnPercent[ i ] < 0 )
                                {
                                fError = TRUE;
                                break;
                                }
                            nSum += rgnPercent[ i ];
                            }

                        if ( fError )
                            {
                            MsgBox( hDlg, IDS_BADPERCENT2 );
                            SetFocus( hwndPercent );
                            SelectEdit( hwndPercent );
                            QUIT;
                            }

                        if ( nSum != 100 )
                            {
                            MsgBox( hDlg, IDS_BADPERCENT );
                            SetFocus( hwndPercent );
                            SelectEdit( hwndPercent );
                            QUIT;
                            }

                        fError = FALSE;
                        for ( i = 0; !fError && i < _cViewLayouts; i++ )
                            {
                            if ( rgnGrids[ i ] <= 0 )
                                {
                                fError = TRUE;
                                }
                            }
                        
                        if ( fError )
                            {
                            MsgBox( hDlg, IDS_BADGRIDS );
                            SetFocus( hwndGrids );
                            SelectEdit( hwndGrids );
                            QUIT;
                            }
                             
                        pwl = _ptpl->m_wllist.Find( szName );
                        if ( !pwl )
                            {
                            pwl = new QWindowLayout;
                            if ( !pwl )
                                {
                                THROW_EXCEPTION( exMemory );
                                }
                            fAllocated = TRUE;
                            }
                        pwl->SetName( szName );
                        pwl->SetDxIncr( (USHORT) dxincr );
                        pwl->SetAutoOpen( fAutoOpen );
                        for ( i = 0; i < _cViewLayouts; i++ )
                            {
                            pwl->m_rgvl[ i ].SetTitle( rgszViewTitle[ i ] );
                            pwl->m_rgvl[ i ].SetHLCV( rgfHLCV[ i ] );
                            pwl->m_rgvl[ i ].SetHorz( rgfHorz[ i ] );
                            pwl->m_rgvl[ i ].SetLogScale( rgfLogScaling[ i ] );
                            pwl->m_rgvl[ i ].SetRoundScale( rgfRoundScaling[ i ] );
                            pwl->m_rgvl[ i ].SetGrids( (USHORT) rgnGrids[ i ] );
                            pwl->m_rgvl[ i ].SetPercent( (USHORT) rgnPercent[ i ] );
                            pwl->m_rgvl[ i ].m_dict = rgDictInc[ i ];
                            if ( rgnRuleIndex[ i ] > 0 )
                                {
                                int nTmp = GetCBText( hwndRule, szRule, sizeof( szRule ), rgnRuleIndex[ i ] );
                                ASSERT( nTmp != CB_ERR );
                                pwl->m_rgvl[ i ].SetRule( szRule );
                                }
                            else
                                {                                
                                // <norule> case
                                pwl->m_rgvl[ i ].SetRule( NULL );
                                }

                            if ( rgnCondIndex[ i ] > 0 )
                                {
                                int nTmp = GetCBText( hwndCond, szCond, sizeof( szCond ), rgnCondIndex[ i ] );
                                ASSERT( nTmp != CB_ERR );
                                pwl->m_rgvl[ i ].SetCondition( szCond );
                                }
                            else
                                {
                                // <nocond> case
                                pwl->m_rgvl[ i ].SetCondition( NULL );
                                }
                            }
                        if ( fAllocated )
                            {
                            _ptpl->m_wllist.AddTail( pwl );
                            fAdded = TRUE;
                            SendMessage( hwndName, CB_ADDSTRING, 0, (LPARAM)(LPSTR) szName );
                            }
                        _ptpl->SetModifiedFlag( TRUE );
                        UpdateMenu( );
                        fModified = TRUE;
                        SetFocus( hwndName );
                        }
                    ENDTASKLIST
                    }
                CATCH( exMemory )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    if ( fAdded )
                        {
                        f = _ptpl->m_wllist.Remove( pwl );
                        ASSERT( f );
                        }
                    if ( fAllocated )
                        {
                        delete pwl;
                        }
                    for ( i = 0; i < _cViewLayouts; i++ )
                        {
                        rgDictInc[ i ].Cleanup( );
                        }
                    EndDialog( hDlg, FALSE );
                    }
                CATCH_ALL
                    {
                    MsgBox( hDlg, IDS_GENERALERROR );
                    if ( fAdded )
                        {
                        f = _ptpl->m_wllist.Remove( pwl );
                        ASSERT( f );
                        }
                    if ( fAllocated )
                        {
                        delete pwl;
                        }
                    for ( i = 0; i < _cViewLayouts; i++ )
                        {
                        rgDictInc[ i ].Cleanup( );
                        }
                    EndDialog( hDlg, FALSE );
                    }
                END_CATCH
                return TRUE;

            case IDCANCEL:
                for ( i = 0; i < _cViewLayouts; i++ )
                    {
                    rgDictInc[ i ].Cleanup( );
                    }
                if ( fModified )
                    {
                    UpdateAllViews( TRUE );
                    }
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// DeleteWLProc( )
//
// Manages the delete window layout dialog box.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export DeleteWLProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    BOOL f;
    QWindowLayout *pwl;
    static HWND hwndName;
    CHAR szName[ _cbNameMax + 1 ];

    switch( message )
        {
    case WM_INITDIALOG:
        hwndName = GetDlgItem( hDlg, IDC_LAYOUT );
        SendMessage( hwndName, CB_LIMITTEXT, _cbNameMax, 0L );
        ComboAddWLs( hwndName );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
            case IDOK:
                TRY
                    {
                    TASKLIST
                        {
                        GetText( hwndName, szName, sizeof( szName ) );
                        if ( LayoutInUse( szName ) )
                            {
                            MsgBox( hDlg, IDS_LAYOUTINUSE );
                            QUIT;
                            }
                        pwl = _ptpl->m_wllist.Find( szName );
                        ASSERT( pwl != NULL );
                        f = _ptpl->m_wllist.Remove( pwl );
                        ASSERT( f );
                        delete pwl;
                        ComboDeleteString( hwndName, szName );
                        UpdateMenu( );
                        _ptpl->SetModifiedFlag( TRUE );
                        if ( SendMessage( hwndName, CB_GETCOUNT, 0, 0L ) != 0 )
                            {
                            QUIT;
                            }
                        UpdateAllViews( TRUE );
                        EndDialog( hDlg, TRUE );
                        }
                    ENDTASKLIST
                    }
                CATCH( exMemory )
                    {
                    MsgBox( hDlg, IDS_MEMORY );
                    EndDialog( hDlg, FALSE );
                    }
                CATCH_ALL
                    {
                    MsgBox( hDlg, IDS_GENERALERROR );
                    EndDialog( hDlg, FALSE );
                    }
                END_CATCH
                return TRUE;

            case IDCANCEL:
                EndDialog( hDlg, FALSE );
                return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// ParseDate( )
//
// Parses a date in mm/dd/yy format into 3 integer values.
//////////////////////////////////////////////////////////////////////////////
static BOOL ParseDate( CHAR *szDate, int *pmonth, int *pday, int *pyear )
    {
    int rgnDays[] =
        {
        31, // Jan
        28, // Feb
        31, // Mar
        30, // Apr
        31, // May
        30, // Jun
        31, // Jul
        31, // Aug
        30, // Sep
        31, // Oct
        30, // Nov
        31  // Dec
        };
    int nMonth;
    static CHAR szTokens[] = "/";
    CHAR *pch;
    BOOL fRet = FALSE;

    TASKLIST
        {
        pch = strtok( szDate, szTokens );
        if ( !pch )
            {
            QUIT;
            }
        *pmonth = atoi( pch );
        if ( *pmonth < 1 || *pmonth > 12 )
            {
            QUIT;
            }
        nMonth = *pmonth - 1;

        pch = strtok( NULL, szTokens );
        if ( !pch )
            {
            QUIT;
            }
        *pday = atoi( pch );
        if ( *pday < 1 )
            {
            QUIT;
            }
        
        pch = strtok( NULL, szTokens );
        if ( !pch )
            {
            QUIT;
            }
        *pyear = atoi( pch );
        *pyear = AdjustCentury( *pyear );

        pch = strtok( NULL, szTokens );
        if ( pch )
            {
            /* can't let the guy keep going! */
            QUIT;
            }

        if ( *pyear % 4 == 0 && *pmonth == 2 )
            {
            if ( *pday > 29 )
                {
                QUIT;
                }
            }
        else
            {
            if ( *pday > rgnDays[ nMonth ] )
                {
                QUIT;
                }
            }

        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// Adjust8( )
//
// This routine filters a text string such that if it contains
// a '+' or a ',', the next token is treated as a number in
// eighths.
//
// Example:
//
// 9+2 is interpreted as 9 + 2/8 = 9.25.
//////////////////////////////////////////////////////////////////////////////
static void Adjust8( CHAR *sz )
    {
    CHAR *pch;
    NUM   num;

    pch = strchr( sz, ',' );
    if ( pch == NULL )
        {
        pch = strchr( sz, '+' );
        if ( pch == NULL )
            {
            return;
            }
        }

    *pch++ = '\0';
    num = (NUM) atof( sz );
    num += (NUM) atof( pch ) / (NUM) 8;
    sprintf( sz, "%f", num );
    }

//////////////////////////////////////////////////////////////////////////////
// SimulatorProc( )
//
// This dialogbox manages requests for the trade simulator.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export SimulatorProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    SimulatorRequest req;
    static HWND hwndName;
    static HWND hwndLoadedCompany;
    static HWND hwndAllCompanies;
    static HWND hwndInitialAccount;
    static HWND hwndPosition;
    static HWND hwndComm;
    static HWND hwndExt;
    
    CHAR   szName[ _cbNameMax + 1 ];
    static CHAR szInitialAccount[ _cbNameMax + 1 ];
    static CHAR szPos [ _cbNameMax + 1 ];
    static CHAR szComm[ _cbNameMax + 1 ];
    static BOOL fLoadedCompany = TRUE;
    static BOOL fFirst = TRUE;
    static CHAR *szFilter[ ] = { "*.QT", "*.TXT", "*.*" };
    int n;
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndName           = GetDlgItem( hDlg, IDC_RULE );
        hwndLoadedCompany  = GetDlgItem( hDlg, IDC_LOADED_COMPANY );
        hwndAllCompanies   = GetDlgItem( hDlg, IDC_ALL_COMPANIES );
        hwndInitialAccount = GetDlgItem( hDlg, IDC_INITIAL_ACCOUNT );
        hwndPosition       = GetDlgItem( hDlg, IDC_POSITION );
        hwndComm           = GetDlgItem( hDlg, IDC_COMMISSION );
        hwndExt            = GetDlgItem( hDlg, IDC_FILE_EXT );

        SendMessage( hwndInitialAccount, EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndPosition,       EM_LIMITTEXT, _cbNameMax, 0L );
        SendMessage( hwndComm,           EM_LIMITTEXT, _cbNameMax, 0L );

        if ( fFirst )
            {
            fFirst = FALSE;
            lstrcpy( szInitialAccount, "10000.00" );
            lstrcpy( szPos, "2000.00" );
            lstrcpy( szComm, "25.00" );
            }

        ComboAddRules( hwndName );
        SendMessage( hwndLoadedCompany, BM_SETCHECK, fLoadedCompany, 0L );
        SendMessage( hwndAllCompanies,  BM_SETCHECK, !fLoadedCompany, 0L );
        SetWindowText( hwndInitialAccount, szInitialAccount );
        SetWindowText( hwndPosition, szPos );
        SetWindowText( hwndComm, szComm );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter1 );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter2 );
        SendMessage( hwndExt, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _pszFilter3 );
        SendMessage( hwndExt, CB_SELECTSTRING, -1, (LPARAM)(LPCSTR) _pszFilter2 );
        return TRUE;

    case WM_CTLCOLOR:
        return (BOOL) ManageCtlColor( wParam, lParam );

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                req.hwndParent = hDlg;
                GetText( hwndName, szName, sizeof( szName ) );
                req.sRuleName = szName;
                fLoadedCompany = (BOOL) SendMessage( hwndLoadedCompany, BM_GETCHECK, 0, 0L );
                if ( fLoadedCompany )
                    req.type = SimulatorRequest::eLoadedCompany;
                else
                    req.type = SimulatorRequest::eAllCompanies;
                
                GetText( hwndInitialAccount, szInitialAccount, sizeof( szInitialAccount ) );
                req.numInitialAccount = (NUM) atof( szInitialAccount );

                GetText( hwndPosition, szPos, sizeof( szPos ) );
                req.numPosition = (NUM) atof( szPos );

                GetText( hwndComm, szComm, sizeof( szComm ) );
                req.numComm = (NUM) atof( szComm );

                n = (int) SendMessage( hwndExt, CB_GETCURSEL, 0, 0L );
                ASSERT( n >= 0 && n < 3 );
                req.sFilter = szFilter[ n ];
                
                if ( req.numInitialAccount <= 0 ||
                     req.numComm           <  0 ||
                     req.numPosition       <  0 ||
                     req.numInitialAccount < req.numComm )
                    {
                    MessageBox( hDlg, "Invalid simulator request",
                        GetString( IDS_APPNAME), MB_ICONEXCLAMATION );
                    }
                else
                    {
                    ::Simulator( req );
                    EndDialog( hDlg, TRUE );
                    }
                }
            CATCH( exMemory )
                {
                req.Cleanup( );
                MsgBox( hDlg, IDS_MEMORY );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                req.Cleanup( );
                MsgBox( hDlg, IDS_GENERALERROR );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }
