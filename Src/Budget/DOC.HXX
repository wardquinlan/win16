#ifndef _DOC_HXX
#define _DOC_HXX

// define the NIL sequence number
#define seqNil (0L)

class Money
    {
public:
    Money( );
    Money( const Money &rhs );
    Money( const CHAR *sz );
    Money( long cents );
    virtual ~Money( );

    // Assignment operators
    const Money &operator=( const Money &rhs );
    const Money &operator+=( const Money &rhs );
    const Money &operator-=( const Money &rhs );

    // String output
    const CHAR *asString( ) const;

    // Archival support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Friend Operators
    friend Money operator-( const Money &rhs );
    friend Money operator+( const Money &rhs );
    friend Money operator+( const Money &lhs, const Money &rhs );
    friend Money operator-( const Money &lhs, const Money &rhs );
    friend BOOL operator==( const Money &lhs, const Money &rhs );
    friend BOOL operator!=( const Money &lhs, const Money &rhs );
    friend BOOL operator< ( const Money &lhs, const Money &rhs );
    friend BOOL operator<=( const Money &lhs, const Money &rhs );
    friend BOOL operator> ( const Money &lhs, const Money &rhs );
    friend BOOL operator>=( const Money &lhs, const Money &rhs );

private:
    static long asciiToMoney( const CHAR *szMoney );
    static long longExp( long n );

    long cents_;
    };

class QTransaction : public QObject
    {
public:
    // Construction
    QTransaction( );
    QTransaction( QTime date, const CHAR *pszDescription, Money monAmount );

    // Destruction
    virtual ~QTransaction( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const QTime &GetDate( ) const;
    void  SetDate( const QTime &date );
    Money GetAmount( ) const;
    void  SetAmount( Money monAmount );
    const CHAR *GetDescription( ) const;
    void  SetDescription( const CHAR *pszDescription );
    
private:
    QTime   m_date;
    Money   m_monAmount;
    QString m_sDescription;
    };

// Transaction/Blance objects.
// Essentially a list of these make up an account.
class QTxb : public QListObject
    {
public:
    // Construction
    QTxb( );
    QTxb( const QTransaction &tx, long seq );
    
    // Destruction
    virtual ~QTxb( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const QTransaction &GetTransaction( ) const;
    void  SetTransaction( const QTransaction &tx );
    Money GetBalance( ) const;
    void  SetBalance( Money mon );
    long  GetSeq( ) const;
    void  SetSeq( long seq );
    
private:
    QTransaction m_tx;
    Money        m_monBalance;
    long         m_seq;
    };

class QTxbList : public QVirtualList
    {
public:
    // Construction
    QTxbList( );
    
    // Destruction
    virtual ~QTxbList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    void AddTransaction( BOOL insert, const QTransaction &tx, long seq );
    void InsertRawTxb( const QTxb &txb );
    long getHighestSeq( const QTime &date ) const;
    void reBalance( );
    void DeleteTransaction( long seq );
    
private:
    // disable copy constructor and assignment operator
    QTxbList( const QTxbList &rhs );
    const QTxbList &operator=( const QTxbList &list );

    enum Direction { UP, DOWN };
    void reBalance( QTxb *ptxb );
    void reSequence( QTxb *ptxb, Direction d );
    virtual void Abstract( ) { }
    };

class QAccount : public QListObject
    {
public:
    // Construction
    QAccount( );
    QAccount( const CHAR *pszName );

    // Destruction
    virtual ~QAccount( );
    virtual void Cleanup( );
    void Empty( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    long  getHighestSeq( const QTime &date ) const;
    long  getHighestSeq( ) const;
    void  reBalance( );
    const QVirtualList *model( ) const;
    UINT  GetCount( ) const;
    const CHAR *GetName( ) const;
    void  SetName( const CHAR *pszName );
    void  AddTransaction( BOOL insert, const QTransaction &tx, long seq );
    void  InsertRawTxb( const QTxb &txb );
    void  DeleteTransaction( long seq );
    Money GetBalance( ) const;
    const QTxb *txbFromSeq( long seq ) const;
    
    // QTxbList accessors
    QTxb *GetHead( ) const;
    QTxb *GetTail( ) const;
    QTxb *GetNext( const QTxb *ptxb ) const;
    QTxb *GetPrev( const QTxb *ptxb ) const;

private:
    QString  m_sName;
    QTxbList m_list;
    };

class QSequenceElement : public QListObject
    {
public:
    // Construction
    QSequenceElement( long seq = seqNil );
    
    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Accessors
    long GetSeq( ) const;
    void SetSeq( long seq );

private:
    long seq_;
    };

class QAccountList : public QVirtualList
    {
public:
    // Construction
    QAccountList( );
    
    // Destruction
    virtual ~QAccountList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    BOOL AddTransaction( const CHAR *pszAccount, const QTransaction &tx, long &seq );
    void DeleteTransaction( long seq );
    long getHighestSeq( const QTime &date ) const;
    long getHighestSeq( ) const;
    const QTxb *txbFromSeq( long seq ) const;
    
private:
    // disable copy constructor and assignment operator
    QAccountList( const QAccountList &rhs );
    const QAccountList &operator=( const QAccountList &list );

    virtual void Abstract( ) { }
    };

class QSequenceList : public QVirtualList
    {
public:
    // Construction
    QSequenceList( );

    // Destruction
    virtual ~QSequenceList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

private:
    // disable copy constructor and assignment operator
    QSequenceList( const QSequenceList &rhs );
    const QSequenceList &operator=( const QSequenceList &rhs );

    virtual void Abstract( ) { }
    };

class QDocument : public QObject
    {
public:
    // Construction
    QDocument( );

    // Destruction
    virtual ~QDocument( );
    virtual void Cleanup( );
    void Empty( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const QVirtualList *model( ) const;
    void  SetFileName( const CHAR *pszFileName );
    const CHAR *GetFileName( ) const;
    void  ClearFileName( );
    BOOL  IsModified( ) const;
    void  SetModifiedFlag( );
    void  ClearModifiedFlag( );
    BOOL  Balance( long seq );
    BOOL  UnBalance( );
    const QTxb *txbFromSeq( long seq ) const;
    const QTxb *txbFromSeqGeneral( long seq ) const;
    long  getHighestSeq( ) const;
    long  getHighestSeqBalance( ) const;
    void  AddAccount( const CHAR *pszName );
    BOOL  AddTransaction( const CHAR *pszAccount, const QTransaction &tx );
    BOOL  DeleteTransaction( long seq );
    BOOL  TransferTransaction( const CHAR *pszAccountFrom, const CHAR *pszAccountTo, const QTime &tDate, Money monAmount,const CHAR *pszDesc );
    UINT  NumberOfAccounts( ) const;
    QAccount *GetGeneralAccount( );
    QAccount *FindAccount( const CHAR *pszAccountName );
#ifdef DEBUG
    void checkConsistency( ) const;
#else
    void checkConsistency( ) const { }
#endif
          
    // QAccountList accessors
    QAccount *GetHead( ) const;
    QAccount *GetTail( ) const;
    QAccount *GetNext( const QAccount *pacc ) const;
    QAccount *GetPrev( const QAccount *pacc ) const;

private:
    void  BuildGeneralAccount( );    

    QString       m_sFileName;  // file name associated with this document
    BOOL          m_fModified;  // modified flag
    QAccount      m_accGeneral; // the general account
    QAccountList  m_list;       // list of accounts
    QSequenceList m_listSeq;    // list of sequences
    static const  QVersion m_v; // Document version
    static const  UINT m_nSig;  // signature
    };

#endif

