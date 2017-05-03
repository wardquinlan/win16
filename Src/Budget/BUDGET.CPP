#include <qobject.hxx>
#include <stdlib.h>
#include <ctype.h>
#include <commdlg.h>
#include "id1.hxx"
#include "budget.hxx"
#include "dlg.hxx"
#include "view.hxx"
#include "profile.hxx"
#include "boxctrl.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

// constant strings
const CHAR _szAppName[]      = "Budget";
const CHAR _szClient[]       = "MDICLIENT";
const CHAR _szEmpty[]        = "";
const CHAR _szGeneral[]      = "General";
const CHAR _szSummary[]      = "Account Summary";
const CHAR _szTransactionsFormat[] = "%b %d, %Y";
const CHAR _szFilter[]              = "Budget Files (*.bdg)\0*.bdg\0All Files (*.*)\0*.*\0";
const CHAR _szTitleFormat[]         = "%s - %s";
const CHAR _szUntitled[]            = "Untitled";
const CHAR _szBudgetClass[] = "Budget";
const CHAR _szSummaryClass[ ] = "Summary";
const CHAR _szFrameClass[]  = "WinFrame";
const CHAR _szOperationFailed[]     = "Operation failed: %s %d.\n";

CHAR _szTitle[ cbTitleMax + 1 ];

HANDLE _hInstance;   // Application instance data
HWND   _hwndFrame;   // Main Frame window
HWND   _hwndClient;  // Client window
HWND   _hDlgModeless;// modeless dialog box
HMENU  _hmenu;       // main menu
HMENU  _hmenuWindow; // window menu
HANDLE _hAccel;      // Accelerator handle

// Windows Resources
HPEN _hpenDkGray;
HPEN _hpenLtGray;
HPEN _hpenGray;
HFONT _hfontNormal;
HFONT _hfontBold;
HBRUSH _hbrFill;

// Helper functions
#ifdef DEBUG
static VOID SetTracing( int nIdm, int nTraceLevel );
#endif

#ifdef DEBUG
int _nTraceIdm;      // trace level IDM
#endif

BOOL _fCriticalResources = FALSE; // Set to TRUE when Resources M.B. displayed

// Declare the document object
QDocument *_pdoc;

// Declare the version object
QVersion _vBudget( 2, 0, 1 );

// Declare the owner-draw button stuff
static WORD _rgidm[] = 
    {
    IDM_NEW,
    IDM_OPEN,
    IDM_SAVE,
    IDM_EXIT,
    IDM_NOOP,
    IDM_CREATE,
    IDM_TRANSACTIONS,
    IDM_DELETE,
    IDM_XFER,
    IDM_MACRO,
    IDM_NOOP,
    IDM_TILE,
    IDM_CALCULATOR,
    IDM_NEWWINDOW
    };
#define nButtons ( sizeof( _rgidm ) / sizeof( _rgidm[ 0 ] ) )
static HWND     _rgrghwndButton[ nButtons ][ 2 ];
static HANDLE   _rgrghButton[ nButtons ][ 2 ];
static CHAR     *_rgszButton[ nButtons ][ 2 ]  =
    {
        { "new",      "new2"      },
        { "open",     "open2"     },
        { "save",     "save2"     },
        { "x",        "x2"        },
        { "blank",    "blank2"    },
        { "account",  "account2"  },
        { "pen",      "pen2"      },
        { "erase",    "erase2"    },
        { "xfer",     "xfer2"     },
        { "m",        "m2"        },
        { "blank",    "blank2"    },
        { "tile",     "tile2"     },
        { "calc",     "calc2"     },
        { "wind",     "wind2"     }
    };

BOOL MenuController::editTransactionsMode_ = FALSE;

int FAR PASCAL WinMain(HANDLE hInst,
                       HANDLE hPrevInstance,
                       LPSTR  lpszCmdLine,
                       int    nCmdShow)
    {
    MSG      msg;
    WNDCLASS wc;
    RECT     rc;
    int      x, y, dx, dy;

    // Default exception handler
    DEFAULT_EXCEPTION_BLOCK
        {
        MessageBox( NULL, "An exception has occurred in budget.", _szAppName, MB_OK );
        return 0;
        }

    _hInstance = hInst;
    if (!hPrevInstance)
        {
        // register the frame class
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = FrameWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_BUDGET ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = _szFrameClass;

        if ( !RegisterClass( &wc ) )
            {
            return 0;
            }

        // register the child window classes
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = BudgetWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = cbWindowExtra;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_DOCUMENT ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( WHITE_BRUSH );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = _szBudgetClass;

        if ( !RegisterClass( &wc ) )
            {
            return 0;
            }

        // register the summary window classes
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = SummaryWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = cbWindowExtra;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_DOCUMENT ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( WHITE_BRUSH );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = _szSummaryClass;
        
        if ( !RegisterClass( &wc ) )
            {
            return 0;
            }

        if ( !BoxCtrlInit( ) )
            {
            return 0;
            }
        }

#ifdef DEBUG
    SetMemoryLimit( 16384 );
#endif

    GetWindowRect( GetDesktopWindow( ), &rc );
    x = ( rc.right - rc.left ) / 8;
    y = 0;
    dx = 3 * ( rc.right  - rc.left ) / 4;
    dy = 3 * ( rc.bottom - rc.top  ) / 4;

    ProfileInit( lpszCmdLine );
    _hmenu       = LoadMenu( _hInstance, MAKEINTRESOURCE( IDM_MENUMAIN ) );
    _hmenuWindow = GetSubMenu( _hmenu, nWindowMenu );
    _hwndFrame   = CreateWindow( _szFrameClass, NULL,
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                               x, y, dx, dy,
                               NULL, _hmenu,
                               _hInstance, NULL );

    // Ensure we have the right version of qobject
    // (revision number is not important)
    if ( _vQObject.GetMajorNumber( ) != 1 || 
         _vQObject.GetMinorNumber( ) != 4 ||
         _vQObject.GetRevisionNumber( ) != 2 )
        {
        MessageBox( _hwndFrame, "Budget is running the wrong version of QObject.", _szAppName, MB_OK );
        DestroyWindow( _hwndFrame );
        return 0;
        }

#ifdef DEBUG
    SetTraceLevel( TR_NORMAL );
    _nTraceIdm = IDM_TR_NORMAL;
    CheckMenuItem( _hmenu, _nTraceIdm, MF_BYCOMMAND | MF_CHECKED );
#endif

#ifdef TEST
    TRY
        {
        int i;
        BYTE *pb;

        QTest( );
        // Introduce a memory leak
        pb = new BYTE[ 10 ];
        // corrupt a memory object
        for ( i = 0; i <= 10; i++ )
            {
            *( pb + i ) = '#';
            }
        }
    CATCH_ALL
        {
        MessageBox( _hwndFrame, "An exception during testing has occurred.", _szAppName, MB_OK );
        }
    END_CATCH
#endif

    _hwndClient = GetWindow( _hwndFrame, GW_CHILD );
    SyncTitle( );

    ShowWindow( _hwndFrame, nCmdShow );
    UpdateWindow( _hwndFrame );

    

    _hAccel = LoadAccelerators( _hInstance, MAKEINTRESOURCE( IDA_BUDGET ) );
    while ( GetMessage( &msg, NULL, NULL, NULL ) )
        {
        if ( _hDlgModeless == 0 || !IsDialogMessage( _hDlgModeless, &msg ) )
            {
            if ( !TranslateMDISysAccel( _hwndClient, &msg ) &&
                 !TranslateAccelerator( _hwndFrame, _hAccel, &msg ) )
                {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
                }
            }
        }

    return ( msg.wParam );
    }

LONG FAR PASCAL _export FrameWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    CLIENTCREATESTRUCT clientcreate;
    LPDRAWITEMSTRUCT lpdis;
    int dx;
    int dy;
    RECT rc;
    int i;
    int j;
    FARPROC lpfn;
    HWND hwndChild = NULL;

    switch( msg )
        {
    case WM_CREATE:
        _pdoc = new QDocument;

        // create the client window
        clientcreate.hWindowMenu  = _hmenuWindow;
        clientcreate.idFirstChild = IDM_FIRSTCHILD;
        CreateWindow( _szClient, NULL,
                      WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
                      0, 0, 0, 0,
                      hwnd, 1, _hInstance, (LPSTR) &clientcreate );
        // create the owner-draw buttons
        for ( i = 0; i < nButtons; i++ )
            {
            int x = dxButton / 4;
            int y = dyButton / 4;
            for ( j = 0; j < 2; j++ )
                {
                _rgrghwndButton[ i ][ j ] = CreateWindow( "button", _szEmpty, 
                                WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                                x + i * dxButton, y, dxButton, dyButton, 
                                hwnd, i, _hInstance, NULL );

                _rgrghButton[ i ][ j ] = LoadBitmap( _hInstance, 
                                                     _rgszButton[ i ][ j ] );
                }
            }
        if ( !CreateStockObjects( hwnd ) )
            {
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        
        MenuController::updateMenus( );
        return 0L;

    case WM_SIZE:
        GetClientRect( hwnd, &rc );
        rc.top += dyToolBar;
        dx = rc.right  - rc.left;
        dy = rc.bottom - rc.top;
        MoveWindow( _hwndClient, rc.left, rc.top, dx, dy, TRUE );
        return 0L;

    case WM_DRAWITEM:
        lpdis = (LPDRAWITEMSTRUCT) lParam;

        if ( lpdis->itemAction & ODA_DRAWENTIRE )
            {
            DrawBitmap( lpdis->hDC, _rgrghButton[ wParam ][ 0 ], 0, 0 );
            }

        if ( lpdis->itemAction & ODA_SELECT )
            {
            if ( lpdis->itemState & ODS_SELECTED )
                {
                DrawBitmap( lpdis->hDC, _rgrghButton[ wParam ][ 1 ], 
                            0, 0 );
                }
            else
                {
                if ( FCursorInButton( lpdis->hwndItem ) )
                    {
                    /* perform the action here */
                    if ( GetMenuState( GetMenu( hwnd ), _rgidm[ wParam ],
                         MF_BYCOMMAND ) == MF_ENABLED )
                        {
                        PostMessage( hwnd, WM_COMMAND, _rgidm[ wParam ],
                                     0L );
                        }
                    else
                        {
                        if ( _rgidm[ wParam ] != IDM_NOOP )
                            MessageBeep( 0 );
                        SetFocus( _hwndFrame );
                        }
                    }
                else
                    {
                    SetFocus( _hwndFrame );
                    }
                DrawBitmap( lpdis->hDC, _rgrghButton[ wParam ][ 0 ], 0, 0 );
                }
            }
        return 0L;
    
    case WM_COMMAND:

        switch( wParam )
            {
        case IDM_NEW:
            if ( !SaveQuery( hwnd ) )
                QUIT;
                
            DestroyAllChildren( );
            _pdoc->Empty( );
            SyncTitle( );
            MenuController::updateMenus( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_MACRO:
            lpfn = MakeProcInstance( (FARPROC) MacroProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_MACROBOX ),
                       hwnd, lpfn );
            FreeProcInstance( lpfn );
            MenuController::updateMenus( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_OPEN:
            Open( hwnd );
            MenuController::updateMenus( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            return 0L;

        case IDM_SAVE:
            SaveDocument( hwnd );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_SAVEAS:
            SaveAs( hwnd );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_ABOUT:
            lpfn = MakeProcInstance( (FARPROC) AboutProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_ABOUTBOX ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CREATE:
            lpfn = MakeProcInstance( (FARPROC) CreateProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CREATEBOX ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            MenuController::updateMenus( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CALCULATOR:
            if ( ( hwndChild = FindWindow( NULL, "Calculator" ) ) )
                SetActiveWindow( hwndChild );
            else
                {
                hwndChild = WinExec( "CALC", SW_SHOW );
                if ( hwndChild < 32u )
                    {
                    MessageBox( _hwndFrame, "The Calculator could not be launched.", 
                                _szAppName, MB_ICONEXCLAMATION );
                    
                    hwndChild = NULL;
                    }
                }
            return 0L;

        case IDM_XFER:
            lpfn = MakeProcInstance( (FARPROC) XFerProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_XFERBOX ),
                       hwnd, lpfn );
            FreeProcInstance( lpfn );
            MenuController::updateMenus( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TRANSACTIONS:
            _hDlgModeless = CreateDialog( _hInstance, MAKEINTRESOURCE( IDD_TRANSACTIONSBOX ), 
                            _hwndFrame, (DLGPROC) TransactionsProc );
            return 0L;
        
        case IDM_BALANCE:
            lpfn = MakeProcInstance( (FARPROC) BalanceProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_BALANCEBOX ), 
                       hwnd, lpfn );
            FreeProcInstance( lpfn );
            MenuController::updateMenus( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_UNBALANCE:
            if ( MessageBox( hwnd, "Are you sure?", _szAppName, MB_ICONQUESTION | MB_YESNO ) == IDYES )
                {
                BOOL f = _pdoc->UnBalance( );
                ASSERT( f );
                UpdateAllViews( );
                _pdoc->SetModifiedFlag( );
                MenuController::updateMenus( );
                }
            return 0L;

        case IDM_DELETE:
            lpfn = MakeProcInstance( (FARPROC) DeleteProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_DELETEBOX ), 
                       hwnd, lpfn );
            FreeProcInstance( lpfn );
            MenuController::updateMenus( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TILE:
            SendMessage( _hwndClient, WM_MDITILE, MDITILE_VERTICAL, 0L );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CASCADE:
            SendMessage( _hwndClient, WM_MDICASCADE, 0, 0L );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_ARRANGE:
            SendMessage( _hwndClient, WM_MDIICONARRANGE, 0, 0L );
            SetFocus( _hwndFrame );
            return 0L;
        
#ifdef DEBUG
        case IDM_TR_OFF:
            SetTracing( wParam, TR_OFF );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TR_NORMAL:
            SetTracing( wParam, TR_NORMAL );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TR_INTENSE:
            SetTracing( wParam, TR_INTENSE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TR_ASSERTVALID:
            Trace( TR_NORMAL, "Checking data structures...\n" );
            _pdoc->AssertValid( );
            return 0L;

        case IDM_TR_WALKMEMORY:
            WalkMemoryTracking( );
            return 0L;

        case IDM_TR_CHECKMEMORY:
            if ( CheckMemory( ) )
                {
                MessageBox( hwnd, "No memory objects were found corrupt.", _szAppName, MB_OK );
                }
            else
                {
                MessageBox( hwnd, "Some memory objects were found corrupt.", _szAppName, MB_OK );
                }
            return 0L;

        case IDM_TR_MEMORYSTATS:
            {
            Trace( TR_NORMAL, "Memory Limit    : %lu.\n", GetMemoryLimit( ) );
            Trace( TR_NORMAL, "Memory Allocated: %lu.\n", GetMemoryAllocated( ) );
            }
            return 0L;
#endif
        case IDM_NEWWINDOW:
            lpfn = MakeProcInstance( (FARPROC) NewWindowProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_NEWWINDOWBOX ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
            }
        break; // pass unhandled commands to DefFrameProc( )

    case WM_CLOSE:
    case WM_QUERYENDSESSION:
        if ( SaveQuery( hwnd ) )
            {
            DestroyAllChildren( );
            DestroyWindow( hwnd );
            if ( msg == WM_QUERYENDSESSION )
                {
                return 1L;
                }
            }
        return 0L;

    case WM_DESTROY:
        for ( i = 0; i < nButtons; i++ )
            {
            for ( j = 0; j < 2; j++ )
                {
                DeleteObject( _rgrghButton[ i ][ j ] );
                }
            }
        delete _pdoc;
#ifdef DEBUG
        WalkMemoryTracking( );
        PurgeMemoryTracking( );
#endif
        DestroyStockObjects( );
        PostQuitMessage( 0 );
        return 0L;
        }
    return DefFrameProc( hwnd, _hwndClient, msg, wParam, lParam );
    }

#ifdef DEBUG
static VOID SetTracing( int nIdm, int nTraceLevel )
    {
    CheckMenuItem( _hmenu, _nTraceIdm, MF_BYCOMMAND | MF_UNCHECKED );
    _nTraceIdm = nIdm;
    CheckMenuItem( _hmenu, _nTraceIdm, MF_BYCOMMAND | MF_CHECKED ); 
    SetTraceLevel( nTraceLevel );
    }
#endif

LONG FAR PASCAL _export BudgetWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HDC hdc;
    PAINTSTRUCT ps;
    QBudgetView *pview = NULL;
    QAccount *pacc;
    LPCREATESTRUCT lpcreatestruct;
    LPMDICREATESTRUCT lpmdicreatestruct;
    
    if ( msg != WM_CREATE )
        {
#if ( FDATA == 1 )
        pview = (QBudgetView *) GetWindowLong( hwnd, GWL_VIEWPTR );
#else
        pview = (QBudgetView *) GetWindowWord( hwnd, GWW_VIEWPTR );
#endif
        }

    switch( msg )
        {
    case WM_CREATE:
        TASKLIST
            {
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
            SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
            lpcreatestruct = (LPCREATESTRUCT) lParam;
            lpmdicreatestruct = (LPMDICREATESTRUCT) lpcreatestruct->lpCreateParams;
#if ( FDATA == 1 )
            pacc = (QAccount *) lpmdicreatestruct->lParam;
#else
            pacc = (QAccount *) LOWORD( lpmdicreatestruct->lParam );
#endif
            hdc = GetDC( hwnd );
            int dyFontDevice = LoEnglishToDeviceY( hdc, _dyfont );
            ReleaseDC( hwnd, hdc );
            pview = new QBudgetView( hwnd, pacc->model( ), 2 * dyFontDevice, dyFontDevice );
            if ( !pview )
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hwnd );
                PostMessage( _hwndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L );
                QUIT;
                }
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) pview );
#else
            SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) pview );
#endif
            }
        ENDTASKLIST
        return 0L;

    case WM_DESTROY:
        delete pview;
#if ( FDATA == 1 )
        SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
        SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
        return 0L;

    case WM_SIZE:
        TRY
            {
            if ( pview )
                {
                pview->resize( );
                pview->bottom( );
                }
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hwnd );
            delete pview;
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
            SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
            PostMessage( _hwndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L );
            }
        END_CATCH
        break;

    case WM_KEYDOWN:
        switch( wParam )
            {
        case VK_UP:
        case VK_LEFT:
            SendMessage( hwnd, WM_VSCROLL, SB_LINEUP, 0L );
            break;
        case VK_PRIOR:
            SendMessage( hwnd, WM_VSCROLL, SB_PAGEUP, 0L );
            break;
        case VK_DOWN:
        case VK_RIGHT:
            SendMessage( hwnd, WM_VSCROLL, SB_LINEDOWN, 0L );
            break;
        case VK_NEXT:
            SendMessage( hwnd, WM_VSCROLL, SB_PAGEDOWN, 0L );
            break;
        case VK_HOME:
            pview->top( );
            break;
        case VK_END:
            pview->bottom( );
            break;
            }
        return 0L;

    case WM_VSCROLL:
        if ( pview )
            {
            switch( wParam )
                {
            case SB_THUMBPOSITION:
                pview->gotoPos( (int) LOWORD( lParam ) );
                break;
            case SB_LINEUP:
                pview->up( );
                break;
            case SB_PAGEUP:
                pview->pgUp( );
                break;
            case SB_LINEDOWN:
                pview->down( );
                break;
            case SB_PAGEDOWN:
                pview->pgDown( );
                break;
                }
            }
        return 0L;
        
    case WM_PAINT:
        hdc = BeginPaint( hwnd, &ps );
        if ( pview && !_fCriticalResources )
            {
            TRY
                {
                pview->draw( hdc, ps.rcPaint );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hwnd );
                delete pview;
#if ( FDATA == 1 )
                SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
                SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
                PostMessage( _hwndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L );
                }
            END_CATCH
            }
        EndPaint( hwnd, &ps );
        return 0L;
        }

    return DefMDIChildProc( hwnd, msg, wParam, lParam );
    }

BOOL FAR PASCAL _export CloseEnumProc( HWND hwnd, LONG lParam )
    {
    if ( GetWindow( hwnd, GW_OWNER ) )
        {
        return TRUE;
        }
    SendMessage( _hwndClient, WM_MDIRESTORE, (WPARAM) hwnd, 0L );
    SendMessage( _hwndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L );
    return TRUE;
    }

void DestroyAllChildren( )
    {
    FARPROC lpfn;
    lpfn = MakeProcInstance( (FARPROC) CloseEnumProc, _hInstance );
    EnumChildWindows( _hwndClient, lpfn, 0L );
    FreeProcInstance( lpfn );
    }

void UpdateAllViews( )
    {
    HWND hwndChild = GetWindow( _hwndClient, GW_CHILD );
    while ( hwndChild != NULL )
        {
#if ( FDATA == 1 )
        ScrollView *pview = (ScrollView *) GetWindowLong( hwndChild, GWL_VIEWPTR );
#else
        ScrollView *pview = (ScrollView *) GetWindowWord( hwndChild, GWW_VIEWPTR );
#endif
        ASSERT( pview != NULL );
        pview->resize( );
        pview->bottom( );
        InvalidateRect( hwndChild, NULL, TRUE );
        hwndChild = GetWindow( hwndChild, GW_HWNDNEXT );
        }
    }

BOOL FCursorInButton( HWND hwndButton )
    {
    RECT rcButton;
    POINT ptCursor;

    GetWindowRect( hwndButton, &rcButton );
    GetCursorPos( &ptCursor );
    return PtInRect( &rcButton, ptCursor );
    }

VOID DrawBitmap( HDC hdc, HBITMAP hBitmap, short xStart, short yStart )
    {
    BITMAP bm;
    HDC    hdcMem;
    POINT  ptSize,
           ptOrg;

    hdcMem = CreateCompatibleDC( hdc );
    SelectObject( hdcMem, hBitmap );
    SetMapMode( hdcMem, GetMapMode( hdc ) );

    GetObject( hBitmap, sizeof( BITMAP ), (LPSTR) &bm );
    ptSize.x = bm.bmWidth;
    ptSize.y = bm.bmHeight;
    DPtoLP( hdc, &ptSize, 1 );

    ptOrg.x = 0;
    ptOrg.y = 0;
    DPtoLP( hdcMem, &ptOrg, 1 );

    BitBlt( hdc, xStart, yStart, ptSize.x, 
            ptSize.y, hdcMem, ptOrg.x, ptOrg.y, SRCCOPY );

    DeleteDC( hdcMem );
    }

int LoEnglishToDeviceX( HDC hdc, int dxLoEnglish )
    {
    return dxLoEnglish * GetDeviceCaps( hdc, LOGPIXELSX ) / nLogUnitPerLogInch;
    }

int LoEnglishToDeviceY( HDC hdc, int dyLoEnglish )
    {
    return dyLoEnglish * GetDeviceCaps( hdc, LOGPIXELSY ) / nLogUnitPerLogInch;
    }

BOOL CreateStockObjects( HWND hwnd )
    {
    LOGFONT  lf;
    HDC hdc;

    BOOL fRet = FALSE;
    TASKLIST
        {
        lmemset( &lf, 0, sizeof( lf ) );
        _hpenDkGray  = NULL;
        _hpenLtGray  = NULL;
        _hpenGray    = NULL;
        _hfontNormal = NULL;
        _hfontBold   = NULL;
        _hbrFill     = NULL;

        _hpenDkGray = CreatePen( PS_SOLID, 0, clrDkGray );
        if ( !_hpenDkGray )
            QUIT;

        _hpenLtGray = CreatePen( PS_SOLID, 0, clrLtGray );
        if ( !_hpenLtGray )
            QUIT;

        _hpenGray = CreatePen( PS_SOLID, 0, clrGray );
        if ( !_hpenGray )
            QUIT;

        _hbrFill = CreateSolidBrush( clrVLtGray );
        if ( !_hbrFill )
            QUIT;

        hdc = GetDC( hwnd );
        lf.lfHeight = LoEnglishToDeviceY( hdc, _dyfont );
        lf.lfWeight = FW_NORMAL;
        ReleaseDC( hwnd, hdc );
        _hfontNormal = CreateFontIndirect( &lf );
        if ( !_hfontNormal )
            QUIT;

        hdc = GetDC( hwnd );
        lf.lfHeight = LoEnglishToDeviceY( hdc, _dyfont );
        lf.lfWeight = FW_BOLD;
        ReleaseDC( hwnd, hdc );
        _hfontBold = CreateFontIndirect( &lf );
        if ( !_hfontBold )
            QUIT;
        fRet = TRUE;
        }
    ENDTASKLIST
    if ( !fRet )
        {
        DestroyStockObjects( );
        }
    return fRet;
    }

void DestroyStockObjects( )
    {
    if ( _hpenDkGray )    
        DeleteObject( _hpenDkGray );
    if ( _hpenLtGray )
        DeleteObject( _hpenLtGray );
    if ( _hpenGray )
        DeleteObject( _hpenGray );
    if ( _hbrFill )
        DeleteObject( _hbrFill );
    if ( _hfontNormal )
        DeleteObject( _hfontNormal );
    if ( _hfontBold )
        DeleteObject( _hfontBold );
    }

void Open( HWND hwnd )
    {
    OPENFILENAME ofn;
    UINT ot;
    QArchive ar;
    CHAR szFile[ cbPathMax + 1 ];
    CHAR szFileTitle[ cbPathMax + 1 ];

    TRY
        {
        TASKLIST
            {
            if ( !SaveQuery( hwnd ) )
                {
                QUIT;
                }
            lmemset( &ofn, 0, sizeof( ofn ) );
            lmemset( szFile, 0, sizeof( szFile ) );
            ofn.hwndOwner = hwnd;
            ofn.lStructSize = sizeof( ofn );
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof( szFile );
            ofn.lpstrFileTitle = szFileTitle;
            ofn.nMaxFileTitle = sizeof( szFileTitle );
            ofn.lpstrFilter = _szFilter;
            if ( GetOpenFileName( &ofn ) )
                {
                if ( ar.Open( szFile, nModeRead ) )
                    {
                    DestroyAllChildren( );
                    _pdoc->Empty( );
                    ar.ReadHeader( &ot );
                    _pdoc->Retrieve( ar, ot );
                    ar.Close( );
                    _pdoc->SetFileName( szFileTitle );
                    SyncTitle( );
                    if ( _fOpenGeneral )
                        OpenWindow( _szGeneral );
                    }
                else
                    {
                    ResourcesMessageBox( hwnd );
                    }
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        ar.Close( );
        SyncTitle( );
        ResourcesMessageBox( hwnd );
        }
    END_CATCH
    }

BOOL Save( HWND hwnd )
    {
    BOOL fRet = FALSE;
    QArchive ar;
    TRY
        {
        if ( ar.Open( _pdoc->GetFileName( ), nModeWrite ) )
            {
            _pdoc->Store( ar );
            ar.Close( );
            _pdoc->ClearModifiedFlag( );
            fRet = TRUE;
            }
        else
            {
            MessageBox( hwnd, "The file could not be saved.", 
                        _szAppName, MB_ICONEXCLAMATION );
            }
        }
    CATCH( exFile )
        {
        MessageBox( hwnd, "A file exception occurred while storing the document", 
                    _szAppName, MB_ICONEXCLAMATION );
        ar.Close( );
        }
    CATCH_ALL
        {
        MessageBox( hwnd, "The file could not be saved.",
                    _szAppName, MB_ICONEXCLAMATION );
        ar.Close( );
        }
    END_CATCH
    return fRet;
    }

BOOL SaveAs( HWND hwnd )
    {
    BOOL fRet = FALSE;
    int  cb;
    CHAR szFile[ cbPathMax + 1 ];
    CHAR szFileTitle[ cbFileNameMax + 1 ];
    OPENFILENAME ofn;
    QArchive ar;

    lmemset( szFile, 0, sizeof( szFile ) );
    lmemset( szFileTitle, 0, sizeof( szFileTitle ) );
    lmemset( &ofn, 0, sizeof( ofn ) );
    TRY
        {
        ofn.hwndOwner   = hwnd;
        ofn.lStructSize = sizeof( ofn );
        ofn.lpstrFile = (LPSTR) szFile;
        ofn.nMaxFile = sizeof( szFile );

        if ( _pdoc->GetFileName( ) )
            {
            CHAR szDrive[ _MAX_DRIVE + 1 ];
            CHAR szDir  [ _MAX_DIR   + 1 ];
            CHAR szFName[ _MAX_FNAME + 1 ];
            CHAR szExt  [ _MAX_EXT   + 1 ];
            _splitpath( _pdoc->GetFileName( ), szDrive, szDir, szFName, szExt );
            cb = wsprintf( szFile, "%s%s", (LPSTR) szFName, (LPSTR) szExt );
            ASSERT( cb < sizeof( szFile ) );
            }
        ofn.lpstrFileTitle = (LPSTR) szFileTitle;
        ofn.nMaxFileTitle = sizeof( szFileTitle );
        ofn.lpstrFilter = (LPSTR) _szFilter;
        TASKLIST
            {
            if ( GetSaveFileName( &ofn ) )
                {
                if ( FileExists( szFile ) &&
                     MessageBox( hwnd, "Replace existing file?", _szAppName,
                     MB_OKCANCEL ) != IDOK )
                    {
                    QUIT;
                    }
                if ( ar.Open( szFile, nModeWrite ) )
                    {
                    _pdoc->SetFileName( szFileTitle );
                    _pdoc->Store( ar );
                    ar.Close( );
                    _pdoc->ClearModifiedFlag( );
                    SyncTitle( );
                    fRet = TRUE;
                    }
                else
                    {
                    MessageBox( hwnd, "The file could not be saved.",
                                _szAppName, MB_ICONEXCLAMATION );
                    }
                }
            }
        ENDTASKLIST
        }
    CATCH( exFile )
        {
        MessageBox( hwnd, "A file exception occurred while storing the document", 
                    _szAppName, MB_ICONEXCLAMATION );
        ar.Close( );
        SyncTitle( );
        }
    CATCH_ALL
        {
        MessageBox( hwnd, "The file could not be saved.",
                    _szAppName, MB_ICONEXCLAMATION );
        ar.Close( );
        SyncTitle( );
        }
    END_CATCH
    return fRet;
    }

BOOL SaveDocument( HWND hwnd )
    {
    BOOL fRet;
    if ( _pdoc->GetFileName( ) )
        {
        fRet = Save( hwnd );
        }
    else
        {
        fRet = SaveAs( hwnd );
        }
    return fRet;
    }

BOOL SaveQuery( HWND hwnd )
    {
    int  id;
    BOOL fRet = FALSE;

    TASKLIST
        {
        if ( !_pdoc->IsModified( ) )
            {
            fRet = TRUE;
            QUIT;
            }

        id = MessageBox( hwnd, "Save changes to current document?", _szAppName, 
                         MB_YESNOCANCEL | MB_ICONQUESTION );
        if ( id == IDYES )
            {
            TRY
                {
                fRet = SaveDocument( hwnd );
                }
            CATCH_ALL
                {
                }
            END_CATCH
            QUIT;
            }
        else if ( id == IDNO )
            {
            fRet = TRUE;
            QUIT;
            }
        // else, QUIT
        }
    ENDTASKLIST
    return fRet;
    }

void SyncTitle( )
    {
    UINT cb;
    if ( _pdoc->GetFileName( ) )
        {
        cb = wsprintf( _szTitle, _szTitleFormat, (LPSTR) _szAppName, (LPSTR) _pdoc->GetFileName( ) );
        }
    else
        {
        cb = wsprintf( _szTitle, _szTitleFormat, (LPSTR) _szAppName, (LPSTR) _szUntitled );
        }
    ASSERT( cb < sizeof( _szTitle ) );
    SetWindowText( _hwndFrame, _szTitle );
    }

void ResourcesMessageBox( HWND hwnd )
    {
    // This is done to prevent trying to repaint if we've already run
    // out of resources.  Must be made re-entrant.
    if ( !_fCriticalResources )
        {
        _fCriticalResources = TRUE;
        MessageBox( hwnd, "Budget has run out of memory resources.",
                    _szAppName, MB_ICONSTOP | MB_OK );
        _fCriticalResources = FALSE;
        }
    }

void OpenWindow( const CHAR *pszAccount )
    {
    MDICREATESTRUCT mdicreate;
    QAccount *pacc;

    ASSERT( *pszAccount != 0 );
    if ( lstrcmpi( pszAccount, _szGeneral ) == 0 )
        {
        pacc = _pdoc->GetGeneralAccount( );
        }
    else
        {
        pacc = _pdoc->FindAccount( pszAccount );
        }
    ASSERT( pacc != NULL );
    mdicreate.szClass = _szBudgetClass;
    mdicreate.szTitle = pszAccount;
    mdicreate.hOwner  = _hInstance;
    mdicreate.x       = CW_USEDEFAULT;
    mdicreate.y       = CW_USEDEFAULT;
    mdicreate.cx      = CW_USEDEFAULT;
    mdicreate.cy      = CW_USEDEFAULT;
    mdicreate.style   = WS_VSCROLL | WS_MAXIMIZE;
#if ( FDATA == 1 )
    mdicreate.lParam  = (LPARAM) pacc;
#else
    mdicreate.lParam  = (LPARAM) (WORD) pacc;
#endif
    SendMessage( _hwndClient, WM_MDICREATE, 0, 
                 (LONG)(LPMDICREATESTRUCT) &mdicreate );
    }

void OpenSummaryWindow( )
{
    MDICREATESTRUCT mdicreate;

    mdicreate.szClass = _szSummaryClass;
    mdicreate.szTitle = _szSummary;
    mdicreate.hOwner  = _hInstance;
    mdicreate.x       = CW_USEDEFAULT;
    mdicreate.y       = CW_USEDEFAULT;
    mdicreate.cx      = CW_USEDEFAULT;
    mdicreate.cy      = CW_USEDEFAULT;
    mdicreate.style   = WS_VSCROLL | WS_MAXIMIZE;
    mdicreate.lParam  = 0L;
    SendMessage( _hwndClient, WM_MDICREATE, 0, (LONG)(LPMDICREATESTRUCT) &mdicreate );
}

void ExecuteMacro( HWND hwnd, 
                   const CHAR  *pszMacroName, 
                   const CHAR  *pszMacroPath, 
                   const QTime &tDate,
                   const CHAR  *pszDesc )
    {
    FILE *pfile = NULL;

    TRY
        {
        pfile = fopen( pszMacroPath, "rt" );
        if ( !pfile )
            THROW_EXCEPTION( exFile );

        Program( pfile, pszMacroName, tDate, pszDesc );
        }
    CATCH( exFile )
        {
        MessageBox( hwnd, "A file error has occurred.",
                    _szAppName, MB_ICONEXCLAMATION );
        }
    CATCH( exParse )
        {
        MessageBox( hwnd, "A macro error has occurred", 
                    _szAppName, MB_ICONEXCLAMATION );
        }
    CATCH_ALL
        {
        MessageBox( hwnd, "An error has occurred",
                    _szAppName, MB_ICONEXCLAMATION );
        }
    END_CATCH
    if ( pfile )
        fclose( pfile );
    }

//////////////////////////////////////////////////////////////////////////////
// Program( )
//
// This routine implements a macro program.
//////////////////////////////////////////////////////////////////////////////
void Program( FILE *pfile, const CHAR *pszMacroName, const QTime &tDate, const CHAR *pszDesc )
    {
    CHAR szToken[ cbTokenMax + 1 ];

    while ( TRUE )
        {
        ReadToken( pfile, szToken );
        if ( !szToken[ 0 ] )
            break;
        Statement( pfile, pszMacroName, tDate, pszDesc, szToken );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// Statement( )
//
// This routine implements a macro statement.
//////////////////////////////////////////////////////////////////////////////
void Statement( FILE *pfile, const CHAR *pszMacroName, const QTime &tDate, const CHAR *pszDesc, CHAR *szToken )
    {
    QTransaction tx;
    QString      sDesc;
    QString      sAmount;
    
    Money monAmount;
    CHAR szAccountFrom[ cbAccountMax + 1 ];
    CHAR szAccountTo  [ cbAccountMax + 1 ];

    TRY
        {
        sDesc = "[";
        sDesc += pszMacroName;
        sDesc += "] ";
        sDesc += pszDesc;
        
        if ( lstrcmpi( szToken, "DEPOSIT" ) == 0 )
            {
            ReadToken( pfile, szToken );
            if ( !szToken[ 0 ] )
                THROW_EXCEPTION( exParse );
            monAmount = Money( szToken );
        
            ReadToken( pfile, szToken );
            if ( lstrcmpi( szToken, "TO" ) != 0 )
                THROW_EXCEPTION( exParse );

            ReadToken( pfile, szToken );
            if ( !szToken[ 0 ] )
                THROW_EXCEPTION( exParse );
            lstrcpy( szAccountTo, szToken );

            tx.SetDate( tDate );
            tx.SetDescription( sDesc );
            tx.SetAmount( monAmount );
            if ( !_pdoc->AddTransaction( szAccountTo, tx ) )
                THROW_EXCEPTION( exParse );
            _pdoc->SetModifiedFlag( );
            UpdateAllViews( );
            }
        else if ( lstrcmpi( szToken, "WITHDRAW" ) == 0 )
            {
            ReadToken( pfile, szToken );
            if ( !szToken[ 0 ] )
                THROW_EXCEPTION( exParse );
            
            // prepend the minus sign to token
            sAmount = QString( "-" ) + szToken;
            monAmount = Money( sAmount );
        
            ReadToken( pfile, szToken );
            if ( lstrcmpi( szToken, "FROM" ) != 0 )
                THROW_EXCEPTION( exParse );

            ReadToken( pfile, szToken );
            if ( !szToken[ 0 ] )
                THROW_EXCEPTION( exParse );
            lstrcpy( szAccountFrom, szToken );

            tx.SetDate( tDate );
            tx.SetDescription( sDesc );
            tx.SetAmount( monAmount );
            if ( !_pdoc->AddTransaction( szAccountFrom, tx ) )
                THROW_EXCEPTION( exParse );
            _pdoc->SetModifiedFlag( );
            UpdateAllViews( );
            }
        else if ( lstrcmpi( szToken, "TRANSFER" ) == 0 )
            {
            ReadToken( pfile, szToken );
            if ( !szToken[ 0 ] )
                THROW_EXCEPTION( exParse );
            monAmount = Money( szToken );
        
            ReadToken( pfile, szToken );
            if ( lstrcmpi( szToken, "FROM" ) != 0 )
                THROW_EXCEPTION( exParse );

            ReadToken( pfile, szToken );
            if ( !szToken[ 0 ] )
                THROW_EXCEPTION( exParse );
            lstrcpy( szAccountFrom, szToken );
            
            ReadToken( pfile, szToken );
            if ( lstrcmpi( szToken, "TO" ) != 0 )
                THROW_EXCEPTION( exParse );

            ReadToken( pfile, szToken );
            if ( !szToken[ 0 ] )
                THROW_EXCEPTION( exParse );
            lstrcpy( szAccountTo, szToken );

            if ( !_pdoc->TransferTransaction( szAccountFrom, szAccountTo, tDate, monAmount, sDesc ) )
                THROW_EXCEPTION( exParse );
            _pdoc->SetModifiedFlag( );
            UpdateAllViews( );
            }
        else
            {
            THROW_EXCEPTION( exParse );
            }
        }
    CATCH_ALL
        {
        tx.Cleanup( );
        sDesc.Cleanup( );
        sAmount.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

#define stWhiteSpace 0
#define stNormal     1
#define stComment    2
//////////////////////////////////////////////////////////////////////////////
// ReadToken( )
//
// This routine reads the next token.  Implemented
// as a state machine.
//
// End-of-file is detected when the returned token
// is the empty string.
//
// pfile   - file to read from
// szToken - array to place token - assumed to be of length
//           cbTokenMax + 1
//
// Returns:
//     void
//////////////////////////////////////////////////////////////////////////////
void ReadToken( FILE *pfile, CHAR *szToken ) 
    {
    BOOL fDone = FALSE;
    int st = stWhiteSpace;
    int ch;
    int cch = 0;

    lmemset( szToken, 0, cbTokenMax + 1 );

    while ( !fDone )
        {
        ch = fgetc( pfile );
        if ( ch == EOF )
            {
            if ( ferror( pfile ) )
                THROW_EXCEPTION( exFile );
            fDone = TRUE;
            break;
            }
        
        switch( st )
            {
        case stWhiteSpace:
            if ( ch == '#' )
                {
                st = stComment;
                }
            else if ( !isspace( ch ) ) 
                {
                if ( cch == cbTokenMax )
                    THROW_EXCEPTION( exParse );
                szToken[ cch ] = (CHAR) ch;
                cch++;
                st = stNormal;
                }
            // else, eat the character
            break;

        case stNormal:
            if ( ch == '#' )
                {
                st = stComment;
                }
            else if ( isspace( ch ) )
                {
                fDone = TRUE;
                }
            else
                {
                if ( cch == cbTokenMax )
                    THROW_EXCEPTION( exParse );
                szToken[ cch ] = (CHAR) ch;
                cch++;
                }

            break;

        case stComment:
            if ( ch == '\n' )
                {
                if ( szToken[ 0 ] )
                    {
                    // we've already started a token - treat
                    // \n as whitespace, ending the token
                    fDone = TRUE;
                    }
                else
                    {
                    // empty line, with a comment
                    st = stWhiteSpace;
                    }
                }
            break;
            }
        }
    }

LONG FAR PASCAL _export SummaryWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HDC hdc;
    PAINTSTRUCT ps;
    QSummaryView *pview = NULL;
    LPCREATESTRUCT lpcreatestruct;
    LPMDICREATESTRUCT lpmdicreatestruct;
    
    if ( msg != WM_CREATE )
        {
#if ( FDATA == 1 )
        pview = (QSummaryView *) GetWindowLong( hwnd, GWL_VIEWPTR );
#else
        pview = (QSummaryView *) GetWindowWord( hwnd, GWW_VIEWPTR );
#endif
        }

    switch( msg )
        {
    case WM_CREATE:
        TASKLIST
            {
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
            SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
            lpcreatestruct = (LPCREATESTRUCT) lParam;
            lpmdicreatestruct = (LPMDICREATESTRUCT) lpcreatestruct->lpCreateParams;
            
            hdc = GetDC( hwnd );
            int dyFontDevice = LoEnglishToDeviceY( hdc, _dyfont );
            ReleaseDC( hwnd, hdc );
            pview = new QSummaryView( hwnd, _pdoc->model( ), 2 * dyFontDevice, dyFontDevice );
            if ( !pview )
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hwnd );
                PostMessage( _hwndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L );
                QUIT;
                }
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) pview );
#else
            SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) pview );
#endif
            }
        ENDTASKLIST
        return 0L;

    case WM_DESTROY:
        delete pview;
#if ( FDATA == 1 )
        SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
        SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
        return 0L;

    case WM_SIZE:
        TRY
            {
            if ( pview )
                {
                pview->resize( );
                pview->bottom( );
                }
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hwnd );
            delete pview;
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
            SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
            PostMessage( _hwndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L );
            }
        END_CATCH
        break;

    case WM_KEYDOWN:
        switch( wParam )
            {
        case VK_UP:
        case VK_LEFT:
            SendMessage( hwnd, WM_VSCROLL, SB_LINEUP, 0L );
            break;
        case VK_PRIOR:
            SendMessage( hwnd, WM_VSCROLL, SB_PAGEUP, 0L );
            break;
        case VK_DOWN:
        case VK_RIGHT:
            SendMessage( hwnd, WM_VSCROLL, SB_LINEDOWN, 0L );
            break;
        case VK_NEXT:
            SendMessage( hwnd, WM_VSCROLL, SB_PAGEDOWN, 0L );
            break;
        case VK_HOME:
            pview->top( );
            break;
        case VK_END:
            pview->bottom( );
            break;
            }
        return 0L;

    case WM_VSCROLL:
        if ( pview )
            {
            switch( wParam )
                {
            case SB_THUMBPOSITION:
                pview->gotoPos( (int) LOWORD( lParam ) );
                break;
            case SB_LINEUP:
                pview->up( );
                break;
            case SB_PAGEUP:
                pview->pgUp( );
                break;
            case SB_LINEDOWN:
                pview->down( );
                break;
            case SB_PAGEDOWN:
                pview->pgDown( );
                break;
                }
            }
        return 0L;
        
    case WM_PAINT:
        hdc = BeginPaint( hwnd, &ps );
        if ( pview && !_fCriticalResources )
            {
            TRY
                {
                pview->draw( hdc, ps.rcPaint );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hwnd );
                delete pview;
#if ( FDATA == 1 )
                SetWindowLong( hwnd, GWL_VIEWPTR, (DWORD) 0 );
#else
                SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) 0 );
#endif
                PostMessage( _hwndClient, WM_MDIDESTROY, (WPARAM) hwnd, 0L );
                }
            END_CATCH
            }
        EndPaint( hwnd, &ps );
        return 0L;
        }

    return DefMDIChildProc( hwnd, msg, wParam, lParam );
    }

void MenuController::updateMenus( )
{
    UINT mf;

    mf = ( _pdoc->NumberOfAccounts( ) > 0 ? MF_ENABLED : MF_GRAYED );
    EnableMenuItem( _hmenu, IDM_XFER, mf );
    
    mf = ( _pdoc->getHighestSeq( ) > seqNil ? MF_ENABLED : MF_GRAYED );
    EnableMenuItem( _hmenu, IDM_DELETE, mf );
    EnableMenuItem( _hmenu, IDM_BALANCE, mf );

    mf = ( _pdoc->getHighestSeqBalance( ) > seqNil ? MF_ENABLED : MF_GRAYED );
    EnableMenuItem( _hmenu, IDM_UNBALANCE, mf );

    mf = ( ( !editTransactionsMode_ ) ? MF_ENABLED : MF_GRAYED );
    EnableMenuItem( _hmenu, IDM_NEW, mf );
    EnableMenuItem( _hmenu, IDM_OPEN, mf );
    EnableMenuItem( _hmenu, IDM_CREATE, mf );
    
    mf = ( ( !editTransactionsMode_ ) && _pdoc->NumberOfAccounts( ) > 0 ?
        MF_ENABLED : MF_GRAYED );
    EnableMenuItem( _hmenu, IDM_TRANSACTIONS, mf );
}

int Confirm( HWND hwnd, ConfirmStruct::Type t, long seq )
{
    ConfirmStruct cf( t, seq );

    FARPROC lpfn = MakeProcInstance( (FARPROC) ConfirmProc, _hInstance );
    int rc = DialogBoxParam( _hInstance, MAKEINTRESOURCE( IDD_CONFIRMBOX ), hwnd, lpfn, (LPARAM) &cf );
    FreeProcInstance( lpfn );
    return rc;
}
