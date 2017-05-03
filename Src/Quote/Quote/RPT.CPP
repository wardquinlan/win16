//////////////////////////////////////////////////////////////////////////////
// RPT.CPP
//
// This file generates performance test results.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <dos.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <statpak.h>
#include "resource.hxx"
#include "types.hxx"
#include "template.hxx"
#include "rpt.hxx"
#include "dlg.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "tools.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

static void ReportScanResults( HWND hwndParent, 
                               const CHAR *szName, 
                               int nDays, 
                               BOOL fBuys, 
                               BOOL fSells, 
                               const QScanList &scanlist );

static void ReportIndicatorValueResults( HWND hwnd, NUM numValue );

//////////////////////////////////////////////////////////////////////////////
// SaveScanResults( )
//
// This routine will save the results of a chart scan into
// a file
//////////////////////////////////////////////////////////////////////////////
extern void SaveScanResults( HWND hwnd, const ScanResults &res, const CHAR *pszPath )
    {
    QTime dt;
    FILE *pfile = NULL;

    TRY
        {
        dt.Format( 32, "%m/%d/%Y");

        pfile = fopen( pszPath, "wt" );
        if ( !pfile )
            THROW_EXCEPTION( exFile );

        fprintf( pfile, "Chart Scanner Results for '%s'\n\n", res.m_szName );
        fprintf( pfile, "Number of Days    : %d\n", res.m_nDays );
        fprintf( pfile, "Scanning for Buys : %c\n", res.m_fBuys  ? 'Y' : 'N' );
        fprintf( pfile, "Scanning for Sells: %c\n", res.m_fSells ? 'Y' : 'N' );
        fprintf( pfile, "Signals found     : %u\n\n", res.m_scanlist.GetCount( ) );
        fprintf( pfile, "Scan Details\n" );
        fprintf( pfile, "------------\n\n" );

        fprintf( pfile, "%12s  %10s  %4s  %9s\n",
                     "Security",
                     "Date",
                     "Sig",
                     "Price" );

        fprintf( pfile, "%12s  %10s  %4s  %9s\n",
                     "------------",
                     "----------",
                     "----",
                     "---------" );

        for ( const QScan *psc = (const QScan *) res.m_scanlist.GetHead( );
              psc;
              psc = (const QScan *) res.m_scanlist.GetNext( psc ) )
            {
            dt.Set( psc->date( ).GetYear( ),
                    psc->date( ).GetMonth( ),
                    psc->date( ).GetDay( ),
                    12, 0, 0 );

            if ( psc->pendingState( ) )
                {
                fprintf( pfile, "%12s  %10s  %4s  %9s\n",
                         (const CHAR *) psc->company( ),
                         (const CHAR *)(QString) dt,
                         (const CHAR *) ( psc->signaltype( ) == QSignal::sgBuy ? "Buy" : "Sell" ),
                         "<pending>" );
                }
            else
                {
                fprintf( pfile, "%12s  %10s  %4s  %9.3f\n",
                         (const CHAR *) psc->company( ),
                         (const CHAR *)(QString) dt,
                         (const CHAR *) ( psc->signaltype( ) == QSignal::sgBuy ? "Buy" : "Sell" ),
                         psc->price( ) );
                }
            }
        
        fprintf( pfile, "\n\nThere were %u corrupt files\n\n", res.m_scanlist.corruptfiles( ).GetCount( ) );
        if ( res.m_scanlist.corruptfiles( ).GetCount( ) )
            {
            for ( const QWord *pwd = (const QWord *) res.m_scanlist.corruptfiles( ).GetHead( );
                  pwd;
                  pwd = (const QWord *) res.m_scanlist.corruptfiles( ).GetNext( pwd ) )
                {
                fprintf( pfile, "%12s\n", (const CHAR *) pwd->name( ) );
                }
            }

        if ( pfile )
            {
            fclose( pfile );
            pfile = NULL;
            }
        
        ViewFile( hwnd, pszPath );
        }
    CATCH_ALL
        {
        if ( pfile )
            fclose( pfile );
        dt.Cleanup( );
        MsgBox( hwnd, IDS_GENERALERROR );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QScanList::Cleanup( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void QScanList::Cleanup( )
    {
    QScan *psc;

    m_wordlist.Cleanup( );
    while ( psc = (QScan *) RemoveHead( ) )
        delete psc;
    }

//////////////////////////////////////////////////////////////////////////////
// QScanList::SignalList2ScanList
//
// This routine will convert a signal list into a scan list.
//////////////////////////////////////////////////////////////////////////////
void QScanList::SignalList2ScanList( QSignalList &sglist, BOOL fBuys, BOOL fSells, int nDays )
    {
    const QSignal *psg = (QSignal *) sglist.GetTail( );
    QScan *psc = 0;

    TRY
        {
        TASKLIST
            {
            if ( !_pdoc->GetCount( ) || !psg )
                break;

            time_t span = (time_t) _pdoc->GetLast( )->GetDate( ) - (time_t) psg->GetDate( );
            long n = seconds2days( span );
            ASSERT( n >= 0 );
            
            if ( fBuys && psg->GetSg( ) == QSignal::sgBuy &&
                 ( nDays == 0 || n < (long) nDays ) )
                {
                psc = new QScan;
                if ( !psc )
                    THROW_EXCEPTION( exMemory );
                psc->company   ( _pdoc->GetFileName( ) );
                psc->date      ( psg->GetDate( ) );
                psc->signaltype( psg->GetSg( ) );
                
                if ( psg->GetPendingState( ) )
                    {
                    psc->price( 0 );
                    psc->pendingState( TRUE );
                    }
                else
                    {
                    psc->price( psg->GetPrice( ) );
                    }
                AddTail( psc );
                }
            
            if ( fSells && psg->GetSg( ) == QSignal::sgSell &&
                 ( nDays == 0 || n < (long) nDays ) )
                {
                psc = new QScan;
                if ( !psc )
                    THROW_EXCEPTION( exMemory );
                psc->company   ( _pdoc->GetFileName( ) );
                psc->date      ( psg->GetDate( ) );
                psc->signaltype( psg->GetSg( ) );
                
                if ( psg->GetPendingState( ) )
                    {
                    psc->price( 0 );
                    psc->pendingState( TRUE );
                    }
                else
                    {
                    psc->price( psg->GetPrice( ) );
                    }
                AddTail( psc );
                }
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        delete psc;
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QScanList::AppendCompany( )
//
// This method will add the pre-loaded company to the end of self.
//////////////////////////////////////////////////////////////////////////////
void QScanList::AppendCompany( const CHAR *szName, BOOL fBuys, BOOL fSells, int nDays )
    {
    QRule       *prule;
    QSignalList  sglist;

    TRY
        {
        prule = _ptpl->m_rulelist.Find( szName );
        ASSERT( prule != NULL );

        if ( prule->Evaluate( sglist, FALSE, FALSE, 0 ) )
            {
            SignalList2ScanList( sglist, fBuys, fSells, nDays );
            }
        }
    CATCH_ALL
        {
        sglist.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QScanList::Build( )
//
// Builds a scan list.
//////////////////////////////////////////////////////////////////////////////
void QScanList::Build( HWND        hwndParent,
                       const CHAR *szName,
                       int         nDays, 
                       const CHAR *pszFilter, 
                       BOOL        fBuys, 
                       BOOL        fSells )
    {
    struct find_t fileinfo;
    int    rc;
    HCURSOR hCursor = NULL;

    TRY
        {
        m_wordlist.Cleanup( );
        SaveQuery( hwndParent );
        DestroyAllChildren( );
        _pdoc->Cleanup( );
        UpdateName( );
        UpdateMenu( );
        hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
        rc = _dos_findfirst( pszFilter, _A_RDONLY, &fileinfo );
        while ( rc == 0 )
            {
            TRY
                {
                SilentOpen( fileinfo.name );
                AppendCompany( szName, fBuys, fSells, nDays );
                }
            CATCH( exMemory )
                {
                THROW_LAST( );
                }
            CATCH_ALL
                {
                m_wordlist.AddWord( fileinfo.name );
                }
            END_CATCH
            rc = _dos_findnext( &fileinfo );
            }
        }
    CATCH_ALL
        {
        DestroyAllChildren( );
        _pdoc->Cleanup( );
        UpdateName( );
        UpdateMenu( );
        if ( hCursor )
            SetCursor( hCursor );
        THROW_LAST( );
        }
    END_CATCH
    
    DestroyAllChildren( );
    _pdoc->Cleanup( );
    UpdateName( );
    UpdateMenu( );
    if ( hCursor )
        SetCursor( hCursor );
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QScanList::AssertValid( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void QScanList::AssertValid( ) const
    {
    QVirtualList::AssertValid( );
    for ( QScan *psc = (QScan *) GetHead( ); psc; psc = (QScan *) GetNext( psc ) )
        psc->AssertValid( );
    }

//////////////////////////////////////////////////////////////////////////////
// QScan::AssertValid( )
//
// Standard stuff.
//////////////////////////////////////////////////////////////////////////////
void QScan::AssertValid( ) const
    {
    QListObject::AssertValid( );
    m_sName.AssertValid( );
    ASSERT( m_sg == QSignal::sgBuy || m_sg == QSignal::sgSell );
    ASSERT( m_numPrice >= (NUM) 0 );
    }
#endif

//////////////////////////////////////////////////////////////////////////////
// IndicatorValue( )
//
// Reports the value of an indicator on a given date
//////////////////////////////////////////////////////////////////////////////
extern void IndicatorValue( HWND hwnd, const CHAR *pszName, const QTime &dt )
    {
    NUM numValue = 0;
    const QQt *pqt;
    UINT i;
    UINT iFound;
    UINT nNorm;
    BOOL fFound = FALSE;
    HCURSOR hCursor = NULL;

    TRY
        {
        TASKLIST
            {
            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            for ( i = 0, pqt = _pdoc->GetFirst( );
                  pqt;
                  i++, pqt = _pdoc->GetNext( pqt ) )
                {   
                if ( pqt->GetDate( ) == dt )
                    {
                    fFound  = TRUE;
                    iFound  = i;
                    break;
                    }
                }
 
            if ( !fFound )
                {
                MsgBox( hwnd, IDS_NOVALUE );
                break;
                }

            const QCache *pcache = gcache.pcache( pszName, 0 );
            if ( !pcache )
                {
                MsgBox( hwnd, IDS_NOVALUE );
                break;
                }

            if ( iFound < pcache->m_dataset.nOffset )
                {
                MsgBox( hwnd, IDS_NOVALUE );
                break;
                }

            nNorm = iFound - pcache->m_dataset.nOffset;
            numValue = pcache->m_dataset.lpnum[ nNorm ];
            ReportIndicatorValueResults( hwnd, numValue );
            }
        ENDTASKLIST
        }
    CATCH_ALL
        {
        if ( hCursor )
            SetCursor( hCursor );
        THROW_LAST( );
        }
    END_CATCH
    if ( hCursor )
        SetCursor( hCursor );
    }

//////////////////////////////////////////////////////////////////////////////
// ChartScanner( )
//
// This is the mainline into the chart scanner module.
//////////////////////////////////////////////////////////////////////////////
extern void ChartScanner( HWND        hwndParent,
                          const CHAR *szName, 
                          int         nDays, 
                          const CHAR *pszFilter, 
                          BOOL        fBuys, 
                          BOOL        fSells )
    {
    QScanList scanlist;

    TRY
        {
        scanlist.Build( hwndParent, szName, nDays, pszFilter, fBuys, fSells );
#ifdef DEBUG
        scanlist.AssertValid( );
#endif
        ReportScanResults( hwndParent, szName, nDays, fBuys, fSells, scanlist );
        }
    CATCH_ALL
        {
        scanlist.Cleanup( );
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// ReportIndicatorValueResults( )
//
// This routine will report on the scan results.
//////////////////////////////////////////////////////////////////////////////
static void ReportIndicatorValueResults( HWND hwnd, NUM numValue )
    {
    FARPROC lpfn;
    
    TRY
        {
        lpfn = MakeProcInstance( (FARPROC) ReportIndicatorValueResultsProc, _hInstance );
        DialogBoxParam( _hInstance, MAKEINTRESOURCE( IDD_REPORT_INDICATORVALUE_RESULTS ),
                        hwnd, (DLGPROC) lpfn, (LPARAM) &numValue );
        FreeProcInstance( lpfn );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }


//////////////////////////////////////////////////////////////////////////////
// ReportScanResults( )
//
// This routine will report on the scan results.
//////////////////////////////////////////////////////////////////////////////
static void ReportScanResults( HWND hwndParent, const CHAR *szName, int nDays, BOOL fBuys, BOOL fSells, 
                               const QScanList &scanlist )
    {
    ASSERT( lstrlen( szName ) <= _cbNameMax );
    
    ScanResults res( szName, nDays, fBuys, fSells, scanlist );
    FARPROC lpfn;

    TRY
        {
        lpfn = MakeProcInstance( (FARPROC) ReportScanResultsProc, _hInstance );
        DialogBoxParam( _hInstance, MAKEINTRESOURCE( IDD_REPORT_SCAN_RESULTS ),
                        hwndParent, (DLGPROC) lpfn, (LPARAM) &res );
        FreeProcInstance( lpfn );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

