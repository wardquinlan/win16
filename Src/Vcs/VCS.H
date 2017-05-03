//////////////////////////////////////////////////////////////////////////////
// VCS.H
//
// This computer program is copyright (c) Ward Quinlan, 1998
//
// Version Control System Public Includes
//////////////////////////////////////////////////////////////////////////////
#ifndef _VCS_H
#define _VCS_H

#include <string.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "vcsexcpt.h"

//////////////////////////////////////////////////////////////////////////////
// Constants

// Maximum path length
#define _cbPathMax 128

// Maximum file name length
#define _cbFileMax 12

// Maximum header length
#define _cbHeaderMax 128

// Maximum symbol length
#define _cbSymbolMax 16

// Version of VCS
#define _nVCSVersion 1

//////////////////////////////////////////////////////////////////////////////
// Global Variables

// Environment Strings
extern char _szVcsRoot  [ ];
extern char _szVcsBranch[ ];
extern char _szVcsRPath [ ];

// fopen flags
extern char _szBinaryRead  [ ];
extern char _szBinaryWrite [ ];
extern char _szBinaryAppend[ ];
extern char _szTextRead    [ ];
extern char _szTextWrite   [ ];
extern char _szTextAppend  [ ];

// other strings
extern char _szLabels[ ];
extern char _szStar  [ ];
extern char _szSlash [ ];
extern char _szDot   [ ];
extern char _szDot2  [ ];
extern char _szEmpty [ ];
extern char _szCR    [ ];

//////////////////////////////////////////////////////////////////////////////
// Typedefs

#ifndef WINDOWS
typedef int BOOL;
#define FALSE 0
#define TRUE  1
#endif

// Directory Information
struct tagDirect
    {
    char   bAttrib;                  // Attribute byte
    time_t t;                        // File Timestamp
    long   nSize;                    // File Size in bytes
    char   szFile[ _cbFileMax + 1 ]; // File Name
    char   szExt [ _cbFileMax + 1 ]; // Extension Name
    };
typedef struct tagDirect Direct;

// File Version Header
struct tagFileVersionHeader
    {
    char          szBranch[ _cbSymbolMax + 1 ];
    unsigned      nVersion;
    BOOL          fLock;
    long          cb;
    time_t        t;

    // Reserved for internal use
    unsigned      _dos_date;
    unsigned      _dos_time;
    };
typedef struct tagFileVersionHeader FileVersionHeader;

// Label header
struct tagLabelHeader
    {
    char   szBranch[ _cbSymbolMax + 1 ];
    char   szLabel [ _cbSymbolMax + 1 ];
    time_t tCreation;
    };
typedef struct tagLabelHeader LabelHeader;

enum tagLabelRecType
    {
    eFileLabelRec,
    eDirLabelRec,
    eEmptyLabelRec
    };
typedef enum tagLabelRecType LabelRecType;

// Label Records
struct tagLabelRec
    {
    LabelRecType type;
    union
        {
        char szFile[ _cbFileMax + 1 ];
        char szDir [ _cbFileMax + 1 ];
        };
    union
        {
        unsigned nVersion;
        char     szLabel[ _cbSymbolMax + 1 ];
        };
    };
typedef struct tagLabelRec LabelRec;

// Label Command
struct tagLabelCmd
    {
    BOOL fAdd;
    char szTarget [ _cbFileMax   + 1 ];
    char szVersion[ _cbSymbolMax + 1 ];
    };
typedef struct tagLabelCmd LabelCmd;

// Log Callback
typedef void (*PFNLOG)( const char *pszMsg );

//////////////////////////////////////////////////////////////////////////////
// VCS-Specific Exceptions

#define exEnvironment    0x01
#define exSyntax         0x02
#define exCtrlC          0x03
#define exFile           0x04
#define exMemory         0x05
#define exInternal       0x06
#define exCorrupt        0x07
#define exIllegal        0x08

//////////////////////////////////////////////////////////////////////////////
// Public Function prototypes

// Initialize Instance
void vcs_init_instance( void );

// Destruction of Instance
void vcs_destroy_instance( void );

// Initialize VCS System
void vcs_init_system( void );

// Catch Ctrl-C
void vcs_block_ctrl_c( void );

// Release Ctrl-C
void vcs_restore_ctrl_c( void );

// Create Dir
void vcs_create_dir( char *pszPath );

// Remove Dir
void vcs_remove_dir( char *pszPath );

// Create File
void vcs_create_file( char *pszPath );

// Remove File
void vcs_remove_file( char *pszPath );

// Count number of labels
unsigned vcs_list_all_labels_count( char *pszPath, char *pszHeader );

// Label listings
void vcs_list_all_labels( char *pszPath, LabelHeader *ph, unsigned nMax );

// Count number of items in directory
unsigned vcs_directory_count( char *pszPath, unsigned attrib, char *pszHeader );

// Directory Listing
void vcs_directory( char *pszPath, unsigned attrib, Direct *pd, unsigned nMax );

// Versions count
unsigned vcs_versions_count( char *pszPath, char *pszHeader );

// Versions
void vcs_versions( char *pszPath, FileVersionHeader *ph, unsigned nMax );

// log registration
void vcs_register_log( PFNLOG pfn );

// standard VCS log file
void vcs_standard_log( const char *pszMsg );

// log interface
void vcs_log( const char *pszFormat, ... );

// type log interface
void vcs_type_log( void (*pfn)( char *szLine ) );

// create a Version
void vcs_create_version( char *pszPath, const char *pszBranch );

// lock a Version
void vcs_lock_version( char *pszPath, const char *pszBranch );

// Remove a version
void vcs_remove_version( char *pszPath, const char *pszBranch, unsigned nVersion );

// Get a File
void vcs_getfile( char *pszTarget, const char *pszBranch, unsigned nVersion );

// Get nodes in a Label
void vcs_getlabel( char *pszPath, const char *pszBranch, const char *pszLabel );

// Creates a Label
void vcs_create_label( char *pszPath, const char *pszBranch, const char *pszLabel );

// Remove a Label
void vcs_remove_label( char *pszPath, const char *pszBranch, const char *pszLabel );

// General Label manipulation
void vcs_label( char *pszPath, const char *pszBranch, const char *pszLabel, LabelCmd *pcmd );

// Count List particular label
unsigned vcs_list_label_count( char *pszPath, const char *pszBranch, const char *pszLabel, char *pszHeader );

// List particular label
void vcs_list_label( char *pszPath, const char *pszBranch, const char *pszLabel, LabelRec *rgrec, unsigned nMax );

// Wrapper for ctime( ), but strips \n character
char *vcs_ctime( const time_t *timer );

// Purge the log file
void vcs_purge_log( void );

// Hide Files
void vcs_hidefile( char *pszTarget );

// Hide Labels
void vcs_hidelabel( char *pszPath, const char *pszBranch, const char *pszLabel );

#endif
