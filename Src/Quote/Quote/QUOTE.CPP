//////////////////////////////////////////////////////////////////////////////
// QUOTE.CPP
//
// This file contains the main WndProc's,
// most of the UI code, and other helper functions.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <commdlg.h>
#include <stdlib.h>
#include <boxctrl.h>
#include <statpak.h>
#include "resource.hxx"
#include "ot.hxx"
#include "types.hxx"
#include "template.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "tools.hxx"
#include "dlg.hxx"
#include "view.hxx"
#include "textview.hxx"
#include "trdview.hxx"
#include "perfsum.hxx"
#include "rpt.hxx"
#include "perfeng.hxx"
#include "simeng.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////////
HANDLE _hInstance;                      // Data Instance
HANDLE _hAccel;                         // Accelerator table
HMENU  _hmenu;                          // Main Menu
HMENU  _hmenuWindow;                    // Window Menu
HPEN   _hpenGrids;                      // Grid lines

HWND   _hwndFrame;                      // Frame Window
HWND   _hwndClient;                     // Client Window

#ifdef DEBUG
int _nTraceIdm;                         // trace level IDM
#endif

#define WM_SAVETEXT WM_USER             // User-defined save text windows message

QTemplate *_ptpl = NULL;                // Template object
QDocument *_pdoc = NULL;                // Document object
QExternalIndicatorDoc *_pExtDoc = NULL; // External indicator object
QTradeList *_ptrlist = NULL;            // Trade List object
QTradeList *_ptrlistSim = NULL;         // Simulator trade list
NVPairList *_psummary;                  // Performance Summary
NVPairList *_psummarySim;               // Simulator Summary
QGCache gcache;                         // Global Cache object
const QVersion _vApp( 1, 11, 5 );       // Application version
CHAR *_pszCmdLine = NULL;               // Command Line

static CHAR _szPrinterBanner[ ] = "Printing Document...";

const USHORT nIdxWhite  = 124;
const USHORT nIdxLtGray = 93;
const USHORT nIdxGray   = 62;
const USHORT nIdxDkGray = 31;
const USHORT nIdxBlack  = 0;
const USHORT nIdxRed    = 4;
const USHORT nIdxBlue   = 100;
const USHORT nIdxPurple = 52;

COLORREF _rgclr[ nColors ] =            // The Color Table
    {
    RGB(   0,   0,   0 ), //0
    RGB(  64,   0,   0 ), //1
    RGB( 128,   0,   0 ), //2
    RGB( 192,   0,   0 ), //3
    RGB( 255,   0,   0 ), //4
    RGB(   0,  64,   0 ), //5
    RGB(  64,  64,   0 ), //6
    RGB( 128,  64,   0 ), //7
    RGB( 192,  64,   0 ), //8
    RGB( 255,  64,   0 ), //9
    RGB(   0, 128,   0 ), //10
    RGB(  64, 128,   0 ), //11
    RGB( 128, 128,   0 ), //12
    RGB( 192, 128,   0 ), //13
    RGB( 255, 128,   0 ), //14
    RGB(   0, 192,   0 ), //15
    RGB(  64, 192,   0 ), //16
    RGB( 128, 192,   0 ), //17
    RGB( 192, 192,   0 ), //18
    RGB( 255, 192,   0 ), //19
    RGB(   0, 255,   0 ), //20
    RGB(  64, 255,   0 ), //21
    RGB( 128, 255,   0 ), //22
    RGB( 192, 255,   0 ), //23
    RGB( 255, 255,   0 ), //24
    RGB(   0,   0,  64 ), //25
    RGB(  64,   0,  64 ), //26
    RGB( 128,   0,  64 ), //27
    RGB( 192,   0,  64 ), //28
    RGB( 255,   0,  64 ), //29
    RGB(   0,  64,  64 ), //30
    RGB(  64,  64,  64 ), //31
    RGB( 128,  64,  64 ), //32
    RGB( 192,  64,  64 ), //33
    RGB( 255,  64,  64 ), //34
    RGB(   0, 128,  64 ), //35
    RGB(  64, 128,  64 ), //36
    RGB( 128, 128,  64 ), //37
    RGB( 192, 128,  64 ), //38
    RGB( 255, 128,  64 ), //39
    RGB(   0, 192,  64 ), //40
    RGB(  64, 192,  64 ), //41
    RGB( 128, 192,  64 ), //42
    RGB( 192, 192,  64 ), //43
    RGB( 255, 192,  64 ), //44
    RGB(   0, 255,  64 ), //45
    RGB(  64, 255,  64 ), //46
    RGB( 128, 255,  64 ), //47
    RGB( 192, 255,  64 ), //48
    RGB( 255, 255,  64 ), //49
    RGB(   0,   0, 128 ), //50
    RGB(  64,   0, 128 ), //51
    RGB( 128,   0, 128 ), //52
    RGB( 192,   0, 128 ), //53
    RGB( 255,   0, 128 ), //54
    RGB(   0,  64, 128 ), //55
    RGB(  64,  64, 128 ), //56
    RGB( 128,  64, 128 ), //57
    RGB( 192,  64, 128 ), //58
    RGB( 255,  64, 128 ), //59
    RGB(   0, 128, 128 ), //60
    RGB(  64, 128, 128 ), //61
    RGB( 128, 128, 128 ), //62
    RGB( 192, 128, 128 ), //63
    RGB( 255, 128, 128 ), //64
    RGB(   0, 192, 128 ), //65
    RGB(  64, 192, 128 ), //66
    RGB( 128, 192, 128 ), //67
    RGB( 192, 192, 128 ), //68
    RGB( 255, 192, 128 ), //69
    RGB(   0, 255, 128 ), //70
    RGB(  64, 255, 128 ), //71
    RGB( 128, 255, 128 ), //72
    RGB( 192, 255, 128 ), //73
    RGB( 255, 255, 128 ), //74
    RGB(   0,   0, 192 ), //75
    RGB(  64,   0, 192 ), //76
    RGB( 128,   0, 192 ), //77
    RGB( 192,   0, 192 ), //78
    RGB( 255,   0, 192 ), //79
    RGB(   0,  64, 192 ), //80
    RGB(  64,  64, 192 ), //81
    RGB( 128,  64, 192 ), //82
    RGB( 192,  64, 192 ), //83
    RGB( 255,  64, 192 ), //84
    RGB(   0, 128, 192 ), //85
    RGB(  64, 128, 192 ), //86
    RGB( 128, 128, 192 ), //87
    RGB( 192, 128, 192 ), //88
    RGB( 255, 128, 192 ), //89
    RGB(   0, 192, 192 ), //90
    RGB(  64, 192, 192 ), //91
    RGB( 128, 192, 192 ), //92
    RGB( 192, 192, 192 ), //93
    RGB( 255, 192, 192 ), //94
    RGB(   0, 255, 192 ), //95
    RGB(  64, 255, 192 ), //96
    RGB( 128, 255, 192 ), //97
    RGB( 192, 255, 192 ), //98
    RGB( 255, 255, 192 ), //99
    RGB(   0,   0, 255 ), //100
    RGB(  64,   0, 255 ), //101
    RGB( 128,   0, 255 ), //102
    RGB( 192,   0, 255 ), //103
    RGB( 255,   0, 255 ), //104
    RGB(   0,  64, 255 ), //105
    RGB(  64,  64, 255 ), //106
    RGB( 128,  64, 255 ), //107
    RGB( 192,  64, 255 ), //108
    RGB( 255,  64, 255 ), //109
    RGB(   0, 128, 255 ), //110
    RGB(  64, 128, 255 ), //111
    RGB( 128, 128, 255 ), //112
    RGB( 192, 128, 255 ), //113
    RGB( 255, 128, 255 ), //114
    RGB(   0, 192, 255 ), //115
    RGB(  64, 192, 255 ), //116
    RGB( 128, 192, 255 ), //117
    RGB( 192, 192, 255 ), //118
    RGB( 255, 192, 255 ), //119
    RGB(   0, 255, 255 ), //120
    RGB(  64, 255, 255 ), //121
    RGB( 128, 255, 255 ), //122
    RGB( 192, 255, 255 ), //123
    RGB( 255, 255, 255 )  //124
    };

//////////////////////////////////////////////////////////////////////////////
// Declare constant strings.  Note most strings
// are in resource string table resource.
//////////////////////////////////////////////////////////////////////////////
static const CHAR _szClassFrame[] = "clsFrame";
const CHAR * const _pszFilter1 = "Quote Files (*.qt)\0*.qt\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
const CHAR * const _pszFilter2 = "Text Files (*.txt)\0*.txt\0Quote Files (*.qt)\0*.qt\0All Files (*.*)\0*.*\0";
const CHAR * const _pszFilter3 = "All Files (*.*)\0*.*\0Quote Files (*.qt)\0*.qt\0Text Files (*.txt)\0*.txt\0";
const CHAR * const _pszDefExt  = "txt";
const CHAR *_pszFilter = _pszFilter2;
const CHAR * const _pszFilterExport = "All Files (*.*)\0*.*\0";
#ifdef T_BG
static const UINT nBackground = 3;
#else
static const UINT nBackground = 50;
#endif

#ifdef DEBUG
static const int nLinkCountMax = 4;
#else
static const int nLinkCountMax = 8;
#endif
const CHAR *_pszFontName = "Courier New";

//////////////////////////////////////////////////////////////////////////////
// Static Function Declarations
//////////////////////////////////////////////////////////////////////////////
static void LoadTemplate( HWND hwnd );
static void LoadNext( HWND hwndParent, BOOL fNext );
static void StoreTemplate( HWND hwnd, BOOL fQuery = TRUE );
static BOOL Open( HWND hwnd );
static BOOL SaveDocument( HWND hwnd, BOOL fQuery = TRUE );
static BOOL Save( HWND hwnd, const CHAR *pszFileName, BOOL fQuery = TRUE );
static BOOL SaveAs( HWND hwnd, BOOL fQuery = TRUE );
static BOOL ExportTemplate( HWND hwnd, BOOL &fViewed );
static void DoReSize( HWND hwnd, QView *pview, BOOL fScrollToEnd );
static void BackgroundTask( );
static void QuoteBox( HWND hwnd );
static HDC  GetPrinterDC( void );
static BOOL PrintSetup( HWND hwnd );
static void PrintPage( QView *pview, int nPage, PrintStruct &ps );
static FILE *softOpen( const CHAR *szFileName, const CHAR *szMode, BOOL &fReadOnly );
static void AutoOpen( void );
static BOOL SaveTextWindow( HWND hwnd, BOOL &fViewed );
static LONG DefaultTextWndProc( 
    ScrollView *pview, BOOL fGotoBottomOnResize, HWND hwnd, UINT msg, UINT wParam, LONG lParam );
typedef void (FAR PASCAL *DEVMODEPROC)( HWND, HANDLE, LPSTR, LPSTR );
static const CHAR *GetVersionHeaderString( );
static const CHAR *GetDeveloperString( );
#define XOR_BYTE 0x96

//////////////////////////////////////////////////////////////////////////////
// Tool Bar Stuff 
//////////////////////////////////////////////////////////////////////////////
static WORD _rgidm[] = 
    {
    IDM_NEW,
    IDM_OPEN,
    IDM_SAVE,
    IDM_SAVETEMPLATE,
    IDM_PRINT,
    IDM_EXIT,
    IDM_NOOP,
    IDM_CREATE_IT,
    IDM_CUSTOM_IT,
    IDM_EXTERNAL_IT,
    IDM_CREATE_WL,
    IDM_CREATE_RULE,
    IDM_CREATE_COND,
    IDM_CREATE_CONST,
    IDM_NOOP,
    IDM_PERFORMANCE,
    IDM_TRADESIM,
    IDM_CHART_SCANNER,
    IDM_INDVAL,
    IDM_NEW_WINDOW,
    IDM_NEW_TEXTWINDOW,
    IDM_LOAD_PREV,
    IDM_LOAD_NEXT,
    };
const  USHORT _nButtons = ( sizeof( _rgidm ) / sizeof( _rgidm[ 0 ] ) );
const  USHORT _dxButton = 32;
const  USHORT _dyButton = 32;
const  USHORT _dyToolBar = _dyButton + 2 * ( _dyButton / 4 );
static HWND   _rgrghwndButton[ _nButtons ][ 2 ];
static HANDLE _rgrghButton[ _nButtons ][ 2 ];
static USHORT _rgrgwButton[ _nButtons ][ 2 ] =
    {
        { IDB_NEW,         IDB_NEW2 },
        { IDB_OPEN,        IDB_OPEN2 },
        { IDB_SAVE,        IDB_SAVE2 },
        { IDB_SAVETP,      IDB_SAVETP2 },
        { IDB_PRINT,       IDB_PRINT2 },
        { IDB_EXIT,        IDB_EXIT2 },
        { IDB_BLANK,       IDB_BLANK2 },
        { IDB_CIT,         IDB_CIT2 },
        { IDB_CSIT,        IDB_CSIT2 },
        { IDB_XIT,         IDB_XIT2 },
        { IDB_CWL,         IDB_CWL2 },
        { IDB_RULE,        IDB_RULE2 },
        { IDB_COND,        IDB_COND2 },
        { IDB_CONST,       IDB_CONST2 },
        { IDB_BLANK,       IDB_BLANK2 },
        { IDB_PERFORMANCE, IDB_PERFORMANCE2 },
        { IDB_SIM,         IDB_SIM2 },
        { IDB_SCANNER,     IDB_SCANNER2 },
        { IDB_INDVAL,      IDB_INDVAL2 },
        { IDB_NEW_WINDOW,  IDB_NEW_WINDOW2 },
        { IDB_NEW_TEXTWINDOW, IDB_NEW_TEXTWINDOW2 },
        { IDB_LP,          IDB_LP2 },
        { IDB_LN,          IDB_LN2 },
    };

//////////////////////////////////////////////////////////////////////////////
// WinMain( )
//
// Main window function.
//////////////////////////////////////////////////////////////////////////////
int FAR PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow )
    {
    RECT     rc;
    int      x, y, dx, dy;
    MSG      msg;
    WNDCLASS wc;

    // Default Exception Handler
    DEFAULT_EXCEPTION_BLOCK
        {
        Trace( TR_NORMAL, "Default exception handler called.\n" );
        return 0;
        }

    _hInstance = hInstance;
    ASSERT( lpszCmdLine != NULL );
    _pszCmdLine = new CHAR[ lstrlen( lpszCmdLine ) + 1 ];
    ASSERT( _pszCmdLine != NULL );
    lstrcpy( _pszCmdLine, lpszCmdLine );

    if ( !hPrevInstance )
        {
        lmemset( &wc, 0, sizeof( wc ) );
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = FrameWndProc;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_QUOTE ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = _szClassFrame;
        if ( !RegisterClass( &wc ) )
            {
            ASSERT( FALSE );
            return 0;
            }

        lmemset( &wc, 0, sizeof( wc ) );
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = ChildWndProc;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_QUOTE ) );
        wc.hCursor       = LoadCursor( NULL, IDC_CROSS );
        wc.hbrBackground = GetStockObject( WHITE_BRUSH );
        wc.cbWndExtra    = GWW_MAX;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = GetString( IDS_CLSCHILD );
        if ( !RegisterClass( &wc ) )
            {
            ASSERT( FALSE );
            return 0;
            }
        
        lmemset( &wc, 0, sizeof( wc ) );
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = ChildTextWndProc;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_QUOTE ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( WHITE_BRUSH );
        wc.cbWndExtra    = sizeof( QTextView * );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = GetString( IDS_CLSCHILDTEXT );
        if ( !RegisterClass( &wc ) )
            {
            ASSERT( FALSE );
            return 0;
            }
        
        lmemset( &wc, 0, sizeof( wc ) );
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = ChildTradeListWndProc;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_QUOTE ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( WHITE_BRUSH );
        wc.cbWndExtra    = sizeof( QTradeView * );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = GetString( IDS_CLSCHILDTRADELIST );
        if ( !RegisterClass( &wc ) )
            {
            ASSERT( FALSE );
            return 0;
            }
        
        lmemset( &wc, 0, sizeof( wc ) );
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = ChildPerfSumWndProc;
        wc.hInstance     = _hInstance;
        wc.hIcon         = LoadIcon( _hInstance, MAKEINTRESOURCE( IDI_QUOTE ) );
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( WHITE_BRUSH );
        wc.cbWndExtra    = sizeof( QPerformanceSummary * );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = GetString( IDS_CLSCHILDPERFSUM );
        if ( !RegisterClass( &wc ) )
            {
            ASSERT( FALSE );
            return 0;
            }
        }

    GetWindowRect( GetDesktopWindow( ), &rc );
    x = 0;
    y = 0;
    dx = rc.right - rc.left;
    dy = 3 * ( rc.bottom - rc.top  ) / 4;

    _hmenu       = LoadMenu( _hInstance, MAKEINTRESOURCE( IDM_MAIN ) );
    _hmenuWindow = GetSubMenu( _hmenu, _nWindowMenu );
    _hAccel      = LoadAccelerators( _hInstance, MAKEINTRESOURCE( IDA_QUOTE ) );
    _hwndFrame   = CreateWindow( _szClassFrame, NULL, 
                                 WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                 x, y, dx, dy,
                                 NULL, _hmenu, _hInstance, NULL );
    
    if ( _vQObject.GetMajorNumber( ) != 1 ||
         _vQObject.GetMinorNumber( ) != 4 ||
         _vQObject.GetRevisionNumber( ) != 4 )
        {
        MsgBox( _hwndFrame, IDS_VERSION );
        DestroyWindow( _hwndFrame );
        return 0;
        }

    UpdateName( );
    BoxCtrlInit( );
#ifdef DEBUG
#ifdef T_LM
        SetMemoryLimit( 512 );
#else
        SetMemoryLimit( 40960 ); // 40K
#endif // T_LM
        SetTraceLevel( TR_NORMAL );
        _nTraceIdm = IDM_TR_NORMAL;
        CheckMenuItem( _hmenu, _nTraceIdm, MF_BYCOMMAND | MF_CHECKED );
#endif // DEBUG

    ShowWindow( _hwndFrame, nCmdShow );
    UpdateWindow( _hwndFrame );
    
    // Message Pump
    while ( TRUE )
        {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
            {
            if ( msg.message == WM_QUIT )
                {
                break; // while
                }
            
            if ( !TranslateMDISysAccel( _hwndClient, &msg ) &&
                 !TranslateAccelerator( _hwndFrame, _hAccel, &msg ) )
                {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
                }
            }
        else
            {
            BackgroundTask( );
            }
        }
    return msg.wParam;
    }

//////////////////////////////////////////////////////////////////////////////
// LoEnglishToDeviceY
//
// Converts MM_LOENGLISH units to device units
//////////////////////////////////////////////////////////////////////////////
int LoEnglishToDeviceY( HDC hdc, int dyLoEnglish )
    {
    return dyLoEnglish * GetDeviceCaps( hdc, LOGPIXELSY ) / nLogUnitPerLogInch;
    }

//////////////////////////////////////////////////////////////////////////////
// ChildTradeListWndProc( )
//
// Window Proc for trade list view displays.
//////////////////////////////////////////////////////////////////////////////
LONG FAR PASCAL _export ChildTradeListWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    CHAR szTitle[ _cbBufferMax + 1 ];
    HDC hdc;
    HCURSOR hCursor;
    QTradeView *pview = NULL;
    int dyFont;
    
    // Only large memory model supported
    ASSERT( FDATA == 1 );
    if ( msg != WM_CREATE )
        pview = (QTradeView *) GetWindowLong( hwnd, 0 );

    switch( msg )
        {
    case WM_CREATE:
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        hdc = GetDC( hwnd );
        dyFont = LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) );
        ReleaseDC( hwnd, hdc );
        GetWindowText( hwnd, szTitle, sizeof( szTitle ) );
        if ( lstrcmp( szTitle, GetString( IDS_TITLETRADELIST ) ) == 0 )
            {
            pview = new QTradeView( TRUE, hwnd, _ptrlist, 2 * dyFont, dyFont,  
                ( _ptpl->m_genopts.GetTextGridLines( ) ? _hpenGrids : GetStockObject( NULL_PEN ) ) );
            }
        else
            {
            pview = new QTradeView( FALSE, hwnd, _ptrlistSim, 2 * dyFont, dyFont,  
                ( _ptpl->m_genopts.GetTextGridLines( ) ? _hpenGrids : GetStockObject( NULL_PEN ) ) );
            }
        SetWindowLong( hwnd, 0, (LONG) pview );
        if ( !pview )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        SetCursor( hCursor );
        return 0L;

    case WM_DESTROY:
        delete pview;
        return 0L;
        }

    return DefaultTextWndProc( pview, FALSE, hwnd, msg, wParam, lParam );
    }

//////////////////////////////////////////////////////////////////////////////
// ChildPerfSumWndProc( )
//
// Window Proc for performance summary window.
//////////////////////////////////////////////////////////////////////////////
LONG FAR PASCAL _export ChildPerfSumWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HDC hdc;
    HCURSOR hCursor;
    QPerformanceSummary *psumm = NULL;
    int dyFont;
    CHAR szTitle[ _cbBufferMax + 1 ];

    // Only large memory model supported
    ASSERT( FDATA == 1 );
    if ( msg != WM_CREATE )
        psumm = (QPerformanceSummary *) GetWindowLong( hwnd, 0 );

    switch( msg )
        {
    case WM_CREATE:
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        hdc = GetDC( hwnd );
        dyFont = LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) );
        ReleaseDC( hwnd, hdc );
        GetWindowText( hwnd, szTitle, sizeof( szTitle ) );
        if ( lstrcmp( szTitle, GetString( IDS_TITLEPERFSUM ) ) == 0 )
            {
            psumm = new QPerformanceSummary( hwnd, _psummary, 2 * dyFont, dyFont, 
                ( _ptpl->m_genopts.GetTextGridLines( ) ? _hpenGrids : GetStockObject( NULL_PEN ) ) );
            }
        else
            {
            psumm = new QPerformanceSummary( hwnd, _psummarySim, 2 * dyFont, dyFont, 
                ( _ptpl->m_genopts.GetTextGridLines( ) ? _hpenGrids : GetStockObject( NULL_PEN ) ) );
            }
        SetWindowLong( hwnd, 0, (LONG) psumm );
        if ( !psumm )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        SetCursor( hCursor );
        return 0L;

    case WM_DESTROY:
        delete psumm;
        return 0L;
        }
    
    return DefaultTextWndProc( psumm, FALSE, hwnd, msg, wParam, lParam );
    }

//////////////////////////////////////////////////////////////////////////////
// ChildTextWndProc( )
//
// Window Proc for text view displays.
//////////////////////////////////////////////////////////////////////////////
LONG FAR PASCAL _export ChildTextWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HDC hdc;
    HCURSOR hCursor;
    QTextView *pview = NULL;
    int dyFont;

    // Only large memory model supported
    ASSERT( FDATA == 1 );
    if ( msg != WM_CREATE )
        pview = (QTextView *) GetWindowLong( hwnd, 0 );

    switch( msg )
        {
    case WM_CREATE:
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        hdc = GetDC( hwnd );
        dyFont = LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) );
        ReleaseDC( hwnd, hdc );
        pview = new QTextView( hwnd, _pdoc->model( ), 2 * dyFont, dyFont, 
            ( _ptpl->m_genopts.GetTextGridLines( ) ? _hpenGrids : GetStockObject( NULL_PEN ) ) );
        SetWindowLong( hwnd, 0, (LONG) pview );
        if ( !pview )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        SetCursor( hCursor );
        return 0L;

    // Secret key combination
    case WM_KEYDOWN:
        if ( !pview )
            break;

        switch( wParam )
            {
        case VK_INSERT:
            // Document can't be read only
            if ( _pdoc->readOnly( ) )
                break;

            // Date must exist
            const QQt *pqqtFirst = pview->getFirst( );
            if ( !pqqtFirst )
                break;

            // Date must be Aug 22, 1977
            const QTime &d = pqqtFirst->GetDate( );
            if ( d.GetYear( ) != 1977 )
                break;

            if ( d.GetMonth( ) != 8 )
                break;

            if ( d.GetDay( ) != 22 )
                break;

            QString msg = GetVersionHeaderString( );
            msg += _vApp;
            msg += "\n";
            msg += GetDeveloperString( );
            MessageBox( hwnd, msg, GetString( IDS_APPNAME ), MB_OK );
            return 0L;
            }
        break;

    case WM_DESTROY:
        delete pview;
        return 0L;
        }

    return DefaultTextWndProc( pview, TRUE, hwnd, msg, wParam, lParam );
    }

//////////////////////////////////////////////////////////////////////////////
// GetVersionHeaderString( )
//
// Gets the (encoded) version header string
//////////////////////////////////////////////////////////////////////////////
const CHAR *GetVersionHeaderString( )
    {
    static CHAR ret[ 15 ];
    static int enc[ 15 ] =
        {
        199,
        227,
        249,
        226,
        243,
        182,
        192,
        243,
        228,
        229,
        255,
        249,
        248,
        182,
        150
        };

    for ( int i = 0; i < sizeof( ret ); i++ )
        ret[ i ] = (CHAR) enc[ i ] ^ XOR_BYTE;
    return ret;
    }

//////////////////////////////////////////////////////////////////////////////
// GetDeveloperString( )
//
// Gets the (encoded) developer string
//////////////////////////////////////////////////////////////////////////////
const CHAR *GetDeveloperString( )
    {
    static CHAR ret[ 50 ];
    static int enc[ 50 ] =
        {
        210,
        243,
        229,
        255,
        241,
        248,
        243,
        242,
        182,
        247,
        248,
        242,
        182,
        242,
        243,
        224,
        243,
        250,
        249,
        230,
        243,
        242,
        182,
        244,
        239,
        182,
        193,
        247,
        228,
        242,
        182,
        199,
        227,
        255,
        248,
        250,
        247,
        248,
        186,
        182,
        167,
        175,
        175,
        163,
        187,
        164,
        166,
        166,
        164,
        150
        };

    for ( int i = 0; i < sizeof( ret ); i++ )
        ret[ i ] = (CHAR) enc[ i ] ^ XOR_BYTE;
    return ret;
    }

//////////////////////////////////////////////////////////////////////////////
// DefaultTextWndProc( )
//
// Window Proc for text view displays.
//////////////////////////////////////////////////////////////////////////////
LONG DefaultTextWndProc( ScrollView *pview, BOOL fGotoBottomOnResize, HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    FILE *pfile = NULL;
    PAINTSTRUCT ps;
    HDC hdc;

    switch( msg )
        {
    case WM_SAVETEXT:
        ASSERT( pview != NULL );
        pfile = (FILE *) lParam;
        TRY
            {
            pview->saveText( pfile );
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        END_CATCH
        return 0L;

    case WM_SIZE:
        if ( !pview )
            break;
        TRY
            {
            pview->resize( );
            if ( fGotoBottomOnResize )
                pview->bottom( );
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        END_CATCH
        break;

    case WM_KEYDOWN:
        if ( !pview )
            return 0L;
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
        case VK_SPACE:
        case VK_RETURN:
            PostMessage( _hwndFrame, WM_COMMAND, IDM_LOAD_NEXT, 0L );
            break;
            }
        return 0L;
        
    case WM_VSCROLL:
        if ( !pview )
            return 0L;

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
        return 0L;

    case WM_PAINT:
        if ( !pview )
            break;

        hdc = BeginPaint( hwnd, &ps );
        TRY
            {
            pview->draw( hdc, ps.rcPaint );
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        END_CATCH
        EndPaint( hwnd, &ps );
        return 0L;
        }

    return DefMDIChildProc( hwnd, msg, wParam, lParam );
    }

//////////////////////////////////////////////////////////////////////////////
// ChildWndProc( )
//
// Window Proc for standard view displays.
//////////////////////////////////////////////////////////////////////////////
LONG FAR PASCAL _export ChildWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HDC      hdc;
    RECT     rc;
    RECT     rcDevice;
    POINT    pt;
    HBRUSH   hbr;
    HPEN     hpen;
    int      nIndex;
    int      nWidth;
    int      nHeight;
    int      xLeft;
    int      xRight;
    HWND     hwndBox;
    QView   *pview = NULL;
    CHAR     szName[ _cbNameMax + 1 ];
    LPCREATESTRUCT lpcs;
    HCURSOR  hCursor;
    BOOL     fRet;
    
    if ( msg != WM_CREATE )
        {
        pview = GetViewPtr( hwnd );
        }

    switch( msg )
        {
    case WM_CREATE:
        /////////////////////////
        // Create the box control
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        hwndBox = CreateWindow( GetString( IDS_BOXCTRL ), NULL, 
                      WS_CHILD  | WS_VISIBLE |
                      BOXS_NORMAL | BOXS_LEFT | BOXS_OUTSIDE,
                      0, 0, 0, 0, hwnd, 1, _hInstance, NULL );
        
        hdc = GetDC( hwndBox );
        SetBoxFontHeight( hwndBox, LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) ) );
        ReleaseDC( hwndBox, hdc );
        
        //////////////////
        // Create the view
        lpcs = (LPCREATESTRUCT) lParam;
        lstrcpy( szName, lpcs->lpszName );
        pview = new QView( szName );
        SetViewPtr( hwnd, pview );
        SetPqqt( hwnd, NULL );
#ifdef _STATE_TRACE
        Trace( TR_NORMAL, "State = stNoSync.\n" );
#endif
        SetState( hwnd, stNoSync );
        SetCount( hwnd, 0 );
        SetXPos( hwnd, xPosNull );

        ///////////////////////////////////////////////////////////
        // Handle out of memory after we set the window attributes!
        if ( !pview )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            return 0L;
            }

        //////////////////
        // Update the view
        TRY
            {
            QWindowLayout *pwl = GetWindowLayout( hwnd );
            ASSERT( pwl != NULL );
            pview->OnReBuild( pwl );
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        END_CATCH
        SetCursor( hCursor );
        return 0L;

    case WM_DESTROY:
        delete pview;
        return 0L;

    case WM_HSCROLL:
        ///////////////////////////////////
        // Can't use pview if out of memory
        if ( !pview )
            {
            break;
            }
        fRet = FALSE;
        switch( wParam )
            {
        case SB_THUMBPOSITION:
            fRet = pview->gotopos( hwnd, (int) LOWORD( lParam ) );
            break;

        case SB_PAGEUP:
            fRet = pview->pageleft( hwnd );
            break;

        case SB_PAGEDOWN:
            fRet = pview->pageright( hwnd );
            break;

        case SB_LINEUP:
            fRet = pview->left( hwnd );
            break;

        case SB_LINEDOWN:
            fRet = pview->right( hwnd );
            break;

        case SB_TOP:
            fRet = pview->home( hwnd );
            break;

        case SB_BOTTOM:
            fRet = pview->end( hwnd );
            break;
            }
        if ( fRet )
            {
            if ( _ptpl->m_genopts.Scaling( QGeneralOptions::eScreen ) == 
                 QGeneralOptions::eByPage )
                {
                DoReSize( hwnd, pview, FALSE );
                }
            GetCursorPos( &pt );
            ScreenToClient( hwnd, &pt );
            PostMessage( hwnd, WM_MOUSEMOVE, 0, MAKELPARAM( pt.x, pt.y ) );
            InvalidateRect( hwnd, NULL, FALSE );
            }
        return 0L;

    case WM_KEYDOWN:
        ///////////////////////////////////
        // Can't use pview if out of memory
        if ( !pview )
            {
            break;
            }
        switch( wParam )
            {
        case VK_PRIOR:
            SendMessage( hwnd, WM_HSCROLL, SB_PAGEUP, 0L );
            break;

        case VK_NEXT:
            SendMessage( hwnd, WM_HSCROLL, SB_PAGEDOWN, 0L );
            break;

        case VK_UP:
        case VK_LEFT:
            SendMessage( hwnd, WM_HSCROLL, SB_LINEUP, 0L );
            break;
        
        case VK_DOWN:
        case VK_RIGHT:
            SendMessage( hwnd, WM_HSCROLL, SB_LINEDOWN, 0L );
            break;

        case VK_HOME:
            SendMessage( hwnd, WM_HSCROLL, SB_TOP, 0L );
            break;

        case VK_END:
            SendMessage( hwnd, WM_HSCROLL, SB_BOTTOM, 0L );
            break;
            
        case VK_SPACE:
        case VK_RETURN:
            PostMessage( _hwndFrame, WM_COMMAND, IDM_LOAD_NEXT, 0L );
            break;
            }
        return 0L;

    case WM_SIZE:
        ///////////////////////////////////
        // Can't use pview if out of memory
        if ( !pview )
            {
            break;
            }

        //////////////////////
        // Move the Box window
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        hwndBox = GetWindow( hwnd, GW_CHILD );
        ASSERT( hwndBox != NULL );
        nWidth  = LOWORD( lParam );
        nHeight = HIWORD( lParam );
        MoveWindow( hwndBox, 0, nHeight - _dyBox, nWidth, _dyBox, TRUE );

        //////////////////
        // Update the view
        TRY
            {
            DoReSize( hwnd, pview, TRUE );
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        END_CATCH
        SetCursor( hCursor );
        break;

    case WM_PAINT:
        ///////////////////////////////////
        // Can't use pview if out of memory
        if ( !pview )
            {
            break;
            }
        
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        ////////////////////
        // Keep DB WIN happy
        GetUpdateRect( hwnd, &rc, TRUE );

        /////////////////////////
        // Get the device context
        hdc = GetDC( hwnd );
        ASSERT( hdc != NULL );
        SetMapMode( hdc, MM_LOENGLISH );
        GetClientRectSpecial( hwnd, &rc );
        rcDevice = rc;
        SetViewportOrg( hdc, 0, rc.bottom );
        DPtoLP( hdc, (LPPOINT) &rc, 2 );

        ////////////////////////////////
        // Draw the background rectangle
        SaveDC( hdc );
        nIndex = _ptpl->m_genopts.GetBackgroundColor( );
        hbr = CreateSolidBrush( _rgclr[ nIndex ] );
        ASSERT( hbr != NULL );
        hpen = CreatePen( PS_SOLID, 0, _rgclr[ nIndex ] );
        ASSERT( hpen != NULL );
        SelectObject( hdc, hbr );
        SelectObject( hdc, hpen );
        Rectangle( hdc, rc.left, rc.top, rc.right, rc.bottom );
        RestoreDC( hdc, -1 );
        DeleteObject( hbr );
        DeleteObject( hpen );

        /////////////////
        // Paint the view
        SaveDC( hdc );
        TRY
            {
            QWindowLayout *pwl = GetWindowLayout( hwnd );
            ASSERT( pwl != NULL );
            pview->OnDraw( pwl, hdc );
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            }
        END_CATCH
        RestoreDC( hdc, -1 );

        ///////////////////////
        // Handle the quote box
        if ( _ptpl->m_genopts.GetQuoteBox( ) )
            {
            hwndBox = GetWindow( hwnd, GW_CHILD );
            ASSERT( hwndBox != NULL );
            UpdateWindow( hwndBox );
            }

        ////////////////////////////
        // Handle the mouse tracking
        if ( _ptpl->m_genopts.GetTracker( ) )
            {
            SaveDC( hdc );
            SetROP2( hdc, R2_NOT );
            GetCursorPos( &pt );
            ScreenToClient( hwnd, &pt );
            if ( PtInRect( &rcDevice, pt ) )
                {
                DPtoLP( hdc, &pt, 1 );
                MoveTo( hdc, pt.x, rc.top );
                LineTo( hdc, pt.x, rc.bottom );
                SetXPos( hwnd, pt.x );
                }
            else
                {
                SetXPos( hwnd, xPosNull );
                }
            RestoreDC( hdc, -1 );
            }

        //////////
        // Cleanup
        ReleaseDC( hwnd, hdc );
        ValidateRect( hwnd, NULL );
        SetCursor( hCursor );
        return 0L;

    case WM_MOUSEMOVE:
        ///////////////////////////////////
        // Can't use pview if out of memory
        if ( !pview )
            {
            break;
            }
        
        hdc = GetDC( hwnd );
        ASSERT( hdc != NULL );
        SetMapMode( hdc, MM_LOENGLISH );
        GetClientRectSpecial( hwnd, &rc );
        SetViewportOrg( hdc, 0, rc.bottom );
        DPtoLP( hdc, (LPPOINT) &rc, 2 );

        pt = MAKEPOINT( lParam );
        DPtoLP( hdc, &pt, 1 );

        if ( _ptpl->m_genopts.GetTracker( ) )
            {
            SaveDC( hdc );
            EraseOldLine( hwnd );
            SetROP2( hdc, R2_NOT );
            MoveTo( hdc, pt.x, rc.top );
            LineTo( hdc, pt.x, rc.bottom );
            SetXPos( hwnd, pt.x );
            RestoreDC( hdc, -1 );
            }
        
        if ( _ptpl->m_genopts.GetQuoteBox( ) )
            {
            hwndBox = GetWindow( hwnd, GW_CHILD );
            ASSERT( hwndBox != NULL );
            SetWindowText( hwndBox, "" );
            InvalidateRect( hwndBox, NULL, TRUE );
            UpdateWindow( hwndBox );

            QWindowLayout *pwl = GetWindowLayout( hwnd );
            ASSERT( pwl != NULL );
            pview->GetLogicalBorders( pwl, &xLeft, &xRight );
            if ( pt.x >= xLeft && pt.x < xRight )
                {
                QWindowLayout *pwl = GetWindowLayout( hwnd );
                ASSERT( pwl != NULL );
                int dxIncr = (int) pwl->GetDxIncr( );
                int count = ( pt.x - xLeft ) / dxIncr;
                QQt *pqqt = pview->GetFirstPqqt( );
#ifdef _STATE_TRACE
                Trace( TR_NORMAL, "State = stReStart.\n" );
#endif
                SetState( hwnd, stReStart );
                SetCount( hwnd, count );
                SetPqqt ( hwnd, pqqt );
                SetXPos ( hwnd, pt.x );
                }
            else
                {
#ifdef _STATE_TRACE
                Trace( TR_NORMAL, "State = stNoSync.\n" );
#endif
                SetState( hwnd, stNoSync );
                }
            }
        else
            {
#ifdef _STATE_TRACE
            Trace( TR_NORMAL, "State = stNoSync.\n" );
#endif
            SetState( hwnd, stNoSync );
            }

        ReleaseDC( hwnd, hdc );
        return 0L;
        }
    return DefMDIChildProc( hwnd, msg, wParam, lParam );
    }

//////////////////////////////////////////////////////////////////////////////
// FrameWndProc( )
//
// Window Proc for the frame.
//////////////////////////////////////////////////////////////////////////////
LONG FAR PASCAL _export FrameWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HCURSOR hCursor = NULL;
    LPDRAWITEMSTRUCT   lpdis;
    CLIENTCREATESTRUCT clientcreate;
    RECT    rc;
    USHORT  dx, dy;
    USHORT  x, y;
    USHORT  i, j;
    BOOL    fCheck;
    BOOL    fEnd;
    FARPROC lpfn;
    HDC     hdc;
    PAINTSTRUCT ps;
#ifdef DEBUG
    FARPROC lpfnEnum;
#endif
    BOOL fViewed = FALSE;

    switch( msg )
        {
    case WM_CREATE:
        clientcreate.hWindowMenu  = _hmenuWindow;
        clientcreate.idFirstChild = IDM_FIRST_CHILD;

        _hwndClient = CreateWindow( GetString( IDS_MDICLIENT ), NULL, 
                                    WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
                                    0, 0, 0, 0,
                                    hwnd, 1, _hInstance, (LPSTR) &clientcreate );
        
        // Create the toolbar buttons
        x = _dxButton / 4;
        y = _dyButton / 4;
        for ( i = 0; i < _nButtons; i++ )
            {
            for ( j = 0; j < 2; j++ )
                {
                _rgrghwndButton[ i ][ j ] = CreateWindow( "button", GetString( IDS_EMPTY ), 
                                WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                                x + i * _dxButton, y, _dxButton, _dyButton, 
                                hwnd, i, _hInstance, NULL );

                _rgrghButton[ i ][ j ] = LoadBitmap( _hInstance,
                    MAKEINTRESOURCE( _rgrgwButton[ i ][ j ] ) );
                                                     
                }
            }
        _ptpl = new QTemplate( _pszCmdLine );
        ASSERT( _ptpl != NULL );
        _pdoc = new QDocument;
        ASSERT( _pdoc != NULL );
        _ptrlist = new QTradeList;
        ASSERT( _ptrlist != NULL );
        _ptrlistSim = new QTradeList;
        ASSERT( _ptrlistSim != NULL );
        _psummary = new NVPairList;
        ASSERT( _psummary != NULL );
        _psummarySim = new NVPairList;
        ASSERT( _psummarySim != NULL );
        _pExtDoc = new QExternalIndicatorDoc;
        ASSERT( _pExtDoc != NULL );
        
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        TRY
            {
            LoadTemplate( hwnd );
            }
        CATCH( exMemory )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            MsgBox( hwnd, IDS_MEMORY );
            delete _ptpl;
            _ptpl = new QTemplate( _pszCmdLine );
            ASSERT( _ptpl != NULL );
            }
        CATCH( exFileCorrupt )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            MsgBox( hwnd, IDS_CORRUPTTEMPLATE );
            delete _ptpl;
            _ptpl = new QTemplate( _pszCmdLine );
            ASSERT( _ptpl != NULL );
            }
        CATCH_ALL
            {
            SetCursor( hCursor );
            hCursor = NULL;
            MsgBox( hwnd, IDS_GENERALERROR );
            delete _ptpl;
            _ptpl = new QTemplate( _pszCmdLine );
            ASSERT( _ptpl != NULL );
            }
        END_CATCH
        if ( hCursor )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            }

        // Load the external indicator file
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        TRY
            {
            const CHAR *pch = _ptpl->m_genopts.GetExternalFileName( );
            if ( pch && *pch )
                _pExtDoc->RetrieveText( pch );
            }
        CATCH( exFileCorrupt )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            MsgBox( hwnd, IDS_EXTINDFILE_CORRUPT );
            _pExtDoc->Cleanup( );
            }
        CATCH( exMemory )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            MsgBox( hwnd, IDS_MEMORY );
            _pExtDoc->Cleanup( );
            }
        CATCH_ALL
            {
            SetCursor( hCursor );
            hCursor = NULL;
            MsgBox( hwnd, IDS_EXTINDFILE_ERROR );
            _pExtDoc->Cleanup( );
            }
        END_CATCH
        if ( hCursor )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            }

        UpdateMenu( );
        // Light gray
        _hpenGrids   = CreatePen( PS_SOLID, 0, RGB( 0xC0, 0xC0, 0xC0 ) );
        ASSERT( _hpenGrids );
        return 0L;

    case WM_QUERYENDSESSION:
        fEnd = FALSE;
        TRY
            {
            fEnd = SaveQuery( hwnd );
            if ( fEnd )
                {
                // store template if we are ending Windows session
                StoreTemplate( hwnd );
                }
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            }
        END_CATCH
        if ( fEnd )
            {
            DestroyAllChildren( );
            DestroyWindow( hwnd );
            return 1L;
            }
        return 0L;

    case WM_CLOSE:
        TRY
            {
            SaveQuery( hwnd );
            StoreTemplate( hwnd );
            }
        CATCH( exMemory )
            {
            MsgBox( hwnd, IDS_MEMORY );
            }
        CATCH_ALL
            {
            MsgBox( hwnd, IDS_GENERALERROR );
            }
        END_CATCH
        DestroyAllChildren( );
        DestroyWindow( hwnd );
        return 0L;

    case WM_DESTROY:
        delete _ptpl;
        _ptpl = NULL;
        delete _pdoc;
        _pdoc = NULL;
        delete _ptrlist;
        _ptrlist = NULL;
        delete _ptrlistSim;
        _ptrlistSim = NULL;
        delete _psummary;
        _psummary = NULL;
        delete _psummarySim;
        _psummarySim = NULL;
        delete _pExtDoc;
        _pExtDoc = NULL;
        for ( i = 0; i < _nButtons; i++ )
            {
            for ( j = 0; j < 2; j++ )
                {
                DeleteObject( _rgrghButton[ i ][ j ] );
                }
            }
        delete [] _pszCmdLine;
#ifdef DEBUG
        WalkMemoryTracking( );
        PurgeMemoryTracking( );
#endif
        DeleteObject( _hpenGrids );
        PostQuitMessage( 0 );
        return 0L;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDM_NEW:
            TRY
                {
                if ( SaveQuery( hwnd ) )
                    {
                    DestroyAllChildren( );
                    _pdoc->Cleanup( );
                    UpdateName( );
                    }
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            UpdateMenu( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_OPEN:
            TRY
                {
                Open( hwnd );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH( exFileCorrupt )
                {
                MsgBox( hwnd, IDS_CORRUPTDOCUMENT );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            UpdateMenu( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_SAVE:
            TRY
                {
                SaveDocument( hwnd, FALSE );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_SAVEAS:
            TRY
                {
                SaveAs( hwnd );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_SAVE_TEXT:
            TRY
                {
                SaveTextWindow( hwnd, fViewed );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            if ( !fViewed )
                SetFocus( _hwndFrame );
            return 0L;

        case IDM_EXPORT_TEMPLATE:
            TRY
                {
                ExportTemplate( hwnd, fViewed );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            if ( !fViewed )
                SetFocus( _hwndFrame );
            return 0L;

        case IDM_SAVETEMPLATE:
            TRY
                {
                StoreTemplate( hwnd, FALSE );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_PRINT_SETUP:
            PrintSetup( hwnd );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_PRINT:
            lpfn = MakeProcInstance( (FARPROC) PrintProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_PRINT ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_EXIT:
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            return 0L;

        case IDM_LOAD_PREV:
        case IDM_LOAD_NEXT:
            TRY
                {
                LoadNext( hwnd, wParam == IDM_LOAD_NEXT );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_INDVAL:
            lpfn = MakeProcInstance( (FARPROC) IndValProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_INDICATOR_VALUE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
            
        case IDM_CREATE_RULE:
            lpfn = MakeProcInstance( (FARPROC) CreateRuleProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CREATE_RULE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_DELETE_RULE:
            lpfn = MakeProcInstance( (FARPROC) DeleteRuleProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_DELETE_RULE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_PERFORMANCE:
            lpfn = MakeProcInstance( (FARPROC) PerformanceProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_PERFORMANCE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TRADESIM:
            lpfn = MakeProcInstance( (FARPROC) SimulatorProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_TRADESIM ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CHART_SCANNER:
            lpfn = MakeProcInstance( (FARPROC) ScannerProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CHART_SCANNER ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_ADD_QUOTE:
            lpfn = MakeProcInstance( (FARPROC) AddQuoteProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_ADD_QUOTE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
            
        case IDM_DELETE_QUOTE:
            lpfn = MakeProcInstance( (FARPROC) DeleteQuoteProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_DELETE_QUOTE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_FONT_SIZE:
            lpfn = MakeProcInstance( (FARPROC) FontSizeProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_FONT_SIZE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
        
        case IDM_EXTERNAL_INDICATOR_FILE:
            lpfn = MakeProcInstance( (FARPROC) ExternalIndicatorFileProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_EXTERNAL_INDICATOR_FILE ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
        
        case IDM_BROWSER:
            lpfn = MakeProcInstance( (FARPROC) BrowserProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_BROWSER ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
            
        case IDM_TEXT_FORMAT:
            lpfn = MakeProcInstance( (FARPROC) TextFormatProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_TEXT_FORMAT ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CREATE_IT:
            lpfn = MakeProcInstance( (FARPROC) CreateITProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CREATE_IT ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
            
        case IDM_CUSTOM_IT:
            lpfn = MakeProcInstance( (FARPROC) CustomITProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CUSTOM_IT ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_EXTERNAL_IT:
            lpfn = MakeProcInstance( (FARPROC) ExternalITProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_EXTERNAL_IT ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;
        
        case IDM_DELETE_IT:
            lpfn = MakeProcInstance( (FARPROC) DeleteITProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_DELETE_IT ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CREATE_COND:
            lpfn = MakeProcInstance( (FARPROC) CreateCondProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CREATE_COND ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_DELETE_COND:
            lpfn = MakeProcInstance( (FARPROC) DeleteCondProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_DELETE_COND ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CREATE_CONST:
            lpfn = MakeProcInstance( (FARPROC) CreateConstProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CREATE_CONST ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_DELETE_CONST:
            lpfn = MakeProcInstance( (FARPROC) DeleteConstProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_DELETE_CONST ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CREATE_WL:
            lpfn = MakeProcInstance( (FARPROC) CreateWLProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_CREATE_WL ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_DELETE_WL:
            lpfn = MakeProcInstance( (FARPROC) DeleteWLProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_DELETE_WL ), hwnd, lpfn );
            FreeProcInstance( lpfn );
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

        case IDM_ARRANGE_ICONS:
            SendMessage( _hwndClient, WM_MDIICONARRANGE, 0, 0L );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_CLEAR_PERF_DATA:
            if ( MessageBox( hwnd, GetString( IDS_USERCONFIRM ), "User Confirmation", MB_YESNO ) == IDYES )
                {
                DestroyAllChildren( );
                _ptrlist->Cleanup( );
                _ptrlistSim->Cleanup( );
                _psummary->Cleanup( );
                _psummarySim->Cleanup( );
                UpdateMenu( );
                }
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_NEW_TRADELIST:
        case IDM_NEW_STRADELIST:
            TRY
                {
                OpenTradeListWindow( TRUE, ( wParam == IDM_NEW_TRADELIST ) );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            SetFocus( _hwndFrame );
            return 0L;
        
        case IDM_NEW_SIMRESULTS:
        case IDM_NEW_PERFSUM:
            TRY
                {
                OpenPerfSumWindow( TRUE, ( wParam == IDM_NEW_PERFSUM ) );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_NEW_TEXTWINDOW:
            TRY
                {
                OpenTextWindow( TRUE );
                }
            CATCH( exMemory )
                {
                MsgBox( hwnd, IDS_MEMORY );
                }
            CATCH_ALL
                {
                MsgBox( hwnd, IDS_GENERALERROR );
                }
            END_CATCH
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_NEW_WINDOW:
            lpfn = MakeProcInstance( (FARPROC) NewWindowProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_NEW_WINDOW ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_COLOR:
            lpfn = MakeProcInstance( (FARPROC) ColorProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_COLOR ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            UpdateAllViews( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_LINE_WIDTH:
            lpfn = MakeProcInstance( (FARPROC) LineWidthProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_LINE_WIDTH ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_ABOUT:
            lpfn = MakeProcInstance( (FARPROC) AboutProc, _hInstance );
            DialogBox( _hInstance, MAKEINTRESOURCE( IDD_ABOUT ), hwnd, lpfn );
            FreeProcInstance( lpfn );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_QUOTE_BOX:
            fCheck = GetMenuState( _hmenu, IDM_QUOTE_BOX, MF_BYCOMMAND ) & MF_CHECKED;
            _ptpl->m_genopts.SetQuoteBox( !fCheck );
            CheckMenuItem( _hmenu, IDM_QUOTE_BOX, MF_BYCOMMAND | ( fCheck ? MF_UNCHECKED : MF_CHECKED ) );
            _ptpl->SetModifiedFlag( TRUE );
            UpdateAllViews( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TRACKER:
            fCheck = GetMenuState( _hmenu, IDM_TRACKER, MF_BYCOMMAND ) & MF_CHECKED;
            _ptpl->m_genopts.SetTracker( !fCheck );
            CheckMenuItem( _hmenu, IDM_TRACKER, MF_BYCOMMAND | ( fCheck ? MF_UNCHECKED : MF_CHECKED ) );
            _ptpl->SetModifiedFlag( TRUE );
            UpdateAllViews( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_AUTOOPENTEXT:
            fCheck = GetMenuState( _hmenu, IDM_AUTOOPENTEXT, MF_BYCOMMAND ) & MF_CHECKED;
            _ptpl->m_genopts.SetAutoOpenText( !fCheck );
            CheckMenuItem( _hmenu, IDM_AUTOOPENTEXT, MF_BYCOMMAND | ( fCheck ? MF_UNCHECKED : MF_CHECKED ) );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TEXT_GRID_LINES:
            fCheck = GetMenuState( _hmenu, IDM_TEXT_GRID_LINES, MF_BYCOMMAND ) & MF_CHECKED;
            _ptpl->m_genopts.SetTextGridLines( !fCheck );
            CheckMenuItem( _hmenu, IDM_TEXT_GRID_LINES, MF_BYCOMMAND | ( fCheck ? MF_UNCHECKED : MF_CHECKED ) );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;
        
        case IDM_DATES_NONE:
            _ptpl->m_genopts.SetDateType( dtNone );
            UpdateMenu( );
            UpdateAllViews( );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_DATES_MONTHS:
            _ptpl->m_genopts.SetDateType( dtMonths );
            UpdateMenu( );
            UpdateAllViews( );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_DATES_YEARS:
            _ptpl->m_genopts.SetDateType( dtYears );
            UpdateMenu( );
            UpdateAllViews( );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_SCR_PAGE:
            _ptpl->m_genopts.Scaling( QGeneralOptions::eScreen, QGeneralOptions::eByPage );
            UpdateMenu( );
            UpdateAllViews( TRUE );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_SCR_CHART:
            _ptpl->m_genopts.Scaling( QGeneralOptions::eScreen, QGeneralOptions::eByChart );
            UpdateMenu( );
            UpdateAllViews( TRUE );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_PRT_PAGE:
            _ptpl->m_genopts.Scaling( QGeneralOptions::ePrint, QGeneralOptions::eByPage );
            UpdateMenu( );
            UpdateAllViews( TRUE );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_PRT_CHART:
            _ptpl->m_genopts.Scaling( QGeneralOptions::ePrint, QGeneralOptions::eByChart );
            UpdateMenu( );
            UpdateAllViews( TRUE );
            _ptpl->SetModifiedFlag( TRUE );
            SetFocus( _hwndFrame );
            return 0L;

#ifdef DEBUG
        case IDM_CLEAR_CACHE:
            gcache.Cleanup( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_DUMP_CACHE:
            gcache.DumpContents( );
            SetFocus( _hwndFrame );
            return 0L;
            
        case IDM_TR_COUNTQQTS:
            Trace( TR_NORMAL, "Number of qqt's = %u.\n", _pdoc->GetCount( ) );
            SetFocus( _hwndFrame );
            return 0L;

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
            Trace( TR_NORMAL, "Checking Data Structures...\n" );
            _ptpl->AssertValid( );
            _pdoc->AssertValid( );
            _pExtDoc->AssertValid( );
            _ptrlist->AssertValid( );
            _ptrlistSim->AssertValid( );
            _psummary->AssertValid( );
            _psummarySim->AssertValid( );
            gcache.AssertValid( );
            lpfnEnum = MakeProcInstance( (FARPROC) AssertEnumProc, _hInstance );
            EnumChildWindows( _hwndClient, lpfnEnum, 0L );
            FreeProcInstance( lpfnEnum );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TR_WALKMEMORY:
            WalkMemoryTracking( );
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TR_CHECKMEMORY:
            if ( CheckMemory( ) )
                {
                MsgBox( hwnd, IDS_NOTCORRUPT, MB_OK );
                }
            else
                {
                MsgBox( hwnd, IDS_CORRUPT, MB_OK );
                }
            SetFocus( _hwndFrame );
            return 0L;

        case IDM_TR_MEMORYSTATS:
            Trace( TR_NORMAL, "Memory Limit     : %lu.\n", GetMemoryLimit( ) );
            Trace( TR_NORMAL, "Memory Allocated : %lu.\n", GetMemoryAllocated( ) );
            SetFocus( _hwndFrame );
            return 0L;
#endif
            }
        // Call DefFrameProc( )
        break;

    case WM_SIZE:
        GetClientRect( hwnd, &rc );
        rc.top += _dyToolBar;
        dx = rc.right  - rc.left;
        dy = rc.bottom - rc.top;
        MoveWindow( _hwndClient, rc.left, rc.top, dx, dy, TRUE );
        return 0L;

    case WM_PAINT:
        hdc = BeginPaint( hwnd, &ps );
        GetClientRect( hwnd, &rc );
        SelectObject( hdc, GetStockObject( BLACK_PEN ) );
        MoveTo( hdc, rc.left, rc.top + _dyToolBar - 1 );
        LineTo( hdc, rc.right, rc.top + _dyToolBar - 1 );
        EndPaint( hwnd, &ps );
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
                            {
                            MessageBeep( 0 );
                            }
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
        }
    return DefFrameProc( hwnd, _hwndClient, msg, wParam, lParam );
    }

//////////////////////////////////////////////////////////////////////////////
// StoreTemplate( )
//
// This routine serializes the template file to disk.
//////////////////////////////////////////////////////////////////////////////
static void StoreTemplate( HWND hwnd, BOOL fQuery )
    {
    QArchive ar;
    BOOL     f;
    HCURSOR  hCursor = NULL;

    TRY
        {
        if ( !fQuery || ( _ptpl->GetModifiedFlag( ) && MsgBox( hwnd, IDS_QUERYSAVETEMPLATE, MB_YESNO ) == IDYES ) )
            {
            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            f = ar.Open( _ptpl->GetFileName( ), nModeWrite );
            if ( !f )
                {
                THROW_EXCEPTION( exFile );
                }
            _ptpl->Store( ar );
            ar.Close( );
            _ptpl->SetModifiedFlag( FALSE );
            SetCursor( hCursor );
            }
        }
    CATCH_ALL
        {
        ar.Close( );
        if ( hCursor )
            {
            SetCursor( hCursor );
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// LoadTemplate( )
//
// This routine retrieves the template from disk.
//////////////////////////////////////////////////////////////////////////////
static void LoadTemplate( HWND hwnd )
    {
    HCURSOR  hCursor = NULL;
    QArchive ar;
    BOOL     f;
    UINT     ot;
    
    TRY
        {
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        f = ar.Open( _ptpl->GetFileName( ), nModeRead );
        if ( f )
            {
            f = ar.ReadHeader( &ot );
            if ( !f )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            _ptpl->Retrieve( ar, ot );
            ar.Close( );
            }
        else
            {
            MsgBox( hwnd, IDS_TEMPLATENOTFOUND, MB_OK );
            // make sure we query the user to save the template
            _ptpl->SetModifiedFlag( TRUE );
            }
        SetCursor( hCursor );
        }
    CATCH_ALL
        {
        ar.Close( );
        _ptpl->Cleanup( );
        if ( hCursor )
            {
            SetCursor( hCursor );
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// SaveQuery( )
//
// Returns TRUE if the document has been modified and the user confirms
// he wishes to save the document.
//////////////////////////////////////////////////////////////////////////////
extern BOOL SaveQuery( HWND hwnd )
    {
    int id;
    BOOL fRet = FALSE;

    TASKLIST
        {
        if ( !_pdoc->GetModifiedFlag( ) )
            {
            fRet = TRUE;
            QUIT;
            }
        id = MsgBox( hwnd, IDS_SAVEQUERY, MB_YESNO );
        if ( id == IDYES )
            {
            // let exceptions propagate
            fRet = SaveDocument( hwnd, FALSE );
            }
        else if ( id == IDNO )
            {
            fRet = TRUE;
            }
        // else, QUIT
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// SaveDocument( )
//
// Saves a named/unnamed document.
//////////////////////////////////////////////////////////////////////////////
static BOOL SaveDocument( HWND hwnd, BOOL fQuery )
    {
    return ( _pdoc->GetFileName( ) ) ? Save( hwnd, _pdoc->GetFileName( ), fQuery ) : SaveAs( hwnd, fQuery );
    }

//////////////////////////////////////////////////////////////////////////////
// Save( )
//
// Saves a named document.
//////////////////////////////////////////////////////////////////////////////
static BOOL Save( HWND hwnd, const CHAR *pszFileName, BOOL fQuery )
    {
    HCURSOR  hCursor = NULL;
    QArchive ar;
    QString sName;
    FILE   *pfile = NULL;
    CHAR    szDrive[ _MAX_DRIVE + 1 ];
    CHAR    szDir[ _MAX_DIR + 1 ];
    CHAR    szFName[ _MAX_FNAME + 1 ];
    CHAR    szExt[ _MAX_EXT + 1 ];
    CHAR   *pch;

    BOOL fRet = FALSE;

    TRY
        {
        TASKLIST
            {
            if ( fQuery && ( DosAccess( pszFileName ) && MsgBox( hwnd, IDS_REPLACE, MB_OKCANCEL ) != IDOK ) )
                { 
                QUIT;
                }

            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            _splitpath( pszFileName, szDrive, szDir, szFName, szExt );
            sName = szFName;
            sName += szExt;
            pch = szExt;
            if ( *pch == '.' )
                {
                pch++;
                }
            AnsiUpper( pch );
            if ( lstrcmp( pch, "QT" ) == 0 )
                {
                if ( ar.Open( pszFileName, nModeWrite ) )
                    {
                    _pszFilter = _pszFilter1;
                    _pdoc->SetFileName( sName );
                    _pdoc->Store( ar );
                    ar.Close( );
                    UpdateName( );
                    _pdoc->SetModifiedFlag( FALSE );
                    fRet = TRUE;
                    }
                else
                    {
                    THROW_EXCEPTION( exFile );
                    }
                }
            else
                {
                pfile = fopen( pszFileName, GetString( IDS_FOPEN_WT ) );
                if ( pfile )
                    {
                    if ( lstrcmp( pch, "TXT" ) == 0 )
                        {
                        _pszFilter = _pszFilter2;
                        }
                    else
                        {
                        _pszFilter = _pszFilter3;
                        }
                    _pdoc->StoreText( pfile );
                    fclose( pfile );
                    _pdoc->SetFileName( sName );
                    UpdateName( );
                    _pdoc->SetModifiedFlag( FALSE );
                    fRet = TRUE;
                    }
                else
                    {
                    THROW_EXCEPTION( exFile );
                    }
                }
            SetCursor( hCursor );
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        sName.Cleanup( );
        if ( pfile )
            {
            fclose( pfile );
            }
        ar.Close( );
        if ( hCursor )
            {
            SetCursor( hCursor );
            }
        THROW_LAST( );
        }
    END_CATCH
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// SaveTextWindow( )
//
// Saves a text window to file
//////////////////////////////////////////////////////////////////////////////
static BOOL SaveTextWindow( HWND hwnd, BOOL &fViewed )
    {
    FILE *pfile = NULL;
    HCURSOR hCursor = NULL;
    OPENFILENAME ofn;
    CHAR szFile[ _cbPathMax + 1 ];
    CHAR szFileTitle[ _cbPathMax + 1 ];
    CHAR szInitialDir[ _cbPathMax + 1 ];
    BOOL fRet = FALSE;
    CHAR szClass[ _cbBufferMax + 1 ];

    TRY
        {
        TASKLIST
            {
            lmemset( szClass, 0, sizeof( szClass ) );
            HWND hwndActive = LOWORD( SendMessage( _hwndClient, WM_MDIGETACTIVE, 0, 0L ) );
            if ( !hwndActive )
                break;
        
            GetClassName( hwndActive, szClass, sizeof( szClass ) );
            if ( lstrcmp( szClass, GetString( IDS_CLSCHILDTRADELIST ) ) != 0 &&
                 lstrcmp( szClass, GetString( IDS_CLSCHILDPERFSUM ) ) != 0 )
                {
                MsgBox( hwnd, IDS_BAD_SAVE_TEXT );
                break;
                }
            
            // Set up the 'OFN' structure
            lmemset( &ofn, 0, sizeof( ofn ) );
            lmemset( szFile, 0, sizeof( szFile ) );
            lmemset( szFileTitle, 0, sizeof( szFileTitle ) );
            ofn.hwndOwner       = hwnd;
            ofn.lStructSize     = sizeof( ofn );
            ofn.lpstrFile       = szFile;
            ofn.lpstrInitialDir = szInitialDir;
            ofn.nMaxFile        = sizeof( szFile );
            ofn.lpstrFileTitle  = szFileTitle;
            ofn.nMaxFileTitle   = sizeof( szFileTitle );
            ofn.lpstrFilter     = (LPSTR) _pszFilterExport;

            // Ask user for the file name
            if ( !GetSaveFileName( &ofn ) )
                break;

            // Determine if we are over-writing it; if so, confirm this
            if ( DosAccess( szFile ) && MsgBox( hwnd, IDS_REPLACE, MB_OKCANCEL ) != IDOK )
                break;

            // Get File Handle
            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            pfile = fopen( szFile, GetString( IDS_FOPEN_WT ) );
            if ( !pfile )
                THROW_EXCEPTION( exFile );

            SendMessage( hwndActive, WM_SAVETEXT, 0, (LPARAM) pfile );
            
            // Finish up
            fclose( pfile );
            pfile = NULL;
            SetCursor( hCursor );
            hCursor = NULL;
            
            // Offer to view the file
            fViewed = ::ViewFile( hwnd, szFile );

            fRet = TRUE;
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        if ( pfile )
            {
            fclose( pfile );
            pfile = NULL;
            }
        if ( hCursor )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            }
        THROW_LAST( );
        }
    END_CATCH

    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// ExportTemplate( )
//
// Exports all template settings to a text file
//////////////////////////////////////////////////////////////////////////////
static BOOL ExportTemplate( HWND hwnd, BOOL &fViewed )
    {
    FILE *pfile = NULL;
    HCURSOR hCursor = NULL;
    BOOL fRet = FALSE;
    OPENFILENAME ofn;
    CHAR szFile[ _cbPathMax + 1 ];
    CHAR szFileTitle[ _cbPathMax + 1 ];
    CHAR szInitialDir[ _cbPathMax + 1 ];
    CHAR szDrive[ _MAX_DRIVE + 1 ];
    CHAR szDir[ _MAX_DIR + 1 ];
    CHAR szFName[ _MAX_FNAME + 1 ];
    CHAR szExt[ _MAX_EXT + 1 ];
    
    
    TRY
        {
        TASKLIST
            {
            // Deduce the file name
            lmemset( szFile, 0, sizeof( szFile ) );
            _splitpath( _ptpl->GetFileName( ), szDrive, szDir, szFName, szExt );
            _makepath( szFile, "", "", szFName, ".exp" );            

            // Deduce the initial directory
            lmemset( szInitialDir, 0, sizeof( szInitialDir ) );
            if ( *szDrive && *szDir )
                _makepath( szInitialDir, szDrive, szDir, "", "" );
            else
                GetWindowsDirectory( szInitialDir, sizeof( szInitialDir ) );
                
            // Remove the trailing slash from the directory, if present
            int idx = lstrlen( szInitialDir ) - 1;
            if ( idx >= 0 && szInitialDir[ idx ] == '\\' )
                szInitialDir[ idx ] = '\0';
            
            // Set up the 'OFN' structure
            lmemset( &ofn, 0, sizeof( ofn ) );
            lmemset( szFileTitle, 0, sizeof( szFileTitle ) );
            ofn.hwndOwner       = hwnd;
            ofn.lStructSize     = sizeof( ofn );
            ofn.lpstrFile       = szFile;
            ofn.lpstrInitialDir = szInitialDir;
            ofn.nMaxFile        = sizeof( szFile );
            ofn.lpstrFileTitle  = szFileTitle;
            ofn.nMaxFileTitle   = sizeof( szFileTitle );
            ofn.lpstrFilter     = (LPSTR) _pszFilterExport;
            
            // Ask user for the file name
            if ( !GetSaveFileName( &ofn ) )
                break;
        
            // Cannot use same name as template file itself
            if ( lstrcmpi( szFile, _ptpl->GetFileName( ) ) == 0 )
                {
                MsgBox( hwnd, IDS_BAD_EXPORT );
                break;
                }

            // Determine if we are over-writing it; if so, confirm this
            if ( DosAccess( szFile ) && MsgBox( hwnd, IDS_REPLACE, MB_OKCANCEL ) != IDOK )
                break;

            // Get File Handle
            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            pfile = fopen( szFile, GetString( IDS_FOPEN_WT ) );
            if ( !pfile )
                THROW_EXCEPTION( exFile );

            // Call the template class to export it
            _ptpl->Export( pfile );
            
            // Finish up
            fclose( pfile );
            pfile = NULL;
            SetCursor( hCursor );
            hCursor = NULL;
            fRet = TRUE;

            // Offer to view the file
            fViewed = ::ViewFile( hwnd, szFile );
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        if ( pfile )
            {
            fclose( pfile );
            pfile = NULL;
            }
        if ( hCursor )
            {
            SetCursor( hCursor );
            hCursor = NULL;
            }
        THROW_LAST( );
        }
    END_CATCH
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// SaveAs( )
//
// Saves an unnamed document.
//////////////////////////////////////////////////////////////////////////////
static BOOL SaveAs( HWND hwnd, BOOL fQuery )
    {
    BOOL fRet = FALSE;
    OPENFILENAME ofn;
    CHAR szFile[ _cbPathMax + 1 ];
    CHAR szFileTitle[ _cbPathMax + 1 ];
    
    lmemset( &ofn, 0, sizeof( ofn ) );
    lmemset( szFile, 0, sizeof( szFile ) );
    lmemset( szFileTitle, 0, sizeof( szFileTitle ) );
    ofn.hwndOwner   = hwnd;
    ofn.lStructSize = sizeof( ofn );
    ofn.lpstrFile   = szFile;
    ofn.nMaxFile    = sizeof( szFile );
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle  = sizeof( szFileTitle );
    ofn.lpstrFilter    = (LPSTR) _pszFilter;
    if ( GetSaveFileName( &ofn ) )
        {
        fRet = Save( hwnd, szFile, fQuery );
        }
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// SilentOpen( )
//
// Opens a new document, no questions asked.
//////////////////////////////////////////////////////////////////////////////
extern BOOL SilentOpen( const CHAR *pszName )
    {
    QArchive ar;
    UINT ot;
    FILE *pfile = NULL;
    BOOL fRet( FALSE );
    BOOL fReadOnly( TRUE );
    QString sName;
    CHAR szDrive[ _MAX_DRIVE + 1 ];
    CHAR szDir[ _MAX_DIR + 1 ];
    CHAR szFName[ _MAX_FNAME + 1 ];
    CHAR szExt[ _MAX_EXT + 1 ];
    CHAR *pch;

    TRY
        {
        TASKLIST
            {
            DestroyAllChildren( );
            _pdoc->Cleanup( );
            
            _splitpath( pszName, szDrive, szDir, szFName, szExt );
            sName = szFName;
            sName += szExt;

            pch = szExt;
            if ( *pch == '.' )
                {
                pch++;
                }
            AnsiUpper( pch );

            if ( lstrcmp( pch, "QT" ) == 0 )
                {
                if ( ar.Open( sName, nModeRead ) )
                    {
                    _pdoc->SetFileName( sName );
                    BOOL f = ar.ReadHeader( &ot );
                    if ( !f )
                        THROW_EXCEPTION( exFileCorrupt );
                        
                    _pdoc->Retrieve( ar, ot );
                    ar.Close( );
                    UpdateName( );
                    fRet = TRUE;
                    }
                }
            else
                {
                pfile = softOpen( sName, GetString( IDS_FOPEN_RT ), fReadOnly );
                if ( !pfile )
                    THROW_EXCEPTION( exFile );
                _pdoc->RetrieveText( pfile, TRUE );
                fclose( pfile );
                _pdoc->SetFileName( sName );
                UpdateName( );
                fRet = TRUE;
                }
            _pdoc->readOnly( fReadOnly );
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        if ( pfile )
            fclose( pfile );
        sName.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// LoadNext( )
//
// Opens the previous, or next document, in the current directory
//////////////////////////////////////////////////////////////////////////////
static void LoadNext( HWND hwndParent, BOOL fNext )
    {
    CHAR szDrive[ _MAX_DRIVE + 1 ];
    CHAR szDir  [ _MAX_DIR   + 1 ];
    CHAR szFName[ _MAX_FNAME + 1 ];
    CHAR szExt  [ _MAX_EXT   + 1 ];
    QDirectory dir;
    HCURSOR hCursor = NULL;
    QString sCurrentChart;
    BOOL fFound = FALSE;
    
    TRY
        {
        TASKLIST
            {
            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        
            lmemset( szDrive, 0, sizeof( szDrive ) );
            lmemset( szDir,   0, sizeof( szDir ) );
            lmemset( szFName, 0, sizeof( szFName ) );
            lmemset( szExt,   0, sizeof( szExt ) );
        
            dir.read( );
            if ( !dir.size( ) )
                {
                Trace( TR_NORMAL, "No directory items found\n" );
                MessageBeep( 0 );
                break; // TASKLIST
                }
            sCurrentChart = _pdoc->GetFileName( );     
        
            fFound = FALSE;
            if ( (const CHAR *) sCurrentChart )
                {
                UINT i;

                for ( i = 0; i < dir.size( ); i++ )
                    {
                    if ( lstrcmpi( sCurrentChart, dir[ i ].fullname( ) ) == 0 )
                        {
                        fFound = TRUE;
                        break; // for
                        }
                    }
            
                if ( !fFound )
                    {
                    Trace( TR_NORMAL, "Chart not found in directory\n" );
                    MessageBeep( 0 );
                    break;
                    }

                if ( fNext )
                    {
                    if ( i < dir.size( ) - 1 )
                        {
                        SaveQuery( hwndParent );
                        DestroyAllChildren( );
                        _pdoc->Cleanup( );

                        SilentOpen( dir[ i + 1 ].fullname( ) );
                        AutoOpen( );
                        _pdoc->readOnly( TRUE );
                        }
                    else
                        {
                        Trace( TR_NORMAL, "End of directory list\n" );
                        MessageBeep( 0 );
                        break; // TASKLIST
                        }
                    }
                else
                    {
                    if ( i > 0 )
                        {
                        SaveQuery( hwndParent );
                        DestroyAllChildren( );
                        _pdoc->Cleanup( );
                        
                        SilentOpen( dir[ i - 1 ].fullname( ) );
                        AutoOpen( );
                        _pdoc->readOnly( TRUE );
                        }
                    else
                        {
                        Trace( TR_NORMAL, "End of directory list\n" );
                        MessageBeep( 0 );
                        break; // TASKLIST
                        }
                    }
                }
            else
                {
                if ( fNext )
                    {
                    SaveQuery( hwndParent );
                    DestroyAllChildren( );
                    _pdoc->Cleanup( );
                    
                    SilentOpen( dir[ 0 ].fullname( ) );
                    AutoOpen( );
                    _pdoc->readOnly( TRUE );
                    }
                else
                    {
                    SaveQuery( hwndParent );
                    DestroyAllChildren( );
                    _pdoc->Cleanup( );
                    
                    SilentOpen( dir[ dir.size( ) - 1 ].fullname( ) );
                    AutoOpen( );
                    _pdoc->readOnly( TRUE );
                    }
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        DestroyAllChildren( );
        _pdoc->Cleanup( );
        UpdateName( );
        UpdateMenu( );
        if ( hCursor )
            SetCursor( hCursor );
        dir.Cleanup( );
        sCurrentChart.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    
    UpdateName( );
    UpdateMenu( );
    if ( hCursor )
        SetCursor( hCursor );
    }

//////////////////////////////////////////////////////////////////////////////
// Open( )
//
// Opens a new document.
//////////////////////////////////////////////////////////////////////////////
static BOOL Open( HWND hwnd )
    {
    HCURSOR hCursor = NULL;
    BOOL fRet = FALSE;
    QArchive ar;
    UINT ot;
    FILE *pfile = NULL;
    OPENFILENAME ofn;
    CHAR szFile[ _cbPathMax + 1 ];
    CHAR szFileTitle[ _cbPathMax + 1 ];
    QString sName;
    BOOL fReadOnly;
    
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
            lmemset( szFileTitle, 0, sizeof( szFileTitle ) );
            ofn.hwndOwner      = hwnd;
            ofn.lStructSize    = sizeof( ofn );
            ofn.lpstrFile      = szFile;
            ofn.nMaxFile       = sizeof( szFile );
            ofn.lpstrFileTitle = szFileTitle;
            ofn.lpstrDefExt    = _pszDefExt;
            ofn.nMaxFileTitle  = sizeof( szFileTitle );
            ofn.lpstrFilter    = (LPSTR) _pszFilter;
            ofn.Flags         |= OFN_READONLY;
            if ( GetOpenFileName( &ofn ) )
                {
                CHAR szDrive[ _MAX_DRIVE + 1 ];
                CHAR szDir[ _MAX_DIR + 1 ];
                CHAR szFName[ _MAX_FNAME + 1 ];
                CHAR szExt[ _MAX_EXT + 1 ];
                CHAR *pch;

                fReadOnly = ( ( ofn.Flags & OFN_READONLY ) ? TRUE : FALSE );
                hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                _splitpath( szFile, szDrive, szDir, szFName, szExt );
                sName = szFName;
                sName += szExt;
                pch = szExt;
                if ( *pch == '.' )
                    {
                    pch++;
                    }
                AnsiUpper( pch );
                if ( lstrcmp( pch, "QT" ) == 0 )
                    {
                    if ( ar.Open( szFile, nModeRead ) )
                        {
                        _pszFilter = _pszFilter1;
                        DestroyAllChildren( );
                        _pdoc->Cleanup( );
                        _pdoc->SetFileName( sName );
                        BOOL f = ar.ReadHeader( &ot );
                        if ( !f )
                            {
                            THROW_EXCEPTION( exFileCorrupt );
                            }
                        _pdoc->Retrieve( ar, ot );
                        ar.Close( );
                        UpdateName( );
                        fRet = TRUE;
                        }
                    else
                        {
                        MsgBox( hwnd, IDS_GENERALERROR );
                        QUIT;
                        }
                    }
                else
                    {
                    pfile = softOpen( szFile, GetString( IDS_FOPEN_RT ), fReadOnly );
                    if ( pfile )
                        {
                        if ( lstrcmp( pch, "TXT" ) == 0 )
                            {
                            _pszFilter = _pszFilter2;
                            }
                        else
                            {
                            _pszFilter = _pszFilter3;
                            }
                        DestroyAllChildren( );
                        _pdoc->Cleanup( );
                        _pdoc->RetrieveText( pfile );
                        fclose( pfile );
                        _pdoc->SetFileName( sName );
                        UpdateName( );
                        fRet = TRUE;
                        }
                    else
                        {
                        MsgBox( hwnd, IDS_GENERALERROR );
                        QUIT;
                        }
                    }

                AutoOpen( );
                _pdoc->readOnly( fReadOnly );
                SetCursor( hCursor );
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        if ( pfile )
            {
            fclose( pfile );
            }
        ar.Close( );
        sName.Cleanup( );
        DestroyAllChildren( );
        _pdoc->Cleanup( );
        ClearName( );
        if ( hCursor )
            {
            SetCursor( hCursor );
            }
        THROW_LAST( );
        }
    END_CATCH
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// OpenPerfSumWindow( )
//
// Opens a new trade list child window.
//////////////////////////////////////////////////////////////////////////////
extern void OpenPerfSumWindow( BOOL fMaximized, BOOL fPerformanceTester )
    {
    MDICREATESTRUCT mdicreate;
    QString         sClassChild( GetString( IDS_CLSCHILDPERFSUM ) );
    
    TRY
        {
        mdicreate.szClass = sClassChild;
        if ( fPerformanceTester )
            mdicreate.szTitle = GetString( IDS_TITLEPERFSUM );
        else
            mdicreate.szTitle = GetString( IDS_TITLESIMPERFSUM );
        mdicreate.hOwner  = _hInstance;
        mdicreate.x       = CW_USEDEFAULT;
        mdicreate.y       = CW_USEDEFAULT;
        mdicreate.cx      = CW_USEDEFAULT;
        mdicreate.cy      = CW_USEDEFAULT;
        mdicreate.style   = fMaximized ? WS_MAXIMIZE | WS_VSCROLL : WS_VSCROLL;
        mdicreate.lParam  = 0L;
        SendMessage( _hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT) &mdicreate );
        }
    CATCH_ALL
        {
        sClassChild.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// OpenTradeListWindow( )
//
// Opens a new trade list child window.
//////////////////////////////////////////////////////////////////////////////
extern void OpenTradeListWindow( BOOL fMaximized, BOOL fPerformanceTester )
    {
    MDICREATESTRUCT mdicreate;
    QString         sClassChild( GetString( IDS_CLSCHILDTRADELIST ) );
    
    TRY
        {
        mdicreate.szClass = sClassChild;
        if ( fPerformanceTester )
            mdicreate.szTitle = GetString( IDS_TITLETRADELIST );
        else
            mdicreate.szTitle = GetString( IDS_TITLESIMTRADELIST );
        mdicreate.hOwner  = _hInstance;
        mdicreate.x       = CW_USEDEFAULT;
        mdicreate.y       = CW_USEDEFAULT;
        mdicreate.cx      = CW_USEDEFAULT;
        mdicreate.cy      = CW_USEDEFAULT;
        mdicreate.style   = fMaximized ? WS_MAXIMIZE | WS_VSCROLL : WS_VSCROLL;
        mdicreate.lParam  = 0L;
        SendMessage( _hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT) &mdicreate );
        }
    CATCH_ALL
        {
        sClassChild.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// OpenTextWindow( )
//
// Opens a new text child window.
//////////////////////////////////////////////////////////////////////////////
extern void OpenTextWindow( BOOL fMaximized )
    {
    MDICREATESTRUCT mdicreate;
    QString         sClassChild( GetString( IDS_CLSCHILDTEXT ) );
    
    TRY
        {
        mdicreate.szClass = sClassChild;
        mdicreate.szTitle = GetString( IDS_TITLETEXT );
        mdicreate.hOwner  = _hInstance;
        mdicreate.x       = CW_USEDEFAULT;
        mdicreate.y       = CW_USEDEFAULT;
        mdicreate.cx      = CW_USEDEFAULT;
        mdicreate.cy      = CW_USEDEFAULT;
        mdicreate.style   = fMaximized ? WS_MAXIMIZE | WS_VSCROLL : WS_VSCROLL;
        mdicreate.lParam  = 0L;
        SendMessage( _hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT) &mdicreate );
        }
    CATCH_ALL
        {
        sClassChild.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// OpenWindow( )
//
// Opens a new child window.
//////////////////////////////////////////////////////////////////////////////
extern void OpenWindow( const CHAR *pszLayout, BOOL fMaximized )
    {
    MDICREATESTRUCT mdicreate;
    QString         sClassChild( GetString( IDS_CLSCHILD ) );
    
    TRY
        {
        ASSERT( *pszLayout != 0 );
        mdicreate.szClass = sClassChild;
        mdicreate.szTitle = pszLayout;
        mdicreate.hOwner  = _hInstance;
        mdicreate.x       = CW_USEDEFAULT;
        mdicreate.y       = CW_USEDEFAULT;
        mdicreate.cx      = CW_USEDEFAULT;
        mdicreate.cy      = CW_USEDEFAULT;
        mdicreate.style   = fMaximized ? WS_MAXIMIZE | WS_HSCROLL : WS_HSCROLL;
        mdicreate.lParam  = 0L;
        SendMessage( _hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT) &mdicreate );
        }
    CATCH_ALL
        {
        sClassChild.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// UpdateAllViews( )
//
// Forces a repaint of all child views.
//////////////////////////////////////////////////////////////////////////////
extern void UpdateAllViews( BOOL fReBuild )
    {
    FARPROC lpfnEnum;

    HCURSOR hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
    lpfnEnum = MakeProcInstance( (FARPROC) InvalidateEnumProc, 
                                 _hInstance );
    EnumChildWindows( _hwndClient, lpfnEnum, (LPARAM) fReBuild );
    FreeProcInstance( lpfnEnum );
    SetCursor( hCursor );
    }

//////////////////////////////////////////////////////////////////////////////
// DoReSize( )
//
// Manages resizing of a view.
//////////////////////////////////////////////////////////////////////////////
static void DoReSize( HWND hwnd, QView *pview, BOOL fScrollToEnd )
    {
    RECT rc;
    HDC  hdc;

    hdc = GetDC( hwnd );
    SetMapMode( hdc, MM_LOENGLISH );
    GetClientRectSpecial( hwnd, &rc );
    SetViewportOrg( hdc, 0, rc.bottom );
    DPtoLP( hdc, (LPPOINT) &rc, 2 );
    TRY
        {
        QWindowLayout *pwl = GetWindowLayout( hwnd );
        ASSERT( pwl != NULL );
        pview->OnReSize( pwl, hwnd, hdc, rc, fScrollToEnd );
        }
    CATCH_ALL
        {
        ReleaseDC( hwnd, hdc );
        THROW_LAST( );
        }
    END_CATCH
    ReleaseDC( hwnd, hdc );
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// AssertEnumProc( )
//
// This routine manages the AssertValid calls to the
// different hwnd's.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export AssertEnumProc( HWND hwnd, LONG lParam )
    {
    CHAR    szBuffer[ _cbBufferMax + 1 ];
    QString sClassChild( GetString( IDS_CLSCHILD ) );

    TRY
        {
        GetClassName( hwnd, szBuffer, sizeof( szBuffer ) );
        if ( sClassChild == szBuffer )
            {
            const QView *pview = GetViewPtr( hwnd );
            ASSERT( pview != NULL );
            if ( pview )
                {
                Trace( TR_NORMAL, "Checking view %x.\n", hwnd );
                pview->AssertValid( );
                }
            }
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, "AssertEnumProc( ) exception thrown.\n" );
        sClassChild.Cleanup( );
        }
    END_CATCH
    return TRUE;
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// InvalidateEnumProc( )
//
// Call-back routine used in conjunction with UpdateAllViews( ).
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export InvalidateEnumProc( HWND hwnd, LONG lParam )
    {
    BOOL    fRet( FALSE );
    CHAR    szBuffer[ _cbBufferMax + 1 ];
    QString sClassChild( GetString( IDS_CLSCHILD ) );

    TRY
        {
        TASKLIST
            {
            GetClassName( hwnd, szBuffer, sizeof( szBuffer ) );
            if ( sClassChild != szBuffer )
                {
                QUIT;
                }

            if ( (BOOL) lParam )
                {
                QView *pview = GetViewPtr( hwnd );
                ASSERT( pview != NULL );
                QWindowLayout *pwl = GetWindowLayout( hwnd );
                ASSERT( pwl != NULL );
                
                pview->OnReBuild( pwl );
                DoReSize( hwnd, pview, TRUE );
                }
            }
        ENDTASKLIST
        fRet = TRUE;
        }
    CATCH_ALL
        {
        sClassChild.Cleanup( );
        PostMessage( hwnd, WM_CLOSE, 0, 0L );
        }
    END_CATCH

    // Force a re-paint
    InvalidateRect( hwnd, NULL, TRUE );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// CloseEnumProc( )
//
// Call-back routine used in conjunction with closing all windows.
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
// DestroyAllChildren( )
//
// Call this routine to destroy all child windows.
//////////////////////////////////////////////////////////////////////////////
extern void DestroyAllChildren( )
    {
    FARPROC lpfn;
    lpfn = MakeProcInstance( (FARPROC) CloseEnumProc, _hInstance );
    EnumChildWindows( _hwndClient, lpfn, 0L );
    FreeProcInstance( lpfn );
    }

//////////////////////////////////////////////////////////////////////////////
// GetClientRectSpecial( )
//
// This routine is almost exactly like GetClientRect, except
// it accounts for the size of the quote box width.
//
// NOTE: This routine gets the rect in *device units*.
//////////////////////////////////////////////////////////////////////////////
extern void GetClientRectSpecial( HWND hwnd, LPRECT lprc )
    {
    GetClientRect( hwnd, lprc );
    if ( _ptpl->m_genopts.GetQuoteBox( ) )
        {
        lprc->bottom -= _dyBox;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QuoteBox( )
//
// This routine manages the quote box logic.
//////////////////////////////////////////////////////////////////////////////
static void QuoteBox( HWND hwnd )
    {
    HWND hwndBox;
    CHAR szBuffer[ _cbBufferMax + 1 ];
    int state = GetState( hwnd );
    if ( state == stNoSync || state == stSync )
        {
        // nil state
        return;
        }

    if ( state == stReStart )
        {
        QQt *pqqt = GetPqqt( hwnd );
        int count = GetCount( hwnd );
        ASSERT( count >= 0 );

        if ( count == 0 )
            {
#ifdef _STATE_TRACE
            Trace( TR_NORMAL, "State = stSync.\n" );
#endif
            SetState( hwnd, stSync );

            ASSERT( pqqt != NULL );
            if ( pqqt )
                {
                QTime date = pqqt->GetDate( );
                TRY
                    {
                    hwndBox = GetWindow( hwnd, GW_CHILD );
                    ASSERT( hwndBox != NULL );
                    sprintf( szBuffer, GetString( IDS_QUOTEBOX ), date.GetMonth( ),
                                                                  date.GetDay( ),
                                                                  date.GetYear( ),
                                                                  pqqt->GetOp( ),
                                                                  pqqt->GetHi( ),
                                                                  pqqt->GetLo( ),
                                                                  pqqt->GetCl( ),
                                                                  pqqt->GetVl( ) );
                    SetWindowText( hwndBox, szBuffer );
                    }
                CATCH_ALL
                    {
                    date.Cleanup( );
                    SetWindowText( hwndBox, GetString( IDS_EMPTY ) );
                    }
                END_CATCH
                InvalidateRect( hwndBox, NULL, TRUE );
                UpdateWindow( hwndBox );
                }
            return;
            }

        // do this n times to improve performance
        for ( int i = 0; i < nBackground && count > 0; i++ )
            {
            pqqt = _pdoc->GetNext( pqqt );
            count--;
            }
        SetPqqt( hwnd, pqqt );
        SetCount( hwnd, count );
        return;
        }

    ASSERT( FALSE );
    }

//////////////////////////////////////////////////////////////////////////////
// BackgroundEnumProc( )
//
// This routine is called (once for each window) in the background.
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export BackgroundEnumProc( HWND hwnd, LONG lParam )
    {
    BOOL    fRet( FALSE );
    CHAR    szBuffer[ _cbBufferMax + 1 ];
    QString sClassChild( GetString( IDS_CLSCHILD ) );

    TRY
        {
        TASKLIST
            {
            GetClassName( hwnd, szBuffer, sizeof( szBuffer ) );
            if ( sClassChild != szBuffer )
                {
                QUIT;
                }
            QuoteBox( hwnd );
            }
        ENDTASKLIST
        fRet = TRUE;
        }
    CATCH_ALL
        {
        sClassChild.Cleanup( );
        }
    END_CATCH
    
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// BackgroundTask( )
//
// This is the background task.  Place any background processing in here.
//////////////////////////////////////////////////////////////////////////////
static void BackgroundTask( )
    {
    if ( !_ptpl->m_genopts.GetQuoteBox( ) )
        {
        return;
        }

    FARPROC lpfnEnum = MakeProcInstance( (FARPROC) BackgroundEnumProc, _hInstance );
    EnumChildWindows( _hwndClient, lpfnEnum, 0L );
    FreeProcInstance( lpfnEnum );
    }

//////////////////////////////////////////////////////////////////////////////
// CreatePrinting( )
//
// This routine will create the printer device context.
//////////////////////////////////////////////////////////////////////////////
extern BOOL CreatePrinting( HWND hwndOwner, HDC *phdc )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {
        *phdc = GetPrinterDC( );
        if ( !*phdc )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// DestroyPrinting( )
//
// This routine will destroy the print device context.
//////////////////////////////////////////////////////////////////////////////
extern BOOL DestroyPrinting( HDC hdc )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {
        DeleteDC( hdc );
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// StartPrinting( )
//
// This routine will start the print sequence.
//////////////////////////////////////////////////////////////////////////////
extern BOOL StartPrinting( HDC hdc )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {
        if ( Escape( hdc, STARTDOC, sizeof( _szPrinterBanner ) - 1, 
                     _szPrinterBanner, NULL ) <= 0 )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// NewPage( )
//
// This routine will advance to a new page.
//////////////////////////////////////////////////////////////////////////////
extern BOOL NewPage( HDC hdc )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {           
        if ( Escape( hdc, NEWFRAME, 0, NULL, NULL ) <= 0 )
            break;
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }
    
//////////////////////////////////////////////////////////////////////////////
// StopPrinting( )
//
// This routine will stop the print sequence.
//////////////////////////////////////////////////////////////////////////////
extern BOOL StopPrinting( HDC hdc )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {
        Escape( hdc, ENDDOC, 0, NULL, NULL );
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// GetPrinterDC( )
// 
// This routine gets the printer device context from the WIN.INI file.
//////////////////////////////////////////////////////////////////////////////
HDC GetPrinterDC( VOID )
    {
    CHAR    szPrinter      [ _cbBufferMax + 1 ];
    CHAR    szWinIniWindows[ _cbBufferMax + 1 ];
    CHAR    szWinIniDevice [ _cbBufferMax + 1 ];
    CHAR   *pszDevice;
    CHAR   *pszDriver;
    CHAR   *pszOutput;
    HDC    hdc = NULL;

    TASKLIST
        {
        lstrcpy( szWinIniWindows, GetString( IDS_WININI_WINDOWS ) );
        lstrcpy( szWinIniDevice,  GetString( IDS_WININI_DEVICE ) );
        GetProfileString( szWinIniWindows, szWinIniDevice, ",,,", szPrinter, sizeof( szPrinter ) );
        pszDevice = strtok( szPrinter, "," );
        pszDriver = strtok( NULL, ", " );
        pszOutput = strtok( NULL, ", " );

        if ( !pszDevice || !pszDriver || !pszOutput )
            break;
            
        hdc = CreateDC( pszDriver, pszDevice, pszOutput, NULL );
        if ( !hdc )
            break;
        }
    ENDTASKLIST

    return hdc;
    }

//////////////////////////////////////////////////////////////////////////////
// PrintPage( )
//
// This routine will print one page.  It can be used in one
// of two ways:
//     -directly, to print a single page
//     -as a callback, from QView::iterate( )
//
// Users must first prepare a page by calling preparePage( )
// before calling PrintPage( ).
//
// Parameters:
//     pview - the Printer QView established
//     nPage - the page number  Note: if 0, do not print the page number
//     ps    - the Print Structure
//
// Returns:
//     void
//////////////////////////////////////////////////////////////////////////////
static void PrintPage( QView *pview, int nPage, PrintStruct &ps )
    {
    HFONT   hfont( NULL );
    QString sTitle;
    LOGFONT lf; 

    TRY
        {
        TASKLIST
            {
            SaveDC( ps.hdc );
            
            lmemset( &lf, 0, sizeof( lf ) );
            lf.lfWeight         = FW_NORMAL;
            lf.lfHeight         = _ptpl->m_genopts.GetFontSize( );
            lstrcpy( lf.lfFaceName, _pszFontName );
            
            hfont = CreateFontIndirect( &lf );
            if ( hfont == NULL )
                THROW_EXCEPTION( exGdi );
                    
            SelectObject( ps.hdc, hfont );
            SetBkMode( ps.hdc, TRANSPARENT );

            if ( _pdoc->GetFileName( ) )
                sTitle = _pdoc->GetFileName( );
            else
                sTitle = GetString( IDS_UNTITLED );

            if ( nPage > 0 )
                {
                CHAR szTmp[ 16 + 1 ];
                wsprintf( szTmp, "%d", nPage );
                sTitle += " - ";
                sTitle += szTmp;
                }
            
            int dx = (int) LOWORD( GetTextExtent( ps.hdc, sTitle, (int) sTitle.GetLength( ) ) );
            int x  = ps.rcPage.left + ( ( ps.rcPage.right - ps.rcPage.left ) / 2 ) - ( dx / 2 );
            int y  = ps.rcPage.top + 2 * _ptpl->m_genopts.GetFontSize( );
            TextOut( ps.hdc, x, y, sTitle, (int) sTitle.GetLength( ) );

            pview->OnDraw( ps.pwl, ps.hdc );
            NewPage( ps.hdc );
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        sTitle.Cleanup( );
        RestoreDC( ps.hdc, -1 );
        SafeDeleteObject( hfont );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( ps.hdc, -1 );
    SafeDeleteObject( hfont );
    }

//////////////////////////////////////////////////////////////////////////////
// PrintQuotes( )
//
// This routine will handle printing management.
//
// Parameters:
//     pszName   - The window layout name.
//     numTop    - Top margin, in inches.
//     numLeft   - Left margin, in inches.
//     numBottom - Bottom margin, in inches.
//     numRight  - Right margin, in inches.
//     fLastPageOnly - TRUE if the user selected to print only the last page.
//////////////////////////////////////////////////////////////////////////////
extern void PrintQuotes( HWND hwndOwner, const CHAR *pszName, 
                         NUM numTop, NUM numLeft, NUM numBottom, 
                         NUM numRight, BOOL fLastPageOnly )
    {
    PrintStruct ps;
    BOOL        fStartedPrinting = FALSE;
    HCURSOR     hCursor          = NULL;
    HDC         hdc              = NULL;
    QView      *pview            = NULL;
    
    
    TRY
        {
        TASKLIST
            {
            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            if ( !CreatePrinting( hwndOwner, &hdc ) )
                break;
        
            if ( !StartPrinting( hdc ) )
                break;

            fStartedPrinting = TRUE;

            QWindowLayout *pwl = _ptpl->m_wllist.Find( pszName );
            ASSERT( pwl != NULL );
            pview = new QView( pszName, TRUE );
            if ( !pview )
                THROW_EXCEPTION( exMemory );
            pview->OnReBuild( pwl );

            lmemset( &ps, 0, sizeof( ps ) );
            ps.pwl       = pwl;
            ps.hdc       = hdc;
            ps.numTop    = numTop;
            ps.numLeft   = numLeft;
            ps.numBottom = numBottom;
            ps.numRight  = numRight;

            if ( fLastPageOnly )
                {
                pview->preparePage( ps, TRUE );
                PrintPage( pview, 0, ps );
                }
            else
                {
                pview->home( NULL ); // set up m_pqqtFirst and m_nFirst
                pview->preparePage( ps, FALSE );
                pview->iterate( NULL, PrintPage, ps );
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        delete pview;
        if ( fStartedPrinting )
            StopPrinting( hdc );
        if ( hdc )
            DestroyPrinting( hdc );
        SetCursor( hCursor );
        THROW_LAST( );
        }
    END_CATCH

    delete pview;

    if ( fStartedPrinting )
        StopPrinting( hdc );
    if ( hdc )
        DestroyPrinting( hdc );

    SetCursor( hCursor );
    }

//////////////////////////////////////////////////////////////////////////////
// PrintSetup( )
//
// This routine does the Print Setup thing.
//////////////////////////////////////////////////////////////////////////////
static BOOL PrintSetup( HWND hwnd )
    {
    BOOL    fRet( FALSE );
    HANDLE  hLibrary;
    CHAR    szPrinter      [ _cbBufferMax + 1 ];
    CHAR    szFile         [ _cbBufferMax + 1 ];
    CHAR    szWinIniWindows[ _cbBufferMax + 1 ];
    CHAR    szWinIniDevice [ _cbBufferMax + 1 ];
    CHAR   *pszDevice;
    CHAR   *pszDriver;
    CHAR   *pszOutput;
    DEVMODEPROC lpfn;

    TASKLIST
        {
        hLibrary = NULL;
        lstrcpy( szWinIniWindows, GetString( IDS_WININI_WINDOWS ) );
        lstrcpy( szWinIniDevice,  GetString( IDS_WININI_DEVICE ) );
        GetProfileString( szWinIniWindows, szWinIniDevice, ",,,", 
                          szPrinter, sizeof( szPrinter ) );
        pszDevice = strtok( szPrinter, "," );
        pszDriver = strtok( NULL, ", " );
        pszOutput = strtok( NULL, ", " );
        lstrcpy( szFile, pszDriver );
        lstrcat( szFile, GetString( IDS_DOTDRV ) );
        
        hLibrary = LoadLibrary( szFile );
        if ( hLibrary < 32 )
            {
            hLibrary = NULL;
            break;
            }
        lpfn = (DEVMODEPROC) GetProcAddress( hLibrary, GetString( IDS_DEVICEMODE ) );
        if ( !lpfn )
            break;
        lpfn( hwnd, hLibrary, pszDevice, pszOutput );
        fRet = TRUE;
        }
    ENDTASKLIST

    if ( hLibrary )
        FreeLibrary( hLibrary );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// softOpen( )
//
// This routine will open a text file, supporting symbolic links
// if necessary.  The function will set fReadOnly to TRUE
// if a soft open has occurred; otherwise it will not modify
// the value of fReadOnly.
//////////////////////////////////////////////////////////////////////////////
static FILE *softOpen( const CHAR *szFileName, const CHAR *szMode, BOOL &fReadOnly )
    {
    FILE *pfile    = NULL;
    int nLinkCount = nLinkCountMax;
    BOOL fFound    = FALSE;
    BOOL fEOF;
    QString sPath;
    CHAR    szBuffer[ _cbTextMax + 1 ];

    TRY
        {
        sPath = szFileName;
        while ( !fFound && nLinkCount )
            {
            pfile = fopen( sPath, szMode );
            if ( pfile )
                {
                ReadText( pfile, szBuffer, sizeof( szBuffer ), &fEOF );
                if ( !fEOF && memcmp( szBuffer, "@::", 3 ) == 0 )
                    {
                    fclose( pfile );
                    pfile = NULL;
                    sPath = ( szBuffer + 3 );
                    nLinkCount--;
                    fReadOnly = TRUE;
                    }
                else
                    {
                    if ( fseek( pfile, 0, SEEK_SET ) != 0 )
                        THROW_EXCEPTION( exFile );
                    fFound = TRUE;
                    }
                }
            else
                {
                break;
                }
            }
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, "Exception thrown, caught by softOpen( )\n" );
        if ( pfile )
            fclose( pfile );
        pfile = NULL;
        sPath.Cleanup( );
        }
    END_CATCH

    return pfile;
    }

//////////////////////////////////////////////////////////////////////////////
// AutoOpen( )
//
// Automatically opens any windows marked 'auto-open'
//////////////////////////////////////////////////////////////////////////////
static void AutoOpen( void )
    {
    UINT nCount = 0;
    QWindowLayout *pwl;
    
    if ( _ptpl->m_genopts.GetAutoOpenText( ) )
        ++nCount;

    for ( pwl = (QWindowLayout *) _ptpl->m_wllist.GetHead( );
          pwl;
          pwl = (QWindowLayout *) _ptpl->m_wllist.GetNext( pwl ) )
        {
        if ( pwl->GetAutoOpen( ) )
            ++nCount;
        }

    BOOL fMaximize = ( nCount == 1 );
    for ( pwl = (QWindowLayout *) _ptpl->m_wllist.GetHead( );
          pwl;
          pwl = (QWindowLayout *) _ptpl->m_wllist.GetNext( pwl ) )
        {
        if ( pwl->GetAutoOpen( ) )
            OpenWindow( pwl->GetName( ), fMaximize );
        }
    if ( _ptpl->m_genopts.GetAutoOpenText( ) )
        OpenTextWindow( fMaximize );
    
    if ( !fMaximize )
        PostMessage( _hwndFrame, WM_COMMAND, IDM_TILE, 0L );
    }

