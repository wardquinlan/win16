#ifndef _DLG_HXX
#define _DLG_HXX

extern "C"
{
BOOL FAR PASCAL _export AboutProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export CreateProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export TransactionsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export NewWindowProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export DeleteProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export XFerProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export MacroProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export BalanceProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export ConfirmProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
}

// Some Edit Window helper routines

///////////////////////////////////
// GetText( HWND hwnd, const CHAR *psz, int cbMax )
//
// Gets the text associated with hwnd.
///////////////////////////////////
inline GetText( HWND hwnd, const CHAR *psz, int cbMax )
       { return (int) SendMessage( hwnd, WM_GETTEXT, cbMax, (LPARAM)(LPSTR) psz ); }

///////////////////////////////////
// SelectEdit( HWND hwndEdit )
//
// Selects the entire contents of an edit box
///////////////////////////////////
inline VOID SelectEdit( HWND hwndEdit ) 
       { SendMessage( hwndEdit, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) ); }

///////////////////////////////////
// ClearEditBox( HWND hwndEdit )
//
// Clears an entire edit box
///////////////////////////////////
inline VOID ClearEditBox( HWND hwndEdit )
       { SetWindowText( hwndEdit, (LPSTR) _szEmpty ); }

///////////////////////////////////
// FExtractEditValue( HWND hwnd, CHAR *pszEdit, int cbEdit )
//
// Clears an entire edit box
// cbEdit: max size to copy (including the NULL-terminator)
///////////////////////////////////
BOOL FExtractEditValue( HWND hwnd, CHAR *pszEdit, int cbEdit );

///////////////////////////////////
// FParseDate( CHAR *szDate, int *pmonth, int *pday, int *pyear )
//
// Parses a date separated by the '/' character.
BOOL FParseDate( CHAR *szDate, int *pmonth, int *pday, int *pyear );

///////////////////////////////////
// DecimalAmount( CHAR *pszAmount, LONG *plAmount )
//
// returns the amount, in cents, of pszAmount
BOOL DecimalAmount( CHAR *pszAmount, LONG *plAmount );

#endif
