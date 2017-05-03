                    QObject Class Library, Version 1.00
                    ===================================


Copyright (c) Ward Quinlan, 1996

Library Build Flags
===================

The following environment variables may be used when
building the QObject library:

DEBUG - Set this flag if you want a DEBUG version
        of the library.  This includes ASSERT,
        Tracing, and memory management facilities.

        To make use of the DEBUG QObject library, users must
        declare the following at the top of every source file,
        after including this include file:

        static const CHAR THIS_FILE[] = __FILE__;

TEST  - Set this flag if you want a TEST version
        of the library.  Users can then call the
        QTest routine which automatically verifies
        the integrity of library functionality.

Tracing (DEBUG only)
=======

Tracing is done to the auxillary debug port, if availabe.
It can also be done using the DBWin facility if available
with your compiler.  Tracing is only available in a DEBUG
build.

Memory Management (DEBUG only)
=================

The memory management facility allows users to simulate
out-of-memory conditions after a certain number of calls
to new, or alternatively, after a certain size has been
reached.  The facility also automatically intializes
allocated blocks of memory, and provides an interface
to detect memory leaks (memory tracking component).  
The facility is only available in a DEBUG build.

QObject Class
=============

The QObject class is the root of all QObject library classes.
The class supports the virtual list collection class,
and a facility to manage whether classes have been successfully 
constructed.

QObject classes may throw exceptions during normal processing;
however, it is guaranteed that neither a constructor nor a 
destructor will EVER throw an exception.

To manage exceptional conditions during object construction,
the m_fInit and IsInit( ) members are used.  Essentially,
once the object has been successfully constructed, the
m_fInit member is set to TRUE.  Any calls into the library
will test this member, and if unset, will throw an exInit
exception.

Also note that only constructors will ever set this
member.  Once an object is successfully constructed,
this member will never be reset.

Also note the Cleanup( ) member.  Due to limitations of
MSVC 1.0, destructors are not called during the processing
of an exception.  Therefore, the Cleanup( ) member 
may be called which deletes any memory which the object
may have allocated.  Cleanup also guarantees never to
throw an exception.  There may still be certain circumstances
for which neither destructors nor Cleanup may be called
if an exception is thrown, for example, in the case of
temporary objects being used.

QString Class
=============

QObject strings support standard string operations.

QTime Class
===========

QObject time objects supports standard time operations,
except time arithmetic.

QVersion Class
==============

The version class provides support for versioning.  Versioning
is in the form of:

M.m.rr, where:

M is a major version number 
m is a minor version number
rr is a revision number

Note that a change in M or m constitutes an interface or otherwise
large change.  A change in rr constitutes only implementation changes.

The QObject library itself is a represented by the 
QVersion object _vQObject.

QArchive Class
==============

The QArchive class is used in conjunction with serialization, below.
A routine to check for file existence, FileExists( ), is also 
provided.  An example is given below.

QListObject Class
=================

This class, derived from QObject, supports being a member
of the QVirtualList collection class.  Classes wishing
to facilitate collection classes must derive their
classes from QListObject

QVirtualList Class
==================

This abstract class supports a collection class QListObjects,
or their sub-classes.

The class is implemented as a doubly-linked list, and
the expected facilities are supported.

This class is intentionally written to be subclassed.
Only one QListObject type is recommended in each
derived class, for typesafe reasons.  If more than one
type is used in a list, a mechanism to determine the 
types will be necessary to prevent unsafe casts.

Users must over-ride the Abstract( ) member in order
to instantiate the class.  Abstract( ) is only a place-
holder; do not expect to perform any processing in 
Abstract( ).

Note that the virtual class itself does not allocate
nor deallocate memory; it relies on existing QListObject's
and modifies private previous and next pointers.

Exception Handling
==================

The exception handling facility is built on top
of the Windows Catch( ) and Throw( ) routines.
The major weakness of this scheme is destructors
to any local objects are not called if an
exception is thrown.  Therefore, it is recommended
that any objects allocated on the Frame call
their Cleanup( ) members if an exception is about
to be thrown.  This does not guarantee complete
certainty, however, as tempory objects can be
created from some routines, such as 
operator+( const QString &, const QString & ).  There
is simply no way to call any Cleanup routine for such
temporary object, and a memory leak may be introduced.

The following 'keywords' are introduced:

TRY                   - a TRY initiates an exception block, and any 
                        following operations are attempted.

CATCH( ex )           - CATCH catches the 'ex' exception which may have 
                        been thrown in the TRY block.

CATCH_ALL             - CATCH_ALL catches ANY remaining exceptions which
                        may have been thrown in the TRY block.
                        Note that CATCH_ALL must follow any CATCH's in the
                        block.

END_CATCH             - END_CATCH terminates the exception block.

THROW( ex )           - THROW throws the exception ex.  THROW is used
                        to throw exceptions from within an exception block
                        to the previous exception block.
                        DO NOT ATTEMPT TO USE THROW OUTSIDE AN EXCEPTION BLOCK!! 
                        (use THROW_EXCEPTION instead).

THROW_LAST( )         - THROW_LAST propagates the current exception.
                        This must be used within an exception block.

THROW_EXCEPTION( ex ) - Similar to THROW, but normally used OUTSIDE an exception
                        block.  In certain cases THROW_EXCEPTION can be
                        used within the same exception block; see example
                        below.

GET_EXCEPTION_CODE( ) - returns the exception code from the current  
                        exception.  You can use this macro in an
                        catch block.

DEFAULT_EXCEPTION_BLOCK - Use this macro to write a default exception
                          handler, to handle exceptions which no one
                          else catches.

Note: semi-colons are required for THROW, THROW_LAST and 
THROW_EXCEPTION macros but not for the others.

Following are examples of exception blocks:

    Example 1
    =========
    
    TRY // try to perform the following operations
        {
        // some operations
        }
    CATCH_ALL // catch ALL exceptions
        {
        // cleanup processing goes here
        }
    END_CATCH // finish up the exception block
    
    Example 2
    =========
    
    QString s( "Hello World!" );
    TRY // try to perform the following operations
        {
        f( s );
        // other operations
        }
    CATCH( exInit )
        {
        // recover from initialized exceptions
        s.Cleanup( );
        }
    CATCH( exMemory )
        {
        // recover from memory exceptions
        s.Cleanup( );
        THROW_LAST( ); // propagate the exception
        }
    CATCH_ALL
        {
        // recover from all other exceptions
        s.Cleanup( );
        // throws the exception to the PREVIOUS exception block
        THROW( exGeneric );
        }
    ENDCATCH // finish the exception block
    
    Example 3
    =========
    
    ...
    CHAR *psz = new CHAR[ 10 ];
    if ( psz == NULL )
        {
        THROW_EXCEPTION( exMemory );
        }
    
    Example 4
    =========
    
    WinMain( )
    ...
    DEFAULT_EXCEPTION_BLOCK
        {
        MessageBox( ... );
        // terminate the application
        }
    
    Example 5
    =========
    
    BYTE *pb1 = NULL, *pb2 = NULL;
    QString s( "Hello World" );
    TRY
        {
        f( s );
        pb1 = new BYTE[ 10 ];
        if ( !pb1 )
            {
            // throw an exception to the CURRENT exception block
            THROW_EXCEPTION( exMemory );
            }
        pb2 = new BYTE[ 10 ];
        if ( !pb2 )
            {
            // throw an exception to the CURRENT exception block
            THROW_EXCEPTION( exMemory );
            }
        }
    CATCH( exMemory )
        {
        delete pb1;
        delete pb2;
        s.Cleanup( );
        // propagate the exception to the PREVIOUS exception block
        THROW_LAST( );
        }
    CATCH_ALL
        {
        delete pb1;
        delete pb2;
        s.Cleanup( );
        // Throw an exception to the previous exception block
        THROW( exGeneric );
        }
    END_CATCH

    Example 6
    =========

    TRY
        {
        ...
        }
    CATCH_ALL
        {
        printf( "The exception code is %d.\n", GET_EXCEPTION_CODE( ) );
        }
    END_CATCH

Serialization
=============

The following qobject classes support serialization:

o  QString
o  QTime
o  QVersion

Users may also write their own serialization code.  The only 'rule'
is users must preceed their object data with an 'object type'.  User
types start at otUser (defined from qobject.hxx).  The object type
is 2 bytes long.  After this, users may write any data they choose
to serialize their objects.

Consider the following example.

    Example 1
    =========

    #define otWidget ( otUser + 0 )
    QWidget::Store( QArchive &ar ) const
        {
        ar.WriteHeader( otWidget );
        ar.Write( &m_nWidget, sizeof( m_nWidget ) );
        }

    QWidget::Retrieve( QArchive &ar, UINT ot )
        {
        ASSERT( ot == otWidget );
        ar.Read( &m_nWidget, sizeof( m_nWidget ) );
        }

    main( )
        {
        QArchive ar;
        QWidget  w;
        UINT     ot;
        ar.Open( "widget.dat", nModeWrite );
        w.Store( ar );
        ar.Close( );

        ar.Open( "widget.dat", nModeRead );
        ar.ReadHeader( &ot );
        w.Retrieve( ar, ot );
        ar.Close( );
        }
