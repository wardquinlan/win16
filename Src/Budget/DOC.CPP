#include <qobject.hxx>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "doc.hxx"
#include "budget.hxx"
#include "otdoc.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

const QVersion QDocument::m_v( 2, 0, 0 );
const UINT QDocument::m_nSig( 0x4971 ); // This should never change

/////////////////////////////////////////
// QDocument methods

QDocument::QDocument( )
    :m_accGeneral( _szGeneral ), m_fModified( FALSE )
    {
    }

QDocument::~QDocument( )
    {
    Cleanup( );
    }

void QDocument::Cleanup( )
    {
    m_sFileName.Cleanup( );
    m_accGeneral.Cleanup( );
    m_list.Cleanup( );
    m_listSeq.Cleanup( );
    m_fModified = FALSE;
    }

void QDocument::Empty( )
    {
    m_sFileName.Empty( );
    m_accGeneral.Empty( );
    m_list.Cleanup( );
    m_listSeq.Cleanup( );
    m_fModified = FALSE;
    }

QAccount *QDocument::GetHead( ) const 
    { 
    return (QAccount *) m_list.GetHead( ); 
    }

QAccount *QDocument::GetTail( ) const 
    { 
    return (QAccount *) m_list.GetTail( ); 
    }

QAccount *QDocument::GetNext( const QAccount *pacc ) const 
    { 
    return (QAccount *) m_list.GetNext( pacc ); 
    }

QAccount *QDocument::GetPrev( const QAccount *pacc ) const 
    { 
    return (QAccount *) m_list.GetPrev( pacc ); 
    }

#ifdef DEBUG
void QDocument::checkConsistency( ) const
    {
    QSequenceElement *pseq;
    QAccount *pacc;
    QTxb     *ptxb;
    QTxb     *ptxbPrev;
    long      i;
    long      seqPrev;
    UINT      n( 0 );
    static CHAR sz[ ] = "";
    const CHAR *pszPrev = sz;
    Money monAmount;
    const QSequenceElement *pseqLast  = (const QSequenceElement *) m_listSeq.GetTail( );
    const QSequenceElement *pseqFirst = (const QSequenceElement *) m_listSeq.GetHead( );

    for ( pacc = (QAccount *) m_list.GetHead( ); pacc; pacc = (QAccount *) m_list.GetNext( pacc ) )
        {
        // Check for account names being sorted
        ASSERT( lstrcmpi( pszPrev, pacc->GetName( ) ) < 0 );
        pszPrev = pacc->GetName( );

        seqPrev = seqNil;
        monAmount = 0L;
        for ( ptxb = pacc->GetHead( ); ptxb; ptxb = pacc->GetNext( ptxb ) )
            {
            // Check for all sequence numbers being >= 1
            ASSERT( ptxb->GetSeq( ) >= 1L );

            // Check for all dates being ordered
            ptxbPrev = pacc->GetPrev( ptxb );
            if ( ptxbPrev )
                ASSERT( ptxbPrev->GetTransaction( ).GetDate( ) <= ptxb->GetTransaction( ).GetDate( ) );

            // Check for all sequence numbers being strictly ordered
            ASSERT( seqPrev < ptxb->GetSeq( ) );

            // Check all sequence numbers less than total number of transactions
            ASSERT( ptxb->GetSeq( ) <= (long) m_accGeneral.GetCount( ) );
            
            monAmount += ptxb->GetTransaction( ).GetAmount( );
            seqPrev = ptxb->GetSeq( );
            }
        
        // Check that the account balance is ok
        ptxb = pacc->GetTail( );
        if ( ptxb )
            ASSERT( monAmount == ptxb->GetBalance( ) );

        n += pacc->GetCount( );
        }
    // Check the sum of all accounts add to the number of transactions in
    // the general account
    ASSERT( n == m_accGeneral.GetCount( ) );
    
    monAmount = 0L;
    for ( ptxb = m_accGeneral.GetHead( ), i = 1; 
          i <= (long) m_accGeneral.GetCount( ); 
          ptxb = m_accGeneral.GetNext( ptxb ), ++i )
        {
        ASSERT( ptxb != NULL );

        // Check for all dates being ordered
        ptxbPrev = m_accGeneral.GetPrev( ptxb );
        if ( ptxbPrev )
            ASSERT( ptxbPrev->GetTransaction( ).GetDate( ) <= ptxb->GetTransaction( ).GetDate( ) );

        // Check for strict ordering of the general account sequence numbers
        ASSERT( i == ptxb->GetSeq( ) );

        monAmount += ptxb->GetTransaction( ).GetAmount( );
        }
    ASSERT( ptxb == NULL );

    // Check that the account balance is ok
    ptxb = m_accGeneral.GetTail( );
    if ( ptxb )
        ASSERT( monAmount == ptxb->GetBalance( ) );

    // Check sanity of the sequence list
    if ( pseqFirst )
        ASSERT( pseqFirst->GetSeq( ) >= 1L );
    if ( pseqLast )
        ASSERT( pseqLast->GetSeq( ) <= (long) m_accGeneral.GetCount( ) );

    // Check for strict ordering of the sequence numbers
    seqPrev = seqNil;
    for ( pseq = (QSequenceElement *) m_listSeq.GetHead( ); pseq; pseq = (QSequenceElement *) m_listSeq.GetNext( pseq ) )
        {
        ASSERT( seqPrev < pseq->GetSeq( ) );
        seqPrev = pseq->GetSeq( );
        }
    }
#endif

void QDocument::AddAccount( const CHAR *pszName )
    {
    QAccount *pacc;
    QAccount *paccNew;

    // advance to the insertion point
    for ( pacc = (QAccount *) m_list.GetHead( ); pacc && lstrcmpi( pacc->GetName( ), pszName ) < 0; (QAccount *) pacc = (QAccount *) m_list.GetNext( pacc ) )
        ;

    paccNew = new QAccount( pszName );
    if ( !paccNew )
        THROW_EXCEPTION( exMemory );
    
    if ( pacc )
        m_list.InsertBefore( pacc, paccNew );
    else
        m_list.AddTail( paccNew );

    checkConsistency( );
    }

BOOL QDocument::TransferTransaction( const CHAR  *pszAccountFrom,
                                     const CHAR  *pszAccountTo,
                                     const QTime &tDate,
                                     Money        monAmount,
                                     const CHAR  *pszDesc )
    {
    QString      sDesc;
    QTransaction txFrom;
    QTransaction txTo;
    QAccount    *pacc;
    BOOL         f( FALSE );
    
    TRY
        {
        pacc = FindAccount( pszAccountTo );
        if ( pacc == NULL )
            THROW_EXCEPTION( exGeneric );

        sDesc = QString( "[To " ) + pacc->GetName( ) + "] " + pszDesc;
        txFrom.SetDate( tDate );
        txFrom.SetDescription( sDesc );
        txFrom.SetAmount( -monAmount );
        f = _pdoc->AddTransaction( pszAccountFrom, txFrom );
                
        if ( f )
            {
            pacc = FindAccount( pszAccountFrom );
            if ( pacc == NULL )
                THROW_EXCEPTION( exGeneric );

            sDesc = QString( "[From " ) + pacc->GetName( ) + "] " + pszDesc;
            txTo.SetDate( tDate );
            txTo.SetDescription( sDesc );
            txTo.SetAmount( +monAmount );
            f = _pdoc->AddTransaction( pszAccountTo, txTo );
            ASSERT( f );
            }
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        sDesc.Cleanup( );
        txFrom.Cleanup( );
        txTo.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    checkConsistency( );
    return f;
    }

BOOL QDocument::AddTransaction( const CHAR *pszAccount, const QTransaction &tx )
    {
    QTransaction txGeneral;
    QString sDescriptionNew;
    BOOL fRet( FALSE );
    
    TRY
        {
        TASKLIST
            {
            long seq = getHighestSeqBalance( );
            if ( seq != seqNil )
                {
                const QTxb *ptxb = txbFromSeq( seq );
                ASSERT( ptxb != NULL );
                if ( tx.GetDate( ) < ptxb->GetTransaction( ).GetDate( ) )
                    break;
                }

            if ( !m_list.AddTransaction( pszAccount, tx, seq ) )
                break;

            txGeneral = tx;
            sDescriptionNew = QString( "[" ) + pszAccount + "] " + txGeneral.GetDescription( );
            txGeneral.SetDescription( sDescriptionNew );
            m_accGeneral.AddTransaction( TRUE, txGeneral, seq );
            fRet = TRUE;
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        txGeneral.Cleanup( );
        sDescriptionNew.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    checkConsistency( );
    return fRet;
    }

const QTxb *QDocument::txbFromSeqGeneral( long seq ) const
    {
    return m_accGeneral.txbFromSeq( seq );
    }

const QTxb *QDocument::txbFromSeq( long seq ) const
    {
    return m_list.txbFromSeq( seq );
    }

BOOL QDocument::Balance( long seq )
    {
    // exclusive min. and max. sequence numbers
    long seqMin( getHighestSeqBalance( ) );
    long seqMax( getHighestSeq( ) + 1L );

    ASSERT( seqMin < seqMax );
    if ( seq <= seqMin || seq >= seqMax )
        return FALSE;
    
    QSequenceElement *pseq = new QSequenceElement( seq );
    if ( !pseq )
        THROW_EXCEPTION( exMemory );
    
    m_listSeq.AddTail( pseq );
    checkConsistency( );
    return TRUE;
    }

BOOL QDocument::DeleteTransaction( long seq )
    {
    // exclusive min. and max. sequence numbers
    long seqMin( getHighestSeqBalance( ) );
    long seqMax( getHighestSeq( ) + 1L );

    ASSERT( seqMin < seqMax );
    if ( seq <= seqMin || seq >= seqMax )
        return FALSE;
    
    m_list.DeleteTransaction( seq );
    m_accGeneral.DeleteTransaction( seq );
    checkConsistency( );
    return TRUE;
    }

BOOL QDocument::UnBalance( )
    {
    if ( !m_listSeq.GetTail( ) )
        return FALSE;

    delete (QSequenceElement *) m_listSeq.RemoveTail( );

    checkConsistency( );
    return TRUE;
    }

long QDocument::getHighestSeqBalance( ) const
    {
    QSequenceElement *pseq = (QSequenceElement *) m_listSeq.GetTail( );
    if ( pseq )
        return pseq->GetSeq( );
    return seqNil;
    }

long QDocument::getHighestSeq( ) const
    {
    return m_list.getHighestSeq( );
    }

UINT QDocument::NumberOfAccounts( ) const
    {
    return m_list.GetCount( );
    }

QAccount *QDocument::GetGeneralAccount( )
    {
    return &m_accGeneral;
    }

QAccount *QDocument::FindAccount( const CHAR *pszAccountName )
    {
    for ( QAccount *pacc = (QAccount *) m_list.GetHead( ); 
          pacc; 
          pacc = (QAccount *) m_list.GetNext( pacc ) )
        {
        if ( lstrcmpi( pacc->GetName( ), pszAccountName ) == 0 )
            break;
        }
    return pacc;
    }

const QVirtualList *QDocument::model( ) const
{
    return &m_list;
}

void QDocument::SetModifiedFlag( )
    {
    m_fModified = TRUE;
    }

void QDocument::ClearModifiedFlag( )
    {
    m_fModified = FALSE;
    }

BOOL QDocument::IsModified( ) const
    {
    return m_fModified;
    }

void QDocument::SetFileName( const CHAR *pszFileName )
    {
    m_sFileName = pszFileName;
    }

const  CHAR *QDocument::GetFileName( ) const
    {
    return m_sFileName;
    }

void QDocument::BuildGeneralAccount( )
    {
    QTransaction txGeneral;
    QString      sDescriptionNew;
    QTxb         txbNew;

    QAccount *pacc;
    QTxb     *ptxb;
    
    TRY
        {
        m_accGeneral.Empty( );
        for ( pacc = (QAccount *) m_list.GetHead( ); 
              pacc;
              pacc = (QAccount *) m_list.GetNext( pacc ) )
            {
            for ( ptxb = pacc->GetHead( );
                  ptxb;
                  ptxb = pacc->GetNext( ptxb ) )
                {
                txGeneral = ptxb->GetTransaction( );
                sDescriptionNew = QString( "[" ) + pacc->GetName( ) + "] " + txGeneral.GetDescription( );
                txGeneral.SetDescription( sDescriptionNew );
                txbNew.SetTransaction( txGeneral );
                txbNew.SetSeq( ptxb->GetSeq( ) );
                m_accGeneral.InsertRawTxb( txbNew );
                }
            }
        
        // general account has not been balanced yet
        m_accGeneral.reBalance( );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        txGeneral.Cleanup( );
        sDescriptionNew.Cleanup( );
        txbNew.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

void QDocument::Store( QArchive &ar )
    {
    QVersion ver( m_v.GetMajorNumber( ), m_v.GetMinorNumber( ), m_v.GetRevisionNumber( ) );
    ar.WriteHeader( otDocument );
    ar.Write( &m_nSig, sizeof( m_nSig ) );
    ver.Store( ar );
    m_list.Store( ar );
    m_listSeq.Store( ar );
    }

void QDocument::Retrieve( QArchive &ar, UINT ot )
    {
    QVersion ver;
    UINT cb;
    UINT nSig;

    TRY
        {
        if ( ot != otDocument )
            THROW_EXCEPTION( exFile );
        cb = ar.Read( &nSig, sizeof( nSig ) );
        if ( cb != sizeof( nSig ) || nSig != m_nSig )
            THROW_EXCEPTION( exFile );
            
        ar.ReadHeader( &ot );
        ver.Retrieve( ar, ot );
        if ( ver.GetMajorNumber( )    != m_v.GetMajorNumber( ) ||
             ver.GetMinorNumber( )    != m_v.GetMinorNumber( ) ||
             ver.GetRevisionNumber( ) != m_v.GetRevisionNumber( ) )
            THROW_EXCEPTION( exFile );

        ar.ReadHeader( &ot );
        m_list.Retrieve( ar, ot );
        ar.ReadHeader( &ot );
        m_listSeq.Retrieve( ar, ot );

        BuildGeneralAccount( );
        checkConsistency( );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        Empty( );
        THROW_LAST( );
        }
    END_CATCH
    }

/////////////////////////////////////////
// QAccount methods

QAccount::QAccount( )
    {
    }

QAccount::QAccount( const CHAR *pszName )
    :m_sName( pszName )
    {
    }

QAccount::~QAccount( )
    {
    Cleanup( );
    }

void QAccount::Cleanup( )
    {
    m_sName.Cleanup( );
    m_list.Cleanup( );
    }

void QAccount::Empty( )
    {
    m_list.Cleanup( );
    }

UINT QAccount::GetCount( ) const
    {
    return m_list.GetCount( );
    }
    
QTxb *QAccount::GetHead( ) const 
    { 
    return (QTxb *) m_list.GetHead( ); 
    }

QTxb *QAccount::GetTail( ) const 
    { 
    return (QTxb *) m_list.GetTail( ); 
    }

QTxb *QAccount::GetNext( const QTxb *ptxb ) const 
    { 
    return (QTxb *) m_list.GetNext( ptxb ); 
    }

QTxb *QAccount::GetPrev( const QTxb *ptxb ) const 
    { 
    return (QTxb *) m_list.GetPrev( ptxb ); 
    }

const CHAR *QAccount::GetName( ) const
    {
    return m_sName;
    }

void QAccount::SetName( const CHAR *pszName )
    {
    m_sName = pszName;
    }

void QAccount::AddTransaction( BOOL insert, const QTransaction &tx, long seq )
    {
    m_list.AddTransaction( insert, tx, seq );
    }

void QAccount::DeleteTransaction( long seq )
    {
    m_list.DeleteTransaction( seq );
    }

void QAccount::reBalance( )
    {
    m_list.reBalance( );
    }

Money QAccount::GetBalance( ) const
    {
    Money monRet;
    const QTxb *ptxb;

    ptxb = (QTxb *) m_list.GetTail( );
    if ( ptxb )
        monRet = ptxb->GetBalance( );

    return monRet;
    }

const QTxb *QAccount::txbFromSeq( long seq ) const
    {
    const QTxb *ptxb;

    for ( ptxb = (QTxb *) m_list.GetHead( ); ptxb; ptxb = (QTxb *) m_list.GetNext( ptxb ) )
        if ( ptxb->GetSeq( ) == seq )
            return ptxb;

    return NULL;
    }

long QAccount::getHighestSeq( ) const
    {
    QTxb *ptxb = (QTxb *) m_list.GetTail( );
    if ( ptxb )
        return ptxb->GetSeq( );
    return seqNil;
    }

long QAccount::getHighestSeq( const QTime &date ) const
    {
    return m_list.getHighestSeq( date );
    }

void QAccount::Store( QArchive &ar )
    {
    ar.WriteHeader( otAccount );
    m_sName.Store( ar );
    m_list.Store( ar );
    }

void QAccount::InsertRawTxb( const QTxb &txb )
    {
    m_list.InsertRawTxb( txb );
    }

void QAccount::Retrieve( QArchive &ar, UINT ot )
    {
    TRY
        {
        ASSERT( ot == otAccount );
        ar.ReadHeader( &ot );
        m_sName.Retrieve( ar, ot );
        ar.ReadHeader( &ot );
        m_list.Retrieve( ar, ot );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

const QVirtualList *QAccount::model( ) const
{
    return &m_list;
}

/////////////////////////////////////////
// QSequenceElement methods

QSequenceElement::QSequenceElement( long seq )
    :seq_( seq )
    {
    }
    
long QSequenceElement::GetSeq( ) const
    {
    return seq_;
    }

void QSequenceElement::SetSeq( long seq )
    {
    ASSERT( seq_ >= 0L );
    seq_ = seq;
    }

void QSequenceElement::Store( QArchive &ar )
    {
    ar.WriteHeader( otSequenceElement );
    ar.Write( &seq_, sizeof( seq_ ) );
    }

void QSequenceElement::Retrieve( QArchive &ar, UINT ot )
    {
    ASSERT( ot == otSequenceElement );
    UINT cb = ar.Read( &seq_, sizeof( seq_ ) );
    if ( cb != sizeof( seq_ ) )
        THROW_EXCEPTION( exFile );
    }

/////////////////////////////////////////
// QSequenceList methods

QSequenceList::QSequenceList( )
    {
    }

QSequenceList::~QSequenceList( )
    {
    Cleanup( );
    }

void QSequenceList::Cleanup( )
    {
    QListObject *plob;
    while ( plob = RemoveHead( ) )
        delete (QSequenceElement *) plob;
    }

void QSequenceList::Store( QArchive &ar )
    {
    ar.WriteHeader( otSequenceList );
    for( QSequenceElement *pseq = (QSequenceElement *) GetHead( );
         pseq;
         pseq = (QSequenceElement *) GetNext( pseq ) )
        {
        pseq->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QSequenceList::Retrieve( QArchive &ar, UINT ot )
    {
    QSequenceElement *pseq;

    TRY
        {
        ASSERT( ot == otSequenceList );
        while( 1 )
            {
            ar.ReadHeader( &ot );
            if ( ot == otEndMarker )
                break;

            pseq = new QSequenceElement;
            if ( !pseq )
                THROW_EXCEPTION( exMemory );

            pseq->Retrieve( ar, ot );
            AddTail( pseq );
            }
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

/////////////////////////////////////////
// QAccountList methods

QAccountList::QAccountList( )
    {
    }

QAccountList::~QAccountList( )
    {
    Cleanup( );
    }

void QAccountList::Cleanup( )
    {
    QListObject *plob;
    while ( plob = RemoveHead( ) )
        delete (QAccount *) plob;
    }

const QTxb *QAccountList::txbFromSeq( long seq ) const
    {
    QAccount *pacc;
    for ( pacc = (QAccount *) GetHead( ); pacc; pacc = (QAccount *) GetNext( pacc ) )
        {
        const QTxb *ptxb = pacc->txbFromSeq( seq );
        if ( ptxb )
            return ptxb;
        }
    return NULL;
    }

long QAccountList::getHighestSeq( ) const
    {
    QAccount *pacc;
    long seq;
    long seqMax = seqNil;

    for ( pacc = (QAccount *) GetHead( ); pacc; pacc = (QAccount *) GetNext( pacc ) )
        {
        seq = pacc->getHighestSeq( );
        if ( seq > seqMax )
            seqMax = seq;
        }

    ASSERT( seqMax == _pdoc->GetGeneralAccount( )->getHighestSeq( ) );
    return seqMax;
    }

long QAccountList::getHighestSeq( const QTime &date ) const
    {
    QAccount *pacc;
    long seq;
    long seqMax = seqNil;

    for ( pacc = (QAccount *) GetHead( ); pacc; pacc = (QAccount *) GetNext( pacc ) )
        {
        seq = pacc->getHighestSeq( date );
        if ( seq > seqMax )
            seqMax = seq;
        }
    return seqMax;
    }

void QAccountList::DeleteTransaction( long seq )
    {
    QAccount *pacc;

    for ( pacc = (QAccount *) GetHead( ); pacc; pacc = (QAccount *) GetNext( pacc ) )
        pacc->DeleteTransaction( seq );
    }

BOOL QAccountList::AddTransaction( const CHAR *pszAccount, const QTransaction &tx, long &seq )
    {
    QAccount *pacc;
    UINT n = 0;

    // find the largest sequence number for given date
    seq = getHighestSeq( tx.GetDate( ) );

    // new sequence number is one greater than highest sequence number
    seq++;

    // insert the transaction into each account
    for ( pacc = (QAccount *) GetHead( ); pacc; pacc = (QAccount *) GetNext( pacc ) )
        {
        BOOL insert = ( lstrcmpi( pacc->GetName( ), pszAccount ) == 0 );

        // only one account (at most) allowed
        if ( insert )
            ++n;
        pacc->AddTransaction( insert, tx, seq );
        }
    ASSERT( n <= 1 );
    return ( n > 0 );
    }

void QAccountList::Store( QArchive &ar )
    {
    ar.WriteHeader( otAccountList );
    for( QAccount *pacc = (QAccount *) GetHead( );
         pacc;
         pacc = (QAccount *) GetNext( pacc ) )
        {
        pacc->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QAccountList::Retrieve( QArchive &ar, UINT ot )
    {
    QAccount *pacc;

    TRY
        {
        ASSERT( ot == otAccountList );
        while( 1 )
            {
            ar.ReadHeader( &ot );
            if ( ot == otEndMarker )
                break;

            pacc = new QAccount;
            if ( !pacc )
                THROW_EXCEPTION( exMemory );

            pacc->Retrieve( ar, ot );
            AddTail( pacc );
            }
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

/////////////////////////////////////////
// QTransaction methods

QTransaction::QTransaction( )
    {
    }

QTransaction::QTransaction( QTime date, const CHAR *pszDescription, Money monAmount )
    :m_date( date ), m_monAmount( monAmount ), m_sDescription( pszDescription )
    {
    }

QTransaction::~QTransaction( )
    {
    Cleanup( );
    }

void QTransaction::Cleanup( )
    {
    m_date.Cleanup( );
    m_sDescription.Cleanup( );
    }

const QTime &QTransaction::GetDate( ) const
    {
    return m_date;
    }

void QTransaction::SetDate( const QTime &date )
    {
    m_date = date;
    }

const CHAR *QTransaction::GetDescription( ) const
    {
    return m_sDescription;
    }

void QTransaction::SetDescription( const CHAR *pszDescription )
    {
    m_sDescription = pszDescription;
    }

Money QTransaction::GetAmount( ) const
    {
    return m_monAmount;
    }

void QTransaction::SetAmount( Money monAmount )
    {
    m_monAmount = monAmount;
    }

void QTransaction::Store( QArchive &ar )
    {
    ar.WriteHeader( otTransaction );
    m_date.Store( ar );
    m_monAmount.Store( ar );
    m_sDescription.Store( ar );
    }

void QTransaction::Retrieve( QArchive &ar, UINT ot )
    {
    TRY
        {
        ASSERT( ot == otTransaction );
        ar.ReadHeader( &ot );
        m_date.Retrieve( ar, ot );
        ar.ReadHeader( &ot );
        m_monAmount.Retrieve( ar, ot );
        ar.ReadHeader( &ot );
        m_sDescription.Retrieve( ar, ot );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

/////////////////////////////////////////
// QTxb methods

QTxb::QTxb( )
    :m_seq( seqNil )
    {
    }

QTxb::QTxb( const QTransaction &tx, long seq )
    :m_tx( tx ), m_seq( seq )
    {
    }

QTxb::~QTxb( )
    {
    Cleanup( );
    }

void QTxb::Cleanup( )
    {
    m_tx.Cleanup( );
    m_monBalance = 0L;
    m_seq = seqNil;
    }

void QTxb::SetTransaction( const QTransaction &tx )
    {
    m_tx = tx;
    }

const QTransaction &QTxb::GetTransaction( ) const
    {
    return m_tx;
    }

void QTxb::SetBalance( Money monBalance )
    {
    m_monBalance = monBalance;
    }

Money QTxb::GetBalance( ) const
    {
    return m_monBalance;
    }

long QTxb::GetSeq( ) const
    {
    return m_seq;
    }

void QTxb::SetSeq( long seq )
    {
    ASSERT( seq >= 0L );
    m_seq = seq;
    }

void QTxb::Store( QArchive &ar )
    {
    ar.WriteHeader( otTxb );
    m_tx.Store( ar );
    m_monBalance.Store( ar );
    ar.Write( &m_seq, sizeof( m_seq ) );
    }

void QTxb::Retrieve( QArchive &ar, UINT ot )
    {
    TRY
        {
        ASSERT( ot == otTxb );
        ar.ReadHeader( &ot );
        m_tx.Retrieve( ar, ot );
        ar.ReadHeader( &ot );
        m_monBalance.Retrieve( ar, ot );
        UINT cb = ar.Read( &m_seq, sizeof( m_seq ) );
        if ( cb != sizeof( m_seq ) )
            THROW_EXCEPTION( exFile );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

/////////////////////////////////////////
// QTxbList methods

QTxbList::QTxbList( )
    {
    }

QTxbList::~QTxbList( )
    {
    Cleanup( );
    }

void QTxbList::Cleanup( )
    {
    QListObject *plob;
    while ( plob = RemoveHead( ) )
        delete (QTxb *) plob;
    }

void QTxbList::reBalance( )
    {
    reBalance( (QTxb *) GetHead( ) );
    }

void QTxbList::reBalance( QTxb *ptxb )
    {
    for ( ; ptxb; ptxb = (QTxb *) GetNext( ptxb ) )
        {
        QTxb *ptxbPrev = (QTxb *) GetPrev( ptxb );
        Money monBalancePrev;

        if ( ptxbPrev )
            monBalancePrev = ptxbPrev->GetBalance( );
        else
            monBalancePrev = 0L;
        ptxb->SetBalance( monBalancePrev + ptxb->GetTransaction( ).GetAmount( ) );
        }
    }

void QTxbList::reSequence( QTxb *ptxb, QTxbList::Direction d )
    {
    long n = ( d == UP ? 1L : -1L );

    for ( ; ptxb; ptxb = (QTxb *) GetNext( ptxb ) )
        ptxb->SetSeq( ptxb->GetSeq( ) + n );
    }
    
void QTxbList::InsertRawTxb( const QTxb &txb )
    {
    QTxb *ptxb;
    QTxb *ptxbNew = NULL;

    // advance to insertion position
    for ( ptxb = (QTxb *) GetHead( ); ptxb && ptxb->GetSeq( ) < txb.GetSeq( ); ptxb = (QTxb *) GetNext( ptxb ) )
        ;
    
#ifdef DEBUG    
    // strict inequality
    if ( ptxb )
        ASSERT( ptxb->GetSeq( ) > txb.GetSeq( ) );
#endif

    ptxbNew = new QTxb( txb );
    if ( !ptxbNew )
        THROW_EXCEPTION( exMemory );
    
    if ( ptxb )
        InsertBefore( ptxb, ptxbNew );
    else
        AddTail( ptxbNew );
    }

void QTxbList::DeleteTransaction( long seq )
    {
    QTxb *ptxb;
        
    // advance to deletion position
    for ( ptxb = (QTxb *) GetHead( ); ptxb && ptxb->GetSeq( ) < seq; ptxb = (QTxb *) GetNext( ptxb ) )
        ;

    // perform the deletion
    if ( ptxb && ptxb->GetSeq( ) == seq )
        {
        QTxb *ptxbSav = ptxb;
        ptxb = (QTxb *) GetNext( ptxb );
        Remove( ptxbSav );
        delete ptxbSav;
        }

    // resequence and rebalance
    reSequence( ptxb, DOWN );
    reBalance( ptxb );
    }

void QTxbList::AddTransaction( BOOL insert, const QTransaction &tx, long seq )
    {
    QTxb *ptxb;
    QTxb *ptxbNew = NULL;

    // advance to insertion position
    for ( ptxb = (QTxb *) GetHead( ); ptxb && ptxb->GetSeq( ) < seq; ptxb = (QTxb *) GetNext( ptxb ) )
        ;
        
    // if inserting into this account, allocate Txb and insert it
    if ( insert )
        {
        ptxbNew = new QTxb( tx, seq );
        if ( !ptxbNew )
            THROW_EXCEPTION( exMemory );
        
        if ( ptxb )
            InsertBefore( ptxb, ptxbNew );
        else
            AddTail( ptxbNew );
        
        // must resequence starting from the next entry
        reSequence( (QTxb *) GetNext( ptxbNew ), UP );

        // must rebalance starting from the current entry
        reBalance( ptxbNew );
        }
    else
        {
        // rebalance and resequence from the current entry
        reSequence( ptxb, UP );
        reBalance( ptxb );
        }
    }

void QTxbList::Store( QArchive &ar )
    {
    ar.WriteHeader( otTxbList );
    for ( QTxb *ptxb = (QTxb *) GetHead( );
          ptxb;
          ptxb = (QTxb *) GetNext( ptxb ) )
        {
        ptxb->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QTxbList::Retrieve( QArchive &ar, UINT ot )
    {
    QTxb *ptxb;

    TRY
        {
        ASSERT( ot == otTxbList );
        while( 1 )
            {
            ar.ReadHeader( &ot );
            if ( ot == otEndMarker )
                break;

            ptxb = new QTxb;
            if ( !ptxb )
                THROW_EXCEPTION( exMemory );
            ptxb->Retrieve( ar, ot );
            AddTail( ptxb );
            }
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
        Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

long QTxbList::getHighestSeq( const QTime &date ) const
    {
    QTxb *ptxb;

    long seqMax = seqNil;
    for ( ptxb = (QTxb *) GetHead( ); ptxb && ptxb->GetTransaction( ).GetDate( ) <= date; ptxb = (QTxb *) GetNext( ptxb ) )
        {
        if ( ptxb->GetSeq( ) > seqMax )
            seqMax = ptxb->GetSeq( );
        }
    return seqMax;
    }

/////////////////////////////////////////
// Money methods

Money::Money( )
    :cents_( 0l )
{
}

Money::Money( const Money &rhs )
    :cents_( rhs.cents_ )
{
}

Money::Money( long cents )
    :cents_( cents )
{
}

Money::Money( const CHAR *sz )
    :cents_( Money::asciiToMoney( sz ) )
{
}

Money::~Money( )
{
}

const CHAR *Money::asString( ) const
{
    static CHAR rgb[ cbBufferMax + 1 ];
    long absCents = ( cents_ >= 0l ? cents_ : -cents_ );
    long dollars  = absCents / 100l;
    long cents    = absCents % 100l;

    if ( cents_ >= 0l )
        sprintf( rgb, "%ld.%02ld", dollars, cents );
    else
        sprintf( rgb, "(%ld.%02ld)", dollars, cents );

    return rgb;
}

const Money &Money::operator=( const Money &rhs )
{
    if ( this != &rhs )
        this->cents_ = rhs.cents_;
    return *this;
}

const Money &Money::operator+=( const Money &rhs )
{
    cents_ += rhs.cents_;
    return *this;
}

const Money &Money::operator-=( const Money &rhs )
{
    cents_ -= rhs.cents_;
    return *this;
}

Money operator+( const Money &rhs )
{
    return rhs;
}

Money operator-( const Money &rhs )
{
    Money m;
    m.cents_ = -rhs.cents_;
    return m;
}

Money operator+( const Money &lhs, const Money &rhs )
{
    Money m( lhs );
    m += rhs;
    return m;
}

Money operator-( const Money &lhs, const Money &rhs )
{
    Money m( lhs );
    m -= rhs;
    return m;
}

BOOL operator==( const Money &lhs, const Money &rhs )
{
    return lhs.cents_ == rhs.cents_;
}

BOOL operator!=( const Money &lhs, const Money &rhs )
{
    return lhs.cents_ != rhs.cents_;
}

BOOL operator<( const Money &lhs, const Money &rhs )
{
    return lhs.cents_ < rhs.cents_;
}

BOOL operator<=( const Money &lhs, const Money &rhs )
{
    return lhs.cents_ <= rhs.cents_;
}

BOOL operator>( const Money &lhs, const Money &rhs )
{
    return lhs.cents_ > rhs.cents_;
}

BOOL operator>=( const Money &lhs, const Money &rhs )
{
    return lhs.cents_ >= rhs.cents_;
}
    
void Money::Store( QArchive &ar )
{
    ar.WriteHeader( otMoney );
    ar.Write( &cents_, sizeof( cents_ ) );
}

void Money::Retrieve( QArchive &ar, UINT ot )
{
    ASSERT( ot == otMoney );
    UINT cb = ar.Read( &cents_, sizeof( cents_ ) );
    if ( cb != sizeof( cents_ ) )
        THROW_EXCEPTION( exFile );
}

long Money::asciiToMoney( const CHAR *szMoney )
{
    int  i;
    long sign;
    long dollars;
    long cents;
    long money;
    CHAR *psz;
    CHAR sz[ cbBufferMax + 1];

    lmemset( sz, 0, sizeof( sz ) );
    ASSERT( lstrlen( szMoney ) < sizeof( sz ) );
    lstrcpy( sz, szMoney );

    // determine if the string is valid
    for ( i = 0; i < lstrlen( sz ); ++i )
    {
        if ( !isdigit( sz[ i ] ) && sz[ i ] != '.' && sz[ i ] != '+' && sz[ i ] != '-' )
            return 0L;
    }

    // compute the sign multiplier
    sign = 1L;
    psz  = sz;
    if ( *psz == '+' || *psz == '-' )
    {
        sign = ( *psz == '+' ? 1L : -1L );
        ++psz;
    }

    // if we've reached the end of the string, return error
    if ( !*psz )
        return 0L;

    // if there are any more occurences of signs, return error
    if ( strchr( psz, '+' ) != NULL || strchr( psz, '-' ) != NULL )
        return 0L;

    // compute the dollars amount, and position psz for the cents' extraction
    if ( psz[ 0 ] == '.' )
    {
        dollars = 0L;
        psz = strtok( psz, "." );
    }
    else
    {
        psz = strtok( psz, "." );
        ASSERT( psz != NULL );
        dollars = atol( psz );
        psz = strtok( NULL, "." );
    }
    
    // compute the cents amount
    cents = 0L;
    if ( psz )
    {
        long nDecimals = lstrlen( psz );

        // cannot have more than 2 decimals
        if ( nDecimals > 2 )
            return 0L;
        
        ASSERT( nDecimals >= 0 );
        ASSERT( nDecimals <= 2 );
        for ( i = 0; i < nDecimals; ++i )
        {
            long dec = ( psz[ i ] - '0' );
            dec *= longExp( 1 - i );
            cents += dec;
        }
    }

    // if any more decimal-separated tokens, return error
    psz = strtok( NULL, "." );
    if ( psz )
        return 0L;

    // return the amount
    money = sign * ( cents + dollars * 100L );
    return money;
}

// Compute 10 to the n'th power
long Money::longExp( long n )
{
    long i;
    long ret = 1L;

    ASSERT( n >= 0 );
    for ( i = 0; i < n; ++i )
        ret *= 10L;

    return ret;
}
