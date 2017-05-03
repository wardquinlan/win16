#include "qobject.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

// #define DOASSERT

void QTestMem( );
void QTestTrace( );
void QTestString( );
void QTestTime( );
void QTestList( );
void QTestArchive( );

class QListString : public QListObject
    {
public:
    QListString( ) { }
    QListString( const CHAR *psz ) { SetString( psz ); }
    ~QListString( ) { Cleanup( ); }
    virtual void Cleanup( ) { m_s.Cleanup( ); }
    void     SetString( const CHAR *psz ) { m_s = psz; } 
    QString &GetString( ) { return m_s; }
private:
    QString m_s;
    };

class QDictionary : public QVirtualList
    {
public:
    virtual ~QDictionary( );
    void Cleanup( );
    void Abstract( ) { }
    void Dump( ) const;
private:
    };
    
QDictionary::~QDictionary( )
    {
    Cleanup( );
    }

void QDictionary::Cleanup( )
    {
    QListString *pls;
    while ( pls = (QListString *)RemoveHead( ) )
        {
        delete pls;
        }
    }

void QDictionary::Dump( ) const
    {
    QListString *pls;

    Trace( TR_NORMAL, "Start of dictionary dump.\n" );
    for ( pls = (QListString *) GetHead( );
          pls;
          pls = (QListString *) GetNext( pls ) )
        {
        pls->GetString( ).Dump( );
        }
    Trace( TR_NORMAL, "End of dictionary dump.\n" );
    }

void QTestList( )
    {
    QDictionary list;
    QListString *pls;
    QListObject *plob;
    QListObject *plobSav;

    TRY
        {
        list.AssertValid( );

        pls = new QListString( "String 1" );
        if ( !pls )
            {
            THROW_EXCEPTION( exMemory );
            }
        list.AddHead( (QListObject *) pls );
        list.AssertValid( );

        plobSav = pls = new QListString( "String 22" );
        if ( !pls )
            {
            THROW_EXCEPTION( exMemory );
            }
        list.AddTail( (QListObject *) pls );
        list.AssertValid( );

        pls = new QListString( "String 333" );
        if ( !pls )
            {
            THROW_EXCEPTION( exMemory );
            }
        list.AddHead( (QListObject *) pls );
        list.AssertValid( );

        pls = new QListString( "String 4444" );
        if ( !pls )
            {
            THROW_EXCEPTION( exMemory );
            }
        list.AddTail( (QListObject *) pls );
        list.AssertValid( );

        pls = new QListString( "String 55555" );
        if ( !pls )
            {
            THROW_EXCEPTION( exMemory );
            }
        plob = list.GetTail( );
        list.InsertBefore( plob, (QListObject *) pls );

        pls = new QListString( "String 666666" );
        if ( !pls )
            {
            THROW_EXCEPTION( exMemory );
            }
        plob = list.GetHead( );
        list.InsertAfter( plob, pls );
        list.AssertValid( );
        list.Dump( );

        plob = list.GetHead( );
        while ( plob )
            {
            ((QListString *)plob)->GetString( ).Dump( );
            plob = list.GetNext( plob );
            }
        plob = list.GetTail( );
        while ( plob )
            {
            ((QListString *)plob)->GetString( ).Dump( );
            plob = list.GetPrev( plob );
            }
        ASSERT( list.IsExist( plobSav ) );

        ASSERT( list.Remove( plobSav )  );
        delete (QListString *) plobSav;
        ASSERT( ( plob = list.RemoveHead( ) ) != NULL );
        delete (QListString *) plob;
        ASSERT( ( plob = list.RemoveTail( ) ) != NULL );
        delete (QListString *) plob;
        list.AssertValid( );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, "An exception has been thrown.\n" );
        list.Cleanup( );
        }
    END_CATCH
    }

void QTestMem( )
    {
    int  i;
    UINT cb;
    CHAR *psz;
    BYTE *pb;
    
    SetFillerByte( 0x35 );
    psz = new CHAR[ 10 ];
    psz[9] = '\0';
    ASSERT( lstrcmp( psz, "555555555" ) == 0 );
    delete psz;

    SetFillerByte( );
    pb = new BYTE[ 10 ];
    ASSERT( *pb == bFillDefault );
    delete pb;

    for ( i = 0; i < 10; i++ )
        {
        psz = new CHAR[ 10 ];
        ASSERT( psz != NULL );
        delete psz;
        }
    
    cb = (UINT) ( GetMemoryLimit( ) - GetMemoryAllocated( ) );
    psz = new CHAR[ cb ];
    ASSERT( psz != NULL );
    pb = new BYTE;
    ASSERT( pb == NULL );
    delete psz;
    ASSERT( cb == (UINT) ( GetMemoryLimit( ) - GetMemoryAllocated( ) ) );

#ifdef DOASSERT
    i = 0;
    psz = new CHAR[ i ];
    ASSERT( psz == NULL );
    delete psz;
#endif

#ifdef DOASSERT
    Trace( TR_NORMAL, "Deleting invalid pointer.\n" );
    delete( (void *) 0x100 ); // This should ASSERT

    CHAR *pch1 = new CHAR[ 10 ];
    lstrcpy( pch1, "1234567890" );

    // memory leak
    CHAR *pch2 = new CHAR[ 10 ];
    lstrcpy( pch3, "123456789" );
    }
#endif
    }

void QTestTrace( )
    {
    int nLevelSav;
    CHAR *psz;

    nLevelSav = GetTraceLevel( );
    SetTraceLevel( TR_OFF );
#ifdef DOASSERT
    Trace( TR_NORMAL, "Assert should not appear.\n" );
    ASSERT( 0 ); // Should not appear
#endif
    Trace( TR_NORMAL,  "This message should not appear.\n" );
    Trace( TR_INTENSE, "This message should not appear.\n" );
    SetTraceLevel( TR_NORMAL );
#ifdef DOASSERT
    Trace( TR_NORMAL, "Assert should appear.\n" );
    ASSERT( 0 ); // Should appear
#endif
    Trace( TR_NORMAL,  "This TR_NORMAL message should appear.\n" );
    Trace( TR_INTENSE, "This TR_INTENSE message should not appear.\n" );
    SetTraceLevel( TR_INTENSE );
#ifdef DOASSERT
    Trace( TR_NORMAL, "Assert should appear.\n" );
    ASSERT( 0 ); // Should appear
#endif
    Trace( TR_NORMAL,  "This TR_NORMAL message should appear.\n" );
    Trace( TR_INTENSE, "This TR_INTENSE message should appear.\n" );
    SetTraceLevel( nLevelSav );
    SetFillerByte( 0 );
    psz = new CHAR[ cbObjMsgMax + 1 ];
    ASSERT( psz != NULL );
    lmemset( psz, 'x', cbObjMsgMax - 1 );
    psz[ cbObjMsgMax - 1 ] = '\n';
    Trace( TR_NORMAL, psz ); // This should be fine
    delete psz;
    SetFillerByte( );

#ifdef DOASSERT
    psz = new CHAR[ cbObjMsgMax + 2 ];
    ASSERT( psz != NULL );
    lmemset( psz, 'x', cbObjMsgMax );
    psz[ cbObjMsgMax ] = '\n';
    Trace( TR_NORMAL, "Invalid Trace Message.\n" );
    Trace( TR_NORMAL, psz ); // This should ASSERT
    delete psz;
#endif
    }

void QTestString( )
    {
    QString s1;
    ASSERT( (const CHAR *) s1 == NULL );

    QString s2( "This is string s2." );
    ASSERT( s2 == "This is string s2." );
    
    QString s3( s1 );
    ASSERT( s1 == s3 );
    ASSERT( (const CHAR *) s1 == NULL );
    
#ifdef DOASSERT
    Trace( TR_NORMAL, "2 Invalid compares.\n" );
    s1 == s2;
    s2 == s1;
#endif

    QString s4( s2 );
    ASSERT( s2 == s4 );
    
    KillMemory( );
    QString s5( "This is string s5" );
    TRY
        {
        s5.Dump( );
        ASSERT( 0 );
        }
    CATCH_ALL
        {
        }
    END_CATCH
    
    QString s6( s1 );
    QString s7( s2 );
    TRY
        {
        s6.Dump( );
        ASSERT( 0 );
        }
    CATCH_ALL
        {
        }
    END_CATCH
    TRY
        {
        s7.Dump( );
        ASSERT( 0 );
        }
    CATCH_ALL
        {
        }
    END_CATCH
    StartMemory( );

    QString s8( "This is string s8." );
    s8.Empty( );
    ASSERT( (const CHAR *) s8 == NULL );
    s8 = "This is string s8.";
    ASSERT( lstrcmp( s8.operator const CHAR *( ), "This is string s8." ) == 0 );
    
    QString s9;
    QString s10;
    s9 = "This is a string.";
    s10 = s9;
    ASSERT( s9 == "This is a string." );
    ASSERT( s9 == s10 );
    
    QString s11( "ABCDE" );
    QString s12( "FGHIJ" );
    QString s13;
    QString s14;

    s13 = s11 + s12;
    ASSERT( s13 == "ABCDEFGHIJ" );
    s13 = s11 + "FGHIJ";
    ASSERT( s13 == "ABCDEFGHIJ" );
    s13 = "ABCDE" + s12;
    ASSERT( s13 == "ABCDEFGHIJ" );

    s13 += "KLMNO";
    ASSERT( s13 == "ABCDEFGHIJKLMNO" );

    s14 = "PQRST";
    s13 += s14;
    ASSERT( s13 == "ABCDEFGHIJKLMNOPQRST" );

    QString s15 = "ABCDE";
    QString s16 = "ABCDE";
    QString s17 = "FGHIJ";

    ASSERT( s15 == s16 );
    ASSERT( s15 == "ABCDE" );
    ASSERT( "ABCDE" == s15 );
    ASSERT( !( s15 == s17 ) );

    ASSERT( s15 != s17 );
    ASSERT( s15 != "FGHIJ" );
    ASSERT( "FGHIJ" != s15 );
    ASSERT( !( s15 != s16 ) );

    ASSERT( s15 < s17 );
    ASSERT( s15 < "FGHIJ" );
    ASSERT( !( "FGHIJ" < s15 ) );

    ASSERT( s15 <= s17 );
    ASSERT( s15 <= "FGHIJ" );
    ASSERT( s15 <= s16 );
    ASSERT( s15 <= "ABCDE" );
    ASSERT( !( "FGHIJ" <= s15 ) );

    ASSERT( s17 > s15 );
    ASSERT( s17 > "ABCDE" );
    ASSERT( !( "ABCDE" > s17 ) );

    ASSERT( s17 >= s15 );
    ASSERT( s17 >= "ABCDE" );
    ASSERT( s16 >= s15 );
    ASSERT( s17 >= "FGHIJ" );
    ASSERT( !( "ABCDE" >= s17 ) );

    QString s18;
    s18 = "12345" + s11 + s12;
    ASSERT( s18 == "12345ABCDEFGHIJ" );
    s18 = s11 + "12345" + s12;
    ASSERT( s18 == "ABCDE12345FGHIJ" );
    s18 = s11 + s12 + "12345";
    ASSERT( s18 == "ABCDEFGHIJ12345" );
    s18 = QString( "12345" ) + "67890" + QString( "12345" );
    ASSERT( s18 == "123456789012345" );

    QString s19( "12345" );
    ASSERT( s19[0] == '1' );
    ASSERT( s19[4] == '5' );
#ifdef DOASSERT
    Trace( TR_NORMAL, "2 invalid indices.\n" );
    s19[-1]; // should assert
    s19[5];  // should assert
#endif
    ASSERT( s19.GetLength( ) == 5 );

    QString s20( "This is " );
    QString s21( "a test." );
    QString s22;
    
    KillMemory( );
    TRY
        {
        s22 = s20 + s21;
        ASSERT( 0 );
        }
    CATCH( exInit )
        {
        ASSERT( 0 );
        s20.Cleanup( );
        s21.Cleanup( );
        s22.Cleanup( );
        }
    CATCH_ALL
        {
        StartMemory( );
        s20.Cleanup( );
        s21.Cleanup( );
        s22.Cleanup( );
        KillMemory( );
        }
    END_CATCH

    QString s23( "Hello World." );
    TRY
        {
        s23 = s23 + s21;
        ASSERT( 0 );
        }
    CATCH( exInit )
        {
        s20.Cleanup( );
        s21.Cleanup( );
        s22.Cleanup( );
        }
    CATCH_ALL
        {
        ASSERT( 0 );
        s20.Cleanup( );
        s21.Cleanup( );
        s22.Cleanup( );
        }
    END_CATCH
    StartMemory( );

    QString s24 = "Hello World.";
    QString s25( s24 );

    s25 += "\n";
    s25.StringFix( );
    ASSERT( s24 == s25 );
    ASSERT( s24.GetLength( ) == s25.GetLength( ) );

    QString s26( "Hello World." );
    QString s27( s26 );

    ASSERT( s26.Compare( "Hello World." ) == 0 );
    ASSERT( s26.Compare( s27 ) == 0 );
    ASSERT( s26.Compare( "Goodbye World." ) != 0 );

    ASSERT( s26.CompareNoCase( "HELLO WORLD." ) == 0 );
    ASSERT( s26.CompareNoCase( "goody world." ) != 0 );
    }

void QTestTime( )
    {
#if ( FDATA == 1 )
    QTime t1;
    Trace( TR_NORMAL, "Today's date is: %Fs.\n", (const CHAR *)(QString) t1 );
    QTime t2( t1 );
    Trace( TR_NORMAL, "Today's date is: %Fs.\n", (const CHAR *)(QString) t2 );
    QTime t3( 1995, 4, 18, 15, 30, 0 );
    Trace( TR_NORMAL, "Monique's birthday is: %Fs.\n", (const CHAR *)(QString) t3 );
    QTime t4( 1995, 10, 18, 6, 45, 15 );
    Trace( TR_NORMAL, "Oct 18, 1995 06:45:15: %Fs.\n", (const CHAR *)(QString) t4 );
#else
    QTime t1;
    Trace( TR_NORMAL, "Today's date is: %s.\n", (const CHAR *)(QString) t1 );
    QTime t2( t1 );
    Trace( TR_NORMAL, "Today's date is: %s.\n", (const CHAR *)(QString) t2 );
    QTime t3( 1995, 4, 18, 15, 30, 0 );
    Trace( TR_NORMAL, "Monique's birthday is: %s.\n", (const CHAR *)(QString) t3 );
    QTime t4( 1995, 10, 18, 6, 45, 15 );
    Trace( TR_NORMAL, "Oct 18, 1995 06:45:15: %s.\n", (const CHAR *)(QString) t4 );
#endif

    QTime tSame1;
    QTime tSame2;
    QTime tDiffer( 1996, 1, 6, 0, 0, 0 );
    ASSERT( tSame1 == tSame2 );
    ASSERT( !( tSame1 == tDiffer ) );

    ASSERT( tSame1 != tDiffer );
    ASSERT( !( tSame1 != tSame2 ) );

    ASSERT( tDiffer < tSame1 );
    ASSERT( !( tSame1 < tDiffer ) );

    ASSERT( tSame1 > tDiffer );
    ASSERT( !( tDiffer > tSame1 ) );

    ASSERT( tDiffer <= tSame1 );
    ASSERT( !( tSame1 <= tDiffer ) );

    ASSERT( tSame1 >= tDiffer );
    ASSERT( !( tDiffer >= tSame1 ) );

    ASSERT( tSame1 <= tSame2 );
    ASSERT( tSame2 >= tSame1 );

    QTime t5;
    const CHAR *psz = "%A, %B %d, %Y %c (Week %W, Day %j)";
    t5.Format( 128, psz );

#if ( FDATA == 1 )
    Trace( TR_NORMAL, "Fancy format: %Fs.\n", (const CHAR *)(QString) t5 );
#else
    Trace( TR_NORMAL, "Fancy format: %s.\n", (const CHAR *)(QString) t5 );
#endif

    QTime t6( 1995, 2, 3, 4, 5, 6 );
    ASSERT( t6.GetYear( ) == 1995 );
    ASSERT( t6.GetMonth( ) == 2 );
    ASSERT( t6.GetDay( ) == 3 );
    ASSERT( t6.GetHour( ) == 4 );
    ASSERT( t6.GetMinute( ) == 5 );
    ASSERT( t6.GetSecond( ) == 6 );

#if ( FDATA == 1 )
    Trace( TR_NORMAL, "t6 value: %Fs.\n", (const CHAR *)(QString) t6 );
#else
    Trace( TR_NORMAL, "t6 value: %s.\n", (const CHAR *)(QString) t6 );
#endif

    QTime t7;
    t7.Set( 1995, 2, 3, 4, 5, 6 );
    ASSERT( t7.GetYear( ) == 1995 );
    ASSERT( t7.GetMonth( ) == 2 );
    ASSERT( t7.GetDay( ) == 3 );
    ASSERT( t7.GetHour( ) == 4 );
    ASSERT( t7.GetMinute( ) == 5 );
    ASSERT( t7.GetSecond( ) == 6 );

#if ( FDATA == 1 )
    Trace( TR_NORMAL, "t7 value: %Fs.\n", (const CHAR *)(QString) t7 );
    t7.Format( 128, psz );
    Trace( TR_NORMAL, "t7 value: %Fs.\n", (const CHAR *)(QString) t7 );
#else
    Trace( TR_NORMAL, "t7 value: %s.\n", (const CHAR *)(QString) t7 );
    t7.Format( 128, psz );
    Trace( TR_NORMAL, "t7 value: %s.\n", (const CHAR *)(QString) t7 );
#endif

    ASSERT( t7.GetYear( ) == 1995 );
    ASSERT( t7.GetMonth( ) == 2 );
    ASSERT( t7.GetDay( ) == 3 );
    ASSERT( t7.GetHour( ) == 4 );
    ASSERT( t7.GetMinute( ) == 5 );
    ASSERT( t7.GetSecond( ) == 6 );
    }

void QTestMisc( )
    {
    QVersion v1( 1, 3, 7 );
    QVersion v2( 1, 3, 7 );
    ASSERT( v1 == v2 );
    ASSERT( v1.GetMajorNumber( ) == 1 );
    ASSERT( v1.GetMinorNumber( ) == 3 );
    ASSERT( v1.GetRevisionNumber( ) == 7 );
    ASSERT( lstrcmp( v1, "1.3.07" ) == 0 );
    }

void QTestArchive( )
    {
    QArchive ar1;
    QString  s1( "This is string s3 blowfish." );
    QString  s2;
    QTime    t1( 1995, 3, 1, 0, 0, 0 );
    QTime    t2;
    QVersion v1( 1, 3, 5 );
    QVersion v2;
    BOOL f;
    UINT ot;
    
    TRY
        {
        t1.Format( 128, "%a %p" );
        ar1.Open( "c:\\c600\\source\\budget\\archive.tst", nModeWrite );
        s1.Store( ar1 );
        t1.Store( ar1 );
        v1.Store( ar1 );
        ar1.Close( );

        ar1.Open( "c:\\c600\\source\\budget\\archive.tst", nModeRead );
        ar1.ReadHeader( &ot );
        s2.Retrieve( ar1, ot );
        ar1.ReadHeader( &ot );
        t2.Retrieve( ar1, ot );
        ar1.ReadHeader( &ot );
        v2.Retrieve( ar1, ot );

        ASSERT( s1 == s2 );
        ASSERT( t1 == t2 );
        ASSERT( v1 == v2 );
        ASSERT( lstrcmp( v1.operator const CHAR *( ), v2.operator const CHAR *( ) ) == 0 );
        }
    CATCH( exFile )
        {
        Trace( TR_NORMAL, "File exception thrown.\n" );
        }
    CATCH_ALL
        {
        Trace( TR_NORMAL, "Another exception thrown.\n" );
        }
    END_CATCH
    ar1.Close( );
    
    // prevent a warning
    f;
#if 0
    f = FileExists( "c:\\c600\\source\\budget\\normal.tst" );
    ASSERT( f );
    f = FileExists( "c:\\c600\\source\\budget\\rdonly.tst" );
    ASSERT( f );
    f = FileExists( "c:\\c600\\source\\budget\\hidden.tst" );
    ASSERT( !f );
    f = FileExists( "c:\\c600\\source\\budget\\system.tst" );
    ASSERT( !f );
    f = FileExists( "c:\\c600\\source\\budget\\arch.tst" );
    ASSERT( f );
    f = FileExists( "c:\\c600\\source\\budget\\notexist.tst" );
    ASSERT( !f );
#endif
    }

void QTest( )
    {
    QString sBegin( "Beginning tests." );
    QString sEnd( "Ending tests." );

    sBegin.Dump( );
    QTestMem( );
    QTestTrace( );
    QTestString( );
    QTestTime( );
    QTestList( );
    QTestMisc( );
    QTestArchive( );
    sEnd.Dump( );
    }
