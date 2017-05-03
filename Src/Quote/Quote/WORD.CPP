//////////////////////////////////////////////////////////////////////////////
// WORD.CPP
//
// This file generates performance test results.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include "word.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// QWord methods
//////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void QWord::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// QWordList methods
//////////////////////////////////////////////////////////////////////////////
void QWordList::Cleanup( )
    {
    QWord *pwd;

    while ( pwd = (QWord *) RemoveHead( ) )
        delete pwd;
    }

void QWordList::AddWord( const CHAR *pszWord )
    {
    QWord *pwd = 0;
    
    TRY
        {
        pwd = new QWord;
        if ( !pwd )
            THROW_EXCEPTION( exMemory );
        pwd->name( pszWord );
        AddTail( pwd );
        }
    CATCH_ALL
        {
        delete pwd;
        THROW_LAST( );
        }
    END_CATCH
    }

#ifdef DEBUG
void QWordList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QWord *pwd = (QWord *) GetHead( ); pwd; pwd = (QWord *) GetNext( pwd ) )
        pwd->AssertValid( );
    }
#endif
