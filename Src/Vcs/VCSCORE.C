//////////////////////////////////////////////////////////////////////////////
// VCSCORE.C
//
// Implements core VCS functionality
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <dos.h>
#include <assert.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "vcs.h"
#include "vcscore.h"
#include "vcsfile.h"

//////////////////////////////////////////////////////////////////////////////
// Global Variables

// Catch Buffer
CATCHBUF _catchbuf;

// Initialized Flag
BOOL _fInitialized = FALSE;

// Ctrl-C Flag
BOOL _fCtrlC = FALSE;

// VCS Root Path
char _szVcsRoot[ _cbPathMax + 1 ];

// VCS Relative Path
char _szVcsRPath[ _cbPathMax + 1 ];

// Default VCS Branch
char _szVcsBranch[ _cbSymbolMax + 1 ];

// Pointer to old Ctrl-C handler
void ( _interrupt _far *_pfCtrlCSav )( );

// fopen flags
char _szBinaryRead  [ ] = "rb";
char _szBinaryWrite [ ] = "wb";
char _szBinaryAppend[ ] = "ab";
char _szTextRead    [ ] = "rt";
char _szTextWrite   [ ] = "wt";
char _szTextAppend  [ ] = "at";

// other strings
char _szEmpty [ ]      = "";
char _szCR    [ ]      = "\n";
char _szLabels[ ]      = "\\LABELS.VCS";
char _szStar  [ ]      = "\\*.*";
char _szSStar [ ]      = "*";
char _szSlash [ ]      = "\\";
char _szDot   [ ]      = ".";
char _szDot2  [ ]      = "..";

//////////////////////////////////////////////////////////////////////////////
// vcs_init_system( )
//
// Initializes the VCS system
//////////////////////////////////////////////////////////////////////////////
void vcs_init_system( void )
    {
    vcs_create_label_file( _szVcsRoot );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_init_instance
//
// Initialization routine
//////////////////////////////////////////////////////////////////////////////
void vcs_init_instance( void )
    {
    char *pch;

    memset( _szVcsRoot,   0, sizeof( _szVcsRoot ) );
    memset( _szVcsBranch, 0, sizeof( _szVcsBranch ) );
    memset( _szVcsRPath,  0, sizeof( _szVcsRPath ) );

    pch = getenv( "VCSROOT" );
    if ( !pch )
        THROW_EXCEPTION( exEnvironment );

    if ( strlen( pch ) >= sizeof( _szVcsRoot ) )
        THROW_EXCEPTION( exEnvironment );

    strcpy( _szVcsRoot, pch );
    vcs_remove_trailing_slash( _szVcsRoot );
    strupr( _szVcsRoot );
    
    pch = getenv( "VCSBRANCH" );
    if ( pch )
        {
        if ( strlen( pch ) >= sizeof( _szVcsBranch ) )
            THROW_EXCEPTION( exEnvironment );
        strcpy( _szVcsBranch, pch );
        }

    pch = getenv( "VCSRPATH" );
    if ( pch )
        {
        if ( strlen( pch ) >= sizeof( _szVcsRPath ) )
            THROW_EXCEPTION( exEnvironment );
        strcpy( _szVcsRPath, pch );
        strupr( _szVcsRPath );
        }

    vcs_log_init( );

    _fInitialized = TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_destroy_instance
//
// Destruction routine
//////////////////////////////////////////////////////////////////////////////
void vcs_destroy_instance( void )
    {
    vcs_log_destroy( );
    }

//////////////////////////////////////////////////////////////////////////////
// fCtrlC
//
// Ctrl-C Handler
//////////////////////////////////////////////////////////////////////////////
void _interrupt _far fCtrlC( )
    {
    _fCtrlC = TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_block_ctrl_c( )
//
// Blocks CTRL-C
//////////////////////////////////////////////////////////////////////////////
void vcs_block_ctrl_c( )
    {
    _pfCtrlCSav = _dos_getvect( 0x23 );
    _dos_setvect( 0x23, fCtrlC );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_restore_ctrl_c( )
//
// Restores CTRL-C
//////////////////////////////////////////////////////////////////////////////
void vcs_restore_ctrl_c( )
    {
    assert( _pfCtrlCSav != NULL );
    _dos_setvect( 0x23, _pfCtrlCSav );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_check_ctrl_c( )
//
// Checks to see if the user hit Ctrl-C
//////////////////////////////////////////////////////////////////////////////
void vcs_check_ctrl_c( void )
    {
    if ( _fCtrlC )
        THROW_EXCEPTION( exCtrlC );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_check_initialized( )
//
// Checks to see user module called vcs_init_instance
//////////////////////////////////////////////////////////////////////////////
void vcs_check_initialized( void )
    {
    if ( !_fInitialized )
        THROW_EXCEPTION( exInternal );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_remove_trailing_slash( )
//
// Removes a trailing '\\' character from a path, if present
//////////////////////////////////////////////////////////////////////////////
void vcs_remove_trailing_slash( char *pszPath )
    {
    unsigned cb = strlen( pszPath );
    
    if ( cb && pszPath[ cb - 1 ] == '\\' )
        pszPath[ cb - 1 ] = '\0';
    }


//////////////////////////////////////////////////////////////////////////////
// vcs_directory_count( )
//
// Counts number of items in directory
// pszHeader must point to a buffer of size ( _cbHeaderMax + 1 )
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_directory_count( char *pszPath, unsigned attrib, char *pszHeader )
    {
    char szPath[ _cbPathMax + 1 ];
    
    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );

    if ( strlen( szPath ) > _cbHeaderMax )
        THROW_EXCEPTION( exInternal );
    strcpy( pszHeader, szPath );
    
    return vcs_directory_count_i( szPath, attrib );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_directory( )
//
// Directory Listing
//////////////////////////////////////////////////////////////////////////////
void vcs_directory( char *pszPath, unsigned attrib, Direct *pd, unsigned nMax )
    {
    char szPath[ _cbPathMax + 1 ];
    
    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    
    vcs_directory_i( szPath, attrib, pd, nMax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_directory_count_i( )
//
// Counts number of items in directory
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_directory_count_i( char *pszPath, unsigned attrib )
    {
    unsigned n = 0u;
    unsigned rc;
    struct find_t ff;
    char szPath[ _cbPathMax + 1 ];
    
    memset( szPath, 0, sizeof( szPath ) );
    if ( strlen( pszPath ) >= sizeof( szPath ) )
        THROW_EXCEPTION( exInternal );
    strcpy( szPath, pszPath );

    if ( vcs_is_dir( szPath ) )
        {
        if ( strlen( szPath ) + strlen( _szStar ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcat( szPath, _szStar );
        }

    rc = _dos_findfirst( szPath, attrib, &ff );
    while ( rc == 0u )
        {
        if ( strcmp( ff.name, _szDot  ) != 0 &&
             strcmp( ff.name, _szDot2 ) != 0 &&
             !vcs_is_reserved( ff.name ) )
            {
            ++n;
            }
        rc = _dos_findnext( &ff );
        }
    vcs_check_ctrl_c( );
    return n;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_directory_i( )
//
// Directory Listing
//////////////////////////////////////////////////////////////////////////////
void vcs_directory_i( char *pszPath, unsigned attrib, Direct *pd, unsigned nMax )
    {
    unsigned n = 0u;
    unsigned rc;
    struct find_t ff;
    char szPath [ _cbPathMax + 1 ];
    char szDrive[ _MAX_DRIVE + 1 ];
    char szDir  [ _MAX_DIR   + 1 ];
    char szFName[ _MAX_FNAME + 1 ];
    char szExt  [ _MAX_EXT   + 1 ];
    
    memset( szPath, 0, sizeof( szPath ) );
    if ( strlen( pszPath ) >= sizeof( szPath ) )
        THROW_EXCEPTION( exInternal );
    strcpy( szPath, pszPath );

    if ( vcs_is_dir( szPath ) )
        {
        if ( strlen( szPath ) + strlen( _szStar ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcat( szPath, _szStar );
        }

    rc = _dos_findfirst( szPath, attrib, &ff );
    while ( rc == 0u )
        {
        if ( strcmp( ff.name, _szDot  ) != 0 &&
             strcmp( ff.name, _szDot2 ) != 0 &&
             !vcs_is_reserved( ff.name ) )
            {
            if ( n == nMax )
                THROW_EXCEPTION( exInternal );
            memset( &pd[ n ], 0, sizeof( Direct ) );
            
            _splitpath( ff.name, szDrive, szDir, szFName, szExt );
            
            if ( strlen( szFName ) >= sizeof( pd[ 0 ].szFile ) )
                THROW_EXCEPTION( exInternal );
            if ( strlen( szExt  ) >= sizeof( pd[ 0 ].szExt  ) )
                THROW_EXCEPTION( exInternal );
            
            strcpy( pd[ n ].szFile, szFName );
            if ( *szExt == '.' )
                strcpy( pd[ n ].szExt, szExt + 1 );
            
            pd[ n ].nSize   = ff.size;
            pd[ n ].bAttrib = ff.attrib;
            vcs_dostime_to_time_t( &pd[ n ].t, &ff.wr_date, &ff.wr_time );
            ++n;
            }
        rc = _dos_findnext( &ff );
        }
    qsort( pd, n, sizeof( Direct ), vcs_direct_cmp );
    vcs_check_ctrl_c( );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_dostime_to_time_t( )
//
// Converts a DOS time to a time_t
//////////////////////////////////////////////////////////////////////////////
void vcs_dostime_to_time_t( time_t *ptime_t, unsigned *pdate, unsigned *ptime )
    {
    struct tm tt;

    memset( &tt, 0, sizeof( tt ) );

    tt.tm_year = ( ( *pdate >>  9 ) & 0x007f ) + 80;
    tt.tm_mon  = ( ( *pdate >>  5 ) & 0x000f ) -  1;
    tt.tm_mday = ( ( *pdate       ) & 0x001f );

    tt.tm_hour = ( ( *ptime >> 11 ) & 0x001f );
    tt.tm_min  = ( ( *ptime >>  5 ) & 0x003f );
    tt.tm_sec  = ( ( *ptime       ) & 0x001f ) << 1;
            
    *ptime_t  = mktime( &tt );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_direct_cmp( )
//
// Comparison routine for qsort.  Comparison
// is based on:
// -All directories are at the beginning
// -For files or directories, sort based on reverse-time
//////////////////////////////////////////////////////////////////////////////
int vcs_direct_cmp( const void *a, const void *b )
    {
    Direct *pA = (Direct *) a;
    Direct *pB = (Direct *) b;

    if (  ( pA->bAttrib & _A_SUBDIR ) && !( pB->bAttrib & _A_SUBDIR ) )
        return -1;

    if ( !( pA->bAttrib & _A_SUBDIR ) &&  ( pB->bAttrib & _A_SUBDIR ) )
        return +1;

    if ( pA->t < pB->t )
        return +1;

    if ( pA->t > pB->t )
        return -1;

    return 0;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_create_dir( )
//
// Create Directory
//////////////////////////////////////////////////////////////////////////////
void vcs_create_dir( char *pszPath )
    {
    char szPath[ _cbPathMax + 1 ];

    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    vcs_log( "creating directory: %s", szPath );

    vcs_check_ctrl_c( );
    
    if ( mkdir( szPath ) != 0 )
        THROW_EXCEPTION( exFile );

    vcs_create_label_file( szPath );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_remove_dir( )
//
// Remove Directory
//////////////////////////////////////////////////////////////////////////////
void vcs_remove_dir( char *pszPath )
    {
    char szPath     [ _cbPathMax + 1 ];
    char szLabelPath[ _cbPathMax + 1 ];
    unsigned n;

    memset( szPath,      0, sizeof( szPath ) );
    memset( szLabelPath, 0, sizeof( szLabelPath ) );

    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    vcs_log( "removing directory: %s", szPath );

    strcpy( szLabelPath, szPath );
    if ( strlen( szLabelPath ) + strlen( _szLabels ) >= sizeof( szLabelPath ) )
        THROW_EXCEPTION( exInternal );
    strcat( szLabelPath, _szLabels );

    n = vcs_list_all_labels_count_i( szPath );
    if ( n > 0u )
        THROW_EXCEPTION( exIllegal );

    n = vcs_directory_count_i( szPath, _A_SUBDIR );
    if ( n > 0u )
        THROW_EXCEPTION( exIllegal );

    vcs_check_ctrl_c( );
    
    vcs_log( "removing label file: %s", szLabelPath );
    vcs_file_remove( szLabelPath );

    if ( rmdir( szPath ) != 0 )
        {
        // This is bad.  We now have to re-create
        // the label file to keep things internally
        // consistent
        vcs_log( "removal of directory failed.  Attempting to re-create label file..." );
        vcs_create_label_file( szPath );
        THROW_EXCEPTION( exFile );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_create_file( )
//
// Create File
//////////////////////////////////////////////////////////////////////////////
void vcs_create_file( char *pszPath )
    {
    FILE *pfile = NULL;
    char szPath   [ _cbPathMax + 1 ];
    char szPathTmp[ _cbPathMax + 1 ];
    FileHeader h;

    TRY
        {
        memset( szPath,    0, sizeof( szPath ) );
        memset( szPathTmp, 0, sizeof( szPathTmp ) );
        
        vcs_check_initialized( );
        vcs_resolve_path( szPath, pszPath );
        vcs_log( "creating file: %s", szPath );

        if ( vcs_file_exists( szPath ) )
            THROW_EXCEPTION( exFile );
            
        vcs_tempfile( szPathTmp, szPath );
        pfile = fopen( szPathTmp, _szBinaryWrite );
        if ( !pfile )
            THROW_EXCEPTION( exFile );

        h.nVCSVersion = _nVCSVersion;
        h.ft          = eDataFile;
        vcs_write_file_header( pfile, &h );
        fclose( pfile );

        vcs_check_ctrl_c( );
        vcs_commit( szPath );
        }
    CATCH_ALL
        {
        if ( pfile )
            fclose( pfile );
        
        vcs_abort( szPath );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_remove_file( )
//
// Remove File
//////////////////////////////////////////////////////////////////////////////
void vcs_remove_file( char *pszPath )
    {
    unsigned n;
    char szDir [ _cbPathMax + 1 ];
    char szFile[ _cbPathMax + 1 ];
    char szPath[ _cbPathMax + 1 ];

    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    vcs_log( "removing file: %s", szPath );

    n = vcs_versions_count_i( szPath );
    if ( n > 0u )
        THROW_EXCEPTION( exIllegal );

    vcs_check_ctrl_c( );
    
    if ( unlink( szPath ) != 0 )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_all_labels_count( )
//
// Count number of labels
// pszHeader must point to a buffer of size ( _cbHeaderMax + 1 )
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_list_all_labels_count( char *pszPath, char *pszHeader )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    
    if ( strlen( szPath ) > _cbHeaderMax )
        THROW_EXCEPTION( exInternal );
    strcpy( pszHeader, szPath );

    return vcs_list_all_labels_count_i( szPath );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_all_labels_count_i( )
//
// Count number of labels
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_list_all_labels_count_i( char *pszPath )
    {
    unsigned n = 0u;
    char szPath[ _cbPathMax + 1 ];
    FILE *pfileRead = NULL;
    FileHeader   h;
    LabelHeader lh;
    
    TRY
        {
        memset( szPath, 0, sizeof( szPath ) );
        
        if ( strlen( pszPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        strcat( szPath, _szLabels );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );

        while ( vcs_read_label_header( pfileRead, &lh ) )
            {
            vcs_skip_label( pfileRead );
            ++n;
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    return n;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_all_labels( )
//
// Label listings
//////////////////////////////////////////////////////////////////////////////
void vcs_list_all_labels( char *pszPath, LabelHeader *ph, unsigned nMax )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    
    vcs_list_all_labels_i( szPath, ph, nMax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_all_labels_i( )
//
// Label listings
//////////////////////////////////////////////////////////////////////////////
void vcs_list_all_labels_i( char *pszPath, LabelHeader *ph, unsigned nMax )
    {
    unsigned n = 0u;
    char szPath[ _cbPathMax + 1 ];
    FILE *pfileRead = NULL;
    FileHeader   h;
    LabelHeader lh;
    
    TRY
        {
        memset( szPath, 0, sizeof( szPath ) );
        
        if ( strlen( pszPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        strcat( szPath, _szLabels );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );

        while ( vcs_read_label_header( pfileRead, &lh ) )
            {
            if ( n == nMax )
                THROW_EXCEPTION( exInternal );

            vcs_skip_label( pfileRead );
            memcpy( &ph[ n ], &lh, sizeof( LabelHeader ) );
            ++n;
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_versions_count( )
//
// Versions count
// pszHeader must point to a buffer of size ( _cbHeaderMax + 1 )
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_versions_count( char *pszPath, char *pszHeader )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );

    if ( strlen( szPath ) > _cbHeaderMax )
        THROW_EXCEPTION( exInternal );
    strcpy( pszHeader, szPath );

    return vcs_versions_count_i( szPath );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_versions( )
//
// Versions
//////////////////////////////////////////////////////////////////////////////
void vcs_versions( char *pszPath, FileVersionHeader *ph, unsigned nMax )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    vcs_versions_i( szPath, ph, nMax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_versions_count_i( )
//
// Versions count
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_versions_count_i( char *pszPath )
    {
    unsigned n = 0u;
    char szPath[ _cbPathMax + 1 ];
    FILE *pfileRead = NULL;
    FileHeader         h;
    FileVersionHeader vh;
    
    TRY
        {
        memset( szPath, 0, sizeof( szPath ) );
        if ( strlen( pszPath ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eDataFile )
            THROW_EXCEPTION( exCorrupt );

        while ( vcs_read_version_header( pfileRead, &vh ) )
            {
            vcs_read_space( pfileRead, vh.cb );
            ++n;
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        
        THROW_LAST( );
        }
    END_CATCH
    return n;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_versions_i( )
//
// Versions
//////////////////////////////////////////////////////////////////////////////
void vcs_versions_i( char *pszPath, FileVersionHeader *ph, unsigned nMax )
    {
    unsigned n = 0u;
    char szPath[ _cbPathMax + 1 ];
    FILE *pfileRead = NULL;
    FileHeader         h;
    FileVersionHeader vh;
    
    TRY
        {
        memset( szPath, 0, sizeof( szPath ) );
        if ( strlen( pszPath ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eDataFile )
            THROW_EXCEPTION( exCorrupt );

        while ( vcs_read_version_header( pfileRead, &vh ) )
            {
            if ( n == nMax )
                THROW_EXCEPTION( exInternal );

            vcs_read_space( pfileRead, vh.cb );
            memcpy( &ph[ n ], &vh, sizeof( FileVersionHeader ) );
            ++n;
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_create_label_file( )
//
// Creates a label file
//////////////////////////////////////////////////////////////////////////////
void vcs_create_label_file( char *pszPath )
    {
    FILE *pfile = NULL;
    char szPath   [ _cbPathMax + 1 ];
    char szPathTmp[ _cbPathMax + 1 ];
    FileHeader h;

    TRY
        {
        memset( szPath,    0, sizeof( szPath ) );
        memset( szPathTmp, 0, sizeof( szPathTmp ) );

        vcs_check_initialized( );
        if ( strlen( pszPath ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        vcs_remove_trailing_slash( szPath );
        
        if ( strlen( szPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcat( szPath, _szLabels );
        
        vcs_log( "creating label file: %s", szPath );

        if ( vcs_file_exists( szPath ) )
            THROW_EXCEPTION( exIllegal );
            
        vcs_tempfile( szPathTmp, szPath );
        pfile = fopen( szPathTmp, _szBinaryWrite );
        if ( !pfile )
            THROW_EXCEPTION( exFile );

        h.nVCSVersion = _nVCSVersion;
        h.ft          = eLabelFile;
        vcs_write_file_header( pfile, &h );
        
        fclose( pfile );

        vcs_check_ctrl_c( );
        vcs_commit( szPath );
        }
    CATCH_ALL
        {
        if ( pfile )
            fclose( pfile );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_make_filename( )
//
// Creates a file name.
//
// Assumptions: pszFileName is of length ( _cbFileMax + 1 )
//////////////////////////////////////////////////////////////////////////////
void vcs_make_filename( char *pszFileName, char *pszName, char *pszExt )
    {
    memset( pszFileName, 0, _cbFileMax + 1 );

    if ( strlen( pszName ) + strlen( _szDot ) + strlen( pszExt ) > _cbFileMax )
        THROW_EXCEPTION( exInternal );

    strcpy( pszFileName, pszName );
    if ( *pszExt )
        {
        strcat( pszFileName, _szDot );
        strcat( pszFileName, pszExt );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_find_direct_entry( )
//
// Given a Direct *, returns the index of an entry
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_find_direct_entry( unsigned *pidx, Direct *pd, unsigned nMax, char *szName )
    {
    char szTmp[ _cbFileMax + 1 ];
    unsigned i;

    for ( i = 0; i < nMax; i++ )
        {
        vcs_make_filename( szTmp, pd[ i ].szFile, pd[ i ].szExt );
        
        if ( strcmp( szTmp, szName ) == 0 )
            {
            *pidx = i;
            return TRUE;
            }
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_get_latest_version( )
//
// Gets the latest version of a file.  If there are no
// versions for the given branch, returns 0u.
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_get_latest_version( char *pszPath, const char *pszBranch )
    {
    unsigned i;
    unsigned nVersionMax  = 0u;
    unsigned nMax         = 0u;
    FileVersionHeader *ph = NULL;
    
    TRY
        {
        nMax = vcs_versions_count_i( pszPath );
        if ( nMax > 0u )
            {
            ph = (FileVersionHeader *) malloc( nMax * sizeof( FileVersionHeader ) );
            if ( !ph )
                THROW_EXCEPTION( exMemory );
            memset( ph, 0, nMax * sizeof( FileVersionHeader ) );
            vcs_versions_i( pszPath, ph, nMax );

            for ( i = 0; i < nMax; i++ )
                {
                if ( strcmp( ph[ i ].szBranch, pszBranch ) == 0 && ph[ i ].nVersion > nVersionMax )
                    nVersionMax = ph[ i ].nVersion;
                }
            free( ph );
            }
        }
    CATCH_ALL
        {
        if ( ph )
            free( ph );
        THROW_LAST( );
        }
    END_CATCH
    return nVersionMax;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_label_count( )
//
// Count List particular label
// pszHeader must point to a buffer of size ( _cbHeaderMax + 1 )
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_list_label_count( char *pszPath, const char *pszBranch, const char *pszLabel, char *pszHeader )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    
    if ( strlen( szPath ) > _cbHeaderMax )
        THROW_EXCEPTION( exInternal );
    strcpy( pszHeader, szPath );
    
    return vcs_list_label_count_i( szPath, pszBranch, pszLabel );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_label_count_i( )
//
// Count List particular label
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_list_label_count_i( char *pszPath, const char *pszBranch, const char *pszLabel )
    {
    unsigned n = 0u;
    State st;
    FILE *pfileRead = NULL;
    LabelRec rec;
    LabelHeader lh;
    FileHeader   h;
    BOOL fEOF = FALSE;
    BOOL fDone = FALSE;
    char szPath[ _cbPathMax + 1 ];
    
    TRY
        {
        memset( szPath, 0, sizeof( szPath ) );
        
        if ( strlen( pszPath ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        
        if ( strlen( szPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcat( szPath, _szLabels );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );
                    
                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    {
                    fDone = FALSE;
                    while ( !fDone )
                        {
                        vcs_read_label_rec( pfileRead, &rec );
                        switch( rec.type )
                            {
                        case eFileLabelRec:
                        case eDirLabelRec:
                            n++;
                            break;
                        
                        case eEmptyLabelRec:
                            fDone = TRUE;
                            break;
                            }
                        }

                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_skip_label( pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_skip_label( pfileRead );
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                break;
                }
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    return n;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_version_is_referenced( )
//
// Returns TRUE if the given version is referenced within
// the Label File
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_version_is_referenced( char *pszPath, char *pszFile, const char *pszBranch, unsigned nVersion )
    {
    LabelRec rec;
    LabelHeader lh;
    FileHeader   h;
    BOOL fRet  = FALSE;
    BOOL fDone = FALSE;
    FILE *pfileRead = NULL;
    char szPath[ _cbPathMax + 1 ];
    char szFile[ _cbPathMax + 1 ];
    
    TRY
        {
        memset( szPath, 0, sizeof( szPath ) );
        memset( szFile, 0, sizeof( szFile ) );

        if ( strlen( pszPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        strcat( szPath, _szLabels );

        if ( strlen( pszFile ) >= sizeof( szFile ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szFile, pszFile );

        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        
        while ( vcs_read_label_header( pfileRead, &lh ) )
            {
            if ( strcmp( lh.szBranch, pszBranch ) == 0 )
                {
                fDone = FALSE;
                while ( !fDone )
                    {
                    vcs_read_label_rec( pfileRead, &rec );
                    switch( rec.type )
                        {
                    case eDirLabelRec:
                        break;

                    case eFileLabelRec:
                        if ( strcmp( szFile, rec.szFile ) == 0 && rec.nVersion == nVersion )
                            fRet = TRUE;
                        break;

                    case eEmptyLabelRec:
                        fDone = TRUE;
                        break;
                        }
                    }
                }
            else
                {
                vcs_skip_label( pfileRead );
                }
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_label_is_referenced( )
//
// Returns TRUE if the given Branch/Label is referenced within
// pszPath's Label file
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_label_is_referenced( char *pszPathParent, char *pszPathChild, const char *pszBranch, const char *pszLabel )
    {
    LabelRec rec;
    LabelHeader lh;
    FileHeader   h;
    BOOL fRet  = FALSE;
    BOOL fDone = FALSE;
    FILE *pfileRead = NULL;
    char szPath [ _cbPathMax + 1 ];
    char szChild[ _cbPathMax + 1 ];
    
    TRY
        {
        memset( szPath,  0, sizeof( szPath ) );
        memset( szChild, 0, sizeof( szChild ) );

        if ( strlen( pszPathParent ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPathParent );
        strcat( szPath, _szLabels );

        if ( strlen( pszPathChild ) >= sizeof( szChild ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szChild, pszPathChild );

        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        
        while ( vcs_read_label_header( pfileRead, &lh ) )
            {
            if ( strcmp( lh.szBranch, pszBranch ) == 0 )
                {
                fDone = FALSE;
                while ( !fDone )
                    {
                    vcs_read_label_rec( pfileRead, &rec );
                    switch( rec.type )
                        {
                    case eDirLabelRec:
                        if ( strcmp( rec.szDir, szChild ) == 0 && strcmp( rec.szLabel, pszLabel ) == 0 )
                            fRet = TRUE;
                        break;

                    case eFileLabelRec:
                        // ignore
                        break;

                    case eEmptyLabelRec:
                        fDone = TRUE;
                        break;
                        }
                    }
                }
            else
                {
                vcs_skip_label( pfileRead );
                }
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_label( )
//
// List particular label
//////////////////////////////////////////////////////////////////////////////
void vcs_list_label( char *pszPath, const char *pszBranch, const char *pszLabel, LabelRec *rgrec, unsigned nMax )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    
    vcs_list_label_i( szPath, pszBranch, pszLabel, rgrec, nMax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_list_label_i( )
//
// List particular label
//////////////////////////////////////////////////////////////////////////////
void vcs_list_label_i( char *pszPath, const char *pszBranch, const char *pszLabel, LabelRec *rgrec, unsigned nMax )
    {
    State st;
    FILE *pfileRead = NULL;
    LabelRec rec;
    LabelHeader lh;
    FileHeader   h;
    BOOL fEOF = FALSE;
    BOOL fDone = FALSE;
    char szPath[ _cbPathMax + 1 ];
    unsigned i = 0u;

    TRY
        {
        memset( szPath, 0, sizeof( szPath ) );
        if ( strlen( pszPath ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );
        
        if ( strlen( szPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcat( szPath, _szLabels );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );
                    
                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    {
                    fDone = FALSE;
                    while ( !fDone )
                        {
                        vcs_read_label_rec( pfileRead, &rec );
                        switch( rec.type )
                            {
                        case eFileLabelRec:
                        case eDirLabelRec:
                            if ( i == nMax )
                                THROW_EXCEPTION( exInternal );
                            memcpy( &rgrec[ i ], &rec, sizeof( LabelRec ) );
                            i++;
                            break;
                        
                        case eEmptyLabelRec:
                            fDone = TRUE;
                            break;
                            }
                        }

                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_skip_label( pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_skip_label( pfileRead );
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                break;
                }
            }
        fclose( pfileRead );
        pfileRead = NULL;

        qsort( rgrec, nMax, sizeof( LabelRec ), vcs_label_cmp );
        vcs_check_ctrl_c( );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_label_cmp( )
//
// Comparison routine for Listing Labels
//////////////////////////////////////////////////////////////////////////////
int vcs_label_cmp( const void *a, const void *b )
    {
    LabelRec *pA = (LabelRec *) a;
    LabelRec *pB = (LabelRec *) b;

    if ( pA->type == eDirLabelRec  && pB->type == eFileLabelRec )
        return -1;

    if ( pA->type == eFileLabelRec && pB->type == eDirLabelRec )
        return +1;

    return 0;
    }
            
//////////////////////////////////////////////////////////////////////////////
// vcs_labelable_label( )
//
// Checks to see if the given label exists in the given directory
//////////////////////////////////////////////////////////////////////////////
void vcs_labelable_label( char *pszPath, const char *pszBranch, const char *pszLabel )
    {
    unsigned i;
    unsigned nMax   = 0u;
    LabelHeader *ph = NULL;
    BOOL fFound     = FALSE;

    TRY
        {
        nMax = vcs_list_all_labels_count_i( pszPath );
        if ( nMax == 0u )
            THROW_EXCEPTION( exIllegal );

        ph = (LabelHeader *) malloc( nMax * sizeof( LabelHeader ) );
        if ( !ph )
            THROW_EXCEPTION( exMemory );
        memset( ph, 0, nMax * sizeof( LabelHeader ) );
        vcs_list_all_labels_i( pszPath, ph, nMax );

        for ( i = 0u; i < nMax; i++ )
            {
            if ( strcmp( ph[ i ].szBranch, pszBranch ) == 0 && 
                 strcmp( ph[ i ].szLabel,  pszLabel  ) == 0 )
                {
                fFound = TRUE;
                }
            }
        if ( !fFound )
            THROW_EXCEPTION( exIllegal );
        free( ph );
        }
    CATCH_ALL
        {
        if ( ph )
            free( ph );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_labelable_version( )
//
// Checks to see if the given version is both valid and locked
//////////////////////////////////////////////////////////////////////////////
void vcs_labelable_version( char *pszPath, const char *pszBranch, unsigned nVersion )
    {
    unsigned i;
    unsigned nMax         = 0u;
    FileVersionHeader *ph = NULL;
    BOOL fFound           = FALSE;
    
    TRY
        {
        if ( nVersion == 0u )
            THROW_EXCEPTION( exIllegal );

        nMax = vcs_versions_count_i( pszPath );
        if ( nMax == 0u )
            THROW_EXCEPTION( exIllegal );

        ph = (FileVersionHeader *) malloc( nMax * sizeof( FileVersionHeader ) );
        if ( !ph )
            THROW_EXCEPTION( exMemory );
        memset( ph, 0, nMax * sizeof( FileVersionHeader ) );
        vcs_versions_i( pszPath, ph, nMax );

        for ( i = 0; i < nMax; i++ )
            {
            if ( strcmp( ph[ i ].szBranch, pszBranch ) == 0 && ph[ i ].nVersion == nVersion )
                {
                if ( !ph[ i ].fLock )
                    THROW_EXCEPTION( exIllegal );
                fFound = TRUE;
                }
            }
        if ( !fFound )
            THROW_EXCEPTION( exIllegal );
        free( ph );
        }
    CATCH_ALL
        {
        if ( ph )
            free( ph );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_fixup_target( )
//
// This is a hack.  Fixes a TARGET for directory listings,
// by appending a '*' character.  This forces DOS to get
// a listing of the directory name itself, rather than
// the directory contents.
//
// pszTarget is assumed to be of maximum length _cbPathMax + 1
//////////////////////////////////////////////////////////////////////////////
void vcs_fixup_target( char *pszTarget )
    {
    char szDrive[ _MAX_DRIVE + 1 ];
    char szDir  [ _MAX_DIR   + 1 ];
    char szFName[ _MAX_FNAME + 1 ];
    char szExt  [ _MAX_EXT   + 1 ];
    
    if ( vcs_is_dir( pszTarget ) )
        {
        _splitpath( pszTarget, szDrive, szDir, szFName, szExt );

        if ( strlen( szFName ) < 8 )
            {
            strcat( szFName, _szSStar );
            }
        else if ( strlen( szFName ) == 8 )
            {
            // over-write last character!!
            szFName[ 7 ] = '*';
            }
        else
            THROW_EXCEPTION( exInternal );

        if ( strlen( szDrive ) + strlen( szDir ) + strlen( szFName ) + strlen( szExt ) > _cbPathMax )
            THROW_EXCEPTION( exInternal );
             
        _makepath( pszTarget, szDrive, szDir, szFName, szExt );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_labelcmd_add
//
// Label Command: add to label
//////////////////////////////////////////////////////////////////////////////
void vcs_labelcmd_add( char *pszPath, const char *pszBranch, const char *pszLabel, LabelCmd *pcmd )
    {
    char szLabelFile   [ _cbPathMax + 1 ];
    char szLabelFileTmp[ _cbPathMax + 1 ];
    char szTarget      [ _cbPathMax + 1 ];
    char szFile        [ _cbPathMax + 1 ];
    char szDir         [ _cbPathMax + 1 ];
    
    FILE *pfileRead    = NULL;
    FILE *pfileWrite   = NULL;
    Direct *pd         = NULL;
    BOOL *rgfCheckMark = NULL;
    unsigned nMax      = 0u;
    FileHeader  h;
    LabelHeader lh;
    BOOL fEOF = FALSE;
    BOOL fDone;
    State st;
    BOOL fFound;
    LabelRec rec;
    unsigned idx;
    unsigned i;
    unsigned nVersion;

    TRY
        {
        memset( szLabelFile,    0, sizeof( szLabelFile ) );
        memset( szLabelFileTmp, 0, sizeof( szLabelFileTmp ) );
        memset( szTarget,       0, sizeof( szTarget ) );
        
        vcs_check_initialized( );
        vcs_resolve_path( szLabelFile, pszPath );
        if ( strlen( szLabelFile ) + strlen( _szLabels ) >= sizeof( szLabelFile ) )
            THROW_EXCEPTION( exInternal );
        strcat( szLabelFile, _szLabels );
        vcs_log( "modifying label: %s - %s - %s", szLabelFile, pszBranch, pszLabel );

        vcs_tempfile( szLabelFileTmp, szLabelFile );

        vcs_resolve_path( szTarget, pszPath );
        if ( strlen( szTarget ) + strlen( _szSlash ) + strlen( pcmd->szTarget ) >= sizeof( szTarget ) )
            THROW_EXCEPTION( exInternal );
        strcat( szTarget, _szSlash );
        strcat( szTarget, pcmd->szTarget );
        vcs_fixup_target( szTarget );

        nMax = vcs_directory_count_i( szTarget, _A_SUBDIR );
        if ( !nMax )
            THROW_EXCEPTION( exIllegal );
        
        pd = (Direct *) malloc( nMax * sizeof( Direct ) );
        if ( !pd )
            THROW_EXCEPTION( exMemory );
        memset( pd, 0, nMax * sizeof( Direct ) );

        rgfCheckMark = (BOOL *) malloc( nMax * sizeof( BOOL ) );
        if ( !rgfCheckMark )
            THROW_EXCEPTION( exMemory );
        memset( rgfCheckMark, 0, nMax * sizeof( BOOL ) );

        vcs_directory_i( szTarget, _A_SUBDIR, pd, nMax );

        pfileRead = fopen( szLabelFile, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        pfileWrite = fopen( szLabelFileTmp, _szBinaryWrite );
        if ( !pfileWrite )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        vcs_write_file_header( pfileWrite, &h );

        // Now, enter the state machine
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );
                    
                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    {
                    vcs_write_label_header( pfileWrite, &lh );

                    fDone = FALSE;
                    while ( !fDone )
                        {
                        vcs_read_label_rec( pfileRead, &rec );
                        switch( rec.type )
                            {
                        case eFileLabelRec:
                            fFound = vcs_find_direct_entry( &idx, pd, nMax, rec.szFile );
                            if ( fFound )
                                {
                                memset( szFile, 0, sizeof( szFile ) );
                                vcs_resolve_path( szFile, pszPath );
                                if ( strlen( szFile ) + strlen( _szSlash ) + strlen( rec.szFile ) >= 
                                     sizeof( szFile ) )
                                    THROW_EXCEPTION( exInternal );
                                strcat( szFile, _szSlash );
                                strcat( szFile, rec.szFile );
                                
                                nVersion = vcs_resolve_version( szFile, pszBranch, (unsigned) atoi( pcmd->szVersion ) );
                                if ( nVersion != rec.nVersion )
                                    THROW_EXCEPTION( exIllegal );
                                rgfCheckMark[ idx ] = TRUE;
                                }
                            vcs_write_label_rec( pfileWrite, &rec );
                            break;

                        case eDirLabelRec:
                            fFound = vcs_find_direct_entry( &idx, pd, nMax, rec.szDir );
                            if ( fFound )
                                {
                                if ( strcmp( pcmd->szVersion, rec.szLabel ) != 0 )
                                    THROW_EXCEPTION( exIllegal );
                                rgfCheckMark[ idx ] = TRUE;
                                }
                            vcs_write_label_rec( pfileWrite, &rec );
                            break;

                        case eEmptyLabelRec:
                            fDone = TRUE;
                            break;
                            }
                        }

                    // Now, we must loop through our rgfCheckMark, and
                    // write any that were not found
                    for ( i = 0; i < nMax; i++ )
                        {
                        if ( !rgfCheckMark[ i ] )
                            {
                            if ( pd[ i ].bAttrib & _A_SUBDIR )
                                {
                                if ( !*pcmd->szVersion )
                                    THROW_EXCEPTION( exIllegal );
                                
                                memset( &rec, 0, sizeof( rec ) );
                                rec.type = eDirLabelRec;
                                vcs_make_filename( rec.szDir, pd[ i ].szFile, pd[ i ].szExt );

                                memset( szDir, 0, sizeof( szDir ) );
                                vcs_resolve_path( szDir, pszPath );
                                if ( strlen( szDir ) + strlen( _szSlash ) + strlen( rec.szDir ) >= sizeof( szDir ) )
                                    THROW_EXCEPTION( exInternal );
                                strcat( szDir, _szSlash );
                                strcat( szDir, rec.szDir );

                                if ( strlen( pcmd->szVersion ) >= sizeof( rec.szLabel ) )
                                    THROW_EXCEPTION( exInternal );
                                strcpy( rec.szLabel, pcmd->szVersion );
                                
                                vcs_labelable_label( szDir, pszBranch, rec.szLabel );
                                vcs_write_label_rec( pfileWrite, &rec );
                                }
                            else
                                {
                                memset( &rec, 0, sizeof( rec ) );
                                rec.type = eFileLabelRec;
                                vcs_make_filename( rec.szFile, pd[ i ].szFile, pd[ i ].szExt );

                                memset( szFile, 0, sizeof( szFile ) );
                                vcs_resolve_path( szFile, pszPath );
                                if ( strlen( szFile ) + strlen( _szSlash ) + strlen( rec.szFile ) >= 
                                     sizeof( szFile ) )
                                    THROW_EXCEPTION( exInternal );
                                strcat( szFile, _szSlash );
                                strcat( szFile, rec.szFile );
                                
                                rec.nVersion = vcs_resolve_version( szFile, pszBranch, (unsigned) atoi( pcmd->szVersion ) );
                                
                                vcs_labelable_version( szFile, pszBranch, rec.nVersion );
                                vcs_write_label_rec( pfileWrite, &rec );
                                }
                            }
                        }
                    vcs_write_empty_label_rec( pfileWrite );
                    
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_write_label_header( pfileWrite, &lh );
                    vcs_copy_label( pfileWrite, pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_write_label_header( pfileWrite, &lh );
                vcs_copy_label( pfileWrite, pfileRead );
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                break;
                }
            }

        free( pd );
        free( rgfCheckMark );
        fclose( pfileRead );
        fclose( pfileWrite );
        
        vcs_check_ctrl_c( );
        vcs_commit( szLabelFile );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );

        if ( pfileWrite )
            fclose( pfileWrite );

        if ( pd )
            free( pd );

        if ( rgfCheckMark )
            free( rgfCheckMark );

        vcs_abort( szLabelFile );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_labelcmd_remove
//
// Label Command: remove from label
//////////////////////////////////////////////////////////////////////////////
void vcs_labelcmd_remove( char *pszPath, const char *pszBranch, const char *pszLabel, LabelCmd *pcmd )
    {
    char szLabelFile   [ _cbPathMax + 1 ];
    char szLabelFileTmp[ _cbPathMax + 1 ];
    char szTarget      [ _cbPathMax + 1 ];
    FILE *pfileRead      = NULL;
    FILE *pfileWrite     = NULL;
    unsigned nMax        = 0u;
    Direct *pd           = NULL;
    BOOL        fDone    = FALSE;
    FileHeader  h;
    LabelHeader lh;
    BOOL        fEOF;
    BOOL        fFound = FALSE;
    unsigned    idx;
    State       st;
    LabelRec    rec;
    
    TRY
        {
        memset( szLabelFile,    0, sizeof( szLabelFile ) );
        memset( szLabelFileTmp, 0, sizeof( szLabelFileTmp ) );
        memset( szTarget,       0, sizeof( szTarget ) );

        if ( *pcmd->szVersion )
            THROW_EXCEPTION( exSyntax );
        
        vcs_check_initialized( );
        vcs_resolve_path( szLabelFile, pszPath );
        if ( strlen( szLabelFile ) + strlen( _szLabels ) >= sizeof( szLabelFile ) )
            THROW_EXCEPTION( exInternal );
        strcat( szLabelFile, _szLabels );
        vcs_log( "modifying label: %s - %s - %s", szLabelFile, pszBranch, pszLabel );

        vcs_tempfile( szLabelFileTmp, szLabelFile );
        
        vcs_resolve_path( szTarget, pszPath );
        if ( strlen( szTarget ) + strlen( _szSlash ) + strlen( pcmd->szTarget ) >= sizeof( szTarget ) )
            THROW_EXCEPTION( exInternal );
        strcat( szTarget, _szSlash );
        strcat( szTarget, pcmd->szTarget );
        vcs_fixup_target( szTarget );

        nMax = vcs_directory_count_i( szTarget, _A_SUBDIR );
        if ( !nMax )
            THROW_EXCEPTION( exIllegal );
        
        pd = (Direct *) malloc( nMax * sizeof( Direct ) );
        if ( !pd )
            THROW_EXCEPTION( exMemory );
        memset( pd, 0, nMax * sizeof( Direct ) );
        
        vcs_directory_i( szTarget, _A_SUBDIR, pd, nMax );

        pfileRead = fopen( szLabelFile, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        pfileWrite = fopen( szLabelFileTmp, _szBinaryWrite );
        if ( !pfileWrite )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        vcs_write_file_header( pfileWrite, &h );

        // Now, enter the state machine
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                st = stBefore;
                break;
            
            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    {
                    vcs_write_label_header( pfileWrite, &lh );

                    fDone = FALSE;
                    while ( !fDone )
                        {
                        vcs_read_label_rec( pfileRead, &rec );
                        switch( rec.type )
                            {
                        case eFileLabelRec:
                            fFound = vcs_find_direct_entry( &idx, pd, nMax, rec.szFile );
                            if ( !fFound )
                                vcs_write_label_rec( pfileWrite, &rec );
                            break;

                        case eDirLabelRec:
                            fFound = vcs_find_direct_entry( &idx, pd, nMax, rec.szDir );
                            if ( !fFound )
                                vcs_write_label_rec( pfileWrite, &rec );
                            break;

                        case eEmptyLabelRec:
                            fDone = TRUE;
                            break;
                            }
                        }
                    
                    vcs_write_empty_label_rec( pfileWrite );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_write_label_header( pfileWrite, &lh );
                    vcs_copy_label( pfileWrite, pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_write_label_header( pfileWrite, &lh );
                vcs_copy_label( pfileWrite, pfileRead );
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                break;
                }
            }

        free( pd );
        fclose( pfileRead );
        fclose( pfileWrite );
        
        vcs_check_ctrl_c( );
        vcs_commit( szLabelFile );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );

        if ( pfileWrite )
            fclose( pfileWrite );

        if ( pd )
            free( pd );

        vcs_abort( szLabelFile );

        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_label
//
// General Label manipulation
//////////////////////////////////////////////////////////////////////////////
void vcs_label( char *pszPath, const char *pszBranch, const char *pszLabel, LabelCmd *pcmd )
    {
    if ( pcmd->fAdd )
        vcs_labelcmd_add( pszPath, pszBranch, pszLabel, pcmd );
    else
        vcs_labelcmd_remove( pszPath, pszBranch, pszLabel, pcmd );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_read_label_header_check( )
//
// Read Label Header; check to see if label already exists
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_read_label_header_check( FILE *pfile, LabelHeader *plh, LabelHeader *plhCheck )
    {
    if ( !vcs_read_label_header( pfile, plh ) )
        return FALSE;

    if ( strcmp( plh->szBranch, plhCheck->szBranch ) == 0 &&
         strcmp( plh->szLabel,  plhCheck->szLabel  ) == 0 )
        THROW_EXCEPTION( exIllegal );

    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// Remove a Label
//////////////////////////////////////////////////////////////////////////////
void vcs_remove_label( char *pszPath, const char *pszBranch, const char *pszLabel )
    {
    char szPath      [ _cbPathMax + 1 ];
    char szPathTmp   [ _cbPathMax + 1 ];
    char szPathParent[ _cbPathMax + 1 ];
    char szPathChild [ _cbPathMax + 1 ];
    FILE *pfileRead  = NULL;
    FILE *pfileWrite = NULL;
    BOOL  fEOF       = FALSE;
    FileHeader  h;
    LabelHeader lh;
    unsigned    n;
    State       st;
    
    TRY
        {
        memset( szPath,       0, sizeof( szPath ) );
        memset( szPathTmp,    0, sizeof( szPathTmp ) );
        memset( szPathParent, 0, sizeof( szPathParent ) );
        memset( szPathChild,  0, sizeof( szPathChild ) );
        
        vcs_check_initialized( );
        vcs_resolve_path( szPath, pszPath );
        vcs_log( "removing label: %s - %s - %s", szPath, pszBranch, pszLabel );
        
        n = vcs_list_label_count_i( szPath, pszBranch, pszLabel );
        if ( n )
            THROW_EXCEPTION( exIllegal );

        if ( strcmp( szPath, _szVcsRoot ) != 0 )
            {
            vcs_splitpath( szPathParent, szPathChild, szPath );

            if ( vcs_label_is_referenced( szPathParent, szPathChild, pszBranch, pszLabel ) )
                THROW_EXCEPTION( exIllegal );
            }

        if ( strlen( szPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcat( szPath, _szLabels );
        vcs_tempfile( szPathTmp, szPath );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        pfileWrite = fopen( szPathTmp, _szBinaryWrite );
        if ( !pfileWrite )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        vcs_write_file_header( pfileWrite, &h );
        
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                st = stBefore;
                break;
            
            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    {
                    vcs_skip_label( pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_write_label_header( pfileWrite, &lh );
                    vcs_copy_label( pfileWrite, pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }
                
                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    THROW_EXCEPTION( exCorrupt );
                
                vcs_write_label_header( pfileWrite, &lh );
                vcs_copy_label( pfileWrite, pfileRead );
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                break;
                }
            }
        fclose( pfileWrite );
        fclose( pfileRead );
        
        vcs_check_ctrl_c( );
        vcs_commit( szPath );
        }
    CATCH_ALL
        {
        if ( pfileWrite )
            fclose( pfileWrite );

        if ( pfileRead )
            fclose( pfileRead );

        vcs_abort( szPath );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_create_label( )
//
// Creates a new label
//////////////////////////////////////////////////////////////////////////////
void vcs_create_label( char *pszPath, const char *pszBranch, const char *pszLabel )
    {
    char szPath   [ _cbPathMax + 1 ];
    char szPathTmp[ _cbPathMax + 1 ];
    FILE *pfileRead  = NULL;
    FILE *pfileWrite = NULL;
    FileHeader  h;
    LabelHeader lh;
    LabelHeader lhNew;
    LabelRec    rec;
    BOOL fEOF = FALSE;
    State st;

    TRY
        {
        memset( szPath,    0, sizeof( szPath ) );
        memset( szPathTmp, 0, sizeof( szPathTmp ) );
        
        vcs_check_initialized( );
        vcs_resolve_path( szPath, pszPath );
        vcs_log( "creating label: %s - %s - %s", szPath, pszBranch, pszLabel );

        vcs_valid_symbol( pszBranch );
        vcs_valid_symbol( pszLabel );
        if ( strlen( szPath ) + strlen( _szLabels ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcat( szPath, _szLabels );
        vcs_tempfile( szPathTmp, szPath );

        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        pfileWrite = fopen( szPathTmp, _szBinaryWrite );
        if ( !pfileWrite )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        vcs_write_file_header( pfileWrite, &h );

        memset( &lhNew, 0, sizeof( lhNew ) );
        if ( strlen( pszBranch ) >= sizeof( lhNew.szBranch ) )
            THROW_EXCEPTION( exSyntax );
        strcpy( lhNew.szBranch, pszBranch );

        if ( strlen( pszLabel ) >= sizeof( lhNew.szLabel ) )
            THROW_EXCEPTION( exSyntax );
        strcpy( lhNew.szLabel, pszLabel );

        time( &lhNew.tCreation );

        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header_check( pfileRead, &lh, &lhNew );
                st = stBefore;
                break;
            
            case stBefore:
                if ( fEOF )
                    {
                    vcs_write_label_header( pfileWrite, &lhNew );
                    vcs_write_empty_label_rec( pfileWrite );
                    st = stStop;
                    break;
                    }
                
                if ( strcmp( lhNew.szBranch, lh.szBranch ) == 0 )
                    {
                    vcs_write_label_header( pfileWrite, &lh );
                    vcs_copy_label( pfileWrite, pfileRead );
                    fEOF = !vcs_read_label_header_check( pfileRead, &lh, &lhNew );
                    st = stDuring;
                    }
                else
                    {
                    vcs_write_label_header( pfileWrite, &lh );
                    vcs_copy_label( pfileWrite, pfileRead );
                    fEOF = !vcs_read_label_header_check( pfileRead, &lh, &lhNew );
                    }
                break;
            
            case stDuring:
                if ( fEOF )
                    {
                    vcs_write_label_header( pfileWrite, &lhNew );
                    vcs_write_empty_label_rec( pfileWrite );
                    st = stStop;
                    break;
                    }
                if ( strcmp( lhNew.szBranch, lh.szBranch ) == 0 )
                    {
                    vcs_write_label_header( pfileWrite, &lh );
                    vcs_copy_label( pfileWrite, pfileRead );
                    fEOF = !vcs_read_label_header_check( pfileRead, &lh, &lhNew );
                    }
                else
                    {
                    vcs_write_label_header( pfileWrite, &lhNew );
                    vcs_write_empty_label_rec( pfileWrite );
                    vcs_write_label_header( pfileWrite, &lh );
                    vcs_copy_label( pfileWrite, pfileRead );
                    fEOF = !vcs_read_label_header_check( pfileRead, &lh, &lhNew );
                    st = stAfter;
                    }
                break;
            
            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }
                if ( strcmp( lhNew.szBranch, lh.szBranch ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_write_label_header( pfileWrite, &lh );
                vcs_copy_label( pfileWrite, pfileRead );
                fEOF = !vcs_read_label_header_check( pfileRead, &lh, &lhNew );
                break;
                }
            }

        fclose( pfileWrite );
        fclose( pfileRead );
        
        vcs_check_ctrl_c( );
        vcs_commit( szPath );
        }
    CATCH_ALL
        {
        if ( pfileWrite )
            fclose( pfileWrite );

        if ( pfileRead )
            fclose( pfileRead );

        vcs_abort( szPath );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_getfile( )
//
// Gets a File
//////////////////////////////////////////////////////////////////////////////
void vcs_getfile( char *pszTarget, const char *pszBranch, unsigned nVersion )
    {
    char     szTarget[ _cbPathMax + 1 ];
    char     szParent[ _cbPathMax + 1 ];
    char     szChild [ _cbPathMax + 1 ];
    char     szFile  [ _cbPathMax + 1 ];
    char     szPath  [ _cbPathMax + 1 ];
    Direct  *pd   = NULL;
    unsigned nMax = 0u;
    unsigned i    = 0u;
                                                         
    TRY
        {
        memset( szTarget, 0, sizeof( szTarget ) );
        memset( szParent, 0, sizeof( szParent ) );
        memset( szChild,  0, sizeof( szChild ) );
        memset( szFile,   0, sizeof( szFile ) );
        memset( szPath,   0, sizeof( szPath ) );

        vcs_check_initialized( );
        vcs_resolve_path( szTarget, pszTarget );

        nMax = vcs_directory_count_i( szTarget, _A_NORMAL );
        if ( !nMax )
            THROW_EXCEPTION( exIllegal );
        
        pd = (Direct *) malloc( nMax * sizeof( Direct ) );
        if ( !pd )
            THROW_EXCEPTION( exMemory );
        memset( pd, 0, nMax * sizeof( Direct ) );

        vcs_directory_i( szTarget, _A_NORMAL, pd, nMax );

        vcs_splitpath( szParent, szChild, szTarget );
        for ( i = 0u; i < nMax; i++ )
            {
            vcs_make_filename( szFile, pd[ i ].szFile, pd[ i ].szExt );
            vcs_makepath( szPath, szParent, szFile );
            vcs_getfile_i( szPath, pszBranch, nVersion, TRUE );
            }
        free( pd );
        }
    CATCH_ALL
        {
        if ( pd )
            free( pd );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_getlabel( )
//
// Get nodes in a Label
//////////////////////////////////////////////////////////////////////////////
void vcs_getlabel( char *pszPath, const char *pszBranch, const char *pszLabel )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );
    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    vcs_getlabel_i( szPath, _szEmpty, pszBranch, pszLabel );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_hidefile( )
//
// Hide Files
//////////////////////////////////////////////////////////////////////////////
void vcs_hidefile( char *pszTarget )
    {
    char     szTarget[ _cbPathMax + 1 ];
    char     szParent[ _cbPathMax + 1 ];
    char     szChild [ _cbPathMax + 1 ];
    char     szFile  [ _cbPathMax + 1 ];
    char     szPath  [ _cbPathMax + 1 ];
    Direct  *pd   = NULL;
    unsigned nMax = 0u;
    unsigned i    = 0u;
                                                         
    TRY
        {
        memset( szTarget, 0, sizeof( szTarget ) );
        memset( szParent, 0, sizeof( szParent ) );
        memset( szChild,  0, sizeof( szChild ) );
        memset( szFile,   0, sizeof( szFile ) );
        memset( szPath,   0, sizeof( szPath ) );

        vcs_check_initialized( );
        vcs_resolve_path( szTarget, pszTarget );

        nMax = vcs_directory_count_i( szTarget, _A_NORMAL );
        if ( !nMax )
            THROW_EXCEPTION( exIllegal );
        
        pd = (Direct *) malloc( nMax * sizeof( Direct ) );
        if ( !pd )
            THROW_EXCEPTION( exMemory );
        memset( pd, 0, nMax * sizeof( Direct ) );

        vcs_directory_i( szTarget, _A_NORMAL, pd, nMax );

        vcs_splitpath( szParent, szChild, szTarget );
        for ( i = 0u; i < nMax; i++ )
            {
            vcs_make_filename( szFile, pd[ i ].szFile, pd[ i ].szExt );
            vcs_makepath( szPath, szParent, szFile );
            vcs_hidefile_i( szPath );
            }
        free( pd );
        }
    CATCH_ALL
        {
        if ( pd )
            free( pd );
        THROW_LAST( );
        }
    END_CATCH
    }        
    
//////////////////////////////////////////////////////////////////////////////
// vcs_hidefile_i( )
//
// Hide Files
//////////////////////////////////////////////////////////////////////////////
void vcs_hidefile_i( char *pszPath )
    {
    char szParent[ _cbPathMax + 1 ];
    char szChild [ _cbPathMax + 1 ];
    char szPath  [ _cbPathMax + 1 ];

    memset( szParent, 0, sizeof( szParent ) );
    memset( szChild,  0, sizeof( szChild ) );
    memset( szPath,   0, sizeof( szPath ) );

    if ( strlen( pszPath ) >= sizeof( szPath ) )
        THROW_EXCEPTION( exInternal );
    strcpy( szPath, pszPath );

    vcs_splitpath( szParent, szChild, szPath );
    vcs_hidefile_i2( szParent, _szEmpty, szChild );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_hidelabel( )
//
// Hide Labels
//////////////////////////////////////////////////////////////////////////////
void vcs_hidelabel( char *pszPath, const char *pszBranch, const char *pszLabel )
    {
    char szPath[ _cbPathMax + 1 ];

    memset( szPath, 0, sizeof( szPath ) );

    vcs_check_initialized( );
    vcs_resolve_path( szPath, pszPath );
    vcs_hidelabel_i( szPath, _szEmpty, pszBranch, pszLabel );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_hidefile_i2( )
//
// Hide Files
//////////////////////////////////////////////////////////////////////////////
void vcs_hidefile_i2( char *pszPath, char *pszPathOffset, char *pszFile )
    {
    char szPathLocal [ _cbPathMax + 1 ];

    memset( szPathLocal, 0, sizeof( szPathLocal ) );
        
    if ( *pszPathOffset )
        {
        if ( strlen( pszPathOffset ) +
             strlen( _szSlash      ) +
             strlen( pszFile       ) >= sizeof( szPathLocal ) )
            THROW_EXCEPTION( exInternal );
           
        strcpy( szPathLocal, pszPathOffset );
        strcat( szPathLocal, _szSlash      );
        strcat( szPathLocal, pszFile       );
        }
    else
        {
        if ( strlen( pszFile ) >= sizeof( szPathLocal ) )
            THROW_EXCEPTION( exInternal );
            
        strcpy( szPathLocal, pszFile );
        }

    vcs_log( "hiding local file: %s", szPathLocal );
        
    if ( vcs_file_exists( szPathLocal ) )
        {
        if ( !vcs_get_readonly_bit( szPathLocal ) )
            THROW_EXCEPTION( exIllegal );
        vcs_set_readonly_bit( szPathLocal, FALSE );

        if ( remove( szPathLocal ) != 0 )
            THROW_EXCEPTION( exFile );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_hidelabel_i( )
//
// Hide Labels
//////////////////////////////////////////////////////////////////////////////
void vcs_hidelabel_i( char *pszPath, char *pszPathOffset, const char *pszBranch, const char *pszLabel )
    {
    FILE       *pfileRead = NULL;
    char        szPath      [ _cbPathMax + 1 ];
    char        szLabelPath [ _cbPathMax + 1 ];
    char        szPathOffset[ _cbPathMax + 1 ];
    char        szDir       [ _cbPathMax + 1 ];
    FileHeader  h;
    State       st;
    BOOL        fEOF  = FALSE;
    BOOL        fDone = FALSE;
    LabelHeader lh;
    LabelRec    rec;

    TRY
        {
        memset( szPath,       0, sizeof( szPath ) );
        memset( szLabelPath,  0, sizeof( szLabelPath ) );
        memset( szPathOffset, 0, sizeof( szPathOffset ) );
        
        if ( strlen( pszPath ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );

        if ( strlen( pszPathOffset ) >= sizeof( szPathOffset ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPathOffset, pszPathOffset );
        
        if ( *szPathOffset )
            {
            if ( strlen( pszPath      ) + 
                 strlen( _szSlash     ) + 
                 strlen( szPathOffset ) +
                 strlen( _szLabels    ) >= sizeof( szLabelPath ) )
                THROW_EXCEPTION( exInternal );
            strcpy( szLabelPath, pszPath );
            strcat( szLabelPath, _szSlash );
            strcat( szLabelPath, szPathOffset );
            strcat( szLabelPath, _szLabels );
            }
        else
            {
            if ( strlen( pszPath ) + strlen( _szLabels ) >= sizeof( szLabelPath ) )
                THROW_EXCEPTION( exInternal );
            strcpy( szLabelPath, pszPath );
            strcat( szLabelPath, _szLabels );
            }
        
        vcs_log( "hiding label: %s - %s - %s", szLabelPath, pszBranch, pszLabel );
        
        pfileRead = fopen( szLabelPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );
                    
                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    {
                    fDone = FALSE;
                    while ( !fDone )
                        {
                        vcs_read_label_rec( pfileRead, &rec );
                        switch( rec.type )
                            {
                        case eFileLabelRec:
                            vcs_hidefile_i2( szPath, szPathOffset, rec.szFile );
                            break;
                        
                        case eDirLabelRec:
                            memset( szDir, 0, sizeof( szDir ) );
                            if ( *szPathOffset )
                                {
                                if ( strlen( szPathOffset ) >= sizeof( szDir ) )
                                    THROW_EXCEPTION( exInternal );
                                strcpy( szDir, szPathOffset );
                                
                                if ( strlen( szDir ) + strlen( _szSlash ) >= sizeof( szDir ) )
                                    THROW_EXCEPTION( exInternal );
                                strcat( szDir, _szSlash );
                                }

                            if ( strlen( szDir ) + strlen( rec.szDir ) >= sizeof( szDir ) )
                                THROW_EXCEPTION( exInternal );
                            strcat( szDir, rec.szDir );
                            vcs_hidelabel_i( szPath, szDir, pszBranch, rec.szLabel );
                            break;

                        case eEmptyLabelRec:
                            fDone = TRUE;
                            break;
                            }
                        }

                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_skip_label( pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_skip_label( pfileRead );
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                break;
                }
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_getlabel_i( )
//
// Get nodes in a Label
//////////////////////////////////////////////////////////////////////////////
void vcs_getlabel_i( char *pszPath, char *pszPathOffset, const char *pszBranch, const char *pszLabel )
    {
    FILE       *pfileRead = NULL;
    char        szPath      [ _cbPathMax + 1 ];
    char        szLabelPath [ _cbPathMax + 1 ];
    char        szPathOffset[ _cbPathMax + 1 ];
    char        szDir       [ _cbPathMax + 1 ];
    FileHeader  h;
    State       st;
    BOOL        fEOF  = FALSE;
    BOOL        fDone = FALSE;
    LabelHeader lh;
    LabelRec    rec;

    TRY
        {
        memset( szPath,       0, sizeof( szPath ) );
        memset( szLabelPath,  0, sizeof( szLabelPath ) );
        memset( szPathOffset, 0, sizeof( szPathOffset ) );
        
        if ( strlen( pszPath ) >= sizeof( szPath ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPath, pszPath );

        if ( strlen( pszPathOffset ) >= sizeof( szPathOffset ) )
            THROW_EXCEPTION( exInternal );
        strcpy( szPathOffset, pszPathOffset );
        
        if ( *szPathOffset )
            {
            if ( strlen( pszPath      ) + 
                 strlen( _szSlash     ) + 
                 strlen( szPathOffset ) +
                 strlen( _szLabels    ) >= sizeof( szLabelPath ) )
                THROW_EXCEPTION( exInternal );
            strcpy( szLabelPath, pszPath );
            strcat( szLabelPath, _szSlash );
            strcat( szLabelPath, szPathOffset );
            strcat( szLabelPath, _szLabels );
            }
        else
            {
            if ( strlen( pszPath ) + strlen( _szLabels ) >= sizeof( szLabelPath ) )
                THROW_EXCEPTION( exInternal );
            strcpy( szLabelPath, pszPath );
            strcat( szLabelPath, _szLabels );
            }
        
        vcs_log( "getting label: %s - %s - %s", szLabelPath, pszBranch, pszLabel );
        
        pfileRead = fopen( szLabelPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eLabelFile )
            THROW_EXCEPTION( exCorrupt );
        
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );
                    
                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    {
                    fDone = FALSE;
                    while ( !fDone )
                        {
                        vcs_read_label_rec( pfileRead, &rec );
                        switch( rec.type )
                            {
                        case eFileLabelRec:
                            vcs_getfile_i2( szPath, szPathOffset, rec.szFile, pszBranch, rec.nVersion, TRUE );
                            break;
                        
                        case eDirLabelRec:
                            memset( szDir, 0, sizeof( szDir ) );
                            if ( *szPathOffset )
                                {
                                if ( strlen( szPathOffset ) >= sizeof( szDir ) )
                                    THROW_EXCEPTION( exInternal );
                                strcpy( szDir, szPathOffset );
                                
                                if ( strlen( szDir ) + strlen( _szSlash ) >= sizeof( szDir ) )
                                    THROW_EXCEPTION( exInternal );
                                strcat( szDir, _szSlash );
                                }

                            if ( strlen( szDir ) + strlen( rec.szDir ) >= sizeof( szDir ) )
                                THROW_EXCEPTION( exInternal );
                            strcat( szDir, rec.szDir );

                            if ( !vcs_file_exists( szDir ) )
                                {
                                vcs_log( "creating local directory: %s", szDir );
                                if ( mkdir( szDir ) != 0 )
                                    THROW_EXCEPTION( exFile );
                                }
                            vcs_getlabel_i( szPath, szDir, pszBranch, rec.szLabel );
                            break;

                        case eEmptyLabelRec:
                            fDone = TRUE;
                            break;
                            }
                        }

                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_skip_label( pfileRead );
                    fEOF = !vcs_read_label_header( pfileRead, &lh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( lh.szBranch, pszBranch ) == 0 &&
                     strcmp( lh.szLabel,  pszLabel  ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_skip_label( pfileRead );
                fEOF = !vcs_read_label_header( pfileRead, &lh );
                break;
                }
            }
        fclose( pfileRead );
        }
    CATCH_ALL
        {
        if ( pfileRead )
            fclose( pfileRead );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_getfile_i( )
//
// Gets a File
//////////////////////////////////////////////////////////////////////////////
void vcs_getfile_i( char *pszPath, const char *pszBranch, unsigned nVersion, BOOL fReadOnly )
    {
    char szParent[ _cbPathMax + 1 ];
    char szChild [ _cbPathMax + 1 ];
    char szPath  [ _cbPathMax + 1 ];

    if ( strlen( pszPath ) >= sizeof( szPath ) )
        THROW_EXCEPTION( exInternal );
    strcpy( szPath, pszPath );

    vcs_splitpath( szParent, szChild, szPath );
    vcs_getfile_i2( szParent, 
                    _szEmpty, 
                    szChild, 
                    pszBranch, 
                    vcs_resolve_version( szPath, pszBranch, nVersion ), 
                    fReadOnly );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_resolve_version( )
//
// Resolves version numbers: if the version is 0, gets the
// latest version.
//////////////////////////////////////////////////////////////////////////////
unsigned vcs_resolve_version( char *pszPath, const char *pszBranch, unsigned nVersion )
    {
    if ( nVersion == 0u )
        {
        nVersion = vcs_get_latest_version( pszPath, pszBranch );
        if ( nVersion == 0u )
            THROW_EXCEPTION( exIllegal );
        }
    
    return nVersion;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_getfile_i2( )
//
// Gets a File
//////////////////////////////////////////////////////////////////////////////
void vcs_getfile_i2( char *pszPath, char *pszPathOffset, char *pszFile, const char *pszBranch, unsigned nVersion, BOOL fReadOnly )
    {
    FILE *pfileRead  = NULL;
    FILE *pfileWrite = NULL;
    char szPathVCS   [ _cbPathMax + 1 ];
    char szPathLocal [ _cbPathMax + 1 ];
    FileHeader h;
    FileVersionHeader vh;
    BOOL fEOF    = FALSE;
    BOOL fCopied = FALSE;
    State st;
    unsigned _dos_date = 0u;
    unsigned _dos_time = 0u;

    TRY
        {
        memset( szPathVCS,    0, sizeof( szPathVCS ) );
        memset( szPathLocal,  0, sizeof( szPathLocal ) );
        
        if ( *pszPathOffset )
            {
            if ( strlen( pszPath       ) + 
                 strlen( _szSlash      ) + 
                 strlen( pszPathOffset ) +
                 strlen( _szSlash      ) +
                 strlen( pszFile       ) >= sizeof( szPathVCS ) )
                THROW_EXCEPTION( exInternal );

            strcpy( szPathVCS, pszPath       );
            strcat( szPathVCS, _szSlash      );
            strcat( szPathVCS, pszPathOffset );
            strcat( szPathVCS, _szSlash      );
            strcat( szPathVCS, pszFile       );
            
            if ( strlen( pszPathOffset ) +
                 strlen( _szSlash      ) +
                 strlen( pszFile       ) >= sizeof( szPathLocal ) )
                THROW_EXCEPTION( exInternal );
            
            strcpy( szPathLocal, pszPathOffset );
            strcat( szPathLocal, _szSlash      );
            strcat( szPathLocal, pszFile       );
            }
        else
            {
            if ( strlen( pszPath  ) + 
                 strlen( _szSlash ) +
                 strlen( pszFile  ) >= sizeof( szPathVCS ) )
                THROW_EXCEPTION( exInternal );

            strcpy( szPathVCS, pszPath  );
            strcat( szPathVCS, _szSlash );
            strcat( szPathVCS, pszFile  );
            
            if ( strlen( pszFile ) >= sizeof( szPathLocal ) )
                THROW_EXCEPTION( exInternal );
            
            strcpy( szPathLocal, pszFile );
            }

        vcs_log( "creating local file: %s - %s - %u", szPathLocal, 
                                                      pszBranch, 
                                                      nVersion );

        pfileRead = fopen( szPathVCS, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );
        
        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eDataFile )
            THROW_EXCEPTION( exCorrupt );

        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );
                
                if ( strcmp( vh.szBranch, pszBranch ) == 0 && 
                     vh.nVersion == nVersion &&
                     vh.fLock )
                    {
                    if ( vcs_file_exists( szPathLocal ) )
                        {
                        if ( !vcs_get_readonly_bit( szPathLocal ) )
                            THROW_EXCEPTION( exIllegal );

                        vcs_dos_getftime( szPathLocal, &_dos_date, &_dos_time );
                        if ( vh._dos_date == _dos_date && vh._dos_time == _dos_time )
                            {
                            vcs_read_space( pfileRead, vh.cb );
                            }
                        else
                            {
                            vcs_set_readonly_bit( szPathLocal, FALSE );
                            vcs_file_remove( szPathLocal );

                            _dos_date = vh._dos_date;
                            _dos_time = vh._dos_time;

                            pfileWrite = fopen( szPathLocal, _szBinaryWrite );
                            if ( !pfileWrite )
                                THROW_EXCEPTION( exFile );
                            fCopied = TRUE;
                            vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                            }
                        }
                    else
                        {
                        _dos_date = vh._dos_date;
                        _dos_time = vh._dos_time;

                        pfileWrite = fopen( szPathLocal, _szBinaryWrite );
                        if ( !pfileWrite )
                            THROW_EXCEPTION( exFile );
                        fCopied = TRUE;
                        vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                        }
                    
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_read_space( pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( vh.szBranch, pszBranch ) == 0 &&
                     vh.nVersion == nVersion )
                    THROW_EXCEPTION( exCorrupt );

                vcs_read_space( pfileRead, vh.cb );
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                break;
                }
            }
        
        if ( pfileRead )
            fclose( pfileRead );
        if ( pfileWrite )
            fclose( pfileWrite );
        
        if ( fCopied )
            vcs_dos_setftime( szPathLocal, _dos_date, _dos_time );
            
        vcs_set_readonly_bit( szPathLocal, fReadOnly );
        }
    CATCH( exCtrlC )
        {
        if ( pfileWrite )
            fclose( pfileWrite );
            
        if ( pfileRead )
            fclose( pfileRead );
        
        if ( fCopied )
            remove( szPathLocal );
        THROW_LAST( );
        }
    CATCH_ALL
        {
        if ( pfileWrite )
            fclose( pfileWrite );
            
        if ( pfileRead )
            fclose( pfileRead );
        
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_lock_version( )
//
// This routine will lock a Version
//////////////////////////////////////////////////////////////////////////////
void vcs_lock_version( char *pszPath, const char *pszBranch )
    {
    FILE *pfileRead  = NULL;
    FILE *pfileWrite = NULL;
    FILE *pfileLocal = NULL;
    char szPath     [ _cbPathMax + 1 ];
    char szPathTmp  [ _cbPathMax + 1 ];
    char szParent   [ _cbPathMax + 1 ];
    char szLocalFile[ _cbPathMax + 1 ];
    FileHeader h;
    FileVersionHeader vh;
    BOOL fEOF = FALSE;
    struct stat ss;
    
    // Local File attributes
    long   cbLocal;
    time_t tLocal;
    State  st;
    unsigned _dos_date = 0u;
    unsigned _dos_time = 0u;

    TRY
        {
        memset( szPath,      0, sizeof( szPath ) );
        memset( szPathTmp,   0, sizeof( szPathTmp ) );
        memset( szParent,    0, sizeof( szParent ) );
        memset( szLocalFile, 0, sizeof( szLocalFile ) );

        vcs_check_initialized( );
        vcs_resolve_path( szPath, pszPath );
        vcs_log( "locking version: %s - %s", szPath, pszBranch );

        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_tempfile( szPathTmp, szPath );
        pfileWrite = fopen( szPathTmp, _szBinaryWrite );
        if ( !pfileWrite )
            THROW_EXCEPTION( exFile );
        
        vcs_splitpath( szParent, szLocalFile, szPath );
        
        if ( stat( szLocalFile, &ss ) < 0 )
            THROW_EXCEPTION( exFile );
        cbLocal = ss.st_size;
        tLocal  = ss.st_mtime;
        vcs_dos_getftime( szLocalFile, &_dos_date, &_dos_time );
        
        pfileLocal = fopen( szLocalFile, _szBinaryRead );
        if ( !pfileLocal )
            THROW_EXCEPTION( exFile );

        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eDataFile )
            THROW_EXCEPTION( exCorrupt );

        vcs_write_file_header( pfileWrite, &h );
        
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );

                if ( strcmp( vh.szBranch, pszBranch ) == 0 && !vh.fLock )
                    {
                    vh.cb        = cbLocal;
                    vh.t         = tLocal;
                    vh._dos_date = _dos_date;
                    vh._dos_time = _dos_time;
                    vh.fLock     = TRUE;
                    
                    vcs_write_version_header( pfileWrite, &vh );
                    vcs_copy_version( pfileWrite, pfileLocal, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_write_version_header( pfileWrite, &vh );
                    vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                // 2nd unlocked version found within the same branch
                if ( strcmp( vh.szBranch, pszBranch ) == 0 && !vh.fLock )
                    THROW_EXCEPTION( exCorrupt );

                vcs_write_version_header( pfileWrite, &vh );
                vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                break;
                }
            }
        fclose( pfileWrite );
        fclose( pfileRead  );
        fclose( pfileLocal );

        vcs_check_ctrl_c( );
        vcs_commit( szPath );
        vcs_set_readonly_bit( szLocalFile, TRUE );
        }
    CATCH_ALL
        {
        if ( pfileWrite )
            fclose( pfileWrite );

        if ( pfileRead )
            fclose( pfileRead );

        if ( pfileLocal )
            fclose( pfileLocal );
        
        vcs_abort( szPath );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_remove_version
//
// This routine will remove a Version
//////////////////////////////////////////////////////////////////////////////
void vcs_remove_version( char *pszPath, const char *pszBranch, unsigned nVersion )
    {
    FILE *pfileRead  = NULL;
    FILE *pfileWrite = NULL;
    char szPath      [ _cbPathMax + 1 ];
    char szParentPath[ _cbPathMax + 1 ];
    char szChildPath [ _cbPathMax + 1 ]; 
    char szPathTmp   [ _cbPathMax + 1 ];
    FileHeader        h;
    FileVersionHeader vh;
    BOOL fEOF        = FALSE;
    State st;
    
    TRY
        {
        memset( szPath,       0, sizeof( szPath ) );
        memset( szPathTmp,    0, sizeof( szPathTmp ) );
        memset( szParentPath, 0, sizeof( szParentPath ) );
        memset( szChildPath,  0, sizeof( szChildPath ) );

        vcs_check_initialized( );
        vcs_resolve_path( szPath, pszPath );
        vcs_log( "removing version: %s - %s - %u", szPath, pszBranch, nVersion );

        vcs_splitpath( szParentPath, szChildPath, szPath );

        if ( vcs_version_is_referenced( szParentPath, szChildPath, pszBranch, nVersion ) )
            THROW_EXCEPTION( exIllegal );

        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_tempfile( szPathTmp, szPath );
        pfileWrite = fopen( szPathTmp, _szBinaryWrite );
        if ( !pfileWrite )
            THROW_EXCEPTION( exFile );
        
        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eDataFile )
            THROW_EXCEPTION( exCorrupt );

        vcs_write_file_header( pfileWrite, &h );
        
        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    THROW_EXCEPTION( exIllegal );
                
                if ( strcmp( vh.szBranch, pszBranch ) == 0 &&
                     vh.nVersion == nVersion )
                    {
                    vcs_read_space( pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    st = stAfter;
                    }
                else
                    {
                    vcs_write_version_header( pfileWrite, &vh );
                    vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }

                if ( strcmp( vh.szBranch, pszBranch ) == 0 &&
                     vh.nVersion == nVersion )
                    THROW_EXCEPTION( exCorrupt );

                vcs_write_version_header( pfileWrite, &vh );
                vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                break;
                }
            }
        
        fclose( pfileWrite );
        fclose( pfileRead );
        
        vcs_check_ctrl_c( );
        vcs_commit( szPath );

        if ( vcs_file_exists( szChildPath ) )
            {
            if ( vcs_get_readonly_bit( szChildPath ) )
                {
                vcs_log( "warning: removing local file: %s", szChildPath );
                vcs_set_readonly_bit( szChildPath, FALSE );
                vcs_file_remove( szChildPath );
                }
            else
                {
                vcs_log( "warning: not removing read-write local file: %s", szChildPath );
                }
            }
        }
    CATCH_ALL
        {
        if ( pfileWrite )
            fclose( pfileWrite );
            
        if ( pfileRead )
            fclose( pfileRead );
        
        vcs_abort( szPath );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_create_version( )
//
// This routine will create a Version
//////////////////////////////////////////////////////////////////////////////
void vcs_create_version( char *pszPath, const char *pszBranch )
    {
    FILE *pfileRead  = NULL;
    FILE *pfileWrite = NULL;
    char szPath      [ _cbPathMax + 1 ];
    char szPathTmp   [ _cbPathMax + 1 ];
    char szParentPath[ _cbPathMax + 1 ];
    char szChildPath [ _cbPathMax + 1 ]; 
    FileHeader        h;
    FileVersionHeader vh;
    FileVersionHeader vhNew;
    BOOL fEOF        = FALSE;
    State st;
    unsigned nVersionSync = 0u; // Version to synchronize Local File

    TRY
        {
        memset( szPath,    0, sizeof( szPath ) );
        memset( szPathTmp, 0, sizeof( szPathTmp ) );
        memset( szParentPath, 0, sizeof( szParentPath ) );
        memset( szChildPath,  0, sizeof( szChildPath ) );

        vcs_check_initialized( );
        vcs_resolve_path( szPath, pszPath );
        vcs_log( "creating version: %s - %s", szPath, pszBranch );
        
        vcs_valid_symbol( pszBranch );

        vcs_splitpath( szParentPath, szChildPath, szPath );
        
        pfileRead = fopen( szPath, _szBinaryRead );
        if ( !pfileRead )
            THROW_EXCEPTION( exFile );

        vcs_tempfile( szPathTmp, szPath );
        pfileWrite = fopen( szPathTmp, _szBinaryWrite );
        if ( !pfileWrite )
            THROW_EXCEPTION( exFile );
        
        vcs_read_file_header( pfileRead, &h );
        if ( h.ft != eDataFile )
            THROW_EXCEPTION( exCorrupt );

        vcs_write_file_header( pfileWrite, &h );

        memset( &vhNew, 0, sizeof( vhNew ) );
        if ( strlen( pszBranch ) >= sizeof( vhNew.szBranch ) )
            THROW_EXCEPTION( exSyntax );
        strcpy( vhNew.szBranch, pszBranch );
        vhNew.nVersion = 1u;

        st = stStart;
        while ( st != stStop )
            {
            switch( st )
                {
            case stStart:
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                st = stBefore;
                break;

            case stBefore:
                if ( fEOF )
                    {
                    vcs_write_version_header( pfileWrite, &vhNew );
                    st = stStop;
                    break;
                    }
                
                if ( strcmp( vh.szBranch, vhNew.szBranch ) == 0 )
                    {
                    if ( !vh.fLock )
                        THROW_EXCEPTION( exIllegal );
                    nVersionSync = vh.nVersion;
                    vhNew.nVersion = vh.nVersion + 1;
                    vcs_write_version_header( pfileWrite, &vh );
                    vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    st = stDuring;
                    }
                else
                    {
                    vcs_write_version_header( pfileWrite, &vh );
                    vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    }
                break;

            case stDuring:
                if ( fEOF )
                    {
                    vcs_write_version_header( pfileWrite, &vhNew );
                    st = stStop;
                    break;
                    }
                
                if ( strcmp( vh.szBranch, vhNew.szBranch ) == 0 )
                    {
                    if ( !vh.fLock )
                        THROW_EXCEPTION( exIllegal );
                    nVersionSync = vh.nVersion;
                    vhNew.nVersion = vh.nVersion + 1;
                    vcs_write_version_header( pfileWrite, &vh );
                    vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    }
                else
                    {
                    vcs_write_version_header( pfileWrite, &vhNew );
                    vcs_write_version_header( pfileWrite, &vh );
                    vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                    fEOF = !vcs_read_version_header( pfileRead, &vh );
                    st = stAfter;
                    }
                break;

            case stAfter:
                if ( fEOF )
                    {
                    st = stStop;
                    break;
                    }
                
                if ( strcmp( vh.szBranch, vhNew.szBranch ) == 0 )
                    THROW_EXCEPTION( exCorrupt );

                vcs_write_version_header( pfileWrite, &vh );
                vcs_copy_version( pfileWrite, pfileRead, vh.cb );
                fEOF = !vcs_read_version_header( pfileRead, &vh );
                break;
                }
            }
        fclose( pfileWrite );
        fclose( pfileRead );
        
        vcs_check_ctrl_c( );
        vcs_commit( szPath );
        
        // Now, perform a getfile with nVersionSync.  Leave
        // the file read-write
        if ( nVersionSync == 0u )
            {
            vcs_log( "warning: not syncing first version: %s", szChildPath );
            }
        else if ( vcs_file_exists( szChildPath ) && !vcs_get_readonly_bit( szChildPath ) )
            {
            vcs_log( "warning: not replacing read-write local file: %s", szChildPath );
            }
        else
            {
            vcs_getfile_i( szPath, pszBranch, nVersionSync, FALSE );
            }
        }
    CATCH_ALL
        {
        if ( pfileWrite )
            fclose( pfileWrite );
            
        if ( pfileRead )
            fclose( pfileRead );
        
        vcs_abort( szPath );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_ctime( )
//
// Wrapper for ctime( ), but removes \n character
//////////////////////////////////////////////////////////////////////////////
char *vcs_ctime( const time_t *timer )
    {
    char  *pszTime;
    char  *pch;
    
    pszTime = ctime( timer );
    pch     = strchr( pszTime, '\n' );
    if ( pch )
        *pch = '\0';
    return pszTime;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_makepath( )
//
// Customized Make Path
//
// Assumptions:
//     . pszPath is of length _cbPathMax + 1
//////////////////////////////////////////////////////////////////////////////
void vcs_makepath( char *pszPath, char *pszPathParent, char *pszPathChild )
    {
    if ( strlen( pszPathParent ) + 1 + strlen( pszPathChild ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );
    sprintf( pszPath, "%s\\%s", pszPathParent, pszPathChild );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_splitpath( )
//
// Customized Split Path.
//
// Assumptions:
//
//     . pszPathParent and pszChildPath are of length _cbPathMax + 1
//     . pszPath has already had its trailing slash removed
//////////////////////////////////////////////////////////////////////////////
void vcs_splitpath( char *pszPathParent, char *pszPathChild, char *pszPath )
    {
    char szDrive[ _MAX_DRIVE + 1 ];
    char szDir  [ _MAX_DIR   + 1 ];
    char szFile [ _MAX_FNAME + 1 ];
    char szExt  [ _MAX_EXT   + 1 ];

    _splitpath( pszPath, szDrive, szDir, szFile, szExt );
    if ( strlen( szDrive ) + strlen( szDir ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );
    strcpy( pszPathParent, szDrive );
    strcat( pszPathParent, szDir );
    vcs_remove_trailing_slash( pszPathParent );

    if ( strlen( szFile ) + strlen( szExt ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );
    strcpy( pszPathChild, szFile );
    strcat( pszPathChild, szExt );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_valid_symbol( )
//
// Valid Symbol check (throws exSyntax if not valid )
//////////////////////////////////////////////////////////////////////////////
void vcs_valid_symbol( const char *pszSymbol )
    {
    const char *pch;

    if ( strlen( pszSymbol ) > _cbSymbolMax )
        THROW_EXCEPTION( exSyntax );
    
    pch = pszSymbol;

    if ( !isalpha( *pch ) && *pch != '_' )
        THROW_EXCEPTION( exSyntax );

    pch++;

    while ( *pch )
        {
        if ( !isalnum( *pch ) && *pch != '_' && *pch != '.' )
            THROW_EXCEPTION( exSyntax );
        pch++;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_resolve_path_reserved( )
//
// Resolves a VCS path to a DOS path.  Does not check for a
// .VCS extension
//
// ASSUMPTIONS: DOS-Path must be of size _cbPathMax + 1
//////////////////////////////////////////////////////////////////////////////
void vcs_resolve_path_reserved( char *pszDosPath, char *pszVcsPath )
    {
    if ( !*pszVcsPath )
        THROW_EXCEPTION( exSyntax );
    
    if ( pszVcsPath[ 0 ] == '\\' && pszVcsPath[ 1 ] == '\\' )
        {
        // Absolute path
        
        // skip over the \\ in pszVcsPath
        if ( strlen( _szVcsRoot ) + 1 + strlen( pszVcsPath + 2 ) > _cbPathMax )
            THROW_EXCEPTION( exInternal );
        sprintf( pszDosPath, "%s\\%s", _szVcsRoot, pszVcsPath + 2 );
        }
    else if ( strcmp( pszVcsPath, _szDot ) == 0 )
        {
        // . path

        if ( _szVcsRPath[ 0 ] )
            {
            if ( strlen( _szVcsRoot ) + 1 + strlen( _szVcsRPath ) > _cbPathMax )
                THROW_EXCEPTION( exInternal );
            sprintf( pszDosPath, "%s\\%s", _szVcsRoot, _szVcsRPath );
            }
        else
            {
            if ( strlen( _szVcsRoot ) > _cbPathMax )
                THROW_EXCEPTION( exInternal );
            strcpy( pszDosPath, _szVcsRoot );
            }
        }
    else
        {
        // Relative path
        
        if ( _szVcsRPath[ 0 ] )
            {
            if ( strlen( _szVcsRoot ) + 1 + strlen( _szVcsRPath ) + 1 + strlen( pszVcsPath ) > _cbPathMax )
                THROW_EXCEPTION( exInternal );
            sprintf( pszDosPath, "%s\\%s\\%s", _szVcsRoot, _szVcsRPath, pszVcsPath );
            }
        else
            {
            if ( strlen( _szVcsRoot ) + 1 + strlen( pszVcsPath ) > _cbPathMax )
                THROW_EXCEPTION( exInternal );
            sprintf( pszDosPath, "%s\\%s", _szVcsRoot, pszVcsPath );
            }
        }
    strupr( pszDosPath );
    strupr( pszVcsPath );
    
    vcs_remove_trailing_slash( pszDosPath );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_resolve_path( )
//
// Resolves a VCS path to a DOS path
//
// ASSUMPTIONS: DOS-Path must be of size _cbPathMax + 1
//////////////////////////////////////////////////////////////////////////////
void vcs_resolve_path( char *pszDosPath, char *pszVcsPath )
    {
    vcs_resolve_path_reserved( pszDosPath, pszVcsPath );

    if ( vcs_is_reserved( pszDosPath ) )
        THROW_EXCEPTION( exSyntax );
    }

