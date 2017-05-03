//////////////////////////////////////////////////////////////////////////////
// SLNK
//
// Quote SOFTLINK utility
//
// This computer program was developed by Ward Quinlan, 1997
//
// This program is not part of any Makefile, but may
// be compiled as follows:
//
// cl slnk.c
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

void main ( int argc, char *argv[ ] );
void usage( void );

void main( int argc, char *argv[ ] )
    {
    FILE *pfile;

    if ( argc != 3 )
        {
        usage( );
        return;
        }

    pfile = fopen( argv[ 2 ], "wt" );
    if ( !pfile )
        {
        fprintf( stderr, "slnk: cannot open %s\n", argv[ 2 ] );
        return;
        }

    if ( fprintf( pfile, "@::%s\n", argv[ 1 ] ) < 0 )
        {
        fprintf( stderr, "slnk: cannot write %s\n", argv[ 2 ] );
        fclose( pfile );
        return;
        }

    fclose( pfile );
    fprintf( stderr, "slnk: %s --> %s\n", argv[ 2 ], argv[ 1 ] );
    }

void usage( void )
    {
    fprintf( stderr, "slnk: version 1.00\n" );
    fprintf( stderr, "slnk: usage: slink path new-path\n" );
    }

