//////////////////////////////////////////////////////////////////////////////
// DOC.CPP
//
// This file contains the document class methods
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <windowsx.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <statpak.h>
#include <ctype.h>
#include <dos.h>
#include <statpak.h>
#include "types.hxx"
#include "ot.hxx"
#include "resource.hxx"
#include "template.hxx"
#include "tools.hxx"
#include "doc.hxx"
#include "quote.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

const CHAR QDocument::m_szYr[]  = "yr";
const CHAR QDocument::m_szMn[]  = "mn";
const CHAR QDocument::m_szDy[]  = "dy";
const CHAR QDocument::m_szOp[]  = "op";
const CHAR QDocument::m_szHi[]  = "hi";
const CHAR QDocument::m_szLo[]  = "lo";
const CHAR QDocument::m_szCl[]  = "cl";
const CHAR QDocument::m_szVl[]  = "vl";
const CHAR QDocument::m_szIg[]  = "ig";
const CHAR QDocument::m_szYMD[] = "ymd";
const CHAR QDocument::m_szX[]   = "idx";
const CHAR QDocument::m_szCount[] = "count";

//////////////////////////////////////////////////////////////////////////////
// QQt::QQt( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
QQt::QQt( )
    {
    m_numOp = m_numHi = m_numLo = m_numCl = m_numVl = (NUM) 0;
    }

QQt::QQt( const QQt &qt )
    {
    m_date  = qt.m_date;
    m_numOp = qt.m_numOp;
    m_numHi = qt.m_numHi;
    m_numLo = qt.m_numLo;
    m_numCl = qt.m_numCl;
    m_numVl = qt.m_numVl;
    }

QQt::QQt( const QTime &date, NUM numOp, NUM numHi, NUM numLo, NUM numCl, NUM numVl )
    {
    m_date  = date;
    m_numOp = numOp;
    m_numHi = numHi;
    m_numLo = numLo;
    m_numCl = numCl;
    m_numVl = numVl;
    }

QQt::~QQt( )
    {
    Cleanup( );
    }

void QQt::Cleanup( )
    {
    m_date.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QQt::Store( QArchive &ar )
    {
    ar.WriteHeader( otQQt );
    m_date.Store( ar );
    ar.Write( &m_numOp, sizeof( m_numOp ) );
    ar.Write( &m_numHi, sizeof( m_numHi ) );
    ar.Write( &m_numLo, sizeof( m_numLo ) );
    ar.Write( &m_numCl, sizeof( m_numCl ) );
    ar.Write( &m_numVl, sizeof( m_numVl ) );
    }

void QQt::Retrieve( QArchive &ar, UINT ot )
    {
    USHORT cb;
    BOOL   f;

    if ( ot != otQQt )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_date.Retrieve( ar, ot );

    cb = ar.Read( &m_numOp, sizeof( m_numOp ) );
    if ( cb != sizeof( m_numOp ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    cb = ar.Read( &m_numHi, sizeof( m_numHi ) );
    if ( cb != sizeof( m_numHi ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    cb = ar.Read( &m_numLo, sizeof( m_numLo ) );
    if ( cb != sizeof( m_numLo ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    cb = ar.Read( &m_numCl, sizeof( m_numCl ) );
    if ( cb != sizeof( m_numCl ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    cb = ar.Read( &m_numVl, sizeof( m_numVl ) );
    if ( cb != sizeof( m_numVl ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::operator=( )
//
// QQt Assignment operator.
//////////////////////////////////////////////////////////////////////////////
const QQt &QQt::operator=( const QQt &qt )
    {
    if ( &qt != this )
        {
        m_date  = qt.m_date;
        m_numOp = qt.m_numOp;
        m_numHi = qt.m_numHi;
        m_numLo = qt.m_numLo;
        m_numCl = qt.m_numCl;
        m_numVl = qt.m_numVl;
        }
    return *this;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::GetDate( )
//
// Gets the quote date.
//////////////////////////////////////////////////////////////////////////////
const QTime &QQt::GetDate( ) const
    {
    return m_date;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::SetDate( )
//
// Sets the quote date.
//////////////////////////////////////////////////////////////////////////////
void QQt::SetDate( const QTime &date )
    {
    m_date = date;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::GetOp( )
//
// Gets the quote open.
//////////////////////////////////////////////////////////////////////////////
NUM QQt::GetOp( ) const
    {
    return m_numOp;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::SetOp( )
//
// Sets the quote open.
//////////////////////////////////////////////////////////////////////////////
void QQt::SetOp( NUM num )
    {
    m_numOp = num;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::GetHi( )
//
// Gets the quote high.
//////////////////////////////////////////////////////////////////////////////
NUM QQt::GetHi( ) const
    {
    return m_numHi;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::SetHi( )
//
// Sets the quote high.
//////////////////////////////////////////////////////////////////////////////
void QQt::SetHi( NUM num )
    {
    m_numHi = num;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::GetLo( )
//
// Gets the quote low.
//////////////////////////////////////////////////////////////////////////////
NUM QQt::GetLo( ) const
    {
    return m_numLo;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::SetLo( )
//
// Sets the quote low.
//////////////////////////////////////////////////////////////////////////////
void QQt::SetLo( NUM num )
    {
    m_numLo = num;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::GetCl( )
//
// Gets the quote close.
//////////////////////////////////////////////////////////////////////////////
NUM QQt::GetCl( ) const
    {
    return m_numCl;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::SetCl( )
//
// Sets the quote close.
//////////////////////////////////////////////////////////////////////////////
void QQt::SetCl( NUM num )
    {
    m_numCl = num;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::GetVl( )
//
// Gets the quote volume.
//////////////////////////////////////////////////////////////////////////////
NUM QQt::GetVl( ) const
    {
    return m_numVl;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::SetVl( )
//
// Sets the quote volume.
//////////////////////////////////////////////////////////////////////////////
void QQt::SetVl( NUM num )
    {
    m_numVl = num;
    }

//////////////////////////////////////////////////////////////////////////////
// QQt::IsValid( )
//
// This routine will return TRUE if the following condition is
// satisfied:
//
// numLo <= numCl <= numHi && numVl >= 0 and
// numLo <= numOp <= numHi
//////////////////////////////////////////////////////////////////////////////
BOOL QQt::IsValid( ) const
    {
    return ( 0 < m_numLo && m_numLo <= m_numCl && m_numCl <= m_numHi && 
             m_numLo <= m_numOp && m_numOp <= m_numHi && m_numVl >= 0 );
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QQt::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QQt::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_date.AssertValid( );
    ASSERT( m_numHi >= m_numCl );
    ASSERT( m_numCl >= m_numLo );
    ASSERT( m_numHi >= m_numOp );
    ASSERT( m_numOp >= m_numLo );
    ASSERT( m_numVl >= 0.0 );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QDocument::QDocument( )
//
// Constructors and destructors.
//////////////////////////////////////////////////////////////////////////////
const QVersion QDocument::m_v( 1, 1, 0 );
const CHAR *QDocument::szOutputFormat        = "%.3f";
const CHAR *QDocument::szOutputFormatDate    = "%d";
const CHAR *QDocument::szOutputFormatDate2   = "%04d%02d%02d";
const CHAR *QDocument::szOutputFormatVl      = "%.0f";
const CHAR *QDocument::szOutputFormatDataSet = "%8.3f";
const CHAR *QDocument::szDataSetBlank        = "        ";

QDocument::QDocument( )
    {
    m_fModified = FALSE;
    m_fReadOnly = FALSE;
    }

QDocument::~QDocument( )
    {
    Cleanup( );
    }

void QDocument::Cleanup( )
    {
    m_sFileName.Cleanup( );
    m_list.Cleanup( );
    gcache.Cleanup( );
    m_fModified = FALSE;
    m_fReadOnly = FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QDocument::Store( QArchive &ar )
    {
    QVersion v( m_v.GetMajorNumber( ),
                m_v.GetMinorNumber( ),
                m_v.GetRevisionNumber( ) );
    ar.WriteHeader( otQDocument );
    v.Store( ar );
    m_list.Store( ar );
    }

void QDocument::Retrieve( QArchive &ar, UINT ot )
    {
    QVersion v;
    BOOL   f;

    if ( ot != otQDocument )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    v.Retrieve( ar, ot );
    if ( !( v == m_v ) )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }

    f = ar.ReadHeader( &ot );
    if ( !f )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    m_list.Retrieve( ar, ot );
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::GetModifiedFlag( )
//
// Returns TRUE if the document has been modified.
//////////////////////////////////////////////////////////////////////////////
BOOL QDocument::GetModifiedFlag( ) const
    {
    return m_fModified;
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::SetModifiedFlag( )
//
// Marks the document as modified/not modified.
//////////////////////////////////////////////////////////////////////////////
void QDocument::SetModifiedFlag( BOOL fModified )
    {
    m_fModified = fModified;
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::GetFileName( )
//
// Returns the document file name.
//////////////////////////////////////////////////////////////////////////////
const CHAR *QDocument::GetFileName( ) const
    {
    return m_sFileName;
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::SetFileName( )
//
// Sets the document file name.
//////////////////////////////////////////////////////////////////////////////
void QDocument::SetFileName( const CHAR *pszFileName )
    {
    m_sFileName = pszFileName;
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::BuildQuoteData( )
//
// Builds a series of DATASETS based on the internal document data of
// the highs, lows, closes, and volumes.
//////////////////////////////////////////////////////////////////////////////
void QDocument::BuildQuoteData( LPDATASET lpdatasetOp,
                                LPDATASET lpdatasetHi,
                                LPDATASET lpdatasetLo,
                                LPDATASET lpdatasetCl,
                                LPDATASET lpdatasetVl, 
                                LPDATASET lpdatasetX,
                                LPDATASET lpdatasetCount )
    {
    BOOL fNoMemory = FALSE;
    QQt  *pqt;
    WORD nPoints;
    WORD n;

    ASSERT( lpdatasetOp    != NULL );
    ASSERT( lpdatasetHi    != NULL );
    ASSERT( lpdatasetLo    != NULL );
    ASSERT( lpdatasetCl    != NULL );
    ASSERT( lpdatasetVl    != NULL );
    ASSERT( lpdatasetX     != NULL );
    ASSERT( lpdatasetCount != NULL );

    DataSetZero( lpdatasetOp );
    DataSetZero( lpdatasetHi );
    DataSetZero( lpdatasetLo );
    DataSetZero( lpdatasetCl );
    DataSetZero( lpdatasetVl );
    DataSetZero( lpdatasetX );
    DataSetZero( lpdatasetCount );

    TASKLIST
        {
        nPoints = m_list.GetCount( );
        if ( nPoints == 0 )
            {
            QUIT;
            }

        if ( !DataSetAlloc( lpdatasetOp, nPoints, 0 ) ) 
            {
            fNoMemory = TRUE;
            QUIT;
            }

        if ( !DataSetAlloc( lpdatasetHi, nPoints, 0 ) )
            {
            fNoMemory = TRUE;
            QUIT;
            }

        if ( !DataSetAlloc( lpdatasetLo, nPoints, 0 ) )
            {
            fNoMemory = TRUE;
            QUIT;
            }

        if ( !DataSetAlloc( lpdatasetCl, nPoints, 0 ) )
            {
            fNoMemory = TRUE;
            QUIT;
            }

        if ( !DataSetAlloc( lpdatasetVl, nPoints, 0 ) )
            {
            fNoMemory = TRUE;
            QUIT;
            }
        
        if ( !DataSetAlloc( lpdatasetX, nPoints, 0 ) )
            {
            fNoMemory = TRUE;
            QUIT;
            }
        
        if ( !DataSetAlloc( lpdatasetCount, nPoints, 0 ) )
            {
            fNoMemory = TRUE;
            QUIT;
            }

        for( n = 0, pqt = (QQt *) m_list.GetHead( );
             pqt;
             n++, pqt = (QQt *) m_list.GetNext( pqt ) )
            {
            lpdatasetOp->lpnum[ n ] = pqt->GetOp( );
            lpdatasetHi->lpnum[ n ] = pqt->GetHi( );
            lpdatasetLo->lpnum[ n ] = pqt->GetLo( );
            lpdatasetCl->lpnum[ n ] = pqt->GetCl( );
            lpdatasetVl->lpnum[ n ] = pqt->GetVl( );

            lpdatasetX->lpnum[ n ]     = (NUM) ( n + 1 );
            lpdatasetCount->lpnum[ n ] = (NUM) m_list.GetCount( );
            }
        }
    ENDTASKLIST

    if ( fNoMemory )
        {
        DataSetFree( lpdatasetOp );
        DataSetFree( lpdatasetHi );
        DataSetFree( lpdatasetLo );
        DataSetFree( lpdatasetCl );
        DataSetFree( lpdatasetVl );
        DataSetFree( lpdatasetX );
        DataSetFree( lpdatasetCount );
        THROW_EXCEPTION( exMemory );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::GetFirst( )
//
// Gets the first quote.
//////////////////////////////////////////////////////////////////////////////
QQt *QDocument::GetFirst( )
    {
    return (QQt *) m_list.GetHead( );
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::GetLast( )
//
// Gets the last quote.
//////////////////////////////////////////////////////////////////////////////
QQt *QDocument::GetLast( )
    {
    return (QQt *) m_list.GetTail( );
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::GetNext( )
//
// Gets the next quote.
//////////////////////////////////////////////////////////////////////////////
QQt *QDocument::GetNext( const QQt *pqqt )
    {
    return (QQt *) m_list.GetNext( pqqt );
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::GetPrev( )
//
// Gets the previous quote.
//////////////////////////////////////////////////////////////////////////////
QQt *QDocument::GetPrev( const QQt *pqqt )
    {
    return (QQt *) m_list.GetPrev( pqqt );
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::GetCount( )
//
// Gets the number of quotes present in the document.
//////////////////////////////////////////////////////////////////////////////
UINT QDocument::GetCount( ) const
    {
    return m_list.GetCount( );
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::Add( )
//
// Adds the given quote to the document.  If fSearch is TRUE, it will
// insert the quote; otherwise, it will add the quote at the end of
// the document.
//////////////////////////////////////////////////////////////////////////////
void QDocument::Add( const QQt &qt, BOOL fSearch )
    {
    BOOL fReplace = FALSE;
    QQt *pqtSearch;
    QQt *pqt = NULL;
    BOOL fAdded = FALSE;
    
    TRY
        {
        pqt = new QQt( qt );
        if ( !pqt )
            {
            THROW_EXCEPTION( exMemory );
            }
        if ( fSearch )
            {
            for( pqtSearch = (QQt *) m_list.GetHead( );
                 pqtSearch;
                 pqtSearch = (QQt *) m_list.GetNext( pqtSearch ) )
                {
                if ( pqtSearch->GetDate( ) == pqt->GetDate( ) )
                    {
                    fReplace = TRUE;
                    break;
                    }
                else if ( pqtSearch->GetDate( ) > pqt->GetDate( ) )
                    {
                    break;
                    }
                // else, keep searching
                }
            if ( pqtSearch )
                {
                if ( fReplace )
                    {
                    *pqtSearch = qt;
                    }
                else
                    {
                    m_list.InsertBefore( pqtSearch, pqt );
                    fAdded = TRUE;
                    }
                }
            else
                {
                m_list.AddTail( pqt );
                fAdded = TRUE;
                }
            }
        else
            {
            m_list.AddTail( pqt );
            fAdded = TRUE;
            }
        gcache.Cleanup( );
        }
    CATCH_ALL
        {
        if ( !fAdded )
            {
            delete pqt;
            }
        gcache.Cleanup( ); // just to be sure
        THROW_LAST( );
        }
    END_CATCH
    if ( !fAdded )
        {
        // we must have done a replacement: delete pqt
        delete pqt;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::Remove( )
//
// Removes the quote indexed by date from the document.
//////////////////////////////////////////////////////////////////////////////
BOOL QDocument::Remove( const QTime &date )
    {
    BOOL fRet = FALSE;
    QQt *pqtSearch;

    TASKLIST
        {
        for ( pqtSearch = (QQt *) m_list.GetHead( );
              pqtSearch;
              pqtSearch = (QQt *) m_list.GetNext( pqtSearch ) )
            {
            if ( pqtSearch->GetDate( ) == date )
                {
                break;
                }
            }
        if ( !pqtSearch )
            {
            QUIT;
            }
        if ( !m_list.Remove( pqtSearch ) )
            {
            QUIT;
            }
        delete pqtSearch;
        gcache.Cleanup( );
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::StoreText( )
//
// Stores the document in text format.
//////////////////////////////////////////////////////////////////////////////
void QDocument::StoreText( FILE *pfile )
    {
    QQt       *pqt;
    USHORT     i;
    CHAR       szBuffer[ _cbTextMax + 1 ];
    QString    s;
    
    TRY
        {
        for ( pqt = (QQt *) m_list.GetHead( ), i = 0;
              pqt;
              pqt = (QQt *) m_list.GetNext( pqt ), i++ )
            {
            lmemset( szBuffer, 0, sizeof( szBuffer ) );
            BuildOutputString( szBuffer, sizeof( szBuffer ), pqt, i );
            s = szBuffer;
            s += "\n";
            if ( fputs( s, pfile ) < 0 )
                {
                THROW_EXCEPTION( exFile );
                }
            }
        }
    CATCH_ALL
        {
        s.Cleanup( );
        gcache.Cleanup( ); // just in case
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::BuildOutputString( )
//
// Helper function used to store text.  Based on the save string format,
// this routine will build an output string suitable for output
// to the text file.
//
// pszBuffer - the output buffer
// cb        - the max. length of the output buffer
//////////////////////////////////////////////////////////////////////////////
void QDocument::BuildOutputString( CHAR *pszBuffer, USHORT cb, QQt *pqt, USHORT nIndex )
    {
    const CHAR *pchFormat;
    State state = stateChar;
    CHAR *pchOut = pszBuffer;
    CHAR  szT[ _cbTextMax + 1 ];
    NUM   num = 0.0;
    QIndicatorTemplate *pi;         
    int   cbLongestSoFar;
    int   n;
    CHAR *pchT;
    const QCache *pcache = NULL;
    
    TRY
        {
        ASSERT( cb > 0 );
        cb--; // for the null-terminator
        pchFormat = _ptpl->m_format.GetSaveString( );
        
        while( *pchFormat )
            {
            switch( state )
                {
            case stateChar:
                switch( *pchFormat )
                    {
                case '$':
                    if ( cb >= 1 )
                        {
                        pchFormat++;
                        state = stateNativeValue;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;

                case '#':
                    if ( cb >= 1 )
                        {
                        pchFormat++;
                        state = stateIndicatorValue;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
    
                case '\\':
                    if ( cb >= 1 )
                        {
                        pchFormat++;
                        state = stateEscape;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;

                default:
                    if ( cb >= 1 )
                        {
                        *pchOut = *pchFormat;
                        pchOut++;
                        cb--;
                        pchFormat++;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }
                break;

            case stateIndicatorValue:
                pcache = NULL;
                cbLongestSoFar = -1;
                for ( pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetHead( ); 
                      pi; 
                      pi = (QIndicatorTemplate *) _ptpl->m_itlist.GetNext( pi ) )
                    {
                    ASSERT( pi->GetName( ) != NULL );
                    UINT cb2 = lstrlen( pi->GetName( ) );
                    
                    // We will always grab the *longest* indicator found
                    if ( memcmp( pi->GetName( ), pchFormat, cb2 ) == 0 && ( (int) cb2 ) > cbLongestSoFar )
                        {
                        // even if we can't build the indicator, he still counts as the longest
                        // indicator found so far.   
                        cbLongestSoFar = cb2;
                        pcache = gcache.pcache( pi->GetName( ), 0 );
                        } // if
                    } // for
                
                // Invariant Assertion: pcache points to the largest indicator found so far.
                if ( pcache )
                    {
                    ASSERT( cbLongestSoFar > 0 );
                    WORD nOffset = pcache->m_dataset.nOffset;
                    if ( nIndex < nOffset )
                        {
                        lstrcpy( szT, szDataSetBlank );
                        if ( cb >= (USHORT) lstrlen( szT ) )
                            {
                            lstrcpy( pchOut, szDataSetBlank );
                            pchFormat += lstrlen( pcache->GetName( ) );
                            pchOut += (USHORT) lstrlen( szT );
                            cb -= (USHORT) lstrlen( szT );
                            state = stateChar;
                            }
                        else
                            {
                            THROW_EXCEPTION( exFileCorrupt );
                            }
                        }
                    else
                        {
                        sprintf( szT, szOutputFormatDataSet, pcache->m_dataset.lpnum[ nIndex - nOffset ] );
                        if ( cb >= (USHORT) lstrlen( szT ) )
                            {
                            sprintf( pchOut, szOutputFormatDataSet, pcache->m_dataset.lpnum[ nIndex - nOffset ] );
                            pchFormat += lstrlen( pcache->GetName( ) );
                            pchOut += (USHORT) lstrlen( szT );
                            cb -= (USHORT) lstrlen( szT );
                            state = stateChar;
                            }
                        else
                            {
                            THROW_EXCEPTION( exFileCorrupt );
                            }
                        }
                    break;
                    }
                
                // Didn't find it anywhere, so ...
                THROW_EXCEPTION( exFileCorrupt );
                break;

            case stateNativeValue:
                if ( memcmp( pchFormat, m_szOp, 2 ) == 0 )
                    {
                    sprintf( szT, szOutputFormat, pqt->GetOp( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormat, pqt->GetOp( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }
                
                if ( memcmp( pchFormat, m_szHi, 2 ) == 0 )
                    {
                    sprintf( szT, szOutputFormat, pqt->GetHi( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormat, pqt->GetHi( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }

                if ( memcmp( pchFormat, m_szLo, 2 ) == 0 ) 
                    {
                    sprintf( szT, szOutputFormat, pqt->GetLo( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormat, pqt->GetLo( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }

                if ( memcmp( pchFormat, m_szCl, 2 ) == 0 )
                    {
                    sprintf( szT, szOutputFormat, pqt->GetCl( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormat, pqt->GetCl( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }

                if ( memcmp( pchFormat, m_szVl, 2 ) == 0 )
                    {
                    sprintf( szT, szOutputFormatVl, pqt->GetVl( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormatVl, pqt->GetVl( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }

                if ( memcmp( pchFormat, m_szYMD, 3 ) == 0 )
                    {
                    sprintf( szT, szOutputFormatDate2, 
                                  pqt->GetDate( ).GetYear( ), 
                                  pqt->GetDate( ).GetMonth( ),
                                  pqt->GetDate( ).GetDay( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormatDate2, 
                                         pqt->GetDate( ).GetYear( ), 
                                         pqt->GetDate( ).GetMonth( ),
                                         pqt->GetDate( ).GetDay( ) );
                        pchFormat += 3;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }

                if ( memcmp( pchFormat, m_szYr, 2 ) == 0 )
                    {
                    sprintf( szT, szOutputFormatDate, pqt->GetDate( ).GetYear( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormatDate, pqt->GetDate( ).GetYear( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }

                if ( memcmp( pchFormat, m_szMn, 2 ) == 0 )
                    {
                    sprintf( szT, szOutputFormatDate, pqt->GetDate( ).GetMonth( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormatDate, pqt->GetDate( ).GetMonth( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }
                
                if ( memcmp( pchFormat, m_szDy, 2 ) == 0 )
                    {
                    sprintf( szT, szOutputFormatDate, pqt->GetDate( ).GetDay( ) );
                    if ( cb >= (USHORT) lstrlen( szT ) )
                        {
                        sprintf( pchOut, szOutputFormatDate, pqt->GetDate( ).GetDay( ) );
                        pchFormat += 2;
                        pchOut += (USHORT) lstrlen( szT );
                        cb -= (USHORT) lstrlen( szT );
                        state = stateChar;
                        }
                    else
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        }
                    break;
                    }
                
                // Didn't find it anywhere, so ...
                THROW_EXCEPTION( exFileCorrupt );
                break; // will never get hit

            case stateEscape:
                if ( isdigit( *pchFormat ) )
                    {
                    lmemset( szT, 0, sizeof( szT ) );
                    for ( pchT = szT; 
                          isdigit( *pchFormat ) &&
                          szT + sizeof( szT ) > pchT;
                          pchT++, pchFormat++ )
                        {
                        *pchT = *pchFormat;
                        }
                    n = atoi( szT );
                    if ( n < 0 || n > 255 )
                        {
                        THROW_EXCEPTION( exFileCorrupt );
                        break;
                        }
                    if ( cb >= 1 )
                        {
                        *pchOut = (CHAR) n;
                        pchOut++;
                        cb--;
                        state = stateChar;
                        }
                    } // if
                else
                    {
                    switch( *pchFormat )
                        {
                    case 't':
                        if ( cb >= 1 )
                            {
                            *pchOut = '\t';
                            pchOut++;
                            pchFormat++;
                            cb--;
                            state = stateChar;
                            }
                        else
                            {   
                            THROW_EXCEPTION( exFileCorrupt );
                            }
                        break;

                    case 'b':
                        if ( cb >= 1 )
                            {
                            *pchOut = ' ';
                            pchOut++;
                            pchFormat++;
                            cb--;
                            state = stateChar;
                            }
                        else
                            {
                            THROW_EXCEPTION( exFileCorrupt );
                            }
                        break;

                    default:
                        if ( cb >= 1 )
                            {
                            *pchOut = *pchFormat;
                            pchOut++;
                            pchFormat++;
                            cb--;
                            state = stateChar;
                            }
                        else
                            {
                            THROW_EXCEPTION( exFileCorrupt );
                            }
                        break;
                        } // switch
                    } // else
                break;

            default:
                ASSERT( FALSE );
                break;
                } // switch 
            } // while 
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    if ( state != stateChar )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::RetrieveText( )
//
// Retrieves a document in text format.
//////////////////////////////////////////////////////////////////////////////
void QDocument::RetrieveText( FILE *pfile, BOOL fSilent )
    {
    BOOL  fMsgBox;
    BOOL  fEOF;
    BOOL  fFirst;
    CHAR  szBuffer[ _cbTextMax + 1 ];
    CHAR  szMsg[ _cbBufferMax + 1 ];
    QTime date;
    QTime datePrev;
    NUM   numOp;
    NUM   numHi;
    NUM   numLo;
    NUM   numCl;
    NUM   numVl;
    QQt   qt;
    long  lLine;
    
    TRY
        {
        fMsgBox = FALSE;
        fFirst = TRUE;
        lLine = 1L;
        while ( TRUE )
            {
            ReadText( pfile, szBuffer, sizeof( szBuffer ), &fEOF );
            if ( fEOF )
                {
                break;
                }

            ParseInputString( szBuffer, &date, &numOp, &numHi, &numLo, &numCl, &numVl );
            if ( !fFirst && datePrev >= date )
                {
                fMsgBox = TRUE;
                wsprintf( szMsg, GetString( IDS_TEXT_FILE_CORRUPT_AT ), lLine );
                if ( !fSilent )
                    MessageBox( _hwndFrame, szMsg, GetString( IDS_APPNAME ), MB_ICONEXCLAMATION );
                THROW_EXCEPTION( exFileCorrupt );
                }
            qt.SetDate( date );
            qt.SetOp( numOp );
            qt.SetHi( numHi );
            qt.SetLo( numLo );
            qt.SetCl( numCl );
            qt.SetVl( numVl );
            if ( !qt.IsValid( ) )
                {
                fMsgBox = TRUE;
                wsprintf( szMsg, GetString( IDS_TEXT_FILE_CORRUPT_AT ), lLine );
                if ( !fSilent )
                    MessageBox( _hwndFrame, szMsg, GetString( IDS_APPNAME ), MB_ICONEXCLAMATION );
                THROW_EXCEPTION( exFileCorrupt );
                }
            Add( qt, FALSE );
            datePrev = date;
            fFirst = FALSE;
            lLine++;
            }
        }
    CATCH_ALL
        {
        date.Cleanup( );
        datePrev.Cleanup( );
        qt.Cleanup( );
        if ( !fMsgBox )
            {
            wsprintf( szMsg, GetString( IDS_TEXT_FILE_CORRUPT_AT ), lLine );
            if ( !fSilent )
                MessageBox( _hwndFrame, szMsg, GetString( IDS_APPNAME ), MB_ICONEXCLAMATION );
            }
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::ConvertDelimiters( )
//
// This routine will parse any delimiters present in an input string,
// and build an output string of 'cooked' delimiters.
//////////////////////////////////////////////////////////////////////////////
void QDocument::ConvertDelimiters( CHAR *pszDelimitersDst, const CHAR *pszDelimitersSrc )
    {
    const CHAR *pchSrc = pszDelimitersSrc;
    CHAR *pchDst = pszDelimitersDst;
    CHAR  szT[ _cbTextMax + 1 ];
    CHAR *pchT;
    int   n;

    while ( *pchSrc )
        {
        if ( *pchSrc == '\\' )
            {
            pchSrc++;
            if ( isdigit( *pchSrc ) )
                {
                lmemset( szT, 0, sizeof( szT ) );
                for ( pchT = szT; 
                      isdigit( *pchSrc ) &&
                      szT + sizeof( szT ) > pchT;
                      pchT++, pchSrc++ )
                    {
                    *pchT = *pchSrc;
                    }
                n = atoi( szT );
                if ( n < 0 || n > 255 )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                *pchDst = (CHAR) n;
                pchDst++;
                }
            else if ( *pchSrc == 't' )
                {
                *pchDst = '\t';
                pchSrc++;
                pchDst++;
                }
            else if ( *pchSrc == 'b' )
                {
                *pchDst = ' ';
                pchSrc++;
                pchDst++;
                }
            else if ( *pchSrc == '\\' )
                {
                *pchDst = '\\';
                pchSrc++;
                pchDst++;
                }
            else
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            }
        else
            {
            *pchDst = *pchSrc;
            pchSrc++;
            pchDst++;
            }
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QDocument::ParseInputString( )
//
// Based upon the internal set of delimiters and input string format,
// this routine will parse an input string and return the date, high, low,
// close, and volume values.
//////////////////////////////////////////////////////////////////////////////
void QDocument::ParseInputString( CHAR *pszBuffer, QTime *pdate, NUM *pnumOp, 
    NUM *pnumHi, NUM *pnumLo, NUM *pnumCl, NUM *pnumVl )
    {
    CHAR *pch;

    int  year;
    int  month;
    int  day;

    BOOL fYr    = FALSE;
    BOOL fMn    = FALSE;
    BOOL fDy    = FALSE;
    BOOL fOp    = FALSE;
    BOOL fHi    = FALSE;
    BOOL fLo    = FALSE;
    BOOL fCl    = FALSE;
    BOOL fVl    = FALSE;
    enum Type { tYr, tMn, tDy, tYMD, tOp, tHi, tLo, tCl, tVl, tIg };

    USHORT iMap;
    USHORT iMapMax;
    USHORT cb;

    CHAR *pszDelimiters  = "\t ";
    WORD *rgnMap         = NULL;
    CHAR *pszLoadString  = NULL;
    CHAR *pszDelimiters2 = NULL;
    
    TRY
        {
        pszLoadString = new CHAR[ lstrlen( _ptpl->m_format.GetLoadString( ) ) + 1 ];
        if ( !pszLoadString )
            {
            THROW_EXCEPTION( exMemory );
            }
    
        lstrcpy( pszLoadString, _ptpl->m_format.GetLoadString( ) );
        iMapMax = 0;
        pch = GetChar( pszLoadString, pszDelimiters );
        while ( pch )
            {
            // first check to ensure we have the $ character
            if ( *pch != '$' )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            // skip over the $ character
            pch++;
            if ( memcmp( pch, m_szYr,  2 ) == 0 ||
                 memcmp( pch, m_szMn,  2 ) == 0 ||
                 memcmp( pch, m_szDy,  2 ) == 0 ||
                 memcmp( pch, m_szOp,  2 ) == 0 ||
                 memcmp( pch, m_szHi,  2 ) == 0 ||
                 memcmp( pch, m_szLo,  2 ) == 0 ||
                 memcmp( pch, m_szCl,  2 ) == 0 ||
                 memcmp( pch, m_szVl,  2 ) == 0 ||
                 memcmp( pch, m_szIg,  2 ) == 0 )
                { 
                iMapMax++;
                pch += 2;
                }
            else if ( memcmp( pch, m_szYMD, 3 ) == 0 )
                {
                iMapMax++;
                pch += 3;
                }
            else
                {
                THROW_EXCEPTION( exFileCorrupt );
                }
            pch = GetChar( pch, pszDelimiters );
            }

        ASSERT( iMapMax > 0 );
        rgnMap = new WORD[ iMapMax ];
        if ( !rgnMap )
            {
            THROW_EXCEPTION( exMemory );
            }

        iMap = 0;
        pch = GetChar( pszLoadString, pszDelimiters );
        while ( pch )
            {
            ASSERT( iMap < iMapMax );
            // first check to ensure we have the $ character
            if ( *pch != '$' )
                {
                THROW_EXCEPTION( exFileCorrupt );
                }

            // skip over the $ character
            pch++;
            if ( memcmp( pch, m_szYr, 2 ) == 0 )
                {
                if ( fYr )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tYr;
                iMap++;
                pch += 2;
                fYr = TRUE;
                }
            else if ( memcmp( pch, m_szMn, 2 ) == 0 )
                {
                if ( fMn )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tMn;
                iMap++;
                pch += 2;
                fMn = TRUE;
                }
            else if ( memcmp( pch, m_szDy, 2 ) == 0 )
                {
                if ( fDy )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tDy;
                iMap++;
                pch += 2;
                fDy = TRUE;
                }
            else if ( memcmp( pch, m_szYMD, 3 ) == 0 )
                {
                if ( fYr || fMn || fDy )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tYMD;
                iMap++;
                pch += 3;
                fYr = fMn = fDy = TRUE;
                }
            else if ( memcmp( pch, m_szOp, 2 ) == 0 )
                {
                if ( fOp )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tOp;
                iMap++;
                pch += 2;
                fOp = TRUE;
                }
            else if ( memcmp( pch, m_szHi, 2 ) == 0 )
                {
                if ( fHi )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tHi;
                iMap++;
                pch += 2;
                fHi = TRUE;
                }
            else if ( memcmp( pch, m_szLo, 2 ) == 0 )
                {
                if ( fLo )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tLo;
                iMap++;
                pch += 2;
                fLo = TRUE;
                }
            else if ( memcmp( pch, m_szCl, 2 ) == 0 )
                {
                if ( fCl )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tCl;
                iMap++;
                pch += 2;
                fCl = TRUE;
                }
            else if ( memcmp( pch, m_szVl, 2 ) == 0 )
                {
                if ( fVl )
                    {
                    THROW_EXCEPTION( exFileCorrupt );
                    }
                rgnMap[ iMap ] = tVl;
                iMap++;
                pch += 2;
                fVl = TRUE;
                }
            else if ( memcmp( pch, m_szIg, 2 ) == 0 )
                {
                rgnMap[ iMap ] = tIg;
                iMap++;
                pch += 2;
                }
            else
                {
                THROW_EXCEPTION( exFileCorrupt );
                }

            pch = GetChar( pch, pszDelimiters );
            }

        if ( !fYr || !fMn || !fDy || !fOp || !fHi || !fLo || !fCl || !fVl )
            {
            THROW_EXCEPTION( exFileCorrupt );
            }

        iMap = 0;

        cb = (USHORT) lstrlen( _ptpl->m_format.GetLoadDelimiters( ) ) + 1;
        pszDelimiters2 = new CHAR[ cb ];
        if ( !pszDelimiters2 )
            {
            THROW_EXCEPTION( exMemory );
            }
        lmemset( pszDelimiters2, 0, cb );
        ConvertDelimiters( pszDelimiters2, _ptpl->m_format.GetLoadDelimiters( ) );

        pch = strtok( pszBuffer, pszDelimiters2 );
        // only process what delimiters are present (not necessarily
        // whole szBuffer )
        while ( pch && iMap < iMapMax )
            {
            if ( rgnMap[ iMap ] == tYr )
                {
                year = atoi( pch );
                year = AdjustCentury( year );
                }
            else if ( rgnMap[ iMap ] == tMn )    
                {
                month = atoi( pch );
                }
            else if ( rgnMap[ iMap ] == tDy )    
                {
                day = atoi( pch );
                }
            else if ( rgnMap[ iMap ] == tYMD )
                {
                CHAR szFourDigits[ 5 ];
                CHAR szTwoDigits [ 3 ];

                if ( lstrlen( pch ) == 6 )
                    {
                    szTwoDigits[ 0 ] = *( pch + 0 );
                    szTwoDigits[ 1 ] = *( pch + 1 );
                    szTwoDigits[ 2 ] = '\0';
                    year = AdjustCentury( atoi( szTwoDigits ) );

                    szTwoDigits[ 0 ] = *( pch + 2 );
                    szTwoDigits[ 1 ] = *( pch + 3 );
                    szTwoDigits[ 2 ] = '\0';
                    month = atoi( szTwoDigits );

                    szTwoDigits[ 0 ] = *( pch + 4 );
                    szTwoDigits[ 1 ] = *( pch + 5 );
                    szTwoDigits[ 2 ] = '\0';
                    day = atoi( szTwoDigits );
                    }
                else if ( lstrlen( pch ) == 8 )
                    {
                    szFourDigits[ 0 ] = *( pch + 0 );
                    szFourDigits[ 1 ] = *( pch + 1 );
                    szFourDigits[ 2 ] = *( pch + 2 );
                    szFourDigits[ 3 ] = *( pch + 3 );
                    szFourDigits[ 4 ] = '\0';
                    year = AdjustCentury( atoi( szFourDigits ) );

                    szTwoDigits[ 0 ] = *( pch + 4 );
                    szTwoDigits[ 1 ] = *( pch + 5 );
                    szTwoDigits[ 2 ] = '\0';
                    month = atoi( szTwoDigits );

                    szTwoDigits[ 0 ] = *( pch + 6 );
                    szTwoDigits[ 1 ] = *( pch + 7 );
                    szTwoDigits[ 2 ] = '\0';
                    day = atoi( szTwoDigits );
                    }
                else
                    THROW_EXCEPTION( exFileCorrupt );
                
                }
            else if ( rgnMap[ iMap ] == tOp )    
                {
                *pnumOp = (NUM) atof( pch );
                }
            else if ( rgnMap[ iMap ] == tHi )    
                {
                *pnumHi = (NUM) atof( pch );
                }
            else if ( rgnMap[ iMap ] == tLo )    
                {
                *pnumLo = (NUM) atof( pch );
                }
            else if ( rgnMap[ iMap ] == tCl )    
                {
                *pnumCl = (NUM) atof( pch );
                }
            else if ( rgnMap[ iMap ] == tVl )    
                {
                *pnumVl = (NUM) atof( pch );
                }
            else if ( rgnMap[ iMap ] == tIg )    
                {
                // don't do anything!!
                }
            else
                {
                ASSERT( FALSE );
                THROW_EXCEPTION( exFileCorrupt );
                }
            iMap++;
            pch = strtok( NULL, pszDelimiters2 );
            }
            
        if ( iMap < iMapMax )
            {
            // Our load string specifies more data, but
            // we ran out of data on the input string.
            // This is an error.
            THROW_EXCEPTION( exFileCorrupt );
            }
        pdate->Set( year, month, day, 12, 0, 0 );
        }
    CATCH_ALL
        {
        delete [] pszLoadString;
        delete [] rgnMap;
        delete [] pszDelimiters2;
        THROW_LAST( );
        }
    END_CATCH

    delete [] pszLoadString;
    delete [] rgnMap;
    delete [] pszDelimiters2;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QDocument::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QDocument::AssertValid( ) const
    {
    QObject::AssertValid( );
    m_v.AssertValid( );
    m_sFileName.AssertValid( );
    m_list.AssertValid( );
    }
#endif

////////////////////////////////////////
// QQtList class
////////////////////////////////////////
QQtList::QQtList( )
    {
    }

QQtList::~QQtList( )
    {
    Cleanup( );
    }

void QQtList::Cleanup( )
    {
    QQt *pqt;

    while ( pqt = (QQt *) RemoveHead( ) )
        {
        delete pqt;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QQtList::Store( )/Retrieve( )
//
// Standard persistant storage routines.
//////////////////////////////////////////////////////////////////////////////
void QQtList::Store( QArchive &ar )
    {
    ar.WriteHeader( otQQtList );
    for ( QQt *pqt = (QQt *) GetHead( );
          pqt;
          pqt = (QQt *) GetNext( pqt ) )
        {
        pqt->Store( ar );
        }
    ar.WriteHeader( otEndMarker );
    }

void QQtList::Retrieve( QArchive &ar, UINT ot )
    {
    QQt *pqt;

    if ( ot != otQQtList )
        {
        THROW_EXCEPTION( exFileCorrupt );
        }
    while ( TRUE )
        {
        ar.ReadHeader( &ot );
        if ( ot == otEndMarker )
            {
            break;
            }
        pqt = new QQt;
        if ( !pqt )
            {
            THROW_EXCEPTION( exMemory );
            }
        pqt->Retrieve( ar, ot );
        AddTail( pqt );
        }
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QQtList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QQtList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QQt *pqt = (QQt *) GetHead( ); 
          pqt; 
          pqt = (QQt *) GetNext( pqt ) )
        {
        pqt->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QGCache::QGCache( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QGCache::QGCache( )
    : m_cachelist( ), m_rulecachelist( ), m_condcachelist( )
    {               
    DataSetZero( &m_datasetOp );
    DataSetZero( &m_datasetHi );
    DataSetZero( &m_datasetLo );
    DataSetZero( &m_datasetCl );
    DataSetZero( &m_datasetVl );
    DataSetZero( &m_datasetX );
    DataSetZero( &m_datasetCount );
    m_fDataSetsDirty = TRUE;
    }

QGCache::~QGCache( )
    {
    Cleanup( );
    }

void QGCache::Cleanup( )
    {
    m_cachelist.Cleanup( );
    m_rulecachelist.Cleanup( );
    m_condcachelist.Cleanup( );
    DataSetFree( &m_datasetOp );
    DataSetFree( &m_datasetHi );
    DataSetFree( &m_datasetLo );
    DataSetFree( &m_datasetCl );
    DataSetFree( &m_datasetVl );
    DataSetFree( &m_datasetX );
    DataSetFree( &m_datasetCount );
    m_fDataSetsDirty = TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// HandleDirtyDataSets( )
//
// Handles building any dirty data sets.
//////////////////////////////////////////////////////////////////////////////
void QGCache::HandleDirtyDataSets( )
    {
    if ( m_fDataSetsDirty )
        {
        _pdoc->BuildQuoteData( &m_datasetOp,
                               &m_datasetHi, 
                               &m_datasetLo, 
                               &m_datasetCl, 
                               &m_datasetVl, 
                               &m_datasetX,
                               &m_datasetCount );
        m_fDataSetsDirty = FALSE;
        }
    }
    
//////////////////////////////////////////////////////////////////////////////
// QGCache::op
//
// Gets the op dataset.
//////////////////////////////////////////////////////////////////////////////
const DATASET &QGCache::op( )
    {
    HandleDirtyDataSets( );
    return m_datasetOp;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::hi( )
//
// Get the hi dataset.    
//////////////////////////////////////////////////////////////////////////////
const DATASET &QGCache::hi( )
    {
    HandleDirtyDataSets( );
    return m_datasetHi;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::lo( )
//
// Get the lo dataset.    
//////////////////////////////////////////////////////////////////////////////
const DATASET &QGCache::lo( )
    {
    HandleDirtyDataSets( );
    return m_datasetLo;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::cl( )
//
// Get the cl dataset.    
//////////////////////////////////////////////////////////////////////////////
const DATASET &QGCache::cl( )
    {
    HandleDirtyDataSets( );
    return m_datasetCl;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::vl( )
//
// Get the vl dataset.    
//////////////////////////////////////////////////////////////////////////////
const DATASET &QGCache::vl( )
    {
    HandleDirtyDataSets( );
    return m_datasetVl;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::x( )
//
// Get the x dataset.    
//////////////////////////////////////////////////////////////////////////////
const DATASET &QGCache::x( )
    {
    HandleDirtyDataSets( );
    return m_datasetX;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::count( )
//
// Get the count dataset.    
//////////////////////////////////////////////////////////////////////////////
const DATASET &QGCache::count( )
    {
    HandleDirtyDataSets( );
    return m_datasetCount;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::pcondcache( )
//
// Get the cache associated with a condition.
//////////////////////////////////////////////////////////////////////////////
const QCondCache *QGCache::pcondcache( const CHAR *pszName, int nRecurseLevel )
    {            
    QCondCache *pcondcache = 0;
    
    TRY
        {              
        TASKLIST
            {
            // We're going to increment nRecurseLevel shortly...
            if ( nRecurseLevel + 1 > nRecurseLevelMax )
                break;
                
            pcondcache = m_condcachelist.Find( pszName );
            if ( !pcondcache )
                {
                pcondcache = new QCondCache;
                if ( !pcondcache )
                    {
                    THROW_EXCEPTION( exMemory );
                    }
                pcondcache->SetName( pszName );
                if ( pcondcache->Build( ++nRecurseLevel ) )
                    {
                    m_condcachelist.AddTail( pcondcache );
                    }
                else
                    {
                    delete pcondcache;
                    pcondcache = 0;
                    }
                }
            }                      
        ENDTASKLIST
        }
    CATCH_ALL
        {
        delete pcondcache;
        THROW_LAST( );
        }
    END_CATCH
    return pcondcache;
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::pcache( )
//
// Get the cache associated with an indicator.
//////////////////////////////////////////////////////////////////////////////
const QCache *QGCache::pcache( const CHAR *pszIndicatorName, int nRecurseLevel )
    {            
    QCache *pcache = 0;
    
    TRY
        {              
        TASKLIST
            {
            // We're going to increment nRecurseLevel shortly...
            if ( nRecurseLevel + 1 > nRecurseLevelMax )
                break;
                
            pcache = m_cachelist.Find( pszIndicatorName );
            if ( !pcache )
                {
                pcache = new QCache;
                if ( !pcache )
                    {
                    THROW_EXCEPTION( exMemory );
                    }
                pcache->SetName( pszIndicatorName );
                if ( pcache->Build( ++nRecurseLevel ) )
                    {
                    m_cachelist.AddTail( pcache );
                    }
                else
                    {
                    delete pcache;
                    pcache = 0;
                    }
                }
            }                      
        ENDTASKLIST
        }
    CATCH_ALL
        {
        delete pcache;
        THROW_LAST( );
        }
    END_CATCH
    return pcache;
    }

#ifdef DEBUG
const int QGCache::nRecurseLevelMax = 4;
#else
const int QGCache::nRecurseLevelMax = 8;
#endif

//////////////////////////////////////////////////////////////////////////////
// QGCache::prulecache( )
//
// Get the rule cache associated with the name.
//////////////////////////////////////////////////////////////////////////////
const QRuleCache *QGCache::prulecache( const CHAR *pszName, int nRecurseLevel )
    {
    QRuleCache *prulecache = 0;

    TRY
        {
        TASKLIST
            {
            // We're going to increment nRecurseLevel shortly...
            if ( nRecurseLevel + 1 > nRecurseLevelMax )
                {
                QUIT;
                }

            prulecache = m_rulecachelist.Find( pszName );
            if ( !prulecache )
                {
                prulecache = new QRuleCache;
                if ( !prulecache )
                    THROW_EXCEPTION( exMemory );

                prulecache->SetName( pszName );
                if ( prulecache->Build( ++nRecurseLevel ) )
                    {
                    m_rulecachelist.AddTail( prulecache );
                    }
                else
                    {
                    delete prulecache;
                    prulecache = 0;
                    }
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        delete prulecache;
        THROW_LAST( );
        }
    END_CATCH
    return prulecache;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QGCache::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QGCache::AssertValid( ) const
    {                             
    QObject::AssertValid( );
    m_cachelist.AssertValid( );
    m_rulecachelist.AssertValid( );
    }

//////////////////////////////////////////////////////////////////////////////
// QGCache::DumpContents( )
//
// Dumps the contents of the global cache to DBWin.
//////////////////////////////////////////////////////////////////////////////
void QGCache::DumpContents( ) const
    {                                                          
    Trace( TR_NORMAL, "Here is the global cache:\n" );
    for ( QCache *pcache = (QCache *) m_cachelist.GetHead( );
          pcache;
          pcache = (QCache *) m_cachelist.GetNext( pcache ) )
        {
        Trace( TR_NORMAL, "Name = %s.\n", pcache->GetName( ) );
        }  

    Trace( TR_NORMAL, "Here is the global rule cache:\n" );
    for ( QRuleCache *prulecache = (QRuleCache *) m_rulecachelist.GetHead( );
          prulecache;
          prulecache = (QRuleCache *) m_rulecachelist.GetNext( prulecache ) )
        {
        Trace( TR_NORMAL, "Name = %s.\n", prulecache->GetName( ) );
        }

    Trace( TR_NORMAL, "Here is the global condition cache:\n" );
    for ( QCondCache *pcondcache = (QCondCache *) m_condcachelist.GetHead( );
          pcondcache;
          pcondcache = (QCondCache *) m_condcachelist.GetNext( pcondcache ) )
        {
        Trace( TR_NORMAL, "Name = %s.\n", pcondcache->GetName( ) );
        }
    }    
#endif    

//////////////////////////////////////////////////////////////////////////////
// QDirElement::QDirElement( )
//////////////////////////////////////////////////////////////////////////////
QDirElement::QDirElement( )
    {
    lmemset( m_szFullName, 0, sizeof( m_szFullName ) );
    lmemset( m_szFile,     0, sizeof( m_szFile ) );
    lmemset( m_szExt,      0, sizeof( m_szExt ) );
    }

QDirElement::~QDirElement( )
    {
    }

void QDirElement::set( const CHAR *pszName )
    {
    CHAR szDrive[ _MAX_DRIVE + 1 ];
    CHAR szDir  [ _MAX_DIR   + 1 ];
    CHAR szFName[ _MAX_FNAME + 1 ];
    CHAR szExt  [ _MAX_EXT   + 1 ];
    CHAR *pszExt;

    ASSERT( lstrlen( pszName ) < sizeof( m_szFullName ) );
    lstrcpy( m_szFullName, pszName );
    AnsiUpper( m_szFullName );

    _splitpath( m_szFullName, szDrive, szDir, szFName, szExt );
    pszExt = ( *szExt == '.' ? ( szExt + 1 ) : "" );

    ASSERT( lstrlen( szFName ) < sizeof( m_szFile ) );
    lstrcpy( m_szFile, szFName );
    
    ASSERT( lstrlen( pszExt )  < sizeof( m_szExt ) );
    lstrcpy( m_szExt, pszExt );
    }

int QDirElement::compare( const void *elem1, const void *elem2 )
    {
    QDirElement *pElem1 = (QDirElement *) elem1;
    QDirElement *pElem2 = (QDirElement *) elem2;

    if ( lstrcmp( pElem1->extension( ), pElem2->extension( ) ) < 0 )
        return -1;
    else if ( lstrcmp( pElem1->extension( ), pElem2->extension( ) ) > 0 )
        return +1;
    else
        return lstrcmp( pElem1->filename( ), pElem2->filename( ) );
    }

//////////////////////////////////////////////////////////////////////////////
// QDirectory::QDirectory( )
//////////////////////////////////////////////////////////////////////////////
QDirectory::QDirectory( )
    :m_rg( 0 ), m_nAlloc( 0 )
    {
    }

QDirectory::~QDirectory( )
    {
    Cleanup( );
    }

void QDirectory::Cleanup( )
    {
    if ( m_rg )
        {
        delete [ ] m_rg;
        }
    m_rg = 0;
    m_nAlloc = 0;
    }
    
void QDirectory::read( void )
    {
    static const CHAR szFilter1[ ] = "*.qt";
    static const CHAR szFilter2[ ] = "*.txt";
    static const CHAR szFilter3[ ] = "*.*";
    const CHAR *pszFilter;

    UINT n = 0u;
    UINT i = 0u;
    UINT rc;
    struct find_t ff;
    
    Cleanup( );
    
    if ( _pszFilter == _pszFilter1 )
        pszFilter = szFilter1;
    else if ( _pszFilter == _pszFilter2 )
        pszFilter = szFilter2;
    else if ( _pszFilter == _pszFilter3 )
        pszFilter = szFilter3;
    else
        {
        ASSERT( FALSE );
        }

    rc = _dos_findfirst( pszFilter, _A_NORMAL, &ff );
    while ( rc == 0u )
        {
        ++n;
        rc = _dos_findnext( &ff );
        }
    
    if ( !n )
        return;

    m_rg = new QDirElement[ n ];
    if ( !m_rg )
        THROW_EXCEPTION( exMemory );
    m_nAlloc = n;

    rc = _dos_findfirst( pszFilter, _A_NORMAL, &ff );
    while ( rc == 0u )
        {
        ASSERT( i < m_nAlloc );
        m_rg[ i ].set( ff.name );
        ++i;
        rc = _dos_findnext( &ff );
        }
    ASSERT( i == n );
    qsort( m_rg, m_nAlloc, sizeof( QDirElement ), QDirElement::compare );

#ifdef DEBUG    
    Trace( TR_NORMAL, "Here is the directory listing:\n" );
    for ( i = 0; i < n; i++ )
        Trace( TR_NORMAL, "%s\n", m_rg[ i ].fullname( ) );
#endif
    }

const QDirElement &QDirectory::operator[ ]( UINT idx ) const
    {
    ASSERT( idx < m_nAlloc );
    return m_rg[ idx ];
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::QExternalIndicatorRecord( )
//
// Constructors and Destructors
//////////////////////////////////////////////////////////////////////////////
QExternalIndicatorRecord::QExternalIndicatorRecord( )
    :m_numValue( 0 )
    {
    }

QExternalIndicatorRecord::QExternalIndicatorRecord( const QExternalIndicatorRecord &rhs )
    :m_date( rhs.m_date ),
     m_sFileName( rhs.m_sFileName ),
     m_sName( rhs.m_sName ),
     m_numValue( rhs.m_numValue )
    {
    if ( !m_date.IsInit( ) || !m_sFileName.IsInit( ) || !m_sName.IsInit( ) )
        {
        m_date.Cleanup( );
        m_sFileName.Cleanup( );
        m_sName.Cleanup( );
        THROW_EXCEPTION( exInit );
        }
    }

QExternalIndicatorRecord::~QExternalIndicatorRecord( )
    {
    Cleanup( );
    }

void QExternalIndicatorRecord::Cleanup( )
    {
    m_date.Cleanup( );
    m_sFileName.Cleanup( );
    m_sName.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::GetDate( )
//
// Gets an external record's date
//////////////////////////////////////////////////////////////////////////////
const QTime &QExternalIndicatorRecord::GetDate( ) const
    {
    return m_date;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::SetDate( )
//
// Sets an external record's date
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorRecord::SetDate( const QTime &date )
    {
    m_date = date;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::GetFileName( )
//
// Gets an external record's file name
//////////////////////////////////////////////////////////////////////////////
const CHAR *QExternalIndicatorRecord::GetFileName( ) const
    {
    return m_sFileName;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::SetFileName( )
//
// Sets an external record's file name
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorRecord::SetFileName( const CHAR *pszName )
    {
    m_sFileName = pszName;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::GetName( )
//
// Gets an external record's name
//////////////////////////////////////////////////////////////////////////////
const CHAR *QExternalIndicatorRecord::GetName( ) const
    {
    return m_sName;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::SetName( )
//
// Sets an external record's name
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorRecord::SetName( const CHAR *pszName )
    {
    m_sName = pszName;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::GetValue( )
//
// Gets an external record's value
//////////////////////////////////////////////////////////////////////////////
NUM QExternalIndicatorRecord::GetValue( ) const
    {
    return m_numValue;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::SetValue( )
//
// Sets an external record's value
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorRecord::SetValue( NUM num )
    {
    m_numValue = num;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorRecord::AssertValid( )
//
// Standard AssertValid routine
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorRecord::AssertValid( ) const
    {
    QObject::AssertValid( );
    m_date.AssertValid( );
    m_sFileName.AssertValid( );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorList::QExternalIndicatorList
//
// Constructors and Desctructors
//////////////////////////////////////////////////////////////////////////////
QExternalIndicatorList::QExternalIndicatorList( )
    {
    }

QExternalIndicatorList::~QExternalIndicatorList( )
    {
    Cleanup( );
    }

void QExternalIndicatorList::Cleanup( )
    {
    QExternalIndicatorRecord *prec;

    while ( prec = (QExternalIndicatorRecord *) RemoveHead( ) )
        delete prec;
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorList::InsertIndicator( )
//
// Inserts an external indicator into the list (date-sorted)
//////////////////////////////////////////////////////////////////////////////
BOOL QExternalIndicatorList::InsertIndicator( QExternalIndicatorRecord *prec )
    {
    QExternalIndicatorRecord *precNew = NULL;
    BOOL fError( FALSE );
    
    TRY
        {
        for ( QExternalIndicatorRecord *precCurr = (QExternalIndicatorRecord *) GetHead( );
              precCurr; 
              precCurr = (QExternalIndicatorRecord *) GetNext( precCurr ) )
            {
            if ( prec->GetDate( ) < precCurr->GetDate( ) )
                {
                precNew = new QExternalIndicatorRecord( *prec );
                if ( !precNew )
                    THROW_EXCEPTION( exMemory );

                InsertBefore( precCurr, precNew );
                precNew = NULL;
                break;
                }
            
            if ( prec->GetDate( ) == precCurr->GetDate( ) )
                {
                fError = TRUE;
                break;
                }
            }

        if ( precCurr == NULL )
            {
            precNew = new QExternalIndicatorRecord( *prec );
            if ( !precNew )
                THROW_EXCEPTION( exMemory );
            AddTail( precNew );
            precNew = NULL;
            }
        }
    CATCH_ALL
        {
        delete precNew;
        Cleanup( );
        THROW_LAST( );
        }
    END_CATCH

    return !fError;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorList::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QExternalIndicatorRecord *prec = (QExternalIndicatorRecord *) GetHead( ); 
          prec; 
          prec = (QExternalIndicatorRecord *) GetNext( prec ) )
        {
        prec->AssertValid( );
        }
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorDoc::QExternalIndicatorDoc( )
//
// Construction and Destruction
//////////////////////////////////////////////////////////////////////////////
QExternalIndicatorDoc::QExternalIndicatorDoc( )
    {
    }

QExternalIndicatorDoc::~QExternalIndicatorDoc( )
    {
    Cleanup( );
    }

void QExternalIndicatorDoc::Cleanup( )
    {
    m_list.Cleanup( );
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorDoc::RetrieveText( )
//
// Retrieves and builds the document from a text file
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorDoc::RetrieveText( const CHAR *pszFileName )
    {
    FILE *pfile = NULL;
    QExternalIndicatorRecord *prec = NULL;
    QTime date;
    CHAR szBuffer  [ _cbTextMax + 1 ];
    CHAR szFileName[ _cbTextMax + 1 ];
    CHAR szName    [ _cbTextMax + 1 ];
    NUM  numValue;
    BOOL fEOF;
    BOOL fAdded;
    
    TRY
        {
        pfile = fopen( pszFileName, "rt" );
        if ( !pfile )
            THROW_EXCEPTION( exFile );

        Cleanup( );
        while ( TRUE )
            {
            prec = NULL;
            fAdded = FALSE;
            ReadText( pfile, szBuffer, sizeof( szBuffer ), &fEOF );
            if ( fEOF )
                break;

            ParseInputString( szBuffer, &date, szFileName, szName, &numValue );
            
            prec = new QExternalIndicatorRecord;
            if ( !prec )
                THROW_EXCEPTION( exMemory );
            prec->SetDate( date );
            prec->SetFileName( szFileName );
            prec->SetName( szName );
            prec->SetValue( numValue );
            m_list.AddTail( prec );
            fAdded = TRUE;
            }
            
        fclose( pfile );
        }
    CATCH_ALL
        {
        if ( pfile )
            fclose( pfile );
        date.Cleanup( );
        Cleanup( );
        if ( !fAdded )
            delete prec;
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorDoc::ParseInputString( )
//
// Parses an input string into Date, Filename, Name, Value
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorDoc::ParseInputString( CHAR *pszBuffer, QTime *pdate, CHAR *pszFileName, CHAR *pszName, NUM *pnumValue )
    {
    int year;
    int month;
    int day;
    CHAR *pch;
    CHAR szFourDigits[ 5 ];
    CHAR szTwoDigits [ 3 ];
    const CHAR *pszDelimit = ", \t";

    pch = strtok( pszBuffer, pszDelimit );
    if ( pch == NULL )
        THROW_EXCEPTION( exFileCorrupt );
    
    if ( lstrlen( pch ) == 6 )
        {
        szTwoDigits[ 0 ] = *( pch + 0 );
        szTwoDigits[ 1 ] = *( pch + 1 );
        szTwoDigits[ 2 ] = '\0';
        year = AdjustCentury( atoi( szTwoDigits ) );

        szTwoDigits[ 0 ] = *( pch + 2 );
        szTwoDigits[ 1 ] = *( pch + 3 );
        szTwoDigits[ 2 ] = '\0';
        month = atoi( szTwoDigits );

        szTwoDigits[ 0 ] = *( pch + 4 );
        szTwoDigits[ 1 ] = *( pch + 5 );
        szTwoDigits[ 2 ] = '\0';
        day = atoi( szTwoDigits );
        }
    else if ( lstrlen( pch ) == 8 )
        {
        szFourDigits[ 0 ] = *( pch + 0 );
        szFourDigits[ 1 ] = *( pch + 1 );
        szFourDigits[ 2 ] = *( pch + 2 );
        szFourDigits[ 3 ] = *( pch + 3 );
        szFourDigits[ 4 ] = '\0';
        year = AdjustCentury( atoi( szFourDigits ) );

        szTwoDigits[ 0 ] = *( pch + 4 );
        szTwoDigits[ 1 ] = *( pch + 5 );
        szTwoDigits[ 2 ] = '\0';
        month = atoi( szTwoDigits );

        szTwoDigits[ 0 ] = *( pch + 6 );
        szTwoDigits[ 1 ] = *( pch + 7 );
        szTwoDigits[ 2 ] = '\0';
        day = atoi( szTwoDigits );
        }
    else
        THROW_EXCEPTION( exFileCorrupt );
    
    pdate->Set( year, month, day, 12, 0, 0 );

    pch = strtok( NULL, pszDelimit );
    if ( pch == NULL )
        THROW_EXCEPTION( exFileCorrupt );

    if ( lstrlen( pch ) > _cbTextMax )
        THROW_EXCEPTION( exFileCorrupt );
    lstrcpy( pszFileName, pch );
    AnsiUpper( pszFileName );

    pch = strtok( NULL, pszDelimit );
    if ( pch == NULL )
        THROW_EXCEPTION( exFileCorrupt );

    if ( lstrlen( pch ) > _cbTextMax )
        THROW_EXCEPTION( exFileCorrupt );
    lstrcpy( pszName, pch );

    pch = strtok( NULL, pszDelimit );
    if ( pch == NULL )
        THROW_EXCEPTION( exFileCorrupt );
    *pnumValue = (NUM) atof( pch );

    pch = strtok( NULL, pszDelimit );
    if ( pch != NULL )
        THROW_EXCEPTION( exFileCorrupt );
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QExternalIndicatorDoc::AssertValid( )
//
// Standard AssertValid routine
//////////////////////////////////////////////////////////////////////////////
void QExternalIndicatorDoc::AssertValid( ) const
    {
    QObject::AssertValid( );
    m_list.AssertValid( );
    }
#endif
