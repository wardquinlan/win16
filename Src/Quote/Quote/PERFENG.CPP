//////////////////////////////////////////////////////////////////////////////
// PERFENG.CPP
//
// This file generates performance test results.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <dos.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <statpak.h>
#include "resource.hxx"
#include "types.hxx"
#include "template.hxx"
#include "perfeng.hxx"
#include "dlg.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "tools.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

// Not-A-Number
#define NUM_NAN ( -FLT_MAX )

//////////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////////
static void PerformanceRun( const PerformanceRequest &req, ResultsList &resList );
static void BuildTrades( const PerformanceRequest &req, QTradeList *ptrlist );
static void AppendTradeCompany( const PerformanceRequest &req, QTradeList *ptrlist );
static void AppendAllTradeCompanies( const PerformanceRequest &req, QTradeList *ptrlist );
static void CreateTradeList( QSignalList &sglist, const PerformanceRequest &req, QTradeList *ptrlist );
static void PostProcessTrades( const PerformanceRequest &req, QTradeList *ptrlist, QTradeList *ptrlistOpenPosition );
static void TabulateResults( const PerformanceRequest &req, QTradeList *ptrlist, QTradeList *ptrlistOpenPosition,  
    Results &res );
static void PopulateSummaries( const PerformanceRequest &req, const ResultsList &resList, NVPairList *psummary );
static void PopulateSummary( const PerformanceRequest &req, const Results &res, NVPairList *psummary, BOOL addSeparator );
static ULONG CalculateBars( const QTime &dtBuy, const QTime &dtSell );
static void Optimizer( const PerformanceRequest &req, ResultsList &resList, int nConstant );

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous
//////////////////////////////////////////////////////////////////////////////
extern QTradeList *_ptrlist;
extern NVPairList *_psummary;

//////////////////////////////////////////////////////////////////////////////
// QTrade::barsSinceEntry( )
//
// Sets bars since entry
//////////////////////////////////////////////////////////////////////////////
void QTrade::barsSinceEntry( ULONG bars )
    {
    ASSERT( m_sg == QSignal::sgSell );
    m_bars = bars;
    }

//////////////////////////////////////////////////////////////////////////////
// QTrade::barsSinceEntry( )
//
// Gets bars since entry
//////////////////////////////////////////////////////////////////////////////
ULONG QTrade::barsSinceEntry( ) const
    {
    ASSERT( m_sg == QSignal::sgSell );
    return m_bars;
    }

//////////////////////////////////////////////////////////////////////////////
// QTrade::profit( )
//
// Sets net profit
//////////////////////////////////////////////////////////////////////////////
void QTrade::profit( NUM numProfit )
    {
    ASSERT( m_sg == QSignal::sgSell );
    m_numProfit = numProfit;
    }

//////////////////////////////////////////////////////////////////////////////
// QTrade::profit( )
//
// Gets net profit
//////////////////////////////////////////////////////////////////////////////
NUM QTrade::profit( ) const
    {
    ASSERT( m_sg == QSignal::sgSell );
    return m_numProfit;
    }

//////////////////////////////////////////////////////////////////////////////
// QTradeList::Cleanup( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void QTradeList::Cleanup( )
    {
    QTrade *ptr;

    while ( ptr = (QTrade *) RemoveHead( ) )
        delete ptr;
    }

//////////////////////////////////////////////////////////////////////////////
// QTradeList::FindNextSell( )
//
// Finds the next sell signal given a trade
//////////////////////////////////////////////////////////////////////////////
QTrade *QTradeList::FindNextSell( QTrade *ptr ) const
    {
    QTrade *ptrCurr;
    ASSERT( ptr->signalType( ) == QSignal::sgBuy );

    for ( ptrCurr = (QTrade *) GetNext( ptr );
          ptrCurr && ( lstrcmp( ptrCurr->security( ), ptr->security( ) ) != 0 );
          ptrCurr = (QTrade *) GetNext( ptrCurr ) )
        {
        }

#ifdef DEBUG
    if ( ptrCurr )
        {
        ASSERT( ptrCurr->signalType( ) == QSignal::sgSell );
        }
#endif

    return ptrCurr;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QTradeList::AssertValid( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void QTradeList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QTrade *ptr = (QTrade *) GetHead( ); ptr; ptr = (QTrade *) GetNext( ptr ) )
        ptr->AssertValid( );
    }

//////////////////////////////////////////////////////////////////////////////
// QTrade::AssertValid( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void QTrade::AssertValid( ) const
    {
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// PerformanceTester( )
//
// This is the mainline into the performance tester module.
//////////////////////////////////////////////////////////////////////////////
extern void PerformanceTester( const PerformanceRequest &req )
    {
    HCURSOR hCursor = NULL;
    ResultsList resList;
    
    TRY
        {
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        DestroyAllChildren( );
        _psummary->Cleanup( );
        Optimizer( req, resList, 0 );
        PopulateSummaries( req, resList, _psummary );

        // Clean out trade list window if more than 1 run
        if ( req.performanceRuns( ) > 1l )
            _ptrlist->Cleanup( );
        
        OpenPerfSumWindow( TRUE );
        UpdateMenu( );
        
        // cache may be dirty with modified constants
        if ( req.optimization( ) )
            gcache.Cleanup( );
        }
    CATCH_ALL
        {
        _psummary->Cleanup( );
        if ( hCursor )
            SetCursor( hCursor );
        gcache.Cleanup( );
        resList.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    
    if ( hCursor )
        SetCursor( hCursor );
    }

//////////////////////////////////////////////////////////////////////////////
// Optimizer( )
//
// Optimizer code
//////////////////////////////////////////////////////////////////////////////
static void Optimizer( const PerformanceRequest &req, ResultsList &resList, int nConstant )
    {
    if ( nConstant < OPT_REQUESTS_MAX && req.optRequests[ nConstant ].inUse )
        {
        // Get the constant
        QConstant *pconst = _ptpl->m_constlist.Find( req.optRequests[ nConstant ].sConstName );
        ASSERT( pconst != NULL );

        // Save its value away
        NUM numSav = pconst->GetValue( );
            
        // Perform the iteration
        TRY
            {
            int n        = req.optRequests[ nConstant ].n;
            NUM numCurr  = req.optRequests[ nConstant ].numStart;
            while ( n-- )
                {
                pconst->SetValue( numCurr );
                Optimizer( req, resList, nConstant + 1 );
                numCurr += req.optRequests[ nConstant ].numStep;
                }
            }
        CATCH_ALL
            {
            pconst->SetValue( numSav );
            THROW_LAST( );
            }
        END_CATCH
            
        // Restore the constant's original value
        pconst->SetValue( numSav );
        }
    else
        {
        if ( req.optimization( ) )
            gcache.Cleanup( );
        PerformanceRun( req, resList );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// PerformanceRun( )
//
// This executes a single performance run.
//////////////////////////////////////////////////////////////////////////////
static void PerformanceRun( const PerformanceRequest &req, ResultsList &resList )
    {
    QTradeList openPositionList;
    Results *pres = NULL;
    BOOL inserted = FALSE;
    
    TRY
        {
        _ptrlist->Cleanup( );
        BuildTrades( req, _ptrlist );
        PostProcessTrades( req, _ptrlist, &openPositionList );

        pres = new Results;
        if ( !pres )
            THROW_EXCEPTION( exMemory );
        TabulateResults( req, _ptrlist, &openPositionList, *pres );
        resList.InsertResults( req, pres );
        inserted = TRUE;
#ifdef DEBUG        
        _ptrlist->AssertValid( );
        _psummary->AssertValid( );
        openPositionList.AssertValid( );
#endif
        }
    CATCH_ALL
        {
        _ptrlist->Cleanup( );
        openPositionList.Cleanup( );
        if ( !inserted )
            delete pres;
        THROW_LAST( );
        }
    END_CATCH

    openPositionList.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// BuildTrades
//
// This function builds a trade list
//////////////////////////////////////////////////////////////////////////////
static void BuildTrades( const PerformanceRequest &req, QTradeList *ptrlist )
    {
    TRY
        {
        if ( req.type == PerformanceRequest::eLoadedCompany )
            ::AppendTradeCompany( req, ptrlist );
        else
            ::AppendAllTradeCompanies( req, ptrlist );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// AppendAllTradeCompanies( )
//
// This function will append all companies to the trade and open position lists
//////////////////////////////////////////////////////////////////////////////
static void AppendAllTradeCompanies( const PerformanceRequest &req, QTradeList *ptrlist )
    {
    struct find_t fileinfo;
    int    rc;
    HCURSOR hCursor = NULL;

    TRY
        {
        SaveQuery( req.hwndParent );
        DestroyAllChildren( );
        _pdoc->Cleanup( );
        UpdateName( );
        UpdateMenu( );
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        rc = _dos_findfirst( req.sFilter, _A_RDONLY, &fileinfo );
        while ( rc == 0 )
            {
            TRY
                {
                SilentOpen( fileinfo.name );
                ::AppendTradeCompany( req, ptrlist );
                }
            CATCH( exMemory )
                {
                THROW_LAST( );
                }
            CATCH_ALL
                {
                // other problems, ignore.
                }
            END_CATCH
            rc = _dos_findnext( &fileinfo );
            }
        }
    CATCH_ALL
        {
        DestroyAllChildren( );
        _pdoc->Cleanup( );
        UpdateName( );
        UpdateMenu( );
        if ( hCursor )
            SetCursor( hCursor );
        THROW_LAST( );
        }
    END_CATCH
    
    DestroyAllChildren( );
    _pdoc->Cleanup( );
    UpdateName( );
    UpdateMenu( );
    if ( hCursor )
        SetCursor( hCursor );
    }

//////////////////////////////////////////////////////////////////////////////
// AppendTradeCompany( )
//
// This function will append a company to the trade list and open position list
//////////////////////////////////////////////////////////////////////////////
static void AppendTradeCompany( const PerformanceRequest &req, QTradeList *ptrlist )
    {
    QRule *prule;
    QSignalList sglist;

    TRY
        {
        prule = _ptpl->m_rulelist.Find( req.sRuleName );
        ASSERT( prule != NULL );

        if ( prule->Evaluate( sglist, FALSE, FALSE, 0 ) )
            ::CreateTradeList( sglist, req, ptrlist );
        }
    CATCH_ALL
        {
        sglist.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// CreateTradeList
//
// This routine will build a trade list given a signal list.
//////////////////////////////////////////////////////////////////////////////
static void CreateTradeList( QSignalList &sglist, const PerformanceRequest &req, QTradeList *ptrlist )
    {
    QTrade *ptr = NULL;
    QTime dtBuy;
    QTime dtSell;
    
    TRY
        {
        for ( QSignal *psg = (QSignal *) sglist.GetHead( ); 
              psg && !psg->GetPendingState( ); 
              psg = (QSignal *) sglist.GetNext( psg ) )
            {
            ptr = new QTrade;
            if ( !ptr )
                THROW_EXCEPTION( exMemory );
            ptr->security( _pdoc->GetFileName( ) );
            ptr->date( psg->GetDate( ) );
            ptr->price( psg->GetPrice( ) );
            ASSERT( _pdoc->GetLast( ) != NULL );
            ptr->lastClose( _pdoc->GetLast( )->GetCl( ) );
            ptr->signalType( psg->GetSg( ) );
            
            // Calculate bars here, as 
            // correct security is already loaded
            if ( psg->GetSg( ) == QSignal::sgBuy )
                {
                dtBuy = psg->GetDate( );
                }
            else
                {
                dtSell = psg->GetDate( );
                ptr->barsSinceEntry( CalculateBars( dtBuy, dtSell ) );
                }
            
            ptrlist->InsertTrade( ptr );
            }
        }
    CATCH_ALL
        {
        dtBuy.Cleanup( );
        dtSell.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// CalculateBars( )
//
// Calculates bars since entry
//////////////////////////////////////////////////////////////////////////////
static ULONG CalculateBars( const QTime &dtBuy, const QTime &dtSell )
    {
    // First loop: find the buy signal
    const QQt *pqt = (const QQt *) _pdoc->GetFirst( );
    while ( pqt && pqt->GetDate( ) < dtBuy )
        pqt = (const QQt *) _pdoc->GetNext( pqt );

    // Must be found because dtBuy was generated from the signal list
    ASSERT( pqt != NULL );
    ASSERT( pqt->GetDate( ) == dtBuy );

    ULONG nCount = 0;
    while ( pqt && pqt->GetDate( ) < dtSell )
        {
        pqt = (const QQt *) _pdoc->GetNext( pqt );
        nCount++;
        }

    ASSERT( pqt != NULL );
    ASSERT( pqt->GetDate( ) == dtSell );

    return nCount;
    }

//////////////////////////////////////////////////////////////////////////////
// PostProcessTrades( )
//
// Finishes up creating the trades, also populates the open position table
//////////////////////////////////////////////////////////////////////////////
static void PostProcessTrades( const PerformanceRequest &req, QTradeList *ptrlist, QTradeList *ptrlistOpenPosition )
    {
    TRY
        {
        NUM drawdown = (NUM) 0.0;
        for ( QTrade *ptr = (QTrade *) ptrlist->GetHead( );
              ptr; 
              ptr = (QTrade *) ptrlist->GetNext( ptr ) )
            {
            // Insert a BUY --or-- Remove a SELL
            // from the Open Position Table
            if ( ptr->signalType( ) == QSignal::sgBuy )
                {
                // calculate the drawdown on the purchase
                drawdown -= ( req.numPosition + req.numComm );
                ptr->drawdown( drawdown );

                // place a copy of the trade into the open position table
                ASSERT( ptrlistOpenPosition->FindSecurity( ptr->security( ) ) == NULL );
                QTrade *ptrOpen = new QTrade;
                if ( !ptrOpen )
                    THROW_EXCEPTION( exMemory );
                ptrOpen->security( ptr->security( ) );
                ptrOpen->date( ptr->date( ) );
                ptrOpen->price( ptr->price( ) );
                ptrOpen->lastClose( ptr->lastClose( ) );
                ptrOpen->signalType( ptr->signalType( ) );
                ptrlistOpenPosition->InsertTrade( ptrOpen );
                }
            else
                {
                QTrade *ptrOpen = ptrlistOpenPosition->FindSecurity( ptr->security( ) );
                ASSERT( ptrOpen != NULL );

                // calculate the profit
                ASSERT( ptrOpen->price( ) != (NUM) 0 );
                NUM numRatio  = ptr->price( ) / ptrOpen->price( );
                NUM numGrossProfit = req.numPosition * ( numRatio - (NUM) 1.0 );
                NUM numNetProfit = numGrossProfit - (NUM) 2.0 * req.numComm;
                ptr->profit( numNetProfit );

                // calculate the drawdown on the sale
                drawdown += ( req.numPosition + numGrossProfit - (NUM) req.numComm );
                ptr->drawdown( drawdown );

                // remove trade from open position table
                ptrlistOpenPosition->Remove( ptrOpen );
                delete ptrOpen;
                }
            }
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// TabulateResults( )
//
// Fills in the Results structure with the performance results
//////////////////////////////////////////////////////////////////////////////
void TabulateResults( const PerformanceRequest &req, QTradeList *ptrlist, QTradeList *ptrlistOpenPosition,  
    Results &res )
    {
    TRY
        {
        WORD  nConsecWinners = 0;
        WORD  nConsecLosers = 0;
        ULONG nTotalWinnerBars = 0;
        ULONG nTotalLoserBars = 0;
        
        for ( int i = 0; i < OPT_REQUESTS_MAX; i++ )
            {
            if ( req.optRequests[ i ].inUse )
                {
                QConstant *pconst = _ptpl->m_constlist.Find( req.optRequests[ i ].sConstName );
                ASSERT( pconst != NULL );

                res.optConstValues[ i ].inUse = TRUE;
                res.optConstValues[ i ].sConstName = pconst->GetName( );
                res.optConstValues[ i ].numConstValue = pconst->GetValue( );
                }
            }

        for ( QTrade *ptr = (QTrade *) ptrlist->GetHead( );
              ptr; 
              ptr = (QTrade *) ptrlist->GetNext( ptr ) )
            {
            if ( ptr->drawdown( ) < res.numMaximumDrawdown )
                res.numMaximumDrawdown = ptr->drawdown( );

            // Only interested in sells for the following...
            if ( ptr->signalType( ) == QSignal::sgSell )
                {
                if ( ptr->profit( ) >= (NUM) 0 )
                    {
                    if ( ++nConsecWinners > res.nMaxConsecutiveWinners )
                        res.nMaxConsecutiveWinners = nConsecWinners;
                    nConsecLosers = 0;
                    res.numGrossProfit += ptr->profit( );
                    res.nWinningTrades++;
                    if ( ptr->profit( ) > res.numLargestWinningTrade )
                        res.numLargestWinningTrade = ptr->profit( );
                    nTotalWinnerBars += ptr->barsSinceEntry( );
                    }
                else
                    {
                    if ( ++nConsecLosers > res.nMaxConsecutiveLosers )
                        res.nMaxConsecutiveLosers = nConsecLosers;
                    nConsecWinners = 0;
                    res.numGrossLoss += ptr->profit( );
                    res.nLosingTrades++;
                    if ( ptr->profit( ) < res.numLargestLosingTrade )
                        res.numLargestLosingTrade = ptr->profit( );
                    nTotalLoserBars += ptr->barsSinceEntry( );
                    }
                }
            }
        
        if ( res.nWinningTrades == 0 )
            res.numLargestWinningTrade = NUM_NAN;
            
        if ( res.nLosingTrades == 0 )
            res.numLargestLosingTrade = NUM_NAN;
            
        res.numTotalNetProfit = res.numGrossProfit + res.numGrossLoss;
        
        if ( res.numGrossLoss == (NUM) 0 )
            res.numProfitFactor = NUM_NAN;
        else
            res.numProfitFactor = -res.numGrossProfit / res.numGrossLoss;

        res.nTrades = res.nWinningTrades + res.nLosingTrades;

        if ( res.nTrades == 0 )
            res.numPercentProfitable = NUM_NAN;
        else
            res.numPercentProfitable = (NUM) 100.0 * (NUM) res.nWinningTrades / (NUM) res.nTrades;
        
        if ( res.nWinningTrades == 0 )
            res.numAverageWinningTrade = NUM_NAN;
        else
            res.numAverageWinningTrade = res.numGrossProfit / (NUM) res.nWinningTrades;

        if ( res.nLosingTrades == 0 )
            res.numAverageLosingTrade = NUM_NAN;
        else
            res.numAverageLosingTrade = res.numGrossLoss / (NUM) res.nLosingTrades;

        if ( res.nTrades == 0 )
            res.numAverageTrade = NUM_NAN;
        else
            res.numAverageTrade = res.numTotalNetProfit / (NUM) res.nTrades;

        if ( res.numAverageWinningTrade == NUM_NAN ||
             res.numAverageLosingTrade == NUM_NAN ||
             res.numAverageLosingTrade == (NUM) 0 )
            res.numRatioAvgWinAvgLoss = NUM_NAN;
        else
            res.numRatioAvgWinAvgLoss = -res.numAverageWinningTrade / res.numAverageLosingTrade;

        res.numAccountSizeRequired = -res.numMaximumDrawdown;
        
        if ( res.nWinningTrades == 0 )
            res.numAverageBarsInWinners = NUM_NAN;
        else
            res.numAverageBarsInWinners = (NUM) nTotalWinnerBars / (NUM) res.nWinningTrades;

        if ( res.nLosingTrades == 0 )
            res.numAverageBarsInLosers = NUM_NAN;
        else
            res.numAverageBarsInLosers = (NUM) nTotalLoserBars / ( NUM) res.nLosingTrades;

        res.nMaxContractsHeld = ptrlistOpenPosition->GetMaxSize( );
        
        if ( res.numAccountSizeRequired == (NUM) 0 )
            res.numReturnOnAccount = NUM_NAN;
        else
            res.numReturnOnAccount = (NUM) 100.0 * res.numTotalNetProfit / res.numAccountSizeRequired;

        for ( ptr = (QTrade *) ptrlistOpenPosition->GetHead( );
              ptr; 
              ptr = (QTrade *) ptrlistOpenPosition->GetNext( ptr ) )
            {
            ASSERT( ptr->price( ) != (NUM) 0 );
            NUM numRatio  = ptr->lastClose( ) / ptr->price( );
            NUM numGrossProfit = req.numPosition * ( numRatio - (NUM) 1.0 );
            // include only 'buy' commission on an open position
            NUM numNetProfit = numGrossProfit - req.numComm;
            res.numOpenPositionPL += numNetProfit;
            }
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// PopulateSummaries( )
//
// Populates all results in the summary window
//////////////////////////////////////////////////////////////////////////////
void PopulateSummaries( const PerformanceRequest &req, const ResultsList &resList, NVPairList *psummary )
    {
    for ( Results *pres = (Results *) resList.GetHead( );
          pres;
          pres = (Results *) resList.GetNext( pres ) )
        {
        PopulateSummary( req, *pres, psummary, ( resList.GetNext( pres ) != NULL ) );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// PopulateSummary( )
//
// Populates results in the summary window
//////////////////////////////////////////////////////////////////////////////
void PopulateSummary( const PerformanceRequest &req, const Results &res, NVPairList *psummary, BOOL addSeparator )
    {
    NVPair *pr = NULL;
    CHAR sz[ _cbBufferMax + 1 ];
    CHAR sz2[ _cbBufferMax + 1 ];

    TRY
        {
        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, "%13s", (const CHAR *) req.sRuleName );
        pr = NVPair::create( " Rule name", sz, TRUE );
        psummary->AddTail( pr );
        
        for ( int i = 0; i < OPT_REQUESTS_MAX; i++ )
            {
            if ( res.optConstValues[ i ].inUse )
                {
                lmemset( sz, 0, sizeof( sz ) );
                sprintf( sz, " %s", (const CHAR *) res.optConstValues[ i ].sConstName );
                lmemset( sz2, 0, sizeof( sz2 ) );
                sprintf( sz2, "%13f", res.optConstValues[ i ].numConstValue );
                pr = NVPair::create( sz, sz2, TRUE );
                psummary->AddTail( pr );
                }
            }
        
        pr = NVPair::createEmpty( );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, " $ %10.2f", req.numPosition );
        pr = NVPair::create( " Position size", sz, TRUE );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, " $ %10.2f", req.numComm );
        pr = NVPair::create( " Commission size", sz, TRUE );
        psummary->AddTail( pr );
        
        pr = NVPair::createEmpty( );
        psummary->AddTail( pr );
        
        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numTotalNetProfit != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numTotalNetProfit );
        pr = NVPair::create( " Total net profit", sz, FALSE, 
            ( req.sortKey == PerformanceRequest::NetProfit ) );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numOpenPositionPL != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numOpenPositionPL );
        pr = NVPair::create( " Open position P/L", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numGrossProfit != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numGrossProfit );
        pr = NVPair::create( " Gross profit", sz );
        psummary->AddTail( pr );
        
        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numGrossLoss != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numGrossLoss );
        pr = NVPair::create( " Gross loss", sz );
        psummary->AddTail( pr );

        pr = NVPair::createEmpty( );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, "   %10u", res.nTrades );
        pr = NVPair::create( " Total # of trades", sz, FALSE,
            ( req.sortKey == PerformanceRequest::NTrades ) );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numPercentProfitable != NUM_NAN )
            sprintf( sz, "   %10.f%%", res.numPercentProfitable );
        pr = NVPair::create( " Percent profitable", sz, FALSE, 
            ( req.sortKey == PerformanceRequest::PctProfitable ) );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, "   %10u", res.nWinningTrades );
        pr = NVPair::create( " Number winning trades", sz );
        psummary->AddTail( pr );
        
        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, "   %10u", res.nLosingTrades );
        pr = NVPair::create( " Number losing trades", sz );
        psummary->AddTail( pr );
        
        pr = NVPair::createEmpty( );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numLargestWinningTrade != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numLargestWinningTrade );
        pr = NVPair::create( " Largest winning trade", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numLargestLosingTrade != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numLargestLosingTrade );
        pr = NVPair::create( " Largest losing trade", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numAverageWinningTrade != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numAverageWinningTrade );
        pr = NVPair::create( " Average winning trade", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numAverageLosingTrade != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numAverageLosingTrade );
        pr = NVPair::create( " Average losing trade", sz );
        psummary->AddTail( pr );
                                                     
        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numRatioAvgWinAvgLoss != NUM_NAN )
            sprintf( sz, "   %10.2f", res.numRatioAvgWinAvgLoss );
        pr = NVPair::create( " Ratio avg win/avg loss", sz, FALSE, 
            ( req.sortKey == PerformanceRequest::RatioAvgWinAvgLoss ) );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numAverageTrade != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numAverageTrade );
        pr = NVPair::create( " Avg trade(win + loss)", sz, FALSE, 
            ( req.sortKey == PerformanceRequest::AvgTrade ) );
        psummary->AddTail( pr );

        pr = NVPair::createEmpty( );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, "   %10u", res.nMaxConsecutiveWinners );
        pr = NVPair::create( " Max consec. winners", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, "   %10u", res.nMaxConsecutiveLosers );
        pr = NVPair::create( " Max consec. losers", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numAverageBarsInWinners != NUM_NAN )
            sprintf( sz, "   %10.f", res.numAverageBarsInWinners );
        pr = NVPair::create( " Avg # bars in winners", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numAverageBarsInLosers != NUM_NAN )
            sprintf( sz, "   %10.f", res.numAverageBarsInLosers );
        pr = NVPair::create( " Avg # bars in losers", sz );
        psummary->AddTail( pr );

        pr = NVPair::createEmpty( );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numMaximumDrawdown != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numMaximumDrawdown );
        pr = NVPair::create( " Max drawdown", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numProfitFactor != NUM_NAN )
            sprintf( sz, "   %10.2f", res.numProfitFactor );
        pr = NVPair::create( " Profit factor", sz, FALSE, 
            ( req.sortKey == PerformanceRequest::ProfitFactor ) );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        sprintf( sz, "   %10u", res.nMaxContractsHeld );
        pr = NVPair::create( " Max # securities held", sz );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numAccountSizeRequired != NUM_NAN )
            sprintf( sz, " $ %10.2f", res.numAccountSizeRequired );
        pr = NVPair::create( " Account size required", sz, FALSE, 
            ( req.sortKey == PerformanceRequest::AccountSize ) );
        psummary->AddTail( pr );

        lmemset( sz, 0, sizeof( sz ) );
        if ( res.numReturnOnAccount != NUM_NAN )
            sprintf( sz, "   %10.f%%", res.numReturnOnAccount );
        pr = NVPair::create( " Return on account", sz, FALSE, 
            ( req.sortKey == PerformanceRequest::ROA ) );
        psummary->AddTail( pr );

        pr = NVPair::createEmpty( );
        psummary->AddTail( pr );

        if ( addSeparator )
            {
            pr = NVPair::createSeparator( );
            psummary->AddTail( pr );
            }
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QTradeList::FindSecurity( )
//
// Finds first instance of security in a trade list; returns NULL if not found
//////////////////////////////////////////////////////////////////////////////
QTrade *QTradeList::FindSecurity( const CHAR *pszSecurity ) const
    {
    for ( QTrade *ptrCurr = (QTrade *) GetHead( );
          ptrCurr; 
          ptrCurr = (QTrade *) GetNext( ptrCurr ) )
        {
        if ( lstrcmp( ptrCurr->security( ), pszSecurity ) == 0 )
            return ptrCurr;
        }

    return NULL;
    }

//////////////////////////////////////////////////////////////////////////////
// QTradeList::InsertTrade( )
//
// Inserts a trade into self
//////////////////////////////////////////////////////////////////////////////
void QTradeList::InsertTrade( QTrade *ptr )
    {
    for ( QTrade *ptrCurr = (QTrade *) GetHead( );
          ptrCurr; 
          ptrCurr = (QTrade *) GetNext( ptrCurr ) )
        {
        if ( ptr->date( ) < ptrCurr->date( ) )
            {
            InsertBefore( ptrCurr, ptr );
            break;
            }
        }

    if ( ptrCurr == NULL )
        AddTail( ptr );

    if ( GetCount( ) > m_nMaxSize )
        m_nMaxSize = GetCount( );
    }

//////////////////////////////////////////////////////////////////////////////
// NVPair::NVPair( )
//////////////////////////////////////////////////////////////////////////////
NVPair::NVPair( const CHAR *pszName, const CHAR *pszValue, BOOL fMetadata, BOOL fSelected )
    :m_fSeparator( FALSE ), m_fMetadata( fMetadata ), m_fSelected( fSelected )
    {
    TRY
        {
        m_sName = pszName;
        m_sValue = pszValue;
        }
    CATCH_ALL
        {
        m_sName.Cleanup( );
        m_sValue.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// NVPair::create( )
//////////////////////////////////////////////////////////////////////////////
NVPair *NVPair::create( const CHAR *pszName, const CHAR *pszValue, BOOL fMetadata, BOOL fSelected )
    {
    NVPair *pr = new NVPair( pszName, pszValue, fMetadata, fSelected );
    if ( !pr )
        THROW_EXCEPTION( exMemory );

    return pr;
    }

//////////////////////////////////////////////////////////////////////////////
// NVPair::createEmpty( )
//////////////////////////////////////////////////////////////////////////////
NVPair *NVPair::createEmpty( )
    {
    return NVPair::create( "", "" );
    }

//////////////////////////////////////////////////////////////////////////////
// NVPair::createSeparator( )
//////////////////////////////////////////////////////////////////////////////
NVPair *NVPair::createSeparator( )
    {
    NVPair *pr = NVPair::createEmpty( );
    pr->m_fSeparator = TRUE;
    return pr;
    }

//////////////////////////////////////////////////////////////////////////////
// NVPairList::Cleanup( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void NVPairList::Cleanup( )
    {
    NVPair *pr;

    while ( pr = (NVPair *) RemoveHead( ) )
        delete pr;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// NVPair::AssertValid( )
//////////////////////////////////////////////////////////////////////////////
void NVPair::AssertValid( ) const
    {
    }

//////////////////////////////////////////////////////////////////////////////
// NVPairList::AssertValid( )
//////////////////////////////////////////////////////////////////////////////
void NVPairList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( NVPair *pr = (NVPair *) GetHead( ); pr; pr = (NVPair *) GetNext( pr ) )
        pr->AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// ResultsList::Cleanup( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void ResultsList::Cleanup( )
    {
    Results *pres;

    while ( pres = (Results *) RemoveHead( ) )
        delete pres;
    }

//////////////////////////////////////////////////////////////////////////////
// Results::NANCompare( )
//
// Performs comparison, taking into account NAN
//////////////////////////////////////////////////////////////////////////////
int Results::NANCompare( NUM lhs, NUM rhs )
    {
    if ( lhs == NUM_NAN && rhs == NUM_NAN )
        return 0;

    if ( lhs == NUM_NAN )
        return -1;

    if ( rhs == NUM_NAN )
        return +1;

    if ( lhs < rhs )
        return -1;

    if ( lhs > rhs )
        return +1;

    return 0;
    }

//////////////////////////////////////////////////////////////////////////////
// Results::compare( )
//
// Performs a comparison based upon sort key
//////////////////////////////////////////////////////////////////////////////
int Results::compare( const PerformanceRequest &req, const Results &lhs, const Results &rhs )
    {
    switch( req.sortKey )
        {
    case PerformanceRequest::NetProfit:
        return NANCompare( lhs.numTotalNetProfit, rhs.numTotalNetProfit );
        
    case PerformanceRequest::ROA:
        return NANCompare( lhs.numReturnOnAccount, rhs.numReturnOnAccount );
        
    case PerformanceRequest::ProfitFactor:
        return NANCompare( lhs.numProfitFactor, rhs.numProfitFactor );
        
    case PerformanceRequest::AvgTrade:
        return NANCompare( lhs.numAverageTrade, rhs.numAverageTrade );
        
    case PerformanceRequest::PctProfitable:
        return NANCompare( lhs.numPercentProfitable, rhs.numPercentProfitable );
        
    case PerformanceRequest::RatioAvgWinAvgLoss:
        return NANCompare( lhs.numRatioAvgWinAvgLoss, rhs.numRatioAvgWinAvgLoss );
        
    case PerformanceRequest::AccountSize:
        return NANCompare( lhs.numAccountSizeRequired, rhs.numAccountSizeRequired );

    case PerformanceRequest::NTrades:
        if ( lhs.nTrades < rhs.nTrades )
            return -1;
        if ( lhs.nTrades > rhs.nTrades )
            return +1;
        return 0;
        
    default:
        ASSERT( FALSE );
        return 0;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// ResultsList::InsertResults( )
//
// Insert the given results according to the sorting defined in the request
//////////////////////////////////////////////////////////////////////////////
void ResultsList::InsertResults( const PerformanceRequest &req, Results *pres )
    {
    if ( req.sortKey == PerformanceRequest::None )
        {
        AddTail( pres );
        }
    else
        {
        for ( Results *presCurr = (Results *) GetHead( );
              presCurr; 
            presCurr = (Results *) GetNext( presCurr ) )
            {
            if ( Results::compare( req, *pres, *presCurr ) > 0 )
                {
                InsertBefore( presCurr, pres );
                break;
                }
            }

        if ( presCurr == NULL )
            AddTail( pres );
        }
    }
    
#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// ResultsList::AssertValid( )
//////////////////////////////////////////////////////////////////////////////
void ResultsList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    }
#endif
