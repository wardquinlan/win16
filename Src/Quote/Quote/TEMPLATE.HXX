//////////////////////////////////////////////////////////////////////////////
// TEMPLATE.HXX
//
// This file contains the template class definitions
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _TEMPLATE_HXX
#define _TEMPLATE_HXX

enum GraphType { gtLine, gtBar, gtPoint };
enum DateType  { dtNone, dtMonths, dtYears };

//////////////////////////////////////////////////////////////////////////////
// class QFormat
//
// QFormat manages the file format loading/saving TEXT (.TXT) files.
//////////////////////////////////////////////////////////////////////////////
class QFormat : public QObject
    {
public:
    // Construction/Destruction
    QFormat( );
    virtual ~QFormat( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const CHAR *GetLoadString( );
    const CHAR *GetLoadDelimiters( );
    const CHAR *GetSaveString( );
    void  SetLoadString( const CHAR *psz );
    void  SetLoadDelimiters( const CHAR *psz );
    void  SetSaveString( const CHAR *psz );

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif
    
private:
    QString m_sLoad;
    QString m_sDelimiters;
    QString m_sSave;
    };

//////////////////////////////////////////////////////////////////////////////
// class QGeneralOptions
//
// QGeneralOptions manages general options data, such as global
// colors, font size, line widths, etc.
//////////////////////////////////////////////////////////////////////////////
class QGeneralOptions : public QObject
    {
public:
    enum Output
        {
        eScreen,
        ePrint
        };

    enum ScalingType
        {
        eByPage,
        eByChart
        };

    // Construction/Destruction
    QGeneralOptions( );
    virtual ~QGeneralOptions( );
    virtual void Cleanup( );
    
    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    USHORT      GetFontSize( ) const;
    void        SetFontSize( USHORT dyFont );
    
    const CHAR *GetBrowserName( ) const;
    void SetBrowserName( const CHAR *s );
    
    const CHAR *GetExternalFileName( ) const;
    void SetExternalFileName( const CHAR *s );

    USHORT      GetLineWidths( ) const;
    void        SetLineWidths( USHORT dxLine );
    USHORT      GetBackgroundColor( ) const;
    void        SetBackgroundColor( USHORT nIdxClr );
    USHORT      GetTextColor( ) const;
    void        SetTextColor( USHORT nIdxClr );
    USHORT      GetHLCColor( ) const;
    void        SetHLCColor( USHORT nIdxClr );
    USHORT      GetVolumeColor( ) const;
    void        SetVolumeColor( USHORT nIdxClr );
    USHORT      GetGridsColor( ) const;
    void        SetGridsColor( USHORT nIdxClr );
    USHORT      GetBuyColor( ) const;
    void        SetBuyColor( USHORT nIdxClr );
    USHORT      GetSellColor( ) const;
    void        SetSellColor( USHORT nIdxClr );
    BOOL        GetAutoOpenText( ) const;
    void        SetAutoOpenText( BOOL fAutoOpenText );
    BOOL        GetQuoteBox( ) const;
    void        SetQuoteBox( BOOL fQuoteBox );
    BOOL        GetTracker( ) const;
    void        SetTracker( BOOL fTracker );
    BOOL        GetTextGridLines( ) const;
    void        SetTextGridLines( BOOL f );
    DateType    GetDateType( ) const;
    void        SetDateType( DateType dt );
    ScalingType Scaling( Output device ) const;
    void        Scaling( Output device, ScalingType sc );
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    USHORT      m_dyFont;
    USHORT      m_dxLine;
    USHORT      m_nIdxClrBackground;
    USHORT      m_nIdxClrText;
    USHORT      m_nIdxClrHLC;
    USHORT      m_nIdxClrVolume;
    USHORT      m_nIdxClrGrids;
    USHORT      m_nIdxClrBuy;
    USHORT      m_nIdxClrSell;
    BOOL        m_fQuoteBox;
    BOOL        m_fTracker;
    BOOL        m_fAutoOpenText;
    BOOL        m_fTextGridLines;
    DateType    m_dt;
    ScalingType m_stScreen;
    ScalingType m_stPrint;
    QString     m_sBrowser;
    QString     m_sExternalFile;
    };

//////////////////////////////////////////////////////////////////////////////
// class QToken
//
// QToken is a collectable class of parser tokens.
//////////////////////////////////////////////////////////////////////////////
class QToken : public QListObject
    {
public:
    QToken( );
    virtual ~QToken( );
    virtual void Cleanup( );
    
    enum Tk
        {
        tkUnknown,         // <unknown>
        tkLBracket,        // [
        tkRBracket,        // ]
        tkLParen,          // (
        tkRParen,          // )
        tkPlus,            // +
        tkMinus,           // -
        tkTimes,           // *
        tkDivide,          // /
        tkEqual,           // ==
        tkNotEqual,        // !=
        tkLess,            // <
        tkLessEqual,       // <=
        tkGreater,         // >
        tkGreaterEqual,    // >=
        tkAnd,             // and
        tkOr,              // or
        tkFloat,           // <floating-point number>
        tkIndicator,       // #
        tkNative,          // $
        tkFunc,            // @
        tkScalarFunc,      // @
        tkComma,           // ,
        tkCond,            // ?
        tkRule,            // ~ (recursive rule)
        tkConst            // ^
        };
    
    Tk    GetTk( ) const;
    void  SetTk( Tk tk );
    const CHAR *GetName( ) const;
    void  SetName( const CHAR *pszName );
    NUM   GetValue( ) const;
    void  SetValue( NUM num );
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:    
    // This is a bit of a waste of space, but it isn't feasible 
    // to use a union.  Think about what would happen
    // in Cleanup( ).
    Tk      m_tk;
    NUM     m_numValue;
    QString m_sName;
    };

//////////////////////////////////////////////////////////////////////////////
// class QTokenList
//
// QTokenList is the list of tokens, input into the parser.
//////////////////////////////////////////////////////////////////////////////
class QTokenList : public QVirtualList
    {
public:
    // Construction/Destruction
    QTokenList( );
    virtual ~QTokenList( );
    virtual void Cleanup( );

    // Serialization Support not required
    
    // Other Operations
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QTokenIter
//
// QTokenList iterator object.
//////////////////////////////////////////////////////////////////////////////
class QTokenIter : public QObject
    {
public:
    // Construction/Destruction
    QTokenIter( const QTokenList &list );
    virtual ~QTokenIter( );
    virtual void Cleanup( );
    
    // Other Operations
    QToken *operator()( );
    QToken *Peek( ) const;
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    const QTokenList &m_list;
    QToken           *m_ptk;  // pointer to the next token about to be read
    };
    
    
//////////////////////////////////////////////////////////////////////////////
// QSignal
//
// This class encapsulates a signal to buy or sell.
//////////////////////////////////////////////////////////////////////////////
class QSignal : public QListObject
    {            
public:
    enum SignalType { sgBuy, sgSell };
    
    // Construction/Destruction
    QSignal( );
    virtual ~QSignal( );
    virtual void Cleanup( );
    
    // Serialization Support not required (for now)
    
    // Other Operations
    BOOL GetPendingState( ) const;
    void SetPendingState( BOOL f );
    const QTime &GetDate( ) const;
    void SetDate( const QTime &date );
    SignalType GetSg( ) const;
    void SetSg( SignalType sg );
    NUM GetPrice( ) const;
    void SetPrice( NUM price );
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    QTime      m_date;
    SignalType m_sg;
    NUM        m_numPrice;
    BOOL       m_fPendingState;
    };
    
//////////////////////////////////////////////////////////////////////////////
// QSignalList
//
// QSignalList encapsulates a list of QSignals.
//////////////////////////////////////////////////////////////////////////////
class QSignalList : public QVirtualList
    {
public:
    // Construction/Destruction
    QSignalList( );
    virtual ~QSignalList( );
    virtual void Cleanup( );

    // Serialization Support not required
    
    // Other Operations
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

    WORD offset( void ) const;
    void offset( WORD nOffset );

private:
    // This data member is the offset from the beginning of
    // the quote data for which the signal is valid.  For
    // example, if the signal only starts on the 4th day
    // of data (eg., 4-day moving average), then the offset
    // would be 3 -> 4th data element in.  The offset
    // is 0-based.
    WORD m_nOffset;

    inline virtual void Abstract( ) { }
    };
    
//////////////////////////////////////////////////////////////////////////////
// class QCondCache
//
// QCondCache is used to cache condition datasets.
//////////////////////////////////////////////////////////////////////////////
class QCondCache : public QListObject
    {
public:
    QCondCache( );
    virtual ~QCondCache( );
    virtual void Cleanup( );

    // No serialization support required.

    // Other Operations
    const CHAR *GetName( ) const 
        { 
        return m_sName;
        }

    void SetName( const CHAR *psz )
        {
        m_sName = psz;
        }

    const DATASET &DataSet( ) const
        {
        return m_dataset;
        }

    void SetDataSet( DATASET &dataset );
    BOOL Build( int nRecurseLevel );

    #ifdef DEBUG
    virtual void AssertValid( ) const;
    #endif

private:
    QString m_sName;
    DATASET m_dataset;
    };

//////////////////////////////////////////////////////////////////////////////
// class QCondCacheList
//
// QCondCacheList is the list of condition caches.
//////////////////////////////////////////////////////////////////////////////
class QCondCacheList : public QVirtualList
    {
public:
    // Construction/Destruction
    QCondCacheList( );
    virtual ~QCondCacheList( );
    virtual void Cleanup( );
    
    // Other Operations
    QCondCache *Find( const CHAR *psz ) const;
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;
    #endif

private:
    inline virtual void Abstract( ) { }
    };


//////////////////////////////////////////////////////////////////////////////
// class QCache
//
// QCaches are used by views and other entities to temporarily hold
// calculated indicator template data.
//////////////////////////////////////////////////////////////////////////////
class QCache : public QListObject
    {
public:
    QCache( );
    virtual ~QCache( );
    virtual void Cleanup( );

    // No serialization support required.

    // Other Operations
    const CHAR *GetName( ) const;
    void  SetName( const CHAR *pszName );
    BOOL Build( int nRecurseLevel );
    BOOL GetDataSet( const CHAR *pszName, DATASET &ds, int nRecurseLevel ) const;
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

    // Public members
    DATASET m_dataset;
    DATASET m_datasetHorz;

private:
    const CHAR *funcCopy( CHAR *szToken, const CHAR *pszString ) const;
    void setConstant( DATASET &ds, WORD nPoints, WORD nOffset, NUM num ) const;
    void setYear( DATASET &ds ) const;
    void setMonth( DATASET &ds ) const;
    void setDay( DATASET &ds ) const;
    void setWeekDay( DATASET &ds ) const;
    void setYearDay( DATASET &ds ) const;

    QString m_sName;
    };

//////////////////////////////////////////////////////////////////////////////
// class QCacheList
//
// QCacheList is simply a list of QCaches.
//////////////////////////////////////////////////////////////////////////////
class QCacheList : public QVirtualList
    {
public:
    QCacheList( );
    virtual ~QCacheList( );
    virtual void Cleanup( );

    // No serialization support required.
    
    // Other Operations
    QCache *Find( const CHAR *pszName ) const;

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QRuleCache
//
// QRuleCache cache rule DATASET values.
//////////////////////////////////////////////////////////////////////////////
class QRuleCache : public QListObject
    {
public:
    // Construction/Destruction
    QRuleCache( );
    virtual ~QRuleCache( );
    virtual void Cleanup( );

    // Other Operations
    const CHAR *GetName( ) const 
        { 
        return m_sName;
        }

    void SetName( const CHAR *psz )
        {
        m_sName = psz;
        }

    const DATASET &DataSet( ) const
        {
        return m_dataset;
        }

    void SetDataSet( DATASET &dataset );
    BOOL Build( int nRecurseLevel );

    #ifdef DEBUG
    virtual void AssertValid( ) const;
    #endif

private:
    QString m_sName;
    DATASET m_dataset;
    };

//////////////////////////////////////////////////////////////////////////////
// class QRuleCacheList
//
// QRuleCache is the list of rule caches.
//////////////////////////////////////////////////////////////////////////////
class QRuleCacheList : public QVirtualList
    {
public:
    // Construction/Destruction
    QRuleCacheList( );
    virtual ~QRuleCacheList( );
    virtual void Cleanup( );
    
    // Other Operations
    QRuleCache *Find( const CHAR *psz ) const;
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QCondition
//
// QCondition implements conditions.
//////////////////////////////////////////////////////////////////////////////
class QCondition : public QListObject
    {
public:
    // Construction/Destruction
    QCondition( );
    virtual ~QCondition( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const CHAR *GetName( ) const;
    void        SetName( const CHAR *psz );
    const CHAR *GetDefinition( ) const;
    void        SetDefinition( const CHAR *psz );
    USHORT      GetIdxClr( ) const;
    void        SetIdxClr( USHORT nIdxClr );
    BOOL        BuildDataSet( LPDATASET lpdataset, int nRecurseLevel );
    void        Export( FILE *pfile ) const;
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    USHORT  m_nIdxClr;
    QString m_sName;
    QString m_sDefinition;
    };

//////////////////////////////////////////////////////////////////////////////
// class QConstant
//
// QConstant implements constants.
//////////////////////////////////////////////////////////////////////////////
class QConstant : public QListObject
    {
public:
    // Construction/Destruction
    QConstant( );
    virtual ~QConstant( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const CHAR *GetName( ) const;
    void        SetName( const CHAR *psz );
    NUM         GetValue( ) const;
    void        SetValue( NUM num );
    void        Export( FILE *pfile ) const;
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    QString m_sName;
    NUM     m_numValue;
    };

//////////////////////////////////////////////////////////////////////////////
// class QRule
//
// QRule is a collectable class which implements trading rules.
//////////////////////////////////////////////////////////////////////////////
class QRule : public QListObject
    {
public:
    // Construction/Destruction
    QRule( );
    virtual ~QRule( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    BOOL        GetNextOpenFlag( ) const;
    void        SetNextOpenFlag( BOOL f );
    const CHAR *GetName( ) const;
    void        SetName( const CHAR *psz );
    const CHAR *GetBuyString( ) const;
    void        SetBuyString( const CHAR *psz );
    const CHAR *GetSellString( ) const;
    void        SetSellString( const CHAR *psz );
    const CHAR *GetFilterString( ) const;
    void        SetFilterString( const CHAR *psz );
    const CHAR *GetFilter2String( ) const;
    void        SetFilter2String( const CHAR *psz );
    BOOL        Evaluate( QSignalList &sglist, BOOL fIgnoreFirst, BOOL fIgnoreIncomplete, int nRecurseLevel );
    void        Export( FILE *pfile ) const;
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif
    
private: 
    BOOL HandleFiltering( QSignalList &sglist, const QTokenList &listFilter, int nRecurseLevel ) const;
    BOOL HandleFiltering2( QSignalList &sglist, const QTokenList &listFilter, int nRecurseLevel ) const;

    QString m_sName;
    QString m_sBuy;
    QString m_sSell;
    QString m_sFilter;
    QString m_sFilter2;
    BOOL    m_fNextOpen;
    };

//////////////////////////////////////////////////////////////////////////////
// QParser
//
// This is the parser class.  It really is just a wrapper for a bunch
// of static parsing routines.
//////////////////////////////////////////////////////////////////////////////
class QParser : public QObject
    {
public:
    enum Rc 
        { 
        rcOk, 
        rcUnAvail, 
        rcFatalError 
        };
    static BOOL Analyze( QTokenList &list, const QString &sTokens, BOOL fFullParser );
    static Rc   program( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel, 
                         BOOL fScalarFunc = FALSE, NUM numEntry = 0, UINT iEntry = 0, NUM numTrail = 0 );
    static Rc   expr( UINT i, QTokenIter &iter, NUM &numValue, int nRecurseLevel,
                      BOOL fScalarFunc = FALSE, NUM numEntry = 0, UINT iEntry = 0, NUM numTrail = 0 );
    static Rc   condition( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel,
                           BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail = 0 );

private:
    static BOOL PostAnalyze( QTokenList &list );
    static BOOL ReadSymbol( QToken *ptk, const CHAR *pszToken, UINT &cb );
    static BOOL ReadValue( QToken *ptk, const CHAR *pszToken, UINT &cb );
    static Rc   rule( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel, 
                      BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail = 0 );
    static Rc   comparison( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel,
                            BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail = 0 );
    static Rc   term( UINT i, QTokenIter &iter, NUM &numValue, int nRecurseLevel,
                      BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail = 0 );
    static Rc   primary( UINT i, QTokenIter &iter, NUM &numValue, int nRecurseLevel, 
                         BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail = 0 );

    static const CHAR m_szTokenDelimiters[ ];
    };

//////////////////////////////////////////////////////////////////////////////
// class QConditionList
//
// QConditionList is the list of conditions.
//////////////////////////////////////////////////////////////////////////////
class QConditionList : public QVirtualList
    {
public:
    // Construction/Destruction
    QConditionList( );
    virtual ~QConditionList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    QCondition *Find( const CHAR *psz ) const;
    void Export( FILE *pfile ) const;
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif
    
private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QConstantList
//
// QConstantList is the list of constants.
//////////////////////////////////////////////////////////////////////////////
class QConstantList : public QVirtualList
    {
public:
    // Construction/Destruction
    QConstantList( );
    virtual ~QConstantList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    QConstant *Find( const CHAR *psz ) const;
    void Export( FILE *pfile ) const;
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif
    
private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QRuleList
//
// QRuleList is the list of rules.
//////////////////////////////////////////////////////////////////////////////
class QRuleList : public QVirtualList
    {
public:
    // Construction/Destruction
    QRuleList( );
    virtual ~QRuleList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    QRule *Find( const CHAR *psz ) const;
    void Export( FILE *pfile ) const;
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QIndicatorName
//
// QIndicatorName is a collectable string-like class used
// to collect indicator names into a dictionary.  This was necessary
// due the limitations of the QObject collection class implementation.
//////////////////////////////////////////////////////////////////////////////
class QIndicatorName : public QListObject
    {
public:
    // Construction/Destruction
    QIndicatorName( );
    QIndicatorName( const CHAR *pszName );
    QIndicatorName( const QIndicatorName &name );
    virtual ~QIndicatorName( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    void Set( const CHAR *psz );
    operator QString( ) const;
    
    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    QString m_sName;
    };

//////////////////////////////////////////////////////////////////////////////
// class QIndicatorDictionary
//
// An indication dictionary is no more than a list of indicator names.
//////////////////////////////////////////////////////////////////////////////
class QIndicatorDictionary : public QVirtualList
    {
public:
    // Construction/Destruction
    QIndicatorDictionary( );
    virtual ~QIndicatorDictionary( );
    virtual void Cleanup( );
    
    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const QIndicatorDictionary &operator=( const QIndicatorDictionary &dict );
    QIndicatorName *QIndicatorDictionary::Find( const CHAR *pszName ) const;

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// class QIndicatorTemplate
//
// This is the base class for indicator templates.  It provides
// support for indicator template names, colors, and graph types.
//////////////////////////////////////////////////////////////////////////////
class QIndicatorTemplate : public QListObject
    {
public:
    enum Rtti
        {
        rtAll,
        rtNormal,
        rtCustom,
        rtExternal
        };

    // Construction/Destruction
    QIndicatorTemplate( );
    virtual ~QIndicatorTemplate( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const     CHAR *GetName( ) const;
    void      SetName( const CHAR *pszName );
    USHORT    GetIdxClr( ) const;
    void      SetIdxClr( USHORT nIdxClr );
    GraphType GetGraphType( ) const;
    void      SetGraphType( GraphType gt );
    
    // Virtual functions
    virtual const CHAR *GetLibrary( ) const = 0;
    virtual BOOL BuildDataSet( LPDATASET lpdataset, LPDATASET lpdatasetHorz, int nRecurseLevel ) = 0;
    virtual Rtti rtti( void ) const = 0;
    virtual const CHAR *VerString( void ) const = 0;
    virtual void Export( FILE *pfile ) const = 0;

    #ifdef DEBUG
    virtual void AssertValid( ) const;
    #endif

private:                                
    QString   m_sName;
    USHORT    m_nIdxClr;
    GraphType m_gt;
    };

//////////////////////////////////////////////////////////////////////////////
// class QIndicatorTemplateNormal
//
// This class manages normal (real) indicator templates.
//////////////////////////////////////////////////////////////////////////////
class QIndicatorTemplateNormal : public QIndicatorTemplate
    {
public:
    // Construction/Destruction
    QIndicatorTemplateNormal( );
    virtual ~QIndicatorTemplateNormal( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    void      SetLibrary( const CHAR *pszLibrary );
    LPOPTIONS GetOptions( );
    void      SetOptions( LPOPTIONS lpopts );

    // Virtual functions
    virtual const CHAR *GetLibrary( ) const;
    virtual BOOL BuildDataSet( LPDATASET lpdataset, LPDATASET lpdatasetHorz, int nRecurseLevel );
    virtual Rtti rtti( void ) const;
    virtual const CHAR *VerString( void ) const;
    virtual void Export( FILE *pfile ) const;

    #ifdef DEBUG
    virtual void AssertValid( ) const;
    #endif

private:
    OPTIONS   m_opts;
    QString   m_sPath;
    };

//////////////////////////////////////////////////////////////////////////////
// class QIndicatorTemplateCustom
//
// This class manages custom-defined indicator templates.
//////////////////////////////////////////////////////////////////////////////
class QIndicatorTemplateCustom: public QIndicatorTemplate
    {
public:
    // Construction/Destruction
    QIndicatorTemplateCustom( );
    virtual ~QIndicatorTemplateCustom( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );
    
    // Other Operations
    const CHAR *GetDefn( ) const;
    void        SetDefn( const CHAR *pszDefn );
    const CHAR *GetHorz( ) const;
    void        SetHorz( const CHAR *pszHorz );

    // Virtual functions
    virtual const CHAR *GetLibrary( ) const;
    virtual BOOL BuildDataSet( LPDATASET lpdataset, LPDATASET lpdatasetHorz, int nRecurseLevel );
    virtual Rtti rtti( void ) const;
    virtual const CHAR *VerString( void ) const;
    virtual void Export( FILE *pfile ) const;

private:
    QString   m_sDefn;
    QString   m_sHorz; // Horizontal lines
    };

// forward declarations
class QExternalIndicatorRecord;
class QExternalIndicatorList;
//////////////////////////////////////////////////////////////////////////////
// class QIndicatorTemplateExternal
//
// This class manages externally-defined indicator templates.
//////////////////////////////////////////////////////////////////////////////
class QIndicatorTemplateExternal : public QIndicatorTemplate
    {
public:
    // Construction/Destruction
    QIndicatorTemplateExternal( );
    virtual ~QIndicatorTemplateExternal( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );
    
    // Other Operations
    const CHAR *GetExternalName( ) const;
    void        SetExternalName( const CHAR *pszDefn );
    const CHAR *GetHorz( ) const;
    void        SetHorz( const CHAR *pszHorz );

    // Virtual functions
    virtual const CHAR *GetLibrary( ) const;
    virtual BOOL BuildDataSet( LPDATASET lpdataset, LPDATASET lpdatasetHorz, int nRecurseLevel );
    virtual Rtti rtti( void ) const;
    virtual const CHAR *VerString( void ) const;
    virtual void Export( FILE *pfile ) const;

private:
    BOOL BuildLocalList( QExternalIndicatorList &localList ) ;
    BOOL PopulateDataSet( const QExternalIndicatorList &localList, LPDATASET lpdataset );
    QExternalIndicatorRecord *FindRecord( const QExternalIndicatorList &localList, const QTime &t );

    QString   m_sExternalName;
    QString   m_sHorz; // Horizontal lines
    };

//////////////////////////////////////////////////////////////////////////////
// class QITList
//
// QITList is a collection of indicator template classes.
//////////////////////////////////////////////////////////////////////////////
class QITList : public QVirtualList
    {
public:
    // Construction/Destruction
    QITList( );
    virtual ~QITList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    QIndicatorTemplate *Find( 
        const CHAR *pszTemplate, 
        QIndicatorTemplate::Rtti rt = QIndicatorTemplate::rtAll ) const;
    void Export( FILE *pfile ) const;

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// QViewLayout
//
// QViewLayout represents a single 'view' (i.e., A, B, C, or D) of
// data.  This includes the real-estate percentage of the view,
// which indicators are stored in the view, and the like.
//////////////////////////////////////////////////////////////////////////////
class QViewLayout : public QObject
    {
public:
    // Construction/Destruction
    QViewLayout( );                                        
    ~QViewLayout( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    const CHAR *GetTitle( ) const;
    void        SetTitle( const CHAR *pszTitle );
    const CHAR *GetRule( ) const;
    void        SetRule( const CHAR *pszRule );
    const CHAR *GetCondition( ) const;
    void        SetCondition( const CHAR *pszCond );
    BOOL        GetHLCV( ) const;
    void        SetHLCV( BOOL fHLCV );
    BOOL        GetHorz( ) const;
    void        SetHorz( BOOL fHorz );
    BOOL        GetRoundScale( ) const;
    void        SetRoundScale( BOOL fRoundScale );
    BOOL        GetLogScale( ) const;
    void        SetLogScale( BOOL fLogScale );
    USHORT      GetGrids( ) const;
    void        SetGrids( USHORT nGrids );
    USHORT      GetPercent( ) const;
    void        SetPercent( USHORT nPercent );

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

    QIndicatorDictionary m_dict;

private:
    QString m_sTitle;
    QString m_sRule;
    QString m_sCond;
    BOOL    m_fHLCV;
    BOOL    m_fHorz;
    BOOL    m_fRoundScale;
    BOOL    m_fLogScale;
    USHORT  m_nGrids;
    USHORT  m_nPercent;
    };

//////////////////////////////////////////////////////////////////////////////
// class QWindowLayout
//
// QWindowLayout contains all data regarding window layouts: the array
// of 4 QViewLayout objects, the DX Increment, the name, etc.
//////////////////////////////////////////////////////////////////////////////
class QWindowLayout : public QListObject
    {
public:
    // Construction/Destruction
    QWindowLayout( );
    virtual ~QWindowLayout( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    const  CHAR *GetName( ) const;
    void   SetName( const CHAR *pszName );
    USHORT GetDxIncr( ) const;
    void   SetDxIncr( USHORT dxincr );
    BOOL   GetAutoOpen( ) const;
    void   SetAutoOpen( BOOL fAutoOpen );

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

    QViewLayout m_rgvl[ _cViewLayouts ];

private:
    QString m_sName;
    USHORT  m_dxincr;
    BOOL    m_fAutoOpen;
    };

class QWLList : public QVirtualList
    {
public:
    // Construction/Destruction
    QWLList( ); 
    virtual ~QWLList( );
    virtual void Cleanup( );

    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    QWindowLayout *Find( const CHAR *pszLayout ) const;

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    inline virtual void Abstract( ) { }
    };
    
//////////////////////////////////////////////////////////////////////////////
// class QTemplate
//
// The template class is the class which represents the entire template:
// window layouts, indicator templates, text file format, etc.
//////////////////////////////////////////////////////////////////////////////
class QTemplate : public QObject
    {
public:
    // Construction/Destruction
    QTemplate( CHAR *pszFileName );
    virtual ~QTemplate( );
    virtual void Cleanup( );
    
    // Serialization Support
    virtual void Store( QArchive &ar );
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    BOOL GetModifiedFlag( ) const;
    void SetModifiedFlag( BOOL fModified );
    const CHAR *GetFileName( ) const;
    BOOL IndicatorInUse( const CHAR *pszIndicator ) const;
    BOOL RuleInUse( const CHAR *pszRule ) const;
    BOOL ConditionInUse( const CHAR *pszCond ) const;

    // Export to Text Operation
    void Export( FILE *pfile ) const;

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

    // public data members
    QGeneralOptions m_genopts;
    QITList         m_itlist;
    QWLList         m_wllist;
    QFormat         m_format;
    QRuleList       m_rulelist;
    QConditionList  m_condlist;
    QConstantList   m_constlist;

    static const QVersion m_v;

private:
    QString         m_sFileName;
    BOOL            m_fModified;
    };

#endif
