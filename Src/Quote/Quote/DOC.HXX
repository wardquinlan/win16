//////////////////////////////////////////////////////////////////////////////
// DOC.HXX
//
// This file contains the document class definitions
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _DOC_HXX
#define _DOC_HXX

// File Maximum name length
#define _cbFileMax 12

//////////////////////////////////////////////////////////////////////////////
// class QQt
//
// A quote class.  Encapsulates a stock quote.
//////////////////////////////////////////////////////////////////////////////
class QQt : public QListObject
    {
public:
    QQt( );
    QQt( const QQt &qt );
    QQt( const QTime &date, NUM numOp, NUM numHi, NUM numLo, NUM numCl, NUM numVl );
    virtual ~QQt( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    const QQt &operator=( const QQt &qt );

    const QTime &GetDate( ) const;
    void SetDate( const QTime &date );

    NUM  GetOp( ) const;
    void SetOp( NUM num );

    NUM  GetHi( ) const;
    void SetHi( NUM num );

    NUM  GetLo( ) const;
    void SetLo( NUM num );

    NUM  GetCl( ) const;
    void SetCl( NUM num );

    NUM  GetVl( ) const;
    void SetVl( NUM num );

    BOOL IsValid( ) const;

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    QTime m_date;
    NUM   m_numOp;
    NUM   m_numHi;
    NUM   m_numLo;
    NUM   m_numCl;
    NUM   m_numVl;
    };

//////////////////////////////////////////////////////////////////////////////
// class QQtList
//
// Quote list class.  Maintains a virtual list of quotes.
//////////////////////////////////////////////////////////////////////////////
class QQtList : public QVirtualList
    {
public:
    QQtList( );
    virtual ~QQtList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QDocument
//
// The document class.  Maintains all information about the loaded document
// (there is only one).
//////////////////////////////////////////////////////////////////////////////
class QDocument : public QObject
    {
public:
    QDocument( );
    virtual ~QDocument( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );
    virtual void StoreText( FILE *pfile );
    virtual void RetrieveText( FILE *pfile, BOOL fSilent = FALSE );

    // Other Operations
    const QVirtualList *model( ) const { return &m_list; }
    BOOL  GetModifiedFlag( ) const;
    void  SetModifiedFlag( BOOL fModified );
    void  Add( const QQt &qt, BOOL fSearch );
    QQt  *GetFirst( );
    QQt  *GetLast( );
    QQt  *GetNext( const QQt *pqqt );
    QQt  *GetPrev( const QQt *pqqt );
    UINT  GetCount( ) const;
    BOOL  Remove( const QTime &date );
    const CHAR *GetFileName( ) const;
    void  SetFileName( const CHAR *pszFileName );
    void  BuildQuoteData( LPDATASET lpdatasetOp,
                          LPDATASET lpdatasetHi, 
                          LPDATASET lpdatasetLo, 
                          LPDATASET lpdatasetCl, 
                          LPDATASET lpdatasetVl,
                          LPDATASET lpdatasetX,
                          LPDATASET lpdatasetCount );
    BOOL  readOnly( ) const
        {
        return m_fReadOnly;
        }
    void  readOnly( BOOL fReadOnly )
        {
        m_fReadOnly = fReadOnly;
        }
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

    // Public members
    static const QVersion m_v;          // QObject version
    static const CHAR *szOutputFormat;  // Output format
                                        // Date Output formats
    static const CHAR *szOutputFormatDate;
    static const CHAR *szOutputFormatDate2;
    static const CHAR *szOutputFormatVl;// Volume format
    static const CHAR *szOutputFormatDataSet; // Dataset output format
    static const CHAR *szDataSetBlank; // Dataset blanks

#if ( FDATA == 1 )
    static const CHAR FAR m_szYr[];
    static const CHAR FAR m_szMn[];
    static const CHAR FAR m_szDy[];
    static const CHAR FAR m_szOp[];
    static const CHAR FAR m_szHi[];
    static const CHAR FAR m_szLo[];
    static const CHAR FAR m_szCl[];
    static const CHAR FAR m_szVl[];
    static const CHAR FAR m_szIg[];
    static const CHAR FAR m_szYMD[];
    static const CHAR FAR m_szX[];
    static const CHAR FAR m_szCount[];
#else    
    static const CHAR m_szYr[];
    static const CHAR m_szMn[];
    static const CHAR m_szDy[];
    static const CHAR m_szOp[];
    static const CHAR m_szHi[];
    static const CHAR m_szLo[];
    static const CHAR m_szCl[];
    static const CHAR m_szVl[];
    static const CHAR m_szIg[];
    static const CHAR m_szYMD[];
    static const CHAR m_szX[];
    static const CHAR m_szCount[];
#endif

private:
    enum State
        {
        stateChar,
        stateEscape,
        stateNativeValue,
        stateIndicatorValue
        };
    
    void BuildOutputString( CHAR *pszBuffer, USHORT cb, QQt *pqt, USHORT nIndex );
    void ParseInputString( CHAR *pszBuffer, QTime *pdate, NUM *pnumOp, NUM *pnumHi, NUM *pnumLo, NUM *pnumCl, NUM *pnumVl );
    void ConvertDelimiters( CHAR *pszDelimitersDst, const CHAR *pszDelimitersSrc );
    
    QString  m_sFileName;               // DOS file name 
    BOOL     m_fModified;               // Modified Flag
    BOOL     m_fReadOnly;
    QQtList  m_list;                    // List of quotes
    };

//////////////////////////////////////////////////////////////////////////////
// class QGCache
//
// High-level interface class to cached indicator templates and DATASETs.
// These members can throw exceptions (typically exMemory) if resources
// can't be allocated.
//////////////////////////////////////////////////////////////////////////////
class QGCache : public QObject
    {
friend class QDocument;
public: 
    // Constructors/Destructors
    QGCache( );
    virtual ~QGCache( );
    virtual void Cleanup( );
    
    // Other Operations
    const DATASET &op( );
    const DATASET &hi( );
    const DATASET &lo( );
    const DATASET &cl( );
    const DATASET &vl( );
    const DATASET &x( );
    const DATASET &count( );

    const QCache  *pcache( const CHAR *pszIndicatorName, int nRecurseLevel );
    const QRuleCache *prulecache( const CHAR *pszName, int nRecurseLevel );
    const QCondCache *pcondcache( const CHAR *pszName, int nRecurseLevel );
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    void DumpContents( ) const;
    #endif

    static const int nRecurseLevelMax;

private:
    void           HandleDirtyDataSets( );
    void           BuildStatistics( );

    QCacheList     m_cachelist;
    QRuleCacheList m_rulecachelist;
    QCondCacheList m_condcachelist;
    DATASET        m_datasetOp;
    DATASET        m_datasetHi;
    DATASET        m_datasetLo;
    DATASET        m_datasetCl;
    DATASET        m_datasetVl;
    DATASET        m_datasetX;
    DATASET        m_datasetCount;
    BOOL           m_fDataSetsDirty;
    };
    
//////////////////////////////////////////////////////////////////////////////
// classes QDirElement, QDirectory
//
// Encapsulates a DOS directory
//////////////////////////////////////////////////////////////////////////////
struct QDirElement
    {
public:
    // Constructors
    QDirElement( );
    ~QDirElement( );
    
    // Operations
    void set( const CHAR *pszName );
    static int compare( const void *elem1, const void *elem2 );
    
    // Accessors
    const CHAR *filename( ) const
        {
        return m_szFile;
        }
    const CHAR *extension( ) const
        {
        return m_szExt;
        }
    const CHAR *fullname( ) const
        {
        return m_szFullName;
        }
    
private:
    CHAR m_szFullName[ _cbFileMax + 1 ];
    CHAR m_szFile    [ _cbFileMax + 1 ];
    CHAR m_szExt     [ _cbFileMax + 1 ];
    };

class QDirectory
    {
public:
    // Constructors
    QDirectory( );
    virtual ~QDirectory( );
    virtual void Cleanup( );

    // Operations
    void  read( void );
    UINT  size( ) const
        {
        return m_nAlloc;
        }
    const QDirElement &operator[ ]( UINT idx ) const;

private:

    QDirElement *m_rg;
    UINT         m_nAlloc;
    };

//////////////////////////////////////////////////////////////////////////////
// class QExternalIndicatorRecord
//
// External indicator record
//////////////////////////////////////////////////////////////////////////////
class QExternalIndicatorRecord : public QListObject
{
public:
    QExternalIndicatorRecord( );
    QExternalIndicatorRecord( const QExternalIndicatorRecord &rhs );
    virtual ~QExternalIndicatorRecord( );
    virtual void Cleanup( );

    const QTime &GetDate( ) const;
    void SetDate( const QTime &date );

    const CHAR *GetFileName( ) const;
    void SetFileName( const CHAR *pszName );

    const CHAR *GetName( ) const;
    void SetName( const CHAR *pszName );

    NUM GetValue( ) const;
    void SetValue( NUM num );

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    QTime   m_date;
    QString m_sFileName;
    QString m_sName;
    NUM     m_numValue;
};

//////////////////////////////////////////////////////////////////////////////
// class QExternalIndicatorList
//
// QExternalIndicator list class.  Maintains a virtual list of external indicators.
//////////////////////////////////////////////////////////////////////////////
class QExternalIndicatorList : public QVirtualList
    {
public:
    QExternalIndicatorList( );
    virtual ~QExternalIndicatorList( );
    virtual void Cleanup( );

    BOOL InsertIndicator( QExternalIndicatorRecord *prec );

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QExternalIndicatorDoc
//
// Document class of external indicators
//////////////////////////////////////////////////////////////////////////////
class QExternalIndicatorDoc : public QObject
    {
public:
    QExternalIndicatorDoc( );
    virtual ~QExternalIndicatorDoc( );
    virtual void Cleanup( );

    virtual void RetrieveText( const CHAR *pszFileName );
    const QVirtualList *model( ) const { return &m_list; }

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    void ParseInputString( CHAR *pszBuffer, QTime *pdate, CHAR *pszFileName, CHAR *pszName, NUM *pnumValue );

    QExternalIndicatorList m_list;
    };

#endif
