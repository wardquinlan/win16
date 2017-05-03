//////////////////////////////////////////////////////////////////////////////
// RSI.CPP
//
// Relative Strength Index Indicator
//
// This is for use with the Quote package.
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <stdlib.h>
#include <statpak.h>
#include "rsi.hxx"

#define _cbBufferMax 128
static BOOL Rsi( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod );

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
    return (LPCSTR) "RSI Version 1.10";
    }

//////////////////////////////////////////////////////////////////////////////
// ParamString( )
//
// Returns RSI parameters formatted as a string
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
    if ( !DataSetAlloc( &lpcalc->datasetHorz, 5, 0 ) )
        {
        return FALSE;
        }
    lpcalc->datasetHorz.lpnum[ 0 ] =   0.0;
    lpcalc->datasetHorz.lpnum[ 1 ] =  30.0;
    lpcalc->datasetHorz.lpnum[ 2 ] =  50.0;
    lpcalc->datasetHorz.lpnum[ 3 ] =  70.0;
    lpcalc->datasetHorz.lpnum[ 4 ] = 100.0;
    
    return Rsi( (LPDATASET) &lpcalc->datasetCalc, 
                (LPDATASET) &lpcalc->datasetCl,
                nPeriod );
    }

//////////////////////////////////////////////////////////////////////////////
// Rsi
//
// Relative Strength Index calculation
//////////////////////////////////////////////////////////////////////////////
static BOOL Rsi( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod )
    {
    BOOL    fError = FALSE;
    BOOL    fRet = FALSE;
    WORD    nPointsData;
    WORD    i;
    NUM     numMean;
    DATASET datasetPC;
    DATASET datasetNC;
    DATASET datasetP;
    DATASET datasetN;
    DATASET datasetRSI;

    TASKLIST
        {
        DataSetZero( &datasetPC  );
        DataSetZero( &datasetNC  );
        DataSetZero( &datasetP   );
        DataSetZero( &datasetN   );
        DataSetZero( &datasetRSI );

        if ( nPeriod >= lpdatasetData->nPoints )
            {
            QUIT;
            }

        nPointsData = lpdatasetData->nPoints;
        if ( !DataSetAlloc( &datasetPC, nPointsData - 1, 1 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetNC, nPointsData - 1, 1 ) )
            {
            QUIT;
            }
        
        if ( !DataSetAlloc( &datasetP, nPointsData - nPeriod, nPeriod  + 1 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetN, nPointsData - nPeriod, nPeriod + 1 ) )
            {
            QUIT;
            }

        if ( !DataSetAlloc( &datasetRSI, nPointsData - nPeriod, nPeriod ) )
            {
            QUIT;
            }

        for ( i = 0; i < datasetPC.nPoints; i++ )
            {
            datasetPC.lpnum[ i ] = max( lpdatasetData->lpnum[ i + 1 ] - lpdatasetData->lpnum[ i ], (NUM) 0.0 );
            datasetNC.lpnum[ i ] = max( lpdatasetData->lpnum[ i ] - lpdatasetData->lpnum[ i + 1 ], (NUM) 0.0 );
            }

        numMean = 0.0;
        for ( i = 0; i < nPeriod; i++ )
            {
            numMean += datasetPC.lpnum[ i ];
            }
        numMean /= (NUM) nPeriod;

        datasetP.lpnum[ 0 ] = numMean;
        for ( i = 1; i < datasetP.nPoints; i++ )
            {
            NUM num;
            num  = datasetP.lpnum[ i - 1 ];
            num *= ( nPeriod - 1 );
            num += datasetPC.lpnum[ i + nPeriod - 1 ]; // PC and P are offset by nPeriod - 1
            num /= (NUM) nPeriod;
            
            datasetP.lpnum[ i ] = num;
            }

        numMean = 0.0;
        for ( i = 0; i < nPeriod; i++ )
            {
            numMean += datasetNC.lpnum[ i ];
            }
        numMean /= (NUM) nPeriod;

        datasetN.lpnum[ 0 ] = numMean;
        for ( i = 1; i < datasetN.nPoints; i++ )
            {
            NUM num;
            num  = datasetN.lpnum[ i - 1 ];
            num *= ( nPeriod - 1 );
            num += datasetNC.lpnum[ i + nPeriod - 1 ]; // NC and N are offset by nPeriod - 1
            num /= (NUM) nPeriod;

            datasetN.lpnum[ i ] = num;
            }

        for ( i = 0; i < datasetRSI.nPoints && !fError; i++ )
            {
            NUM numRS;
            NUM numRSI;

            if ( datasetN.lpnum[ i ] == 0.0 )
                {
                // catch divide by zero
                fError = TRUE;
                break;
                }
            numRS  = datasetP.lpnum[ i ] / datasetN.lpnum[ i ];
            numRSI = (NUM) 1.0 + numRS;
            numRSI = (NUM) 100.0 / numRSI;
            numRSI = (NUM) 100.0 - numRSI;
            datasetRSI.lpnum[ i ] = numRSI;
            }

        if ( fError )
            {
            QUIT;
            }

        if ( !DataSetCopy( lpdatasetResult, &datasetRSI ) )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST

    DataSetFree( &datasetPC  );
    DataSetFree( &datasetNC  );
    DataSetFree( &datasetP   );
    DataSetFree( &datasetN   );
    DataSetFree( &datasetRSI );
    return fRet;
    }
