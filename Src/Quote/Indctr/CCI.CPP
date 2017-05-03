//////////////////////////////////////////////////////////////////////////////
// CCI.CPP
//
// Commodity Channel Index Indicator
//
// This is for use with the Quote package.
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <stdlib.h>
#include <statpak.h>
#include "cci.hxx"

#define _cbBufferMax 128
static BOOL Cci( LPDATASET lpdatasetResult, LPDATASET lpdatasetHi, LPDATASET lpdatasetLo, LPDATASET lpdatasetCl, WORD nPeriod );

#define TASKLIST do
#define ENDTASKLIST while ( 0 );
#define QUIT break

//////////////////////////////////////////////////////////////////////////////
// LibMain( )
//
// Library entrance routine.
//////////////////////////////////////////////////////////////////////////////
int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine )
    {
    if ( wHeapSize > 0 )
        {
        UnlockData( 0 );
        }
    return 1;
    }

//////////////////////////////////////////////////////////////////////////////
// WEP( )
//
// Windows Exit Procedure
//////////////////////////////////////////////////////////////////////////////
int FAR PASCAL _export WEP( int nParam )
    {
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// VerString( )
//
// Version string routine
//////////////////////////////////////////////////////////////////////////////
LPCSTR FAR PASCAL _export VerString( void )
    {
    return (LPCSTR) "CCI Version 1.10";
    }

//////////////////////////////////////////////////////////////////////////////
// ParamString( )
//
// Returns CCI parameters formatted as a string
//////////////////////////////////////////////////////////////////////////////
LPCSTR FAR PASCAL _export ParamString( LPOPTIONS lpopts )
    {
    static char szBuffer[ _cbBufferMax + 1 ];
    memset( szBuffer, 0, sizeof( szBuffer ) );
    WORD nPeriod = LOWORD( lpopts->lOptions );
    wsprintf( szBuffer, "%u", nPeriod );
    return (LPCSTR) szBuffer;
    }

//////////////////////////////////////////////////////////////////////////////
// Options( )
//
// Options routine
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Options( LPOPTIONS lpopts )
    {
    return PeriodOptions( lpopts );
    }

//////////////////////////////////////////////////////////////////////////////
// Calculate( )
//
// Calculate routine
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Calculate( LPCALCSTRUCT lpcalc )
    {
    WORD nPeriod;
    
    nPeriod = LOWORD( lpcalc->opts.lOptions );
    if ( !DataSetAlloc( &lpcalc->datasetHorz, 2, 0 ) )
        {
        return FALSE;
        }
    lpcalc->datasetHorz.lpnum[ 0 ] =  -100.0;
    lpcalc->datasetHorz.lpnum[ 1 ] =   100.0;
    
    return Cci( (LPDATASET) &lpcalc->datasetCalc, 
                (LPDATASET) &lpcalc->datasetHi,
                (LPDATASET) &lpcalc->datasetLo,
                (LPDATASET) &lpcalc->datasetCl,
                nPeriod );
    }

//////////////////////////////////////////////////////////////////////////////
// Cci
//
// Commodity Channel Index calculation
//////////////////////////////////////////////////////////////////////////////
static BOOL Cci( LPDATASET lpdatasetResult, 
                 LPDATASET lpdatasetHi, 
                 LPDATASET lpdatasetLo, 
                 LPDATASET lpdatasetCl, 
                 WORD      nPeriod )
    {
    WORD    i;
    WORD    j;
    DATASET datasetM;
    DATASET datasetMBar;
    DATASET datasetDBar;
    DATASET datasetCCI;
    BOOL    fRet( FALSE );
    BOOL    fError( FALSE );
    
    TASKLIST
        {
        DataSetZero( &datasetM );
        DataSetZero( &datasetMBar );
        DataSetZero( &datasetDBar );
        DataSetZero( &datasetCCI );

        if ( lpdatasetHi->nPoints != lpdatasetLo->nPoints ||
             lpdatasetLo->nPoints != lpdatasetCl->nPoints )
            {
            QUIT;
            }

        if ( nPeriod <= 1 || nPeriod > lpdatasetHi->nPoints )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetM, lpdatasetHi->nPoints, 0 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetDBar, lpdatasetHi->nPoints - nPeriod + 1, nPeriod - 1 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetCCI, lpdatasetHi->nPoints - nPeriod + 1, nPeriod - 1 ) )
            {
            QUIT;
            }

        for ( i = 0; i < lpdatasetHi->nPoints; i++ )
            {
            datasetM.lpnum[ i ] = ( lpdatasetHi->lpnum[ i ] + lpdatasetLo->lpnum[ i ] + lpdatasetCl->lpnum[ i ] ) / (NUM) 3.0;
            }
        
        if ( !Mean( &datasetMBar, &datasetM, nPeriod ) )
            {
            QUIT;
            }
        
        // i indexes through MBar (and DBar and CCI)
        for ( i = 0; i < datasetMBar.nPoints; i++ )
            {
            NUM numD( 0.0 ); // deviation

            // j indexes through M
            for ( j = i; j < i + nPeriod; j++ )
                {
                numD += ( datasetMBar.lpnum[ i ] > datasetM.lpnum[ j ] ) ?
                          datasetMBar.lpnum[ i ] - datasetM.lpnum[ j ]   :
                          datasetM.lpnum[ j ] - datasetMBar.lpnum[ i ];
                }
            
            if ( numD == (NUM) 0.0 )
                {
                // Better not divide by 0
                fError = TRUE;
                break;
                }

            // finally, calculate the mean deviation
            numD /= nPeriod;
            datasetDBar.lpnum[ i ] = numD;

            // to index M using i, we must transform i --> i + nPeriod - 1
            datasetCCI.lpnum[ i ] = ( datasetM.lpnum[ i + nPeriod - 1 ] - datasetMBar.lpnum[ i ] ) /
                                    ( (NUM) 0.015 * datasetDBar.lpnum[ i ] );
            }

        if ( fError )
            {
            QUIT;
            }

        if ( !DataSetCopy( lpdatasetResult, &datasetCCI ) )
            {
            QUIT;
            }

        fRet = TRUE;
        }
    ENDTASKLIST

    DataSetFree( &datasetM );
    DataSetFree( &datasetMBar );
    DataSetFree( &datasetDBar );
    DataSetFree( &datasetCCI );
    return fRet;
    }
