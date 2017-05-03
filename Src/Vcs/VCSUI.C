//////////////////////////////////////////////////////////////////////////////
// VCSUI.C
//
// Version Control System - User Interface Module
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <dos.h>
#include <time.h>
#include "vcs.h"
#include "vcsui.h"

static FuncTable rgFn[ ] =
    {
        { "init",        vcsui_init_system       },
        { "mkdir",       vcsui_create_dir        },
        { "md",          vcsui_create_dir        },
        { "rmdir",       vcsui_remove_dir        },
        { "rd",          vcsui_remove_dir        },
        { "mkfile",      vcsui_create_file       },
        { "mf",          vcsui_create_file       },
        { "rmfile",      vcsui_remove_file       },
        { "rf",          vcsui_remove_file       },
        { "log",         vcsui_type_log          },
        { "dir",         vcsui_dir               },
        { "mkver",       vcsui_create_ver        },
        { "mv",          vcsui_create_ver        },
        { "lkver",       vcsui_lock_ver          },
        { "lk",          vcsui_lock_ver          },
        { "lsver",       vcsui_list_ver          },
        { "lv",          vcsui_list_ver          },
        { "rmver",       vcsui_remove_ver        },
        { "rv",          vcsui_remove_ver        },
        { "mklab",       vcsui_create_label      },
        { "ml",          vcsui_create_label      },
        { "rmlab",       vcsui_remove_label      },
        { "rl",          vcsui_remove_label      },
        { "label",       vcsui_label             },
        { "lb",          vcsui_label             },
        { "lslab",       vcsui_list_labels       },
        { "ll",          vcsui_list_labels       },
        { "purge",       vcsui_purge             },
        { "help",        vcsui_help              },
        { "h",           vcsui_help              },
        { "getfile",     vcsui_getfile           },
        { "gf",          vcsui_getfile           },
        { "getlab",      vcsui_getlabel          },
        { "gl",          vcsui_getlabel          },
        { "hdfile",      vcsui_hidefile          },
        { "hf",          vcsui_hidefile          },
        { "hdlab",       vcsui_hidelabel         },
        { "hl",          vcsui_hidelabel         },
    };

// Help Table
static HelpTable rgHelp[ ] =
    {
        { 
        "init",
        "",
        "Initializes VCS System", 
        ""  
        },
        
        { 
        "mkdir",
        "md",
        "Creates a Directory",    
        "<dir-path>"  
        },
        
        {
        "rmdir",
        "rd",
        "Removes a Directory",
        "<dir-path>"
        },

        {
        "mkfile",
        "mf",
        "Creates a File",
        "<file-path>"
        },

        {
        "rmfile",
        "rf",
        "Removes a File",
        "<file-path>"
        },

        {
        "log",
        "",
        "Displays the VCS System Log",
        ""
        },

        {
        "dir",
        "",
        "Displays Directory Listings",
        "[<dos-pattern>]"
        },

        {
        "mkver",
        "mv",
        "Creates a Version",
        "<file-path> [<branch>]"
        },

        {
        "lkver",
        "lk",
        "Locks a Version",
        "<file-path> [<branch>]"
        },

        {
        "lsver",
        "lv",
        "Lists Versions",
        "<file-path>"
        },

        {
        "rmver",
        "rv",
        "Removes a Version",
        "<file-path> [<branch>] <version-number>"
        },

        {
        "mklab",
        "ml",
        "Creates a Label",
        "<dir-path> [<branch>] <label>"
        },

        {
        "rmlab",
        "rl",
        "Removes a Label",
        "<dir-path> [<branch>] <label>"
        },

        {
        "label",
        "lb",
        "Modifies Labels",
        "<path> [<branch>] <label> <{+|-}dos-pattern> [version]"
        },

        {
        "lslab",
        "ll",
        "Lists Labels",
        "<path> [ [<branch>] <label> ]"
        },

        {
        "getfile",
        "gf",
        "Gets Files",
        "<dos-pattern> [<branch>] <version>"
        },

        {
        "getlab",
        "gl",
        "Gets Nodes in a Label",
        "<path> [<branch>] <label>"
        },

        {
        "hdfile",
        "hl",
        "Hides Files",
        "<dos-pattern>"
        },

        {
        "hdlab",
        "hl",
        "Hides Files in a Label",
        "<path> [<branch>] <label>"
        },

        {
        "purge",
        "",
        "Purges the VCS System Log",
        ""
        },

        {
        "help",
        "h",
        "Displays Help Screens",
        "[<command>]"
        }
    };

static char _szEmptyList[ ] = "<empty list>";

//////////////////////////////////////////////////////////////////////////////
// main( )
//
// mainline
//////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[ ] )
    {
    char      *pchCommand;
    PFNCOMMAND pfn;
    int ret = 1;
    
    TRY
        {
        vcs_block_ctrl_c( );
        
        // Because we may not have registered with the log
        // system, we have to manually print out error
        // messages if something goes wrong
        TRY
            {
            vcs_init_instance( );
            }
        CATCH_ALL
            {
            printf( "Exception: Can't initialize\n" );
            THROW_LAST( );
            }
        END_CATCH
        
        vcs_register_log( vcs_standard_log );
        vcs_register_log( vcsui_log );
        
        if ( argc < 2 )
            THROW_EXCEPTION( exSyntax );

        pfn = vcsui_lookup( argv[ 1 ] );
        if ( !pfn )
            THROW_EXCEPTION( exSyntax );

        pfn( argc - 2, &argv[ 2 ] );
        ret = 0;
        }
    CATCH( exEnvironment )
        {
        vcs_log( "Exception: Environment not set up.  Type 'VCS HELP'" );
        }
    CATCH( exSyntax )
        {
        vcs_log( "Exception: Syntax error.  Type 'VCS HELP'" );
        }
    CATCH( exIllegal )
        {
        vcs_log( "Exception: Illegal operation attempted" );
        }
    CATCH( exCtrlC )
        {
        vcs_log( "Exception: Break detected" );
        }
    CATCH( exFile )
        {
        vcs_log( "Exception: File I/O" );
        }
    CATCH( exMemory )
        {
        vcs_log( "Exception: Out of Memory" );
        }
    CATCH( exInternal )
        {
        vcs_log( "Exception: Internal Error" );
        }
    CATCH( exCorrupt )
        {
        vcs_log( "Exception: Corruption detected" );
        }
    CATCH_ALL
        {
        vcs_log( "Exception: PANIC: Unexpected Exception" );
        }
    END_CATCH

    vcs_destroy_instance( );
    vcs_restore_ctrl_c( );
    return ret;
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_help( )
//
// COMMAND: help system
//////////////////////////////////////////////////////////////////////////////
void vcsui_help( int argc, char *argv[ ] )
    {
    int i;

    printf( "VCS Version 1.01\n\n" );
    if ( argc == 0 )
        {
        printf( "Environment Overview\n\n" );
        printf( "VCSROOT   (mandatory) - Root of VCS System\n"  );
        printf( "VCSBRANCH (optional)  - Default VCS Branch\n"  );
        printf( "VCSRPATH  (optional)  - Relative VCS Path\n\n" );

        printf( "Command Overview\n\n" );
        for ( i = 0; i < sizeof( rgHelp ) / sizeof( rgHelp[ 0 ] ); i++ )
            printf( "%-8s %-2s   %s\n", rgHelp[ i ].pszCommand, 
                                        rgHelp[ i ].pszShortForm,
                                        rgHelp[ i ].pszOverview );
        
        printf( "\nType 'VCS HELP <COMMAND>' for detailed help\n" );
        }
    else if ( argc == 1 )
        {
        for ( i = 0; i < sizeof( rgHelp ) / sizeof( rgHelp[ 0 ] ); i++ )
            {
            if ( stricmp( argv[ 0 ], rgHelp[ i ].pszCommand ) == 0 )
                break;
            }
        
        if ( i == sizeof( rgHelp ) / sizeof( rgHelp[ 0 ] ) )
            THROW_EXCEPTION( exSyntax );
        
        printf( "%-8s    %s\n\n", rgHelp[ i ].pszCommand, 
                                  rgHelp[ i ].pszOverview );
        printf( "Synopsis:   VCS %s %s\n", rgHelp[ i ].pszCommand,
                                           rgHelp[ i ].pszSynopsis );
        }
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_log( )
//
// Log routine
//////////////////////////////////////////////////////////////////////////////
void vcsui_log( const char *pszMsg )
    {
    printf( "%s\n", pszMsg );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_lookup( )
//
// Looks up a command in the command table
//////////////////////////////////////////////////////////////////////////////
PFNCOMMAND vcsui_lookup( const char *pszCommand )
    {
    int i;

    for ( i = 0; i < sizeof( rgFn ) / sizeof( rgFn[ 0 ] ); i++ )
        {
        if ( stricmp( pszCommand, rgFn[ i ].pszCommand ) == 0 )
            return rgFn[ i ].pfnCommand;
        }
    return NULL;
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_purge( )
//
// COMMAND: Purges the log file
//////////////////////////////////////////////////////////////////////////////
void vcsui_purge( int argc, char *argv[ ] )
    {
    if ( argc != 0 )
        THROW_EXCEPTION( exSyntax );
    
    vcs_purge_log( );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_init_system( )
//
// COMMAND: respond to "init"
//////////////////////////////////////////////////////////////////////////////
void vcsui_init_system( int argc, char *argv[ ] )
    {
    if ( argc != 0 )
        THROW_EXCEPTION( exSyntax );
    vcs_init_system( );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_create_dir( )
//
// COMMAND: respond to "mkdir"
//////////////////////////////////////////////////////////////////////////////
void vcsui_create_dir( int argc, char *argv[ ] )
    {
    if ( argc != 1 )
        THROW_EXCEPTION( exSyntax );

    vcs_create_dir( argv[ 0 ] );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_remove_dir( )
//
// COMMAND: respond to "rmdir"
//////////////////////////////////////////////////////////////////////////////
void vcsui_remove_dir( int argc, char *argv[ ] )
    {
    if ( argc != 1 )
        THROW_EXCEPTION( exSyntax );

    vcs_remove_dir( argv[ 0 ] );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_create_file( )
//
// COMMAND: respond to "mkfile"
//////////////////////////////////////////////////////////////////////////////
void vcsui_create_file( int argc, char *argv[ ] )
    {
    if ( argc != 1 )
        THROW_EXCEPTION( exSyntax );

    vcs_create_file( argv[ 0 ] );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_remove_file( )
//
// COMMAND: respond to "rmfile"
//////////////////////////////////////////////////////////////////////////////
void vcsui_remove_file( int argc, char *argv[ ] )
    {
    if ( argc != 1 )
        THROW_EXCEPTION( exSyntax );

    vcs_remove_file( argv[ 0 ] );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_create_label( )
//
// COMMAND: Creates a Label
//////////////////////////////////////////////////////////////////////////////
void vcsui_create_label( int argc, char *argv[ ] )
    {
    if ( argc == 2 )
        vcs_create_label( argv[ 0 ], getVcsBranch( ), argv[ 1 ] );
    else if ( argc == 3 )
        vcs_create_label( argv[ 0 ], argv[ 1 ], argv[ 2 ] );
    else
        THROW_EXCEPTION( exSyntax );
    }
    
//////////////////////////////////////////////////////////////////////////////
// vcsui_remove_label( )
//
// COMMAND: Removes a label
//////////////////////////////////////////////////////////////////////////////
void vcsui_remove_label( int argc, char *argv[ ] )
    {
    if ( argc == 2 )
        vcs_remove_label( argv[ 0 ], getVcsBranch( ), argv[ 1 ] );
    else if ( argc == 3 )
        vcs_remove_label( argv[ 0 ], argv[ 1 ], argv[ 2 ] );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_label( )
//
// COMMAND: Label Modification
//////////////////////////////////////////////////////////////////////////////
void vcsui_label( int argc, char *argv[ ] )
    {
    char *pszPath    = NULL;
    char *pszBranch  = NULL;
    char *pszLabel   = NULL;
    char *pszTarget  = NULL;
    char *pszVersion = NULL;
    BOOL fAdd        = FALSE;
    LabelCmd cmd;

    memset( &cmd, 0, sizeof( cmd ) );

    if ( argc < 3 )
        THROW_EXCEPTION( exSyntax );

    pszPath = argv[ 0 ];
    if ( argv[ 2 ][ 0 ] == '+' || argv[ 2 ][ 0 ] == '-' )
        {
        pszBranch = getVcsBranch( );
        pszLabel  = argv[ 1 ];
        if ( argv[ 2 ][ 0 ] == '+' )
            fAdd = TRUE;
        else if ( argv[ 2 ][ 0 ] == '-' )
            fAdd = FALSE;
        else
            THROW_EXCEPTION( exSyntax );

        pszTarget = &argv[ 2 ][ 1 ];
        if ( argc == 3 )
            pszVersion = _szEmpty;
        else if ( argc == 4 )
            pszVersion = argv[ 3 ];
        else
            THROW_EXCEPTION( exSyntax );
        }
    else
        {
        pszBranch = argv[ 1 ];
        pszLabel  = argv[ 2 ];
        if ( argv[ 3 ][ 0 ] == '+' )
            fAdd = TRUE;
        else if ( argv[ 3 ][ 0 ] == '-' )
            fAdd = FALSE;
        else
            THROW_EXCEPTION( exSyntax );
        pszTarget = &argv[ 3 ][ 1 ];
        if ( argc == 4 )
            pszVersion = _szEmpty;
        else if ( argc == 5 )
            pszVersion = argv[ 4 ];
        else
            THROW_EXCEPTION( exSyntax );
        }
    
    cmd.fAdd = fAdd;
    if ( strlen( pszVersion ) >= sizeof( cmd.szVersion ) )
        THROW_EXCEPTION( exSyntax );
    strcpy( cmd.szVersion, pszVersion );
    
    if ( strlen( pszTarget ) >= sizeof( cmd.szTarget ) )
        THROW_EXCEPTION( exSyntax );
    strcpy( cmd.szTarget, pszTarget );
    
    vcs_label( pszPath, pszBranch, pszLabel, &cmd );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_create_ver( )
//
// COMMAND: respond to "mkver"
//////////////////////////////////////////////////////////////////////////////
void vcsui_create_ver( int argc, char *argv[ ] )
    {
    if ( argc == 1 )
        vcs_create_version( argv[ 0 ], getVcsBranch( ) );
    else if ( argc == 2 )
        vcs_create_version( argv[ 0 ], argv[ 1 ] );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_lock_ver( )
//
// COMMAND: respond to "lkver"
//////////////////////////////////////////////////////////////////////////////
void vcsui_lock_ver( int argc, char *argv[ ] )
    {
    if ( argc == 1 )
        vcs_lock_version( argv[ 0 ], getVcsBranch( ) );
    else if ( argc == 2 )
        vcs_lock_version( argv[ 0 ], argv[ 1 ] );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_remove_ver( )
//
// COMMAND: respond to "rmver"
//////////////////////////////////////////////////////////////////////////////
void vcsui_remove_ver( int argc, char *argv[ ] )
    {
    if ( argc == 2 )
        vcs_remove_version( argv[ 0 ], getVcsBranch( ), (unsigned) atoi( argv[ 1 ] ) );
    else if ( argc == 3 )
        vcs_remove_version( argv[ 0 ], argv[ 1 ], (unsigned) atoi( argv[ 2 ] ) );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_getlabel( )
//
// COMMAND: respond to "getlabel"
//////////////////////////////////////////////////////////////////////////////
void vcsui_getlabel( int argc, char *argv[ ] )
    {
    if ( argc == 2 )
        vcs_getlabel( argv[ 0 ], getVcsBranch( ), argv[ 1 ] );
    else if ( argc == 3 )
        vcs_getlabel( argv[ 0 ], argv[ 1 ], argv[ 2 ] );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_getfile( )
//
// COMMAND: respond to "getfile"
//////////////////////////////////////////////////////////////////////////////
void vcsui_getfile( int argc, char *argv[ ] )
    {
    if ( argc == 2 )
        vcs_getfile( argv[ 0 ], getVcsBranch( ), (unsigned) atoi( argv[ 1 ] ) );
    else if ( argc == 3 )
        vcs_getfile( argv[ 0 ], argv[ 1 ], (unsigned) atoi( argv[ 2 ] ) );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_hidefile( )
//
// COMMAND: respond to 'hdfile'
//////////////////////////////////////////////////////////////////////////////
void vcsui_hidefile( int argc, char *argv[ ] )
    {
    if ( argc == 1 )
        vcs_hidefile( argv[ 0 ] );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_hidelabel( )
//
// COMMAND: respond to 'hdlabel'
//////////////////////////////////////////////////////////////////////////////
void vcsui_hidelabel( int argc, char *argv[ ] )
    {
    if ( argc == 2 )
        vcs_hidelabel( argv[ 0 ], getVcsBranch( ), argv[ 1 ] );
    else if ( argc == 3 )
        vcs_hidelabel( argv[ 0 ], argv[ 1 ], argv[ 2 ] );
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_logmsg_callback( )
//
// Callback routine for vcs_type_log.
//////////////////////////////////////////////////////////////////////////////
void vcsui_logmsg_callback( char *szMsg )
    {
    printf( "%s\n", szMsg );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_type_log( )
//
// COMMAND: respond to "log"
//////////////////////////////////////////////////////////////////////////////
void vcsui_type_log( int argc, char *argv[ ] )
    {
    if ( argc != 0 )
        THROW_EXCEPTION( exSyntax );

    vcs_type_log( vcsui_logmsg_callback );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_list_labels( )
//
// COMMAND: respond to "lslab"
//////////////////////////////////////////////////////////////////////////////
void vcsui_list_labels( int argc, char *argv[ ] )
    {
    char szHeader[ _cbHeaderMax + 1 ];
    unsigned n;
    unsigned i;

    if ( argc == 1 )
        {
        LabelHeader *ph = NULL;
        TRY
            {
            n = vcs_list_all_labels_count( argv[ 0 ], szHeader );
            printf( "\nList of Labels for %s\n\n", szHeader );
            if ( n > 0u )
                {
                ph = (LabelHeader *) malloc( n * sizeof( LabelHeader ) );
                if ( !ph )
                    THROW_EXCEPTION( exMemory );
                vcs_list_all_labels( argv[ 0 ], ph, n );
                
                for ( i = 0; i < n; i++ )
                    {
                    printf( "%-16s %-16s %24s\n", ph[ i ].szBranch, 
                                                  ph[ i ].szLabel, 
                                                  vcs_ctime( &ph[ i ].tCreation ) );
                    }
                free( ph );
                }
            else
                {
                printf( "%s\n", _szEmptyList );
                }
            }
        CATCH_ALL
            {
            if ( ph )
                free( ph );
            THROW_LAST( );
            }
        END_CATCH
        }
    else if ( argc == 2 || argc == 3 )
        {
        LabelRec *rgrec = NULL;
        TRY
            {
            if ( argc == 2 )
                n = vcs_list_label_count( argv[ 0 ], getVcsBranch( ), argv[ 1 ], szHeader );
            else
                n = vcs_list_label_count( argv[ 0 ], argv[ 1 ], argv[ 2 ], szHeader );
            printf( "\nList of Labels for %s\n\n", szHeader );
            if ( n > 0u )
                {
                rgrec = (LabelRec *) malloc( n * sizeof( LabelRec ) );
                if ( !rgrec )
                    THROW_EXCEPTION( exMemory );
                if ( argc == 2 )
                    vcs_list_label( argv[ 0 ], getVcsBranch( ), argv[ 1 ], rgrec, n );
                else
                    vcs_list_label( argv[ 0 ], argv[ 1 ], argv[ 2 ], rgrec, n );

                for ( i = 0; i < n; i++ )
                    {
                    if ( rgrec[ i ].type == eFileLabelRec )
                        printf( "%-16s %-16u\n", rgrec[ i ].szFile, 
                                                rgrec[ i ].nVersion );
                    else if ( rgrec[ i ].type == eDirLabelRec )
                        printf( "%-16s %-16s\n", rgrec[ i ].szDir, 
                                                rgrec[ i ].szLabel );
                        
                    else
                        THROW_EXCEPTION( exInternal );
                    }
                free( rgrec );
                }
            else
                {
                printf( "%s\n", _szEmptyList );
                }
            }
        CATCH_ALL
            {
            if ( rgrec )
                free( rgrec );
            THROW_LAST( );
            }
        END_CATCH
        }
    else
        THROW_EXCEPTION( exSyntax );
    }

//////////////////////////////////////////////////////////////////////////////
// vcsui_list_ver
//
// COMMAND: respond to "lsver"
//////////////////////////////////////////////////////////////////////////////
void vcsui_list_ver( int argc, char *argv[ ] )
    {
    char szHeader[ _cbHeaderMax + 1 ];
    FileVersionHeader *ph = NULL;
    unsigned n;
    unsigned i;

    if ( argc != 1 )
        THROW_EXCEPTION( exSyntax );

    TRY
        {
        n = vcs_versions_count( argv[ 0 ], szHeader );
        printf( "\nList of Versions for %s\n\n", szHeader );
        if ( n > 0u )
            {
            ph = (FileVersionHeader *) malloc( n * sizeof( FileVersionHeader ) );
            if ( !ph )
                THROW_EXCEPTION( exMemory );
            vcs_versions( argv[ 0 ], ph, n );

            for ( i = 0; i < n; i++ )
                {
                printf( "%-16s %4u %c %10ld %24s\n", ph[ i ].szBranch, 
                                                     ph[ i ].nVersion,
                                                     ph[ i ].fLock ? 'L' : '*',
                                                     ph[ i ].cb, 
                                                     ph[ i ].fLock ? vcs_ctime( &ph[ i ].t ) : _szEmpty );
                }
            free( ph );
            }
        else
            {
            printf( "%s\n", _szEmptyList );
            }
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
// vcsui_dir( )
//
// COMMAND: respond to "dir"
//////////////////////////////////////////////////////////////////////////////
void vcsui_dir( int argc, char *argv[ ] )
    {
    char szHeader[ _cbHeaderMax + 1 ];
    unsigned n;
    unsigned i;
    Direct *pd  = NULL;
    char   *pch = NULL;

    if ( argc > 1 )
        THROW_EXCEPTION( exSyntax );

    pch = ( argc == 1 ? argv[ 0 ] : _szDot );

    TRY
        {
        n = vcs_directory_count( pch, _A_SUBDIR, szHeader );
        printf( "\nDirectory List for %s\n\n", szHeader );
        if ( n > 0u )
            {
            pd = (Direct *) malloc( n * sizeof( Direct ) );
            if ( !pd )
                THROW_EXCEPTION( exMemory );
            vcs_directory( pch, _A_SUBDIR, pd, n );
            
            for ( i = 0u; i < n; ++i )
                {
                if ( pd[ i ].bAttrib & _A_SUBDIR )
                    {
                    printf( "%-8s %-3s %8s %24s\n", pd[ i ].szFile, 
                                                    pd[ i ].szExt,
                                                    "<DIR>", 
                                                    vcs_ctime( &pd[ i ].t ) );
                    }
                else
                    {
                    printf( "%-8s %-3s %8ld %24s\n", pd[ i ].szFile, 
                                                     pd[ i ].szExt,
                                                     pd[ i ].nSize, 
                                                     vcs_ctime( &pd[ i ].t ) );
                    }
                }
            free( pd );
            }
        else
            {
            printf( "%s\n", _szEmptyList );
            }
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
// getVcsBranch( )
//
// Helper function to get _szVcsBranch
//////////////////////////////////////////////////////////////////////////////
char *getVcsBranch( void )
    {
    if ( !*_szVcsBranch )
        THROW_EXCEPTION( exSyntax );
    return _szVcsBranch;
    }

