//////////////////////////////////////////////////////////////////////////////
// WORD.HXX
//
// This file defines a linked list to store arbitrary strings
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _WORD_HXX
#define _WORD_HXX

#include <qobject.hxx>

class QWord : public QListObject
    {
public:
    QWord( )
        {
        }

    virtual ~QWord( )
        {
        Cleanup( );
        }

    virtual void Cleanup( )
        {
        m_sName.Cleanup( );
        }
        
    const CHAR *name( ) const
        {
        return m_sName;
        }
    
    void name( const CHAR *pszName )
        {
        m_sName = pszName;
        }

#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    QString m_sName;
    };

class QWordList : public QVirtualList
    {
public:
    QWordList( )
        {
        }

    virtual ~QWordList( )
        {
        Cleanup( );
        }

    virtual void Cleanup( );

    void AddWord( const CHAR *pszWord );

#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    virtual void Abstract( )
        {
        }
    };

#endif
