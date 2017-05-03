#include <windows.h>
#include <memory.h>
#include "boxctrl.h"

////////////////////////////////////////
// General type definitions
typedef char CHAR;
typedef unsigned long ULONG;
#define TASKLIST    do
#define ENDTASKLIST while ( 0 );
#define QUIT        break
#define QUITIF( f ) if ( f ) break;

////////////////////////////////////////
// Constants
#define clrBlack  RGB( 0x00, 0x00, 0x00 )
#define clrDkGray RGB( 0x40, 0x40, 0x40 )
#define cbBufferMax 128

////////////////////////////////////////
// Box Control Information
typedef struct tagBOXSTRUCT
    {
    RECT     rc;
    COLORREF clr;
    int      weight;
    int      jst;
    int      st;
    int      dyFont;
    }
BOXSTRUCT, NEAR *NPBOXSTRUCT, FAR *LPBOXSTRUCT;
#define GWW_BOXPTR ( 0 )
#define cbBoxExtra ( GWW_BOXPTR + sizeof( WORD ) )

#define dyFontDefault ( 18 )            // Maximum font height (log. units)

////////////////////////////////////////
// Helper Macros
#define MIAlloc( cb )   ( LocalAlloc( LMEM_FIXED, ( UINT ) cb ) )
#define MIFree( pb )    ( LocalFree( ( HLOCAL ) pb ) )

////////////////////////////////////////
// Global Variables
static const CHAR _szBoxClass[] = "boxctrl";
static HINSTANCE _hInstance = NULL;

int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine )
    {
    WNDCLASS wc;

    if ( wHeapSize > 0 )
        {
        UnlockData( 0 );
        }

    if ( _hInstance == NULL )
        {
        // register the box control window classe
        wc.style         = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
        wc.lpfnWndProc   = BoxWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = cbBoxExtra;
        wc.hInstance     = hInstance;
        wc.hIcon         = NULL;
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = _szBoxClass;
        if ( !RegisterClass( &wc ) )
            {
            return FALSE;
            }
        _hInstance = hInstance;
        }
    
    return TRUE;
    }

int FAR PASCAL _export WEP( int nParam )
    {
    _hInstance = NULL;
    return TRUE;
    }

LONG FAR PASCAL _export BoxWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HDC         hdc;
    PAINTSTRUCT ps;
    NPBOXSTRUCT npbs = NULL;
    BOOL        fOk;
    CHAR        szBuffer[ cbBufferMax + 1 ];
    HPEN        hpen;
    HFONT       hfont;
    RECT        rc;
    int         dyFont;
    static LOGFONT lf;
    LPCREATESTRUCT lpcs;
    
    if ( msg != WM_CREATE )
        {
        npbs = (NPBOXSTRUCT) GetWindowWord( hwnd, GWW_BOXPTR );
        }

    switch( msg )
        {
    case WM_CREATE:
        fOk = FALSE;
        TASKLIST
            {
            SetWindowWord( hwnd, GWW_BOXPTR, (WORD) 0 );
            npbs = (NPBOXSTRUCT) MIAlloc( sizeof( BOXSTRUCT ) );
            if ( npbs == NULL )
                {
                QUIT;
                }
            memset( npbs, 0, sizeof( *npbs ) );
            SetWindowWord( hwnd, GWW_BOXPTR, (WORD) npbs );
            fOk = TRUE;
            }
        ENDTASKLIST
        if ( fOk )
            {
            lpcs = (LPCREATESTRUCT) lParam;
            SetBoxColor( hwnd, clrBlack );
            if ( lpcs->style & BOXS_THIN )
                {
                SetBoxWeight( hwnd, BOX_THIN );
                }
            else if ( lpcs->style & BOXS_BOLD )
                {
                SetBoxWeight( hwnd, BOX_BOLD );
                }
            else
                {
                SetBoxWeight( hwnd, BOX_NORMAL );
                }

            if ( lpcs->style & BOXS_CENTER )
                {
                SetBoxJustify( hwnd, BOX_CENTER );
                }
            else if ( lpcs->style & BOXS_RIGHT )
                {
                SetBoxJustify( hwnd, BOX_RIGHT );
                }
            else
                {
                SetBoxJustify( hwnd, BOX_LEFT );
                }
            
            if ( lpcs->style & BOXS_INSIDE )
                {
                SetBoxState( hwnd, BOX_INSIDE );
                }
            else if ( lpcs->style & BOXS_OUTSIDE )
                {
                SetBoxState( hwnd, BOX_OUTSIDE );
                }
            else
                {
                SetBoxState( hwnd, BOX_FRAME );
                }
            SetBoxFontHeight( hwnd, dyFontDefault );
            }
        return (LRESULT) fOk;
        
    case WM_DESTROY:
        MIFree( npbs );
        npbs = NULL;
        return (LRESULT) TRUE;

    case WM_SIZE:
        if ( npbs )
            {
            GetClientRect( hwnd, &npbs->rc );
            }
        return (LRESULT) TRUE;

    case WM_PAINT:
        GetClientRect( hwnd, &rc );
        dyFont = npbs->dyFont;
        if ( dyFont > ( rc.bottom - rc.top ) )
            {
            dyFont = ( rc.bottom - rc.top );
            }

        memset( &lf, 0, sizeof( lf ) );
        hpen = CreatePen( PS_SOLID, 0, clrDkGray );
        lf.lfWeight         = npbs->weight;
        lf.lfHeight         = dyFont;
        lstrcpy( lf.lfFaceName, "Courier New" );
        hfont = CreateFontIndirect( &lf );

        hdc = BeginPaint( hwnd, &ps );

        if ( npbs->st == BOX_INSIDE )
            {
            SelectObject( hdc, GetStockObject( WHITE_PEN ) );
            SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
            Rectangle( hdc, npbs->rc.left, npbs->rc.top, npbs->rc.right, npbs->rc.bottom );
            SelectObject( hdc, hpen );
            }
        else if ( npbs->st == BOX_OUTSIDE )
            {
            SelectObject( hdc, hpen );
            SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
            Rectangle( hdc, npbs->rc.left, npbs->rc.top, npbs->rc.right, npbs->rc.bottom );
            SelectObject( hdc, GetStockObject( WHITE_PEN ) );
            }
        else
            {
            // Frame mode
            SelectObject( hdc, hpen );
            SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
            Rectangle( hdc, npbs->rc.left, npbs->rc.top, npbs->rc.right, npbs->rc.bottom );
            }
        if ( npbs->st != BOX_FRAME )
            {
            MoveTo( hdc, npbs->rc.left,  npbs->rc.top );
            LineTo( hdc, npbs->rc.right, npbs->rc.top );
            MoveTo( hdc, npbs->rc.left,  npbs->rc.top );
            LineTo( hdc, npbs->rc.left,  npbs->rc.bottom );
            }
        GetWindowText( hwnd, szBuffer, sizeof( szBuffer ) );
        SelectObject( hdc, hfont );
        SetBkMode( hdc, TRANSPARENT );
        SetTextColor( hdc, npbs->clr );
        DrawText( hdc, szBuffer, lstrlen( szBuffer ), &npbs->rc, 
                  npbs->jst | DT_VCENTER | DT_SINGLELINE );
        EndPaint( hwnd, &ps );

        DeleteObject( hpen );
        DeleteObject( hfont );
        return (LRESULT) TRUE;

    case BOX_TEXTCOLOR:
        if ( npbs )
            {
            npbs->clr = (COLORREF) lParam;
            }
        return (LRESULT) TRUE;

    case BOX_WEIGHT:
        fOk = TRUE;
        if ( npbs )
            {
            if ( wParam == BOX_THIN )
                {
                npbs->weight = FW_THIN;
                }
            else if ( wParam == BOX_NORMAL )
                {
                npbs->weight = FW_NORMAL;
                }
            else if ( wParam == BOX_BOLD )
                {
                npbs->weight = FW_BOLD;
                }
            else
                {
                fOk = FALSE;
                }
            }
        return (LRESULT) fOk;

    case BOX_JUSTIFY:
        fOk = TRUE;
        if ( npbs )
            {
            if ( wParam == BOX_LEFT )
                {
                npbs->jst = DT_LEFT;
                }
            else if ( wParam == BOX_CENTER )
                {
                npbs->jst = DT_CENTER;
                }
            else if ( wParam == BOX_RIGHT )
                {
                npbs->jst = DT_RIGHT;
                }
            else
                {
                fOk = FALSE;
                }
            }
        return (LRESULT) fOk;

    case BOX_STATE:
        fOk = TRUE;
        if ( npbs )
            {
            if ( wParam == BOX_INSIDE )
                {
                npbs->st = BOX_INSIDE;
                }
            else if ( wParam == BOX_OUTSIDE )
                {
                npbs->st = BOX_OUTSIDE;
                }
            else
                {
                fOk = FALSE;
                }
            }
        return (LRESULT) fOk;

    case BOX_FONTHEIGHT:
        dyFont = (int) wParam;
        if ( dyFont > 0 )
            {
            npbs->dyFont = dyFont;
            InvalidateRect( hwnd, NULL, TRUE );
            fOk = TRUE;
            }
        else
            {
            fOk = FALSE;
            }
        return (LRESULT) fOk;
        }
    
    return DefWindowProc( hwnd, msg, wParam, lParam );
    }

// Why, you ask?  If we don't force the user
// to call a routine in the DLL, the DLL
// LibMain won't be called, because the linker
// links the module away.  This can be
// very bad if the DLL is used as a
// control in a resources script
BOOL FAR PASCAL _export BoxCtrlInit( void )
    {
    return TRUE;
    }


