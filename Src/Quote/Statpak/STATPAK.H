//////////////////////////////////////////////////////////////////////////////
// STATPAK.H
//
// Statistics and memory management tool functions for use with the
// Quote package.
//
// Copyright (c) Ward Quinlan, 1996
//
//////////////////////////////////////////////////////////////////////////////
#ifndef _STATPAK_H
#define _STATPAK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef float NUM; 
typedef float FAR *LPNUM;

//////////////////////////////////////////////////////////////////////////////
// WORD3
//
// This data structure is used to hold 3 words for 3-word options.  Note
// that WORD1 and WORD2 data structures are not defined because
// these fit inside 32 bits.
//////////////////////////////////////////////////////////////////////////////
typedef struct tagWORD3
    {
    WORD w1;
    WORD w2;
    WORD w3;
    } WORD3, FAR *LPWORD3;

//////////////////////////////////////////////////////////////////////////////
// OPTIONS
//
// Options data structure: used to interface between Quote
// and the indicator library.
//
// hwndParent - the parent HWND in question
// cbOptions  - count of options data:
//              If cbOptions is 0, then lOptions is treated as the options
//              data itself.  If cbOptions is > 0, then lOptions is treated
//              as a *pointer* to the options data, with length cbOptions.
// lOptions   - user-defined options data (or pointer to data)
//////////////////////////////////////////////////////////////////////////////
typedef struct tagOPTIONS
    {
    HWND   hwndParent;
    WORD   cbOptions;
    DWORD  lOptions;
    } OPTIONS, FAR *LPOPTIONS;

//////////////////////////////////////////////////////////////////////////////
// DATASET
//
// This fundamental data structure is used to pass data to and from
// Quote and the indicator library.
//
// nPoints - The number of points (NUMs) in the dataset
// nOffset - The offset of the dataset with respect to the Quote data.
// lpnum   - Array of data.
//
//////////////////////////////////////////////////////////////////////////////
typedef struct tagDATASET
    {
    WORD       nPoints;
    WORD       nOffset;
    NUM FAR   *lpnum;
    } DATASET, FAR *LPDATASET;

//////////////////////////////////////////////////////////////////////////////
// CALCSTRUCT
//
// Calcstructs are used to interface with the Calculate( ) API
// in indicators.
//
// datasetOp - dataset of Quote Op data.
// datasetHi - dataset of Quote Hi data.
// datasetLo - dataset of Quote Lo data.
// datasetCl - dataset of Quote Close data.
// datasetVl - dataset of Quote Volume data.
// opts      - options data.
// datasetCalc - return value of Calculate( ).
// datasetHorz - dataset of horizontal lines (optional) used by the indicator.
//////////////////////////////////////////////////////////////////////////////
typedef struct tagCALCSTRUCT
    {
    DATASET datasetOp;
    DATASET datasetHi;
    DATASET datasetLo;
    DATASET datasetCl;
    DATASET datasetVl;
    OPTIONS opts;

    DATASET datasetCalc;
    DATASET datasetHorz;
    } CALCSTRUCT, FAR *LPCALCSTRUCT;

//////////////////////////////////////////////////////////////////////////////
// User-define routines and typedefs
//
// LPVERSTRINGPROC, VerString - Users must define a version routine.
// LPPARAMSTRINGPROC, ParamString - Users must define a param string routine.
// LPOPTIONSPROC,   Options   - Users must define an options routine.
// LPCALCPROC,      Calculate - Users must define a Calculate routine.
//////////////////////////////////////////////////////////////////////////////
typedef LPCSTR (FAR PASCAL _export *LPVERSTRINGPROC)( void );
typedef LPCSTR (FAR PASCAL _export *LPPARAMSTRINGPROC)( LPOPTIONS );
typedef BOOL (FAR PASCAL _export *LPOPTIONSPROC)( LPOPTIONS );
typedef BOOL (FAR PASCAL _export *LPCALCPROC)( LPCALCSTRUCT );

LPCSTR FAR PASCAL _export VerString( void );
LPCSTR FAR PASCAL _export ParamString( LPOPTIONS lpopts );
BOOL   FAR PASCAL _export Options( LPOPTIONS lpopts );
BOOL   FAR PASCAL _export Calculate( LPCALCSTRUCT lpcalc );

//////////////////////////////////////////////////////////////////////////////
// Memory management routines
//
//////////////////////////////////////////////////////////////////////////////
void FAR *FAR PASCAL _export GAlloc( UINT cb );
void FAR PASCAL _export GFree( void FAR *lp );

//////////////////////////////////////////////////////////////////////////////
// Helper routines
//
//////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export DataSetZero ( LPDATASET lpdataset );
BOOL FAR PASCAL _export DataSetAlloc( LPDATASET lpdataset, WORD nPoints, WORD nOffset );
void FAR PASCAL _export DataSetFree ( LPDATASET lpdataset );
BOOL FAR PASCAL _export DataSetCopy ( LPDATASET lpdatasetDst, LPDATASET lpdatasetSrc );

void FAR PASCAL _export OptionsZero ( LPOPTIONS lpoptions );
BOOL FAR PASCAL _export OptionsAlloc( LPOPTIONS lpoptions, HWND hwnd, WORD cbOptions, DWORD lOptions );
void FAR PASCAL _export OptionsFree ( LPOPTIONS lpoptions );
BOOL FAR PASCAL _export OptionsCopy ( LPOPTIONS lpoptionsDst, LPOPTIONS lpoptionsSrc );

BOOL FAR PASCAL _export PeriodOptions( LPOPTIONS lpopts );
BOOL FAR PASCAL _export PeriodDlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export Options2( LPOPTIONS lpopts );
BOOL FAR PASCAL _export Options2DlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL FAR PASCAL _export Options3( LPOPTIONS lpopts );
BOOL FAR PASCAL _export Options3DlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );

HBRUSH FAR PASCAL _export ManageCtlColor( WPARAM wParam, LPARAM lParam );

//////////////////////////////////////////////////////////////////////////////
// Statistical routines.
//
//////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL _export Mean( LPDATASET lpdatasetResult,  
                              LPDATASET lpdatasetData,  
                              WORD      nPeriod );

BOOL FAR PASCAL _export ExpSmooth( LPDATASET lpdatasetResult,
                                   LPDATASET lpdatasetData,
                                   WORD      nPeriod );

BOOL FAR PASCAL _export Lowest( LPDATASET lpdatasetResult,
                                LPDATASET lpdatasetData,
                                WORD      nPeriod );

BOOL FAR PASCAL _export Highest( LPDATASET lpdatasetResult,
                                 LPDATASET lpdatasetData,
                                 WORD      nPeriod );

BOOL FAR PASCAL _export StdDev( LPDATASET lpdatasetResult,
                                LPDATASET lpdatasetData,
                                WORD      nPeriod );

BOOL FAR PASCAL _export NewMinimum( LPDATASET lpdatasetResult,
                                    LPDATASET lpdatasetData,
                                    WORD      nPeriod );

BOOL FAR PASCAL _export NewMaximum( LPDATASET lpdatasetResult,
                                    LPDATASET lpdatasetData,
                                    WORD      nPeriod );

BOOL FAR PASCAL _export Rate1( LPDATASET lpdatasetResult,
                               LPDATASET lpdatasetData,
                               WORD      nPeriod );

BOOL FAR PASCAL _export Rate2( LPDATASET lpdatasetResult,
                               LPDATASET lpdatasetData,
                               WORD      nPeriod );

BOOL FAR PASCAL _export Absolute( LPDATASET lpdatasetResult,
                                  LPDATASET lpdatasetData );
                                  
BOOL FAR PASCAL _export Shift( LPDATASET lpdatasetResult,
                               LPDATASET lpdatasetData,
                               WORD      nPeriod );

BOOL FAR PASCAL _export LinReg( LPDATASET lpdatasetResult,
                                LPDATASET lpdatasetData,
                                WORD      nPeriod );

BOOL FAR PASCAL _export Correl( LPDATASET lpdatasetResult,
                                LPDATASET lpdsX,
                                LPDATASET lpdsY,
                                WORD      nPeriod );

BOOL FAR PASCAL _export Rsi( LPDATASET lpdatasetResult, LPDATASET lpdatasetData, WORD nPeriod );

BOOL FAR PASCAL _export Vi( LPDATASET lpdsResult, 
                LPDATASET lpdsHi, 
                LPDATASET lpdsLo,
                LPDATASET lpdsCl,
                WORD      nPeriod );

BOOL FAR PASCAL _export Sar( LPDATASET lpdatasetResult, 
                             LPDATASET lpdatasetHi,
                             LPDATASET lpdatasetLo,     
                             NUM numAccel,
                             NUM numAccelMax );

BOOL FAR PASCAL _export Log( LPDATASET lpdsResult, LPDATASET lpdsData );
BOOL FAR PASCAL _export Exp( LPDATASET lpdsResult, LPDATASET lpdsData );
BOOL FAR PASCAL _export Power( LPDATASET lpdsResult, LPDATASET lpdsBase, LPDATASET lpdsExponent );
BOOL FAR PASCAL _export MinMax( LPDATASET lpdsResult, LPDATASET lpdsA, LPDATASET lpdsB, BOOL fMin );

NUM FAR PASCAL _export KFromPeriod( WORD nPeriod );
WORD FAR PASCAL _export PeriodFromK( NUM numK );

#ifdef __cplusplus
};
#endif

#endif
