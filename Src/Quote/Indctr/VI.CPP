//////////////////////////////////////////////////////////////////////////////
// VI.CPP
//
// Volatility Index Indicator
//
// This is for use with the Quote package.
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <statpak.h>
#include "vi.hxx"

#define _cbBufferMax 128
static BOOL Vi( LPDATASET lpdsResult, 
                LPDATASET lpdsHi, 
                LPDATASET lpdsLo,
                LPDATASET lpdsCl,
                WORD      nPeriod );

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
    return (LPCSTR) "VI Version 1.10";
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
// ParamString( )
//
// Returns VI parameters formatted as a string
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
// Calculate( )
//
// Calculate routine
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Calculate( LPCALCSTRUCT lpcalc )
    {
    WORD nPeriod;
    
    nPeriod = LOWORD( lpcalc->opts.lOptions );
    return Vi( (LPDATASET) &lpcalc->datasetCalc, 
               (LPDATASET) &lpcalc->datasetHi,
               (LPDATASET) &lpcalc->datasetLo,
               (LPDATASET) &lpcalc->datasetCl,
               nPeriod );
    }

//////////////////////////////////////////////////////////////////////////////
// Vi
//
// Volatility Index calculation
//////////////////////////////////////////////////////////////////////////////
static BOOL Vi( LPDATASET lpdsResult, 
                LPDATASET lpdsHi, 
                LPDATASET lpdsLo,
                LPDATASET lpdsCl,
                WORD      nPeriod )
    {
    DATASET dsTR;
    BOOL fRet = FALSE;
    
    TASKLIST
        {
        if ( lpdsCl->nPoints <= 1 )
            break;
        
        if ( lpdsCl->nPoints != lpdsHi->nPoints ||
             lpdsCl->nPoints != lpdsLo->nPoints )
            break;
        
        if ( !DataSetAlloc( &dsTR, lpdsCl->nPoints - 1, 1 ) )
            break;

        for ( WORD iCl = 1; iCl < lpdsCl->nPoints; ++iCl )
            {
            WORD FAR &iHi = iCl;
            WORD FAR &iLo = iCl;
            const WORD iTr = iCl - 1;
            
            NUM num = (NUM) fabs( (double) ( lpdsHi->lpnum[ iHi ] - lpdsLo->lpnum[ iLo ] ) );
            num = max( (NUM) fabs( (double) ( lpdsHi->lpnum[ iHi ] - lpdsCl->lpnum[ iCl - 1 ] ) ), num );
            num = max( (NUM) fabs( (double) ( lpdsLo->lpnum[ iLo ] - lpdsCl->lpnum[ iCl - 1 ] ) ), num );            
            
            dsTR.lpnum[ iTr ] = num;
            }

        fRet = Mean( lpdsResult, &dsTR, nPeriod );
        DataSetFree( &dsTR );
        }
    ENDTASKLIST
    return fRet;
    }
