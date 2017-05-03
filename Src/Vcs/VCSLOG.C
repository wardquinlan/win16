//////////////////////////////////////////////////////////////////////////////
// VCSLOG.C
//
// Implements VCS logging
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>
#include "vcs.h"
#include "vcslog.h"
#include "vcscore.h"

//////////////////////////////////////////////////////////////////////////////
// Globals

static PFNLOG _rgpfnLog[ _cLogMax ];
static int    _cLogCur = 0;
static FILE  *_pfile   = NULL;

//////////////////////////////////////////////////////////////////////////////
// Constants
static char _szLogPath[ ] = "\\\\LOG.VCS";

//////////////////////////////////////////////////////////////////////////////
// vcs_log_init( )
//
// Initializes the log system
//////////////////////////////////////////////////////////////////////////////
void vcs_log_init( void )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( _rgpfnLog, 0, sizeof( _rgpfnLog ) );
    vcs_resolve_path_reserved( szPath, _szLogPath );

    _pfile = fopen( szPath, _szTextAppend );
    if ( !_pfile )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_log_destroy( )
//
// Destroys the log system
//////////////////////////////////////////////////////////////////////////////
void vcs_log_destroy( void )
    {
    if ( _pfile )
        fclose( _pfile );
    _pfile = NULL;
    memset( _rgpfnLog, 0, sizeof( _rgpfnLog ) );
    _cLogCur = 0;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_register_log( )
//
// registers a log function
//////////////////////////////////////////////////////////////////////////////
void vcs_register_log( PFNLOG pfn )
    {
    if ( _cLogCur == _cLogMax )
        THROW_EXCEPTION( exInternal );

    _rgpfnLog[ _cLogCur ] = pfn;
    _cLogCur++;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_purge_log( )
//
// Purge the log file
//////////////////////////////////////////////////////////////////////////////
void vcs_purge_log( void )
    {
    char szPath[ _cbPathMax + 1 ];
    
    vcs_resolve_path_reserved( szPath, _szLogPath );
    if ( _pfile )
        fclose( _pfile );
    
    _pfile = fopen( szPath, _szTextWrite );
    if ( !_pfile )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_type_log( )
//
// 'Types' the log routine to the callback function.
//////////////////////////////////////////////////////////////////////////////
void vcs_type_log( void (*pfn)( char *szLine ) )
    {
    char szPath[ _cbPathMax   + 1 ];
    char szMsg [ _cbLogMsgMax + 1 ];
    FILE *pfile = NULL;
    int i;

    // we must delete all references to the standard log
    for ( i = 0; i < _cLogMax; i++ )
        {
        if ( _rgpfnLog[ i ] == vcs_standard_log )
            _rgpfnLog[ i ] = NULL;
        }

    // now, close the file, then read it
    if ( _pfile )
        fclose( _pfile );
    _pfile = NULL;

    TRY
        {
        vcs_resolve_path_reserved( szPath, _szLogPath );
        pfile = fopen( szPath, _szTextRead );
        if ( !pfile )
            THROW_EXCEPTION( exFile );
        
        while ( vcs_read_line( pfile, szMsg, sizeof( szMsg ) ) )
            {
            pfn( szMsg );
            }
        fclose( pfile );
        }
    CATCH_ALL
        {
        if ( pfile )
            fclose( pfile );
        pfile = NULL;
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_standard_log( )
//
// VCS Standard log function.  Do not call this routine directly;
// have it registered through the log system.
//////////////////////////////////////////////////////////////////////////////
void vcs_standard_log( const char *pszMsg )
    {
    time_t now;

    time( &now );
    if ( _pfile )
        fprintf( _pfile, "%s: %s\n", vcs_ctime( &now ), pszMsg );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_log
//
// Logs messages to log callback routines.
//////////////////////////////////////////////////////////////////////////////
void vcs_log( const char *pszFormat, ... )
    {
    va_list    marker;
    struct tm *ptm;
    time_t     t;
    char       szBuffer[ _cbLogMsgMax + 1 ];
    char       szTime[ 32 + 1 ];
    int        i;

    va_start( marker, pszFormat );
    vsprintf( szBuffer, pszFormat, marker );
    va_end( marker );
    assert( strlen( szBuffer ) < sizeof( szBuffer ) );
        
    for ( i = 0; i < _cLogCur; i++ )
        {
        if ( _rgpfnLog [ i ] )
            _rgpfnLog[ i ]( szBuffer );
        }
    }
