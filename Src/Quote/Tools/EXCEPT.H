//////////////////////////////////////////////////////////////////////////////
// EXCEPT.H
//
// This computer program is copyright (c) Ward Quinlan, 1996, 1997
//
// Exception handling macros
//////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <setjmp.h>

typedef jmp_buf CATCHBUF;
#define Catch( catchbuf )     setjmp( catchbuf )
#define Throw( catchbuf, ex ) longjmp( catchbuf, ex )

#define catchcpy( catchbufDst, catchbufSrc ) \
    memcpy( catchbufDst, catchbufSrc, sizeof( CATCHBUF ) )

extern CATCHBUF _catchbuf;

// exception codes
#define exNone        0x00 // no exception
#define exUser        0x10 // beginning of user exceptions

#define TRY                             \
    {                                   \
    CATCHBUF catchbufSav;               \
    int      __ex;                      \
                                        \
    catchcpy( catchbufSav, _catchbuf ); \
    __ex = Catch( _catchbuf );          \
    if ( __ex == exNone )

#define CATCH( ex )                     \
    else if ( __ex == ex )

#define CATCH_ALL else

#define END_CATCH                       \
    catchcpy( _catchbuf, catchbufSav ); \
    }

#define THROW( ex )                     \
    catchcpy( _catchbuf, catchbufSav ); \
    Throw( _catchbuf, ex );

#define THROW_LAST( )                   \
    catchcpy( _catchbuf, catchbufSav ); \
    Throw( _catchbuf, __ex )

#define THROW_EXCEPTION( ex )           \
    Throw( _catchbuf, ex )

#define DEFAULT_EXCEPTION_BLOCK         \
    if ( Catch( _catchbuf ) != exNone )

