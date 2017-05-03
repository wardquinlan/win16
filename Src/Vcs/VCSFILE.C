//////////////////////////////////////////////////////////////////////////////
// VCSFILE.C
//
// Implements file management
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include "vcs.h"
#include "vcsfile.h"

char _szNewFileTmp[ ] = "TMPNEW";
char _szNewExtTmp [ ] = ".VCS";
char _szOldFileTmp[ ] = "TMPOLD";
char _szOldExtTmp [ ] = ".VCS";

static unsigned char _rgbBuffer[ _cbBlock ];

//////////////////////////////////////////////////////////////////////////////
// vcs_tempfile( )
//
// Returns a temporary file given a VCS file
//////////////////////////////////////////////////////////////////////////////
void vcs_tempfile( char *pszTmpFile, char *pszFile )
    {
    char szDrive[ _MAX_DRIVE + 1 ];
    char szDir  [ _MAX_DIR   + 1 ];
    char szFile [ _MAX_FNAME + 1 ];
    char szExt  [ _MAX_EXT   + 1 ];

    _splitpath( pszFile, szDrive, szDir, szFile, szExt );
    if ( strlen( szDrive ) + strlen( szDir ) + strlen( _szNewFileTmp ) + strlen( _szNewExtTmp ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );
    _makepath ( pszTmpFile, szDrive, szDir, _szNewFileTmp, _szNewExtTmp );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_get_readonly_bit( )
//
// ReadOnly Get Function
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_get_readonly_bit( char *pszPath )
    {
    unsigned flag;
    if ( _dos_getfileattr( pszPath, &flag ) != 0 )
        THROW_EXCEPTION( exFile );

    return ( flag & _A_RDONLY );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_set_readonly_bit( )
//
// ReadOnly Set Function
//////////////////////////////////////////////////////////////////////////////
void vcs_set_readonly_bit( char *pszPath, BOOL fSet )
    {
    unsigned flag;

    if ( _dos_getfileattr( pszPath, &flag ) != 0 )
        THROW_EXCEPTION( exFile );

    if ( fSet )
        {
        if ( !( flag & _A_RDONLY ) )
            {
            flag |= _A_RDONLY;
            if ( _dos_setfileattr( pszPath, flag ) != 0 )
                THROW_EXCEPTION( exFile );
            }
        }
    else
        {
        if ( flag & _A_RDONLY )
            {
            flag &= ( ~_A_RDONLY );
            if ( _dos_setfileattr( pszPath, flag ) != 0 )
                THROW_EXCEPTION( exFile );
            }
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_file_exists( )
//
// Returns TRUE if the given file exists
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_file_exists( char *pszPath )
    {
    return ( access( pszPath, 0 ) == 0 );
    }
     
//////////////////////////////////////////////////////////////////////////////
// vcs_is_reserved( )
//
// Returns TRUE if the given path is reserved
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_is_reserved( char *pszPath )
    {
    char szDrive[ _MAX_DRIVE + 1 ];
    char szDir  [ _MAX_DIR   + 1 ];
    char szFile [ _MAX_FNAME + 1 ];
    char szExt  [ _MAX_EXT   + 1 ];
    
    _splitpath( pszPath, szDrive, szDir, szFile, szExt );
    return ( strcmp( szExt, ".VCS" ) == 0 );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_abort( )
//
// Removes temporary files, but will not throw an exception
//////////////////////////////////////////////////////////////////////////////
void vcs_abort( char *pszFile )
    {
    char szDrive [ _MAX_DRIVE + 1 ];
    char szDir   [ _MAX_DIR   + 1 ];
    char szFile  [ _MAX_FNAME + 1 ];
    char szExt   [ _MAX_EXT   + 1 ];
    char szOldTmp[ _cbPathMax + 1 ];
    char szNewTmp[ _cbPathMax + 1 ];

    if ( !*pszFile )
        return;

    _splitpath( pszFile,  szDrive, szDir, szFile, szExt );

    if ( strlen( szDrive ) + strlen( szDir ) + strlen( _szOldFileTmp ) + strlen( _szOldExtTmp ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );
    if ( strlen( szDrive ) + strlen( szDir ) + strlen( _szNewFileTmp ) + strlen( _szNewExtTmp ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );

    _makepath ( szOldTmp, szDrive, szDir, _szOldFileTmp, _szOldExtTmp );
    _makepath ( szNewTmp, szDrive, szDir, _szNewFileTmp, _szNewExtTmp );
    
    remove( szOldTmp );
    remove( szNewTmp );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_file_remove( )
//
// Removes a file
//////////////////////////////////////////////////////////////////////////////
void vcs_file_remove( char *pszPath )
    {
    if ( remove( pszPath ) != 0 )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_file_rename( )
//
// Renames a file
//////////////////////////////////////////////////////////////////////////////
void vcs_file_rename( char *pszOldName, char *pszNewName )
    {
    if ( rename( pszOldName, pszNewName ) != 0 )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_read_version_header( )
//
// Reads the file version header
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_read_version_header( FILE *pfile, FileVersionHeader *ph )
    {
    if ( fread( ph, sizeof( *ph ), 1, pfile ) < 1 )
        return FALSE;
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_write_version_header( )
//
// Writes the file version header
//////////////////////////////////////////////////////////////////////////////
void vcs_write_version_header( FILE *pfile, FileVersionHeader *ph )
    {
    if ( fwrite( ph, sizeof( *ph ), 1, pfile ) < 1 )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_read_space( )
//
// Reads cb bytes from a file, ignoring their contents
//////////////////////////////////////////////////////////////////////////////
void vcs_read_space( FILE *pfileRead, long cb )
    {
    size_t cbBlock;

    while ( cb > 0l )
        {
        cbBlock = (size_t) min( cb, _cbBlock );
        if ( fread( _rgbBuffer, cbBlock, 1, pfileRead ) < 1 )
            THROW_EXCEPTION( exCorrupt );

        cb -= cbBlock;
        vcs_check_ctrl_c( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_copy_version( )
//
// Copies cb bytes from pfileRead to pfileWrite
//////////////////////////////////////////////////////////////////////////////
void vcs_copy_version( FILE *pfileWrite, FILE *pfileRead, long cb )
    {
    size_t cbBlock;

    while ( cb > 0l )
        {
        cbBlock = (size_t) min( cb, _cbBlock );
        if ( fread ( _rgbBuffer, cbBlock, 1, pfileRead  ) < 1 )
            THROW_EXCEPTION( exCorrupt );

        if ( fwrite( _rgbBuffer, cbBlock, 1, pfileWrite ) < 1 )
            THROW_EXCEPTION( exFile );

        cb -= cbBlock;
        vcs_check_ctrl_c( );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_read_file_header( )
//
// Reads the file header
//////////////////////////////////////////////////////////////////////////////
void vcs_read_file_header( FILE *pfile, FileHeader *ph )
    {
    if ( fread( ph, sizeof( *ph ), 1, pfile ) < 1 )
        THROW_EXCEPTION( exCorrupt );

    if ( ph->nVCSVersion != _nVCSVersion )
        THROW_EXCEPTION( exCorrupt );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_write_file_header( )
//
// Writes the file header
//////////////////////////////////////////////////////////////////////////////
void vcs_write_file_header( FILE *pfile, FileHeader *ph )
    {
    if ( fwrite( ph, sizeof( *ph ), 1, pfile ) < 1 )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_commit( )
//
// Commits a file
//////////////////////////////////////////////////////////////////////////////
void vcs_commit( char *pszFile )
    {
    BOOL f;
    char szDrive [ _MAX_DRIVE + 1 ];
    char szDir   [ _MAX_DIR   + 1 ];
    char szFile  [ _MAX_FNAME + 1 ];
    char szExt   [ _MAX_EXT   + 1 ];
    char szOldTmp[ _cbPathMax + 1 ];
    char szNewTmp[ _cbPathMax + 1 ];

    _splitpath( pszFile,  szDrive, szDir, szFile, szExt );

    if ( strlen( szDrive ) + strlen( szDir ) + strlen( _szOldFileTmp ) + strlen( _szOldExtTmp ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );
    if ( strlen( szDrive ) + strlen( szDir ) + strlen( _szNewFileTmp ) + strlen( _szNewExtTmp ) > _cbPathMax )
        THROW_EXCEPTION( exInternal );

    _makepath ( szOldTmp, szDrive, szDir, _szOldFileTmp, _szOldExtTmp );
    _makepath ( szNewTmp, szDrive, szDir, _szNewFileTmp, _szNewExtTmp );

    f = vcs_file_exists( pszFile );
    
    if ( f )
        vcs_file_rename( pszFile, szOldTmp );
    
    vcs_file_rename( szNewTmp, pszFile  );

    if ( f )
        vcs_file_remove( szOldTmp );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_is_dir( )
//
// Returns TRUE if the given path is a directory
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_is_dir( char *pszPath )
    {
    unsigned flags = 0u;

    if ( _dos_getfileattr( pszPath, &flags ) != 0 )
        return FALSE;

    return ( flags & _A_SUBDIR );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_read_line( )
//
// This routine reads one line from a file.
//
// Returns:
//     TRUE  - read successful
//     FALSE - end-of-file
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_read_line( FILE *pfile, char *szLine, int cbLine )
    {
    BOOL fRet = FALSE;
    char *pch;

    pch = fgets( szLine, cbLine, pfile );
    if ( !pch )
        {
        if ( ferror( pfile ) )
            THROW_EXCEPTION( exFile );
        else
            return FALSE;
        }

    pch = strchr( szLine, '\n' );
    if ( !pch )
        THROW_EXCEPTION( exCorrupt );
    
    *pch = '\0';
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_read_label_header( )
//
// Read Label Header
//////////////////////////////////////////////////////////////////////////////
BOOL vcs_read_label_header( FILE *pfile, LabelHeader *plh )
    {
    if ( fread( plh, sizeof( *plh ), 1, pfile ) < 1 )
        return FALSE;
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_write_label_header( )
//
// Write Label Header
//////////////////////////////////////////////////////////////////////////////
void vcs_write_label_header( FILE *pfile, LabelHeader *plh )
    {
    if ( fwrite( plh, sizeof( *plh ), 1, pfile ) < 1 )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_read_label_rec( )
//
// Read File Label Rec
//////////////////////////////////////////////////////////////////////////////
void vcs_read_label_rec( FILE *pfile, LabelRec *prec )
    {
    if ( fread( prec, sizeof( *prec ), 1, pfile ) < 1 )
        THROW_EXCEPTION( exCorrupt );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_write_label_rec( )
//
// Write File Label Rec
//////////////////////////////////////////////////////////////////////////////
void vcs_write_label_rec( FILE *pfile, LabelRec *prec )
    {
    if ( fwrite( prec, sizeof( *prec ), 1, pfile ) < 1 )
        THROW_EXCEPTION( exFile );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_write_empty_label_rec( )
//
// Write empty label record
//////////////////////////////////////////////////////////////////////////////
void vcs_write_empty_label_rec( FILE *pfile )
    {
    LabelRec rec;

    memset( &rec, 0, sizeof( rec ) );
    rec.type = eEmptyLabelRec;
    vcs_write_label_rec( pfile, &rec );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_skip_label( )
//
// Skip a label
//////////////////////////////////////////////////////////////////////////////
void vcs_skip_label( FILE *pfileRead )
    {
    BOOL fDone = FALSE;
    LabelRec rec;

    while ( TRUE )
        {
        vcs_read_label_rec( pfileRead, &rec );
        if ( rec.type == eEmptyLabelRec )
            break;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_copy_label( )
//
// Copies a label body (not the header)
//////////////////////////////////////////////////////////////////////////////
void vcs_copy_label( FILE *pfileWrite, FILE *pfileRead )
    {
    BOOL fDone = FALSE;
    LabelRec rec;
    
    while ( TRUE )
        {
        vcs_read_label_rec( pfileRead, &rec );
        if ( rec.type == eEmptyLabelRec )
            break;
        vcs_write_label_rec( pfileWrite, &rec );
        }
    vcs_write_empty_label_rec( pfileWrite );
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_dos_getftime( )
//
// Get DOS date/time
//////////////////////////////////////////////////////////////////////////////
void vcs_dos_getftime( char *pszFile, unsigned *p_dos_date, unsigned *p_dos_time )
    {
    int h = 0;

    TRY
        {
        if ( _dos_open( pszFile, O_RDONLY, &h ) != 0 )
            THROW_EXCEPTION( exFile );

        if ( _dos_getftime( h, p_dos_date, p_dos_time ) != 0 )
            THROW_EXCEPTION( exFile );

        _dos_close( h );
        }
    CATCH_ALL
        {
        if ( h )
            _dos_close( h );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// vcs_dos_setftime( )
//
// Set DOS date/time
//////////////////////////////////////////////////////////////////////////////
void vcs_dos_setftime( char *pszFile, unsigned _dos_date, unsigned _dos_time )
    {
    int h = 0;

    TRY
        {
        if ( _dos_open( pszFile, O_RDONLY, &h ) != 0 )
            THROW_EXCEPTION( exFile );

        if ( _dos_setftime( h, _dos_date, _dos_time ) != 0 )
            THROW_EXCEPTION( exFile );

        _dos_close( h );
        }
    CATCH_ALL
        {
        if ( h )
            _dos_close( h );
        THROW_LAST( );
        }
    END_CATCH
    }

