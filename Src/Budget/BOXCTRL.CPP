#include <qobject.hxx>
// Must include budget.hxx to get GDI objects, colors, etc.
#include "budget.hxx"
#include "boxctrl.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

const CHAR _szBoxClass[] = "boxctrl";

BOOL BoxCtrlInit( )
    {
    WNDCLASS wc;

    // register the box control window classe
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = BoxWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = cbBoxExtra;
    wc.hInstance     = _hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( LTGRAY_BRUSH );
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _szBoxClass;
    
    if ( !RegisterClass( &wc ) )
        {
        return FALSE;
        }
    return TRUE;
    }

LONG FAR PASCAL _export BoxWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam )
    {
    HDC hdc;
    PAINTSTRUCT ps;
    BOXSTRUCT *pbs = NULL;
    BOOL fOk;
    CHAR szBuffer[ cbBufferMax + 1 ];

    if ( msg != WM_CREATE )
        {
#if ( FDATA == 1 )
        pbs = (BOXSTRUCT *) GetWindowLong( hwnd, GWL_BOXPTR );
#else
        pbs = (BOXSTRUCT *) GetWindowWord( hwnd, GWW_BOXPTR );
#endif
        }

    switch( msg )
        {
    case WM_CREATE:
        fOk = FALSE;
        TASKLIST
            {
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_BOXPTR, (DWORD) 0 );
#else
            SetWindowWord( hwnd, GWW_BOXPTR, (WORD) 0 );
#endif
            pbs = new BOXSTRUCT;
            if ( !pbs )
                {
                pbs = new BOXSTRUCT;
                if ( !pbs )
                    {
                    QUIT;
                    }
                }
            lmemset( pbs, 0, sizeof( BOXSTRUCT ) );
#if ( FDATA == 1 )
            SetWindowLong( hwnd, GWL_BOXPTR, (DWORD) pbs );
#else
            SetWindowWord( hwnd, GWW_BOXPTR, (WORD) pbs );
#endif
            fOk = TRUE;
            }
        ENDTASKLIST
        if ( fOk )
            {
            SendMessage( hwnd, BOX_SETTEXTCOLOR, 0, clrBlack );
            SendMessage( hwnd, BOX_SETWEIGHT, FW_NORMAL, 0L );
            }
        else
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            }
        return 0L;
        
    case WM_DESTROY:
        delete pbs;
        return 0L;

    case WM_SIZE:
        if ( pbs )
            {
            GetClientRect( hwnd, &pbs->rc );
            }
        return 0L;

    case WM_PAINT:
        hdc = BeginPaint( hwnd, &ps );
        if ( pbs && !_fCriticalResources )
            {
            SelectObject( hdc, _hpenDkGray );
            SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
            Rectangle( hdc, pbs->rc.left, pbs->rc.top, pbs->rc.right, pbs->rc.bottom );
            SelectObject( hdc, GetStockObject( WHITE_PEN ) );
            MoveTo( hdc, pbs->rc.left,  pbs->rc.top );
            LineTo( hdc, pbs->rc.right, pbs->rc.top );
            MoveTo( hdc, pbs->rc.left,  pbs->rc.top );
            LineTo( hdc, pbs->rc.left,  pbs->rc.bottom );
            GetWindowText( hwnd, szBuffer, sizeof( szBuffer ) );
            SelectObject( hdc, pbs->hfont );
            SetBkMode( hdc, TRANSPARENT );
            SetTextColor( hdc, pbs->clr );
            DrawText( hdc, szBuffer, lstrlen( szBuffer ), &pbs->rc, 
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE );
            }
        EndPaint( hwnd, &ps );
        return 0L;

    case BOX_SETTEXTCOLOR:
        if ( pbs )
            {
            pbs->clr = (COLORREF) lParam;
            }
        return 0L;

    case BOX_SETWEIGHT:
        if ( pbs )
            {
            if ( wParam == FW_NORMAL )
                {
                pbs->hfont = _hfontNormal;
                }
            else if ( wParam == FW_BOLD )
                {
                pbs->hfont = _hfontBold;
                }
            else
                {
                ASSERT( FALSE );
                }
            }
        return 0L;
        }
    
    return DefWindowProc( hwnd, msg, wParam, lParam );
    }



