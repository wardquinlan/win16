#include "qobject.hxx"
#include <stdio.h>
#include <stdarg.h>

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

// Global variables
static int _nTraceLevel = TR_NORMAL;

int GetTraceLevel( )
    {
    return _nTraceLevel;
    }

void Assert( BOOL f, const CHAR *pszFile, int nLine )
    {
    if ( !f )
        {
        MessageBeep( 0 );
        Trace( TR_NORMAL, _szAssert, pszFile, nLine );
        }
    }

void SetTraceLevel( int nLevel )
    {
    static const CHAR *const rgsz[] = { "TR_OFF", "TR_NORMAL", "TR_INTENSE" };
    int  n;
    TASKLIST
        {
        if ( nLevel != TR_OFF && nLevel != TR_NORMAL && nLevel != TR_INTENSE )
            {
            QUIT;
            }
        if ( nLevel == TR_OFF )
            {
            n = 0;
            }
        else if ( nLevel == TR_NORMAL )
            {
            n = 1;
            }
        else if ( nLevel == TR_INTENSE )
            {
            n = 2;
            }
        else
            {
            ASSERT( FALSE );
            QUIT;
            }
#if ( FDATA == 1 )
        Trace( TR_NORMAL, "Tracing level now set to %Fs.\n", rgsz[ n ] );
#else
        Trace( TR_NORMAL, "Tracing level now set to %s.\n", rgsz[ n ] );
#endif
        _nTraceLevel = nLevel;
        }
    ENDTASKLIST
    }

void Trace( int nLevel, const CHAR *pszFormat, ... )
    {
    va_list marker;
    struct tm *ptm;
    time_t t;
    CHAR szBuffer[ cbObjMsgMax + 1 ];
    CHAR szTime[ 32 + 1 ];

    TASKLIST
        {
        if ( nLevel != TR_NORMAL && nLevel != TR_INTENSE )
            {
            QUIT;
            }
        if ( nLevel != _nTraceLevel &&
             _nTraceLevel != TR_INTENSE )
            {
            QUIT;
            }
        va_start( marker, pszFormat );
        vsprintf( szBuffer, pszFormat, marker );
        va_end( marker );
        ASSERT( lstrlen( szBuffer ) < sizeof( szBuffer ) );
        t = time( NULL );
        ptm = localtime( &t );
        strftime( szTime, sizeof( szTime ), "%c: ", ptm );
        ASSERT( lstrlen( szTime ) < sizeof( szTime ) );
        OutputDebugString( szTime );
        OutputDebugString( szBuffer );
        }
    ENDTASKLIST
    }
