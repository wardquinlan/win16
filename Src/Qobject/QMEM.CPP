#define _QMEM_CPP
#include "qobject.hxx"
#include <windowsx.h>

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#undef new
#endif

#if ( FDATA == 1 )
#define MIAlloc( lcb )  ( GlobalAllocPtr( GMEM_MOVEABLE, ( DWORD ) ( lcb ) ) )
#define MIFree( lpb )   ( GlobalFreePtr( lpb ) )
#else
#define MIAlloc( cb )   ( LocalAlloc( LMEM_FIXED, ( UINT ) cb ) )
#define MIFree( pb )    ( LocalFree( ( HLOCAL ) pb ) )
#endif

// Local data types
typedef struct tagLINK
    {
    BYTE  *pbCooked;
    UINT   cbCooked;
    CHAR   szFileName[ cbFileNameMax + 1 ];
    int    nLine;
    struct tagLINK *plinkNext;
    } LINK, *PLINK;

static const ULONG _ulGuard = 0xaaaaaaaa;

// static global variables
static BYTE  _bFill           = bFillDefault;
static BOOL  _fMemory         = TRUE;
static ULONG _lcbAllocSizeCur = 0ul;    // current count of size
static ULONG _lcbAllocSizeMax = 4096ul; // maximum count of size

static LINK *_plinkTop = NULL;          // Top of the linked list

// static functions
static BOOL Link( BYTE *pbCooked, UINT cbCooked, const CHAR *pszFileName, int nLine );
static BOOL Unlink( BYTE *pbCooked );
static void *NewAllocate( UINT cbCooked, const CHAR *pszFileName, int nLine);
static UINT GetLinkSize( BYTE *pbCooked );

void SetFillerByte( BYTE bFill )
    {
    Trace( TR_INTENSE, "SetFillerByte( BYTE ) called.\n" );
    _bFill = bFill;
    }

void StartMemory( )
    {
    Trace( TR_INTENSE, "StartMemory( ) called.\n" );
    _fMemory = TRUE;
    }

void KillMemory( )
    {
    Trace( TR_INTENSE, "StartMemory( ) called.\n" );
    _fMemory = FALSE;
    }
  
void SetMemoryLimit( ULONG lcb )
    {
    Trace( TR_INTENSE, "AllocLimitSize( UINT ) called.\n" );
    _lcbAllocSizeMax = max( lcb, _lcbAllocSizeCur );
    }

ULONG GetMemoryLimit( )
    {
    Trace( TR_INTENSE, "GetMemoryAllocated( ) called.\n" );
    return _lcbAllocSizeMax;
    }

ULONG GetMemoryAllocated( )
    {
    Trace( TR_INTENSE, "GetMemoryAllocated( ) called.\n" );
    return _lcbAllocSizeCur;
    }

void *operator new( UINT cb, const CHAR *pszFileName, int nLine )
    {
    return NewAllocate( cb, pszFileName, nLine );
    }

void *NewAllocate( UINT cbCooked, const CHAR *pszFileName, int nLine )
    {
    BYTE *pbCooked = NULL;
    BYTE *pbRaw    = NULL;
    UINT  cbRaw;
    
    TASKLIST
        {
        if ( !_fMemory )
            {
            QUIT;
            }

        if ( cbCooked == 0 )
            {
            ASSERT( FALSE );
            QUIT;
            }

        if ( _lcbAllocSizeCur + (ULONG) cbCooked > _lcbAllocSizeMax )
            {
            QUIT;
            }
        _lcbAllocSizeCur += cbCooked;
        
        cbRaw = cbCooked + sizeof( _ulGuard );
        pbRaw = (BYTE *) MIAlloc( cbRaw );
        if ( !pbRaw )
            {
            QUIT;
            }
        lmemset( pbRaw, _bFill, cbRaw );
        pbCooked = pbRaw;
        ( *( (ULONG *) ( pbCooked + cbCooked ) ) ) = _ulGuard;

        if ( !Link( pbCooked, cbCooked, pszFileName, nLine ) )
            {
            MIFree( pbRaw );
            pbRaw = NULL;
            pbCooked = NULL;
            QUIT;
            }
        }
    ENDTASKLIST
    return (void *) pbCooked;
    }

void operator delete( void *pv )
    {
    BYTE *pbCooked = (BYTE *) pv;
    UINT cbCooked;
    BYTE *pbRaw;
    TASKLIST
        {
        if ( !_fMemory )
            {
            QUIT;
            }

        if ( !pbCooked )
            {
            // by C++ definition
            QUIT;
            }
        pbRaw = pbCooked;
        cbCooked = GetLinkSize( pbCooked );
        ASSERT( cbCooked > 0 );
        if ( !Unlink( pbCooked ) )
            {
            ASSERT( FALSE );
            QUIT;
            }
        ASSERT( _lcbAllocSizeCur >= cbCooked );
        _lcbAllocSizeCur -= cbCooked;
        MIFree( pbRaw );
        }
    ENDTASKLIST
    }

void WalkMemoryTracking( )
    {
    PLINK plink;
    Trace( TR_INTENSE, "WalkMemoryTracking( ) called.\n" );
    if ( _plinkTop )
        {
        Trace( TR_NORMAL, "The following memory objects are tracked:\n" );
        }
    else
        {
        Trace( TR_NORMAL, "<There are no memory objects currently tracked.>\n" );
        }
    for ( plink = _plinkTop; plink; plink = plink->plinkNext )
        {
#if ( FDATA == 1 )
        Trace( TR_NORMAL, "%#lx (%u bytes) <File %Fs Line %d>.\n", 
               plink->pbCooked, plink->cbCooked, plink->szFileName, plink->nLine );
#else
        Trace( TR_NORMAL, "%#x (%u bytes) <File %s Line %d>.\n", 
               plink->pbCooked, plink->cbCooked, plink->szFileName, plink->nLine );
#endif
        }
    }

static BOOL Link( BYTE *pbCooked, UINT cbCooked, const CHAR *pszFileName, int nLine )
    {
    PLINK plink;
    BOOL fRet = FALSE;
    TASKLIST
        {
        plink = (PLINK) MIAlloc( sizeof( *plink ) );
        if ( !plink )
            {
            QUIT;
            }
        plink->pbCooked = pbCooked;
        plink->cbCooked = cbCooked;
        lstrcpy( plink->szFileName, pszFileName );
        plink->nLine = nLine;
        plink->plinkNext = _plinkTop;
        _plinkTop = plink;
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

static UINT GetLinkSize( BYTE *pbCooked )
    {
    UINT cbCooked = 0; // size of 0 means we didn't find the memory object
    for ( PLINK plink = _plinkTop; plink; plink = plink->plinkNext )
        {
        if ( plink->pbCooked == pbCooked )
            {
            cbCooked = plink->cbCooked;
            break;
            }
        }
    return cbCooked;
    }

static BOOL Unlink( BYTE *pbCooked )
    {
    PLINK plink;
    PLINK plinkPrev;
    PLINK plinkTmp;
    BOOL fRet = FALSE;
    TASKLIST
        {
        plinkPrev = NULL;
        plink = _plinkTop;
        while ( plink )
            {
            if ( plink->pbCooked == pbCooked )
                {
                break;
                }
            plinkPrev = plink;
            plink = plink->plinkNext;
            }
        if ( !plink )
            {
            // Not found case
            QUIT;
            }
        if ( plinkPrev )
            {
            // middle of the list case
            plinkPrev->plinkNext = plink->plinkNext;
            MIFree( plink );
            fRet = TRUE;
            }
        else
            {
            // head of the list case
            plinkTmp = _plinkTop;
            _plinkTop = _plinkTop->plinkNext;
            MIFree( plinkTmp );
            fRet = TRUE;
            }
        }
    ENDTASKLIST
    return fRet;
    }

void PurgeMemoryTracking( )
    {
    PLINK plinkSav;
    PLINK plink = _plinkTop;
    Trace( TR_INTENSE, "PurgeMemoryTracking( ) called.\n" );
    while ( plink )
        {
        plinkSav = plink;
        plink = plink->plinkNext;
        MIFree( plinkSav );
        }
    _plinkTop = NULL;
    }

BOOL CheckMemory( )
    {
    PLINK plink;
    ULONG ulGuard;
    BOOL  f = FALSE;

    Trace( TR_INTENSE, "CheckMemory( ) called.\n" );
    Trace( TR_NORMAL, "The following memory objects are corrupt:\n" );
    for ( plink = _plinkTop; plink; plink = plink->plinkNext )
        {
        ulGuard  = *(ULONG *)( plink->pbCooked + plink->cbCooked );
        if ( ulGuard != _ulGuard )
            {
#if ( FDATA == 1 )
            Trace( TR_NORMAL, "%#lx (%u bytes) <File %Fs Line %d>.\n", 
                   plink->pbCooked, plink->cbCooked, plink->szFileName, plink->nLine );
#else
            Trace( TR_NORMAL, "%#x (%u bytes) <File %s Line %d>.\n", 
                   plink->pbCooked, plink->cbCooked, plink->szFileName, plink->nLine );
#endif
            f = TRUE;
            }
        }
    if ( !f )
        {
        Trace( TR_NORMAL, "<No corrupt memory objects were found.>\n" );
        }
    return !f;
    }

