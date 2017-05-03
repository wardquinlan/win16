#include "qobject.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

// Global Variables
#if ( FDATA == 1 )
const CHAR _szMemory[]  = "Memory allocation failure: File %Fs Line %d.\n";
const CHAR _szInit[]    = "Initialization failure: File %Fs Line %d.\n";
const CHAR _szGeneric[] = "Generic error occurred: File %Fs Line %d.\n";
const CHAR _szAssert[]  = "ASSERT: File %Fs Line %d.\n";
const CHAR _szUnknown[] = "Unknown";
const CHAR _szNull[]    = "(NULL)";
const CHAR _szCR[]      = "\n";
#else
const CHAR _szMemory[]  = "Memory allocation failure: File %s Line %d.\n";
const CHAR _szInit[]    = "Initialization failure: File %s Line %d.\n";
const CHAR _szGeneric[] = "Generic error occurred: File %s Line %d.\n";
const CHAR _szAssert[]  = "ASSERT: File %s Line %d.\n";
const CHAR _szUnknown[] = "Unknown";
const CHAR _szNull[]    = "(NULL)";
const CHAR _szCR[]      = "\n";
#endif

// QObject version
const QVersion _vQObject( 1, 4, 4 );

CATCHBUF _catchbuf; // global exception handling buffer

////////////////////////////////////////////////////
// Utility routines

void FAR *lmemset( void FAR *pv, int b, UINT cb )
    {
    for ( UINT i = 0; i < cb; i++ )
        {
        *( ( (BYTE FAR *) pv ) + i ) = b;
        }
    return pv;
    }

void FAR *lmemcpy( void FAR *pvDst, void FAR *pvSrc, UINT cb )
    {
    for ( UINT i = 0; i < cb; i++ )
        {
        *( ( (BYTE FAR *) pvDst ) + i ) = *( ( (BYTE FAR *) pvSrc ) + i );
        }
    return pvDst;
    }

////////////////////////////////////////////////////
// QObject methods

QObject::QObject( )
    {
    Trace( TR_INTENSE, "QObject::QObject( ) called.\n" );
    m_fInit = TRUE;
    }

BOOL QObject::IsInit( ) const
    {
    Trace( TR_INTENSE, "QObject::IsInit( ) called.\n" );
    return m_fInit;
    }

void QObject::Store( QArchive &ar )
    {
    Trace( TR_INTENSE, "QObject::Store( QArchive & ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    // no action required
    }

void QObject::Retrieve( QArchive &ar, UINT ot )
    {
    Trace( TR_INTENSE, "QObject::Retrieve( QArchive &, UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    // no action required.
    }

#ifdef DEBUG
void QObject::AssertValid( ) const
    {
    Trace( TR_INTENSE, "QObject::AssertValid( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    // no other action necessary
    }
#endif

////////////////////////////////////////////////////
// QListObject methods

QListObject::QListObject( )
    {
    Trace( TR_INTENSE, "QListObject::QListObject( ) called.\n" );
    if ( m_fInit )
        {
        m_plobPrev = NULL;
        m_plobNext = NULL;
        }
    }

////////////////////////////////////////////////////
// QVirtualList methods

QVirtualList::QVirtualList( )
    {
    Trace( TR_INTENSE, "QVirtualList::QVirtualList( ) called.\n" );
    if ( m_fInit )
        {
        m_plobHead = NULL;
        m_plobTail = NULL;
        m_nCount = 0;
        }
    }

#ifdef DEBUG
void QVirtualList::AssertValid( ) const
    {
    BOOL fFirst = TRUE;
    QListObject *plob;
    QListObject *plobPrev;
    QListObject *plobNext;
    UINT nCount = 0;

    Trace( TR_INTENSE, "QVirtualList::AssertValid( ) called.\n" );
    if ( !m_fInit )
        {
        THROW_EXCEPTION( exInit );
        }
    QObject::AssertValid( );

    // verify the head and tail
    if ( m_plobHead != NULL )
        {
        ASSERT( m_plobTail != NULL );
        }
    if ( m_plobHead == NULL )
        {
        ASSERT( m_plobTail == NULL );
        }

    // verify the list isn't circular
    for ( plob = GetHead( ); plob; plob = GetNext( plob ) )
        {
        if ( fFirst )
            {
            fFirst = FALSE;
            }
        else
            {
            if ( plob == GetHead( ) )
                {
                ASSERT( FALSE );
                break;
                }
            }
        // verify the count
        nCount++;
        }
    ASSERT( nCount == m_nCount );

    // verify the pointers
    for ( plob = GetHead( ); plob; plob = GetNext( plob ) )
        {
        plobPrev = GetPrev( plob );
        plobNext = GetNext( plob );
        if ( plobPrev )
            {
            ASSERT( GetNext( plobPrev ) == plob );
            }
        else
            {
            ASSERT( plob == m_plobHead );
            }
        if ( plobNext )
            {
            ASSERT( GetPrev( plobNext ) == plob );
            }
        else
            {
            ASSERT( plob == m_plobTail );
            }
        }
    }
#endif

void QVirtualList::InsertBefore( QListObject *plobList, QListObject *plobNew )
    {
    Trace( TR_INTENSE, "QVirtualList::InsertBefore( QListObject *, QListObject * ) called.\n" );
    if ( !m_fInit || !plobList->IsInit( ) || !plobNew->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plobList != NULL );
    ASSERT( plobNew != NULL );
    ASSERT( plobNew->m_plobPrev == NULL );
    ASSERT( plobNew->m_plobNext == NULL );
    if ( plobList == m_plobHead )
        {
        AddHead( plobNew );
        }
    else
        {
        plobNew->m_plobPrev = plobList->m_plobPrev;
        plobNew->m_plobNext = plobList;
        plobList->m_plobPrev = plobNew;
        plobNew->m_plobPrev->m_plobNext = plobNew;
        m_nCount++;
        }
    }

void QVirtualList::InsertAfter( QListObject *plobList, QListObject *plobNew )
    {
    Trace( TR_INTENSE, "QVirtualList::InsertAfter( QListObject *, QListObject * ) called.\n" );
    if ( !m_fInit || !plobList->IsInit( ) || !plobNew->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plobList != NULL );
    ASSERT( plobNew != NULL );
    ASSERT( plobNew->m_plobPrev == NULL );
    ASSERT( plobNew->m_plobNext == NULL );
    if ( plobList == m_plobTail )
        {
        AddTail( plobNew );
        }
    else
        {
        plobNew->m_plobNext = plobList->m_plobNext;
        plobNew->m_plobPrev = plobList;
        plobList->m_plobNext = plobNew;
        plobNew->m_plobNext->m_plobPrev = plobNew;
        m_nCount++;
        }
    }

void QVirtualList::AddHead( QListObject *plob )
    {
    Trace( TR_INTENSE, "QVirtualList::AddTail( QListObject * ) called.\n" );
    if ( !m_fInit || !plob->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plob != NULL );
    ASSERT( plob->m_plobPrev == NULL );
    ASSERT( plob->m_plobNext == NULL );
    if ( m_plobHead )
        {
        ASSERT( m_plobTail != NULL );
        plob->m_plobPrev = NULL;
        plob->m_plobNext = m_plobHead;
        m_plobHead->m_plobPrev = plob;
        m_plobHead = plob;
        }
    else
        {
        ASSERT( m_plobTail == NULL );
        m_plobHead = m_plobTail = plob;
        plob->m_plobNext = plob->m_plobPrev = NULL;
        }
    m_nCount++;
    }

void QVirtualList::AddTail( QListObject *plob )
    {
    Trace( TR_INTENSE, "QVirtualList::AddTail( QListObject * ) called.\n" );
    if ( !m_fInit || !plob->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plob != NULL );
    ASSERT( plob->m_plobPrev == NULL );
    ASSERT( plob->m_plobNext == NULL );
    if ( m_plobTail )
        {
        ASSERT( m_plobHead != NULL );
        plob->m_plobNext = NULL;
        plob->m_plobPrev = m_plobTail;
        m_plobTail->m_plobNext = plob;
        m_plobTail = plob;
        }
    else
        {
        ASSERT( m_plobHead == NULL );
        m_plobTail = m_plobHead = plob;
        plob->m_plobPrev = plob->m_plobNext = NULL;
        }
    m_nCount++;
    }

QListObject *QVirtualList::GetHead( ) const
    {
    Trace( TR_INTENSE, "QVirtualList::GetHead( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_plobHead;
    }

QListObject *QVirtualList::GetTail( ) const
    {
    Trace( TR_INTENSE, "QVirtualList::GetTail( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_plobTail;
    }

QListObject *QVirtualList::GetPrev( const QListObject *plob ) const
    {
    Trace( TR_INTENSE, "QVirtualList::GetPrev( QListObject * ) called.\n" );
    if ( !m_fInit || !plob->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plob != NULL );
    return plob->m_plobPrev;
    }

QListObject *QVirtualList::GetNext( const QListObject *plob ) const
    {
    Trace( TR_INTENSE, "QVirtualList::GetNext( QListObject * ) called.\n" );
    if ( !m_fInit || !plob->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plob != NULL );
    return plob->m_plobNext;
    }

QListObject *QVirtualList::RemoveHead( )
    {
    QListObject *plob = NULL;
    Trace( TR_INTENSE, "QVirtualList::RemoveHead( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    plob = m_plobHead;
    if ( plob )
        {
        m_plobHead = plob->m_plobNext;
        if ( m_plobHead )
            {
            m_plobHead->m_plobPrev = NULL;
            }
        else
            {
            m_plobTail = NULL;
            }
        plob->m_plobNext = NULL;
        }
    if ( plob )
        {
        m_nCount--;
        }
    return plob;
    }

QListObject *QVirtualList::RemoveTail( )
    {
    QListObject *plob = NULL;
    Trace( TR_INTENSE, "QVirtualList::RemoveTail( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    plob = m_plobTail;
    if ( plob )
        {
        m_plobTail = plob->m_plobPrev;
        if ( m_plobTail )
            {
            m_plobTail->m_plobNext = NULL;
            }
        else
            {
            m_plobHead = NULL;
            }
        plob->m_plobPrev = NULL;
        }
    if ( plob )
        {
        m_nCount--;
        }
    return plob;
    }

BOOL QVirtualList::IsExist( const QListObject *plob ) const
    {
    BOOL fRet = FALSE;
    Trace( TR_INTENSE, "QVirtualList::IsExist( const QListObject * ) called.\n" );
    if ( !m_fInit || !plob->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plob != NULL );
    for ( QListObject *plob2 = m_plobHead; plob2; plob2 = plob2->m_plobNext )
        {
        if ( plob2 == plob )
            {
            fRet = TRUE;
            break;
            }
        }
    return fRet;
    }

BOOL QVirtualList::Remove( QListObject *plob )
    {
    BOOL fRet = FALSE;
    Trace( TR_INTENSE, "QVirtualList::Remove( QListObject * ) called.\n" );
    if ( !m_fInit || !plob->IsInit( ) )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( plob != NULL );

    TASKLIST
        {
        if ( !IsExist( plob ) )
            {
            QUIT;
            }

        if ( plob == m_plobHead && plob == m_plobTail )
            {
            m_plobHead = m_plobTail = NULL;
            plob->m_plobPrev = plob->m_plobNext = NULL;
            fRet = TRUE;
            QUIT;
            }

        if ( plob == m_plobHead )
            {
            ASSERT( m_nCount >= 2 );
            plob->m_plobNext->m_plobPrev = NULL;
            m_plobHead = m_plobHead->m_plobNext;
            plob->m_plobPrev = plob->m_plobNext = NULL;
            fRet = TRUE;
            QUIT;
            }

        if ( plob == m_plobTail )
            {
            ASSERT( m_nCount >= 2 );
            plob->m_plobPrev->m_plobNext = NULL;
            m_plobTail = m_plobTail->m_plobPrev;
            plob->m_plobPrev = plob->m_plobNext = NULL;
            fRet = TRUE;
            QUIT;
            }

        ASSERT( m_nCount >= 3 );
        plob->m_plobNext->m_plobPrev = plob->m_plobPrev;
        plob->m_plobPrev->m_plobNext = plob->m_plobNext;
        plob->m_plobPrev = plob->m_plobNext = NULL;
        fRet = TRUE;
        }
    ENDTASKLIST
    if ( fRet )
        {
        m_nCount--;
        }
    return fRet;
    }

UINT QVirtualList::GetCount( ) const
    {
    Trace( TR_INTENSE, "QVirtualList::GetCount( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_nCount;
    }

////////////////////////////////////////////////////
// QVersion methods

QVersion::QVersion( )
    {
    UINT cb;
    Trace( TR_INTENSE, "QVersion::QVersion( ) called.\n" );
    if ( m_fInit )
        {
        m_nMajor = 0u;
        m_nMinor = 0u;
        m_nRevision = 0u;
        cb = wsprintf( m_szVersion, "%u.%u.%02u", m_nMajor, m_nMinor, m_nRevision );
        ASSERT( cb < sizeof( m_szVersion ) );
        }
    }

QVersion::QVersion( UINT nMajor, UINT nMinor, UINT nRevision )
    {
    UINT cb;
    Trace( TR_INTENSE, "QVersion::QVersion( UINT, UINT, UINT ) called.\n" );
    if ( m_fInit )
        {
        m_nMajor = nMajor;
        m_nMinor = nMinor;
        m_nRevision = nRevision;
        cb = wsprintf( m_szVersion, "%u.%u.%02u", m_nMajor, m_nMinor, m_nRevision );
        ASSERT( cb < sizeof( m_szVersion ) );
        }
    }

UINT QVersion::GetMajorNumber( ) const
    {
    Trace( TR_INTENSE, "QVersion::GetMajorNumber( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_nMajor;
    }

UINT QVersion::GetMinorNumber( ) const
    {
    Trace( TR_INTENSE, "QVersion::GetMinorNumber( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_nMinor;
    }

UINT QVersion::GetRevisionNumber( ) const
    {
    Trace( TR_INTENSE, "QVersion::GetRevisionNumber( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_nRevision;
    }

QVersion::operator const CHAR *( ) const
    {
    Trace( TR_INTENSE, "QVersion::operator const CHAR *( ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return m_szVersion;
    }

BOOL QVersion::operator==( const QVersion &v ) const
    {
    Trace( TR_INTENSE, "QVersion::operator==( const QVersion & ) called.\n" );
    if ( !m_fInit || !v.m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    return ( m_nMajor    == v.m_nMajor &&
             m_nMinor    == v.m_nMinor &&
             m_nRevision == v.m_nRevision );
    }

void QVersion::Store( QArchive &ar )
    {
    Trace( TR_INTENSE, "QVersion::Store( QArchive & ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ar.WriteHeader( otVersion );
    ar.Write( &m_nMajor, sizeof( UINT ) );
    ar.Write( &m_nMinor, sizeof( UINT ) );
    ar.Write( &m_nRevision, sizeof( UINT ) );
    }

void QVersion::Retrieve( QArchive &ar, UINT ot )
    {
    UINT cb2;
    
    Trace( TR_INTENSE, "QVersion::Retrieve( QArchive &, UINT ) called.\n" );
    if ( !m_fInit )
        {
        Trace( TR_NORMAL, _szInit, THIS_FILE, __LINE__ );
        THROW_EXCEPTION( exInit );
        }
    ASSERT( ot == otVersion );
    cb2 = ar.Read( &m_nMajor, sizeof( UINT ) );
    if ( cb2 != sizeof( UINT ) )
        {
        THROW_EXCEPTION( exFile );
        }
    cb2 = ar.Read( &m_nMinor, sizeof( UINT ) );
    if ( cb2 != sizeof( UINT ) )
        {
        THROW_EXCEPTION( exFile );
        }
    cb2 = ar.Read( &m_nRevision, sizeof( UINT ) );
    if ( cb2 != sizeof( UINT ) )
        {
        THROW_EXCEPTION( exFile );
        }
    cb2 = wsprintf( m_szVersion, "%u.%u.%02u", m_nMajor, m_nMinor, m_nRevision );
    ASSERT( cb2 < sizeof( m_szVersion ) );
    }

