//////////////////////////////////////////////////////////////////////////////
// QUOTE.HXX
//
// Quote include file.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _QUOTE_HXX
#define _QUOTE_HXX

extern HANDLE _hInstance;               // Data Instance
extern HANDLE _hAccel;                  // Accelerator table
extern HMENU  _hmenu;                   // Main Menu
extern HMENU  _hmenuWindow;             // Window Menu

extern HWND   _hwndFrame;               // Frame Window
extern HWND   _hwndClient;              // Client Window

extern QDocument *_pdoc;                // Document object
extern QTemplate *_ptpl;                // Template object
extern QExternalIndicatorDoc *_pExtDoc; // External indicator object
extern QGCache    gcache;               // Global Cache object
extern const QVersion _vApp;            // Application version

extern const USHORT nIdxWhite;          // Color table indices
extern const USHORT nIdxLtGray;
extern const USHORT nIdxGray;
extern const USHORT nIdxDkGray;
extern const USHORT nIdxBlack;
extern const USHORT nIdxRed;
extern const USHORT nIdxBlue;
extern const USHORT nIdxPurple;

// Print Dialog global variables
extern HWND    _hDlgProcess1;
extern FARPROC _lpfnProcessDialogProc1;
extern BOOL    _fUserAbort1;
extern FARPROC _lpfnAbortProc;

// Profit Table Dialog global variables
extern HWND    _hDlgProcess2;
extern FARPROC _lpfnProcessDialogProc2;
extern BOOL    _fUserAbort2;
extern BOOL    _fProfitTable;

extern const CHAR *_pszFilter1;
extern const CHAR *_pszFilter2;
extern const CHAR *_pszFilter3;
extern const CHAR *_pszFilter;
extern const CHAR *_pszFontName;

#define nColors 125
extern COLORREF _rgclr[ nColors ];      // The color table

#define RGCLRSIZE ( sizeof( _rgclr ) / sizeof( _rgclr[ 0 ] ) )

#ifdef DEBUG
extern int _nTraceIdm;                  // trace level IDM
#endif

//////////////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////////////
extern "C"
    {
    LONG FAR PASCAL _export FrameWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    LONG FAR PASCAL _export ChildWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    LONG FAR PASCAL _export ChildTextWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    LONG FAR PASCAL _export ChildTradeListWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    LONG FAR PASCAL _export ChildPerfSumWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export InvalidateEnumProc( HWND hwnd, LONG lParam );
    BOOL FAR PASCAL _export CloseEnumProc( HWND hwnd, LONG lParam );
    BOOL FAR PASCAL _export BackgroundEnumProc( HWND hwnd, LONG lParam );
    BOOL FAR PASCAL _export PrintDlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
#ifdef DEBUG
    BOOL FAR PASCAL _export AssertEnumProc( HWND hwnd, LONG lParam );
#endif
    };

#define nLogUnitPerLogInch 100

extern void OpenWindow( const CHAR *pszLayout, BOOL fMaximized );
extern void OpenTextWindow( BOOL fMaximized );
extern void OpenTradeListWindow( BOOL fMaximized, BOOL fPerformanceTester );
extern void OpenPerfSumWindow( BOOL fMaximized, BOOL fPerformanceTester = TRUE );
extern VOID UpdateAllViews( BOOL fRebuild = FALSE );
extern void GetClientRectSpecial( HWND hwnd, LPRECT lprc );
extern BOOL CreatePrinting( HWND hwndOwner, HDC *phdc );
extern BOOL StartPrinting( HDC hdc );
extern BOOL StopPrinting( HDC hdc );
extern BOOL DestroyPrinting( HDC hdc );
extern void PrintQuotes( HWND hwndOwner, const CHAR *pszName, 
                         NUM numTop, NUM numLeft, NUM numBottom, 
                         NUM numRight, BOOL fLastPageOnly );
extern BOOL SaveQuery( HWND hwnd );
extern void DestroyAllChildren( );
extern BOOL SilentOpen( const CHAR *pszName );
extern int LoEnglishToDeviceY( HDC hdc, int dyLoEnglish );

//////////////////////////////////////////////////////////////////////////////
// Other miscellaneous stuff
//////////////////////////////////////////////////////////////////////////////
#define xPosNull    ( -1 )

// Quote box states
#define stNoSync     ( 0 )              // No sync'ing going on state
#define stSync       ( 1 )              // quote box is sync'ed
#define stReStart    ( 2 )              // Re-start the sync'ing

#if ( FDATA == 1 )
#define GWW_XPOS    ( 0 * sizeof( WORD ) )
#define GWW_STATE   ( 1 * sizeof( WORD ) )
#define GWL_PQQT    ( 2 * sizeof( WORD ) )
#define GWW_COUNT   ( 4 * sizeof( WORD ) )
#define GWL_VIEWPTR ( 5 * sizeof( WORD ) )
#define GWW_MAX     ( 7 * sizeof( WORD ) )
#else
#define GWW_XPOS    ( 0 * sizeof( WORD ) )
#define GWW_STATE   ( 1 * sizeof( WORD ) )
#define GWW_PQQT    ( 2 * sizeof( WORD ) )
#define GWW_COUNT   ( 3 * sizeof( WORD ) )
#define GWW_VIEWPTR ( 4 * sizeof( WORD ) )
#define GWW_MAX     ( 5 * sizeof( WORD ) )
#endif

#define _dyBox      ( 20 )              // device units

struct PrintStruct
    {
    QWindowLayout *pwl;
    HDC  hdc;
    RECT rcPage;
    NUM  numTop;
    NUM  numLeft;
    NUM  numBottom;
    NUM  numRight;
    };

//////////////////////////////////////////////////////////////////////////////
// Exception types
//////////////////////////////////////////////////////////////////////////////
#define exFileCorrupt      ( exUser + 0 ) // this will become obsolete
                                          // once qobject is updated accordingly.
#define exInvalidIndicator ( exUser + 1 ) // invalid indicator exception

#define exGdi              ( exUser + 2 ) // Some GDI problem occurred.

#define exFloat            ( exUser + 3 ) // Floating point exception.

class QView;

//////////////////////////////////////////////////////////////////////////////
// GetXPos( )
//
// This routine gets the x-position of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline int GetXPos( HWND hwnd )
    {
    return (int) GetWindowWord( hwnd, GWW_XPOS );
    }

//////////////////////////////////////////////////////////////////////////////
// SetXPos( )
//
// This routine sets the x-position of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline void SetXPos( HWND hwnd, int xPos )
    {
    SetWindowWord( hwnd, GWW_XPOS, (WORD) xPos );
    }

//////////////////////////////////////////////////////////////////////////////
// GetState( )
//
// This routine gets the state of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline int GetState( HWND hwnd )
    {
    return (int) GetWindowWord( hwnd, GWW_STATE );
    }

//////////////////////////////////////////////////////////////////////////////
// SetState( )
//
// This routine sets the state of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline void SetState( HWND hwnd, int state )
    {
    SetWindowWord( hwnd, GWW_STATE, (WORD) state );
    }

//////////////////////////////////////////////////////////////////////////////
// GetPqqt( )
//
// This routine gets the QQt of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline QQt *GetPqqt( HWND hwnd )
    {                  
#if ( FDATA == 1 )
    return (QQt *) GetWindowLong( hwnd, GWL_PQQT );
#else    
    return (QQt *) GetWindowWord( hwnd, GWW_PQQT );
#endif    
    }

//////////////////////////////////////////////////////////////////////////////
// SetPqqt( )
//
// This routine sets the QQt of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline void SetPqqt( HWND hwnd, QQt *pqqt )
    {
#if ( FDATA == 1 )
    SetWindowLong( hwnd, GWL_PQQT, (LONG) pqqt );
#else    
    SetWindowWord( hwnd, GWW_PQQT, (WORD) pqqt ); 
#endif    
    }

//////////////////////////////////////////////////////////////////////////////
// GetCount( )
//
// This routine gets the count of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline int GetCount( HWND hwnd )
    {
    return (int) GetWindowWord( hwnd, GWW_COUNT );
    }

//////////////////////////////////////////////////////////////////////////////
// SetCount( )
//
// This routine sets the count of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline void SetCount( HWND hwnd, int count )
    {
    SetWindowWord( hwnd, GWW_COUNT, (WORD) count );
    }

//////////////////////////////////////////////////////////////////////////////
// GetViewPtr( )
//
// This routine gets the view pointer of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline QView *GetViewPtr( HWND hwnd )
    {
#if ( FDATA == 1 )
    return (QView *) GetWindowLong( hwnd, GWL_VIEWPTR );
#else    
    return (QView *) GetWindowWord( hwnd, GWW_VIEWPTR );
#endif    
    }

//////////////////////////////////////////////////////////////////////////////
// SetViewPtr( )
//
// This routine sets the view pointer of the quote box updating.
//////////////////////////////////////////////////////////////////////////////
inline void SetViewPtr( HWND hwnd, QView *pview )
    {
#if ( FDATA == 1 )
    SetWindowLong( hwnd, GWL_VIEWPTR, (LONG) pview );
#else    
    SetWindowWord( hwnd, GWW_VIEWPTR, (WORD) pview );
#endif    
    }

#endif
