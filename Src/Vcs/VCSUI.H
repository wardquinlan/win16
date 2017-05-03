//////////////////////////////////////////////////////////////////////////////
// VCSUI.H
//
// Version Control System - User Interface Includes
//////////////////////////////////////////////////////////////////////////////

#ifndef _VCSUI_H
#define _VCSUI_H

//////////////////////////////////////////////////////////////////////////////
// Type Definitions

// Command Table definitions
typedef void ( *PFNCOMMAND )( int argc, char *argv[ ] );
typedef struct tagFuncTable
    {
    char       *pszCommand;
    PFNCOMMAND  pfnCommand;
    } FuncTable;

// Help Table
typedef struct tagHelpTable
    {
    char       *pszCommand;
    char       *pszShortForm;
    char       *pszOverview;
    char       *pszSynopsis;
    } HelpTable;

//////////////////////////////////////////////////////////////////////////////
// Function prototypes

// Mainline
int main( int argc, char *argv[ ] );

// Command lookup routine
PFNCOMMAND vcsui_lookup( const char *pszCommand );

// Log routine
void vcsui_log( const char *pszMsg );

// Callback routine for typing the log file
void vcsui_logmsg_callback( char *szMsg );

// Helper function to get _szVcsBranch
char *getVcsBranch( void );

// Commands
void vcsui_init_system      ( int argc, char *argv[ ] );
void vcsui_create_dir       ( int argc, char *argv[ ] );
void vcsui_remove_dir       ( int argc, char *argv[ ] );
void vcsui_create_file      ( int argc, char *argv[ ] );
void vcsui_remove_file      ( int argc, char *argv[ ] );
void vcsui_dir              ( int argc, char *argv[ ] );
void vcsui_type_log         ( int argc, char *argv[ ] );
void vcsui_create_ver       ( int argc, char *argv[ ] );
void vcsui_lock_ver         ( int argc, char *argv[ ] );
void vcsui_list_ver         ( int argc, char *argv[ ] );
void vcsui_remove_ver       ( int argc, char *argv[ ] );
void vcsui_create_label     ( int argc, char *argv[ ] );
void vcsui_remove_label     ( int argc, char *argv[ ] );
void vcsui_label            ( int argc, char *argv[ ] );
void vcsui_list_labels      ( int argc, char *argv[ ] );
void vcsui_purge            ( int argc, char *argv[ ] );
void vcsui_help             ( int argc, char *argv[ ] );
void vcsui_getfile          ( int argc, char *argv[ ] );
void vcsui_getlabel         ( int argc, char *argv[ ] );
void vcsui_hidefile         ( int argc, char *argv[ ] );
void vcsui_hidelabel        ( int argc, char *argv[ ] );

#endif
