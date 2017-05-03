#include <stdio.h>
#include <string.h>
#include <dos.h>

void _interrupt _far fCtrlC( );

void ( _interrupt _far *_pfCtrlCSav )( );

int _nCount = 0;
int _fBlock = 0;

void main( int argc, char *argv[ ] );

void main( int argc, char *argv[ ] )
    {
    long i;
    long j;

    _fBlock = ( argc == 2 && strcmp( argv[ 1 ], "BLOCK" ) == 0 );

    if ( _fBlock )
        {
        _pfCtrlCSav = _dos_getvect( 0x23 );
        _dos_setvect( 0x23, fCtrlC );
        }

    for ( i = 0; i < 1000; ++i )
        {
        printf( "DOS doing stuff (%ld)...\n", i );
        }
    printf( "DOS done; Ctrl-C hit %d times\n", _nCount );

    if ( _fBlock )
        _dos_setvect( 0x23, _pfCtrlCSav );
    }

void _interrupt _far fCtrlC( )
    {
    _nCount++;
    }
