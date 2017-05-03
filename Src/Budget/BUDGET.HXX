#ifndef _BUDGET_HXX
#define _BUDGET_HXX

// include files
#include "doc.hxx"

// function declarations
extern "C"
    {
    LONG FAR PASCAL _export FrameWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    LONG FAR PASCAL _export BudgetWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    LONG FAR PASCAL _export SummaryWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export CloseEnumProc( HWND hwnd, LONG lParam );
    };

struct ConfirmStruct
{
    enum Type { Delete, Balance };

    ConfirmStruct( Type t, long seq )
        :t_( t ), seq_( seq ) { }

    Type t_;
    long seq_;
};

// confirmation values
#define cfNo    0
#define cfYes   1
#define cfError 2

// Windows Resources
extern HPEN   _hpenDkGray;
extern HPEN   _hpenLtGray;
extern HPEN   _hpenGray;
extern HBRUSH _hbrFill;
extern HFONT  _hfontNormal;
extern HFONT  _hfontBold;
BOOL CreateStockObjects( HWND hwnd );
void DestroyStockObjects( );

// Helper functions
int  Confirm( HWND hwnd, ConfirmStruct::Type t, long seq );
void OpenWindow( const CHAR *pszAccount );
void OpenSummaryWindow( );
void SyncTitle( );
BOOL SaveQuery( HWND hwnd );
BOOL SaveDocument( HWND hwnd );
BOOL Save( HWND hwnd );
BOOL SaveAs( HWND hwnd );
void Open( HWND hwnd );
void UpdateAllViews( );
BOOL FCursorInButton( HWND hwndButton );
VOID DrawBitmap( HDC hdc, HBITMAP hBitmap, short xStart, short yStart );
void DestroyAllChildren( );
int  LoEnglishToDeviceX( HDC hdc, int dxLoEnglish );
int  LoEnglishToDeviceY( HDC hdc, int dyLoEnglish );
void ResourcesMessageBox( HWND hwnd );
void ExecuteMacro( HWND  hwnd, 
                   const CHAR *pszMacroName, 
                   const CHAR *pszMacroPath,
                   const QTime &tDate,
                   const CHAR *pszDesc );
void Program( FILE *pfile, const CHAR *pszMacroName, const QTime &tDate, const CHAR *pszDesc );
void Statement( FILE *pfile, const CHAR *pszMacroName, const QTime &tDate, const CHAR *pszDesc, CHAR *szToken );
void ReadToken( FILE *pfile, CHAR *szToken );
#define nLogUnitPerLogInch ( 100 ) // for MM_LOENGLISH

// Define some constants
// NB: these guys do not include the NULL terminator
#define cbAccountMax  ( 16 )      // Maximum length of an account name
#define cbTokenMax    cbAccountMax// Maximum token length
#define cbDateMax     ( 16 )      // Maximum date length (when entering)
#define cbAmountMax   ( 32 )      // Maximum amount length
#define cbDescMax     ( 128 )     // Maximum description
#define cbBufferMax   ( 128 )     // Generic buffer maximum
#define cbPathMax     ( 128 )     // maximum file name length
#define cbTitleMax    ( 128 )     // maximum title length
#define cbCritical    ( 1024 )    // critical resources area
#define cbTransactionsFormatMax ( 64 )

// constant strings
#if ( FDATA == 1 )
extern const CHAR FAR _szAppName[];
extern const CHAR FAR _szClient[];
extern const CHAR FAR _szEmpty[];
extern const CHAR FAR _szGeneral[];
extern const CHAR FAR _szTransactionsFormat[];
extern const CHAR FAR _szOperationFailed[];
extern const CHAR FAR _szBudgetClass[];
extern const CHAR FAR _szFrameClass[];
extern const CHAR FAR _szUntitled[];
#else
extern const CHAR _szAppName[];
extern const CHAR _szClient[];
extern const CHAR _szEmpty[];
extern const CHAR _szGeneral[];
extern const CHAR _szTransactionsFormat[];
extern const CHAR _szOperationFailed[];
extern const CHAR _szBudgetClass[];
extern const CHAR _szFrameClass[];
extern const CHAR _szUntitled[];
#endif

extern HANDLE _hInstance;
extern HWND   _hwndFrame;
extern HWND   _hwndClient;
extern HWND   _hDlgModeless;
extern HMENU  _hmenu;       
extern HMENU  _hmenuWindow; 
extern HANDLE _hAccel;
extern BOOL   _fCriticalResources;

#ifdef DEBUG
extern int _nTraceIdm;      // trace level IDM
#endif

extern QDocument *_pdoc;
extern QVersion _vBudget;

#define nWindowMenu 2 // position of window menu
#define dxButton   32 // width of a button
#define dyButton   32 // height of a button
#define dyToolBar  ( 3 * dyButton / 2 )

// define some colours
#define clrGray    RGB( 0x80, 0x80, 0x80 )
#define clrLtGray  RGB( 0xC0, 0xC0, 0xC0 )
#define clrVLtGray RGB( 0xE0, 0xE0, 0xE0 )
#define clrDkGray  RGB( 0x40, 0x40, 0x40 )
#define clrWhite   RGB( 0xFF, 0xFF, 0xFF )
#define clrBlack   RGB( 0x00, 0x00, 0x00 )
#define clrBlue    RGB( 0x00, 0x00, 0xFF )
#define clrYellow  RGB( 0xFF, 0xFF, 0x00 )
#define clrRed     RGB( 0xFF, 0x00, 0x00 )
#define clrGreen   RGB( 0x00, 0xFF, 0x00 )

#define exParse ( exUser + 0 )

class MenuController
{
public:
    static void setEditTransactionsMode( BOOL mode )
    {
        editTransactionsMode_ = mode;
        updateMenus( );
    }
    static void updateMenus( );

private:
    static BOOL editTransactionsMode_;
};

#endif // _BUDGET_HXX
