//////////////////////////////////////////////////////////////////////////////
// RPT.HXX
//
// This file generates scan results
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _RPT_HXX
#define _RPT_HXX

#include "word.hxx"

//////////////////////////////////////////////////////////////////////////////
// seconds2days( )
//
// Helper to convert seconds to days
//////////////////////////////////////////////////////////////////////////////
inline ULONG seconds2days( time_t seconds )
    {
    return ( (ULONG) seconds / (ULONG) 86400 );
    }

//////////////////////////////////////////////////////////////////////////////
// struct ScanResults
//////////////////////////////////////////////////////////////////////////////
class QScanList;
struct ScanResults
    {
    ScanResults( const CHAR *szName, int nDays, BOOL fBuys, BOOL fSells, const QScanList &scanlist )
        :m_nDays( nDays ), m_fBuys( fBuys ), m_fSells( fSells ), m_scanlist( scanlist )
        {
        lstrcpy( m_szName, szName );
        }
    
    CHAR m_szName[ _cbNameMax + 1 ];
    int  m_nDays;
    BOOL m_fBuys;
    BOOL m_fSells;
    const QScanList &m_scanlist;
    };

//////////////////////////////////////////////////////////////////////////////
// class QScan
//
// Defines one discovered (scanned) signal
//////////////////////////////////////////////////////////////////////////////
class QScan : public QListObject
    {
public:
    QScan( )
        : m_sName( ), m_date( ), m_sg( QSignal::sgBuy ), m_numPrice( 0 ), m_fPendingState( FALSE )
        {
        }

    ~QScan( )
        {
        Cleanup( );
        }

    virtual void Cleanup( )
        {
        m_sName.Cleanup( );
        m_date.Cleanup( );
        }

    const CHAR *company( ) const
        {
        return m_sName;
        }
    
    void company( const CHAR *pszName )
        {
        m_sName = pszName;
        }

    const QTime &date( ) const
        {
        return m_date;
        }

    void date( const QTime &d )
        {
        m_date = d;
        }

    const QSignal::SignalType &signaltype( ) const
        {
        return m_sg;
        }

    void signaltype( const QSignal::SignalType &sg )
        {
        m_sg = sg;
        }

    const NUM &price( ) const
        {
        return m_numPrice;
        }

    void price( const NUM &num )
        {
        m_numPrice = num;
        }

    BOOL pendingState( ) const
        {
        return m_fPendingState;
        }

    void pendingState( BOOL f ) 
        {
        m_fPendingState = f;
        }

#ifdef DEBUG
    void AssertValid( ) const;
#endif
        
private:
    BOOL                m_fPendingState;
    QString             m_sName;
    QTime               m_date;
    QSignal::SignalType m_sg;
    NUM                 m_numPrice;
    };

//////////////////////////////////////////////////////////////////////////////
// class QScanList
//
// Collection class for the scanner
//////////////////////////////////////////////////////////////////////////////
class QScanList : public QVirtualList
    {
public:
    QScanList( )
        {
        }

    ~QScanList( )
        {
        Cleanup( );
        }

    virtual void Cleanup( );

    void Build( HWND        hwndParent,
                const CHAR *szName,
                int         nDays,
                const CHAR *pszFilter,
                BOOL        fBuys,
                BOOL        fSells );

    const QWordList &corruptfiles( ) const
        {
        return m_wordlist;
        }

#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    virtual void Abstract( )
        {
        }
    void AppendCompany( const CHAR *szName, BOOL fBuys, BOOL fSells, int nDays );
    void SignalList2ScanList( QSignalList &sglist, BOOL fBuys, BOOL fSells, int nDays );
    NUM  Price( const QTime &date ) const;

    QWordList m_wordlist;
    };

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous
//////////////////////////////////////////////////////////////////////////////
extern void ChartScanner( HWND        hwndParent,
                          const CHAR *szName, 
                          int         nDays, 
                          const CHAR *pszFilter, 
                          BOOL        fBuys, 
                          BOOL        fSells );

extern void IndicatorValue( HWND hwnd, const CHAR *pszName, const QTime &dt );
extern void SaveScanResults( HWND hwnd, const ScanResults &res, const CHAR *pszPath );

#endif

