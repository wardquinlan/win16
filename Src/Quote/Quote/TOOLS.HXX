//////////////////////////////////////////////////////////////////////////////
// TOOLS.HXX
//
// Tools include file.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _TOOLS_HXX
#define _TOOLS_HXX

#include <windowsx.h>

extern BOOL FExtractEditValue( HWND hwnd, CHAR *pszEdit, int cbEdit );
extern BOOL ComboDeleteString( HWND hwndCombo, const CHAR *psz );
extern BOOL ComboSelectString( HWND hwndCombo, const CHAR *psz );
extern void ComboAddColors( HWND hwndCombo );
extern void ComboAddTemplates( HWND hwndCombo );
extern void ComboAddConditions( HWND hwndCombo, BOOL fAddNullCond = FALSE );
extern void ComboAddRules( HWND hwndCombo, BOOL fAddNullRule = FALSE );
extern void ComboAddWLs( HWND hwndCombo );
extern void UpdateMenu( );
extern void ReadText( FILE *pfile, CHAR *psz, USHORT cb, BOOL *pfEOF );
extern void WriteText( FILE *pfile, CHAR *psz );
extern CHAR *GetChar( CHAR *pszBuffer, const CHAR *pszDelimiters );
extern BOOL DosAccess( const CHAR *szPathName );
extern void UpdateName( );
extern void ClearName( );
extern int  MsgBox( HWND hwnd, USHORT ids, USHORT mb = MB_ICONEXCLAMATION );
extern BOOL ValidName( const CHAR *pszName );
extern void ComboAddExclude( HWND hwndExclude, const QIndicatorDictionary &dict );
extern void ComboAddInclude( HWND hwndInclude, const QIndicatorDictionary &dict );
extern void EraseOldLine( HWND hwnd );
extern BOOL SafeDeleteObject( HANDLE h );
extern const CHAR *GetString( USHORT idResource );
extern BOOL FCursorInButton( HWND hwndButton );
extern VOID DrawBitmap( HDC hdc, HBITMAP hBitmap, short xStart, short yStart );
extern void DrawColors( LPDRAWITEMSTRUCT lpdis );
extern int  AdjustCentury( int year );
extern QWindowLayout *GetWindowLayout( HWND hwnd );
extern BOOL LayoutInUse( const CHAR *pszLayout );
extern int ComboFindString( HWND hwndCombo, const CHAR *psz );
extern int GetCBText( HWND hwndCombo, CHAR *psz, int cbMax, int nIndex );
extern int GetText( HWND hwnd, CHAR *psz, int cbMax );
extern void ComboAddConstants( HWND hwndCombo, BOOL fAddNullConst = FALSE );
extern const CHAR *RemoveCR( const CHAR *psz );
extern BOOL ViewFile( HWND hwnd, const CHAR *pszFile );

#ifdef DEBUG
extern void SetTracing( int nIdm, int nTraceLevel );
#endif

inline int SetText( HWND hwnd, const CHAR *psz )
       { return (int) SendMessage( hwnd, WM_SETTEXT, 0, (LPARAM)(LPSTR) psz ); }
       
inline VOID SelectEdit( HWND hwndEdit ) 
       { SendMessage( hwndEdit, EM_SETSEL, 0, MAKELPARAM( 0, -1 ) ); }

inline VOID SelectCBEdit( HWND hwndCB ) 
       { SendMessage( hwndCB, CB_SETEDITSEL, 0, MAKELPARAM( 0, -1 ) ); }

inline VOID ClearEdit( HWND hwndEdit )
       { SetWindowText( hwndEdit, (LPSTR) GetString( IDS_EMPTY ) ); }

inline BOOL IsEven( UINT n )
    {
    return ( ( n % 2 ) == 0 );
    }
    
inline BOOL IsOdd( UINT n )
    {
    return ( ( n % 2 ) == 1 );
    }

inline NUM num2Percent( NUM num )
    {
    return num * (NUM) 100.0;
    }

#endif

