///////////////////////////////////////////////////////////
// QOBJECT Library Include File, Version 1.00
//
// Copyright (c) Ward Quinlan, 1996
//
// Include this file to access QObject classes, type
// definitions, and other facilities.
// 
// To make use of the DEBUG QObject library, users must
// declare the following at the top of every source file,
// after including this include file:
//
// static const CHAR THIS_FILE[] = __FILE__;
//
///////////////////////////////////////////////////////////

#ifndef _QOBJECT_HXX
#define _QOBJECT_HXX

///////////////////////////////////////////////////////////
// include files

#include <windows.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "ot.hxx"

///////////////////////////////////////////////////////////
// General type definitions

typedef char CHAR;
typedef unsigned long ULONG;

#define TASKLIST    do
#define ENDTASKLIST while ( 0 );
#define QUIT        break
#define QUITIF( f ) if ( f ) break;

///////////////////////////////////////////////////////////
// Tracing facility
//

#define TR_OFF     0  // All tracing is disabled
#define TR_NORMAL  1  // Normal tracing level
#define TR_INTENSE 2  // Intense tracing level, such as function calls
                      // Note this level includes any normal tracing.

#define cbObjMsgMax ( 256 ) // Maximum size of a Trace message (excluding
                            // the null-terminator)

#ifdef DEBUG
                            // Users should use the ASSERT macro, below,
                            // rather than calling Assert directly.
void Assert( BOOL f, const CHAR *pszFile, int nLine );
                            // Trace call.  The format is the same as
                            // used by the printf family of functions.
                            // Additional parameters are allowed, providing
                            // the total length of the buffer does not
                            // exceed cbObjMsgMax
void Trace( int nLevel, const CHAR *pszFormat, ... );
                            // Interface to set tracing level
void SetTraceLevel( int nLevel );
                            // Interface to retrieve tracing level
int  GetTraceLevel( );
                            // General ASSERT macro.  ASSERT makes
                            // use of the tracing facility, so if
                            // tracing is off, ASSERT will not output
                            // any messages, although an audible beep
                            // is heard.
#define ASSERT( f ) Assert( ( f ), THIS_FILE, __LINE__ )
#else
                            // Stubs which get compiled away in the
                            // non-DEBUG build
inline void Trace( int nLevel, const CHAR *pszFormat, ... ) { }
inline void SetTraceLevel( int nLevel ) { }
inline int  GetTraceLevel( ) { return TR_OFF; }
#define ASSERT( f ) { }
#define THIS_FILE NULL
#endif

///////////////////////////////////////////////////////////
// Utility routines

void FAR *lmemset( void FAR *pv, int b, UINT cb );
void FAR *lmemcpy( void FAR *pvDst, void FAR *pvSrc, UINT cb );

///////////////////////////////////////////////////////////
// Memory management facility
//

#ifdef DEBUG
                            // Interface to set the byte which automatically
                            // initializes allocated blocks of memory.
#define bFillDefault 0xcc
void SetFillerByte( BYTE bFill = bFillDefault );

void KillMemory( );         // any subsequent new's are NULL
void StartMemory( );        // back to normal

                            // Limit the maximum number of bytes available
                            // for allocation to cb.
void  SetMemoryLimit( ULONG lcb );
ULONG GetMemoryLimit( );    // Size of memory limit
ULONG GetMemoryAllocated( );// Number of allocated bytes
void  WalkMemoryTracking( );// walk memory tracking information
                            // Purge any information regarding allocated
                            // blocks.  Note: the data allocated by new
                            // itself is not purged!
void PurgeMemoryTracking( );

BOOL CheckMemory( );        // Checks for corrupt memory objects

                            // General-purpose over-loaded new, with
                            // tracking facilities
void *operator new( UINT cb, const CHAR *pszFileName, int nLine );
                            // overloaded delete
void operator delete( void *pv );

                            // 'new' macro to incorporate memory tracking
#define new new( THIS_FILE, __LINE__ )
#endif

///////////////////////////////////////////////////////////
// QObject Class

class QArchive;
class QObject
    {
public:
    // Construction
    QObject( );                         // throws: exNone

    // Serialization Support            // object storage
    virtual void Store( QArchive &ar ); // throws: exNone

                                        // object retrieval
                                        // throws: exNone
    virtual void Retrieve( QArchive &ar, UINT ot );
    
    // Other Operations
    virtual BOOL IsInit( ) const;       // Determine if object initialized
                                        // throws: exNone
#ifdef DEBUG
    virtual void AssertValid( ) const;  // throws: exInit
#else
                                        // throws: exNone
    inline virtual void AssertValid( ) const { }
#endif    

protected:
    BYTE m_fInit;                       // initialization member
    };

///////////////////////////////////////////////////////////
// QListObject Class

class QListObject : public QObject
    {
friend class QVirtualList;
public:
    // Construction
    QListObject( );                     // throws: exNone

private:
    QListObject *m_plobPrev;             // virtual list support
    QListObject *m_plobNext;
    };

///////////////////////////////////////////////////////////
// QString Class

class QString : public QObject
    {
public:
    // Construction
    QString( );                                   // throws: exNone
    QString( const CHAR *psz );                   // throws: exNone
    QString( const QString &s );                  // throws: exNone

    // Destruction
    virtual ~QString( );                          // throws: exNone
    virtual void Cleanup( );                      // throws: exNone

    // Operators
    operator const CHAR*( ) const;                // throws: exInit
    const QString &operator=( const CHAR *psz );  // throws: exInit, exMemory
    const QString &operator=( const QString &s ); // throws: exInit, exMemory
    const QString &operator+=( const CHAR *psz ); // throws: exInit, exMemory
    CHAR operator[]( int ich ) const;             // throws: exInit

    // Serialization Support
    virtual void Store( QArchive &ar );           // object storage
                                                  // throws: exInit, exFile

                                                  // object retrieval
                                                  // throws: exInit, exFile, exMemory
    virtual void Retrieve( QArchive &ar, UINT ot );
                                                  
    // Other Operations
    UINT GetLength( ) const;                      // throws: exInit
    void Empty( );                                // throws: exInit
    void StringFix( );                            // throws: exInit
    int  Compare( const CHAR *psz ) const;        // throws: exInit
    int  CompareNoCase( const CHAR *psz ) const;  // throws: exInit
#ifdef DEBUG
    void Dump( ) const;                           // throws: exInit
#endif                                            

protected:

private:
    CHAR *m_pch;                                  // pointer to string data
    };

// QString operator+ routines
// throws: exInit, exMemory
const QString Concat( const CHAR *psz1, const CHAR *psz2 );
const QString operator+( const QString &s1, const QString &s2 );
const QString operator+( const QString &s, const CHAR *psz );
const QString operator+( const CHAR *psz, const QString &s );

// QString compare operator routines
// throws: exInit
BOOL operator==( const QString &s1, const QString &s2 );
BOOL operator==( const QString &s, const CHAR *psz );
BOOL operator==( const CHAR *psz, const QString &s );
BOOL operator!=( const QString &s1, const QString &s2 );
BOOL operator!=( const QString &s, const CHAR *psz );
BOOL operator!=( const CHAR *psz, const QString &s );
BOOL operator<( const QString &s1, const QString &s2 );
BOOL operator<( const QString &s, const CHAR *psz );
BOOL operator<( const CHAR *psz, const QString &s );
BOOL operator>( const QString &s1, const QString &s2 );
BOOL operator>( const QString &s, const CHAR *psz );
BOOL operator>( const CHAR *psz, const QString &s );
BOOL operator<=( const QString &s1, const QString &s2 );
BOOL operator<=( const QString &s, const CHAR *psz );
BOOL operator<=( const CHAR *psz, const QString &s );
BOOL operator>=( const QString &s1, const QString &s2 );
BOOL operator>=( const QString &s, const CHAR *psz );
BOOL operator>=( const CHAR *psz , const QString &s );

///////////////////////////////////////////////////////////
// QTime Class
//

#define cbAsciiFormatMax ( 25 )  // max size of runtime routine asctime( ) buffer

class QTime : public QObject
    {
public:
    // Construction
    QTime( );                                     // Constructs using
                                                  // current system time.
                                                  // throws: exNone

    QTime( const QTime &t );                      // throws: exNone
    QTime( time_t t );                            // throws: exNone
    QTime( int nYear, int nMonth, int nDay,       // throws: exNone
           int nHour, int nMinute, int nSecond, 
           BOOL fAdjustDST = FALSE ); 

    // Destruction
    virtual ~QTime( );                            // throws: exNone
    virtual void Cleanup( );                      // throws: exNone

    // operators
    operator QString( ) const;                    // throws: exInit, exMemory, exGeneric
    operator time_t( ) const;                     // throws: exInit
    BOOL operator==( const QTime &t ) const;      // throws: exInit
    BOOL operator!=( const QTime &t ) const;      // throws: exInit
    BOOL operator<( const QTime &t ) const;       // throws: exInit
    BOOL operator>( const QTime &t ) const;       // throws: exInit
    BOOL operator<=( const QTime &t ) const;      // throws: exInit
    BOOL operator>=( const QTime &t ) const;      // throws: exInit
    
    // Serialization Support
    virtual void Store( QArchive &ar );           // object storage
                                                  // throws: exInit, exFile

                                                  // object retrieval
                                                  // throws: exInit, exFile, exMemory
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    void Set( int nYear, int nMonth, int nDay,    // throws: exInit, exGeneric
              int nHour, int nMinute, int nSecond, 
              BOOL fAdjustDST = FALSE );
    int GetYear( ) const;                         // throws: exInit, exGeneric
    int GetMonth( ) const;                        // throws: exInit, exGeneric
    int GetDay( ) const;                          // throws: exInit, exGeneric
    int GetHour( ) const;                         // throws: exInit, exGeneric
    int GetMinute( ) const;                       // throws: exInit, exGeneric
    int GetSecond( ) const;                       // throws: exInit, exGeneric
    int GetWeekDay( ) const;                      // throws: exInit, exGeneric
    int GetYearDay( ) const;                      // throws: exInit, exGeneric

                                   // Use to set string formats for
                                   // casting as a QString.  Refer
                                   // to the run-time routine strftime( )
                                   // for more details.
                                   // throws: exInit, exMemory
    void Format( UINT cbFormatMax, const CHAR *pszFormat );
    
protected:

private:
    time_t  m_t;                   // internal representation of a time
    BOOL    m_fDefaultFormat;      // set to true if using default format
    UINT    m_cbFormatMax;         // size of maximum output string
    QString m_sFormat;             // internal format string
    };

///////////////////////////////////////////////////////////
// QVersion Class
//

#define cbVersionMax ( 32 )
class QVersion : public QObject
    {
public:
    // Construction
                                                 // Default constructor 
                                                 // constructs 0.0.00 version
                                                 // objects
    QVersion( );                                 // throws: exNone
                                                 // throws: exNone
    QVersion( UINT nMajor, UINT nMinor, UINT nRevision );

    // Serialization Support
    virtual void Store( QArchive &ar );          // object storage
                                                 // throws: exInit, exFile

                                                 // object retrieval
                                                 // throws: exInit, exFile, exMemory
    virtual void Retrieve( QArchive &ar, UINT ot );

    // Other Operations
    UINT GetMajorNumber( ) const;                // throws: exInit
    UINT GetMinorNumber( ) const;                // throws: exInit
    UINT GetRevisionNumber( ) const;             // throws: exInit
    operator const CHAR *( ) const;              // throws: exInit
    BOOL operator==( const QVersion &v ) const;  // throws: exInit

private:
    UINT m_nMajor;
    UINT m_nMinor;
    UINT m_nRevision;
    CHAR m_szVersion[ cbVersionMax + 1 ];
    };

///////////////////////////////////////////////////////////
// QArchive Class
//

#define nModeRead  0
#define nModeWrite 1
class QArchive : public QObject
    {
public:
    // Construction
    QArchive( );                                  // throws: exNone
    
    // Destruction
    ~QArchive( );                                 // throws: exNone
    virtual void Cleanup( );                      // throws: exNone

    // Other Operations
                                                  // throws: exInit
    BOOL Open( const CHAR *pszFile, int nMode = nModeRead );
    void Close( );                                // throws: exInit
    BOOL ReadHeader( UINT *pot );                 // throws: exInit, exFile
    UINT Read( void *pv, UINT cb );               // throws: exInit, exFile
    void WriteHeader( UINT ot );                  // throws: exInit, exFile
    void Write( const void *pv, UINT cb );        // throws: exInit, exFile

private:
    FILE *m_pfile;                                // file handle
    };

// Serialization Support
#define otQObject 0x0800 // Start of QObject object types
#define otUser    0x1000 // Start of user object types

// Utility routines
BOOL FileExists( const CHAR *pszFile );           // throws: exNone

///////////////////////////////////////////////////////////
// QVirtualList Class
//

class QVirtualList : public QObject
    {
public:
    // Construction
    QVirtualList( );                             // throws: exNone

    // Add Opertions
    void AddHead( QListObject *plob );           // throws: exInit
    void AddTail( QListObject *plob );           // throws: exInit

    // Insert Operations
                                                 // Inserts plobNew BEFORE
                                                 // plobList in the list
                                                 // throws: exInit
    void InsertBefore( QListObject *plobList, QListObject *plobNew );
                                                 // Inserts plobNew AFTER
                                                 // plobList in the list
                                                 // throws: exInit
    void InsertAfter( QListObject *plobList, QListObject *plobNew );

    // Remove Operations
    QListObject *RemoveHead( );                  // throws: exInit
    QListObject *RemoveTail( );                  // throws: exInit
    BOOL Remove( QListObject *plob );            // throws: exInit
    
    // Iteration Operations
    QListObject *GetHead( ) const;               // throws: exInit
    QListObject *GetTail( ) const;               // throws: exInit
                                                 // throws: exInit
    QListObject *GetNext( const QListObject *plob ) const;
                                                 // throws: exInit
    QListObject *GetPrev( const QListObject *plob ) const;

    // Other Operations
    UINT GetCount( ) const;                      // returns # of elements 
                                                 // in the list
                                                 // throws: exInit

                                                 // returns true if given
    BOOL IsExist( const QListObject *plob ) const;
                                                 // object is in the list
                                                 // throws: exInit
#ifdef DEBUG
    virtual void AssertValid( ) const;           // throws: exInit
#endif

protected:
    // Abstract Base Class (must be over-ridden)
    virtual void Abstract( ) = 0;                // throws: n/a

private:
    QListObject *m_plobHead;                     // head of the list
    QListObject *m_plobTail;                     // tail of the list
    UINT     m_nCount;                           // cardinality of the list
    };

///////////////////////////////////////////////////////////
// Exception Handling
//

#define exNone    0x00   // no exception thrown.  NEVER THROW THIS EXCEPTION!!!
#define exInit    0x01   // an initialization exception thrown.  A
                         // parameter, or the object itself, was
                         // not initialized properly, and an attempt
                         // was being made to use it in this state.
#define exMemory  0x02   // out-of-memory exception
#define exGeneric 0x03   // generic exception
#define exFile    0x04   // file exception
#define exUser    0x100  // users may define their own exceptions
                         // from here

                         // internal exception buffer.  Not needed
                         // by users if using exception macros, below.
extern  CATCHBUF _catchbuf;

                         // internal exception buffer copy routine
#define catchcpy( catchbufDst, catchbufSrc ) \
        lmemcpy( catchbufDst, catchbufSrc, sizeof( CATCHBUF ) )

#define TRY           {                                   \
                      CATCHBUF catchbufSav;               \
                      int __ex;                           \
                      catchcpy( catchbufSav, _catchbuf ); \
                      __ex = Catch( _catchbuf );          \
                      if ( __ex == exNone )

#define CATCH( ex )   else if ( __ex == ex )

#define CATCH_ALL     else

#define END_CATCH     catchcpy( _catchbuf, catchbufSav ); \
                      }

#define THROW( ex )   catchcpy( _catchbuf, catchbufSav ); \
                      Throw( _catchbuf, ex )

#define THROW_LAST( ) catchcpy( _catchbuf, catchbufSav ); \
                      Throw( _catchbuf, __ex )

#define THROW_EXCEPTION( ex ) Throw( _catchbuf, ex )

#define DEFAULT_EXCEPTION_BLOCK if ( Catch( _catchbuf ) != exNone )

///////////////////////////////////////////////////////////
// Miscellaneous items

#if ( FDATA == 1 )
extern const CHAR FAR _szMemory[];
extern const CHAR FAR _szInit[];
extern const CHAR FAR _szGeneric[];
extern const CHAR FAR _szUnknown[];
extern const CHAR FAR _szNull[];
extern const CHAR FAR _szCR[];
#ifdef DEBUG
extern const CHAR FAR _szAssert[];
#endif
#else
extern const CHAR _szMemory[];
extern const CHAR _szInit[];
extern const CHAR _szGeneric[];
extern const CHAR _szUnknown[];
extern const CHAR _szNull[];
extern const CHAR _szCR[];
#ifdef DEBUG
extern const CHAR _szAssert[];
#endif
#endif

extern const QVersion _vQObject;

#define cbFileNameMax ( 8 + 1 + 3 ) // maximum file name length

#ifdef TEST
extern void QTest( ); // your application may call this routine
                      // to automatically test the integrity of
                      // the QObject library.
#endif

#endif

