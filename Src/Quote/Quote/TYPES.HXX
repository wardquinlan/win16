//////////////////////////////////////////////////////////////////////////////
// TYPES.HXX
//
// This file contains basic type definitions and #define's
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _TYPES_HXX
#define _TYPES_HXX

typedef unsigned short USHORT;
typedef unsigned long  ULONG;

#define NUM_MAX       ( FLT_MAX )

#define _cViewLayouts    ( 4 )          // number of views
#define _nWindowMenu     ( 5 )          // position of Window menu
#define _cbBufferMax     ( 256 )        // generic buffer size
#define _cbNameMax       ( 16 )         // Names maximum
#define _cbTitleMax      ( 64 )         // View Title Max
#define _cbPathMax       ( 128 )        // Path maximum

#ifdef T_TX

#define _cbTextMax       ( 32 )         // Text line maximum

#else

#define _cbTextMax       ( 256 )        // Text line maximum

#endif

#define _cbFuncNameMax   ( 64 )         // Function 'Indicator name' maximums
#define _cbFloatMax      ( 32 )         // Floating point max (ASCII)

#endif
