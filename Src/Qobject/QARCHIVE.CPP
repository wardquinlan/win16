#include "qobject.hxx"
#include <dos.h>

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

#define HFILE_NULL HFILE_ERROR

///////////////////////////////////////////////////
// QArchive Utility routine

BOOL FileExists( const CHAR *pszFile )
    {
    struct find_t ft;
    return ( _dos_findfirst( pszFile, _A_RDONLY, &ft ) == 0 );
    }

QArchive::QArchive( )
    {
    Trace( TR_INTENSE, "QArchive::QArchive( ) called.\n" );
    m_pfile = NULL;
    }

QArchive::~QArchive( )
    {
    Trace( TR_INTENSE, "QArchive::~QArchive( ) called.\n" );
    Cleanup( );
    }

void QArchive::Cleanup( )
    {
    Trace( TR_INTENSE, "QArchive::Cleanup( ) called.\n" );
    if ( m_fInit )
        {
        Close( );
        }
    }

BOOL QArchive::Open( const CHAR *pszFile, int nMode )
    {
    BOOL fRet = FALSE;
    FILE *pfile;
    
    Trace( TR_INTENSE, "QArchive::Open( const CHAR *, int ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    TASKLIST
        {
        pfile = fopen( pszFile, nMode == nModeWrite ? "wb" : "rb" );
        if ( !pfile )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    if ( fRet )
        {
        m_pfile = pfile;
        }
    return fRet;
    }

void QArchive::Close( )
    {
    Trace( TR_INTENSE, "QArchive::Close( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( m_pfile )
        {
        fclose( m_pfile );
        m_pfile = NULL;
        }
    }

BOOL QArchive::ReadHeader( UINT *pot )
    {
    BOOL fRet = FALSE;
    UINT nCount;

    Trace( TR_INTENSE, "QArchive::ReadHeader( UINT * ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( !m_pfile )
        {
        THROW_EXCEPTION( exFile );
        }
    TASKLIST
        {
        nCount = fread( pot, sizeof( *pot ), 1, m_pfile );
        if ( nCount != 1 )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

UINT QArchive::Read( void *pv, UINT cb )
    {
    UINT nCount;
    Trace( TR_INTENSE, "QArchive::Read( void *, UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( !m_pfile )
        {
        THROW_EXCEPTION( exFile );
        }
    if ( cb > 0 )
        {
        nCount = fread( pv, cb, 1, m_pfile );
        if ( nCount != 1 )
            {
            cb = 0;
            }
        }
    return cb;
    }

void QArchive::WriteHeader( UINT ot )
    {
    UINT nCount;
    Trace( TR_INTENSE, "QArchive::WriteHeader( UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( !m_pfile )
        {
        THROW_EXCEPTION( exFile );
        }
    nCount = fwrite( &ot, sizeof( UINT ), 1, m_pfile );
    if ( nCount != 1 )
        {
        THROW_EXCEPTION( exFile );
        }
    }

void QArchive::Write( const void *pv, UINT cb )
    {
    UINT nCount;
    Trace( TR_INTENSE, "QArchive::Write( const void *, UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( !m_pfile )
        {
        THROW_EXCEPTION( exFile );
        }
    if ( cb > 0 )
        {
        nCount = fwrite( pv, cb, 1, m_pfile );
        if ( nCount != 1 )
            {
            THROW_EXCEPTION( exFile );
            }
        }
    }
