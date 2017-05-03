#include "qobject.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

QString::QString( )
    {
    Trace( TR_INTENSE, "QString::QString( ) called.\n" );
    if ( m_fInit )
        {
        m_pch = NULL;
        }
    }

QString::QString( const CHAR *psz )
    {
    BOOL fInit = FALSE;
    Trace( TR_INTENSE, "QString::QString( const CHAR * ) called.\n" );
    TASKLIST
        {
        // base class failed to init
        if ( !m_fInit )
            {
            QUIT;
            }
        m_pch = NULL;
        if ( psz )
            {
            m_pch = new CHAR[ lstrlen( psz ) + 1 ];
            if ( !m_pch )
                {
                Trace( TR_NORMAL, _szMemory, THIS_FILE, __LINE__ );
                QUIT;
                }
            lstrcpy( m_pch, psz );
            }
        fInit = TRUE;
        }
    ENDTASKLIST
    m_fInit = fInit;
    }

QString::QString( const QString &s )
    {
    BOOL fInit = FALSE;
    Trace( TR_INTENSE, "QString::QString( const QString & ) called.\n" );
    TASKLIST
        {
        if ( !m_fInit || !s.m_fInit )
            {
            QUIT;
            }
        m_pch = NULL;
        if ( s.m_pch )
            {
            m_pch = new CHAR[ s.GetLength( ) + 1 ];
            if ( !m_pch )
                {
                Trace( TR_NORMAL, _szMemory, THIS_FILE, __LINE__ );
                QUIT;
                }
            lstrcpy( m_pch, s.m_pch );
            }
        fInit = TRUE;
        }
    ENDTASKLIST
    m_fInit = fInit;
    }

QString::~QString( )
    {
    Trace( TR_INTENSE, "QString::~QString( ) called.\n" );
    Cleanup( );
    }

void QString::Cleanup( )
    {
    Trace( TR_INTENSE, "QString::Cleanup( ) called.\n" );
    if ( m_fInit )
        {
        delete [] m_pch;
        m_pch = NULL;
        }
    }

void QString::Empty( )
    {
    Trace( TR_INTENSE, "QString::Empty( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    Cleanup( );
    }

QString::operator const CHAR *( ) const
    {
    Trace( TR_INTENSE, "QString::operator const CHAR*( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_pch;
    }

const QString &QString::operator=( const CHAR *psz )
    {
    Trace( TR_INTENSE, "QString::operator=( const CHAR * ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    Empty( );
    if ( psz )
        {
        m_pch = new CHAR[ lstrlen( psz ) + 1 ];
        if ( !m_pch )
            {
            Trace( TR_NORMAL, _szMemory, THIS_FILE, __LINE__ );
            THROW_EXCEPTION( exMemory );
            }
        lstrcpy( m_pch, psz );
        }
    return *this;
    }

const QString &QString::operator=( const QString &s )
    {
    Trace( TR_INTENSE, "QString::operator=( const Qstring & ) called.\n" );
    if ( !m_fInit || !s.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    if ( &s != this )
        {
        Empty( );
        if ( s.m_pch )
            {
            m_pch = new CHAR[ s.GetLength( ) + 1 ];
            if ( !m_pch )
                {
                Trace( TR_NORMAL, _szMemory, THIS_FILE, __LINE__ );
                THROW_EXCEPTION( exMemory );
                }
            lstrcpy( m_pch, s.m_pch );
            }
        }
    return *this;
    }

const QString &QString::operator+=( const CHAR *psz )
    {
    Trace( TR_INTENSE, "QString::operator+=( const CHAR * ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    // only have work to do if psz is non-empty
    if ( psz && *psz )
        {
        if ( m_pch )
            {
            CHAR *pch = new CHAR[ GetLength( ) + lstrlen( psz ) + 1 ];
            if ( !pch )
                {
                Trace( TR_NORMAL, _szMemory, THIS_FILE, __LINE__ );
                THROW_EXCEPTION( exMemory );
                }
            lstrcpy( pch, m_pch );
            lstrcat( pch, psz );
            Empty( );
            m_pch = pch;
            }
        else
            {
            *this = psz;
            }
        }
    return *this;
    }
    
const QString Concat( const CHAR *psz1, const CHAR *psz2 )
    {
    QString sRet;
    Trace( TR_INTENSE, "Concat( const CHAR *, const CHAR * ) called.\n" );
    if ( !sRet.IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    
    TRY    
        {
        sRet = psz1;
        sRet += psz2;
        }
    CATCH_ALL
        {
        sRet.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    
    return sRet;
    }

const QString operator+( const QString &s1, const QString &s2 )
    {
    Trace( TR_INTENSE, "QString::operator+( const QString &, const QString & ) called.\n" );
    return Concat( s1, s2 );
    }

const QString operator+( const QString &s, const CHAR *psz )
    {
    Trace( TR_INTENSE, "QString::operator+( const QString &, const CHAR * ) called.\n" );
    return Concat( s, psz );
    }

const QString operator+( const CHAR *psz, const QString &s )
    {
    Trace( TR_INTENSE, "QString::operator+( const CHAR *, const QString & ) called.\n" );
    return Concat( psz, s );
    }

int QString::Compare( const CHAR *psz ) const
    {
    Trace( TR_INTENSE, "QString::Compare( const CHAR * ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( m_pch == NULL && psz == NULL )
        {
        // define this to be equality
        return 0;
        }
    ASSERT( m_pch != NULL );
    ASSERT( psz != NULL );
    return ( lstrcmp( m_pch, psz ) );
    }

int QString::CompareNoCase( const CHAR *psz ) const
    {
    Trace( TR_INTENSE, "QString::CompareNoCase( const CHAR * ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( m_pch == NULL && psz == NULL )
        {
        // define this to be equality
        return 0;
        }
    ASSERT( m_pch != NULL );
    ASSERT( psz != NULL );
    return ( lstrcmpi( m_pch, psz ) );
    }

BOOL operator==( const QString &s, const CHAR *psz )
    {
    Trace( TR_INTENSE, "operator==( const QString &, const CHAR * ) called.\n" );
    return ( s.Compare( psz ) == 0 );
    }

BOOL operator==( const CHAR *psz, const QString &s )
    {
    Trace( TR_INTENSE, "operator==( const CHAR *, const QString & ) called.\n" );
    return ( s.Compare( psz ) == 0 );
    }

BOOL operator==( const QString &s1, const QString &s2 )
    {
    Trace( TR_INTENSE, "operator==( const QString &, const QString & ) called.\n" );
    return ( s1.Compare( s2 ) == 0 );
    }

BOOL operator!=( const QString &s, const CHAR *psz )
    {
    Trace( TR_INTENSE, "operator!=( const QString &, const CHAR * ) called.\n" );
    return ( s.Compare( psz ) != 0 );
    }

BOOL operator!=( const CHAR *psz, const QString &s )
    {
    Trace( TR_INTENSE, "operator!=( const CHAR *, const QString & ) called.\n" );
    return ( s.Compare( psz ) != 0 );
    }

BOOL operator!=( const QString &s1, const QString &s2 )
    {
    Trace( TR_INTENSE, "operator!=( const QString &, const QString & ) called.\n" );
    return ( s1.Compare( s2 ) != 0 );
    }

BOOL operator<( const QString &s, const CHAR *psz )
    {
    Trace( TR_INTENSE, "operator<( const QString &, const CHAR * ) called.\n" );
    return ( s.Compare( psz ) < 0 );
    }

BOOL operator<( const CHAR *psz, const QString &s )
    {
    Trace( TR_INTENSE, "operator<( const CHAR *, const QString & ) called.\n" );
    return ( s.Compare( psz ) > 0 );
    }

BOOL operator<( const QString &s1, const QString &s2 )
    {
    Trace( TR_INTENSE, "operator<( const QString &, const QString & ) called.\n" );
    return ( s1.Compare( s2 ) < 0 );
    }

BOOL operator>( const QString &s, const CHAR *psz )
    {
    Trace( TR_INTENSE, "operator>( const QString &, const CHAR * ) called.\n" );
    return ( s.Compare( psz ) > 0 );
    }

BOOL operator>( const CHAR *psz, const QString &s )
    {
    Trace( TR_INTENSE, "operator>( const CHAR *, const QString & ) called.\n" );
    return ( s.Compare( psz ) < 0 );
    }

BOOL operator>( const QString &s1, const QString &s2 )
    {
    Trace( TR_INTENSE, "operator>( const QString &, const QString & ) called.\n" );
    return ( s1.Compare( s2 ) > 0 );
    }

BOOL operator<=( const QString &s, const CHAR *psz )
    {
    Trace( TR_INTENSE, "operator<=( const QString &, const CHAR * ) called.\n" );
    return ( s.Compare( psz ) <= 0 );
    }

BOOL operator<=( const CHAR *psz, const QString &s )
    {
    Trace( TR_INTENSE, "operator<=( const CHAR *, const QString & ) called.\n" );
    return ( s.Compare( psz ) >= 0 );
    }

BOOL operator<=( const QString &s1, const QString &s2 )
    {
    Trace( TR_INTENSE, "operator<=( const QString &, const QString & ) called.\n" );
    return ( s1.Compare( s2 ) <= 0 );
    }

BOOL operator>=( const QString &s, const CHAR *psz )
    {
    Trace( TR_INTENSE, "operator>=( const QString &, const CHAR * ) called.\n" );
    return ( s.Compare( psz ) >= 0 );
    }

BOOL operator>=( const CHAR *psz, const QString &s )
    {
    Trace( TR_INTENSE, "operator>=( const CHAR *, const QString & ) called.\n" );
    return ( s.Compare( psz ) <= 0 );
    }

BOOL operator>=( const QString &s1, const QString &s2 )
    {
    Trace( TR_INTENSE, "operator>=( const QString &, const QString & ) called.\n" );
    return ( s1.Compare( s2 ) >= 0 );
    }

CHAR QString::operator[]( int ich ) const
    {
    Trace( TR_INTENSE, "QString::operator[]( UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
#ifdef DEBUG
    LONG l = (LONG) ich;
    ASSERT( l >= 0L );
    ASSERT( l < (LONG) GetLength( ) );
#endif
    return *( m_pch + ich );
    }

UINT QString::GetLength( ) const
    {
    Trace( TR_INTENSE, "QString::GetLength( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( m_pch != NULL );
    return ( lstrlen( m_pch ) );
    }

void QString::StringFix( )
    {
    Trace( TR_INTENSE, "QString::StringFix( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( m_pch )
        {
        for ( UINT i = 0; i < GetLength( ); i++ )
            {
            if ( m_pch[ i ] == '\n' )
                {
                m_pch[ i ] = '\0';
                }
            }
        }
    }

void QString::Store( QArchive &ar )
    {
    UINT cb;
    BYTE b;
    Trace( TR_INTENSE, "QString::Store( QArchive & ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ar.WriteHeader( otString );
    if ( m_pch )
        {
        b = TRUE;
        ar.Write( &b, sizeof( b ) );
        cb = GetLength( );
        ar.Write( &cb, sizeof( cb ) );
        ar.Write( m_pch, cb );
        }
    else
        {
        b = FALSE;
        ar.Write( &b, sizeof( b ) );
        }
    }

void QString::Retrieve( QArchive &ar, UINT ot )
    {
    UINT cb2;
    UINT cb;
    BYTE b;
    Trace( TR_INTENSE, "QString::Retrieve( QArchive &, UINT, UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( ot == otString );
    cb2 = ar.Read( &b, sizeof( b ) );
    if ( cb2 != sizeof( b ) )
        {
        THROW_EXCEPTION( exFile );
        }
    if ( b )  
        {
        cb2 = ar.Read( &cb, sizeof( UINT ) );
        if ( cb2 != sizeof( UINT ) )
            {
            THROW_EXCEPTION( exFile );
            }
        Cleanup( );
        m_pch = new CHAR[ cb + 1 ];
        if ( !m_pch )
            {
            THROW_EXCEPTION( exMemory );
            }
        lmemset( m_pch, 0, cb + 1 );
        cb2 = ar.Read( m_pch, cb );
        if ( cb2 != cb )
            {
            THROW_EXCEPTION( exFile );
            }
        }
    else
        {
        Cleanup( );
        }
    }

#ifdef DEBUG
void QString::Dump( ) const
    {
    QString sTmp;

    Trace( TR_INTENSE, "QString::Dump( ) called.\n" );
    if ( !m_fInit || !sTmp.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    TRY
        {
        if ( m_pch )
            {
            sTmp = *this + _szCR;
            }
        else
            {
            sTmp  = _szNull;
            sTmp += _szCR;
            }
        Trace( TR_NORMAL, sTmp );
        }
    CATCH_ALL
        {
        sTmp.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }
#endif
