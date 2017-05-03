//////////////////////////////////////////////////////////////////////////////
// DMI.CPP
//
// Directional Movement Indicator
//
// This is for use with the Quote package.
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <stdlib.h>
#include <statpak.h>
#include "dmi.hxx"

#define _cbBufferMax 128
inline NUM numabs( NUM num );

#define TASKLIST do
#define ENDTASKLIST while ( 0 );
#define QUIT break

//////////////////////////////////////////////////////////////////////////////
// numabs( )
//
// absolute value
//////////////////////////////////////////////////////////////////////////////
inline NUM numabs( NUM num )
    {
    return ( num < 0 ? -num : num );
    }

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
    return (LPCSTR) "DMI Version 1.20";
    }

//////////////////////////////////////////////////////////////////////////////
// ParamString( )
//
// Returns DMI parameters formatted as a string
//////////////////////////////////////////////////////////////////////////////
LPCSTR FAR PASCAL _export ParamString( LPOPTIONS lpopts )
    {
    static char szBuffer[ _cbBufferMax + 1 ];
    memset( szBuffer, 0, sizeof( szBuffer ) );
    WORD nPeriod = LOWORD( lpopts->lOptions );
    WORD nOption = HIWORD( lpopts->lOptions );
    wsprintf( szBuffer, "%u,%u", nPeriod, nOption );
    return (LPCSTR) szBuffer;
    }

//////////////////////////////////////////////////////////////////////////////
// Options( )
//
// Options routine
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Options( LPOPTIONS lpopts )
    {
    return Options2( lpopts );
    }

//////////////////////////////////////////////////////////////////////////////
// Calculate( )
//
// Calculate routine
//////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _export Calculate( LPCALCSTRUCT lpcalc )
    {
    BOOL    fRet   = FALSE;
    BOOL    fError = FALSE;
    WORD    nPeriod1;
    WORD    nOption;
    DATASET dsPdm; // Plus  DM
    DATASET dsMdm; // Minum DM
    DATASET dsTr;
    DATASET dsAPdm;
    DATASET dsAMdm;
    DATASET dsATr;
    DATASET dsPdi;
    DATASET dsMdi;
    DATASET dsDx;
    WORD    iCl;
    DATASET FAR &dsHi = lpcalc->datasetHi;
    DATASET FAR &dsLo = lpcalc->datasetLo;
    DATASET FAR &dsCl = lpcalc->datasetCl;

    TASKLIST
        {
        // initialize our result dataset
        DataSetZero( &dsPdm  );
        DataSetZero( &dsMdm  );
        DataSetZero( &dsTr   );
        DataSetZero( &dsAPdm );
        DataSetZero( &dsAMdm );
        DataSetZero( &dsATr  );
        DataSetZero( &dsPdi  );
        DataSetZero( &dsMdi  );
        DataSetZero( &dsDx   );
        
        nPeriod1 = LOWORD( lpcalc->opts.lOptions );
        nOption  = HIWORD( lpcalc->opts.lOptions );
        
        // do some sanity checks
        if ( nPeriod1 < 1 )
            break;
        if ( nOption != 0 && nOption != 1 && nOption != 2 )
            break;
        if ( dsHi.nPoints != dsLo.nPoints || dsHi.nPoints != dsCl.nPoints )
            break;
        if ( lpcalc->datasetCl.nPoints < nPeriod1 + 1 )
            break;

        if ( !DataSetAlloc( &dsPdm, dsCl.nPoints - 1, 1 ) )
            break;

        if ( !DataSetAlloc( &dsMdm, dsCl.nPoints - 1, 1 ) )
            break;

        if ( !DataSetAlloc( &dsTr,  dsCl.nPoints - 1, 1 ) )
            break;

        for ( iCl = 1; iCl < dsCl.nPoints; iCl++ )
            {
            const WORD iHi  = iCl;
            const WORD iLo  = iCl;
            const WORD iPdm = iCl - 1;
            const WORD iMdm = iPdm;

            dsPdm.lpnum[ iPdm ] = 0;
            dsMdm.lpnum[ iMdm ] = 0;

            if ( dsHi.lpnum[ iHi ] >  dsHi.lpnum[ iHi - 1 ] &&
                 dsLo.lpnum[ iLo ] >= dsLo.lpnum[ iLo - 1 ] )
                {
                // UP day
                dsPdm.lpnum[ iPdm ] = dsHi.lpnum[ iHi ] - dsHi.lpnum[ iHi - 1 ];
                }
            else if ( dsHi.lpnum[ iHi ] <= dsHi.lpnum[ iHi - 1 ] &&
                      dsLo.lpnum[ iLo ] <  dsLo.lpnum[ iLo - 1 ] )
                {
                // DOWN day
                dsMdm.lpnum[ iMdm ] = dsLo.lpnum[ iLo - 1 ] - dsLo.lpnum[ iLo ];
                }
            else if ( dsHi.lpnum[ iHi ] > dsHi.lpnum[ iHi - 1 ] &&
                      dsLo.lpnum[ iLo ] < dsLo.lpnum[ iLo - 1 ] )
                {
                // OUTSIDE day
                NUM numUp   = dsHi.lpnum[ iHi ] - dsHi.lpnum[ iHi - 1 ];
                NUM numDown = dsLo.lpnum[ iLo - 1 ] - dsLo.lpnum[ iLo ];

                if ( numUp > numDown )
                    {
                    dsPdm.lpnum[ iPdm ] = numUp;
                    }
                else if ( numUp < numDown )
                    {
                    dsMdm.lpnum[ iMdm ] = numDown;
                    }
                // else, no directional movement
                }
            }

        for ( iCl = 1; iCl < dsCl.nPoints; iCl++ )
            {
            const WORD iHi  = iCl;
            const WORD iLo  = iCl;
            const WORD iTr  = iCl - 1;

            NUM num = dsHi.lpnum[ iHi ] - dsLo.lpnum[ iLo ];
            num = max( numabs( dsHi.lpnum[ iHi ] - dsCl.lpnum[ iCl - 1 ] ), num );
            num = max( numabs( dsLo.lpnum[ iLo ] - dsCl.lpnum[ iCl - 1 ] ), num );

            dsTr.lpnum[ iTr ] = num;
            }

        if ( !Mean( &dsAPdm, &dsPdm, nPeriod1 ) )
            break;

        if ( !Mean( &dsAMdm, &dsMdm, nPeriod1 ) )
            break;

        if ( !Mean( &dsATr,  &dsTr,  nPeriod1 ) )
            break;
        
        if ( dsAPdm.nPoints != dsAMdm.nPoints || dsAPdm.nOffset != dsAMdm.nOffset ) 
            break;

        if ( dsAPdm.nPoints != dsATr.nPoints || dsAPdm.nOffset != dsATr.nOffset )
            break;

        if ( dsAMdm.nPoints != dsATr.nPoints || dsAMdm.nOffset != dsATr.nOffset)
            break;

        if ( !DataSetAlloc( &dsPdi, dsAPdm.nPoints, dsAPdm.nOffset ) )
            break;

        if ( !DataSetAlloc( &dsMdi, dsAMdm.nPoints, dsAMdm.nOffset ) )
            break;

        if ( !DataSetAlloc( &dsDx,  dsAPdm.nPoints, dsAPdm.nOffset ) )
            break;

        for ( WORD iPdm = 0; iPdm < dsAPdm.nPoints; iPdm++ )
            {
            const WORD iMdm = iPdm;
            const WORD iPdi = iPdm;
            const WORD iMdi = iPdm;
            const WORD iTr  = iPdm;
            const WORD iRes = iPdm;
            const WORD iDx  = iPdm;

            if ( dsATr.lpnum[ iTr ] == 0 )
                {
                fError = TRUE;
                break;
                }

            dsPdi.lpnum[ iPdi ] = (NUM) 100.0 * dsAPdm.lpnum[ iPdm ] / dsATr.lpnum[ iTr ];
            dsMdi.lpnum[ iMdi ] = (NUM) 100.0 * dsAMdm.lpnum[ iMdm ] / dsATr.lpnum[ iTr ];

            if ( dsPdi.lpnum[ iPdi ] + dsMdi.lpnum[ iMdi ] == 0 )
                {
                fError = TRUE;
                break;
                }

            dsDx.lpnum[ iDx ]   = (NUM) 100.0 * 
                                  numabs( dsPdi.lpnum[ iPdi ] - dsMdi.lpnum[ iMdi ] ) /
                                  ( dsPdi.lpnum[ iPdi ] + dsMdi.lpnum[ iMdi ] );
            }
        
        if ( fError )
            break;
        
        if ( nOption == 0 )
            {
            if ( DataSetCopy( &lpcalc->datasetCalc, &dsDx ) )
                fRet = TRUE;
            }
        else if ( nOption == 1 )
            {
            if ( DataSetCopy( &lpcalc->datasetCalc, &dsPdi ) )
                fRet = TRUE;
            }
        else if ( nOption == 2 )
            {
            if ( DataSetCopy( &lpcalc->datasetCalc, &dsMdi ) )
                fRet = TRUE;
            }
        }
    ENDTASKLIST
    DataSetFree( &dsPdm  );
    DataSetFree( &dsMdm  );
    DataSetFree( &dsTr   );
    DataSetFree( &dsAPdm );
    DataSetFree( &dsAMdm );
    DataSetFree( &dsATr  );
    DataSetFree( &dsPdi  );
    DataSetFree( &dsMdi  );
    DataSetFree( &dsDx   );
    
    return fRet;
    }
