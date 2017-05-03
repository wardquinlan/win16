//////////////////////////////////////////////////////////////////////////////
// TEMPLATE.CPP
//
// This file contains the template class methods
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <windowsx.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <statpak.h>
#include "types.hxx"
#include "resource.hxx"
#include "ot.hxx"
#include "template.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "tools.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::QGeneralOptions( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QGeneralOptions::QGeneralOptions( )
    {
    m_dyFont            = 20;
    m_dxLine            = 0;
    m_nIdxClrBackground = nIdxLtGray;
    m_nIdxClrText       = nIdxBlack;
    m_nIdxClrHLC        = nIdxPurple;
    m_nIdxClrVolume     = nIdxBlack;
    m_nIdxClrGrids      = nIdxDkGray;
    m_nIdxClrBuy        = nIdxBlue;
    m_nIdxClrSell       = nIdxRed;
    m_fQuoteBox         = TRUE;
    m_fTracker          = TRUE;
    m_fAutoOpenText     = FALSE;
    m_fTextGridLines    = FALSE;
    m_dt                = dtMonths;
    m_stScreen          = eByPage;
    m_stPrint           = eByPage;
    m_sBrowser          = "notepad";
    }

QGeneralOptions::~QGeneralOptions( )
    {
    Cleanup( );
    }

void QGeneralOptions::Cleanup( )
    {
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::Store( QArchive &ar )
    {
    ar.WriteHeader( otGeneralOptions );
    ar.Write( &m_dyFont, sizeof( m_dyFont ) );
    ar.Write( &m_dxLine, sizeof( m_dxLine ) );
    ar.Write( &m_nIdxClrBackground, sizeof( m_nIdxClrBackground ) );
    ar.Write( &m_nIdxClrText, sizeof( m_nIdxClrText ) );
    ar.Write( &m_nIdxClrHLC, sizeof( m_nIdxClrHLC ) );
    ar.Write( &m_nIdxClrVolume, sizeof( m_nIdxClrVolume ) );
    ar.Write( &m_nIdxClrGrids, sizeof( m_nIdxClrGrids ) );
    ar.Write( &m_nIdxClrBuy, sizeof( m_nIdxClrBuy ) );
    ar.Write( &m_nIdxClrSell, sizeof( m_nIdxClrSell ) );
    ar.Write( &m_fQuoteBox, sizeof( m_fQuoteBox ) );
    ar.Write( &m_fTracker, sizeof( m_fTracker ) );
    ar.Write( &m_fAutoOpenText, sizeof( m_fAutoOpenText ) );
    ar.Write( &m_fTextGridLines, sizeof( m_fTextGridLines ) ) ;
    ar.Write( &m_dt, sizeof( m_dt ) );
    ar.Write( &m_stScreen, sizeof( m_stScreen ) );
    ar.Write( &m_stPrint, sizeof( m_stPrint ) );
    m_sBrowser.Store( ar );
    m_sExternalFile.Store( ar );
    }

void QGeneralOptions::Retrieve( QArchive &ar, UINT ot )
    {
    USHORT cb;

    if ( ot != otGeneralOptions )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_dyFont, sizeof( m_dyFont ) );
    if ( cb != sizeof( m_dyFont ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_dxLine, sizeof( m_dxLine ) );
    if ( cb != sizeof( m_dxLine ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nIdxClrBackground, sizeof( m_nIdxClrBackground ) );
    if ( cb != sizeof( m_nIdxClrBackground ) || m_nIdxClrBackground >= RGCLRSIZE )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nIdxClrText, sizeof( m_nIdxClrText ) );
    if ( cb != sizeof( m_nIdxClrText ) || m_nIdxClrText >= RGCLRSIZE )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nIdxClrHLC, sizeof( m_nIdxClrHLC ) );
    if ( cb != sizeof( m_nIdxClrHLC ) || m_nIdxClrHLC >= RGCLRSIZE )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nIdxClrVolume, sizeof( m_nIdxClrVolume ) );
    if ( cb != sizeof( m_nIdxClrVolume ) || m_nIdxClrVolume >= RGCLRSIZE )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nIdxClrGrids, sizeof( m_nIdxClrGrids ) );
    if ( cb != sizeof( m_nIdxClrGrids ) || m_nIdxClrGrids >= RGCLRSIZE )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nIdxClrBuy, sizeof( m_nIdxClrBuy ) );
    if ( cb != sizeof( m_nIdxClrBuy ) || m_nIdxClrBuy >= RGCLRSIZE )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nIdxClrSell, sizeof( m_nIdxClrSell ) );
    if ( cb != sizeof( m_nIdxClrSell ) || m_nIdxClrSell >= RGCLRSIZE )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_fQuoteBox, sizeof( m_fQuoteBox ) );
    if ( cb != sizeof( m_fQuoteBox ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_fTracker, sizeof( m_fTracker ) );
    if ( cb != sizeof( m_fTracker ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_fAutoOpenText, sizeof( m_fAutoOpenText ) );
    if ( cb != sizeof( m_fAutoOpenText ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_fTextGridLines, sizeof( m_fTextGridLines ) );
    if ( cb != sizeof( m_fTextGridLines ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    
    cb = ar.Read( &m_dt, sizeof( m_dt ) );
    if ( cb != sizeof( m_dt ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    cb = ar.Read( &m_stScreen, sizeof( m_stScreen ) );
    if ( cb != sizeof( m_stScreen ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    
    cb = ar.Read( &m_stPrint, sizeof( m_stPrint ) );
    if ( cb != sizeof( m_stPrint ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    
    BOOL f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sBrowser.Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sExternalFile.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::Scaling( )
//
// Returns the scaling type: eByPage or eByChart, for a given output
// device, eScreen or ePrint.
//////////////////////////////////////////////////////////////////////////////
QGeneralOptions::ScalingType QGeneralOptions::Scaling( Output device ) const
    {
    ASSERT( device == eScreen || device == ePrint );
    return ( device == eScreen ? m_stScreen : m_stPrint );
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::Scaling( )
//
// Sets the scaling type: eByPage or eByChart, for a given output
// device, eScreen or ePrinter.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::Scaling( Output device, QGeneralOptions::ScalingType sc )
    {
    ASSERT( device == eScreen || device == ePrint );
    ASSERT( sc == eByPage || sc == eByChart );
    if ( device == eScreen )
        m_stScreen = sc;
    else
        m_stPrint  = sc;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetExternalFileName( )
//
// Gets the external indicator file name
//////////////////////////////////////////////////////////////////////////////
const CHAR *QGeneralOptions::GetExternalFileName( ) const
    {
    return m_sExternalFile;
    }

//////////////////////////////////////////////////////////////////////////////    
// QGeneralOptions::SetExternalFileName( )
//
// Sets the external indicator file name
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetExternalFileName( const CHAR *s )
    {                 
    m_sExternalFile = s;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetBrowserName( )
//
// Gets the browser name
//////////////////////////////////////////////////////////////////////////////
const CHAR *QGeneralOptions::GetBrowserName( ) const
    {
    return m_sBrowser;
    }

//////////////////////////////////////////////////////////////////////////////    
// QGeneralOptions::SetBrowserName( )
//
// Sets the browser name
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetBrowserName( const CHAR *s )
    {                 
    m_sBrowser = s;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetFontSize( )
//
// Gets the text font size, in logical units.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetFontSize( ) const
    {
    return m_dyFont;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetFontSize( )
//
// Sets the text font size, in logical units.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetFontSize( USHORT dyFont )
    {
    m_dyFont = dyFont;
    }
    
//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetLineWidths( )
//
// Gets the line widths, in logical units.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetLineWidths( ) const
    {
    return m_dxLine;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetLineWidths( )
//
// Sets the line widths, in logical units.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetLineWidths( USHORT dxLine )
    {
    m_dxLine = dxLine;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetBackgroundColor( )
//
// Gets the window background color.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetBackgroundColor( ) const
    {
    return m_nIdxClrBackground;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetBackgroundColor( )
//
// Sets the window background color.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetBackgroundColor( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClrBackground = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetTextColor( )
//
// Gets the text color.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetTextColor( ) const
    {
    return m_nIdxClrText;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetTextColor( )
//
// Sets the text color.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetTextColor( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClrText = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetHLCColor( )
//
// Gets the hi-lo-close bar color.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetHLCColor( ) const
    {
    return m_nIdxClrHLC;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetHLCColor( )
//
// Sets the hi-lo-close bar color.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetHLCColor( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClrHLC = nIdxClr;
    }
 
//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetVolumeColor( )
//
// Gets the volume bar color.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetVolumeColor( ) const
    {
    return m_nIdxClrVolume;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetVolumeColor( )
//
// Sets the volume bar color.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetVolumeColor( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClrVolume = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetGridsColor( )
//
// Gets the gridlines color.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetGridsColor( ) const
    {
    return m_nIdxClrGrids;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetGridsColor( )
//
// Sets the gridlines color.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetGridsColor( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClrGrids = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetBuyColor( )
//
// Gets the buy-signal color.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetBuyColor( ) const
    {
    return m_nIdxClrBuy;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetBuyColor( )
//
// Sets the buy-signal color.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetBuyColor( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClrBuy = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetSellColor( )
//
// Gets the sell-signal color.
//////////////////////////////////////////////////////////////////////////////
USHORT QGeneralOptions::GetSellColor( ) const
    {
    return m_nIdxClrSell;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetSellColor( )
//
// Sets the sell-signal color.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetSellColor( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClrSell = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetAutoOpenText( )
//
// Returns TRUE if the auto-open for text enabled
//////////////////////////////////////////////////////////////////////////////
BOOL QGeneralOptions::GetAutoOpenText( ) const
    {
    return m_fAutoOpenText;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetAutoOpenText( )
//
// Set whether the auto-open for text enabled
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetAutoOpenText( BOOL fAutoOpenText )
    {
    m_fAutoOpenText = fAutoOpenText;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetQuoteBox( )
//
// Returns TRUE if the quote box is in use.
//////////////////////////////////////////////////////////////////////////////
BOOL QGeneralOptions::GetQuoteBox( ) const
    {
    return m_fQuoteBox;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetQuoteBox( )
//
// Set whether the quote box is in use or not.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetQuoteBox( BOOL fQuoteBox )
    {
    m_fQuoteBox = fQuoteBox;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetTextGridLines( )
//
// Returns TRUE if text grid lines are in use
//////////////////////////////////////////////////////////////////////////////
BOOL QGeneralOptions::GetTextGridLines( ) const
    {
    return m_fTextGridLines;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetTextGridLines( )
//
// Set if text grid lines are in use
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetTextGridLines( BOOL f )
    {
    m_fTextGridLines = f;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetTracker( )
//
// Returns TRUE if the tracker is in use.
//////////////////////////////////////////////////////////////////////////////
BOOL QGeneralOptions::GetTracker( ) const
    {
    return m_fTracker;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetTracker( )
//
// Set whether the tracker is in use or not.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetTracker( BOOL fTracker )
    {
    m_fTracker = fTracker;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::GetDateType( )
//
// This routine will get the date type to display on the views.
//////////////////////////////////////////////////////////////////////////////
DateType QGeneralOptions::GetDateType( ) const
    {
    return m_dt;
    }

//////////////////////////////////////////////////////////////////////////////
// QGeneralOptions::SetDateType( )
//
// This routine will set the date type to display on the views.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::SetDateType( DateType dt )
    {
    ASSERT( dt == dtNone || dt == dtMonths || dt == dtYears );
    m_dt = dt;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QGernalOptions::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QGeneralOptions::AssertValid( ) const
    {
    QObject::AssertValid( );
    ASSERT( m_nIdxClrBackground < RGCLRSIZE );
    ASSERT( m_nIdxClrText       < RGCLRSIZE );
    ASSERT( m_nIdxClrHLC        < RGCLRSIZE );
    ASSERT( m_nIdxClrVolume     < RGCLRSIZE );
    ASSERT( m_nIdxClrGrids      < RGCLRSIZE );
    ASSERT( m_nIdxClrBuy        < RGCLRSIZE );
    ASSERT( m_nIdxClrSell       < RGCLRSIZE );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QTemplate::m_v
//
// Template version static object
//////////////////////////////////////////////////////////////////////////////
const QVersion QTemplate::m_v( 1, 11, 0 );

//////////////////////////////////////////////////////////////////////////////
// QTemplate::QTemplate( )
//
// Constructor and destructors.
//////////////////////////////////////////////////////////////////////////////
QTemplate::QTemplate( CHAR *pszFileName )
    {
    QString sFileName;
    CHAR szPath [ _cbPathMax + 1 ];
    CHAR szDrive[ _MAX_DRIVE + 1 ];
    CHAR szDir  [ _MAX_DIR   + 1 ];
    CHAR szFName[ _MAX_FNAME + 1 ];
    CHAR szExt  [ _MAX_EXT   + 1 ];

    TRY
        {
        if ( pszFileName && *pszFileName )
            {
            sFileName = pszFileName;
            }
        else
            {
            sFileName = GetString( IDS_DEFAULT_TPL );
            }

        _splitpath( sFileName, szDrive, szDir, szFName, szExt );
        if ( *szDrive && *szDir )
            {
            m_sFileName = sFileName;
            }
        else
            {
            GetWindowsDirectory( szPath, sizeof( szPath ) );
            m_sFileName  = szPath;
            m_sFileName += "\\";
            m_sFileName += sFileName;
            }
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, "QTemplate constructor failed.\n" );
        sFileName.Cleanup( );
        m_sFileName.Cleanup( );
        m_fInit = FALSE;
        }
    END_CATCH

    m_fModified = FALSE;
    }

QTemplate::~QTemplate( )
    {
    Cleanup( );
    }

void QTemplate::Cleanup( )
    {
    if ( m_fInit )
        {
        m_itlist.Cleanup( );
        m_wllist.Cleanup( );
        m_genopts.Cleanup( );
        m_sFileName.Cleanup( );
        m_format.Cleanup( );
        m_fModified = FALSE;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QTemplate::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QTemplate::Store( QArchive &ar )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    QVersion v( m_v.GetMajorNumber( ),
                m_v.GetMinorNumber( ),
                m_v.GetRevisionNumber( ) );

    ar.WriteHeader( otTemplate );
    v.Store( ar );
    m_genopts.Store( ar );
    m_format.Store( ar );
    m_itlist.Store( ar );
    m_wllist.Store( ar );
    m_rulelist.Store( ar );
    m_condlist.Store( ar );
    m_constlist.Store( ar );
    }

void QTemplate::Retrieve( QArchive &ar, UINT ot )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    QVersion v;
    BOOL f;

    if ( ot != otTemplate )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    v.Retrieve( ar, ot );
    if ( !( v == m_v ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_genopts.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_format.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if (!f ) 
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_itlist.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if (!f ) 
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_wllist.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_rulelist.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_condlist.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_constlist.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QTemplate::Export( )
//
// Export template file to text file
//////////////////////////////////////////////////////////////////////////////
void QTemplate::Export( FILE *pfile ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    fprintf( pfile, "Quote Template Definitions\n" );
    fprintf( pfile, "--------------------------\n\n" );
    fprintf( pfile, "%-16s : %s\n", "Template File", GetFileName( ) );
    fprintf( pfile, "%-16s : %s\n\n\n", "External File", 
        ( m_genopts.GetExternalFileName( ) ? m_genopts.GetExternalFileName( ) : "<none>" ) );

    m_constlist.Export( pfile );
    m_rulelist.Export( pfile );
    m_itlist.Export( pfile );
    m_condlist.Export( pfile );
    }

//////////////////////////////////////////////////////////////////////////////
// QTemplate::GetModifiedFlag( )
//
// Returns TRUE if the template has been modifed.
//////////////////////////////////////////////////////////////////////////////
BOOL QTemplate::GetModifiedFlag( ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    return m_fModified;
    }
    
//////////////////////////////////////////////////////////////////////////////
// QTemplate::SetModifiedFlag( )
//
// Updates the modified state of the template accordingly.
//////////////////////////////////////////////////////////////////////////////
void QTemplate::SetModifiedFlag( BOOL fModified )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    m_fModified = fModified;
    }

//////////////////////////////////////////////////////////////////////////////
// QTemplate::GetFileName( )
//
// Returns the file to which the template is stored.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QTemplate::GetFileName( ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    return m_sFileName;
    }

//////////////////////////////////////////////////////////////////////////////
// QTemplate::RuleInUse( )
//
// Returns TRUE if the given rule is in use.
//////////////////////////////////////////////////////////////////////////////
BOOL QTemplate::RuleInUse( const CHAR *pszRule ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    
    BOOL fFound = FALSE;
    QWindowLayout *pwl;
    int i;
    
    for ( pwl = (QWindowLayout *) m_wllist.GetHead( );
          pwl && !fFound;
          pwl = (QWindowLayout *) m_wllist.GetNext( pwl ) )
        {
        for ( i = 0; i < _cViewLayouts && !fFound; i++ )
            {
            const CHAR *psz = pwl->m_rgvl[ i ].GetRule( );
            if ( psz && lstrcmp( psz, pszRule ) == 0 )
                {
                fFound = TRUE;
                }
            }
        }
    return fFound;
    }

//////////////////////////////////////////////////////////////////////////////
// QTemplate::ConditionInUse( )
//
// Returns TRUE if the given condition is in use.
//////////////////////////////////////////////////////////////////////////////
BOOL QTemplate::ConditionInUse( const CHAR *pszCond ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    
    BOOL fFound = FALSE;
    QWindowLayout *pwl;
    int i;
    
    for ( pwl = (QWindowLayout *) m_wllist.GetHead( );
          pwl && !fFound;
          pwl = (QWindowLayout *) m_wllist.GetNext( pwl ) )
        {
        for ( i = 0; i < _cViewLayouts && !fFound; i++ )
            {
            const CHAR *psz = pwl->m_rgvl[ i ].GetCondition( );
            if ( psz && lstrcmp( psz, pszCond ) == 0 )
                {
                fFound = TRUE;
                }
            }
        }
    return fFound;
    }

//////////////////////////////////////////////////////////////////////////////
// QTemplate::IndicatorInUse( )
//
// Returns TRUE if the given indicator is in use.
//////////////////////////////////////////////////////////////////////////////
BOOL QTemplate::IndicatorInUse( const CHAR *pszIndicator ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    BOOL fFound = FALSE;
    QWindowLayout *pwl;
    int i;
    
    for ( pwl = (QWindowLayout *) m_wllist.GetHead( );
          pwl && !fFound;
          pwl = (QWindowLayout *) m_wllist.GetNext( pwl ) )
        {
        for ( i = 0; i < _cViewLayouts && !fFound; i++ )
            {
            if ( pwl->m_rgvl[ i ].m_dict.Find( pszIndicator ) )
                {
                fFound = TRUE;
                }
            }
        }
    return fFound;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QTemplate::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QTemplate::AssertValid( ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, "QTemplate::AssertValid( ) called with m_fInit FALSE.\n" );
        ASSERT( FALSE );
        return;
        }

    QObject::AssertValid( );
    m_genopts.AssertValid( );
    m_itlist.AssertValid( );
    m_wllist.AssertValid( );
    m_sFileName.AssertValid( );
    m_format.AssertValid( );
    m_rulelist.AssertValid( );
    m_condlist.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::QIndicatorTemplate( )
//
// Constructors and desctructors.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplate::QIndicatorTemplate( )
    :m_sName( )
    {
    m_nIdxClr   = 0;
    m_gt        = gtLine;
    }

QIndicatorTemplate::~QIndicatorTemplate( )
    {
    Cleanup( );
    }

void QIndicatorTemplate::Cleanup( )
    {
    m_sName.Cleanup( );
    m_nIdxClr = 0;
    m_gt      = gtLine;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplate::Store( QArchive &ar )
    {
    ar.WriteHeader( otIndicatorTemplate );
    m_sName.Store( ar );
    ar.Write( &m_nIdxClr, sizeof( m_nIdxClr ) );
    ar.Write( &m_gt, sizeof( m_gt ) );
    }

void QIndicatorTemplate::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL f;
    USHORT cb;

    if ( ot != otIndicatorTemplate )
        THROW_EXCEPTION( exFileCorrupt );
        
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sName.Retrieve( ar, ot );
    
    cb = ar.Read( &m_nIdxClr, sizeof( m_nIdxClr ) );
    if ( cb != sizeof( m_nIdxClr ) )
        THROW_EXCEPTION( exFileCorrupt );
        
    cb = ar.Read( &m_gt, sizeof( m_gt ) );
    if ( cb != sizeof( m_gt ) )
        THROW_EXCEPTION( exFileCorrupt );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::GetName( )
//
// Returns the name of the indicator template.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplate::GetName( ) const
    {
    return m_sName;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::SetName( )
//
// Sets the name of the indicator template.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplate::SetName( const CHAR *pszName )
    {
    m_sName = pszName;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::GetIdxClr( )
//
// Returns the color index of the indicator template.
//////////////////////////////////////////////////////////////////////////////
USHORT QIndicatorTemplate::GetIdxClr( ) const
    {
    return m_nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::SetIdxClr( )
//
// Sets the color index of the indicator template.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplate::SetIdxClr( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClr = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::GetGraphType( )
//
// Returns the graph type: gtLine or gtBar or gtPoint.
//////////////////////////////////////////////////////////////////////////////
GraphType QIndicatorTemplate::GetGraphType( ) const
    {
    return m_gt;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::SetGraphType( )
//
// Sets the graph type: gtLine or gtBar or gtPoint.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplate::SetGraphType( GraphType gt )
    {
    ASSERT( gt == gtLine || gt == gtBar || gt == gtPoint );
    m_gt = gt;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplate::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplate::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    ASSERT( m_nIdxClr < RGCLRSIZE );
    ASSERT( m_gt == gtLine || m_gt == gtBar || m_gt == gtPoint );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::QIndicatorTemplateNormal
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplateNormal::QIndicatorTemplateNormal( )
    :QIndicatorTemplate( )
    {
    OptionsZero( &m_opts );
    }

QIndicatorTemplateNormal::~QIndicatorTemplateNormal( )
    {
    Cleanup( );
    }

void QIndicatorTemplateNormal::Cleanup( )
    {
    m_sPath.Cleanup( );
    OptionsFree( &m_opts );
    QIndicatorTemplate::Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateNormal::Store( QArchive &ar )
    {
    ar.WriteHeader( otIndicatorTemplateNormal );
    QIndicatorTemplate::Store( ar );
    m_sPath.Store( ar );
    ar.Write( &m_opts.cbOptions, sizeof( m_opts.cbOptions ) );
    if ( m_opts.cbOptions )
        {
        // treat lOptions as a pointer to data
        ASSERT( (LPBYTE) m_opts.lOptions != NULL );
        BYTE *pb = new BYTE[ m_opts.cbOptions ];
        TRY
            {
            if ( !pb )
                {
                THROW_EXCEPTION( exMemory );
                }
            lmemcpy( pb, (LPBYTE) m_opts.lOptions, m_opts.cbOptions );
            ar.Write( pb, m_opts.cbOptions );
            }
        CATCH_ALL
            {
            delete [] pb;
            THROW_LAST( );
            }
        END_CATCH
        delete [] pb;
        }
    else
        {
        // treat lOptions as data itself
        ar.Write( &m_opts.lOptions, sizeof( m_opts.lOptions ) );
        }
    }

void QIndicatorTemplateNormal::Retrieve( QArchive &ar, UINT ot )
    {
    BYTE *pb;
    BOOL  f;
    USHORT cb;
    if ( ot != otIndicatorTemplateNormal )
        THROW_EXCEPTION( exFileCorrupt );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    QIndicatorTemplate::Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sPath.Retrieve( ar, ot );
    
    cb = ar.Read( &m_opts.cbOptions, sizeof( m_opts.cbOptions ) );
    if ( cb != sizeof( m_opts.cbOptions ) )
        THROW_EXCEPTION( exFileCorrupt );
        
    if ( m_opts.cbOptions )
        {
        // treat lOptions as a pointer to data
        TRY
            {
            m_opts.lOptions = 0L;
            pb = new BYTE[ m_opts.cbOptions ];
            if ( !pb )
                THROW_EXCEPTION( exMemory );
                
            m_opts.lOptions = (DWORD)(LPBYTE) GAlloc( m_opts.cbOptions );
            if ( !m_opts.lOptions )
                THROW_EXCEPTION( exMemory );
                
            cb = ar.Read( pb, m_opts.cbOptions );
            if ( cb != m_opts.cbOptions )
                THROW_EXCEPTION( exFileCorrupt );
                
            lmemcpy( (LPBYTE) m_opts.lOptions, pb, m_opts.cbOptions );
            delete [] pb;
            }
        CATCH_ALL
            {
            GFree( (LPBYTE) m_opts.lOptions );
            delete [] pb;
            THROW_LAST( );
            }
        END_CATCH
        }
    else
        {
        // treat loptions as data itself
        cb = ar.Read( &m_opts.lOptions, sizeof( m_opts.lOptions ) );
        if ( cb != sizeof( m_opts.lOptions ) )
            THROW_EXCEPTION( exFileCorrupt );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::Export( )
//
// Exports indicator template parameters to text file
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateNormal::Export( FILE *pfile ) const
    {
    static CHAR szBuffer[ _cbBufferMax + 1 ];
    LPCSTR lpsz = NULL;
    LPPARAMSTRINGPROC lpfn = NULL;
    HANDLE hLibrary = NULL;
    
    lmemset( szBuffer, 0, sizeof( szBuffer ) );
    hLibrary = LoadLibrary( m_sPath );
    if ( hLibrary >= 32 )
        {
        lpfn = (LPPARAMSTRINGPROC) GetProcAddress( hLibrary, 
                                   GetString( IDS_ITPROC_PARAMSTRING ) );
        if ( lpfn )
            {
            lpsz = lpfn( (LPOPTIONS) &m_opts );
            ASSERT( lpsz != NULL );
            lstrcpyn( szBuffer, lpsz, _cbBufferMax );
            }
        else
            {
            lstrcpy( szBuffer, GetString( IDS_UNAVAIL ) );
            }

        FreeLibrary( hLibrary );
        }
    else
        {
        lstrcpy( szBuffer, GetString( IDS_UNAVAIL ) );
        }

    fprintf( pfile, "#%-16s: {%s,%s}\n", GetName( ), GetLibrary( ), szBuffer );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::VerString( )
//
// Returns the version string of an indicator template.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateNormal::VerString( ) const
    {
    static CHAR     szBuffer[ _cbBufferMax + 1 ];
    LPCSTR          lpsz;
    LPVERSTRINGPROC lpfn     = NULL;
    HANDLE          hLibrary = NULL;
    
    lmemset( szBuffer, 0, sizeof( szBuffer ) );
    hLibrary = LoadLibrary( m_sPath );
    if ( hLibrary >= 32 )
        {
        lpfn = (LPVERSTRINGPROC) GetProcAddress( hLibrary, 
                                 GetString( IDS_ITPROC_VERSTRING) );
        if ( lpfn )
            lpsz = lpfn( );

        lstrcpyn( szBuffer, lpsz, _cbBufferMax );
        FreeLibrary( hLibrary );
        }

    return szBuffer;
    }


//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::GetLibrary( )
//
// Returns the *name* of the indicator library.
// (i.e., not a handle to the library itself.)
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateNormal::GetLibrary( ) const
    {
    return m_sPath;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::SetLibrary( )
//
// Sets the *name* of the indicator library.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateNormal::SetLibrary( const CHAR *pszLibrary )
    {
    m_sPath = pszLibrary;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::GetOptions( )
//
// Returns a pointer to the options data.  Future enhancement:
// return a pointer to constant option data.
//////////////////////////////////////////////////////////////////////////////
LPOPTIONS QIndicatorTemplateNormal::GetOptions( )
    {
    return &m_opts;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::SetOptions( )
//
// Copies passed-in option data to internal options storage.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateNormal::SetOptions( LPOPTIONS lpopts )
    {
    OptionsCopy( &m_opts, lpopts );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::rtti( )
//
// This routine will return the run-time type information
// for normal indicators.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplate::Rtti QIndicatorTemplateNormal::rtti( ) const
    {
    return QIndicatorTemplate::rtNormal;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::BuildDataSet
//
// This routine builds the data and horizontal lines datasets.
// The routine is invarient: if the data cannot be built, 
// it cleans up after itself.
//////////////////////////////////////////////////////////////////////////////
BOOL QIndicatorTemplateNormal::BuildDataSet( LPDATASET lpdataset, LPDATASET lpdatasetHorz, int nRecurseLevel )
    {
    BOOL fRet       = FALSE;
    LPCALCPROC lpfn = NULL;
    HANDLE hLibrary = NULL;
    CALCSTRUCT calc;
    DATASET dsX;
    DATASET dsCount;
    
    lmemset( &calc, 0, sizeof( calc ) );
    DataSetZero( lpdataset );
    DataSetZero( lpdatasetHorz );

    DataSetZero( &dsX );
    DataSetZero( &dsCount );

    TRY
        {
        TASKLIST
            {
            hLibrary = LoadLibrary( m_sPath );
            if ( hLibrary < 32 )
                {
                hLibrary = NULL;
                QUIT;
                }
            
            lpfn = (LPCALCPROC) GetProcAddress( hLibrary, GetString( IDS_ITPROC_CALC ) );
            QUITIF( !lpfn );
            
            if ( !OptionsCopy( &calc.opts, &m_opts ) )
                {
                THROW_EXCEPTION( exMemory );
                }

            _pdoc->BuildQuoteData( &calc.datasetOp,
                                   &calc.datasetHi,
                                   &calc.datasetLo,
                                   &calc.datasetCl,
                                   &calc.datasetVl, 
                                   &dsX,
                                   &dsCount );

            QUITIF( !( (*lpfn)( (LPCALCSTRUCT) &calc ) ) );
            QUITIF( !DataSetCopy( lpdataset, &calc.datasetCalc ) );
            QUITIF( !DataSetCopy( lpdatasetHorz, &calc.datasetHorz ) );
            fRet = TRUE;
            }
        ENDTASKLIST

        if ( hLibrary )
            {
            FreeLibrary( hLibrary );
            }
        OptionsFree( &calc.opts );
        DataSetFree( &calc.datasetHi );
        DataSetFree( &calc.datasetLo );
        DataSetFree( &calc.datasetCl );
        DataSetFree( &calc.datasetVl );
        DataSetFree( &calc.datasetCalc );
        DataSetFree( &calc.datasetHorz );
        DataSetFree( &dsX );
        DataSetFree( &dsCount );
        if ( !fRet )
            {
            DataSetFree( lpdataset );
            DataSetFree( lpdatasetHorz );
            }
        }
    CATCH_ALL
        {
        if ( hLibrary )
            {
            FreeLibrary( hLibrary );
            }
        OptionsFree( &calc.opts );
        DataSetFree( &calc.datasetHi );
        DataSetFree( &calc.datasetLo );
        DataSetFree( &calc.datasetCl );
        DataSetFree( &calc.datasetVl );
        DataSetFree( &calc.datasetCalc );
        DataSetFree( &calc.datasetHorz );
        DataSetFree( lpdataset );
        DataSetFree( lpdatasetHorz );
        DataSetFree( &dsX );
        DataSetFree( &dsCount );
        THROW_LAST( );
        }
    END_CATCH

    return fRet;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateNormal::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateNormal::AssertValid( ) const
    {
    QIndicatorTemplate::AssertValid( );
    m_sPath.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::QIndicatorTemplateExternal
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplateExternal::QIndicatorTemplateExternal( )
    :QIndicatorTemplate( )
    {
    }

QIndicatorTemplateExternal::~QIndicatorTemplateExternal( )
    {
    Cleanup( );
    }

void QIndicatorTemplateExternal::Cleanup( )
    {
    m_sExternalName.Cleanup( );
    m_sHorz.Cleanup( );
    QIndicatorTemplate::Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateExternal::Store( QArchive &ar )
    {
    ar.WriteHeader( otIndicatorTemplateExternal );
    QIndicatorTemplate::Store( ar );
    m_sExternalName.Store( ar );
    m_sHorz.Store( ar );
    }

void QIndicatorTemplateExternal::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL f;
    if ( ot != otIndicatorTemplateExternal )
        THROW_EXCEPTION( exFileCorrupt );

    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    QIndicatorTemplate::Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sExternalName.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sHorz.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::Export( )
//
// Exports indicator template parameters to text file
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateExternal::Export( FILE *pfile ) const
    {
    fprintf( pfile, "#%-16s: \"%s\"\n", GetName( ), GetExternalName( ) );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::GetLibrary( )
//
// Returns the *name* of the indicator library.
// (i.e., not a handle to the library itself.)
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateExternal::GetLibrary( ) const
    {
    return GetString( IDS_EXTERNAL_LIB );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::VerString( )
//
// Returns the version string of an external indicator template.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateExternal::VerString( ) const
    {
    return GetString( IDS_EXTERNAL_VERSTRING );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::GetExternalName( )
//
// Returns the external indicator name.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateExternal::GetExternalName( ) const
    {
    return m_sExternalName;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::SetExternalName( )
//
// Sets external indicator name.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateExternal::SetExternalName( const CHAR *pszName )
    {
    m_sExternalName = pszName;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::GetHorz( )
//
// Returns the external indicator horizontal line string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateExternal::GetHorz( ) const
    {
    return m_sHorz;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::SetHorz( )
//
// Sets external indicator horizontal line string.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateExternal::SetHorz( const CHAR *pszHorz )
    {
    m_sHorz = pszHorz;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::rtti( )
//
// This routine will return the run-time type information
// for external indicators.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplate::Rtti QIndicatorTemplateExternal::rtti( ) const
    {
    return QIndicatorTemplate::rtExternal;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::BuildDataSet( )
//
// This routine will build the external indicator data set.
//////////////////////////////////////////////////////////////////////////////
BOOL QIndicatorTemplateExternal::BuildDataSet( LPDATASET lpdataset, LPDATASET lpdatasetHorz, int nRecurseLevel )
    {
    CHAR szHorz[ _cbTextMax + 1 ];
    BOOL fRet = FALSE;
    QExternalIndicatorList localList;
    
    TRY
        {
        TASKLIST
            {
            DataSetZero( lpdataset );
            DataSetZero( lpdatasetHorz );
            
            // Build a local external indicator list
            if ( !BuildLocalList( localList ) )
                break;
        
            if ( localList.GetCount( ) == 0 )
                break;

            // Populate the dataset
            if ( !PopulateDataSet( localList, lpdataset ) )
                break;

            // Now deal with the horizontal lines.
            const CHAR *pszDelimiters = " ,\t";
            
            // Pass 1
            ASSERT( m_sHorz.GetLength( ) <= _cbTextMax );
            lstrcpy( szHorz, m_sHorz );
            WORD nCount = 0;
            CHAR *pch = strtok( szHorz, pszDelimiters );
            while ( pch )
                {
                nCount++;
                pch = strtok( NULL, pszDelimiters );
                }

            if ( !DataSetAlloc( lpdatasetHorz, nCount, 0 ) )
                THROW_EXCEPTION( exMemory );

            // Pass 2
            lstrcpy( szHorz, m_sHorz );
            nCount = 0;
            pch = strtok( szHorz, pszDelimiters );
            while ( pch )
                {
                lpdatasetHorz->lpnum[ nCount ] = (NUM) atof( pch );
                nCount++;
                pch = strtok( NULL, pszDelimiters );
                }

            fRet = TRUE;
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        localList.Cleanup( );
        DataSetFree( lpdataset );
        DataSetFree( lpdatasetHorz );
        THROW_LAST( );
        }
    END_CATCH

    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::PopulateDataSet( )
//
// Allocates and Populates an external indicator's dataset
//////////////////////////////////////////////////////////////////////////////
BOOL QIndicatorTemplateExternal::PopulateDataSet( const QExternalIndicatorList &localList, LPDATASET lpdataset )
    {
    // Nothing to display
    if ( _pdoc->GetCount( ) == 0 )
        return FALSE;

    // Calculate the size of the dataset to allocate
    QExternalIndicatorRecord *prec = (QExternalIndicatorRecord *) localList.GetHead( );
    ASSERT( prec != NULL );

    WORD nPoints = _pdoc->GetCount( );
    WORD nOffset = 0u;
    
    QQt *pqqt = _pdoc->GetFirst( );
    while ( pqqt && pqqt->GetDate( ) < prec->GetDate( ) )
        {
        nPoints--;
        nOffset++;
        pqqt = _pdoc->GetNext( pqqt );
        }
    
    ASSERT( nPoints + nOffset == _pdoc->GetCount( ) );

    // Allocate the dataset
    if ( !DataSetAlloc( lpdataset, nPoints, nOffset ) )
        THROW_EXCEPTION( exMemory );

    WORD nCurr = 0;
    for ( ; pqqt; pqqt = _pdoc->GetNext( pqqt ) )
        {
        ASSERT( nCurr < nPoints );

        prec = FindRecord( localList, pqqt->GetDate( ) );
        ASSERT( prec != NULL );
        lpdataset->lpnum[ nCurr++ ] = prec->GetValue( );
        }

    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::FindRecord( )
//
// Find first record >= given pqqt
//////////////////////////////////////////////////////////////////////////////
QExternalIndicatorRecord *QIndicatorTemplateExternal::FindRecord( const QExternalIndicatorList &localList, const QTime &t )
    {
    for ( QExternalIndicatorRecord *prec = (QExternalIndicatorRecord *) localList.GetTail( );
          prec;
          prec = (QExternalIndicatorRecord *) localList.GetPrev( prec ) )
        {
        if ( prec->GetDate( ) <= t )
            return prec;
        }
    
    return NULL;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateExternal::BuildLocalList( )
//
// Creates a localized copy of the external indicator list,
// relevant to the loaded security
//////////////////////////////////////////////////////////////////////////////
BOOL QIndicatorTemplateExternal::BuildLocalList( QExternalIndicatorList &localList ) 
    {
    BOOL fError( FALSE );

    localList.Cleanup( );
    for ( QExternalIndicatorRecord *prec = (QExternalIndicatorRecord *) _pExtDoc->model( )->GetHead( );
          prec;
          prec = (QExternalIndicatorRecord *) _pExtDoc->model( )->GetNext( prec ) )
        {
        if ( lstrcmpi( prec->GetFileName( ), _pdoc->GetFileName( ) ) == 0 ||
             lstrcmp( prec->GetFileName( ), "*" ) == 0 )
            {
            if ( lstrcmp( prec->GetName( ), GetExternalName( ) ) == 0 )
                {
                if ( !localList.InsertIndicator( prec ) )
                    {
                    fError = TRUE;
                    break;
                    }
                }
            }
        }

#ifdef DEBUG
    Trace( TR_NORMAL, "Here is the Local List:\n" );
    for ( prec = (QExternalIndicatorRecord *) localList.GetHead( );
          prec;
          prec = (QExternalIndicatorRecord *) localList.GetNext( prec ) )
        {
        CHAR sz[ _cbBufferMax + 1 ];
        sprintf( sz, "%f", prec->GetValue( ) );

        QString str = (QString) prec->GetDate( );
        str += "-";
        str += prec->GetFileName( );
        str += "-";
        str += prec->GetName( );
        str += "-";
        str += sz;
        Trace( TR_NORMAL, "%s\n", (const CHAR *) str );
        }
#endif

    return !fError;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::QIndicatorTemplateCustom
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplateCustom::QIndicatorTemplateCustom( )
    :QIndicatorTemplate( )
    {
    }

QIndicatorTemplateCustom::~QIndicatorTemplateCustom( )
    {
    Cleanup( );
    }

void QIndicatorTemplateCustom::Cleanup( )
    {
    m_sDefn.Cleanup( );
    m_sHorz.Cleanup( );
    QIndicatorTemplate::Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateCustom::Store( QArchive &ar )
    {
    ar.WriteHeader( otIndicatorTemplateCustom );
    QIndicatorTemplate::Store( ar );
    m_sDefn.Store( ar );
    m_sHorz.Store( ar );
    }

void QIndicatorTemplateCustom::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL f;
    if ( ot != otIndicatorTemplateCustom )
        THROW_EXCEPTION( exFileCorrupt );

    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    QIndicatorTemplate::Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sDefn.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sHorz.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::Export( )
//
// Exports indicator template parameters to text file
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateCustom::Export( FILE *pfile ) const
    {
    fprintf( pfile, "#%-16s: %s\n", GetName( ), RemoveCR( GetDefn( ) ) );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::GetLibrary( )
//
// Returns the *name* of the indicator library.
// (i.e., not a handle to the library itself.)
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateCustom::GetLibrary( ) const
    {
    return GetString( IDS_CUSTOM_LIB );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::VerString( )
//
// Returns the version string of a custom indicator template.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateCustom::VerString( ) const
    {
    return GetString( IDS_CUSTOM_VERSTRING );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::GetDefn( )
//
// Returns the custom indicator definition.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateCustom::GetDefn( ) const
    {
    return m_sDefn;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::SetDefn( )
//
// Sets custom indicator definition.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateCustom::SetDefn( const CHAR *pszDefn )
    {
    m_sDefn = pszDefn;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::GetHorz( )
//
// Returns the custom indicator horizontal line string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QIndicatorTemplateCustom::GetHorz( ) const
    {
    return m_sHorz;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::SetHorz( )
//
// Sets custom indicator horizontal line string.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorTemplateCustom::SetHorz( const CHAR *pszHorz )
    {
    m_sHorz = pszHorz;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::rtti( )
//
// This routine will return the run-time type information
// for custom indicators.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplate::Rtti QIndicatorTemplateCustom::rtti( ) const
    {
    return QIndicatorTemplate::rtCustom;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorTemplateCustom::BuildDataSet( )
//
// This routine will build the custom indicator data set.
//////////////////////////////////////////////////////////////////////////////
BOOL QIndicatorTemplateCustom::BuildDataSet( LPDATASET lpdataset, LPDATASET lpdatasetHorz, int nRecurseLevel )
    {
    DATASET     ds;
    UINT        i;
    QQt        *pqt;
    QParser::Rc rc;
    QTokenList  list;
    NUM         numValue;
    BOOL        fError  = FALSE;
    BOOL        fFirst  = TRUE;
    UINT        nFirst  = 0u;
    WORD        nPoints = 0u;
    WORD        nOffset = 0u;
    CHAR        szHorz[ _cbTextMax + 1 ];

    TRY
        {
        TASKLIST
            {
            DataSetZero( &ds );
            DataSetZero( lpdataset );
            DataSetZero( lpdatasetHorz );
            if ( !DataSetAlloc( &ds, _pdoc->GetCount( ), 0u ) )
                THROW_EXCEPTION( exMemory );

            if ( !QParser::Analyze( list, m_sDefn, FALSE ) )
                {
                fError = TRUE;
                break;
                }

            for ( pqt = _pdoc->GetFirst( ), i = 0u;
                  pqt != NULL;
                  pqt = _pdoc->GetNext( pqt ), i++ )
                {
                QTokenIter iter( list );
                rc = QParser::expr( i, iter, numValue, nRecurseLevel );
                if ( rc == QParser::rcFatalError )
                    {
                    fError = TRUE;
                    break;
                    }
                if ( rc == QParser::rcUnAvail )
                    continue;
                
                if ( iter( ) )
                    {
                    Trace( TR_NORMAL, "QIndicatorTemplateCustom::BuildDataSet: Unexpected Token\n" );
                    fError = TRUE;
                    break;
                    }

                ASSERT( rc == QParser::rcOk );
                
                if ( fFirst )
                    {
                    fFirst = FALSE;
                    nFirst = i;
                    }
                ds.lpnum[ i ] = numValue;
                }
            
            if ( fError )
                break;

            nPoints = _pdoc->GetCount( ) - nFirst;
            nOffset = nFirst;
            if ( !DataSetAlloc( lpdataset, nPoints, nOffset ) )
                THROW_EXCEPTION( exMemory );
            
            for ( i = 0; i < lpdataset->nPoints; i++ )
                {
                lpdataset->lpnum[ i ] = ds.lpnum[ i + nOffset ];
                }

            // Now deal with the horizontal lines.
            const CHAR *pszDelimiters = " ,\t";
            
            // Pass 1
            lstrcpy( szHorz, m_sHorz );
            WORD nCount = 0;
            CHAR *pch = strtok( szHorz, pszDelimiters );
            while ( pch )
                {
                nCount++;
                pch = strtok( NULL, pszDelimiters );
                }

            if ( !DataSetAlloc( lpdatasetHorz, nCount, 0 ) )
                THROW_EXCEPTION( exMemory );

            // Pass 2
            lstrcpy( szHorz, m_sHorz );
            nCount = 0;
            pch = strtok( szHorz, pszDelimiters );
            while ( pch )
                {
                lpdatasetHorz->lpnum[ nCount ] = (NUM) atof( pch );
                nCount++;
                pch = strtok( NULL, pszDelimiters );
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        DataSetFree( &ds );
        DataSetFree( lpdataset );
        DataSetFree( lpdatasetHorz );
        THROW_LAST( );
        }
    END_CATCH

    DataSetFree( &ds );
    return !fError;
    }

//////////////////////////////////////////////////////////////////////////////
// QWLList::QWLList
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QWLList::QWLList( )
    {
    }

QWLList::~QWLList( )
    {
    Cleanup( );
    }
    
void QWLList::Cleanup( )
    {
    QWindowLayout *pwl;

    while ( pwl = (QWindowLayout *) RemoveHead( ) )
        {
        delete pwl;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QWLList::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QWLList::Store( QArchive &ar )
    {
    ar.WriteHeader( otWLList );
    for( QWindowLayout *pwl = (QWindowLayout *) GetHead( );
         pwl;
         pwl = (QWindowLayout *) GetNext( pwl ) )
        {
        pwl->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }
      
void QWLList::Retrieve( QArchive &ar, UINT ot )
    {        
    QWindowLayout *pwl = 0;
    BOOL fAdded = FALSE;
        
    TRY
        {
        if ( ot != otWLList )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }
        while ( TRUE )
            {
            pwl = 0;
            fAdded = FALSE;
            BOOL f = ar.ReadHeader( &ot );
            if ( !f )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            if ( ot == otEndMarker )
                {
                break;
                }
            pwl = new QWindowLayout;
            if ( !pwl )
                {
                THROW_EXCEPTION( exMemory );
                }
            pwl->Retrieve( ar, ot );
            AddTail( pwl );
            fAdded = TRUE;
            }
        }
    CATCH_ALL
        {    
        if ( !fAdded )
            {
            delete pwl;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QWLList::Find( )
//
// Given a window layout name, tries to find it in the list.
//////////////////////////////////////////////////////////////////////////////
QWindowLayout *QWLList::Find( const CHAR *pszLayout ) const
    {
    for ( QWindowLayout *pwl = (QWindowLayout *) GetHead( ); 
          pwl; 
          pwl = (QWindowLayout *) GetNext( pwl ) )
        {
        ASSERT( pwl->GetName( ) != NULL );
        if ( lstrcmp( pwl->GetName( ), pszLayout ) == 0 )
            {
            return pwl;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QWLList::AssertValid( )
//
// Standard persistant storage routine.
//////////////////////////////////////////////////////////////////////////////
void QWLList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QWindowLayout *pwl = (QWindowLayout *) GetHead( ); 
          pwl; 
          pwl = (QWindowLayout *) GetNext( pwl ) )
        {
        pwl->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::QWindowLayout( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QWindowLayout::QWindowLayout( )
    :m_fAutoOpen( FALSE )
    {
    m_dxincr = 4;
    m_rgvl[ 0 ].SetHLCV( TRUE );
    m_rgvl[ 0 ].SetPercent( 100 );
    }

QWindowLayout::~QWindowLayout( )
    {
    Cleanup( );
    }

void QWindowLayout::Cleanup( )
    {
    int i;

    m_sName.Cleanup( );
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        m_rgvl[ i ].Cleanup( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QWindowLayout::Store( QArchive &ar )
    {
    int i;

    ar.WriteHeader( otWindowLayout );
    m_sName.Store( ar );
    ar.Write( &m_dxincr, sizeof( m_dxincr ) );
    ar.Write( &m_fAutoOpen, sizeof( m_fAutoOpen ) );
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        m_rgvl[ i ].Store( ar );
        }
    }

void QWindowLayout::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL   f;
    int    i;
    USHORT cb;

    if ( ot != otWindowLayout )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sName.Retrieve( ar, ot );

    cb = ar.Read( &m_dxincr, sizeof( m_dxincr ) );
    if ( cb != sizeof( m_dxincr ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    cb = ar.Read( &m_fAutoOpen, sizeof( m_fAutoOpen ) );
    if ( cb != sizeof( m_fAutoOpen ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
        
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        f = ar.ReadHeader( &ot );
        if ( !f )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }
        m_rgvl[ i ].Retrieve( ar, ot );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::GetName( )
//
// Returns the name of the window layout.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QWindowLayout::GetName( ) const
    {
    return m_sName;
    }

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::SetName( )
//
// Sets the name of the window layout.
//////////////////////////////////////////////////////////////////////////////
void QWindowLayout::SetName( const CHAR *pszName )
    {
    m_sName = pszName;
    }

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::GetDxIncr( )
//
// Gets the DX increment (in logical units) of the window layout.
//////////////////////////////////////////////////////////////////////////////
USHORT QWindowLayout::GetDxIncr( ) const
    {
    return m_dxincr;
    }

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::SetDxIncr( )
//
// Sets the DX increment (in logical units) of the window layout.
//////////////////////////////////////////////////////////////////////////////
void QWindowLayout::SetDxIncr( USHORT dxincr )
    {
    m_dxincr = dxincr;
    }

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::GetAutoOpen( )
//
// Returns the status of the auto-open flag.
//////////////////////////////////////////////////////////////////////////////
BOOL QWindowLayout::GetAutoOpen( ) const
    {
    return m_fAutoOpen;
    }

//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::SetAutoOpen( )
//
// Sets the status of the auto-open flag.
//////////////////////////////////////////////////////////////////////////////
void QWindowLayout::SetAutoOpen( BOOL fAutoOpen )
    {
    m_fAutoOpen = fAutoOpen;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QWindowLayout::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QWindowLayout::AssertValid( ) const
    {
    int i;

    QListObject::AssertValid( );
    m_sName.AssertValid( );
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        m_rgvl[ i ].AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QITList::QITList( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QITList::QITList( )
    {
    }

QITList::~QITList( )
    {
    Cleanup( );
    }
    
void QITList::Cleanup( )
    {
    QIndicatorTemplate *pi;

    while ( pi = (QIndicatorTemplate *) RemoveHead( ) )
        {
        delete pi;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QITList::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QITList::Store( QArchive &ar )
    {
    ar.WriteHeader( otITList );
    for( QIndicatorTemplate *pi = (QIndicatorTemplate *) GetHead( );
         pi;
         pi = (QIndicatorTemplate *) GetNext( pi ) )
        {
        pi->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QITList::Retrieve( QArchive &ar, UINT ot )
    {
    QIndicatorTemplate *pi = 0;
    BOOL                fAdded = FALSE;
    
    TRY
        {
        if ( ot != otITList )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }
        while ( TRUE )
            {
            pi = 0;
            fAdded = FALSE;
            BOOL f = ar.ReadHeader( &ot );
            if ( !f )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            if ( ot == otEndMarker )
                {
                break;
                }
            if ( ot == otIndicatorTemplateNormal )
                pi = new QIndicatorTemplateNormal;
            else if ( ot == otIndicatorTemplateCustom )
                pi = new QIndicatorTemplateCustom;
            else if ( ot == otIndicatorTemplateExternal )
                pi = new QIndicatorTemplateExternal;
            else
                THROW_EXCEPTION( exFileCorrupt );

            if ( !pi )
                {
                THROW_EXCEPTION( exMemory );
                }
            // call the virtual retrieve function
            pi->Retrieve( ar, ot );
            AddTail( pi );
            fAdded = TRUE;
            }
        }
    CATCH_ALL
        {    
        if ( !fAdded )
            {         
            delete pi;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QITList::Export( )
//
// Export indicator template definitions to text file
//////////////////////////////////////////////////////////////////////////////
void QITList::Export( FILE *pfile ) const
    {
    QIndicatorTemplate *pi;

    fprintf( pfile, "Indicators\n" );
    fprintf( pfile, "------------------\n\n" );

    // Export Normal Indicators
    for ( pi = (QIndicatorTemplate *) GetHead( ); 
          pi; 
          pi = (QIndicatorTemplate *) GetNext( pi ) )
        {
        if ( pi->rtti( ) == QIndicatorTemplate::rtNormal )
            pi->Export( pfile );
        }
    
    // Export Custom Indicators
    for ( pi = (QIndicatorTemplate *) GetHead( ); 
          pi; 
          pi = (QIndicatorTemplate *) GetNext( pi ) )
        {
        if ( pi->rtti( ) == QIndicatorTemplate::rtCustom )
            pi->Export( pfile );
        }
    
    // Export External Indicators
    for ( pi = (QIndicatorTemplate *) GetHead( ); 
          pi; 
          pi = (QIndicatorTemplate *) GetNext( pi ) )
        {
        if ( pi->rtti( ) == QIndicatorTemplate::rtExternal )
            pi->Export( pfile );
        }
    fprintf( pfile, "\n\n" );
    }

//////////////////////////////////////////////////////////////////////////////
// QITList::Find( )
//
// Finds the indicator template, given a text name.
//////////////////////////////////////////////////////////////////////////////
QIndicatorTemplate *QITList::Find( const CHAR *pszTemplate, 
                                   QIndicatorTemplate::Rtti rt ) const
    {
    for ( QIndicatorTemplate *pi = (QIndicatorTemplate *) GetHead( ); 
          pi; 
          pi = (QIndicatorTemplate *) GetNext( pi ) )
        {
        ASSERT( pi->GetName( ) != NULL );
        if ( lstrcmp( pi->GetName( ), pszTemplate ) == 0 )
            {
            if ( rt == QIndicatorTemplate::rtAll ||
                 rt == pi->rtti( ) )
            return pi;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QITList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QITList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QIndicatorTemplate *pi = (QIndicatorTemplate *) GetHead( ); 
          pi; 
          pi = (QIndicatorTemplate *) GetNext( pi ) )
        {
        pi->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QFormat::QFormat( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QFormat::QFormat( )
    {
    TRY
        {
        m_sLoad       = GetString( IDS_LOAD_STRING );
        m_sDelimiters = GetString( IDS_LOAD_DELIMITERS );
        m_sSave       = GetString( IDS_SAVE_STRING );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, "QFormat constructor failed.\n" );
        m_sLoad.Cleanup( );
        m_sDelimiters.Cleanup( );
        m_sSave.Cleanup( );
        m_fInit = FALSE;
        }
    END_CATCH
    }

QFormat::~QFormat( )
    {
    Cleanup( );
    }

void QFormat::Cleanup( )
    {
    if ( m_fInit )
        {
        m_sLoad.Cleanup( );
        m_sDelimiters.Cleanup( );
        m_sSave.Cleanup( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QFormat::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QFormat::Store( QArchive &ar )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    ar.WriteHeader( otFormat );
    m_sLoad.Store( ar );
    m_sDelimiters.Store( ar );
    m_sSave.Store( ar );
    }

void QFormat::Retrieve( QArchive &ar, UINT ot )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    BOOL f;
    if ( ot != otFormat )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sLoad.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sDelimiters.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sSave.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QFormat::GetLoadString( )
//
// Returns the load format string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QFormat::GetLoadString( )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    return m_sLoad;
    }

//////////////////////////////////////////////////////////////////////////////
// QFormat::GetLoadDelimiters( )
//
// Returns the load delimiters string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QFormat::GetLoadDelimiters( )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    return m_sDelimiters;
    }

//////////////////////////////////////////////////////////////////////////////
// QFormat::GetSaveString( )
//
// Returns the save format string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QFormat::GetSaveString( )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    return m_sSave;
    }

//////////////////////////////////////////////////////////////////////////////
// QFormat::SetLoadString( )
//
// Sets the load format string.
//////////////////////////////////////////////////////////////////////////////
void QFormat::SetLoadString( const CHAR *psz )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    m_sLoad = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QFormat::SetLoadDelimiters( )
//
// Sets the load delimiters string.
//////////////////////////////////////////////////////////////////////////////
void QFormat::SetLoadDelimiters( const CHAR *psz )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    m_sDelimiters = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QFormat::SetSaveString( )
//
// Sets the save format string.
//////////////////////////////////////////////////////////////////////////////
void QFormat::SetSaveString( const CHAR *psz )
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    m_sSave = psz;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QFormat::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QFormat::AssertValid( ) const
    {
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, "QFormat::AssertValid( ) called with m_fInit FALSE.\n" );
        ASSERT( FALSE );
        return;
        }

    QObject::AssertValid( );
    m_sLoad.AssertValid( );
    m_sSave.AssertValid( );
    m_sDelimiters.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::QViewLayout( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QViewLayout::QViewLayout( )
    {
    m_fHLCV       = FALSE;
    m_fHorz       = FALSE;
    m_fRoundScale = FALSE;
    m_fLogScale   = FALSE;
    m_nGrids      = 4;
    m_nPercent    = 0;
    }

QViewLayout::~QViewLayout( )
    {
    Cleanup( );
    }

void QViewLayout::Cleanup( )
    {
    m_sTitle.Cleanup( );
    m_sRule.Cleanup( );
    m_sCond.Cleanup( );
    m_dict.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::Store( QArchive &ar )
    {
    ar.WriteHeader( otViewLayout );
    m_sTitle.Store( ar );
    m_sRule.Store( ar );
    m_sCond.Store( ar );
    m_dict.Store( ar );
    ar.Write( &m_fHLCV,       sizeof( m_fHLCV ) );
    ar.Write( &m_fHorz,       sizeof( m_fHorz ) );
    ar.Write( &m_fRoundScale, sizeof( m_fRoundScale ) );
    ar.Write( &m_fLogScale,   sizeof( m_fLogScale ) );
    ar.Write( &m_nGrids,      sizeof( m_nGrids ) );
    ar.Write( &m_nPercent,    sizeof( m_nPercent ) );
    }

void QViewLayout::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL   f;
    USHORT cb;

    if ( ot != otViewLayout )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sTitle.Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sRule.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sCond.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_dict.Retrieve( ar, ot );

    cb = ar.Read( &m_fHLCV, sizeof( m_fHLCV ) );
    if ( cb != sizeof( m_fHLCV ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_fHorz, sizeof( m_fHorz ) );
    if ( cb != sizeof( m_fHorz ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_fRoundScale, sizeof( m_fRoundScale ) );
    if ( cb != sizeof( m_fRoundScale ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_fLogScale, sizeof( m_fLogScale ) );
    if ( cb != sizeof( m_fLogScale ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nGrids, sizeof( m_nGrids ) );
    if ( cb != sizeof( m_nGrids ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    cb = ar.Read( &m_nPercent, sizeof( m_nPercent ) );
    if ( cb != sizeof( m_nPercent ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetTitle( )
//
// Gets the view title string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QViewLayout::GetTitle( ) const
    {
    return m_sTitle;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetTitle( )
//
// Sets the view title string.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetTitle( const CHAR *pszTitle )
    {
    m_sTitle = pszTitle;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetRule( )
//
// Gets the view's attached rule.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QViewLayout::GetRule( ) const
    {
    return m_sRule;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetRule( )
//
// Sets the view's attached rule.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetRule( const CHAR *pszRule )
    {
    m_sRule = pszRule;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetCondition( )
//
// Gets the view's attached condition.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QViewLayout::GetCondition( ) const
    {
    return m_sCond;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetCondition( )
//
// Sets the view's attached condition.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetCondition( const CHAR *pszCond )
    {
    m_sCond = pszCond;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetHLCV( )
//
// Returns the status of the hi-lo-close-volume flag.
//////////////////////////////////////////////////////////////////////////////
BOOL QViewLayout::GetHLCV( ) const
    {
    return m_fHLCV;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetHLCV( )
//
// Sets the hi-lo-close-volume flag.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetHLCV( BOOL fHLCV )
    {
    m_fHLCV = fHLCV;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetHorz( )
//
// Gets the status of the horizontal grid lines flag.
//////////////////////////////////////////////////////////////////////////////
BOOL QViewLayout::GetHorz( ) const
    {
    return m_fHorz;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetHorz( )
//
// Sets the status of the horizontal grid lines flag.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetHorz( BOOL fHorz )
    {
    m_fHorz = fHorz;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetRoundScale( )
//
// Gets the status of the round scaling flag.
//////////////////////////////////////////////////////////////////////////////
BOOL QViewLayout::GetRoundScale( ) const
    {
    return m_fRoundScale;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetRoundScale( )
//
// Sets the status of the round scaling flag.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetRoundScale( BOOL fRoundScale )
    {
    m_fRoundScale = fRoundScale;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetLogScale( )
//
// Gets the status of the log scaling flag.
//////////////////////////////////////////////////////////////////////////////
BOOL QViewLayout::GetLogScale( ) const
    {
    return m_fLogScale;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetLogScale( )
//
// Sets the status of the log scaling flag.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetLogScale( BOOL fLogScale )
    {
    m_fLogScale = fLogScale;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetGrids( )
//
// Returns the number of grids on the view.
//////////////////////////////////////////////////////////////////////////////
USHORT QViewLayout::GetGrids( ) const
    {
    return m_nGrids;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetGrids( )
//
// Sets the number of grids on the view.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetGrids( USHORT nGrids )
    {
    m_nGrids = nGrids;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::GetPercent( )
//
// Gets this view's real-estate percentage.
//////////////////////////////////////////////////////////////////////////////
USHORT QViewLayout::GetPercent( ) const
    {
    return m_nPercent;
    }

//////////////////////////////////////////////////////////////////////////////
// QViewLayout::SetPercent( )
//
// Sets this view's real-estate percentage.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::SetPercent( USHORT nPercent )
    {
    ASSERT( nPercent <= 100 );
    m_nPercent = nPercent;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QViewLayout::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QViewLayout::AssertValid( ) const
    {
    QObject::AssertValid( );
    m_sTitle.AssertValid( );
    m_dict.AssertValid( );
    m_sRule.AssertValid( );
    ASSERT( m_nPercent <= 100 );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QCache::QCache( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QCache::QCache( )
    {
    DataSetZero( &m_dataset );
    DataSetZero( &m_datasetHorz );
    }

QCache::~QCache( )
    {
    Cleanup( );
    }

void QCache::Cleanup( )
    {
    DataSetFree( &m_dataset );
    DataSetFree( &m_datasetHorz );
    m_sName.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::SetName( )
//
// Sets the cache text name.
//////////////////////////////////////////////////////////////////////////////
void QCache::SetName( const CHAR *pszName )
    {
    m_sName = pszName;
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::GetName( )
//
// Gets the cache text name.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QCache::GetName( ) const
    {
    return (const CHAR *) m_sName;
    }
    
//////////////////////////////////////////////////////////////////////////////
// QCache::funcCopy( )
//
// Helper function to parse out @-sign delimited strings into tokens.
// It works in the following way.
//
// You pass in a string in the following format:
//
// "str1@str2@str3@str4\0"
//
// The routine copies the string 'str1' into szToken, and then returns
// a pointer to 'str2'.  You are now ready to recall funcCopy( ) again
// with str2.  This process is repeated until it hits the '\0' character.
// At this point, funcCopy( ) returns blank tokens, signalling the end,
// and returns a NULL pointer.  If you call funcCopy with a NULL pointer,
// it will return a NULL pointer and an empty token string.
//
// Calls to the string "str1@@str2\0" are handled by:
// returning str1, then returning an empty string, then returning str2,
// then returning an empty string, along with a NULL pointer.
//
// It is assumed that szToken is of size ( _cbFuncNameMax + 1 ).
//////////////////////////////////////////////////////////////////////////////
const CHAR *QCache::funcCopy( CHAR *szToken, const CHAR *pszString ) const
    {
    const CHAR *pchSrc;
    CHAR *pchDst;
    
    lmemset( szToken, 0, _cbFuncNameMax + 1 );
    if ( pszString == NULL )
        return NULL;
        
    pchSrc = pszString;
    pchDst = szToken;
    while ( *pchSrc && *pchSrc != '@' )
        *pchDst++ = *pchSrc++;
        
    return ( *pchSrc ? ( pchSrc + 1 ) : NULL );
    }

//////////////////////////////////////////////////////////////////////////////
// QCahce::setConstant( )
//
// This helper routine will set ds to a constant
// value of num.
//////////////////////////////////////////////////////////////////////////////
void QCache::setConstant( DATASET &ds, WORD nPoints, WORD nOffset, NUM num ) const
    {
    WORD i;
    if ( !DataSetAlloc( &ds, nPoints, nOffset ) )
        THROW_EXCEPTION( exMemory );
    for ( i = 0; i < nPoints; ++i )
        ds.lpnum[ i ] = num;
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::setYear( )
//
// This helper will set ds to the value of the year
//////////////////////////////////////////////////////////////////////////////
void QCache::setYear( DATASET &ds ) const
    {
    WORD i;
    QQt *pqt;
    
    if ( !DataSetAlloc( &ds, _pdoc->GetCount( ), 0u ) )
        THROW_EXCEPTION( exMemory );

    for ( i = 0, pqt = _pdoc->GetFirst( );
          pqt;
          i++, pqt = _pdoc->GetNext( pqt ) )
        {
        QTime t = pqt->GetDate( );
        ds.lpnum[ i ] = (NUM) t.GetYear( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::setMonth( )
//
// This helper will set ds to the value of the month
//////////////////////////////////////////////////////////////////////////////
void QCache::setMonth( DATASET &ds ) const
    {
    WORD i;
    QQt *pqt;
    
    if ( !DataSetAlloc( &ds, _pdoc->GetCount( ), 0u ) )
        THROW_EXCEPTION( exMemory );

    for ( i = 0, pqt = _pdoc->GetFirst( );
          pqt;
          i++, pqt = _pdoc->GetNext( pqt ) )
        {
        QTime t = pqt->GetDate( );
        ds.lpnum[ i ] = (NUM) t.GetMonth( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::setDay( )
//
// This helper will set ds to the value of the month day
//////////////////////////////////////////////////////////////////////////////
void QCache::setDay( DATASET &ds ) const
    {
    WORD i;
    QQt *pqt;
    
    if ( !DataSetAlloc( &ds, _pdoc->GetCount( ), 0u ) )
        THROW_EXCEPTION( exMemory );

    for ( i = 0, pqt = _pdoc->GetFirst( );
          pqt;
          i++, pqt = _pdoc->GetNext( pqt ) )
        {
        QTime t = pqt->GetDate( );
        ds.lpnum[ i ] = (NUM) t.GetDay( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::setWeekDay( )
//
// This helper will set ds to the value of the week day
//////////////////////////////////////////////////////////////////////////////
void QCache::setWeekDay( DATASET &ds ) const
    {
    WORD i;
    QQt *pqt;
    
    if ( !DataSetAlloc( &ds, _pdoc->GetCount( ), 0u ) )
        THROW_EXCEPTION( exMemory );

    for ( i = 0, pqt = _pdoc->GetFirst( );
          pqt;
          i++, pqt = _pdoc->GetNext( pqt ) )
        {
        QTime t = pqt->GetDate( );
        ds.lpnum[ i ] = (NUM) t.GetWeekDay( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::setYearDay( )
//
// This helper will set ds to the value of the day-of-year
//////////////////////////////////////////////////////////////////////////////
void QCache::setYearDay( DATASET &ds ) const
    {
    WORD i;
    QQt *pqt;
    
    if ( !DataSetAlloc( &ds, _pdoc->GetCount( ), 0u ) )
        THROW_EXCEPTION( exMemory );

    for ( i = 0, pqt = _pdoc->GetFirst( );
          pqt;
          i++, pqt = _pdoc->GetNext( pqt ) )
        {
        QTime t = pqt->GetDate( );
        ds.lpnum[ i ] = (NUM) ( t.GetYearDay( ) + 1 );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::Build( )
//
// This routine builds the cached data for views and others.
//////////////////////////////////////////////////////////////////////////////
BOOL QCache::Build( int nRecurseLevel )
    {
    BOOL fRet = FALSE;
    NUM  numPeriod;
    NUM  numAccel;
    NUM  numAccelMax;
    CHAR szName [ _cbFuncNameMax + 1 ];
    CHAR szToken[ _cbFuncNameMax + 1 ];
    const CHAR *pszNext;
    DATASET ds;
    DATASET ds2;
    
    TRY
        {
        TASKLIST
            {
            DataSetZero( &ds );
            DataSetZero( &ds2 );
            DataSetFree( &m_dataset );
            DataSetFree( &m_datasetHorz );
            
            if ( GetName( )[ 0 ] == '@' )
                {
                lstrcpy( szName, GetName( ) );
                pszNext = funcCopy( szToken, szName + 1 );
                if ( lstrcmp( szToken, "avg" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Mean( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "lwst" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Lowest( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "hgst" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Highest( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "min" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );
                        
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds2, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds2, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );

                    if ( pszNext )
                        break;
                    fRet = MinMax( &m_dataset, &ds, &ds2, TRUE );
                    }
                else if ( lstrcmp( szToken, "max" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );
                        
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds2, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds2, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );

                    if ( pszNext )
                        break;
                    fRet = MinMax( &m_dataset, &ds, &ds2, FALSE );
                    }
                else if ( lstrcmp( szToken, "xavg" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = ExpSmooth( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "rsi" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Rsi( &m_dataset, (LPDATASET) &gcache.cl( ), (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "vi" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Vi( &m_dataset, 
                        (LPDATASET) &gcache.hi( ), 
                        (LPDATASET) &gcache.lo( ), 
                        (LPDATASET) &gcache.cl( ), 
                        (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "sar" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numAccel = (NUM) atof( szToken );
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numAccelMax = (NUM) atof( szToken );
                    pszNext = funcCopy( szToken, pszNext );
                    if ( pszNext )
                        break;
                    fRet = Sar( &m_dataset,
                                (LPDATASET) &gcache.hi( ),
                                (LPDATASET) &gcache.lo( ),
                                numAccel,
                                numAccelMax );
                    }
                else if ( lstrcmp( szToken, "std" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = StdDev( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "aroc" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Rate1( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "groc" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Rate2( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "nmin" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = NewMinimum( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "nmax" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = NewMaximum( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "shft" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Shift( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "lrg" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = LinReg( &m_dataset, &ds, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "corr" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds2, nRecurseLevel ) )
                        break;
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    numPeriod = (NUM) atof( szToken );
                    if ( pszNext )
                        break;
                    fRet = Correl( &m_dataset, &ds, &ds2, (WORD) numPeriod );
                    }
                else if ( lstrcmp( szToken, "abs" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );
                    
                    if ( pszNext )
                        break;
                    fRet = Absolute( &m_dataset, &ds );
                    }
                else if ( lstrcmp( szToken, "pow" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );

                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds2, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds2, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );

                    if ( pszNext )
                        break;
                    fRet = Power( &m_dataset, &ds, &ds2 );
                    }
                else if ( lstrcmp( szToken, "log" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );
                    
                    if ( pszNext )
                        break;
                    fRet = Log( &m_dataset, &ds );
                    }
                else if ( lstrcmp( szToken, "exp" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken )
                        break;
                    if ( szToken[ 0 ] == '#' || szToken[ 0 ] == '$' )
                        {
                        if ( !GetDataSet( szToken, ds, nRecurseLevel ) )
                            break;
                        }
                    else
                        setConstant( ds, _pdoc->GetCount( ), 0u, (NUM) atof( szToken ) );
                    
                    if ( pszNext )
                        break;
                    fRet = Exp( &m_dataset, &ds );
                    }
                else if ( lstrcmp( szToken, "pts" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    if ( pszNext )
                        break;

                    setConstant( m_dataset, _pdoc->GetCount( ), 0u, (NUM) ds.nPoints );
                    fRet = TRUE;
                    }
                else if ( lstrcmp( szToken, "offs" ) == 0 )
                    {
                    pszNext = funcCopy( szToken, pszNext );
                    if ( !*szToken || !GetDataSet( szToken, ds, nRecurseLevel ) )
                        break;
                    if ( pszNext )
                        break;

                    setConstant( m_dataset, _pdoc->GetCount( ), 0u, (NUM) ds.nOffset );
                    fRet = TRUE;
                    }
                else if ( lstrcmp( szToken, "year" ) == 0 )
                    {
                    if ( pszNext )
                        break;

                    setYear( m_dataset );
                    fRet = TRUE;
                    }
                else if ( lstrcmp( szToken, "mth" ) == 0 )
                    {
                    if ( pszNext )
                        break;

                    setMonth( m_dataset );
                    fRet = TRUE;
                    }
                else if ( lstrcmp( szToken, "mday" ) == 0 )
                    {
                    if ( pszNext )
                        break;

                    setDay( m_dataset );
                    fRet = TRUE;
                    }
                else if ( lstrcmp( szToken, "wday" ) == 0 )
                    {
                    if ( pszNext )
                        break;

                    setWeekDay( m_dataset );
                    fRet = TRUE;
                    }
                else if ( lstrcmp( szToken, "yday" ) == 0 )
                    {
                    if ( pszNext )
                        break;

                    setYearDay( m_dataset );
                    fRet = TRUE;
                    }
                else
                    {
                    break;
                    }
                }
            else
                {
                // We're dealing with a real indicator
                QIndicatorTemplate *pi = _ptpl->m_itlist.Find( m_sName );
                if ( !pi )
                    break;
                fRet = pi->BuildDataSet( &m_dataset, &m_datasetHorz, nRecurseLevel );
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        DataSetFree( &ds );
        DataSetFree( &ds2 );
        DataSetFree( &m_dataset );
        DataSetFree( &m_datasetHorz );
        THROW_LAST( );
        }
    END_CATCH
    DataSetFree( &ds );
    DataSetFree( &ds2 );
    if ( !fRet )
        {
        DataSetFree( &m_dataset );
        DataSetFree( &m_datasetHorz );
        }
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QCache::GetDataSet( )
//
// This member will get a dataset present in a user function.
//////////////////////////////////////////////////////////////////////////////
BOOL QCache::GetDataSet( const CHAR *pszName, DATASET &ds, int nRecurseLevel ) const
    {
    DATASET dsOp;
    DATASET dsHi;
    DATASET dsLo;
    DATASET dsCl;
    DATASET dsVl;
    DATASET dsHorz;
    DATASET dsX;
    DATASET dsCount;
    BOOL    fRet = FALSE;

    TRY
        {
        DataSetZero( &dsOp );
        DataSetZero( &dsHi );
        DataSetZero( &dsLo );
        DataSetZero( &dsCl );
        DataSetZero( &dsVl );
        DataSetZero( &dsHorz );
        DataSetZero( &dsX );
        DataSetZero( &dsCount );

        if ( *pszName == '$' )
            {
            pszName++;
            _pdoc->BuildQuoteData( &dsOp, &dsHi, &dsLo, &dsCl, &dsVl, &dsX, &dsCount );
            if ( lstrcmp( pszName, QDocument::m_szCl ) == 0 )
                {
                if ( !DataSetCopy( &ds, &dsCl ) )
                    THROW_EXCEPTION( exMemory );
                fRet = TRUE;
                }
            else if ( lstrcmp( pszName, QDocument::m_szOp ) == 0 )
                {
                if ( !DataSetCopy( &ds, &dsOp ) )
                    THROW_EXCEPTION( exMemory );
                fRet = TRUE;
                }
            else if ( lstrcmp( pszName, QDocument::m_szHi ) == 0 )
                {
                if ( !DataSetCopy( &ds, &dsHi ) )
                    THROW_EXCEPTION( exMemory );
                fRet = TRUE;
                }
            else if ( lstrcmp( pszName, QDocument::m_szLo ) == 0 )
                {
                if ( !DataSetCopy( &ds, &dsLo ) )
                    THROW_EXCEPTION( exMemory );
                fRet = TRUE;
                }
            else if ( lstrcmp( pszName, QDocument::m_szVl ) == 0 )
                {
                if ( !DataSetCopy( &ds, &dsVl ) )
                    THROW_EXCEPTION( exMemory );
                fRet = TRUE;
                }
            else if ( lstrcmp( pszName, QDocument::m_szX ) == 0 )
                {
                if ( !DataSetCopy( &ds, &dsX ) )
                    THROW_EXCEPTION( exMemory );
                fRet = TRUE;
                }
            else if ( lstrcmp( pszName, QDocument::m_szCount ) == 0 )
                {
                if ( !DataSetCopy( &ds, &dsCount ) )
                    THROW_EXCEPTION( exMemory );
                fRet = TRUE;
                }
            }
        else if ( *pszName == '#' )
            {
            pszName++;
            QIndicatorTemplate *pi = _ptpl->m_itlist.Find( pszName );
            if ( pi && pi->BuildDataSet( &ds, &dsHorz, nRecurseLevel ) )
                fRet = TRUE;
            }
        }
    CATCH_ALL
        {
        DataSetFree( &dsOp );
        DataSetFree( &dsHi );
        DataSetFree( &dsLo );
        DataSetFree( &dsCl );
        DataSetFree( &dsVl );
        DataSetFree( &dsHorz );
        DataSetFree( &dsX );
        DataSetFree( &dsCount );
        THROW_LAST( );
        }
    END_CATCH
    DataSetFree( &dsOp );
    DataSetFree( &dsHi );
    DataSetFree( &dsLo );
    DataSetFree( &dsCl );
    DataSetFree( &dsVl );
    DataSetFree( &dsHorz );
    DataSetFree( &dsX );
    DataSetFree( &dsCount );
    return fRet;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QCache::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QCache::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QCondCacheList::QCondCacheList( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QCondCacheList::QCondCacheList( )
    {
    }

QCondCacheList::~QCondCacheList( )
    {
    Cleanup( );
    }

void QCondCacheList::Cleanup( )
    {
    QCondCache *pcondcache;

    while ( pcondcache = (QCondCache *) RemoveHead( ) )
        {
        delete pcondcache;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QCondCacheList::Find( )
//
// Given a condition name, this routine will find the corresponding
// cache.
//////////////////////////////////////////////////////////////////////////////
QCondCache *QCondCacheList::Find( const CHAR *pszName ) const
    {
    for ( QCondCache *pcondcache = (QCondCache *) GetHead( ); 
          pcondcache; 
          pcondcache = (QCondCache *) GetNext( pcondcache ) )
        {
        ASSERT( pcondcache->GetName( ) != NULL );
        if ( lstrcmp( pcondcache->GetName( ), pszName ) == 0 )
            {
            return pcondcache;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QCondCacheList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QCondCacheList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QCondCache *pcondcache = (QCondCache *) GetHead( ); 
          pcondcache; 
          pcondcache = (QCondCache *) GetNext( pcondcache ) )
        {
        pcondcache->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QCacheList::QCacheList( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QCacheList::QCacheList( )
    {
    }

QCacheList::~QCacheList( )
    {
    Cleanup( );
    }
    
void QCacheList::Cleanup( )
    {
    QCache *pcache;

    while ( pcache = (QCache *) RemoveHead( ) )
        {
        delete pcache;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QCacheList::Find( )
//
// Given an indicator name, this routine will find the corresponding
// cache.
//////////////////////////////////////////////////////////////////////////////
QCache *QCacheList::Find( const CHAR *pszName ) const
    {
    for ( QCache *pcache = (QCache *) GetHead( ); 
          pcache; 
          pcache = (QCache *) GetNext( pcache ) )
        {
        ASSERT( pcache->GetName( ) != NULL );
        if ( lstrcmp( pcache->GetName( ), pszName ) == 0 )
            {
            return pcache;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QCacheList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QCacheList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QCache *pcache = (QCache *) GetHead( ); 
          pcache; 
          pcache = (QCache *) GetNext( pcache ) )
        {
        pcache->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QIndicatorName::QIndicatorName( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QIndicatorName::QIndicatorName( )
    {
    }

QIndicatorName::QIndicatorName( const CHAR *pszName )
    :m_sName( pszName )
    {
    }

QIndicatorName::QIndicatorName( const QIndicatorName &name )
    :m_sName( (QString) name )
    {
    }

QIndicatorName::~QIndicatorName( )
    {
    Cleanup( );
    }

void QIndicatorName::Cleanup( )
    {
    m_sName.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorName::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorName::Store( QArchive &ar )
    {
    ar.WriteHeader( otIndicatorName );
    m_sName.Store( ar );
    }

void QIndicatorName::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL f;

    if ( ot != otIndicatorName )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_sName.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorName::Set( )
//
// Sets the indicator name.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorName::Set( const CHAR *psz )
    {
    m_sName = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorName::operator QString( )
//
// QString( ) conversion operator.
//////////////////////////////////////////////////////////////////////////////
QIndicatorName::operator QString( ) const
    {
    return m_sName;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QIndicatorName::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorName::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QIndicatorDictionary::QIndicatorDictionary( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QIndicatorDictionary::QIndicatorDictionary( )
    {
    }

QIndicatorDictionary::~QIndicatorDictionary( )
    {
    Cleanup( );
    }

void QIndicatorDictionary::Cleanup( )
    {
    QIndicatorName *pName;

    while ( pName = (QIndicatorName *) RemoveHead( ) )
        {
        delete pName;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorDictionary::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorDictionary::Store( QArchive &ar )
    {
    ar.WriteHeader( otIndicatorDictionary );
    for( QIndicatorName *pName = (QIndicatorName *) GetHead( );
         pName;
         pName = (QIndicatorName *) GetNext( pName ) )
        {
        pName->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QIndicatorDictionary::Retrieve( QArchive &ar, UINT ot )
    {
    QIndicatorName *pName = 0;
    BOOL            fAdded = FALSE;
    
    TRY
        {
        if ( ot != otIndicatorDictionary )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }
        while ( TRUE )
            {
            pName = 0;
            fAdded = FALSE;
            BOOL f = ar.ReadHeader( &ot );
            if ( !f )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            if ( ot == otEndMarker )
                {
                break;
                }
            pName = new QIndicatorName;
            if ( !pName )
                {
                THROW_EXCEPTION( exMemory );
                }
            pName->Retrieve( ar, ot );
            AddTail( pName );
            fAdded = TRUE;
            }
        }    
    CATCH_ALL
        {                 
        if ( !fAdded )
            {
            delete pName;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorDictionary::operator=( )
//
// Assignment operator routine.
//////////////////////////////////////////////////////////////////////////////
const QIndicatorDictionary &QIndicatorDictionary::operator=( const QIndicatorDictionary &dict )
    {
    QIndicatorName *pNameNew;

    if ( &dict != this )
        {
        Cleanup( );
        for ( QIndicatorName *pName = (QIndicatorName *) dict.GetHead( ); 
              pName; 
              pName = (QIndicatorName *) dict.GetNext( pName ) )
            {
            pNameNew = new QIndicatorName( *pName );
            if ( !pNameNew )
                {
                THROW_EXCEPTION( exMemory );
                }
            AddTail( pNameNew );
            }
        }
    return *this;
    }

//////////////////////////////////////////////////////////////////////////////
// QIndicatorDictionary::Find( )
//
// Given a text name, returns a pointer to the indictor name in
// the indicator dictionary.
//////////////////////////////////////////////////////////////////////////////
QIndicatorName *QIndicatorDictionary::Find( const CHAR *pszName ) const
    {
    for ( QIndicatorName *pName = (QIndicatorName *) GetHead( ); 
          pName; 
          pName = (QIndicatorName *) GetNext( pName ) )
        {
        ASSERT( ( (const CHAR *)(QString) *pName ) != NULL );
        if ( lstrcmp( (const CHAR *)(QString) *pName, pszName ) == 0 )
            {
            return pName;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QIndicatorDictionary::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QIndicatorDictionary::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QIndicatorName *pName = (QIndicatorName *) GetHead( );
          pName;
          pName = (QIndicatorName *) GetNext( pName ) )
        {
        pName->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QTokenList::QTokenList( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QTokenList::QTokenList( )
    {
    }

QTokenList::~QTokenList( )
    {
    Cleanup( );
    }

void QTokenList::Cleanup( )
    {
    QToken *ptk;

    while ( ptk = (QToken *) RemoveHead( ) )
        {
        delete ptk;
        }
    }        

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QTokenList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QTokenList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QToken *ptk = (QToken *) GetHead( );
          ptk;
          ptk = (QToken *) GetNext( ptk ) )
        {
        ptk->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QTokenIter::QTokenIter( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QTokenIter::QTokenIter( const QTokenList &list )
    : m_list( list )
    {
    m_ptk = (QToken *) m_list.GetHead( );
    }

QTokenIter::~QTokenIter( )
    {
    Cleanup( );
    }

void QTokenIter::Cleanup( )
    {
    }

//////////////////////////////////////////////////////////////////////////////
// QTokenIter::operator()
//
// Iterate one token.  If we've already reached the end of the list,
// the iterator will continually return NULL.
//////////////////////////////////////////////////////////////////////////////
QToken *QTokenIter::operator()( )
    {
    if ( m_ptk )
        {
        QToken *ptkTmp = m_ptk;
        m_ptk = (QToken *) m_list.GetNext( m_ptk );
        return ptkTmp;
        }
    return NULL;
    }

//////////////////////////////////////////////////////////////////////////////
// QTokenIter::Peek( )
//
// Peek ahead one token, but do not iterate.
//////////////////////////////////////////////////////////////////////////////
QToken *QTokenIter::Peek( ) const
    {
    return m_ptk; // return the next token to be read.
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QTokenIter::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QTokenIter::AssertValid( ) const
    {
    QObject::AssertValid( );
    m_list.AssertValid( );
    }
#endif
            
//////////////////////////////////////////////////////////////////////////////
// QToken::QToken( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QToken::QToken( )
    {
    m_tk = tkUnknown;
    m_numValue = 0;
    }

QToken::~QToken( )
    {
    Cleanup( );
    }

void QToken::Cleanup( )
    {
    m_sName.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QToken::GetTk( )
//
// Gets the token type.
//////////////////////////////////////////////////////////////////////////////
QToken::Tk QToken::GetTk( ) const
    {
    return m_tk;
    }

//////////////////////////////////////////////////////////////////////////////
// QToken::SetTk( )
//
// Sets the token type.
//////////////////////////////////////////////////////////////////////////////
void QToken::SetTk( QToken::Tk tk )
    {
    ASSERT( tk >= tkUnknown && tk <= tkConst );
    m_tk = tk;
    }

//////////////////////////////////////////////////////////////////////////////
// QToken::GetName( )
//
// Gets the token name.  Only valid on applicable token types.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QToken::GetName( ) const
    {              
    return m_sName;
    }

//////////////////////////////////////////////////////////////////////////////
// QToken::SetName( )
//
// Sets the token name.  Only valid on applicable token types.
//////////////////////////////////////////////////////////////////////////////
void QToken::SetName( const CHAR *pszName )
    {
    m_sName = pszName;
    }

//////////////////////////////////////////////////////////////////////////////
// QToken::GetValue( )
//
// Gets the token value.  Only valid on applicable token types.
//////////////////////////////////////////////////////////////////////////////
NUM QToken::GetValue( ) const
    {
    return m_numValue;
    }

//////////////////////////////////////////////////////////////////////////////
// QToken::SetValue( )
//
// Sets the token value.  Only valid on applicable token types.
//////////////////////////////////////////////////////////////////////////////
void QToken::SetValue( NUM num )
    {
    m_numValue = num;
    }

#ifdef DEBUG                    
//////////////////////////////////////////////////////////////////////////////
// QToken::AssertValid( )
//
//
//////////////////////////////////////////////////////////////////////////////
void QToken::AssertValid( ) const
    {
    QListObject::AssertValid( );
    // tkUnknown is not really a 'valid' token type
    ASSERT( m_tk > tkUnknown && m_tk <= tkRule );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QCondition::QCondition( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QCondition::QCondition( )
    :m_nIdxClr( 0 )
    {
    }

QCondition::~QCondition( )
    {
    Cleanup( );
    }

void QCondition::Cleanup( )
    {
    m_sName.Cleanup( );
    m_sDefinition.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QCondition::Store( QArchive &ar )
    {
    ar.WriteHeader( otCondition );
    m_sName.Store( ar );
    m_sDefinition.Store( ar );
    ar.Write( &m_nIdxClr, sizeof( m_nIdxClr ) );
    }

void QCondition::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL f;
    USHORT cb;
    
    if ( ot != otCondition )
        THROW_EXCEPTION( exFileCorrupt );
        
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sName.Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sDefinition.Retrieve( ar, ot );

    cb = ar.Read( &m_nIdxClr, sizeof( m_nIdxClr ) );
    if ( cb != sizeof( m_nIdxClr ) )
        THROW_EXCEPTION( exFileCorrupt );
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::GetName( )
//
// Gets the condition name.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QCondition::GetName( ) const
    {
    return m_sName;
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::Export( )
//
// Exports a condition to a text file
//////////////////////////////////////////////////////////////////////////////
void QCondition::Export( FILE *pfile ) const
    {
    fprintf( pfile, "?%-16s: %s\n", GetName( ), RemoveCR( GetDefinition( ) ) );
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::SetName( )
//
// Sets the condition name.
//////////////////////////////////////////////////////////////////////////////
void QCondition::SetName( const CHAR *psz )
    {
    m_sName = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::GetDefinition( )
//
// Gets the condition definition.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QCondition::GetDefinition( ) const
    {
    return m_sDefinition;
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::SetDefinition( )
//
// Sets the condition definition.
//////////////////////////////////////////////////////////////////////////////
void QCondition::SetDefinition( const CHAR *psz )
    {
    m_sDefinition = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::GetIdxClr( )
//
// Gets the color index.
//////////////////////////////////////////////////////////////////////////////
USHORT QCondition::GetIdxClr( ) const
    {
    return m_nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::SetIdxClr( )
//
// Sets the color index.
//////////////////////////////////////////////////////////////////////////////
void QCondition::SetIdxClr( USHORT nIdxClr )
    {
    ASSERT( nIdxClr < RGCLRSIZE );
    m_nIdxClr = nIdxClr;
    }

//////////////////////////////////////////////////////////////////////////////
// QCondition::BuildDataSet( )
//
// Calls the parser to build the condition's dataset.
//////////////////////////////////////////////////////////////////////////////
BOOL QCondition::BuildDataSet( LPDATASET lpdataset, int nRecurseLevel )
    {
    DATASET     ds;
    UINT        i;
    QQt        *pqt;
    QParser::Rc rc;
    QTokenList  list;
    BOOL        fValue  = FALSE;
    BOOL        fError  = FALSE;
    BOOL        fFirst  = TRUE;
    UINT        nFirst  = 0u;
    WORD        nPoints = 0u;
    WORD        nOffset = 0u;
    
    TRY
        {
        TASKLIST
            {
            DataSetZero( &ds );
            DataSetZero( lpdataset );
            if ( !DataSetAlloc( &ds, _pdoc->GetCount( ), 0u ) )
                THROW_EXCEPTION( exMemory );

            if ( !QParser::Analyze( list, m_sDefinition, TRUE ) )
                {
                fError = TRUE;
                break;
                }

            for ( pqt = _pdoc->GetFirst( ), i = 0u;
                  pqt != NULL;
                  pqt = _pdoc->GetNext( pqt ), i++ )
                {
                QTokenIter iter( list );
                rc = QParser::condition( i, iter, fValue, nRecurseLevel, FALSE, 0, 0 );
                if ( rc == QParser::rcFatalError )
                    {
                    fError = TRUE;
                    break;
                    }
                if ( rc == QParser::rcUnAvail )
                    continue;
                
                if ( iter( ) )
                    {
                    Trace( TR_NORMAL, "QCondition::BuildDataSet: Unexpected Token\n" );
                    fError = TRUE;
                    break;
                    }

                ASSERT( rc == QParser::rcOk );
                
                if ( fFirst )
                    {
                    fFirst = FALSE;
                    nFirst = i;
                    }
                ds.lpnum[ i ] = ( fValue ? 1 : 0 );
                }
            
            if ( fError )
                break;

            nPoints = _pdoc->GetCount( ) - nFirst;
            nOffset = nFirst;
            if ( !DataSetAlloc( lpdataset, nPoints, nOffset ) )
                THROW_EXCEPTION( exMemory );
            
            for ( i = 0; i < lpdataset->nPoints; i++ )
                {
                lpdataset->lpnum[ i ] = ds.lpnum[ i + nOffset ];
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        DataSetFree( &ds );
        DataSetFree( lpdataset );
        THROW_LAST( );
        }
    END_CATCH

    DataSetFree( &ds );
    return !fError;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QCondition::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QCondition::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    m_sDefinition.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QRule::QRule( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QRule::QRule( )
    : m_sName( ), m_sBuy( ), m_sSell( ), m_sFilter( ), m_sFilter2( )
    {                                                  
    m_fNextOpen = FALSE;
    }

QRule::~QRule( )
    {
    Cleanup( );
    }

void QRule::Cleanup( )
    {
    m_sName.Cleanup( );
    m_sBuy.Cleanup( );
    m_sSell.Cleanup( );
    m_sFilter.Cleanup( );
    m_sFilter2.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QRule::Store( QArchive &ar )
    {
    ar.WriteHeader( otRule );
    ar.Write( &m_fNextOpen, sizeof( m_fNextOpen ) );
    m_sName.Store( ar );
    m_sBuy.Store( ar );
    m_sSell.Store( ar );
    m_sFilter.Store( ar );
    m_sFilter2.Store( ar );
    }

void QRule::Retrieve( QArchive &ar, UINT ot )
    {
    UINT cb;
    BOOL f;
    
    if ( ot != otRule )
        THROW_EXCEPTION( exFileCorrupt );
        
    cb = ar.Read( &m_fNextOpen, sizeof( m_fNextOpen ) );
    if ( cb != sizeof( m_fNextOpen ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sName.Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sBuy.Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sSell.Retrieve( ar, ot );
    
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sFilter.Retrieve( ar, ot );

    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sFilter2.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::Export( )
//
// Exports the contents of a rule to a text file
//////////////////////////////////////////////////////////////////////////////
void QRule::Export( FILE *pfile ) const
    {
    fprintf( pfile, "~%s\n", GetName( ) );
    fprintf( pfile, "     %-12s: %s\n", "Enter/Exit", ( GetNextOpenFlag( ) ? "On next open" : "On current close" ) );
    fprintf( pfile, "     %-12s: %s\n", "Entry",         RemoveCR( GetBuyString( ) ) );
    fprintf( pfile, "     %-12s: %s\n", "Exit",          RemoveCR( GetSellString( ) ) );
    fprintf( pfile, "     %-12s: %s\n", "Entry Filter",  RemoveCR( GetFilterString( ) ) );
    fprintf( pfile, "     %-12s: %s\n", "Exit Filter", RemoveCR( GetFilter2String( ) ) );
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::GetNextOpenFlag( )
//
// Returns the value of the next open flag
//////////////////////////////////////////////////////////////////////////////
BOOL QRule::GetNextOpenFlag( ) const
    {
    return m_fNextOpen;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::SetNextOpenFlag( )
//
// Sets the value of the next open flag
//////////////////////////////////////////////////////////////////////////////
void QRule::SetNextOpenFlag( BOOL f )
    {
    m_fNextOpen = f;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::GetName( )
//
// Returns the name of the rule.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QRule::GetName( ) const
    {
    return m_sName;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::SetName( )
//
// Sets the name of the rule.
//////////////////////////////////////////////////////////////////////////////
void QRule::SetName( const CHAR *psz )
    {
    m_sName = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::GetBuyString( )
//
// Returns the buy string of the rule.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QRule::GetBuyString( ) const
    {
    return m_sBuy;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::SetBuyString( )
//
// Sets the buy string of the rule.
//////////////////////////////////////////////////////////////////////////////
void QRule::SetBuyString( const CHAR *psz )
    {
    m_sBuy = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::GetSellString( )
//
// Returns the buy string of the rule.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QRule::GetSellString( ) const
    {
    return m_sSell;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::SetSellString( )
//
// Sets the buy string of the rule.
//////////////////////////////////////////////////////////////////////////////
void QRule::SetSellString( const CHAR *psz )
    {
    m_sSell = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::GetFilterString( )
//
// Gets the filter string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QRule::GetFilterString( ) const
    {
    return m_sFilter;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::SetFilterString( )
//
// Sets the filter string.
//////////////////////////////////////////////////////////////////////////////
void QRule::SetFilterString( const CHAR *psz )
    {
    m_sFilter = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::GetFilter2String( )
//
// Gets the exit filter string.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QRule::GetFilter2String( ) const
    {
    return m_sFilter2;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::SetFilter2String( )
//
// Sets the exit filter string.
//////////////////////////////////////////////////////////////////////////////
void QRule::SetFilter2String( const CHAR *psz )
    {
    m_sFilter2 = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::Evaluate( )
//
// Evaluates a rule.
//
// Parameters:
//
// sglist            - The built list of signal objects.
// fIgnoreFirst      - TRUE if you want to ignore the very first transaction.
// fIgnoreIncomplete - TRUE if you want to ignore the last incomplete transaction,
//                     if one exists.
// nRecurseLevel     - Recursion level.
//
// Returns: TRUE if the evaulation was successful.
///         FALSE otherwise.
//////////////////////////////////////////////////////////////////////////////
BOOL QRule::Evaluate( QSignalList &sglist, BOOL fIgnoreFirst, BOOL fIgnoreIncomplete, int nRecurseLevel )
    {                        
    BOOL fAdded( FALSE );
    BOOL fError( FALSE );
    QTokenList listBuy;
    QTokenList listSell;
    QTokenList listFilter;
    QTokenList listFilter2;
    QSignal *psg = 0;
    
    TRY
        {   
        TASKLIST
            {                
            // Clean out any old signals first
            sglist.Cleanup( );
            
            // Now, perform lexical analysis on the buy and sell strings
            if ( !QParser::Analyze( listBuy, m_sBuy, TRUE ) )
                {
                fError = TRUE;
                break;
                }
#ifdef DEBUG
            listBuy.AssertValid( );
#endif                
            
            if ( !QParser::Analyze( listSell, m_sSell, TRUE ) )
                {
                fError = TRUE;
                break;
                }          
#ifdef DEBUG
            listSell.AssertValid( );
#endif                
            
            if ( !QParser::Analyze( listFilter, m_sFilter, TRUE ) )
                {
                fError = TRUE;
                break;
                }
#ifdef DEBUG
            listFilter.AssertValid( );
#endif
            
            if ( !QParser::Analyze( listFilter2, m_sFilter2, TRUE ) )
                {
                fError = TRUE;
                break;
                }
#ifdef DEBUG
            listFilter2.AssertValid( );
#endif
            
            QParser::Rc rc;
            BOOL fValue;
            UINT i;
            const QQt *pqt;
            QSignal::SignalType sgCurr = QSignal::sgBuy;
            
            // for @etp( ) and @bsi( )
            NUM  numEntry = 0;
            UINT iEntry = 0;
            NUM  numTrail = 0;

            for ( i = 0, pqt = _pdoc->GetFirst( );
                  pqt && !fError;
                  i++, pqt = _pdoc->GetNext( pqt ) )
                {   
                psg = 0;
                fAdded = FALSE;
                if ( sgCurr == QSignal::sgBuy )
                    {    
                    // We're looking for buy signals...
                    QTokenIter iter( listBuy );
                    rc = QParser::program( i, iter, fValue, nRecurseLevel );
                    }
                else
                    {
#ifdef _ADJUST_TRS_WITH_OPEN
                    // Adjust our trailing stop
                    if ( GetNextOpenFlag( ) )
                        {
                        if ( pqt->GetOp( ) > numTrail )
                            numTrail = pqt->GetOp( );
                        }
                    else
#endif
                        {
                        if ( pqt->GetCl( ) > numTrail )
                            numTrail = pqt->GetCl( );
                        }

                    // Should be a price set
                    ASSERT( numTrail != 0 );

                    // We're looking for sell signals...
                    QTokenIter iter( listSell );
                    rc = QParser::program( i, iter, fValue, nRecurseLevel, TRUE, numEntry, iEntry, numTrail );
                    }
                
                if ( rc == QParser::rcUnAvail )
                    {
                    if ( sgCurr == QSignal::sgSell )
                        {
                        // If looking for sells and not available, must
                        // delete the previous BUY
                        ASSERT( sglist.GetCount( ) == 1u );
                        QSignal *psgT = (QSignal *) sglist.RemoveHead( );
                        ASSERT( psgT != NULL );
                        ASSERT( psgT->GetSg( ) == QSignal::sgBuy );
                        delete psgT;

                        // Must re-visit the current quote
                        ASSERT( i != 0 );
                        i--;
                        ASSERT( _pdoc->GetPrev( pqt ) != NULL );
                        pqt = _pdoc->GetPrev( pqt );

                        // We are now looking for BUYS again
                        sgCurr = QSignal::sgBuy;
                        }

                    continue;
                    }                 
                    
                if ( rc == QParser::rcFatalError )
                    {
                    fError = TRUE;
                    break;
                    }
                
                if ( fValue )
                    {
                    // A signal has been found
                    QSignal *psg = new QSignal;
                    if ( !psg )
                        {
                        THROW_EXCEPTION( exMemory );
                        }
                    psg->SetDate( pqt->GetDate( ) );
                    psg->SetSg( sgCurr );
                        
                    if ( GetNextOpenFlag( ) )
                        {
                        QQt *pqtNext = _pdoc->GetNext( pqt );

                        // If on the last one, we will never look
                        // at the signal price
                        if ( pqtNext )
                            psg->SetPrice( pqtNext->GetOp( ) );
                        else
                            {
                            psg->SetPendingState( TRUE );
                            psg->SetPrice( 0 );
                            }
                        }
                    else
                        {
                        psg->SetPrice( pqt->GetCl( ) );
                        }
                        
                    sglist.AddTail( psg );
                    fAdded = TRUE;       
                    if ( sgCurr == QSignal::sgBuy )
                        {
                        // support for @etp( ) and @bsi( )
                        numEntry = psg->GetPrice( );
                        iEntry   = i;
                        numTrail = psg->GetPrice( );
                        Trace( TR_NORMAL, "sgBuy[%u]: numEntry = %f\n", i, numEntry );
                        Trace( TR_NORMAL, "sgBuy[%u]: iEntry = %u\n", i, iEntry );
                        Trace( TR_NORMAL, "sgBuy[%u]: numTrail = %f\n", i, numTrail );
                        }
                    else
                        {
                        numTrail = 0;
                        Trace( TR_NORMAL, "sgSell[%u]: numTrail = %f\n", i, numTrail );
                        }

                    sgCurr = ( sgCurr == QSignal::sgBuy ? QSignal::sgSell : QSignal::sgBuy );
                    }
                }
            if ( fError )
                break;

            // Design Decision: Everything should be 0-based offset
            ASSERT( sglist.offset( ) == 0u );

            // Now, perform filtering.
            if ( !HandleFiltering( sglist, listFilter, nRecurseLevel ) )
                {
                fError = TRUE;
                break;
                }

            if ( !HandleFiltering2( sglist, listFilter2, nRecurseLevel ) )
                {
                fError = TRUE;
                break;
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {   
        listBuy.Cleanup( );
        listSell.Cleanup( );
        listFilter.Cleanup( );
        sglist.Cleanup( );
        if ( !fAdded )
            {
            delete psg;
            }
        THROW_LAST( );
        }
    END_CATCH
    if ( fError )
        {
        sglist.Cleanup( );
        if ( !fAdded )
            {
            delete psg;
            }
        }
    return !fError;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::HandleFiltering2( )
//
// This rule will perform post-processing exit-filtering on a signal list.
//////////////////////////////////////////////////////////////////////////////
BOOL QRule::HandleFiltering2( QSignalList &sglist, const QTokenList &listFilter, int nRecurseLevel ) const
    {
    QParser::Rc rc;
    BOOL     fError = FALSE;
    BOOL     fValue;
    QSignal *psg = NULL;
    QQt     *pqt = NULL;
    int      i;
    
    // for @etp( ) and @bsi( )
    NUM  numEntry = 0;
    NUM  numTrail = 0;
    UINT iEntry = 0;
    
    if ( listFilter.GetCount( ) == 0u )
        return TRUE;

    for ( pqt = _pdoc->GetFirst( ), psg = (QSignal *) sglist.GetHead( ), i = 0;
          pqt != NULL && psg != NULL && !fError;
          pqt = _pdoc->GetNext( pqt ), i++ )
        {
        ASSERT( psg->GetSg( ) == QSignal::sgBuy );
        if ( pqt->GetDate( ) == psg->GetDate( ) )
            {
            numEntry = psg->GetPrice( );
            iEntry = i;
            numTrail = psg->GetPrice( );
            }
        else if ( pqt->GetDate( ) > psg->GetDate( ) )
            {
            // Adjust trailing stop
            if ( GetNextOpenFlag( ) )
                {
                if ( pqt->GetOp( ) > numTrail )
                    numTrail = pqt->GetOp( );
                }
            else
                {
                if ( pqt->GetCl( ) > numTrail )
                    numTrail = pqt->GetCl( );
                }
            
            // Trailing stop should be set
            ASSERT( numTrail != 0 );

            QTokenIter iter( listFilter );
            rc = QParser::program( i, iter, fValue, nRecurseLevel, TRUE, numEntry, iEntry, numTrail );
            if ( rc == QParser::rcFatalError )
                {
                fError = TRUE;
                break;
                }
            
            // Get current sell signal
            QSignal *psgSell = (QSignal *) sglist.GetNext( psg );
            
            // If unavailable, then remove both Buy and Sell
            if ( rc == QParser::rcUnAvail )
                {
                // We must filter out the buy and sell signals.
                ASSERT( psg->GetSg( ) == QSignal::sgBuy );
                QSignal *psgOld = psg;
                psg = (QSignal *) sglist.GetNext( psg );
                BOOL f = sglist.Remove( psgOld );
                ASSERT( f );
                delete psgOld;

                if ( psg != NULL )
                    {
                    ASSERT( psg->GetSg( ) == QSignal::sgSell );
                    psgOld = psg;
                    psg = (QSignal *) sglist.GetNext( psg );
                    f = sglist.Remove( psgOld );
                    ASSERT( f );
                    delete psgOld;
                    }

                // Also have to get the sell signal too
                if ( psg )
                    {
                    ASSERT( psg->GetSg( ) == QSignal::sgBuy );
                    psgSell = (QSignal *) sglist.GetNext( psg );
#ifdef DEBUG                    
                    if ( psgSell )
                        ASSERT( psgSell->GetSg( ) == QSignal::sgSell );
#endif
                    }
                else
                    psgSell = NULL;
                }
            else if ( rc == QParser::rcOk && fValue )
                {
                if ( !psgSell )
                    {
                    // If doesn't exist, then we have an open position;
                    // need to create a new sell signal
                    psgSell = new QSignal;
                    if ( !psgSell )
                        {
                        THROW_EXCEPTION( exMemory );
                        }
                    psgSell->SetSg( QSignal::sgSell );
                    sglist.InsertAfter( psg, psgSell );
                        
                    // Nothing should be after this sell if we just inserted it
                    ASSERT( sglist.GetNext( psgSell ) == NULL );
                    }
                 
                // 'Move' the Sell signal to the current position
                psgSell->SetDate( pqt->GetDate( ) );
                if ( GetNextOpenFlag( ) )
                    {
                    QQt *pqtNext = _pdoc->GetNext( pqt );
                    if ( pqtNext )
                        {
                        psgSell->SetPendingState( FALSE );
                        psgSell->SetPrice( pqtNext->GetOp( ) );
                        }
                    else
                        {
                        psgSell->SetPendingState( TRUE );
                        psgSell->SetPrice( 0 );
                        }
                    }
                else
                    psgSell->SetPrice( pqt->GetCl( ) );
                }
            
            // Next signal is the one after the sell
            if ( psgSell && psgSell->GetDate( ) == pqt->GetDate( ) )
                psg = (QSignal *) sglist.GetNext( psgSell );
            }
        }

    return !fError;
    }

//////////////////////////////////////////////////////////////////////////////
// QRule::HandleFiltering( )
//
// This rule will perform post-processing filtering on an signal list.
//////////////////////////////////////////////////////////////////////////////
BOOL QRule::HandleFiltering( QSignalList &sglist, const QTokenList &listFilter, int nRecurseLevel ) const
    {
    QParser::Rc rc;
    BOOL     fError = FALSE;
    BOOL     fValue;
    QSignal *psg = NULL;
    QQt     *pqt = NULL;
    int      i;

    if ( listFilter.GetCount( ) == 0u )
        return TRUE;

    for ( pqt = _pdoc->GetFirst( ), psg = (QSignal *) sglist.GetHead( ), i = 0;
          pqt != NULL && psg != NULL && !fError;
          pqt = _pdoc->GetNext( pqt ), i++ )
        {
        ASSERT( psg->GetSg( ) == QSignal::sgBuy );
        if ( pqt->GetDate( ) == psg->GetDate( ) )
            {
            QTokenIter iter( listFilter );
            rc = QParser::program( i, iter, fValue, nRecurseLevel );
            if ( rc == QParser::rcFatalError )
                {
                fError = TRUE;
                break;
                }
            if ( rc == QParser::rcUnAvail || ( rc == QParser::rcOk && !fValue ) )
                {
                // We must filter out the buy and sell signals.
                ASSERT( psg->GetSg( ) == QSignal::sgBuy );
                QSignal *psgOld = psg;
                psg = (QSignal *) sglist.GetNext( psg );
                BOOL f = sglist.Remove( psgOld );
                ASSERT( f );
                delete psgOld;

                if ( psg != NULL )
                    {
                    ASSERT( psg->GetSg( ) == QSignal::sgSell );
                    psgOld = psg;
                    psg = (QSignal *) sglist.GetNext( psg );
                    f = sglist.Remove( psgOld );
                    ASSERT( f );
                    delete psgOld;
                    }
                }
            else
                {
                psg = (QSignal *) sglist.GetNext( psg );
                if ( psg != NULL )
                    {
                    ASSERT( psg->GetSg( ) == QSignal::sgSell );
                    psg = (QSignal *) sglist.GetNext( psg );
#ifdef DEBUG
                    if ( psg != NULL )
                        ASSERT( psg->GetSg( ) == QSignal::sgBuy );
#endif
                    }
                }
            }
        }
    return !fError;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QRule::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QRule::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    m_sBuy.AssertValid( );
    m_sSell.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QConstant::QConstant( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QConstant::QConstant( )
    :m_numValue( 0 )
    {
    }

QConstant::~QConstant( )
    {
    Cleanup( );
    }

void QConstant::Cleanup( )
    {
    m_sName.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QConstant::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QConstant::Store( QArchive &ar )
    {
    ar.WriteHeader( otConstant );
    m_sName.Store( ar );
    ar.Write( &m_numValue, sizeof( m_numValue ) );
    }

void QConstant::Retrieve( QArchive &ar, UINT ot )
    {
    BOOL f;
    USHORT cb;
    
    if ( ot != otConstant )
        THROW_EXCEPTION( exFileCorrupt );
        
    f = ar.ReadHeader( &ot );
    if ( !f )
        THROW_EXCEPTION( exFileCorrupt );
    m_sName.Retrieve( ar, ot );
    
    cb = ar.Read( &m_numValue, sizeof( m_numValue ) );
    if ( cb != sizeof( m_numValue ) )
        THROW_EXCEPTION( exFileCorrupt );
    }

//////////////////////////////////////////////////////////////////////////////
// QConstant::Export( )
//
// Exports a constant definition to a text file
//////////////////////////////////////////////////////////////////////////////
void QConstant::Export( FILE *pfile ) const
    {
    fprintf( pfile, "^%-16s: %f\n", RemoveCR( GetName( ) ), GetValue( ) );
    }

//////////////////////////////////////////////////////////////////////////////
// QConstant::GetName( )
//
// Gets the constant name.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QConstant::GetName( ) const
    {
    return m_sName;
    }

//////////////////////////////////////////////////////////////////////////////
// QConstant::SetName( )
//
// Sets the constant name.
//////////////////////////////////////////////////////////////////////////////
void QConstant::SetName( const CHAR *psz )
    {
    m_sName = psz;
    }

//////////////////////////////////////////////////////////////////////////////
// QConstant::GetValue( )
//
// Gets the constant value.
//////////////////////////////////////////////////////////////////////////////
NUM QConstant::GetValue( ) const
    {
    return m_numValue;
    }

//////////////////////////////////////////////////////////////////////////////
// QConstant::SetValue( )
//
// Sets the constant value.
//////////////////////////////////////////////////////////////////////////////
void QConstant::SetValue( NUM num )
    {
    m_numValue = num;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QConstant::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QConstant::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QConstantList::QConstantList( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QConstantList::QConstantList( )
    {
    }

QConstantList::~QConstantList( )
    {
    Cleanup( );
    }

void QConstantList::Cleanup( )
    {
    QConstant *pconst;

    while ( pconst = (QConstant *) RemoveHead( ) )
        {
        delete pconst;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QConstantList::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QConstantList::Store( QArchive &ar )
    {
    ar.WriteHeader( otConstantList );
    for ( QConstant *pconst = (QConstant *) GetHead( );
          pconst;
          pconst = (QConstant *) GetNext( pconst ) )
        {
        pconst->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QConstantList::Retrieve( QArchive &ar, UINT ot )
    {
    QConstant *pconst = 0;
    BOOL       fAdded = FALSE;
    
    TRY
        {    
        if ( ot != otConstantList )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }
        while ( TRUE )
            {
            pconst = 0;
            fAdded = FALSE;
            BOOL f = ar.ReadHeader( &ot );
            if ( !f )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            if ( ot == otEndMarker )
                {
                break;
                }
            pconst = new QConstant;
            if ( !pconst )
                {
                THROW_EXCEPTION( exMemory );
                }
            pconst->Retrieve( ar, ot );
            AddTail( pconst );
            fAdded = TRUE;
            }
        }    
    CATCH_ALL
        {
        if ( !fAdded )
            {
            delete pconst;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QConstantList::Export( )
//
// Exports constants to a text file
//////////////////////////////////////////////////////////////////////////////
void QConstantList::Export( FILE *pfile ) const
    {
    fprintf( pfile, "Constants\n" );
    fprintf( pfile, "------------------\n\n" );

    for ( QConstant *pconst = (QConstant *) GetHead( );
          pconst;
          pconst = (QConstant *) GetNext( pconst ) )
        {
        pconst->Export( pfile );
        }
    fprintf( pfile, "\n\n" );
    }

//////////////////////////////////////////////////////////////////////////////
// QConstantList::Find( )
//
// Given a name, find the associated value.
//////////////////////////////////////////////////////////////////////////////
QConstant *QConstantList::Find( const CHAR *psz ) const 
    {
    for ( QConstant *pconst = (QConstant *) GetHead( );
          pconst;
          pconst = (QConstant *) GetNext( pconst ) )
        {
        ASSERT( pconst->GetName( ) != NULL );
        if ( lstrcmp( pconst->GetName( ), psz ) == 0 )
            {
            return pconst;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QConstantList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QConstantList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QConstant *pconst = (QConstant *) GetHead( );
          pconst;
          pconst = (QConstant *) GetNext( pconst ) )
        {
        pconst->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QConditionList::QConditionList( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QConditionList::QConditionList( )
    {
    }

QConditionList::~QConditionList( )
    {
    Cleanup( );
    }

void QConditionList::Cleanup( )
    {
    QCondition *pcond;

    while ( pcond = (QCondition *) RemoveHead( ) )
        {
        delete pcond;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QConditionList::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QConditionList::Store( QArchive &ar )
    {
    ar.WriteHeader( otConditionList );
    for ( QCondition *pcond = (QCondition *) GetHead( );
          pcond;
          pcond = (QCondition *) GetNext( pcond ) )
        {
        pcond->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QConditionList::Retrieve( QArchive &ar, UINT ot )
    {
    QCondition *pcond = 0;
    BOOL       fAdded = FALSE;
    
    TRY
        {    
        if ( ot != otConditionList )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }
        while ( TRUE )
            {
            pcond = 0;
            fAdded = FALSE;
            BOOL f = ar.ReadHeader( &ot );
            if ( !f )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            if ( ot == otEndMarker )
                {
                break;
                }
            pcond = new QCondition;
            if ( !pcond )
                {
                THROW_EXCEPTION( exMemory );
                }
            pcond->Retrieve( ar, ot );
            AddTail( pcond );
            fAdded = TRUE;
            }
        }    
    CATCH_ALL
        {
        if ( !fAdded )
            {
            delete pcond;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QConditionList::Export( )
//
// Exports conditions to a text file
//////////////////////////////////////////////////////////////////////////////
void QConditionList::Export( FILE *pfile ) const
    {
    fprintf( pfile, "Conditions\n" );
    fprintf( pfile, "------------------\n\n" );

    for ( QCondition *pcond = (QCondition *) GetHead( );
          pcond;
          pcond = (QCondition *) GetNext( pcond ) )
        {
        pcond->Export( pfile );
        }
    fprintf( pfile, "\n\n" );
    }

//////////////////////////////////////////////////////////////////////////////
// QConditionList::Find( )
//
// Given a name, find the associated condition.
//////////////////////////////////////////////////////////////////////////////
QCondition *QConditionList::Find( const CHAR *psz ) const 
    {
    for ( QCondition *pcond = (QCondition *) GetHead( );
          pcond;
          pcond = (QCondition *) GetNext( pcond ) )
        {
        ASSERT( pcond->GetName( ) != NULL );
        if ( lstrcmp( pcond->GetName( ), psz ) == 0 )
            {
            return pcond;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QConditionList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QConditionList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QCondition *pcond = (QCondition *) GetHead( );
          pcond;
          pcond = (QCondition *) GetNext( pcond ) )
        {
        pcond->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QRuleList::QRuleList( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QRuleList::QRuleList( )
    {
    }

QRuleList::~QRuleList( )
    {
    Cleanup( );
    }

void QRuleList::Cleanup( )
    {
    QRule *prule;

    while ( prule = (QRule *) RemoveHead( ) )
        {
        delete prule;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QRuleList::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QRuleList::Store( QArchive &ar )
    {
    ar.WriteHeader( otRuleList );
    for ( QRule *prule = (QRule *) GetHead( );
          prule;
          prule = (QRule *) GetNext( prule ) )
        {
        prule->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QRuleList::Retrieve( QArchive &ar, UINT ot )
    {
    QRule *prule = 0;
    BOOL   fAdded = FALSE;
    
    TRY
        {    
        if ( ot != otRuleList )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }
        while ( TRUE )
            {
            prule = 0;
            fAdded = FALSE;
            BOOL f = ar.ReadHeader( &ot );
            if ( !f )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            if ( ot == otEndMarker )
                {
                break;
                }
            prule = new QRule;
            if ( !prule )
                {
                THROW_EXCEPTION( exMemory );
                }
            prule->Retrieve( ar, ot );
            AddTail( prule );
            fAdded = TRUE;
            }
        }    
    CATCH_ALL
        {
        if ( !fAdded )
            {
            delete prule;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QRuleList::Export( )
//
// Exports the list of rules to a text file
//////////////////////////////////////////////////////////////////////////////
void QRuleList::Export( FILE *pfile ) const
    {
    fprintf( pfile, "Trading Rules\n" );
    fprintf( pfile, "------------------\n\n" );

    for ( QRule *prule = (QRule *) GetHead( );
          prule;
          prule = (QRule *) GetNext( prule ) )
        {
        prule->Export( pfile );
        }
    fprintf( pfile, "\n\n" );
    }

//////////////////////////////////////////////////////////////////////////////
// QRuleList::Find( )
//
// Given a name, find the associated rule.
//////////////////////////////////////////////////////////////////////////////
QRule *QRuleList::Find( const CHAR *psz ) const
    {
    for ( QRule *prule = (QRule *) GetHead( );
          prule;
          prule = (QRule *) GetNext( prule ) )
        {
        ASSERT( prule->GetName( ) != NULL );
        if ( lstrcmp( prule->GetName( ), psz ) == 0 )
            {
            return prule;
            }
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QRuleList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QRuleList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QRule *prule = (QRule *) GetHead( );
          prule;
          prule = (QRule *) GetNext( prule ) )
        {
        prule->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QCondCache::QCondCache( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QCondCache::QCondCache( )
    {
    DataSetZero( &m_dataset );
    }

QCondCache::~QCondCache( )
    {
    Cleanup( );
    }

void QCondCache::Cleanup( )
    {
    DataSetFree( &m_dataset );
    }

//////////////////////////////////////////////////////////////////////////////
// QCondCache::SetDataSet( )
//
// Sets the condition cache's dataset.
//////////////////////////////////////////////////////////////////////////////
void QCondCache::SetDataSet( DATASET &dataset )
    {
    if ( !DataSetCopy( &m_dataset, &dataset ) )
        THROW_EXCEPTION( exMemory );
    }

//////////////////////////////////////////////////////////////////////////////
// QCondCache::Build( )
//
// Builds a condition cache.
//////////////////////////////////////////////////////////////////////////////
BOOL QCondCache::Build( int nRecurseLevel )
    {
    BOOL fRet = FALSE;

    TRY
        {
        DataSetFree( &m_dataset );
        TASKLIST
            {
            QCondition *pcond = _ptpl->m_condlist.Find( m_sName );
            if ( !pcond )
                break;
            fRet = pcond->BuildDataSet( &m_dataset, nRecurseLevel );
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        DataSetFree( &m_dataset );
        THROW_LAST( );
        }
    END_CATCH
    
    if ( !fRet )
        DataSetFree( &m_dataset );
    return fRet;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QCondCache::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QCondCache::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QRuleCache::QRuleCache( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QRuleCache::QRuleCache( )
    {
    DataSetZero( &m_dataset );
    }

QRuleCache::~QRuleCache( )
    {
    Cleanup( );
    }

void QRuleCache::Cleanup( )
    {
    DataSetFree( &m_dataset );
    }

//////////////////////////////////////////////////////////////////////////////
// QRuleCache::SetDataSet( )
//
// This method sets the internal dataset to dataset.
//////////////////////////////////////////////////////////////////////////////
void QRuleCache::SetDataSet( DATASET &dataset )
    {
    if ( !DataSetCopy( &m_dataset, &dataset ) )
        THROW_EXCEPTION( exMemory );
    }

//////////////////////////////////////////////////////////////////////////////
// QRuleCache::Build( )
//
// This routine builds a rule cache.
//////////////////////////////////////////////////////////////////////////////
BOOL QRuleCache::Build( int nRecurseLevel )
    {
    BOOL         fRet = FALSE;
    WORD         nPoints;
    WORD         nOffset;
    WORD         n;
    DATASET      dataset;
    QRule       *prule;
    QSignalList  sglist;
    QSignal     *psg;
    WORD         i;
    QQt         *pqt;
    NUM          numVal;

    TRY
        {
        TASKLIST
            {
            DataSetZero( &dataset );
            prule = _ptpl->m_rulelist.Find( m_sName );
            if ( !prule )
                QUIT;

            if ( !prule->Evaluate( sglist, FALSE, FALSE, nRecurseLevel ) )
                QUIT;

            // Now, we have to convert the sglist into a DATASET
            ASSERT( _pdoc->GetCount( ) >= sglist.offset( ) );
            nPoints = _pdoc->GetCount( ) - sglist.offset( );
            nOffset = sglist.offset( );
            if ( !DataSetAlloc( &dataset, nPoints, nOffset ) )
                THROW_EXCEPTION( exMemory );

            numVal = (NUM) 0;
            psg = (QSignal *) sglist.GetHead( );
            for ( i = 0, pqt = _pdoc->GetFirst( );
                  pqt;
                  i++, pqt = _pdoc->GetNext( pqt ) )
                {
                // don't even look at these these guys!!
                if ( i < nOffset )
                    continue;

                if ( psg && ( psg->GetDate( ) == pqt->GetDate( ) ) )
                    {
                    numVal = psg->GetSg( ) == QSignal::sgBuy ? (NUM) 1 : (NUM) 0;
                    psg = (QSignal *) sglist.GetNext( psg );
                    }
                n = i - nOffset;
                ASSERT( n < _pdoc->GetCount( ) );
                dataset.lpnum[ n ] = numVal;
                }
            
            SetDataSet( dataset );
            fRet = TRUE;
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        sglist.Cleanup( );
        DataSetFree( &dataset );
        THROW_LAST( );
        }
    END_CATCH
    DataSetFree( &dataset );
    return fRet;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QRuleCache::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QRuleCache::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QRuleCacheList::QRuleCacheList( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QRuleCacheList::QRuleCacheList( )
    {
    }

QRuleCacheList::~QRuleCacheList( )
    {
    Cleanup( );
    }

void QRuleCacheList::Cleanup( )
    {
    QRuleCache *prulecache;

    while ( prulecache = (QRuleCache *) RemoveHead( ) )
        {
        delete prulecache;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QRuleCacheList::Find( )
//
// Given a name, find the associated rule cache.
//////////////////////////////////////////////////////////////////////////////
QRuleCache *QRuleCacheList::Find( const CHAR *psz ) const
    {
    for ( QRuleCache *prulecache = (QRuleCache *) GetHead( );
          prulecache;
          prulecache = (QRuleCache *) GetNext( prulecache ) )
        {
        ASSERT( prulecache->GetName( ) != NULL );
        if ( lstrcmp( prulecache->GetName( ), psz ) == 0 )
            return prulecache;
        }
    return NULL;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QRuleCacheList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QRuleCacheList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QRuleCache *prulecache = (QRuleCache *) GetHead( );
          prulecache;
          prulecache = (QRuleCache *) GetNext( prulecache ) )
        {
        prulecache->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QSignal::QSignal( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QSignal::QSignal( )
    :m_numPrice( 0.0 ), m_fPendingState( FALSE )
    {
    }
    
QSignal::~QSignal( )
    {
    Cleanup( );
    }

void QSignal::Cleanup( )
    {
    m_date.Cleanup( );
    m_fPendingState = FALSE;
    }
    
//////////////////////////////////////////////////////////////////////////////
// QSignal::GetPendingState( )
//
// Gets the pending state of the signal
//////////////////////////////////////////////////////////////////////////////
BOOL QSignal::GetPendingState( ) const
    {
    return m_fPendingState;
    }

//////////////////////////////////////////////////////////////////////////////
// QSignal::SetPendingState( )
//
// Sets the pending state of the signal
//////////////////////////////////////////////////////////////////////////////
void QSignal::SetPendingState( BOOL f )
    {
    m_fPendingState = f;
    }

//////////////////////////////////////////////////////////////////////////////
// QSignal::GetDate( )
//
// Gets the signal's date.
//////////////////////////////////////////////////////////////////////////////
const QTime &QSignal::GetDate( ) const
    {
    return m_date;
    }
    
//////////////////////////////////////////////////////////////////////////////
// QSignal::SetDate( )
//
// Sets the signal's date.
//////////////////////////////////////////////////////////////////////////////
void QSignal::SetDate( const QTime &date )
    {
    m_date = date;
    }
    
//////////////////////////////////////////////////////////////////////////////
// QSignal::GetSg( )
//
// Gets the signal's type.
//////////////////////////////////////////////////////////////////////////////
QSignal::SignalType QSignal::GetSg( ) const
    {
    return m_sg;
    }
    
//////////////////////////////////////////////////////////////////////////////
// QSignal::SetSg( )
//
// Sets the signal's type.
//////////////////////////////////////////////////////////////////////////////
void QSignal::SetSg( QSignal::SignalType sg )
    {
    m_sg = sg;
    }

//////////////////////////////////////////////////////////////////////////////
// QSignal::GetPrice( )
//
// Gets the signal's price.
//////////////////////////////////////////////////////////////////////////////
NUM QSignal::GetPrice( ) const
    {
    return m_numPrice;
    }

//////////////////////////////////////////////////////////////////////////////
// QSignal::SetPrice( )
//
// Sets the signal's price.
//////////////////////////////////////////////////////////////////////////////
void QSignal::SetPrice( NUM numPrice )
    {
    m_numPrice = numPrice;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QSignal::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QSignal::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_date.AssertValid( );
    ASSERT( m_sg == sgBuy || m_sg == sgSell );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QSignalList::QSignalList( )
//
// Constructors and Destructors.
//////////////////////////////////////////////////////////////////////////////
QSignalList::QSignalList( )
    : m_nOffset( 0u )
    {
    }

QSignalList::~QSignalList( )
    {
    Cleanup( );
    }

void QSignalList::Cleanup( )
    {
    QSignal *psignal;

    while ( psignal = (QSignal *) RemoveHead( ) )
        {
        delete psignal;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QSignalList::offset( )
//
// This routine returns the offset of the signal list, from
// the beginning of Quote data.
//////////////////////////////////////////////////////////////////////////////
WORD QSignalList::offset( void ) const
    {
    return m_nOffset;
    }

//////////////////////////////////////////////////////////////////////////////
// QSignalList::offset( )
//
// This routine sets the offset of the signal list, from
// the beginning of Quote data.
//////////////////////////////////////////////////////////////////////////////
void QSignalList::offset( WORD nOffset )
    {
    // This should always be true
    ASSERT( nOffset <= _pdoc->GetCount( ) );
    m_nOffset = nOffset;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QSignalList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QSignalList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QSignal *psig = (QSignal *) GetHead( );
          psig;
          psig = (QSignal *) GetNext( psig ) )
        {
        psig->AssertValid( );
        }
    ASSERT( m_nOffset <= _pdoc->GetCount( ) );
    }
#endif

