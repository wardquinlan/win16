//////////////////////////////////////////////////////////////////////////////
// VCSCORE.H
//
// Version Control System - Core Includes
//////////////////////////////////////////////////////////////////////////////

#ifndef _VCSCORE_H
#define _VCSCORE_H

//////////////////////////////////////////////////////////////////////////////
// Types
enum tagState
    {
    stStart,
    stBefore,
    stDuring,
    stAfter,
    stStop
    };
typedef enum tagState State;

//////////////////////////////////////////////////////////////////////////////
// Global Variables

// Pointer to old Ctrl-C handler
extern void ( _interrupt _far *_pfCtrlCSav )( );

// Initialized Flag
extern BOOL _fInitialized;

// Ctrl-C Flag
extern BOOL _fCtrlC;

//////////////////////////////////////////////////////////////////////////////
// Function Prototypes

// Ctrl-C Handler
void _interrupt _far fCtrlC( );

// Resolves a VCS path to a DOS path
void vcs_resolve_path( char *pszDosPath, char *pszVcsPath );

// Reserved: resolves a VCS path to a DOS path
void vcs_resolve_path_reserved( char *pszDosPath, char *pszVcsPath );

// Throws an exception if Ctrl-C hit
void vcs_check_ctrl_c( void );

// Throws an exception if not initialized
void vcs_check_initialized( void );

// Removes trailing slash on path, if present
void vcs_remove_trailing_slash( char *pszPath );

// Comparison routines for qsort
int vcs_direct_cmp( const void *a, const void *b );
int vcs_label_cmp ( const void *a, const void *b );

// Convert time formats
void vcs_dostime_to_time_t( time_t *ptime_t, unsigned *pdate, unsigned *ptime );

// Creates a Label File
void vcs_create_label_file( char *pszPath );

// Read Label Header; check to see if label already exists
BOOL vcs_read_label_header_check( FILE *pfile, LabelHeader *plh, LabelHeader *plhCheck );

// Label Command: Add
void vcs_labelcmd_add( char *pszPath, const char *pszBranch, const char *pszLabel, LabelCmd *pcmd );

// Label Command: Remove
void vcs_labelcmd_remove( char *pszPath, const char *pszBranch, const char *pszLabel, LabelCmd *pcmd );

// Count number of items in directory
unsigned vcs_directory_count_i( char *pszPath, unsigned attrib );

// Directory Listing
void vcs_directory_i( char *pszPath, unsigned attrib, Direct *pd, unsigned nMax );

// File Versions count
unsigned vcs_versions_count_i( char *pszPath );

// File Versions
void vcs_versions_i( char *pszPath, FileVersionHeader *ph, unsigned nMax );

// Get a File
void vcs_getfile_i( char *pszPath, const char *pszBranch, unsigned nVersion, BOOL fReadOnly );
void vcs_getfile_i2( char *pszPath, char *pszPathOffset, char *pszFile, const char *pszBranch, unsigned nVersion, BOOL fReadOnly );

// Get nodes in a Label
void vcs_getlabel_i( char *pszPath, char *pszPathOffset, const char *pszBranch, const char *pszLabel );

// Count number of labels
unsigned vcs_list_all_labels_count_i( char *pszPath );

// Label listings
void vcs_list_all_labels_i( char *pszPath, LabelHeader *ph, unsigned nMax );

// Makes a file name
void vcs_make_filename( char *pszFileName, char *pszName, char *pszExt );

// Given a Direct *, returns the index of an entry
BOOL vcs_find_direct_entry( unsigned *pidx, Direct *pd, unsigned nMax, char *szName );

// Gets the latest version of a file.  If there are no
// versions for the given branch, returns 0u.
unsigned vcs_get_latest_version( char *pszPath, const char *pszBranch );

// Returns TRUE if the given Branch/Label is referenced within
// pszPath's Label file
BOOL vcs_label_is_referenced( char *pszPathParent, char *pszPathChild, const char *pszBranch, const char *pszLabel );

// Returns TRUE if the given version is referenced with the label file
BOOL vcs_version_is_referenced( char *pszPath, char *pszFile, const char *pszBranch, unsigned nVersion );

// Customized Split Path
void vcs_splitpath( char *pszPathParent, char *pszPathChild, char *pszPath );

// Customized Make Path
void vcs_makepath( char *pszPath, char *pszPathParent, char *pszPathChild );

// Valid Symbol check (throws exSyntax if not valid )
void vcs_valid_symbol( const char *pszSymbol );

// Checks to see if the given version is both valid and locked
void vcs_labelable_version( char *pszPath, const char *pszBranch, unsigned nVersion );

// Checks to see if a sub-directory contains the given label
void vcs_labelable_label( char *pszPath, const char *pszBranch, const char *pszLabel );

// Hack to fix up targets
void vcs_fixup_target( char *pszTarget );

// Hides Files
void vcs_hidefile_i( char *pszPath );
void vcs_hidefile_i2( char *pszPath, char *pszPathOffset, char *pszFile );

// Hides Labels
void vcs_hidelabel_i( char *pszPath, char *pszPathOffset, const char *pszBranch, const char *pszLabel );

// Resolves version numbers: if the version is 0, gets the latest version
unsigned vcs_resolve_version( char *pszPath, const char *pszBranch, unsigned nversion );

// Count List particular label
unsigned vcs_list_label_count_i( char *pszPath, const char *pszBranch, const char *pszLabel );

// List particular label
void vcs_list_label_i( char *pszPath, const char *pszBranch, const char *pszLabel, LabelRec *rgrec, unsigned nMax );

#endif
