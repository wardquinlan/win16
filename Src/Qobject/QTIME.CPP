#include "qobject.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

QTime::QTime( )
    {
    BOOL fInit = FALSE;
    time_t t;

    Trace( TR_INTENSE, "QTime::QTime( ) called.\n" );
    TASKLIST
        {
        if ( !m_fInit || !m_sFormat.IsInit( ) )
            {
            QUIT;
            }
        m_t = ( time_t ) 0;
        t = time( NULL ); // Get the current time
        if ( t < ( time_t ) 0 )
            {
            ASSERT( FALSE );
            QUIT;
            }
        m_t = t;
        m_fDefaultFormat = TRUE;
        m_cbFormatMax = cbAsciiFormatMax;
        fInit = TRUE;
        }
    ENDTASKLIST
    m_fInit = fInit;
    }

QTime::QTime( const QTime &t )
    {
    BOOL fInit = FALSE;

    Trace( TR_INTENSE, "QTime::QTime( const QTime & ) called.\n" );
    TASKLIST
        {
        if ( !m_fInit || !m_sFormat.IsInit( ) || !t.m_fInit )
            {
            QUIT;
            }
        m_t              = t.m_t;
        m_fDefaultFormat = t.m_fDefaultFormat;
        m_cbFormatMax    = t.m_cbFormatMax;
        TRY
            {
            m_sFormat = t.m_sFormat;
            fInit = TRUE;
            }
        CATCH_ALL
            {
            m_sFormat.Cleanup( );
            // fInit is already FALSE
            }
        END_CATCH
        }
    ENDTASKLIST
    m_fInit = fInit;
    }

QTime::QTime( time_t t )
    {
    BOOL fInit = FALSE;
    Trace( TR_INTENSE, "QTime::QTime( time_t ) called.\n" );
    TASKLIST
        {
        if ( !m_fInit || !m_sFormat.IsInit( ) )
            {
            QUIT;
            }
        m_t = ( time_t ) 0;
        if ( t < ( time_t ) 0 )
            {
            ASSERT( FALSE );
            QUIT;
            }
        m_t = t;
        m_fDefaultFormat = TRUE;
        m_cbFormatMax = cbAsciiFormatMax;
        fInit = TRUE;
        }
    ENDTASKLIST
    m_fInit = fInit;
    }

QTime::QTime( int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, BOOL fAdjustDST )
    {
    BOOL fInit = FALSE;
    Trace( TR_INTENSE, "QTime::QTime( int, int, int, int, int, int ) called.\n" );
    TRY
        {
        Set( nYear, nMonth, nDay, nHour, nMinute, nSecond, fAdjustDST );
        m_fDefaultFormat = TRUE;
        m_cbFormatMax = cbAsciiFormatMax;
        fInit = TRUE;
        }
    CATCH_ALL
        {
        m_sFormat.Cleanup( );
        }
    END_CATCH
    m_fInit = fInit;
    }

QTime::~QTime( )
    {
    Trace( TR_INTENSE, "QTime::~QTime( ) called.\n" );
    Cleanup( );
    }

void QTime::Cleanup( )
    {
    Trace( TR_INTENSE, "QTime::Cleanup( ) called.\n" );
    if ( m_fInit )
        {
        m_sFormat.Cleanup( );
        }
    }

void QTime::Set( int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, BOOL fAdjustDST )
    {
    static struct tm t;
    time_t tt;
    
    Trace( TR_INTENSE, "QTime::QTime( int, int, int, int, int, int ) called.\n" );
    if ( !m_fInit || !m_sFormat.IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }

    m_t = ( time_t ) 0;
    lmemset( &t, 0, sizeof( t ) );
    t.tm_year = nYear - 1900;
    t.tm_mon  = nMonth - 1;
    t.tm_mday = nDay;
    t.tm_hour = nHour;
    t.tm_min  = nMinute;
    t.tm_sec  = nSecond;
    tt = mktime( &t );
    if ( tt < ( time_t ) 0 )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    if ( fAdjustDST && t.tm_isdst )
        {
        // adjust for DST
        tt -= 3600;
        }
    m_t = tt;
    }

int QTime::GetYear( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetYear( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_year + 1900;
    }

int QTime::GetMonth( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetMonth( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_mon + 1;
    }

int QTime::GetDay( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetDay( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_mday;
    }

int QTime::GetHour( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetHour( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_hour;
    }

int QTime::GetMinute( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetMinute( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_min;
    }

int QTime::GetSecond( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetSecond( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_sec;
    }

int QTime::GetWeekDay( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetWeekDay( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_wday;
    }

int QTime::GetYearDay( ) const
    {
    struct tm *ptm;
    Trace( TR_INTENSE, "QTime::GetYearDay( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ptm = localtime( &m_t );
    if ( !ptm )
        {
        Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exGeneric );
        }
    return ptm->tm_yday;
    }

BOOL QTime::operator==( const QTime &t ) const
    {
    Trace( TR_INTENSE, "QTime::operator==( const QTime &) called.\n" );
    if ( !m_fInit || !t.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return ( m_t == t.m_t );
    }

BOOL QTime::operator!=( const QTime &t ) const
    {
    Trace( TR_INTENSE, "QTime::operator!=( const QTime &) called.\n" );
    if ( !m_fInit || !t.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return ( m_t != t.m_t );
    }

BOOL QTime::operator<(const QTime &t ) const
    {
    Trace( TR_INTENSE, "QTime::operator<( const QTime &) called.\n" );
    if ( !m_fInit || !t.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return ( m_t < t.m_t );
    }

BOOL QTime::operator>(const QTime &t ) const
    {
    Trace( TR_INTENSE, "QTime::operator>( const QTime &) called.\n" );
    if ( !m_fInit || !t.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return ( m_t > t.m_t );
    }

BOOL QTime::operator<=(const QTime &t ) const
    {
    Trace( TR_INTENSE, "QTime::operator<=( const QTime &) called.\n" );
    if ( !m_fInit || !t.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return ( m_t <= t.m_t );
    }

BOOL QTime::operator>=(const QTime &t ) const
    {
    Trace( TR_INTENSE, "QTime::operator>=( const QTime &) called.\n" );
    if ( !m_fInit || !t.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return ( m_t >= t.m_t );
    }

void QTime::Format( UINT cbFormatMax, const CHAR *pszFormat )
    {
    Trace( TR_INTENSE, "QTime::Format( UINT, LPSTR ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    m_sFormat = pszFormat;
    m_cbFormatMax = cbFormatMax;
    m_fDefaultFormat = FALSE;
    }

QTime::operator time_t( ) const
    {
    Trace( TR_INTENSE, "QTime::operator time_t( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_t;
    }

QTime::operator QString( ) const
    {
    struct tm *ptm;
    QString sRet;
    
    Trace( TR_INTENSE, "QTime::operator QString( ) called.\n" );
    if ( !m_fInit || !sRet.IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    if ( m_fDefaultFormat )
        {
        ptm = localtime( &m_t );
        if ( !ptm )
            {
            Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
            THROW_EXCEPTION( exGeneric );
            }
        TRY
            {
            sRet = asctime( ptm );
            sRet.StringFix( );
            }
        CATCH_ALL
            {
            sRet.Cleanup( );
            THROW_LAST( );
            }
        END_CATCH
        }
    else
        {
        CHAR *psz = new CHAR[ m_cbFormatMax + 1 ];
        if ( !psz )
            {
            Trace( TR_NORMAL, _szMemory, THIS_FILE, __LINE__ );
            THROW_EXCEPTION( exMemory );
            }
        lmemset( psz, 0, m_cbFormatMax + 1 );
        ptm = localtime( &m_t );
        if ( !ptm )
            {
            delete [] psz;
            Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
            THROW_EXCEPTION( exGeneric );
            }
        UINT cb = strftime( psz, m_cbFormatMax, m_sFormat, ptm );
        if ( cb == 0 )
            {
            delete [] psz;
            Trace( TR_NORMAL, _szGeneric, THIS_FILE, __LINE__ );
            THROW_EXCEPTION( exGeneric );
            }
        TRY
            {
            sRet = psz;
            }
        CATCH_ALL
            {
            sRet.Cleanup( );
            delete psz;
            THROW_LAST( );
            }
        END_CATCH
        delete [] psz;
        }
    return sRet;
    }

void QTime::Store( QArchive &ar )
    {
    Trace( TR_INTENSE, "QTime::Store( QArchive & ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ar.WriteHeader( otTime );
    ar.Write( &m_t, sizeof( m_t ) );
    ar.Write( &m_fDefaultFormat, sizeof( m_fDefaultFormat ) );
    ar.Write( &m_cbFormatMax, sizeof( m_cbFormatMax ) );
    m_sFormat.Store( ar );
    }

void QTime::Retrieve( QArchive &ar, UINT ot )
    {
    UINT cb2;
    Trace( TR_INTENSE, "Time::Retrieve( QArchive &, UINT, UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( ot == otTime );
    Cleanup( );
    cb2 = ar.Read( &m_t, sizeof( m_t ) );
    if ( cb2 != sizeof( m_t ) )
        {
        THROW_EXCEPTION( exFile );
        }
    cb2 = ar.Read( &m_fDefaultFormat, sizeof( m_fDefaultFormat ) );
    if ( cb2 != sizeof( m_fDefaultFormat ) )
        {
        THROW_EXCEPTION( exFile );
        }
    cb2 = ar.Read( &m_cbFormatMax, sizeof( m_cbFormatMax ) );
    if ( cb2 != sizeof( m_cbFormatMax ) )
        {
        THROW_EXCEPTION( exFile );
        }
    if ( !ar.ReadHeader( &ot ) )
        {
        THROW_EXCEPTION( exFile );
        }
    m_sFormat.Retrieve( ar, ot );
    }


