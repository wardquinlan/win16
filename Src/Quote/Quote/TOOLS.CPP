//////////////////////////////////////////////////////////////////////////////
// TOOLS.CPP
//
// Tools helper routines.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <qobject.hxx>
#include <statpak.h>
#include <ctype.h>
#include "resource.hxx"
#include "types.hxx"
#include "template.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "tools.hxx"
#include "perfeng.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

extern NVPairList *_psummary;
extern NVPairList *_psummarySim;

static BOOL EmptyLine( const CHAR *pszLine );

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// SetTracing( )
//
// This routine sets the QObject DEBUG tracing level.
//////////////////////////////////////////////////////////////////////////////
extern void SetTracing( int nIdm, int nTraceLevel )
    {
    CheckMenuItem( _hmenu, _nTraceIdm, MF_BYCOMMAND | MF_UNCHECKED );
    _nTraceIdm = nIdm;
    CheckMenuItem( _hmenu, _nTraceIdm, MF_BYCOMMAND | MF_CHECKED ); 
    SetTraceLevel( nTraceLevel );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// RemoveCR( )
//
// Replaces carriage returns with blanks
//////////////////////////////////////////////////////////////////////////////
extern const CHAR *RemoveCR( const CHAR *psz )
    {
    static CHAR szBuffer[ _cbTextMax + 1 ];
    ASSERT( lstrlen( psz ) <= _cbTextMax );
    lmemset( szBuffer, 0, sizeof( szBuffer ) );
    lstrcpy( szBuffer, psz );

    for ( CHAR *pch = szBuffer; *pch; pch++ )
        if ( isspace( *pch ) )
            *pch = ' ';

    return szBuffer;
    }

//////////////////////////////////////////////////////////////////////////////
// GetText( )
//
// Gets text associated with hwnd.
//////////////////////////////////////////////////////////////////////////////
extern int GetText( HWND hwnd, CHAR *psz, int cbMax )
    {
    int n = (int) SendMessage( hwnd, WM_GETTEXT, cbMax, (LPARAM)(LPSTR) psz );
    if ( !n )
        {
        *psz = '\0';
        }
    return n;
    }

//////////////////////////////////////////////////////////////////////////////
// GetString( )
//
// This routine retrieves the string resource from the executable.
//////////////////////////////////////////////////////////////////////////////
extern const CHAR *GetString( USHORT idResource )
    {
    static CHAR sz[ _cbBufferMax + 1 ];

    LoadString( _hInstance, idResource, sz, sizeof( sz ) );
    return sz;
    }

//////////////////////////////////////////////////////////////////////////////
// FCursorInButton( )
//
// This routine returns TRUE if the cursor is within the bounding
// rectangle of hwndButton.
//////////////////////////////////////////////////////////////////////////////
extern BOOL FCursorInButton( HWND hwndButton )
    {
    RECT rcButton;
    POINT ptCursor;

    GetWindowRect( hwndButton, &rcButton );
    GetCursorPos( &ptCursor );
    return PtInRect( &rcButton, ptCursor );
    }

//////////////////////////////////////////////////////////////////////////////
// DrawColors( )
//
// This routine draws the owner-draw listbox for color-selection.
//////////////////////////////////////////////////////////////////////////////
extern void DrawColors( LPDRAWITEMSTRUCT lpdis )
    {
    HGDIOBJ  hobj;
    HBRUSH   hbr;
    LOGBRUSH lb;

    ASSERT( lpdis->itemID >=0 && lpdis->itemID < RGCLRSIZE );
    memset( &lb, 0, sizeof( lb ) );
    lb.lbStyle = BS_SOLID;
    lb.lbColor = _rgclr[ lpdis->itemID ];
    hbr = CreateBrushIndirect( &lb );
    ASSERT( hbr != NULL );
    hobj = SelectObject( lpdis->hDC, hbr );
    Rectangle( lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top,
                           lpdis->rcItem.right, lpdis->rcItem.bottom );
    SelectObject( lpdis->hDC, hobj );
    DeleteObject( hbr );
    }

//////////////////////////////////////////////////////////////////////////////
// DrawBitmap( )
//
// This routine draws a bitmap.
//////////////////////////////////////////////////////////////////////////////
extern void DrawBitmap( HDC hdc, HBITMAP hBitmap, short xStart, short yStart )
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

//////////////////////////////////////////////////////////////////////////////
// FExtractEditValue( )
//
// This routine retrieves edit string from the edit box hwnd.
//////////////////////////////////////////////////////////////////////////////
extern BOOL FExtractEditValue( HWND hwnd, CHAR *pszEdit, int cbEdit )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {
        SendMessage( hwnd, WM_GETTEXT, cbEdit, (LONG)(LPSTR) pszEdit );
        if ( !*pszEdit )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }
    
//////////////////////////////////////////////////////////////////////////////
// ComboDeleteString( )
//
// This routine deletes the specified string from the given combo box.
//////////////////////////////////////////////////////////////////////////////
extern BOOL ComboDeleteString( HWND hwndCombo, const CHAR *psz )
    {
    int n;
    n = ComboFindString( hwndCombo, psz );
    if ( n == CB_ERR )
        {
        return FALSE;
        }
    n = (int) SendMessage( hwndCombo, CB_DELETESTRING, n, 0L );
    ASSERT( n != CB_ERR );
    SendMessage( hwndCombo, CB_SETCURSEL, 0, 0L );
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// ComboSelectString( )
//
// This routine selects an index within a combo box.
//////////////////////////////////////////////////////////////////////////////
extern BOOL ComboSelectString( HWND hwndCombo, const CHAR *psz )
    {
    int n = ComboFindString( hwndCombo, psz );
    if ( n == CB_ERR )
        {
        return FALSE;
        }
    n = (int) SendMessage( hwndCombo, CB_SETCURSEL, (WPARAM) n, 0L );
    ASSERT( n != CB_ERR );
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddColors( )
//
// This routine adds all colors in the color table to the combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddColors( HWND hwndCombo )
    {
    SendMessage( hwndCombo, CB_RESETCONTENT, 0, 0L );
    for ( int i = 0; i < RGCLRSIZE; i++ )
        {
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_DUMMY ) );
        }
    ASSERT( SendMessage( hwndCombo, CB_GETCOUNT, 0, 0L ) == RGCLRSIZE );
    SendMessage( hwndCombo, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddTemplates( )
//
// This routine adds all indicator template names to a combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddTemplates( HWND hwndCombo )
    {
    QIndicatorTemplate *pi;

    SendMessage( hwndCombo, CB_RESETCONTENT, 0, 0L );
    for ( pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetHead( );
          pi;
          pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetNext( pi ) )
        {
        ASSERT( pi->GetName( ) != NULL );
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) pi->GetName( ) );
        }
    SendMessage( hwndCombo, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddConstants( )
//
// This routine adds all constants to a combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddConstants( HWND hwndCombo, BOOL fAddNullConst )
    {
    QConstant *pconst;

    SendMessage( hwndCombo, CB_RESETCONTENT, 0, 0L );
    for ( pconst = (QConstant *) _ptpl->m_constlist.GetHead( );
          pconst;
          pconst = (QConstant *) _ptpl->m_constlist.GetNext( pconst ) )
        {
        ASSERT( pconst->GetName( ) != NULL );
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) pconst->GetName( ) );
        }
    if ( fAddNullConst )
        {
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_NONE ) );
        }
    SendMessage( hwndCombo, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddConditions( )
//
// This routine adds all conditions to a combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddConditions( HWND hwndCombo, BOOL fAddNullCond )
    {
    QCondition *pcond;

    SendMessage( hwndCombo, CB_RESETCONTENT, 0, 0L );
    for ( pcond = (QCondition *) _ptpl->m_condlist.GetHead( );
          pcond;
          pcond = (QCondition *) _ptpl->m_condlist.GetNext( pcond ) )
        {
        ASSERT( pcond->GetName( ) != NULL );
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) pcond->GetName( ) );
        }
    if ( fAddNullCond )
        {
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_NONE ) );
        }
    SendMessage( hwndCombo, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddRules( )
//
// This routine adds all rule names to a combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddRules( HWND hwndCombo, BOOL fAddNullRule )
    {
    QRule *prule;
    SendMessage( hwndCombo, CB_RESETCONTENT, 0, 0L );
    for ( prule = (QRule *) _ptpl->m_rulelist.GetHead( );
          prule;
          prule = (QRule *) _ptpl->m_rulelist.GetNext( prule ) )
        {
        ASSERT( prule->GetName( ) != NULL );
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) prule->GetName( ) );
        }
    if ( fAddNullRule )
        {
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) GetString( IDS_NONE ) );
        }
    SendMessage( hwndCombo, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddWLs( )
//
// This routine adds all window layout names to a combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddWLs( HWND hwndCombo )
    {
    QWindowLayout *pwl;

    SendMessage( hwndCombo, CB_RESETCONTENT, 0, 0L );
    for ( pwl = (QWindowLayout *) _ptpl->m_wllist.GetHead( );
          pwl;
          pwl = (QWindowLayout *) _ptpl->m_wllist.GetNext( pwl ) )
        {
        ASSERT( pwl->GetName( ) != NULL );
        SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)(LPSTR) pwl->GetName( ) );
        }
    SendMessage( hwndCombo, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddExclude( )
//
// This routine adds all indicator templates not found in the dictionary dict
// to a combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddExclude( HWND hwndExclude, const QIndicatorDictionary &dict )
    {
    QIndicatorTemplate *pi;

    SendMessage( hwndExclude, CB_RESETCONTENT, 0, 0L );
    for ( pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetHead( );
          pi;
          pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetNext( pi ) )
        {
        if ( !dict.Find( pi->GetName( ) ) )
            {
            SendMessage( hwndExclude, CB_ADDSTRING, 0, (LPARAM)(LPSTR) pi->GetName( ) );
            }
        }
    SendMessage( hwndExclude, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ComboAddInclude( )
//
// This routine adds all indicator templates found in the dictionary dict
// to a combo box.
//////////////////////////////////////////////////////////////////////////////
extern void ComboAddInclude( HWND hwndInclude, const QIndicatorDictionary &dict )
    {
    QIndicatorTemplate *pi;

    SendMessage( hwndInclude, CB_RESETCONTENT, 0, 0L );
    for ( pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetHead( );
          pi;
          pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetNext( pi ) )
        {
        if ( dict.Find( pi->GetName( ) ) )
            {
            SendMessage( hwndInclude, CB_ADDSTRING, 0, (LPARAM)(LPSTR) pi->GetName( ) );
            }
        }
    SendMessage( hwndInclude, CB_SETCURSEL, 0, 0L );
    }

//////////////////////////////////////////////////////////////////////////////
// ValidName( )
//
// Returns true if the name pszName is valid.
//////////////////////////////////////////////////////////////////////////////
extern BOOL ValidName( const CHAR *pszName )
    {
    BOOL fError;
    const CHAR *pch;
    int ichLast;

    if ( !isalpha( *pszName ) && *pszName != '_' )
        {
        return FALSE;
        }
    
    ichLast = lstrlen( pszName ) - 1;
    ASSERT( ichLast >= 0 );
    
    // Name can't end in an underscore
    if ( *( pszName + ichLast ) == '_' )
        {
        return FALSE;
        }

    fError = FALSE;
    for ( pch = pszName;
          *pch;
          pch++ )
        {
        if ( !isalnum( *pch ) && *pch != '_' )
            {
            fError = TRUE;
            break;
            }
        }
    return !fError;
    }

//////////////////////////////////////////////////////////////////////////////
// UpdateMenu( )
//
// Synchronizes the menu with the internal template/document data structures.
//////////////////////////////////////////////////////////////////////////////
extern void UpdateMenu( )
    {
    USHORT n;
    BOOL   fCheck;

    if ( _pdoc->readOnly( ) )
        n = MF_GRAYED;
    else
        n = MF_ENABLED;

    EnableMenuItem( _hmenu, IDM_SAVE,         MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_SAVEAS,       MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_ADD_QUOTE,    MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_DELETE_QUOTE, MF_BYCOMMAND | n );

    fCheck = _ptpl->m_genopts.GetQuoteBox( );
    CheckMenuItem( _hmenu, IDM_QUOTE_BOX, MF_BYCOMMAND | ( fCheck ? MF_CHECKED : MF_UNCHECKED ) );
    fCheck = _ptpl->m_genopts.GetTracker( );
    CheckMenuItem( _hmenu, IDM_TRACKER, MF_BYCOMMAND | ( fCheck ? MF_CHECKED : MF_UNCHECKED ) );
    fCheck = _ptpl->m_genopts.GetAutoOpenText( );
    CheckMenuItem( _hmenu, IDM_AUTOOPENTEXT, MF_BYCOMMAND | ( fCheck ? MF_CHECKED : MF_UNCHECKED ) );
    
    n = _ptpl->m_rulelist.GetCount( ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_DELETE_RULE,   MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_PERFORMANCE,   MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_CHART_SCANNER, MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_TRADESIM,      MF_BYCOMMAND | n );
    
    n = _ptpl->m_itlist.GetCount( ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_DELETE_IT, MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_INDVAL,    MF_BYCOMMAND | n );

    n = _ptpl->m_condlist.GetCount( ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_DELETE_COND, MF_BYCOMMAND | n );

    n = _ptpl->m_constlist.GetCount( ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_DELETE_CONST, MF_BYCOMMAND | n );

    n = _ptpl->m_wllist.GetCount( ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_DELETE_WL,  MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_NEW_WINDOW, MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_PRINT,      MF_BYCOMMAND | n );

    CheckMenuItem( _hmenu, IDM_DATES_NONE,   MF_BYCOMMAND | MF_UNCHECKED );
    CheckMenuItem( _hmenu, IDM_DATES_MONTHS, MF_BYCOMMAND | MF_UNCHECKED ); 
    CheckMenuItem( _hmenu, IDM_DATES_YEARS,  MF_BYCOMMAND | MF_UNCHECKED );
    switch( _ptpl->m_genopts.GetDateType( ) )
        {
    case dtNone:
        CheckMenuItem( _hmenu, IDM_DATES_NONE,   MF_BYCOMMAND | MF_CHECKED );
        break;
    case dtMonths:
        CheckMenuItem( _hmenu, IDM_DATES_MONTHS, MF_BYCOMMAND | MF_CHECKED );
        break;
    case dtYears:
        CheckMenuItem( _hmenu, IDM_DATES_YEARS,  MF_BYCOMMAND | MF_CHECKED );
        break;
    default:
        ASSERT( FALSE );
        }
    
    CheckMenuItem( _hmenu, IDM_SCR_PAGE,  MF_BYCOMMAND | MF_UNCHECKED );
    CheckMenuItem( _hmenu, IDM_SCR_CHART, MF_BYCOMMAND | MF_UNCHECKED );
    CheckMenuItem( _hmenu, IDM_PRT_PAGE,  MF_BYCOMMAND | MF_UNCHECKED );
    CheckMenuItem( _hmenu, IDM_PRT_CHART, MF_BYCOMMAND | MF_UNCHECKED );

    if ( _ptpl->m_genopts.Scaling( QGeneralOptions::eScreen ) == QGeneralOptions::eByPage )
        CheckMenuItem( _hmenu, IDM_SCR_PAGE,  MF_BYCOMMAND | MF_CHECKED );
    else
        CheckMenuItem( _hmenu, IDM_SCR_CHART, MF_BYCOMMAND | MF_CHECKED );
    
    if ( _ptpl->m_genopts.Scaling( QGeneralOptions::ePrint ) == QGeneralOptions::eByPage )
        CheckMenuItem( _hmenu, IDM_PRT_PAGE,  MF_BYCOMMAND | MF_CHECKED );
    else
        CheckMenuItem( _hmenu, IDM_PRT_CHART, MF_BYCOMMAND | MF_CHECKED );
    
    if ( _ptpl->m_genopts.GetTextGridLines( ) )
        CheckMenuItem( _hmenu, IDM_TEXT_GRID_LINES, MF_BYCOMMAND | MF_CHECKED );
    else
        CheckMenuItem( _hmenu, IDM_TEXT_GRID_LINES, MF_BYCOMMAND | MF_UNCHECKED );

    n = _psummary->GetCount( ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_NEW_TRADELIST, MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_NEW_PERFSUM, MF_BYCOMMAND | n );
    
    n = _psummarySim->GetCount( ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_NEW_STRADELIST, MF_BYCOMMAND | n );
    EnableMenuItem( _hmenu, IDM_NEW_SIMRESULTS, MF_BYCOMMAND | n );

    n = ( _psummary->GetCount( ) + _psummarySim->GetCount( ) ) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem( _hmenu, IDM_CLEAR_PERF_DATA, MF_BYCOMMAND | n );
    }

//////////////////////////////////////////////////////////////////////////////
// WriteText( )
//
// Writes a single text line to a file.
//////////////////////////////////////////////////////////////////////////////
extern void WriteText( FILE *pfile, CHAR *psz )
    {
    QString s( psz );
    TRY
        {
        s += GetString( IDS_RETURN );
        fputs( s, pfile );
        }
    CATCH_ALL
        {
        s.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// EmptyLine( )
//
// Returns TRUE if a text line is empty
//////////////////////////////////////////////////////////////////////////////
static BOOL EmptyLine( const CHAR *pszLine )
    {
    const CHAR *pch;

    for ( pch = pszLine; *pch; pch++ )
        {
        if ( !isspace( *pch ) )
            return FALSE;
        }

    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// ReadText( )
//
// Reads a single text line from a file.
//////////////////////////////////////////////////////////////////////////////
void ReadText( FILE *pfile, CHAR *psz, USHORT cb, BOOL *pfEOF )
    {
    CHAR *pch;
    
    TASKLIST
        {
        *pfEOF = FALSE;
        
        while ( pch = fgets( psz, cb, pfile ) )
            {
            if ( !EmptyLine( psz ) )
                break;
            }

        if ( !pch )
            {
            *pfEOF = TRUE;
            QUIT;
            }

        pch = strchr( psz, '\n' );
        if ( !pch )
            {
            // buffer overflowed
            THROW_EXCEPTION( exFileCorrupt );
            }
        *pch = '\0';
        }
    ENDTASKLIST
    }

//////////////////////////////////////////////////////////////////////////////
// GetChar( )
//
// Retrieves the next character pointer based on a given set of delimiters.
//////////////////////////////////////////////////////////////////////////////
CHAR *GetChar( CHAR *pszBuffer, const CHAR *pszDelimiters )
    {
    CHAR *pch = pszBuffer;

    while ( *pch && strchr( pszDelimiters, *pch ) )
        {
        pch++;
        }
    if ( *pch )
        {
        return pch;
        }
    else
        {
        return NULL;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// ClearName( )
//
// Clears the frame name without throwing an exception.
//////////////////////////////////////////////////////////////////////////////
void ClearName( )
    {       
    CHAR sz[ _cbBufferMax + 1 ];
    lstrcpy( sz, GetString( IDS_APPNAME ) );
    lstrcat( sz, " - " );
    SetWindowText( _hwndFrame, sz );
    }
    
//////////////////////////////////////////////////////////////////////////////
// UpdateName( )
//
// Updates the frame window name based on the internal document name.
//////////////////////////////////////////////////////////////////////////////
void UpdateName( )
    {
    QString sUntitled( GetString( IDS_UNTITLED ) );
    QString sAppName( GetString( IDS_APPNAME ) );
    QString sName( sAppName );
    const CHAR *psz;

    TRY
        {
        sName += " - ";
        psz = _pdoc->GetFileName( );
        if ( psz )
            {
            sName += psz;
            }
        else
            {
            sName += sUntitled;
            }
        SetWindowText( _hwndFrame, sName );
        }
    CATCH_ALL
        {
        sUntitled.Cleanup( );
        sAppName.Cleanup( );
        sName.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// DosAccess( )
//
// Returns TRUE if the given file is found.
//////////////////////////////////////////////////////////////////////////////
extern BOOL DosAccess( const CHAR *szPathName )
    {
    FILE *pfile;
    BOOL fRet = FALSE;

    TASKLIST
        {
        pfile = fopen( szPathName, GetString( IDS_FOPEN_RB ) );
    
        if ( !pfile )
            {
            QUIT;
            }

        fclose( pfile );
        fRet = TRUE;
        }
    ENDTASKLIST

    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// MsgBox( )
//
// Convenient routine to display a message box given a string resource id.
//////////////////////////////////////////////////////////////////////////////
extern int MsgBox( HWND hwnd, USHORT ids, USHORT mb )
    {
    CHAR szMsg[ _cbBufferMax + 1 ];
    CHAR szApp[ _cbBufferMax + 1 ];

    lstrcpy( szMsg, GetString( ids ) );
    lstrcpy( szApp, GetString( IDS_APPNAME ) );
    return MessageBox( hwnd, szMsg, szApp, mb );
    }

//////////////////////////////////////////////////////////////////////////////
// SafeDeleteObject( )
//
// Calls DeleteObject( ) only if h is not NULL.
//////////////////////////////////////////////////////////////////////////////
extern BOOL SafeDeleteObject( HANDLE h )
    {
    if ( h != NULL )
        {
        return DeleteObject( h );
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// EraseOldLine( )
//
// Erases the old tracker line.
//////////////////////////////////////////////////////////////////////////////
extern void EraseOldLine( HWND hwnd )
    {
    RECT rc;
    HDC  hdc;
    int  xPosOld;

    xPosOld = GetXPos( hwnd );
    if ( xPosOld == xPosNull )
        {
        return;
        }

    hdc = GetDC( hwnd );
    ASSERT( hdc != NULL );
    SetMapMode( hdc, MM_LOENGLISH );
    GetClientRectSpecial( hwnd, &rc );
    SetViewportOrg( hdc, 0, rc.bottom );
    DPtoLP( hdc, (LPPOINT) &rc, 2 );
    SetROP2( hdc, R2_NOT );
    MoveTo( hdc, xPosOld, rc.top );
    LineTo( hdc, xPosOld, rc.bottom );
    ReleaseDC( hwnd, hdc );
    }

//////////////////////////////////////////////////////////////////////////////
// LayoutInUse
//
// This routine will return TRUE if the layout is in use (i.e., is
// currently being displayed as a window).
//////////////////////////////////////////////////////////////////////////////
extern BOOL LayoutInUse( const CHAR *pszLayout )
    {
    BOOL fFound( FALSE );
    QString sClassName( GetString( IDS_CLSCHILD ) );
    QString sTextName( pszLayout );
    CHAR szClassName[ _cbBufferMax + 1 ];
    CHAR szTextName[ _cbBufferMax + 1 ];
    HWND hwndChild;

    TRY
        {
        hwndChild = GetWindow( _hwndClient, GW_CHILD );
        while ( hwndChild != NULL )
            {
            GetClassName( hwndChild,  szClassName, sizeof( szClassName ) );
            GetWindowText( hwndChild, szTextName,  sizeof( szTextName ) );
            if ( sClassName == szClassName && sTextName == szTextName )
                {
                fFound = TRUE;
                break;
                }
            hwndChild = GetWindow( hwndChild, GW_HWNDNEXT );
            }
        }
    CATCH_ALL
        {
        sClassName.Cleanup( );
        sTextName.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    return fFound;
    }

//////////////////////////////////////////////////////////////////////////////
// GetWindowLayout( )
//
// Gets the window layout associated with hwnd.
//////////////////////////////////////////////////////////////////////////////
extern QWindowLayout *GetWindowLayout( HWND hwnd )
    {
    CHAR szName[ _cbNameMax + 1 ];
    GetWindowText( hwnd, szName, sizeof( szName ) );
    QWindowLayout *pwl = _ptpl->m_wllist.Find( szName );
    return pwl;
    }

//////////////////////////////////////////////////////////////////////////////
// AdjustCentury( )
//
// This routine adjusts a year (integer) to handle centuries
// according to the following rules:
//
// if year < 60        --> Assume the year is 20xx.
// if 60 <= year < 100 --> Assume the year is 19xx.
// otherwise           --> Make to assumptions.
//////////////////////////////////////////////////////////////////////////////
extern int AdjustCentury( int year )
    {
    if ( year < 70 )
        {
        year += 2000;
        }
    else if ( 70 <= year && year < 100 )
        {
        year += 1900;
        }
    return year;
    }

//////////////////////////////////////////////////////////////////////////////
// GetCBText( )
//
// Gets the text of a combo box at a given index.  Note that the cbMax
// parameter must be large enough to contain the string and the NULL terminator,
// otherwise the routine returns CB_ERR;
//////////////////////////////////////////////////////////////////////////////
extern int GetCBText( HWND hwndCombo, CHAR *psz, int cbMax, int nIndex )
    {
    if ( (int) SendMessage( hwndCombo, CB_GETLBTEXTLEN, (WPARAM) nIndex, 0L ) >= cbMax )
        {
        return CB_ERR;
        }
    return (int) SendMessage( hwndCombo, CB_GETLBTEXT, (WPARAM) nIndex, (LPARAM)(LPSTR) psz );
    }
    
//////////////////////////////////////////////////////////////////////////////
// ComboFindString( )
//
// Given a string, find the *index* in it, performing a *case-sensitive*
// search.
//
// returns: The index in the combo box if found; otherwise, CB_ERR.
//////////////////////////////////////////////////////////////////////////////
extern int ComboFindString( HWND hwndCombo, const CHAR *psz )
    {   
    int nIndex( CB_ERR );
        
    int nCount  = (int) SendMessage( hwndCombo, CB_GETCOUNT, 0, 0L );
    for ( int i = 0; i < nCount; i++ )
        {
        int  cbBuffer = (int) SendMessage( hwndCombo, CB_GETLBTEXTLEN, i, 0L ) + 1;
        CHAR *pszBuffer = new CHAR[ cbBuffer ];
        if ( !pszBuffer )
            {
            THROW_EXCEPTION( exMemory );
            }
        GetCBText( hwndCombo, pszBuffer, cbBuffer, i );
        if ( lstrcmp( psz, pszBuffer ) == 0 )
            {
            nIndex = i;
            delete [] pszBuffer;
            break;
            }     
        delete [] pszBuffer;
        }                      
    return nIndex;
    }

//////////////////////////////////////////////////////////////////////////////
// ViewFile( )
//
// This routine will launch the file browser to view the contents of a file,
// if the user requests.
//
// Returns TRUE if the user actually viewed the file
//////////////////////////////////////////////////////////////////////////////
extern BOOL ViewFile( HWND hwnd, const CHAR *pszFile )
    {
    QString sCmd;
    BOOL fRet = FALSE;

    TRY
        {
        if ( MessageBox( hwnd, 
                         "File Saved.  View Contents?", 
                         GetString( IDS_APPNAME ),
                         MB_YESNO ) == IDYES )
            {
            sCmd  = _ptpl->m_genopts.GetBrowserName( );
            sCmd += " "; 
            sCmd += pszFile;
            
            if ( WinExec( sCmd, SW_SHOWMAXIMIZED ) < 32 )
                THROW_EXCEPTION( exFile );
            fRet = TRUE;
            }
        }
    CATCH_ALL
        {
        sCmd.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    return fRet;
    }


