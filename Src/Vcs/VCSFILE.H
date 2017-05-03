//////////////////////////////////////////////////////////////////////////////
// VCSFILE.H
//
// Version Control System - File Management
//////////////////////////////////////////////////////////////////////////////

#ifndef _VCSFILE_H
#define _VCSFILE_H

#include <stdio.h>
#include "vcs.h"

//////////////////////////////////////////////////////////////////////////////
// Types

enum tagFileType
    {
    eDataFile,
    eLabelFile,
    };
typedef enum tagFileType FileType;

// File Header
struct tagFileHeader
    {
    FileType ft;
    int      nVCSVersion;
    };
typedef struct tagFileHeader FileHeader;

//////////////////////////////////////////////////////////////////////////////
// Globals

// Tempory file name
extern char _szNewFileTmp[ ];
extern char _szNewExtTmp [ ];
extern char _szOldFileTmp[ ];
extern char _szOldExtTmp [ ];

#define _cbBlock 0x800

//////////////////////////////////////////////////////////////////////////////
// Function prototypes

// Returns a name for a temporary file
void vcs_tempfile( char *pszTmpFile, char *pszFile );

// Returns TRUE if the given file exists
BOOL vcs_file_exists( char *pszPath );

// Returns TRUE if the given pathname is reserved
BOOL vcs_is_reserved( char *pszPath );

// Reads the file header
void vcs_read_file_header( FILE *pfile, FileHeader *ph );

// Writes the file header
void vcs_write_file_header( FILE *pfile, FileHeader *ph );

// Reads the file version header
BOOL vcs_read_version_header( FILE *pfile, FileVersionHeader *ph );

// Writes the file version header
void vcs_write_version_header( FILE *pfile, FileVersionHeader *ph );

// Copies a file version
void vcs_copy_version( FILE *pfileWrite, FILE *pfileRead, long cb );

// Commits a file
void vcs_commit( char *pszFile );

// Removes a file
void vcs_file_remove( char *pszPath );

// Renames a file
void vcs_file_rename( char *pszOldName, char *pszNewName );

// Removes a file, but will not throw an exception
void vcs_abort( char *pszFile );

// Reads whitespace from file
void vcs_read_space( FILE *pfileRead, long cb );

// Read one line from a file                                                            
BOOL vcs_read_line( FILE *pfile, char *szLine, int cbLine );

// Read Label Header
BOOL vcs_read_label_header( FILE *pfile, LabelHeader *plh );

// Write Label Header
void vcs_write_label_header( FILE *pfile, LabelHeader *plh );

// Read File Label Rec
void vcs_read_label_rec( FILE *pfile, LabelRec *prec );

// Write File Label Rec
void vcs_write_label_rec( FILE *pfile, LabelRec *prec );

// Write empty label record
void vcs_write_empty_label_rec( FILE *pfile );

// Copy a label
void vcs_copy_label( FILE *pfileWrite, FILE *pfileRead );

// Skip a label
void vcs_skip_label( FILE *pfileRead );

// Returns TRUE if pszpath is a directory
BOOL vcs_is_dir( char *pszPath );

// Retrieve/Set DOS date/TIME
void vcs_dos_getftime( char *pszFile, unsigned *p_dos_date, unsigned *p_dos_time );
void vcs_dos_setftime( char *pszFile, unsigned _dos_date, unsigned _dos_time );

// ReadOnly functions
BOOL vcs_get_readonly_bit( char *pszPath );
void vcs_set_readonly_bit( char *pszPath, BOOL fSet );

#endif
