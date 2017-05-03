#ifndef _BOXCTRL_HXX
#define _BXOCTRL_HXX

// Box Control Information
#if ( FDATA == 1 )
#define GWL_BOXPTR 0
#define cbBoxExtra ( GWL_BOXPTR + sizeof( DWORD ) )
#else
#define GWW_BOXPTR 0
#define cbBoxExtra ( GWW_BOXPTR + sizeof( WORD ) )
#endif

#define BOX_SETTEXTCOLOR ( WM_USER + 0 )
#define BOX_SETWEIGHT    ( WM_USER + 1 )

typedef struct tagBOXSTRUCT
    {
    RECT     rc;
    COLORREF clr;
    HFONT    hfont;
    }
BOXSTRUCT;

extern "C"
    {
    LONG FAR PASCAL _export BoxWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );
    };
BOOL BoxCtrlInit( );

#if ( FDATA == 1 )
extern const CHAR FAR _szBoxClass[];
#else
extern const CHAR _szBoxClass[];
#endif

#endif

