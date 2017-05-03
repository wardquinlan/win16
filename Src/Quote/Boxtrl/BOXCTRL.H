#ifndef _BOXCTRL_H

#define _BOXCTRL_H

//////////////////
// Box Messages //
//////////////////

// This message is to set the color of the box control text
// SendMessage( hwndBox, BOX_SETTEXTCOLOR, 0, (LPARAM) clr );
// Users may also use the provided macro.
#define BOX_TEXTCOLOR ( WM_USER + 0 )
#define SetBoxColor( hwnd, clr ) ( SendMessage( ( hwnd ),                    \
                                                BOX_TEXTCOLOR,               \
                                                (WPARAM) 0,                  \
                                                (LPARAM) (clr) ) )

// This message is to set the weight of the box control text
// SendMessage( hwndBox, BOX_SETWEIGHT, (WPARAM) wt, 0L );
// where wt is one of { BOX_THIN, BOX_NORMAL, BOX_BOLD }
// Users may also use the provided macro.
#define BOX_WEIGHT       ( WM_USER + 1 )
#define BOX_THIN         ( FW_THIN )
#define BOX_NORMAL       ( FW_NORMAL )
#define BOX_BOLD         ( FW_BOLD )
#define SetBoxWeight( hwnd, wt ) ( SendMessage( ( hwnd ),                    \
                                                BOX_WEIGHT,                  \
                                                (WPARAM) ( wt ),             \
                                                (LPARAM) 0 ) )

// This message is to set the text justification attributes of
// the box control.
// SendMessage( hwndBox, BOX_JUSTIFY, (WPARAM) jst, 0L );
// jst is one of: { BOX_LEFT, BOX_CENTER, BOX_RIGHT }
#define BOX_JUSTIFY    ( WM_USER + 2 )
#define BOX_LEFT       ( DT_LEFT )
#define BOX_CENTER     ( DT_CENTER )
#define BOX_RIGHT      ( DT_RIGHT )
#define SetBoxJustify( hwnd, jst ) ( SendMessage( ( hwnd ),                  \
                                     BOX_JUSTIFY,                            \
                                     (WPARAM) ( jst ),                       \
                                     (LPARAM) 0 ) )

// This message is to set the box state: 'frame', 'inside' or 'outside'.
// SendMessage( hwndBox, BOX_STATE, (WPARAM) st, 0L );
#define BOX_STATE   ( WM_USER + 3 )
#define BOX_FRAME   ( 0 )
#define BOX_INSIDE  ( 1 )
#define BOX_OUTSIDE ( 2 )
#define SetBoxState( hwnd, st ) ( SendMessage( ( hwnd ),                     \
                                  BOX_STATE,                                 \
                                  (WPARAM) ( st ),                           \
                                  (LPARAM) 0 ) )

// This message is to set the font height.  The font 
// height will be in device units.  There is no
// associated style with this.
#define BOX_FONTHEIGHT (WM_USER + 4 )
#define SetBoxFontHeight( hwnd, dy ) ( SendMessage( ( hwnd ),                \
                                       BOX_FONTHEIGHT,                       \
                                       (WPARAM) ( dy ),                      \
                                       (LPARAM) 0 ) )

////////////////
// Box Styles //
////////////////

// Default Box Styles
#define BOXS_NORMAL  0x0000L
#define BOXS_LEFT    0x0000L
#define BOXS_FRAME   0x0000L

// Optional Box Styles
#define BOXS_THIN    0x0001L
#define BOXS_BOLD    0x0002L
#define BOXS_CENTER  0x0004L
#define BOXS_RIGHT   0x0008L
#define BOXS_INSIDE  0x0010L
#define BOXS_OUTSIDE 0x0020L

#ifdef __cplusplus
extern "C"
{
#endif

// The Box Control Window Procedure
LONG FAR PASCAL _export BoxWndProc( HWND hwnd, UINT msg, UINT wParam, LONG lParam );

// Every module, directly or indirectly, which uses the Box Control
// must initialize it at start up time.
BOOL FAR PASCAL _export BoxCtrlInit( void );

// Windows Exit Procedure
int  FAR PASCAL _export WEP( int );

#ifdef __cplusplus
};
#endif

#endif

