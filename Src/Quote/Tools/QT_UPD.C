//////////////////////////////////////////////////////////////////////////////
// QT_UPD.C - Quote Update Tool
//
// This computer program is copyright (c) Ward Quinlan, 1996, 1997
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous type definitions and global variables
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <time.h>
#include <assert.h>
#include "except.h"

typedef float NUM;
typedef int BOOL;
#define FALSE 0
#define TRUE  1
#define TASKLIST do
#define ENDTASKLIST while ( 0 );

#define _cbTickerMax       16   // maximum ticker name length
#define _cbEnvironmentMax 128   // maximum environment string length
#define _cbLineMax        128   // line length maximum
#define _cbBufferMax      1024
#define _cbPathMax        _MAX_PATH // from stdlib.h

char _szBuffer[ _cbBufferMax ];

char _szHistoryFormat   [ _cbEnvironmentMax + 1 ];
char _szUpdateFormat    [ _cbEnvironmentMax + 1 ];
char _szDelimitFormat   [ _cbEnvironmentMax + 1 ];
char _szNewHistoryFormat[ _cbEnvironmentMax + 1 ];
char _szUpdateFile      [ _cbPathMax        + 1 ];
char _szDate            [ _cbPathMax        + 1 ];
char _szSplitDate       [ _cbPathMax        + 1 ];
char _szRemoveDate      [ _cbPathMax        + 1 ];
BOOL _fDate           = FALSE;
BOOL _fSplit          = FALSE;
BOOL _fQuiet          = FALSE;
BOOL _fOverWrite      = FALSE;
BOOL _fFastMode       = FALSE;
BOOL _fRemove         = FALSE;
BOOL _fConversionMode = FALSE;
BOOL _fSortHistoryFile    = FALSE;
BOOL _fReverseHistoryFile = FALSE;
BOOL _fIgnoreFirstLine    = FALSE;

int  _mSplit = 0;
int  _nSplit = 0;

const char _szFormatFloat[ ] = "%.4f";
const char _szFormatInt  [ ] = "%02d";
const char _szFormatYMD  [ ] = "%04d%02d%02d";
const char _szTmpFile    [ ] = "QT_TOOLS.TMP";
const char _szVersion    [ ] = "1.11.02";
const char _szREAD       [ ] = "rt";
const char _szWRITE      [ ] = "wt";
const char _szAPPEND     [ ] = "at";

const char _szOp[ ]  = "$op";
const char _szHi[ ]  = "$hi";
const char _szLo[ ]  = "$lo";
const char _szCl[ ]  = "$cl";
const char _szVl[ ]  = "$vl";
const char _szYr[ ]  = "$yr";
const char _szMn[ ]  = "$mn";
const char _szDy[ ]  = "$dy";
const char _szIg[ ]  = "$ig";
const char _szTk[ ]  = "$tk";
const char _szYmd[ ] = "$ymd";

//////////////////////////////////////////////////////////////////////////////
// History and Update file data records
//////////////////////////////////////////////////////////////////////////////
#define MASK_HI 0x0001
#define MASK_LO 0x0002
#define MASK_CL 0x0004
#define MASK_VL 0x0008
#define MASK_YR 0x0010
#define MASK_MN 0x0020
#define MASK_DY 0x0040
#define MASK_OP 0x0080
#define MASK_TK 0x0100
#define MASK_FULLHISTORY 0x00ff
#define MASK_FULLUPDATE  0x01ff

// Declare the history record
struct tagHistoryRecord
    {
    int year;
    int month;
    int day;
    
    NUM numOp;
    NUM numHi;
    NUM numLo;
    NUM numCl;
    NUM numVl;
    };
typedef struct tagHistoryRecord HistoryRecord;

// Declare the update record
struct tagUpdateRecord
    {
    char szTicker[ _cbTickerMax + 1 ];

    int year;
    int month;
    int day;
    
    NUM numOp;
    NUM numHi;
    NUM numLo;
    NUM numCl;
    NUM numVl;
    };
typedef struct tagUpdateRecord UpdateRecord;

// Declare the hash links
typedef struct tagHashLink
    {
    UpdateRecord        urec;
    BOOL                fPopulated;
    struct tagHashLink *next;
    };
typedef struct tagHashLink HashLink;

// Declare the hash table
#define HASHMAX 67
HashLink *rgHash[ HASHMAX ];

// Declare the global catch buffer
CATCHBUF _catchbuf; // declare global catch buffer

// Declare month string table
char *rgszMonths[] =
    {
    "jan",
    "feb",
    "mar",
    "apr",
    "may",
    "jun",
    "jul",
    "aug",
    "sep",
    "oct",
    "nov",
    "dec"
    };

//////////////////////////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////////////////////////
int  main                 ( int argc, char *argv[ ] );
void usage                ( void );
void ReadEnvironment      ( void );
void UpdateFile           ( char *szPath );
BOOL ReadHistoryRecord    ( FILE *pfile, HistoryRecord *prec, BOOL fIgnoreFirstLine, long *plLine );
BOOL ReadUpdateRecord     ( FILE *pfile, UpdateRecord *prec, BOOL fIgnoreFirstLine, long *plLine );
BOOL ReadLine             ( FILE *pfile, char *szLine, int cbLine, BOOL fIgnoreFirstLine, long *plLine );
BOOL ReadLine2            ( FILE *pfile, char *szLine, int cbLine, long *plLine );
void PostProcessString    ( char *pszDst, const char *pszSrc );
int  AdjustCentury        ( int year );
void ExtractTickerSymbol  ( char *pszTicker, int cbTicker, const char *pszPath );
void PopulateHashTable    ( void );
char *MapExceptionCode    ( int ex );
void WriteHistoryFile     ( char *szPath, UpdateRecord *prec );
void WriteFastHistoryFile ( char *szPath, UpdateRecord *prec );
int  CompareRecords       ( UpdateRecord *purec, HistoryRecord *phrec );
int  CompareHistoryRecords( HistoryRecord *phrec1, HistoryRecord *phrec2 );
void RemoveDate           ( char *szPath );
void Split                ( char *szPath );
void ConvertFile          ( char *szPath );
void CopyFile             ( const char *pszDst, const char *pszSrc );
void Pause                ( void );
void InitHashTable        ( void );
void DestroyHashTable     ( void );
void UpdateCallBack       ( char *szFullPath );
void RemoveCallBack       ( char *szFullPath );
void SplitCallBack        ( char *szFullPath );
void HashCallBack         ( char *szFullPath );
void ConversionCallBack   ( char *szFullPath );
void ArgsLoop             ( int argc, char *argv[ ], int iArg, void (*pfn)( char *szFullPath ) );
void AddEmptyHash         ( char *szTicker );
HashLink *FindHash        ( char *szTicker );
BOOL EmptyLine            ( char *szLine );
int SortCompare           ( HistoryRecord **pphrec1, HistoryRecord **pphrec2 );
void ConvertFileWithSort  ( char *szPath );
#ifdef HASHTEST
void DumpHash             ( void );
#endif

//////////////////////////////////////////////////////////////////////////////
// User-defined exception codes and strings
//////////////////////////////////////////////////////////////////////////////
#define exUsage       ( exUser + 0 ) // usage exception
#define exParameter   ( exUser + 1 ) // invalid parameters
#define exGeneric     ( exUser + 2 ) // generic exception
#define exEnvironment ( exUser + 3 ) // environment string exception
#define exFile        ( exUser + 4 ) // file i/o exception
#define exFormat      ( exUser + 5 ) // file format exception
#define exMemory      ( exUser + 6 ) // memory exception

typedef struct tagExceptionMap
    {
    int   ex;
    char *name;
    } ExceptionMap;
ExceptionMap rgMap[ ] =
    {
        { exUsage,       "usage"         },
        { exParameter,   "parameter"     },
        { exGeneric,     "generic"       },
        { exEnvironment, "environment"   },
        { exFile,        "file"          },
        { exFormat,      "file format"   },
        { exMemory,      "out of memory" }
    };

//////////////////////////////////////////////////////////////////////////////
// main( )
//
// Mainline
//////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[ ] )
    {
    char szDrive[ _MAX_DRIVE + 1 ];
    char szDir  [ _MAX_DIR   + 1 ];
    char szFName[ _MAX_FNAME + 1 ];
    char szExt  [ _MAX_EXT   + 1 ];
    static char szTmp[ _cbPathMax + 1 ]; // Don't want to overflow stack
    char *pchToken;
    
    BOOL   fRet   = FALSE;
    BOOL   fBreak = FALSE;
    struct find_t find;
    int    rc;
    int    i;
    int    cb;
    int    iArg;
    char   szPath[ _cbPathMax + 1 ];
    char  *pch;
    
    TRY
        {
        InitHashTable( );

#ifdef DEBUG
        fprintf( stderr, "DEBUG qt_upd: version %s\n", 
                 _szVersion );
#else
        fprintf( stderr, "qt_upd: version %s\n", 
                 _szVersion );
#endif
        for ( iArg = 1; argv[ iArg ][ 0 ] == '-'; iArg++ )
            {
            switch( argv[ iArg ][ 1 ] )
                {
            case 'i':
                _fIgnoreFirstLine = TRUE;
                break;

            case 'v':
                _fReverseHistoryFile = TRUE;
                break;

            case 'o':
                _fSortHistoryFile = TRUE;
                break;

            case 'c':
                _fConversionMode = TRUE;
                break;

            case 'f':
                _fFastMode = TRUE;
                break;

            case 'w':
                _fOverWrite = TRUE;
                break;

            case 'q':
                _fQuiet = TRUE;
                break;

            case 'r':
                if ( strlen( &argv[ iArg ][ 2 ] ) != 6 &&
                     strlen( &argv[ iArg ][ 2 ] ) != 8 )
                    {
                    usage( );
                    fBreak = TRUE;
                    break;
                    }
                strcpy( _szRemoveDate, &argv[ iArg ][ 2 ] );
                _fRemove = TRUE;
                break;

            case 'd':
                if ( strlen( &argv[ iArg ][ 2 ] ) != 6 &&
                     strlen( &argv[ iArg ][ 2 ] ) != 8 )
                    {
                    usage( );
                    fBreak = TRUE;
                    break;
                    }
                strcpy( _szDate, &argv[ iArg ][ 2 ] );
                _fDate = TRUE;
                break;

            case 's':
                memset( szTmp, 0, sizeof( szTmp ) );
                strcpy( szTmp, &argv[ iArg ][ 2 ] );
                pchToken = strtok( szTmp, ":" );
                if ( !pchToken || ( strlen( pchToken ) != 6 &&
                     strlen( pchToken ) != 8 ) )
                    {
                    usage( );
                    fBreak = TRUE;
                    break;
                    }
                strcpy( _szSplitDate, pchToken );
                
                pchToken = strtok( NULL, "-" );
                if ( !pchToken )
                    {
                    usage( );
                    fBreak = TRUE;
                    break;
                    }
                _mSplit = atoi( pchToken );

                pchToken = strtok( NULL, "" );
                if ( !pchToken )
                    {
                    usage( );
                    fBreak = TRUE;
                    break;
                    }
                _nSplit = atoi( pchToken );

                if ( _mSplit <= 0 || _nSplit <= 0 )
                    {
                    usage( );
                    fBreak = TRUE;
                    break;
                    }

                _fSplit = TRUE;
                break;
            
            case 'h':
            case '?':
            default :
                usage( );
                fBreak = TRUE;
                break;
                }
            }

        if ( fBreak )
            THROW_EXCEPTION( exUsage );

        if ( argc - iArg + 1 < 3 )
            {
            usage( );
            THROW_EXCEPTION( exUsage );
            }
            
        if ( _fRemove && _fSplit )
            {
            usage( );
            THROW_EXCEPTION( exUsage );
            }

        // Invarient: iArg will be pointing to update file
        if ( strlen( argv[ iArg ] ) > _cbPathMax )
            THROW_EXCEPTION( exParameter );

        strcpy( _szUpdateFile, argv[ iArg ] );

        ReadEnvironment( );
        
        iArg++;
        // Invarient: iArg will be pointing to 1st history file
        if ( _fRemove )
            {
            ArgsLoop( argc, argv, iArg, RemoveCallBack );
            }
        else if ( _fSplit )
            {
            ArgsLoop( argc, argv, iArg, SplitCallBack );
            }
        else if ( _fConversionMode )
            {
            ArgsLoop( argc, argv, iArg, ConversionCallBack );
            }
        else
            {
            fprintf( stderr, "qt_upd: initializing...\n" );
#ifdef HASHTEST
            fprintf( stderr, "Initialized Hash Table...\n" );
            DumpHash( );
#endif
            ArgsLoop( argc, argv, iArg, HashCallBack   );
#ifdef HASHTEST
            fprintf( stderr, "Empty Hash Table...\n" );
            DumpHash( );
#endif
            PopulateHashTable( );
            ArgsLoop( argc, argv, iArg, UpdateCallBack );
            }

        fRet = TRUE;
        }
    CATCH( exUsage )
        {
        // do nothing...
        }
    CATCH_ALL
        {
        fprintf( stderr, "qt_upd: error: %s\n",
                 MapExceptionCode( __ex ) );
        }
    END_CATCH

    DestroyHashTable( );
#ifdef HASHTEST
    fprintf( stderr, "Destroyed Hash Table...\n" );
    DumpHash( );
#endif
    return ( fRet ? 0 : 1 );
    }

//////////////////////////////////////////////////////////////////////////////
// UpdateCallBack( )
//
// This routine is called by ArgsLoop for each user path.
//////////////////////////////////////////////////////////////////////////////
void UpdateCallBack( char *szFullPath )
    {
    TRY
        {
        fprintf( stderr, "qt_upd: updating %s\n", szFullPath );
        UpdateFile( szFullPath );
        }
    CATCH( exFormat )
        {
        fprintf( stderr, "qt_upd: warning: formatting error while updating %s\n",
                 szFullPath );
        Pause( );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// ConversionCallBack( )
//
// This routine is called by ArgsLoop for each user path.
//////////////////////////////////////////////////////////////////////////////
void ConversionCallBack( char *szFullPath )
    {
    TRY
        {
        fprintf( stderr, "qt_upd: converting %s\n", szFullPath );
        if ( _fSortHistoryFile || _fReverseHistoryFile )
            ConvertFileWithSort( szFullPath );
        else
            ConvertFile( szFullPath );
        }
    CATCH( exFormat )
        {
        fprintf( stderr, "qt_upd: warning: formatting error while converting %s\n",
                 szFullPath );
        Pause( );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// SplitCallBack( )
//
// This routine is called by ArgsLoop for each user path.
//////////////////////////////////////////////////////////////////////////////
void SplitCallBack( char *szFullPath )
    {
    TRY
        {
        fprintf( stderr, "qt_upd: updating %s\n", szFullPath );
        Split( szFullPath );
        }
    CATCH( exFormat )
        {
        fprintf( stderr, "qt_upd: warning: formatting error while updating %s\n",
                 szFullPath );
        Pause( );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// RemoveCallBack( )
//
// This routine is called by ArgsLoop for each user path.
//////////////////////////////////////////////////////////////////////////////
void RemoveCallBack( char *szFullPath )
    {
    TRY
        {
        fprintf( stderr, "qt_upd: updating %s\n", szFullPath );
        RemoveDate( szFullPath );
        }
    CATCH( exFormat )
        {
        fprintf( stderr, "qt_upd: warning: formatting error while updating %s\n",
                 szFullPath );
        Pause( );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// HashCallBack( )
//
// This routine is called by ArgsLoop for each user path.
//////////////////////////////////////////////////////////////////////////////
void HashCallBack( char *szFullPath )
    {
    char szTicker[ _cbTickerMax + 1 ];
    
    ExtractTickerSymbol( szTicker, sizeof( szTicker ), szFullPath );
    AddEmptyHash( szTicker );
    }

//////////////////////////////////////////////////////////////////////////////
// usage( )
//
// This routine will display the help screen.
//////////////////////////////////////////////////////////////////////////////
void usage( void )
    {
    fprintf( stderr, "qt_upd: usage: qt_upd [options] update-file <history-file>*\n\n" );
    fprintf( stderr, "Options: -s[YY]YYMMDD:m-n splits file on [YY]YYMMDD m for n\n" );
    fprintf( stderr, "         -d[YY]YYMMDD     update file with [YY]YYMMDD\n" );
    fprintf( stderr, "         -r[YY]YYMMDD     removes [YY]YYMMDD entry from history file(s)\n" );
    fprintf( stderr, "         -f               fast mode\n" );
    fprintf( stderr, "         -q               quiet mode\n" );
    fprintf( stderr, "         -w               overwrite mode\n" );
    fprintf( stderr, "         -c               conversion mode\n" );
    fprintf( stderr, "         -i               ignore first history file line\n" );
    fprintf( stderr, "         -o               sort history files when converting\n" );
    fprintf( stderr, "         -v               reverse history files when converting\n" );
    fprintf( stderr, "         -h               print this screen\n" );
    }

//////////////////////////////////////////////////////////////////////////////
// MapExceptionCode( )
//
// This routine will return a string describing the exception code.
//
// Returns:
//     A text string describing the exception.
//////////////////////////////////////////////////////////////////////////////
char *MapExceptionCode( int ex )
    {
    int i;
    for ( i = 0; i < sizeof( rgMap ) / sizeof( rgMap[ 0 ] ); i++ )
        {
        if ( ex == rgMap[ i ].ex )
            return rgMap[ i ].name;
        }
    assert( FALSE );
    }

//////////////////////////////////////////////////////////////////////////////
// UpdateFile( )
//
// This routine updates one history file.
//////////////////////////////////////////////////////////////////////////////
void UpdateFile( char *szPath )
    {
    HashLink *plnk;
    char      szTicker[ _cbTickerMax + 1 ];

    TRY
        {
        ExtractTickerSymbol( szTicker, sizeof( szTicker ), szPath );
        
        // Since we've pre-built the hash table, szPath
        // should *always* be in it.
        plnk = FindHash( szTicker );
        assert( plnk != NULL );
        if ( plnk->fPopulated )
            {
            if ( _fFastMode )
                WriteFastHistoryFile( szPath, &plnk->urec );
            else
                WriteHistoryFile( szPath, &plnk->urec );
            }
        else
            {
            fprintf( stderr, "qt_upd: warning: no symbol for '%s' in update file\n", 
                     szTicker );
            Pause( );
            }
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// CompareHistoryRecords( )
//
// This routine compares two hrecs.
//
// Returns:
//     -1 if hrec1  < hrec2
//      0 if hrec1 == hrec2
//     +1 if hrec1  > hrec2
//////////////////////////////////////////////////////////////////////////////
int CompareHistoryRecords( HistoryRecord *phrec1, HistoryRecord *phrec2 )
    {
    struct tm tm1;
    struct tm tm2;

    time_t t1;
    time_t t2;

    memset( &tm1, 0, sizeof( tm1 ) );
    memset( &tm2, 0, sizeof( tm2 ) );

    tm1.tm_year = phrec1->year  - 1900;
    tm1.tm_mon  = phrec1->month - 1;
    tm1.tm_mday = phrec1->day;
    t1 = mktime( &tm1 );

    tm2.tm_year = phrec2->year  - 1900;
    tm2.tm_mon  = phrec2->month - 1;
    tm2.tm_mday = phrec2->day;
    t2 = mktime( &tm2 );

    if ( t1 < t2 )
        return -1;

    if ( t1 > t2 )
        return +1;

    return 0;
    }

//////////////////////////////////////////////////////////////////////////////
// CompareRecords( )
//
// This routine compares an hrec to a urec.
//
// Returns:
//     -1 if urec  < hrec
//      0 if urec == hrec
//     +1 if urec  > hrec
//////////////////////////////////////////////////////////////////////////////
int CompareRecords( UpdateRecord *purec, HistoryRecord *phrec )
    {
    struct tm tm1;
    struct tm tm2;

    time_t t1;
    time_t t2;

    memset( &tm1, 0, sizeof( tm1 ) );
    memset( &tm2, 0, sizeof( tm2 ) );

    tm1.tm_year = purec->year  - 1900;
    tm1.tm_mon  = purec->month - 1;
    tm1.tm_mday = purec->day;
    t1 = mktime( &tm1 );

    tm2.tm_year = phrec->year  - 1900;
    tm2.tm_mon  = phrec->month - 1;
    tm2.tm_mday = phrec->day;
    t2 = mktime( &tm2 );

    if ( t1 < t2 )
        return -1;

    if ( t1 > t2 )
        return +1;

    return 0;
    }

//////////////////////////////////////////////////////////////////////////////
// WriteRecord( )
//
// This routine will write one history record, according to pchFormat
//////////////////////////////////////////////////////////////////////////////
void WriteRecord( FILE *pfile, HistoryRecord *prec, char *pchFormat )
    {
    char *pch = pchFormat;

    while ( *pch )
        {
        if ( memcmp( pch, _szOp, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatFloat, prec->numOp ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        if ( memcmp( pch, _szHi, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatFloat, prec->numHi ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else if ( memcmp( pch, _szLo, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatFloat, prec->numLo ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else if ( memcmp( pch, _szCl, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatFloat, prec->numCl ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else if ( memcmp( pch, _szVl, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatFloat, prec->numVl ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else if ( memcmp( pch, _szYr, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatInt, prec->year ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else if ( memcmp( pch, _szMn, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatInt, prec->month ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else if ( memcmp( pch, _szDy, 3 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatInt, prec->day ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else if ( memcmp( pch, _szYmd, 4 ) == 0 )
            {
            if ( fprintf( pfile, _szFormatYMD, prec->year,
                                               prec->month, 
                                               prec->day ) < 0 )
                THROW_EXCEPTION( exFile );
            pch += 4;
            }
        else if ( memcmp( pch, _szIg, 3 ) == 0 )
            {
            if ( fputc( '0', pfile ) == EOF )
                THROW_EXCEPTION( exFile );
            pch += 3;
            }
        else
            {
            if ( fputc( *pch, pfile ) == EOF )
                THROW_EXCEPTION( exFile );
            pch++;
            }
        }
    fputc( '\n', pfile );
    }

//////////////////////////////////////////////////////////////////////////////
// WriteFastHistoryFile( )
//
// This routine will write the history file with the
// data contained in the update record.  Data is
// appended at the end, not inserted.
//////////////////////////////////////////////////////////////////////////////
void WriteFastHistoryFile( char *szPath, UpdateRecord *prec )
    {
    FILE *pfileDst = NULL;
    HistoryRecord hrecNew;

    TRY
        {
        memset( &hrecNew, 0, sizeof( hrecNew ) );
        hrecNew.year  = prec->year;
        hrecNew.month = prec->month;
        hrecNew.day   = prec->day;
        hrecNew.numOp = prec->numOp;
        hrecNew.numHi = prec->numHi;
        hrecNew.numLo = prec->numLo;
        hrecNew.numCl = prec->numCl;
        hrecNew.numVl = prec->numVl;

        pfileDst = fopen( szPath, _szAPPEND );
        if ( !pfileDst )
            THROW_EXCEPTION( exFile );

        WriteRecord( pfileDst, &hrecNew, _szHistoryFormat );

        fclose( pfileDst );
        pfileDst = NULL;
        }
    CATCH_ALL
        {
        if ( pfileDst )
            {
            fclose( pfileDst );
            pfileDst = NULL;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// SortCompare
//
// QSort comparison function
//////////////////////////////////////////////////////////////////////////////
int SortCompare( HistoryRecord **pphrec1, HistoryRecord **pphrec2 )
    {
    return CompareHistoryRecords( *pphrec1, *pphrec2 );
    }

//////////////////////////////////////////////////////////////////////////////
// ConvertFileWithSort( )
//
// This routine converts a file, as well as sorts it
//////////////////////////////////////////////////////////////////////////////
void ConvertFileWithSort( char *szPath )
    {
    FILE *pfileSrc = NULL;
    FILE *pfileDst = NULL;
    HistoryRecord hrec;
    HistoryRecord **pphrec = NULL;
    long ct = 0;
    long ctMax = 0;
    long line = 0;
    
    TRY
        {
        /////////////////////////////////////////////
        // Step 1: count how many records in the file
        /////////////////////////////////////////////
        memset( &hrec, 0, sizeof( hrec ) );
        
        pfileSrc = fopen( szPath, _szREAD );
        if ( !pfileSrc )
            THROW_EXCEPTION( exFile );

        line = 0;
        while ( TRUE )
            {
            if ( !ReadHistoryRecord( pfileSrc, &hrec, _fIgnoreFirstLine, &line ) )
                break;
            ctMax++;
            }
        fclose( pfileSrc );
        pfileSrc = NULL;

#ifdef DEBUG
        fprintf( stderr, "Pointer size = %d\n", (int) sizeof( HistoryRecord * ) );
        fprintf( stderr, "Temporary array elements = %ld\n", ctMax );
        fprintf( stderr, "Temporary array size = %ld\n", (long) ctMax * sizeof( HistoryRecord * ) );
#endif

        /////////////////////////////////////////////////////////////
        // Step 2: stick everything into a gigantic array of pointers
        /////////////////////////////////////////////////////////////
        pfileSrc = fopen( szPath, _szREAD );
        if ( !pfileSrc )
            THROW_EXCEPTION( exFile );
        
        if ( ctMax )
            {
            pphrec = (HistoryRecord **) malloc( (size_t) ctMax * sizeof( HistoryRecord * ) );
            if ( !pphrec )
                THROW_EXCEPTION( exMemory );
            }
        
        line = 0;
        while ( TRUE )
            {
            if ( !ReadHistoryRecord( pfileSrc, &hrec, _fIgnoreFirstLine, &line ) )
                break;

            assert( ct < ctMax );
            pphrec[ ct ] = (HistoryRecord *) malloc( sizeof( HistoryRecord ) );
            if ( !pphrec[ ct ] )
                THROW_EXCEPTION( exMemory );
            *( pphrec[ ct ] ) = hrec;
            ct++;
            }
        // Should be at the last element
        assert( ct == ctMax );
        fclose( pfileSrc );
        pfileSrc = NULL;

        /////////////////////////
        // Step 3: Sort the array
        /////////////////////////
        if ( pphrec && _fSortHistoryFile )
            qsort( pphrec, (size_t) ctMax, sizeof( HistoryRecord * ), SortCompare );

        //////////////////////////////////////////////
        // Step 4: write the array back to disk
        //////////////////////////////////////////////
        pfileDst = fopen( _szTmpFile, _szWRITE );
        if ( !pfileDst )
            THROW_EXCEPTION( exFile );
        
        if ( _fSortHistoryFile )
        {
            for ( ct = 0; ct < ctMax; ct++ )
                {
                WriteRecord( pfileDst, pphrec[ ct ], _szNewHistoryFormat );

                // Now that we're done with it, let's free it
                free( pphrec[ ct ] );
                pphrec[ ct ] = NULL;
                }
        }
        else
        {
            for ( ct = ctMax - 1; ct >= 0; ct-- )
                {
                WriteRecord( pfileDst, pphrec[ ct ], _szNewHistoryFormat );

                // Now that we're done with it, let's free it
                free( pphrec[ ct ] );
                pphrec[ ct ] = NULL;
                }
        }
            
        fclose( pfileDst );
        pfileDst = NULL;

        /////////////////////////
        // Step 5: Free the array
        /////////////////////////
        if ( pphrec )
            free( pphrec );
        pphrec = NULL;
        
        ////////////////////////
        // Step 6: Copy the file
        ////////////////////////

        // we must copy file to prevent _dos_find( ) functions from screwing up
        CopyFile( szPath, _szTmpFile );

#ifndef DEBUG
        if ( unlink( _szTmpFile ) != 0 )
            THROW_EXCEPTION( exFile );
#endif
        }
    CATCH_ALL
        {
        if ( pfileSrc )
            {
            fclose( pfileSrc );
            pfileSrc = NULL;
            }
        if ( pfileDst )
            {
            fclose( pfileDst );
            pfileDst = NULL;
            }
#ifndef DEBUG
        // Try our best to delete temporary file
        unlink( _szTmpFile );
#endif
        THROW_LAST( );
        }
    END_CATCH
    }
//////////////////////////////////////////////////////////////////////////////
// ConvertFile( )
//
// This routine converts a file
//////////////////////////////////////////////////////////////////////////////
void ConvertFile( char *szPath )
    {
    FILE *pfileSrc = NULL;
    FILE *pfileDst = NULL;
    HistoryRecord hrec;
    long line = 0;
    
    TRY
        {
        memset( &hrec, 0, sizeof( hrec ) );
        
        pfileSrc = fopen( szPath, _szREAD );
        if ( !pfileSrc )
            THROW_EXCEPTION( exFile );

        pfileDst = fopen( _szTmpFile, _szWRITE );
        if ( !pfileDst )
            THROW_EXCEPTION( exFile );
        
        while ( TRUE )
            {
            if ( !ReadHistoryRecord( pfileSrc, &hrec, _fIgnoreFirstLine, &line ) )
                break;
            
            WriteRecord( pfileDst, &hrec, _szNewHistoryFormat );
            }

        fclose( pfileSrc );
        fclose( pfileDst );
        pfileSrc = NULL;
        pfileDst = NULL;
        
        // we must copy file to prevent _dos_find( ) functions from screwing up
        CopyFile( szPath, _szTmpFile );

#ifndef DEBUG
        if ( unlink( _szTmpFile ) != 0 )
            THROW_EXCEPTION( exFile );
#endif
        }
    CATCH_ALL
        {
        if ( pfileSrc )
            {
            fclose( pfileSrc );
            pfileSrc = NULL;
            }
        if ( pfileDst )
            {
            fclose( pfileDst );
            pfileDst = NULL;
            }
#ifndef DEBUG
        // Try our best to delete temporary file
        unlink( _szTmpFile );
#endif
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// Split( )
//
// This routine Splits files
//////////////////////////////////////////////////////////////////////////////
void Split( char *szPath )
    {
    long line = 0;
    BOOL fFound = FALSE;
    char szTwoDigits [ 3 ];
    char szFourDigits[ 5 ];

    FILE *pfileSrc = NULL;
    FILE *pfileDst = NULL;
    
    HistoryRecord hrec1;
    HistoryRecord hrec2;
    HistoryRecord hrecSplit;

    TRY
        {
        memset( &hrec1, 0, sizeof( hrec1 ) );
        memset( &hrec2, 0, sizeof( hrec2 ) );
        memset( &hrecSplit, 0, sizeof( hrecSplit ) );

        if ( strlen( _szSplitDate ) == 6 )
            {
            szTwoDigits[ 0 ] = *( _szSplitDate + 0 );
            szTwoDigits[ 1 ] = *( _szSplitDate + 1 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.year = AdjustCentury( atoi( szTwoDigits ) );
                
            szTwoDigits[ 0 ] = *( _szSplitDate + 2 );
            szTwoDigits[ 1 ] = *( _szSplitDate + 3 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.month = atoi( szTwoDigits );

            szTwoDigits[ 0 ] = *( _szSplitDate + 4 );
            szTwoDigits[ 1 ] = *( _szSplitDate + 5 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.day = atoi( szTwoDigits );
            }
        else if ( strlen( _szSplitDate ) == 8 )
            {
            szFourDigits[ 0 ] = *( _szSplitDate + 0 );
            szFourDigits[ 1 ] = *( _szSplitDate + 1 );
            szFourDigits[ 2 ] = *( _szSplitDate + 2 );
            szFourDigits[ 3 ] = *( _szSplitDate + 3 );
            szFourDigits[ 4 ] = '\0';
            hrec1.year = AdjustCentury( atoi( szFourDigits ) );
                
            szTwoDigits[ 0 ] = *( _szSplitDate + 4 );
            szTwoDigits[ 1 ] = *( _szSplitDate + 5 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.month = atoi( szTwoDigits );

            szTwoDigits[ 0 ] = *( _szSplitDate + 6 );
            szTwoDigits[ 1 ] = *( _szSplitDate + 7 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.day = atoi( szTwoDigits );
            }
        else
            {
            fprintf( stderr, "qt_upd: unrecognized date format\n" );
            THROW_EXCEPTION( exFormat );
            }

        pfileSrc = fopen( szPath, _szREAD );
        if ( !pfileSrc )
            THROW_EXCEPTION( exFile );

        pfileDst = fopen( _szTmpFile, _szWRITE );
        if ( !pfileDst )
            THROW_EXCEPTION( exFile );
        
        while ( TRUE )
            {
            if ( !ReadHistoryRecord( pfileSrc, &hrec2, _fIgnoreFirstLine, &line ) )
                break;
            
            if ( CompareHistoryRecords( &hrec2, &hrec1 ) < 0 )
                {
                // split the file
                memset( &hrecSplit, 0, sizeof( hrecSplit ) );
                hrecSplit.year  = hrec2.year;
                hrecSplit.month = hrec2.month;
                hrecSplit.day   = hrec2.day;
                hrecSplit.numOp = hrec2.numOp * ( (NUM) _nSplit ) / ( (NUM) _mSplit );
                hrecSplit.numHi = hrec2.numHi * ( (NUM) _nSplit ) / ( (NUM) _mSplit );
                hrecSplit.numLo = hrec2.numLo * ( (NUM) _nSplit ) / ( (NUM) _mSplit );
                hrecSplit.numCl = hrec2.numCl * ( (NUM) _nSplit ) / ( (NUM) _mSplit );
                hrecSplit.numVl = hrec2.numVl * ( (NUM) _mSplit ) / ( (NUM) _nSplit );
                WriteRecord( pfileDst, &hrecSplit, _szHistoryFormat );
                fFound = TRUE;
                }
            else
                {
                WriteRecord( pfileDst, &hrec2, _szHistoryFormat );
                }
            }

        fclose( pfileSrc );
        fclose( pfileDst );
        pfileSrc = NULL;
        pfileDst = NULL;
        
        if ( fFound )
            {
            // we must copy file to prevent _dos_find( ) functions from screwing up
            CopyFile( szPath, _szTmpFile );

#ifndef DEBUG
            if ( unlink( _szTmpFile ) != 0 )
                THROW_EXCEPTION( exFile );
#endif
            }
        else
            {
#ifndef DEBUG
            if ( unlink( _szTmpFile ) != 0 )
                THROW_EXCEPTION( exFile );
#endif
            fprintf( stderr, "qt_upd: warning: date not found in history file\n" );
            Pause( );
            }
        }
    CATCH_ALL
        {
        if ( pfileSrc )
            {
            fclose( pfileSrc );
            pfileSrc = NULL;
            }
        if ( pfileDst )
            {
            fclose( pfileDst );
            pfileDst = NULL;
            }
#ifndef DEBUG
        // Try our best to delete temporary file
        unlink( _szTmpFile );
#endif
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// RemoveDate( )
//
// This routine removes _szRemoveDate from szPath.
//////////////////////////////////////////////////////////////////////////////
void RemoveDate( char *szPath )
    {
    long line = 0;
    BOOL fFound = FALSE;
    char szTwoDigits [ 3 ];
    char szFourDigits[ 5 ];

    FILE *pfileSrc = NULL;
    FILE *pfileDst = NULL;
    
    HistoryRecord hrec1;
    HistoryRecord hrec2;

    TRY
        {
        memset( &hrec1, 0, sizeof( hrec1 ) );
        memset( &hrec2, 0, sizeof( hrec2 ) );

        if ( strlen( _szRemoveDate ) == 6 )
            {
            szTwoDigits[ 0 ] = *( _szRemoveDate + 0 );
            szTwoDigits[ 1 ] = *( _szRemoveDate + 1 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.year = AdjustCentury( atoi( szTwoDigits ) );
                
            szTwoDigits[ 0 ] = *( _szRemoveDate + 2 );
            szTwoDigits[ 1 ] = *( _szRemoveDate + 3 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.month = atoi( szTwoDigits );

            szTwoDigits[ 0 ] = *( _szRemoveDate + 4 );
            szTwoDigits[ 1 ] = *( _szRemoveDate + 5 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.day = atoi( szTwoDigits );
            }
        else if ( strlen( _szRemoveDate ) == 8 )
            {
            szFourDigits[ 0 ] = *( _szRemoveDate + 0 );
            szFourDigits[ 1 ] = *( _szRemoveDate + 1 );
            szFourDigits[ 2 ] = *( _szRemoveDate + 2 );
            szFourDigits[ 3 ] = *( _szRemoveDate + 3 );
            szFourDigits[ 4 ] = '\0';
            hrec1.year = AdjustCentury( atoi( szFourDigits ) );
                
            szTwoDigits[ 0 ] = *( _szRemoveDate + 4 );
            szTwoDigits[ 1 ] = *( _szRemoveDate + 5 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.month = atoi( szTwoDigits );

            szTwoDigits[ 0 ] = *( _szRemoveDate + 6 );
            szTwoDigits[ 1 ] = *( _szRemoveDate + 7 );
            szTwoDigits[ 2 ] = '\0';
            hrec1.day = atoi( szTwoDigits );
            }
        else
            {
            fprintf( stderr, "qt_upd: unrecognized date format\n" );
            THROW_EXCEPTION( exFormat );
            }

        pfileSrc = fopen( szPath, _szREAD );
        if ( !pfileSrc )
            THROW_EXCEPTION( exFile );

        pfileDst = fopen( _szTmpFile, _szWRITE );
        if ( !pfileDst )
            THROW_EXCEPTION( exFile );
        
        while ( TRUE )
            {
            if ( !ReadHistoryRecord( pfileSrc, &hrec2, _fIgnoreFirstLine, &line ) )
                break;
            
            if ( CompareHistoryRecords( &hrec1, &hrec2 ) == 0 )
                {
                // suppress writing the record
                fFound = TRUE;
                }
            else
                {
                WriteRecord( pfileDst, &hrec2, _szHistoryFormat );
                }
            }

        fclose( pfileSrc );
        fclose( pfileDst );
        pfileSrc = NULL;
        pfileDst = NULL;
        
        if ( fFound )
            {
            // we must copy file to prevent _dos_find( ) functions from screwing up
            CopyFile( szPath, _szTmpFile );

#ifndef DEBUG
            if ( unlink( _szTmpFile ) != 0 )
                THROW_EXCEPTION( exFile );
#endif
            }
        else
            {
#ifndef DEBUG
            if ( unlink( _szTmpFile ) != 0 )
                THROW_EXCEPTION( exFile );
#endif
            fprintf( stderr, "qt_upd: warning: date not found in history file\n" );
            Pause( );
            }
        }
    CATCH_ALL
        {
        if ( pfileSrc )
            {
            fclose( pfileSrc );
            pfileSrc = NULL;
            }
        if ( pfileDst )
            {
            fclose( pfileDst );
            pfileDst = NULL;
            }
#ifndef DEBUG
        // Try our best to delete temporary file
        unlink( _szTmpFile );
#endif
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// WriteHistoryFile( )
//
// This routine will write the history file with the data
// contained in the update record.  Data is inserted
// into the file as necessary.
//////////////////////////////////////////////////////////////////////////////
void WriteHistoryFile( char *szPath, UpdateRecord *prec )
    {
    long line = 0;
    FILE *pfileSrc = NULL;
    FILE *pfileDst = NULL;
    
    HistoryRecord hrec;
    HistoryRecord hrecNew;
    int n;
    BOOL fWritten = FALSE;

    TRY
        {
        memset( &hrec,    0, sizeof( hrec ) );
        memset( &hrecNew, 0, sizeof( hrecNew ) );
        hrecNew.year  = prec->year;
        hrecNew.month = prec->month;
        hrecNew.day   = prec->day;
        hrecNew.numOp = prec->numOp;
        hrecNew.numHi = prec->numHi;
        hrecNew.numLo = prec->numLo;
        hrecNew.numCl = prec->numCl;
        hrecNew.numVl = prec->numVl;

        pfileSrc = fopen( szPath, _szREAD );
        if ( !pfileSrc )
            THROW_EXCEPTION( exFile );

        pfileDst = fopen( _szTmpFile, _szWRITE );
        if ( !pfileDst )
            THROW_EXCEPTION( exFile );
        
        while ( TRUE )
            {
            if ( !ReadHistoryRecord( pfileSrc, &hrec, _fIgnoreFirstLine, &line ) )
                break;
            
            n = CompareRecords( prec, &hrec );

            if ( n < 0 && !fWritten )
                {
                WriteRecord( pfileDst, &hrecNew, _szHistoryFormat );
                WriteRecord( pfileDst, &hrec, _szHistoryFormat );
                fWritten = TRUE;
                }
            else if ( n == 0 )
                {
                if ( _fOverWrite )
                    {
                    fprintf( stderr, "qt_upd: warning: record exists; over-writing\n" );
                    Pause( );
                    WriteRecord( pfileDst, &hrecNew, _szHistoryFormat );
                    }
                else
                    {
                    fprintf( stderr, "qt_upd: warning: record exists; ignoring\n" );
                    Pause( );
                    WriteRecord( pfileDst, &hrec, _szHistoryFormat );
                    }
                fWritten = TRUE;
                }
            else
                {
                WriteRecord( pfileDst, &hrec, _szHistoryFormat );
                }
            }
        
        if ( !fWritten )
            {
            WriteRecord( pfileDst, &hrecNew, _szHistoryFormat );
            }

        fclose( pfileSrc );
        fclose( pfileDst );
        pfileSrc = NULL;
        pfileDst = NULL;
        
        CopyFile( szPath, _szTmpFile );
#ifndef DEBUG
        if ( unlink( _szTmpFile ) != 0 )
            THROW_EXCEPTION( exFile );
#endif
        }
    CATCH_ALL
        {
        if ( pfileSrc )
            {
            fclose( pfileSrc );
            pfileSrc = NULL;
            }
        if ( pfileDst )
            {
            fclose( pfileDst );
            pfileDst = NULL;
            }
#ifndef DEBUG
        unlink( _szTmpFile ); // try our best to delete temp file
#endif
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// CopyFile( )
//
// This routine will copy a file from pszSrc to pszDst.
//////////////////////////////////////////////////////////////////////////////
void CopyFile( const char *pszDst, const char *pszSrc )
    {
    BOOL fEOF = FALSE;
    size_t cb;
    size_t cb2;
    FILE *pfileSrc = NULL;
    FILE *pfileDst = NULL;

    TRY
        {
        pfileSrc = fopen( pszSrc, _szREAD );
        if ( !pfileSrc )
            THROW_EXCEPTION( exFile );

        pfileDst = fopen( pszDst, _szWRITE );
        if ( !pfileDst )
            THROW_EXCEPTION( exFile );

        while ( !fEOF )
            {
            cb = fread( _szBuffer, 1, _cbBufferMax, pfileSrc );
            if ( cb != _cbBufferMax )
                {
                if ( ferror( pfileSrc ) )
                    THROW_EXCEPTION( exFile );
                fEOF = TRUE;
                }

            cb2 = fwrite( _szBuffer, 1, cb, pfileDst );
            if ( cb2 != cb )
                THROW_EXCEPTION( exFile );
            }

        fclose( pfileSrc );
        fclose( pfileDst );
        pfileSrc = NULL;
        pfileDst = NULL;
        }
    CATCH_ALL
        {
        if ( pfileSrc )
            {
            fclose( pfileSrc );
            pfileSrc = NULL;
            }
        if ( pfileDst )
            {
            fclose( pfileDst );
            pfileDst = NULL;
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// ExtractTickerSymbol( )
//
// This routine extracts a ticker symbol from a path.
//
// Note: Underscores          --> .
//       Tilde                --> *
//       Reverse single quote --> ?
//////////////////////////////////////////////////////////////////////////////
void ExtractTickerSymbol( char *pszTicker, int cbTicker, const char *pszPath )
    {
    char *pch;
    char szDrive[ _MAX_DRIVE + 1 ];
    char szDir  [ _MAX_DIR   + 1 ];
    char szFName[ _MAX_FNAME + 1 ];
    char szExt  [ _MAX_EXT   + 1 ];

    _splitpath( pszPath, szDrive, szDir, szFName, szExt );
    if ( strlen( szFName ) > cbTicker )
        THROW_EXCEPTION( exGeneric );
    strcpy( pszTicker, szFName );

    while ( pch = strchr( pszTicker, '_' ) ) 
        {
        *pch = '.';
        }
    
    while ( pch = strchr( pszTicker, '~' ) ) 
        {
        *pch = '*';
        }

    while ( pch = strchr( pszTicker, '`' ) )
        {
        *pch = '?';
        }
    }

//////////////////////////////////////////////////////////////////////////////
// PopulateHashTable( )
//
// This routine will read the update file,
// and populate the hash table.
//////////////////////////////////////////////////////////////////////////////
void PopulateHashTable( void )
    {
    long line = 0;
    HashLink     *plnk;
    UpdateRecord  urec;
    FILE         *pfile = NULL;
    int           iHash;
    
    TRY
        {
        memset( &urec, 0, sizeof( urec ) );

        pfile = fopen( _szUpdateFile, _szREAD );
        if ( !pfile )
            THROW_EXCEPTION( exFile );

        while ( TRUE )
            {
            if ( !ReadUpdateRecord( pfile, &urec, FALSE, &line ) )
                break;
            
            // If we find plnk in the hash table, then
            // we are interested in it.  If not,
            // we are not interested in updating it.
            plnk = FindHash( urec.szTicker );
            if ( plnk )
                {
                // If the hash table entry is already populated,
                // then there must be duplicate entries
                // in the update file.
                if ( plnk->fPopulated )
                    {
                    fprintf( stderr, "qt_upd: warning: duplicate entry for '%s' in update file; ignoring\n",
                         urec.szTicker );
                    Pause( );
                    }
                else
                    {
                    // if found, fill in the hash table entry
                    memcpy( &plnk->urec, &urec, sizeof( UpdateRecord ) );
                    plnk->fPopulated = TRUE;
                    }
                }
            }
        }
    CATCH_ALL
        {
        if ( pfile )
            {
            fclose( pfile );
            pfile = NULL;
            }
        THROW_LAST( );
        }
    END_CATCH
    
    if ( pfile )
        {
        fclose( pfile );
        pfile = NULL;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// ReadEnvironment( )
//
// This routine will read the environment strings:
//
// QT_UPDATE  - The file format of the update file
// QT_HISTORY - The file format of the history file
// QT_DELIMIT - (optional) The delimiter string
//////////////////////////////////////////////////////////////////////////////
void ReadEnvironment( void )
    {
    char *pchHistory;
    char *pchUpdate;
    char *pchDelimit;
    char *pchNewHistory;

    pchHistory    = getenv( "QT_HISTORY" );
    pchUpdate     = getenv( "QT_UPDATE"  );
    pchDelimit    = getenv( "QT_DELIMIT" );
    pchNewHistory = getenv( "QT_NEWHISTORY" );

    if ( !pchHistory || !pchUpdate )
        THROW_EXCEPTION( exEnvironment );
            
    if ( _fConversionMode && !pchNewHistory )
        THROW_EXCEPTION( exEnvironment );

    if ( !pchDelimit )
        pchDelimit = ",\t";
    if ( strlen( pchHistory ) > _cbEnvironmentMax )
        THROW_EXCEPTION( exGeneric );
    if ( strlen( pchUpdate )  > _cbEnvironmentMax )
        THROW_EXCEPTION( exGeneric );
    if ( strlen( pchDelimit ) > _cbEnvironmentMax )
        THROW_EXCEPTION( exGeneric );
    if ( strlen( pchNewHistory ) > _cbEnvironmentMax )
        THROW_EXCEPTION( exGeneric );

    PostProcessString( _szHistoryFormat,    pchHistory );
    PostProcessString( _szUpdateFormat ,    pchUpdate  );
    PostProcessString( _szDelimitFormat,    pchDelimit );
    PostProcessString( _szNewHistoryFormat, pchNewHistory );
    }

//////////////////////////////////////////////////////////////////////////////
// PostProcessString( )
//
// This routine translates any \t and \b characters to
// tab and blanks, respectively.
//////////////////////////////////////////////////////////////////////////////
void PostProcessString( char *pchDst, const char *pchSrc )
    {
    while ( *pchSrc )
        {
        if ( *pchSrc == '\\' && *( pchSrc + 1 ) == 't' )
            {
            *pchDst = '\t';
            pchDst++;
            pchSrc += 2;
            }
        else if ( *pchSrc == '\\' && *( pchSrc + 1 ) == 'b' )
            {
            *pchDst = ' ';
            pchDst++;
            pchSrc += 2;
            }
        else
            {
            *pchDst = *pchSrc;
            pchDst++;
            pchSrc++;
            }
        }
    }

//////////////////////////////////////////////////////////////////////////////
// EmptyLine( )
//
// Returns TRUE if an entire string is composed of whitespace characters
//////////////////////////////////////////////////////////////////////////////
BOOL EmptyLine( char *szLine )
{
    char *pch;

    for ( pch = szLine; *pch; pch++ )
    {
        if ( !isspace( *pch ) )
            return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// ReadLine2( )
//
// This routine reads one line from a file, ignoring whitespace lines
//////////////////////////////////////////////////////////////////////////////
BOOL ReadLine2( FILE *pfile, char *szLine, int cbLine, long *plLine )
    {
    BOOL fRet = FALSE;
    char *pch;

    while ( pch = fgets( szLine, cbLine, pfile ) )
    {
        ( *plLine )++;
        if ( !EmptyLine( szLine ) )
            break;
    }
    
    if ( !pch )
        {
        if ( ferror( pfile ) )
            THROW_EXCEPTION( exFile );
        else
            return FALSE;
        }

    pch = strchr( szLine, '\n' );
    if ( !pch )
        THROW_EXCEPTION( exGeneric );
    
    *pch = '\0';
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// ReadLine( )
//
// This routine reads one line from a file, skipping the first line
// if necessary
//
// Returns:
//     TRUE  - read successful
//     FALSE - end-of-file
//////////////////////////////////////////////////////////////////////////////
BOOL ReadLine( FILE *pfile, char *szLine, int cbLine, BOOL fIgnoreFirstLine, long *plLine )
    {
    if ( !ReadLine2( pfile, szLine, cbLine, plLine ) )
        return FALSE;
    
    if ( fIgnoreFirstLine && *plLine == 1 )
        {
        if ( !ReadLine2( pfile, szLine, cbLine, plLine ) )
            return FALSE;
        }

    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// ReadHistoryRecord( )
//
// This routine reads one history record according to the
// environment formatting strings.
//
// Returns:
//     TRUE  - on successful read
//     FALSE - on end-of-file
//////////////////////////////////////////////////////////////////////////////
BOOL ReadHistoryRecord( FILE *pfile, HistoryRecord *prec, BOOL fIgnoreFirstLine, long *plLine )
    {
    char *pchToken;
    char *pchFormat;

    unsigned mask = 0u;
    char szLine[ _cbLineMax + 1 ];
    BOOL fRet = FALSE;
    
    TASKLIST
        {
        // Initialize all fields to 0
        memset( prec, 0, sizeof( *prec ) );

        if ( !ReadLine( pfile, szLine, sizeof( szLine ), fIgnoreFirstLine, plLine ) )
            break; // end-of-file
            
        pchFormat = _szHistoryFormat;
        pchToken = strtok( szLine, _szDelimitFormat );
        while ( pchToken && *pchFormat )
            {
            if ( memcmp( pchFormat, _szOp, strlen( _szOp ) ) == 0 )
                {
                if ( mask & MASK_OP )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numOp = atof( pchToken );
                pchFormat += strlen( _szOp );
                mask |= MASK_OP; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szHi, strlen( _szHi ) ) == 0 )
                {
                if ( mask & MASK_HI )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numHi = atof( pchToken );
                pchFormat += strlen( _szHi );
                mask |= MASK_HI; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szLo, strlen( _szLo ) ) == 0 )
                {
                if ( mask & MASK_LO )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numLo = atof( pchToken );
                pchFormat += strlen( _szLo );
                mask |= MASK_LO; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szCl, strlen( _szCl ) ) == 0 )
                {
                if ( mask & MASK_CL )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numCl = atof( pchToken );
                pchFormat += strlen( _szCl );
                mask |= MASK_CL; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szVl, strlen( _szVl ) ) == 0 )
                {
                if ( mask & MASK_VL )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numVl = atof( pchToken );
                pchFormat += strlen( _szVl );
                mask |= MASK_VL; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szYmd, strlen( _szYmd ) ) == 0 )
                {
                char szTwoDigits [ 3 ];
                char szFourDigits[ 5 ];

                if ( ( mask & MASK_YR ) || ( mask & MASK_MN ) || ( mask & MASK_DY ) )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                
                if ( strlen( pchToken ) == 6 )
                    {
                    szTwoDigits[ 0 ] = *( pchToken + 0 );
                    szTwoDigits[ 1 ] = *( pchToken + 1 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->year = AdjustCentury( atoi( szTwoDigits ) );
                
                    szTwoDigits[ 0 ] = *( pchToken + 2 );
                    szTwoDigits[ 1 ] = *( pchToken + 3 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->month = atoi( szTwoDigits );

                    szTwoDigits[ 0 ] = *( pchToken + 4 );
                    szTwoDigits[ 1 ] = *( pchToken + 5 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->day = atoi( szTwoDigits );
                    }
                else if ( strlen( pchToken ) == 8 )
                    {
                    szFourDigits[ 0 ] = *( pchToken + 0 );
                    szFourDigits[ 1 ] = *( pchToken + 1 );
                    szFourDigits[ 2 ] = *( pchToken + 2 );
                    szFourDigits[ 3 ] = *( pchToken + 3 );
                    szFourDigits[ 4 ] = '\0';
                    prec->year = AdjustCentury( atoi( szFourDigits ) );
                
                    szTwoDigits[ 0 ] = *( pchToken + 4 );
                    szTwoDigits[ 1 ] = *( pchToken + 5 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->month = atoi( szTwoDigits );

                    szTwoDigits[ 0 ] = *( pchToken + 6 );
                    szTwoDigits[ 1 ] = *( pchToken + 7 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->day = atoi( szTwoDigits );
                    }
                else
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }

                pchFormat += strlen( _szYmd );
                mask |= MASK_YR; 
                mask |= MASK_MN;
                mask |= MASK_DY;
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szYr, strlen( _szYr ) ) == 0 )
                {
                if ( mask & MASK_YR )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->year = AdjustCentury( atoi( pchToken ) );
                pchFormat += strlen( _szYr );
                mask |= MASK_YR; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szMn, strlen( _szMn ) ) == 0 )
                {
                if ( mask & MASK_MN )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                if ( isdigit( *pchToken ) )
                    prec->month = atoi( pchToken );
                else    
                    {
                    prec->month = MonthFromString( pchToken );
                    if ( prec->month == 0 )
                        {
                        fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                        THROW_EXCEPTION( exFormat );
                        }
                    }
                pchFormat += strlen( _szMn );
                mask |= MASK_MN; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szDy, strlen( _szDy ) ) == 0 )
                {
                if ( mask & MASK_DY )
                    {
                    fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->day = atoi( pchToken );
                pchFormat += strlen( _szDy );
                mask |= MASK_DY; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szIg, strlen( _szIg ) ) == 0 )
                {
                pchFormat += strlen( _szIg );
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else
                {
                pchFormat++;
                }
            }
        
        // Optional attributes
        if ( !( mask & MASK_VL ) )
            mask |= MASK_VL;
        if ( !( mask & MASK_OP ) )
            mask |= MASK_OP;
        if ( !( mask & MASK_HI ) )
            mask |= MASK_HI;
        if ( !( mask & MASK_LO ) )
            mask |= MASK_LO;

        if ( mask != MASK_FULLHISTORY )
            {
            fprintf( stderr, "qt_upd: invalid history record [%ld]\n", *plLine );
            THROW_EXCEPTION( exFormat );
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// ReadUpdateRecord( )
//
// This routine reads one update record according to the
// environment formatting strings.
//
// Returns:
//     TRUE  - if a record was read
//     FALSE - on end-of-file
//////////////////////////////////////////////////////////////////////////////
BOOL ReadUpdateRecord( FILE *pfile, UpdateRecord *prec, BOOL fIgnoreFirstLine, long *plLine )
    {
    char *pchToken;
    char *pchFormat;

    unsigned mask = 0u;
    char szLine[ _cbLineMax + 1 ];
    char szTwoDigits [ 3 ];
    char szFourDigits[ 5 ];
    BOOL fRet = FALSE;
    
    TASKLIST
        {
        // Initialize all fields to 0
        memset( prec, 0, sizeof( *prec ) );

        if ( _fDate )
            {
            if ( strlen( _szDate ) == 6 )
                {
                szTwoDigits[ 0 ] = *( _szDate + 0 );
                szTwoDigits[ 1 ] = *( _szDate + 1 );
                szTwoDigits[ 2 ] = '\0';
                prec->year = AdjustCentury( atoi( szTwoDigits ) );
                
                szTwoDigits[ 0 ] = *( _szDate + 2 );
                szTwoDigits[ 1 ] = *( _szDate + 3 );
                szTwoDigits[ 2 ] = '\0';
                prec->month = atoi( szTwoDigits );

                szTwoDigits[ 0 ] = *( _szDate + 4 );
                szTwoDigits[ 1 ] = *( _szDate + 5 );
                szTwoDigits[ 2 ] = '\0';
                prec->day = atoi( szTwoDigits );

                mask |= MASK_YR;
                mask |= MASK_MN;
                mask |= MASK_DY;
                }
            else if ( strlen( _szDate ) == 8 )
                {
                szFourDigits[ 0 ] = *( _szDate + 0 );
                szFourDigits[ 1 ] = *( _szDate + 1 );
                szFourDigits[ 2 ] = *( _szDate + 2 );
                szFourDigits[ 3 ] = *( _szDate + 3 );
                szFourDigits[ 4 ] = '\0';
                prec->year = AdjustCentury( atoi( szFourDigits ) );
                
                szTwoDigits[ 0 ] = *( _szDate + 4 );
                szTwoDigits[ 1 ] = *( _szDate + 5 );
                szTwoDigits[ 2 ] = '\0';
                prec->month = atoi( szTwoDigits );

                szTwoDigits[ 0 ] = *( _szDate + 6 );
                szTwoDigits[ 1 ] = *( _szDate + 7 );
                szTwoDigits[ 2 ] = '\0';
                prec->day = atoi( szTwoDigits );

                mask |= MASK_YR;
                mask |= MASK_MN;
                mask |= MASK_DY;
                }
            else
                {
                fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                THROW_EXCEPTION( exFormat );
                }
            }

        if ( !ReadLine( pfile, szLine, sizeof( szLine ), fIgnoreFirstLine, plLine ) )
            break; // end-of-file
            
        pchFormat = _szUpdateFormat;
        pchToken = strtok( szLine, _szDelimitFormat );
        while ( pchToken && *pchFormat )
            {
            if ( memcmp( pchFormat, _szTk, strlen( _szTk ) ) == 0 )
                {
                if ( mask & MASK_TK )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                if ( strlen( pchToken ) > _cbTickerMax )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                strcpy( prec->szTicker, pchToken );
                pchFormat += strlen( _szTk );
                mask |= MASK_TK;
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szOp, strlen( _szOp ) ) == 0 )
                {
                if ( mask & MASK_OP )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numOp = atof( pchToken );
                pchFormat += strlen( _szOp );
                mask |= MASK_OP; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szHi, strlen( _szHi ) ) == 0 )
                {
                if ( mask & MASK_HI )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numHi = atof( pchToken );
                pchFormat += strlen( _szHi );
                mask |= MASK_HI; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szLo, strlen( _szLo ) ) == 0 )
                {
                if ( mask & MASK_LO )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numLo = atof( pchToken );
                pchFormat += strlen( _szLo );
                mask |= MASK_LO; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szCl, strlen( _szCl ) ) == 0 )
                {
                if ( mask & MASK_CL )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numCl = atof( pchToken );
                pchFormat += strlen( _szCl );
                mask |= MASK_CL; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szVl, strlen( _szVl ) ) == 0 )
                {
                if ( mask & MASK_VL )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->numVl = atof( pchToken );
                pchFormat += strlen( _szVl );
                mask |= MASK_VL; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szYmd, strlen( _szYmd ) ) == 0 )
                {
                if ( ( mask & MASK_YR ) || ( mask & MASK_MN ) || ( mask & MASK_DY ) )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                
                if ( strlen( pchToken ) == 6 )
                    {
                    szTwoDigits[ 0 ] = *( pchToken + 0 );
                    szTwoDigits[ 1 ] = *( pchToken + 1 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->year = AdjustCentury( atoi( szTwoDigits ) );
                
                    szTwoDigits[ 0 ] = *( pchToken + 2 );
                    szTwoDigits[ 1 ] = *( pchToken + 3 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->month = atoi( szTwoDigits );

                    szTwoDigits[ 0 ] = *( pchToken + 4 );
                    szTwoDigits[ 1 ] = *( pchToken + 5 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->day = atoi( szTwoDigits );
                    }
                else if ( strlen( pchToken ) == 8 )
                    {
                    szFourDigits[ 0 ] = *( pchToken + 0 );
                    szFourDigits[ 1 ] = *( pchToken + 1 );
                    szFourDigits[ 2 ] = *( pchToken + 2 );
                    szFourDigits[ 3 ] = *( pchToken + 3 );
                    szFourDigits[ 4 ] = '\0';
                    prec->year = AdjustCentury( atoi( szFourDigits ) );
                
                    szTwoDigits[ 0 ] = *( pchToken + 4 );
                    szTwoDigits[ 1 ] = *( pchToken + 5 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->month = atoi( szTwoDigits );

                    szTwoDigits[ 0 ] = *( pchToken + 6 );
                    szTwoDigits[ 1 ] = *( pchToken + 7 );
                    szTwoDigits[ 2 ] = '\0';
                    prec->day = atoi( szTwoDigits );
                    }
                else
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }

                pchFormat += strlen( _szYmd );
                mask |= MASK_YR; 
                mask |= MASK_MN;
                mask |= MASK_DY;
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szYr, strlen( _szYr ) ) == 0 )
                {
                if ( mask & MASK_YR )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->year = AdjustCentury( atoi( pchToken ) );
                pchFormat += strlen( _szYr );
                mask |= MASK_YR; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szMn, strlen( _szMn ) ) == 0 )
                {
                if ( mask & MASK_MN )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                if ( isdigit( *pchToken ) )
                    prec->month = atoi( pchToken );
                else
                    {
                    prec->month = MonthFromString( pchToken );
                    if ( prec->month == 0 )
                        {
                        fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                        THROW_EXCEPTION( exFormat );
                        }
                    }
                pchFormat += strlen( _szMn );
                mask |= MASK_MN; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szDy, strlen( _szDy ) ) == 0 )
                {
                if ( mask & MASK_DY )
                    {
                    fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
                    THROW_EXCEPTION( exFormat );
                    }
                prec->day = atoi( pchToken );
                pchFormat += strlen( _szDy );
                mask |= MASK_DY; 
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else if ( memcmp( pchFormat, _szIg, strlen( _szIg ) ) == 0 )
                {
                pchFormat += strlen( _szIg );
                pchToken = strtok( NULL, _szDelimitFormat );
                }
            else
                {
                pchFormat++;
                }
            }
        
        // Optional attributes
        if ( !( mask & MASK_VL ) )
            mask |= MASK_VL;
        if ( !( mask & MASK_OP ) )
            mask |= MASK_OP;
        if ( !( mask & MASK_HI ) )
            mask |= MASK_HI;
        if ( !( mask & MASK_LO ) )
            mask |= MASK_LO;

        if ( mask != MASK_FULLUPDATE )
            {
            fprintf( stderr, "qt_upd: invalid update record [%ld]\n", *plLine );
            THROW_EXCEPTION( exFormat );
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// MonthFromString( )
//
// This routine converts a string into a month (1-12)
//
// Returns 0 if not found
//////////////////////////////////////////////////////////////////////////////
int MonthFromString( char *szMonth )
    {
    int i;
    for ( i = 0; i < sizeof( rgszMonths ) / sizeof( rgszMonths[ 0 ] ); i++ )
        {
        if ( memicmp( szMonth, rgszMonths[ i ], 3 ) == 0 )
            return i + 1;
        }

    return 0;
    }

//////////////////////////////////////////////////////////////////////////////
// AdjustCentury( )
//
// This routine adjusts a year (integer) to handle centuries
// according to the following rules:
//
// if year < 70        --> Assume the year is 20xx.
// if 70 <= year < 100 --> Assume the year is 19xx.
// otherwise           --> Make to assumptions.
//////////////////////////////////////////////////////////////////////////////
int AdjustCentury( int year )
    {
    if ( year < 70 )
        {
        year += 2000;
        }
    else if ( 70 <= year && year < 100 )
        {
        year += 1900;
        }
    return year;
    }

//////////////////////////////////////////////////////////////////////////////
// Pause( )
//
// Pauses, waiting for input from the console, if _fQuiet is
// FALSE.
//////////////////////////////////////////////////////////////////////////////
void Pause( void )
    {
    if ( !_fQuiet )
        {
        fprintf( stderr, "qt_upd: press any key to continue...\n" );
        getch( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// ArgsLoop( )
//
// This routine will loop through the user-arguments, 
// and call a user-defined call-back routine for
// each path in the arguments.
//////////////////////////////////////////////////////////////////////////////
void ArgsLoop( int argc, char *argv[ ], int iArg, void (*pfn)( char *szFullPath ) )
    {
    struct find_t find;
    char   szDrive [ _MAX_DRIVE + 1 ];
    char   szDir   [ _MAX_DIR   + 1 ];
    char   szFName [ _MAX_FNAME + 1 ];
    char   szExt   [ _MAX_EXT   + 1 ];
    char   szPath  [ _cbPathMax + 1 ];
    int    i;
    int    rc;
    
    // Invarient: iArg will be pointing to 1st history file
    for ( i = iArg; i < argc; i++ )
        {
        rc = _dos_findfirst( argv[ i ], _A_NORMAL, &find );
        while ( rc == 0 )
            {
            _splitpath( argv[ i ], szDrive, szDir, szFName, szExt );
            if ( strlen( szDrive ) + strlen( szDir ) + strlen( find.name )
                 >= sizeof( szPath ) )
                THROW_EXCEPTION( exGeneric );
            strcpy( szPath, szDrive );
            strcat( szPath, szDir );
            strcat( szPath, find.name );
            pfn( szPath );
            rc = _dos_findnext( &find );
            }
        }
    }

//////////////////////////////////////////////////////////////////////////////
// InitHashTable( )
//
// Initializes the hash table.
//////////////////////////////////////////////////////////////////////////////
void InitHashTable( void )
    {
    int iHash;

    for ( iHash = 0; iHash < HASHMAX; iHash++ )
        {
        rgHash[ iHash ] = NULL;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// FindHash( )
//
// This routine will search for entries in the hash table.
// Both empty and populated entries may be returned.
//////////////////////////////////////////////////////////////////////////////
HashLink *FindHash( char *szTicker )
    {
    HashLink *plnk = NULL;
    int       iHash;

    strupr( szTicker );
    iHash = HashTicker( szTicker );

    plnk = rgHash[ iHash ];
    while ( plnk )
        {
        if ( strcmp( plnk->urec.szTicker, szTicker ) == 0 )
            return plnk;
        plnk = plnk->next;
        }
    return NULL;
    }

//////////////////////////////////////////////////////////////////////////////
// AddEmptyHash( )
//
// This routine will add an empty hash entry to the hash table.
//////////////////////////////////////////////////////////////////////////////
void AddEmptyHash( char *szTicker )
    {
    HashLink *plnk = NULL;
    int       iHash;

    strupr( szTicker );
    iHash = HashTicker( szTicker );
    
    // We don't want to add entries twice
    // This could happen if the user did
    // something like: qt_upd -f update.txt abx.txt abx.txt
    if ( FindHash( szTicker ) != NULL )
        {
        fprintf( stderr, "qt_upd: invalid hash\n" );
        THROW_EXCEPTION( exFormat );
        }

    plnk = (HashLink *) malloc( sizeof( HashLink ) );
    if ( plnk == NULL )
        THROW_EXCEPTION( exMemory );

    memset( plnk, 0, sizeof( HashLink ) );
    
    strcpy( plnk->urec.szTicker, szTicker );
    plnk->fPopulated = FALSE;
    plnk->next       = rgHash[ iHash ];
    rgHash[ iHash ]  = plnk;
    }

//////////////////////////////////////////////////////////////////////////////
// HashTicker( )
//
// This routine will hash a ticker symbol, and return
// an integer between ( 0 .. HASHMAX - 1 ).
//////////////////////////////////////////////////////////////////////////////
int HashTicker( char *szTicker )
    {
    char *pch  = szTicker;
    unsigned n = 0u;

    if ( !*pch )
        {
        fprintf( stderr, "qt_upd: cannot hash ticker symbol\n" );
        THROW_EXCEPTION( exFormat );
        }

    while ( *pch )
        {
        n += (unsigned) *pch;
        pch++;
        }

    return ( n % HASHMAX );
    }

//////////////////////////////////////////////////////////////////////////////
// DestroyHashTable( )
//
// Deallocates all elements in the hash table.
//////////////////////////////////////////////////////////////////////////////
void DestroyHashTable( void )
    {
    HashLink *plnk;
    HashLink *plnkNext;
    int       iHash;

    for ( iHash = 0; iHash < HASHMAX; iHash++ )
        {
        plnk = rgHash[ iHash ];
        while ( plnk )
            {
            plnkNext = plnk->next;
            free( plnk );
            plnk = plnkNext;
            }
        }
    InitHashTable( );
    }

#ifdef HASHTEST
//////////////////////////////////////////////////////////////////////////////
// HashTest( )
//
// This test routine will dump the hash table.
//////////////////////////////////////////////////////////////////////////////
void DumpHash( void )
    {
    int iHash;
    HashLink *plnk;

    fprintf( stderr, "----------------------------------------\n" );
    fprintf( stderr, "Contents of Hash Table:\n\n" );

    for ( iHash = 0; iHash < HASHMAX; iHash++ )
        {
        fprintf( stderr, "rgHash[ %d ] = ", iHash );
        for ( plnk = rgHash[ iHash ]; plnk; plnk = plnk->next )
            {
            fprintf( stderr, "%s ", plnk->urec.szTicker );
            }
        fprintf( stderr, "\n\n" );
        }
    Pause( );
    }
#endif
