//////////////////////////////////////////////////////////////////////////////
// PERFENG.HXX
//
// This file generates performance test results.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _PERFENG_HXX
#define _PERFENG_HXX

//////////////////////////////////////////////////////////////////////////////
// Optimization Request structure
//////////////////////////////////////////////////////////////////////////////
struct OptRequest
    {
    OptRequest( )
        :inUse( FALSE ), numStart( 0 ), numStep( 0 ), n( 0 )
        {
        }
    ~OptRequest( )
        {
        Cleanup( );
        }

    void Cleanup( )
        {
        sConstName.Cleanup( );
        }

    BOOL inUse;
    QString sConstName;
    NUM numStart;
    NUM numStep;
    int n;
    };

//////////////////////////////////////////////////////////////////////////////
// Optimization run constant value
//////////////////////////////////////////////////////////////////////////////
struct OptConstValue
    {
    OptConstValue( )
        :inUse( FALSE ), numConstValue( 0 )
        {
        }

    ~OptConstValue( )
        {
        Cleanup( );
        }

    void Cleanup( )
        {
        sConstName.Cleanup( );
        }

    BOOL inUse;
    QString sConstName;
    NUM numConstValue;
    };

// Maximum number of optimization requests
#define OPT_REQUESTS_MAX 3
#define PERF_RUNS_MAX 256

//////////////////////////////////////////////////////////////////////////////
// Performance Request structure
//////////////////////////////////////////////////////////////////////////////
struct PerformanceRequest
    {
    // Parent Window
    HWND hwndParent;

    // Rule Name
    QString sRuleName;

    // Performance Test Type
    enum PerformanceType
        {
        eLoadedCompany,
        eAllCompanies
        };
    PerformanceType type;

    // Position Size, $
    NUM numPosition;

    // Commission, $
    NUM numComm;

    // File Extension Filter
    QString sFilter;

    // Optimization Requests
    OptRequest optRequests[ OPT_REQUESTS_MAX ];

    // Sort Key
    enum SortKey
        {
        None,
        NetProfit,
        ROA,
        ProfitFactor,
        AvgTrade,
        PctProfitable,
        RatioAvgWinAvgLoss,
        AccountSize,
        NTrades
        };
    SortKey sortKey;
    
    PerformanceRequest( )
        : hwndParent( 0 ),
          type( eLoadedCompany ),
          numPosition( 0.00 ),
          numComm( 0.00 ),
          sortKey( None )
        {
        }
    
    ~PerformanceRequest( )
        {
        Cleanup( );
        }
    
    void Cleanup( )
        {
        sRuleName.Cleanup( );
        sFilter.Cleanup( );

        for ( int i = 0; i < OPT_REQUESTS_MAX; i++ )
            optRequests[ i ].Cleanup( );
        }

    BOOL optimization( ) const
        {
        for ( int i = 0; i < OPT_REQUESTS_MAX; i++ )
            {
            if ( optRequests[ i ].inUse )
                return TRUE;
            }

        return FALSE;
        }

    long performanceRuns( ) const
        {
        long perfRuns = 1l;
        for ( int i = 0; i < OPT_REQUESTS_MAX; i++ )
            {
            if ( optRequests[ i ].inUse )
                perfRuns *= optRequests[ i ].n;
            }
        return perfRuns;
        }
    };

//////////////////////////////////////////////////////////////////////////////
// Performance Results 'class' (structure)
//////////////////////////////////////////////////////////////////////////////
class Results : public QListObject
    {
public:
    Results( )
        :numTotalNetProfit( 0 ),
         numOpenPositionPL( 0 ),
         numGrossProfit( 0 ),
         numGrossLoss( 0 ),
         nTrades( 0 ),
         numPercentProfitable( 0 ),
         nWinningTrades( 0 ),
         nLosingTrades( 0 ),
         numLargestWinningTrade( 0 ),
         numLargestLosingTrade( 0 ),
         numAverageWinningTrade( 0 ),
         numAverageLosingTrade( 0 ),
         numRatioAvgWinAvgLoss( 0 ),
         numAverageTrade( 0 ),
         nMaxConsecutiveWinners( 0 ),
         nMaxConsecutiveLosers( 0 ),
         numAverageBarsInWinners( 0 ),
         numAverageBarsInLosers( 0 ),
         numMaximumDrawdown( 0 ),
         numProfitFactor( 0 ),
         nMaxContractsHeld( 0 ),
         numAccountSizeRequired( 0 ),
         numReturnOnAccount( 0 )
        {
        }

    ~Results( )
        {
        Cleanup( );
        }

    void Cleanup( )
        {
        for ( int i = 0; i < OPT_REQUESTS_MAX; i++ )
            optConstValues[ i ].Cleanup( );
        }
    
    static int compare( const PerformanceRequest &req, const Results &lhs, const Results &rhs );
    static int NANCompare( NUM lhs, NUM rhs );

    OptConstValue optConstValues[ OPT_REQUESTS_MAX ];

    NUM  numTotalNetProfit;
    NUM  numOpenPositionPL;
    NUM  numGrossProfit;
    NUM  numGrossLoss;
    WORD nTrades;
    NUM  numPercentProfitable;
    WORD nWinningTrades;
    WORD nLosingTrades;
    NUM  numLargestWinningTrade;
    NUM  numLargestLosingTrade;
    NUM  numAverageWinningTrade;
    NUM  numAverageLosingTrade;
    NUM  numRatioAvgWinAvgLoss;
    NUM  numAverageTrade;
    WORD nMaxConsecutiveWinners;
    WORD nMaxConsecutiveLosers;
    NUM  numAverageBarsInWinners;
    NUM  numAverageBarsInLosers;
    NUM  numMaximumDrawdown;
    NUM  numProfitFactor;
    WORD nMaxContractsHeld;
    NUM  numAccountSizeRequired;
    NUM  numReturnOnAccount;
    };
    
//////////////////////////////////////////////////////////////////////////////
// class ResultsList
//
// This class defines the results list collection
//////////////////////////////////////////////////////////////////////////////
class ResultsList : public QVirtualList
    {
public:
    ResultsList( )
        {
        }

    ~ResultsList( )
        {
        Cleanup( );
        }

    virtual void Cleanup( );

    void InsertResults( const PerformanceRequest &req, Results *pres );
    
#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    virtual void Abstract( )
        {
        }
    };

//////////////////////////////////////////////////////////////////////////////
// class QTrade
//
// This class defines a complete trade: a company name, a buying date&price,
// and a selling date and price.
//////////////////////////////////////////////////////////////////////////////
class QTrade : public QListObject
    {
public:
    QTrade( )
        :m_sg( QSignal::sgBuy ), 
         m_numPrice( 0 ), 
         m_numLastClose( 0 ), 
         m_bars( 0 ), 
         m_numProfit( 0 ), 
         m_numDrawdown( 0 ),
         m_numCashBalance( 0 ),
         m_numPositionSize( 0 )
        {
        }

    ~QTrade( )
        {
        Cleanup( );
        }

    virtual void Cleanup( )
        {
        m_sName.Cleanup( );
        m_dt.Cleanup( );
        }

    void security( const CHAR *pszName )
        {
        m_sName = pszName;
        }

    const CHAR *security( ) const
        {
        return m_sName;
        }

    void date( const QTime &date )
        {
        m_dt = date;
        }

    const QTime &date( ) const
        {
        return m_dt;
        }

    void signalType( QSignal::SignalType sg )
        {
        m_sg = sg;
        }

    QSignal::SignalType signalType( ) const
        {
        return m_sg;
        }

    void price( NUM numPrice )
        {
        m_numPrice = numPrice;
        }
    
    NUM price( ) const
        {
        return m_numPrice;
        }

    void lastClose( NUM numLastClose )
        {
        m_numLastClose = numLastClose;
        }

    NUM lastClose( ) const
        {
        return m_numLastClose;
        }

    void drawdown( NUM dd )
        {
        m_numDrawdown = dd;
        }

    NUM drawdown( ) const
        {
        return m_numDrawdown;
        }

    NUM cashBalance( ) const
        {
        return m_numCashBalance;
        }
       
    void cashBalance( NUM num )
        {
        m_numCashBalance = num;
        }

    NUM positionSize( ) const
        {
        return m_numPositionSize;
        }

    void positionSize( NUM num )
        {
        m_numPositionSize = num;
        }

    void barsSinceEntry( ULONG bars );
    ULONG barsSinceEntry( ) const;
    void profit( NUM numProfit );
    NUM profit( ) const;

#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    QString m_sName;          // Company name of the trade; can be empty if same company
    QTime   m_dt;             // Trade date
    QSignal::SignalType m_sg; // Signal type
    NUM     m_numPrice;       // Price (per share)
    NUM     m_numLastClose;   // Last available close (per share)
    ULONG   m_bars;           // Bars since entry
    NUM     m_numProfit;      // Profit (net, per trade)
    NUM     m_numDrawdown;    // Drawdown
    NUM     m_numCashBalance; // Cash balance
    NUM     m_numPositionSize;// Position size
    };

//////////////////////////////////////////////////////////////////////////////
// class QTradeList
//
// This class defines the trade list collection
//////////////////////////////////////////////////////////////////////////////
class QTradeList : public QVirtualList
    {
public:
    QTradeList( )
        :m_nMaxSize( 0 )
        {
        }

    ~QTradeList( )
        {
        Cleanup( );
        }

    virtual void Cleanup( );

    WORD GetMaxSize( ) const
        {
        return m_nMaxSize;
        }

    void InsertTrade( QTrade *ptr );
    QTrade *FindSecurity( const CHAR *pszSecurity ) const;
    QTrade *FindNextSell( QTrade *ptr ) const;

#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    virtual void Abstract( )
        {
        }

    WORD m_nMaxSize;
    };

//////////////////////////////////////////////////////////////////////////////
// Name-Value pair classes
//////////////////////////////////////////////////////////////////////////////
class NVPair : public QListObject
    {
public:
    // Destructor
    ~NVPair( ) 
        {
        Cleanup( );
        }

    void Cleanup( )
        {
        m_sName.Cleanup( );
        m_sValue.Cleanup( );
        }

    // Factory Methods
    static NVPair *create( const CHAR *pszName, const CHAR *pszValue, BOOL fMetadata = FALSE, 
        BOOL fSelected = FALSE );
    static NVPair *createEmpty( );
    static NVPair *createSeparator( );

    const CHAR *name( ) const { return m_sName; }
    const CHAR *value( ) const { return m_sValue; }
    
    BOOL selected( ) const { return m_fSelected; }
    BOOL metadata( ) const { return m_fMetadata; }
    BOOL separator( ) const { return m_fSeparator; }

#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    // Private Constructors
    NVPair( );
    NVPair( const CHAR *pszName, const CHAR *pszValue, BOOL fMetadata, BOOL fSelected );

    QString m_sName;
    QString m_sValue;
    BOOL m_fSeparator;
    BOOL m_fMetadata;
    BOOL m_fSelected;
    };

class NVPairList : public QVirtualList
    {
public:
    NVPairList( ) { }
    ~NVPairList( )
        {
        Cleanup( );
        }
    virtual void Cleanup( );

#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    virtual void Abstract( ) { }
    };

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous
//////////////////////////////////////////////////////////////////////////////
extern void PerformanceTester( const PerformanceRequest &req );

#endif

