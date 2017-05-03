//////////////////////////////////////////////////////////////////////////////
// SIMENG.CPP
//
// This file generates simulator test results.
//
// Copyright (c) Ward Quinlan, 2002
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
#include "simeng.hxx"
#include "dlg.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "tools.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous
//////////////////////////////////////////////////////////////////////////////
extern QTradeList *_ptrlistSim;
extern NVPairList *_psummarySim;
static void BuildSimulatorTrades( const SimulatorRequest &req, QTradeList *ptrlist );
static void AppendSingle( const SimulatorRequest &req, QTradeList *ptrlist );
static void AppendAll( const SimulatorRequest &req, QTradeList *ptrlist );
static void CreateSimulatorTradeList( QSignalList &sglist, const SimulatorRequest &req, QTradeList *ptrlist );
static void PostProcessSimulatorTrades( const SimulatorRequest &req, 
    QTradeList *ptrlist, QTradeList *ptrlistOpenPosition );
static NUM positionSize( const SimulatorRequest &req, NUM numCash );
static void PopulateSimulatorSummary( const SimulatorRequest &req, 
    QTradeList *ptrlistSim, 
    QTradeList *ptrlistOpenPosition, 
    NVPairList *psummarySim );

//////////////////////////////////////////////////////////////////////////////
// Simulator( )
//
// Entry routine for the trade simulator
//////////////////////////////////////////////////////////////////////////////
extern void Simulator( const SimulatorRequest &req )
    {
    QTradeList openPositionList;
    HCURSOR hCursor = NULL;

    TRY
        {
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );

        DestroyAllChildren( );
        _psummarySim->Cleanup( );
        _ptrlistSim->Cleanup( );
        ::BuildSimulatorTrades( req, _ptrlistSim );
        ::PostProcessSimulatorTrades( req, _ptrlistSim, &openPositionList );
        ::PopulateSimulatorSummary( req, _ptrlistSim, &openPositionList, _psummarySim );
        ::OpenPerfSumWindow( TRUE, FALSE );
        ::UpdateMenu( );
        }
    CATCH_ALL
        {
        if ( hCursor )
            {
            openPositionList.Cleanup( );
            SetCursor( hCursor );
            hCursor = NULL;
            THROW_LAST( );
            }
        }
    END_CATCH
    
    SetCursor( hCursor );
    hCursor = NULL;
    }

//////////////////////////////////////////////////////////////////////////////
// BuildSimulatorTrades( )
//
// Creates a simulator trade list
//////////////////////////////////////////////////////////////////////////////
static void BuildSimulatorTrades( const SimulatorRequest &req, QTradeList *ptrlist )
    {
    TRY
        {
        if ( req.type == SimulatorRequest::eLoadedCompany )
            ::AppendSingle( req, ptrlist );
        else    
            ::AppendAll( req, ptrlist );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// AppendAll( )
//
// Append all securities in current directory to simulator trade list
//////////////////////////////////////////////////////////////////////////////
static void AppendAll( const SimulatorRequest &req, QTradeList *ptrlist )
    {
    struct find_t fileinfo;
    int rc;

    TRY
        {
        SaveQuery( req.hwndParent );
        DestroyAllChildren( );
        _pdoc->Cleanup( );
        UpdateName( );
        UpdateMenu( );

        rc = _dos_findfirst( req.sFilter, _A_RDONLY, &fileinfo );
        while ( rc == 0 )
            {
            TRY
                {
                ::SilentOpen( fileinfo.name );
                ::AppendSingle( req, ptrlist );
                }
            CATCH( exMemory )
                {
                THROW_LAST( );
                }
            CATCH_ALL
                {
                // other problems, ignore
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
        THROW_LAST( );
        }
    END_CATCH

    DestroyAllChildren( );
    _pdoc->Cleanup( );
    UpdateName( );
    UpdateMenu( );
    }

//////////////////////////////////////////////////////////////////////////////
// AppendSingle( )
//
// Append loaded security to simulator trade list
//////////////////////////////////////////////////////////////////////////////
static void AppendSingle( const SimulatorRequest &req, QTradeList *ptrlist )
    {
    QRule *prule;
    QSignalList sglist;

    TRY
        {
        prule = _ptpl->m_rulelist.Find( req.sRuleName );
        ASSERT( prule != NULL );

        if ( prule->Evaluate( sglist, FALSE, FALSE, 0 ) )
            ::CreateSimulatorTradeList( sglist, req, ptrlist );
        }
    CATCH_ALL
        {
        sglist.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// CreateSimulatorTradeList( )
//
// Builds a simulator trade list given a signal list
//////////////////////////////////////////////////////////////////////////////
static void CreateSimulatorTradeList( QSignalList &sglist, const SimulatorRequest &req, QTradeList *ptrlist )
    {
    QTrade *ptr = NULL;

    TRY
        {
        for ( QSignal *psg = (QSignal *) sglist.GetHead( );
              psg && !psg->GetPendingState( );
              psg = (QSignal *) sglist.GetNext( psg ) )
            {
            ptr = new QTrade( );
            if ( !ptr )
                {
                THROW_EXCEPTION( exMemory );
                }

            ptr->security( _pdoc->GetFileName( ) );
            ptr->date( psg->GetDate( ) );
            ptr->signalType( psg->GetSg( ) );
            ptr->price( psg->GetPrice( ) );
            ASSERT( _pdoc->GetLast( ) != NULL );
            ptr->lastClose( _pdoc->GetLast( )->GetCl( ) );

            ptrlist->InsertTrade( ptr );
            }
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// positionSize( )
//
// Returns a trade's position size, *excluding* commission
//////////////////////////////////////////////////////////////////////////////
static NUM positionSize( const SimulatorRequest &req, NUM numCash )
    {
    // Position sizes of 0 are indicative of entire cash balance
    if ( req.numPosition != 0 )
        return req.numPosition;

    if ( numCash > req.numComm )
        return numCash - req.numComm;

    // No position
    return 0;
    }

//////////////////////////////////////////////////////////////////////////////
// PostProcessSimulatorTrades( )
//
// Populates/Filters trades, and populates the open position table
//////////////////////////////////////////////////////////////////////////////
static void PostProcessSimulatorTrades( const SimulatorRequest &req, 
    QTradeList *ptrlist, QTradeList *ptrlistOpenPosition )
    {
    NUM numCash = req.numInitialAccount;
    QTrade *ptr = (QTrade *) ptrlist->GetHead( );
    while ( ptr )
        {
        if ( ptr->signalType( ) == QSignal::sgBuy )
            {
            NUM numPosition = positionSize( req, numCash );
            if ( ( numPosition > 0 ) && ( numPosition + req.numComm <= numCash ) )
                {
                numCash -= ( numPosition + req.numComm );
                ptr->positionSize( numPosition );
                ptr->cashBalance( numCash );

                // Place a copy of the trade into the open position table
                ASSERT( ptrlistOpenPosition->FindSecurity( ptr->security( ) ) == NULL );
                QTrade *ptrOpen = new QTrade;
                if ( !ptrOpen )
                    THROW_EXCEPTION( exMemory );
                ptrOpen->security( ptr->security( ) );
                ptrOpen->date( ptr->date( ) );
                ptrOpen->signalType( ptr->signalType( ) );
                ptrOpen->price( ptr->price( ) );
                ptrOpen->lastClose( ptr->lastClose( ) );
                ptrOpen->positionSize( ptr->positionSize( ) );
                ptrlistOpenPosition->InsertTrade( ptrOpen );

                // Advance to next trade
                ptr = (QTrade *) ptrlist->GetNext( ptr );
                }
            else
                {
                // Remove corresponding sell, if it exists
                QTrade *ptrSell = ptrlist->FindNextSell( ptr );
                
                if ( ptrSell )
                    {
                    ASSERT( ptrSell != ptr );
                    ptrlist->Remove( ptrSell );
                    delete ptrSell;
                    }

                // Remove current trade
                QTrade *ptrSav = ptr;
                ptr = (QTrade *) ptrlist->GetNext( ptr );
                ptrlist->Remove( ptrSav );
                delete ptrSav;
                }
            }
        else
            {
            QTrade *ptrOpen = ptrlistOpenPosition->FindSecurity( ptr->security( ) );
            ASSERT( ptrOpen != NULL );

            // Calculate the profit
            ASSERT( ptrOpen->price( ) != (NUM) 0 );
            NUM numRatio = ptr->price( ) / ptrOpen->price( );
            NUM numGrossProfit = ptrOpen->positionSize( ) * ( numRatio - (NUM) 1.0 );
            NUM numNetProfit = numGrossProfit - (NUM) 2.0 * req.numComm;
            ptr->profit( numNetProfit );

            // Calculate the cash balance
            numCash += ( ptrOpen->positionSize( ) + numGrossProfit - req.numComm );
            ptr->cashBalance( numCash );

            // Remove the trade from the open position table
            ptrlistOpenPosition->Remove( ptrOpen );
            delete ptrOpen;

            // Advance to next trade
            ptr = (QTrade *) ptrlist->GetNext( ptr );
            }
        }
    }

//////////////////////////////////////////////////////////////////////////////
// PopulateSimulatorSummary( )
//
// Populates the simulator summary 
//////////////////////////////////////////////////////////////////////////////
void PopulateSimulatorSummary( const SimulatorRequest &req, 
    QTradeList *ptrlistSim, 
    QTradeList *ptrlistOpenPosition,  
    NVPairList *psummarySim )
    {
    NUM numPortfolioValue = 0;
    NVPair *pr = NULL;
    CHAR sz[ _cbBufferMax + 1 ];
    CHAR sz2[ _cbBufferMax + 1 ];

    TRY
        {
        sprintf( sz, "%13s", (const CHAR *) req.sRuleName );
        pr = NVPair::create( " Rule name", sz, TRUE );
        psummarySim->AddTail( pr );

        pr = NVPair::createEmpty( );
        psummarySim->AddTail( pr );

        sprintf( sz, " $ %10.2f", req.numInitialAccount );
        pr = NVPair::create( " Initial account size", sz, TRUE );
        psummarySim->AddTail( pr );

        sprintf( sz, " $ %10.2f", req.numPosition );
        pr = NVPair::create( " Position size", sz, TRUE );
        psummarySim->AddTail( pr );

        sprintf( sz, " $ %10.2f", req.numComm );
        pr = NVPair::create( " Commission size", sz, TRUE );
        psummarySim->AddTail( pr );

        pr = NVPair::createEmpty( );
        psummarySim->AddTail( pr );

        if ( ptrlistSim->GetCount( ) > 0 )
            {
            for ( QTrade *ptr = (QTrade *) ptrlistOpenPosition->GetHead( );
                  ptr;
                  ptr = (QTrade *) ptrlistOpenPosition->GetNext( ptr ) )
                {
                // Calculate the open position
                NUM numRatio = ptr->lastClose( ) / ptr->price( );
                NUM numGrossProfit = ptr->positionSize( ) * ( numRatio - (NUM) 1.0 );

                // Do not include commission because this is not a net profit
                // calculation; commission was already deducted during the purchase
                NUM numOutstandingPosition = ptr->positionSize( ) + numGrossProfit;
                
                sprintf( sz, " %-13s", (const CHAR *) ptr->security( ) );
                sprintf( sz2, " $ %10.2f", numOutstandingPosition );
                pr = NVPair::create( sz, sz2 );
                psummarySim->AddTail( pr );
                
                numPortfolioValue += numOutstandingPosition;
                }

            pr = NVPair::createEmpty( );
            psummarySim->AddTail( pr );

            QTrade *ptrLast = (QTrade *) ptrlistSim->GetTail( );
            sprintf( sz, " $ %10.2f", ptrLast->cashBalance( ) );
            pr = NVPair::create( " Cash balance", sz );
            psummarySim->AddTail( pr );
            
            numPortfolioValue += ptrLast->cashBalance( );
            sprintf( sz, " $ %10.2f", numPortfolioValue );
            pr = NVPair::create( " Portfolio value", sz );
            psummarySim->AddTail( pr );
            }
        else
            {
            sprintf( sz, " $ %10.2f", req.numInitialAccount );
            pr = NVPair::create( " Cash balance", sz );
            psummarySim->AddTail( pr );
            
            numPortfolioValue += req.numInitialAccount;
            sprintf( sz, " $ %10.2f", numPortfolioValue );
            pr = NVPair::create( " Portfolio value", sz );
            psummarySim->AddTail( pr );
            }
        
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }
