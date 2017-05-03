//////////////////////////////////////////////////////////////////////////////
// VIEW.CPP
//
// This file contains drawing code.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <math.h>
#include <float.h>
#include <statpak.h>
#include "types.hxx"
#include "resource.hxx"
#include "template.hxx"
#include "tools.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "view.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// Constant definitions
//////////////////////////////////////////////////////////////////////////////
const UINT QView::m_nCharsRight     = 12;
const NUM  QView::m_numVlFactor     = 0.25; // percentage of grid volume bar takes
const int  QView::m_nShift          = 8;    // 1/m_nShift-screenfuls on left/right shifts
const int  QView::m_dxBase          = 3;
const int  QView::m_dyHeight        = 6;
const int  QView::m_dySpace         = 3;
const int  QView::m_dLen            = 1;
const int  QView::m_dSpc            = 8;

//////////////////////////////////////////////////////////////////////////////
// ScaleLinear( )
//
// Performs a no-op scaling transformation.
//////////////////////////////////////////////////////////////////////////////
extern NUM ScaleLinear( NUM num )
    {
    return num;
    }

//////////////////////////////////////////////////////////////////////////////
// ScaleLog( )
//
// Performs a log scaling transformation.
//////////////////////////////////////////////////////////////////////////////
extern NUM ScaleLog( NUM num )
    {
    return (NUM) log( num );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::m_rgnum[ ]
//
// Array of round scaling prices.
//////////////////////////////////////////////////////////////////////////////
const NUM QView::m_rgnum[] =
    {
    0.01,
    0.02,
    0.05,
    0.10,
    0.125,
    0.25,
    0.5,
    1.0,
    2.0,
    5.0,
    10.0,
    25.0,
    50.0,
    100.0,
    200.0,
    500.0,
    1000.0,
    2000.0,
    5000.0,
    10000.0
    };

//////////////////////////////////////////////////////////////////////////////
// QView::QView( )
//
// Constructors and desctructors.
//////////////////////////////////////////////////////////////////////////////
QView::QView( const CHAR *pszName, BOOL fPrintView )
    : m_fPrintView( fPrintView ), 
      m_xLines( fPrintView ? 1 : 0 )
    {
    for ( int i = 0; i < _cViewLayouts; i++ )
        {
        m_rgviewinfo[ i ].fPresent = FALSE;
        
        /////////////////////////////////////
        // Assume true until proven otherwise
        m_rgviewinfo[ i ].fAvail   = TRUE;
        m_rgviewinfo[ i ].pdict    = 0;
        
        lmemset( &m_rgviewinfo[ i ].rcGrid, 0, sizeof( RECT ) );
        lmemset( &m_rgviewinfo[ i ].ptTitle, 0, sizeof( POINT ) );

        m_rgnumMinAbs[ i ]  = 0.0;
        m_rgnumMaxAbs[ i ]  = 0.0;
        m_rgnumScale[ i ]   = 0.0;
        m_rgnumMinAdj[ i ]  = 0.0;
        m_rgnumStepAdj[ i ] = 0.0;
        
        m_rgrgnScales[ i ]  = NULL;
        m_rgpfnScale[ i ]   = NULL;
        }
    m_pqqtFirst = NULL;
    m_nMax      = 0;
    m_nFirst    = 0;
    m_nViews    = 0;
    }

QView::~QView( )
    {
    Cleanup( );
    }

void QView::Cleanup( )
    {
    for ( int i = 0; i < _cViewLayouts; i++ )
        {
        m_rgviewinfo[ i ].fPresent = FALSE;
        
        /////////////////////////////////////
        // Assume true until proven otherwise
        m_rgviewinfo[ i ].fAvail   = TRUE;
        m_rgviewinfo[ i ].pdict    = 0;
        
        lmemset( &m_rgviewinfo[ i ].rcGrid, 0, sizeof( RECT ) );
        lmemset( &m_rgviewinfo[ i ].ptTitle, 0, sizeof( POINT ) );
        m_rgviewinfo[ i ].sglist.Cleanup( );
        m_rgviewinfo[ i ].sTitle.Cleanup( );

        m_rgnumMinAbs[ i ]  = 0.0;
        m_rgnumMaxAbs[ i ]  = 0.0;
        m_rgnumScale[ i ]   = 0.0;
        m_rgnumMinAdj[ i ]  = 0.0;
        m_rgnumStepAdj[ i ] = 0.0;
        m_rgnumScaleVl[ i ] = 0.0;

        delete [] m_rgrgnScales[ i ];
        m_rgrgnScales[ i ]  = NULL;
        m_rgpfnScale[ i ]   = NULL;
        }
    m_pqqtFirst = NULL;
    m_nMax      = 0;
    m_nFirst    = 0;
    m_nViews    = 0;
    m_numVlMax  = 0.0;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDraw( )
//
// Draws the entire view.
//
// Parameters:
//
// pwl   - the window layout
// hdc   - the HDC
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDraw( const QWindowLayout *pwl, HDC hdc )
    {   
    HFONT hfont( NULL );
    int   nViewMax;
    
    SaveDC( hdc );
    TRY
        {
        LOGFONT lf; 
        lmemset( &lf, 0, sizeof( lf ) );
        lf.lfWeight         = FW_NORMAL;
        lf.lfHeight         = _ptpl->m_genopts.GetFontSize( );
        lstrcpy( lf.lfFaceName, _pszFontName );
        hfont               = CreateFontIndirect( &lf );
        if ( hfont == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
        SelectObject( hdc, hfont );
#if 0
        FILE *pfile = fopen( "d:\\tmp\\font.txt", "wt" );
        ASSERT( pfile != NULL );
        CHAR sz[ 256 ];
        GetTextFace( hdc, sizeof( sz ), sz );
        fprintf( pfile, "FaceName = %s\n", sz );
        fclose( pfile );
#endif
        
        SetTextColor( hdc, _rgclr[ _ptpl->m_genopts.GetTextColor( ) ] );
        SetBkMode( hdc, TRANSPARENT );
        
        nViewMax = 0;
        for ( int nView = 0; nView < _cViewLayouts; nView++ )
            {
            if ( m_rgviewinfo[ nView ].fPresent )
                {
                if ( m_rgviewinfo[ nView ].fAvail )
                    OnDrawConditions( pwl, hdc, nView );
                
                OnDrawGrids( pwl, hdc, nView );
                
                if ( m_rgviewinfo[ nView ].fAvail )
                    {
                    OnDrawV     ( pwl, hdc, nView );
                    OnDrawPrices( pwl, hdc, nView );
                    }
                nViewMax = nView;
                }
            }
        OnDrawDates( pwl, hdc, nViewMax );
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hfont );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hfont );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawV( )
//
// Draws the volume bars.
//
// Parameters:
//
// pwl   - the window layout
// hdc   - the HDC
// nView - the view number
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawV( const QWindowLayout *pwl, HDC hdc, int nView )
    {
    HPEN hpenVl( NULL );
    
    SaveDC( hdc );
    TRY
        {
        hpenVl = CreatePen( PS_SOLID, m_xLines, _rgclr[ _ptpl->m_genopts.GetVolumeColor( ) ] );
        if ( hpenVl == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
        SelectObject( hdc, hpenVl );

        if ( pwl->m_rgvl [ nView ].GetHLCV( ) )
            {
            ////////////////////
            // Main drawing loop
            QQt *pqqt;
            int x, y, n;
            int dxIncr = (int) pwl->GetDxIncr( );
            ASSERT( dxIncr > 0 );
            for ( x = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
                  n = 0, 
                  pqqt = m_pqqtFirst;

                  pqqt && n < m_nMax;
            
                  x += dxIncr,
                  n++, 
                  pqqt = _pdoc->GetNext( pqqt ) )
                {
                y = m_rgviewinfo[ nView ].rcGrid.bottom + 
                    (int) ( m_rgnumScaleVl[ nView ] * pqqt->GetVl( ) * m_numVlFactor );
                MoveTo( hdc, x, y );
                LineTo( hdc, x, m_rgviewinfo[ nView ].rcGrid.bottom );
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpenVl );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );

    SafeDeleteObject( hpenVl );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawPrices( )
//
// Draws price-related graphs.
//
// Parameters:
//
// pwl   - the window layout
// hdc   - the HDC
// nView - the view number
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawPrices( const QWindowLayout *pwl, HDC hdc, int nView )
    {
    SaveDC( hdc );
    TRY
        {
        OnDrawDateLines ( pwl, hdc, nView );
        OnDrawHLC       ( pwl, hdc, nView );
        OnDrawIndicators( pwl, hdc, nView );
        OnDrawSignals   ( pwl, hdc, nView );
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawIndicators( )
//
// Draws indicators.
//
// Parameters:
//
// pwl   - the window layout
// hdc   - the HDC
// nView - the view number
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawIndicators( const QWindowLayout *pwl, HDC hdc, int nView )
    {
    SaveDC( hdc );       
    TRY      
        {                       
        QIndicatorName *pname;
        ASSERT( m_rgviewinfo[ nView ].pdict != 0 );

        for ( pname = (QIndicatorName *) m_rgviewinfo[ nView ].pdict->GetHead( );
              pname;
              pname = (QIndicatorName *) m_rgviewinfo[ nView ].pdict->GetNext( pname ) )
            {
            QString sName( (QString) *pname );

            QIndicatorTemplate *pit = _ptpl->m_itlist.Find( sName );
            ASSERT( pit != NULL );
            
            if ( pit->GetGraphType( ) == gtBar )
                {
                const QCache *pcache = gcache.pcache( sName, 0 );
                ASSERT( pcache != NULL );
                OnDrawHorizontalLine( pwl, hdc, nView, pcache, pit );
                OnDrawBarIndicator( pwl, hdc, nView, pcache, pit );
                }
            }
        
        for ( pname = (QIndicatorName *) m_rgviewinfo[ nView ].pdict->GetHead( );
              pname;
              pname = (QIndicatorName *) m_rgviewinfo[ nView ].pdict->GetNext( pname ) )
            {
            QString sName( (QString) *pname );

            QIndicatorTemplate *pit = _ptpl->m_itlist.Find( sName );
            ASSERT( pit != NULL );

            if ( pit->GetGraphType( ) == gtLine )
                {
                const QCache *pcache = gcache.pcache( sName, 0 );
                ASSERT( pcache != NULL );
                OnDrawHorizontalLine( pwl, hdc, nView, pcache, pit );
                OnDrawLineIndicator( pwl, hdc, nView, pcache, pit );
                }
            }
        
        for ( pname = (QIndicatorName *) m_rgviewinfo[ nView ].pdict->GetHead( );
              pname;
              pname = (QIndicatorName *) m_rgviewinfo[ nView ].pdict->GetNext( pname ) )
            {
            QString sName( (QString) *pname );

            QIndicatorTemplate *pit = _ptpl->m_itlist.Find( sName );
            ASSERT( pit != NULL );

            if ( pit->GetGraphType( ) == gtPoint )
                {
                const QCache *pcache = gcache.pcache( sName, 0 );
                ASSERT( pcache != NULL );
                OnDrawHorizontalLine( pwl, hdc, nView, pcache, pit );
                OnDrawPointIndicator( pwl, hdc, nView, pcache, pit );
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::hline( )
//
// Draws a horizontal line
//////////////////////////////////////////////////////////////////////////////
void QView::hline( HDC hdc, int y, int x1, int x2 )
    {
    int xCur = x1;
    int xEnd;

    while ( xCur < x2 )
        {
        xEnd = min( xCur + m_dLen, x2 );
        MoveTo( hdc, xCur, y );
        LineTo( hdc, xEnd, y );
        
        xCur = xEnd + m_dSpc;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QView::vline( )
//
// Draws a vertical line
//////////////////////////////////////////////////////////////////////////////
void QView::vline( HDC hdc, int x, int y1, int y2 )
    {
    int yCur = y1;
    int yEnd;

    while ( yCur < y2 )
        {
        yEnd = min( yCur + m_dLen, y2 );
        MoveTo( hdc, x, yCur );
        LineTo( hdc, x, yEnd );
        
        yCur = yEnd + m_dSpc;
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawHorizontalLine( )
//
// Draws a single indicator's horizontal line.    
//
// Parameters:
//
// pwl   - the window layout
// hdc   -  the HDC
// nView -  the view number
// pcache - the indicator cache
// pit   -  pointer to indicator template
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawHorizontalLine( const QWindowLayout *pwl, HDC hdc, int nView, 
                                  const QCache *pcache, const QIndicatorTemplate *pit )
    {    
    HPEN  hpen ( NULL );
    
    SaveDC( hdc );
    TRY      
        {
        hpen = CreatePen( PS_SOLID, m_xLines, _rgclr[ pit->GetIdxClr( ) ] );
        if ( hpen == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
        SelectObject( hdc, hpen );
        
        SetTextColor( hdc, _rgclr[ pit->GetIdxClr( ) ] );
        
        // Get font width
        TEXTMETRIC tm;
        GetTextMetrics( hdc, &tm );
        int dxChar = tm.tmAveCharWidth;
        
        // Draw the axis numbers on the left
        for ( int i = 0; i < (int) pcache->m_datasetHorz.nPoints; i++ )
            {
            NUM num = pcache->m_datasetHorz.lpnum[ i ];
            CHAR sz[ _cbBufferMax + 1 ];
            sprintf( sz, "%-6.3f", num );
            DWORD lw = GetTextExtent( hdc, sz, lstrlen( sz ) );
            int width  = LOWORD( lw );
            int height = HIWORD( lw );
            int x = m_rgviewinfo[ nView ].rcGrid.right + dxChar;
            int y = Scale( nView, num );
            TextOut( hdc, x, y + height / 2, sz, lstrlen( sz ) );

            hline( hdc, y, m_rgviewinfo[ nView ].rcGrid.left, m_rgviewinfo[ nView ].rcGrid.right );
            }
        }
    CATCH_ALL
        {         
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpen );
        THROW_LAST( );
        }             
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpen );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawDateLines( )
//
// Draws the dates at the bottom of the window.  It also
// draws the vertical date lines in the grids.
//
// Parameters:
//     pwl   - The window layout.
//     hdc   - The device context.
//     nView - The view number.
//
// Returns:
//     void.
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawDateLines( const QWindowLayout *pwl, HDC hdc, int nView )
    {
    HPEN hpen( NULL );

    if ( _ptpl->m_genopts.GetDateType( ) == dtNone )
        return;

    SaveDC( hdc );
    TRY
        {
        hpen = CreatePen( PS_SOLID, m_xLines, _rgclr[ _ptpl->m_genopts.GetGridsColor( ) ] );
        if ( hpen == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
        SelectObject( hdc, hpen );
            
        DateType dt = _ptpl->m_genopts.GetDateType( );
        ASSERT( dt != dtNone );
        QQt *pqqt;
        QQt *pqqtPrev;
        int x, n;
        int dxIncr = (int) pwl->GetDxIncr( );
        ASSERT( dxIncr > 0 );

        for ( x = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
              n = 0, 
              pqqtPrev = NULL,
              pqqt = m_pqqtFirst;

              pqqt && n < m_nMax;
    
              x += dxIncr,
              n++, 
              pqqtPrev = pqqt, pqqt = _pdoc->GetNext( pqqt ) )
            {
            if ( IsDrawDate( pqqtPrev, pqqt ) )
                {
                // inject a date line
                vline( hdc, x, m_rgviewinfo[ nView ].rcGrid.bottom, m_rgviewinfo[ nView ].rcGrid.top );
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpen );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpen );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawConditions
//
// Draws conditions.
//
// Parameters:
//     pwl   - The window layout.
//     hdc   - The device context.
//     nView - The view number.
//
// Returns:
//     void.
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawConditions( const QWindowLayout *pwl, HDC hdc, int nView )
    {
    HPEN   hpen( NULL );
    HBRUSH hbr ( NULL );

    SaveDC( hdc );
    TRY
        {
        const CHAR *pszName = pwl->m_rgvl[ nView ].GetCondition( );
        if ( pszName )
            {
            QCondition *pcond = _ptpl->m_condlist.Find( pszName );
            ASSERT( pcond != NULL );

            const QCondCache *pcondcache = gcache.pcondcache( pszName, 0 );
            ASSERT( pcondcache != NULL );
            
            hpen = CreatePen( PS_SOLID, m_xLines, _rgclr[ pcond->GetIdxClr( ) ] );
            if ( hpen == NULL )
                {
                THROW_EXCEPTION( exGdi );
                }       
            SelectObject( hdc, hpen );

            LOGBRUSH lb;
            lmemset( &lb, 0, sizeof( lb ) );
            lb.lbStyle = BS_SOLID;
            lb.lbColor = _rgclr[ pcond->GetIdxClr( ) ];
            hbr = CreateBrushIndirect( &lb );
            if ( hbr == NULL )
                {
                THROW_EXCEPTION( exGdi );
                }
            SelectObject( hdc, hbr );
        
            int nCurMax = (int) _pdoc->GetCount( );    
            ASSERT( nCurMax >= 0 );
            int dxIncr  = (int) pwl->GetDxIncr( );
            ASSERT( dxIncr > 0 );
        
            int x, n, nCur;
            for ( x    = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
                  n    = 0,
                  nCur = m_nFirst;
                      
                  nCur < nCurMax && n < m_nMax;

                  x += dxIncr,
                  n++,
                  nCur++ )
                {
                // nCurNorm is normalized for the dataset
                int nCurNorm = nCur - pcondcache->DataSet( ).nOffset;
                if ( nCurNorm >= 0 && pcondcache->DataSet( ).lpnum[ nCurNorm ] != 0 )
                    {
                    if ( dxIncr == 1 )
                        {
                        MoveTo( hdc, x, m_rgviewinfo[ nView ].rcGrid.bottom );
                        LineTo( hdc, x, m_rgviewinfo[ nView ].rcGrid.top );
                        }
                    else
                        {
                        Rectangle( hdc, x, m_rgviewinfo[ nView ].rcGrid.bottom,
                                   x + dxIncr, m_rgviewinfo[ nView ].rcGrid.top );
                        }
                    }
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpen );
        SafeDeleteObject( hbr );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpen );
    SafeDeleteObject( hbr );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawBarIndicator( )
//
// Draws a bar indicator.
//
// Parameters:
//
// pwl    - the window layout
// hdc    - the HDC
// nView  - the view number
// pcache - the indicator cache
// pit    - pointer to indicator template
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawBarIndicator( const QWindowLayout *pwl, HDC hdc, int nView, 
                                const QCache *pcache, const QIndicatorTemplate *pit )
    {   
    HPEN   hpen( NULL );
    HBRUSH hbr ( NULL );
    
    SaveDC( hdc );
    TRY                  
        {           
        hpen = CreatePen( PS_SOLID, m_xLines, _rgclr[ pit->GetIdxClr( ) ] );
        if ( hpen == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }       
        SelectObject( hdc, hpen );

        LOGBRUSH lb;
        lmemset( &lb, 0, sizeof( lb ) );
        lb.lbStyle = BS_SOLID;
        lb.lbColor = _rgclr[ pit->GetIdxClr( ) ];
        hbr = CreateBrushIndirect( &lb );
        if ( hbr == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
        SelectObject( hdc, hbr );
        
        int nCurMax = (int) _pdoc->GetCount( );    
        ASSERT( nCurMax >= 0 );
        int dxIncr  = (int) pwl->GetDxIncr( );
        ASSERT( dxIncr > 0 );
        
        int x, n, nCur;
        for ( x    = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
              n    = 0,
              nCur = m_nFirst;
                      
              nCur < nCurMax && n < m_nMax;

              x += dxIncr,
              n++,
              nCur++ )
            {
            // nCurNorm is normalized for the dataset
            int nCurNorm = nCur - pcache->m_dataset.nOffset;
            if ( nCurNorm >= 0 )
                {
                int y = Scale( nView, pcache->m_dataset.lpnum[ nCurNorm ] );
                if ( m_rgnumMinAdj[ nView ] > 0.0 )
                    {
                    if ( dxIncr == 1 )
                        {
                        MoveTo( hdc, x, y );
                        LineTo( hdc, x, m_rgviewinfo[ nView ].rcGrid.bottom );
                        }
                    else
                        {
                        Rectangle( hdc, x, y, x + dxIncr / 2, m_rgviewinfo[ nView ].rcGrid.bottom );
                        }
                    }
                else
                    {
                    int nGrids = pwl->m_rgvl[ nView ].GetGrids( );
                    ASSERT( nGrids >= 0 );
                    NUM numMaxAdj = m_rgnumMinAdj[ nView ] + m_rgnumStepAdj[ nView ] * nGrids;
                    if ( numMaxAdj < 0.0 )
                        {
                        if ( dxIncr == 1 )
                            {
                            MoveTo( hdc, x, y );
                            LineTo( hdc, x, m_rgviewinfo[ nView ].rcGrid.top );
                            }
                        else
                            {
                            Rectangle( hdc, x, y, x + dxIncr / 2, m_rgviewinfo[ nView ].rcGrid.top );
                            }
                        }
                    else
                        {
                        // Have to draw from/to zero line
                        int yZero = Scale( nView, 0.0 );
                        if ( dxIncr == 1 )
                            {
                            MoveTo( hdc, x, y );
                            LineTo( hdc, x, yZero );
                            }
                        else
                            {
                            Rectangle( hdc, x, y, x + dxIncr / 2, yZero );
                            }
                        }
                    }
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hbr );
        SafeDeleteObject( hpen );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hbr );
    SafeDeleteObject( hpen );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawLineIndicator( )
//
// Draws a line indicator.
//
// Parameters:
//
// pwl   - the window layout
// hdc    - the HDC
// nView  - the view number
// pcache - the indicator cache
// pit    - pointer to indicator template
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawLineIndicator( const QWindowLayout *pwl, HDC hdc, int nView, 
                                 const QCache *pcache, const QIndicatorTemplate *pit )
    {               
    HPEN hpen( NULL );
    SaveDC( hdc );
    TRY                  
        {             
        hpen = CreatePen( PS_SOLID, m_xLines, _rgclr[ pit->GetIdxClr( ) ] );
        if ( hpen == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }       
        SelectObject( hdc, hpen );
        
        int nCurMax = (int) _pdoc->GetCount( ) - 1;    
        ASSERT( nCurMax >= 0 );
        int dxIncr  = (int) pwl->GetDxIncr( );
        ASSERT( dxIncr > 0 );
        
        int x, n, nCur;
        for ( x    = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
              n    = 0,
              nCur = m_nFirst;
                      
              nCur < nCurMax && n < m_nMax - 1;

              x += dxIncr,
              n++,
              nCur++ )
            {
            // nCurNorm is normalized for the dataset
            int nCurNorm = nCur - pcache->m_dataset.nOffset;
            if ( nCurNorm >= 0 )
                {
                int y1 = Scale( nView, pcache->m_dataset.lpnum[ nCurNorm ] );
                int y2 = Scale( nView, pcache->m_dataset.lpnum[ nCurNorm + 1 ] );
                MoveTo( hdc, x, y1 );
                LineTo( hdc, x + dxIncr, y2 );
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpen );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpen );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawPointIndicator( )
//
// Draws a point indicator.
//
// Parameters:
//
// pwl    - the window layout
// hdc    - the HDC
// nView  - the view number
// pcache - the indicator cache
// pit    - pointer to indicator template
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawPointIndicator( const QWindowLayout *pwl, HDC hdc, int nView, 
                                  const QCache *pcache, const QIndicatorTemplate *pit )
    {               
    HBRUSH hbr ( NULL );
    HPEN   hpen( NULL );

    SaveDC( hdc );
    TRY                  
        {           
        hpen = CreatePen( PS_SOLID, m_xLines, _rgclr[ pit->GetIdxClr( ) ] );
        if ( hpen == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }       
        SelectObject( hdc, hpen );
        
        LOGBRUSH lb;
        lmemset( &lb, 0, sizeof( lb ) );
        lb.lbStyle = BS_SOLID;
        lb.lbColor = _rgclr[ pit->GetIdxClr( ) ];
        hbr = CreateBrushIndirect( &lb );
        if ( hbr == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
        SelectObject( hdc, hbr );
        
        int nCurMax = (int) _pdoc->GetCount( );    
        ASSERT( nCurMax >= 0 );
        int dxIncr  = (int) pwl->GetDxIncr( );
        ASSERT( dxIncr > 0 );
        
        int x, n, nCur;
        for ( x    = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
              n    = 0,
              nCur = m_nFirst;
                      
              nCur < nCurMax && n < m_nMax;

              x += dxIncr,
              n++,
              nCur++ )
            {
            // nCurNorm is normalized for the dataset
            int nCurNorm = nCur - pcache->m_dataset.nOffset;
            if ( nCurNorm >= 0 )
                {
                int y = Scale( nView, pcache->m_dataset.lpnum[ nCurNorm ] );
                BOOL f = Rectangle( hdc, x, y, x + 2, y - 2 );
                ASSERT( f );
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpen );
        SafeDeleteObject( hbr );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpen );
    SafeDeleteObject( hbr );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawHLC( )
//
// Draws hi-lo-close.
//
// Parameters:
//
// pwl   - the window layout
// hdc   - the HDC
// nView - the view number  
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawHLC( const QWindowLayout *pwl, HDC hdc, int nView )
    {
    HPEN hpen( NULL );
    SaveDC( hdc );       
    TRY      
        {
        if ( pwl->m_rgvl[ nView ].GetHLCV( ) )
            {                
            hpen = CreatePen( PS_SOLID, m_xLines, _rgclr[ _ptpl->m_genopts.GetHLCColor( ) ] );
            if ( hpen == NULL )
                {
                THROW_EXCEPTION( exGdi );
                }
            SelectObject( hdc, hpen );
            
            QQt *pqqt;
            int x, n;
            int dxIncr = (int) pwl->GetDxIncr( );
            ASSERT( dxIncr > 0 );
            for ( x = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
                  n = 0, 
                  pqqt = m_pqqtFirst;

                  pqqt && n < m_nMax;
    
                  x += dxIncr,
                  n++, 
                  pqqt = _pdoc->GetNext( pqqt ) )
                {
                int yHi = Scale( nView, pqqt->GetHi( ) );
                int yLo = Scale( nView, pqqt->GetLo( ) );
                int yCl = Scale( nView, pqqt->GetCl( ) );
                int yOp = Scale( nView, pqqt->GetOp( ) );
                
                MoveTo( hdc, x, yHi );
                LineTo( hdc, x, yLo );
                MoveTo( hdc, x, yCl );
                LineTo( hdc, x + dxIncr / 2 + 1, yCl );
                MoveTo( hdc, x, yOp );
                LineTo( hdc, x - ( dxIncr / 2 + 1 ), yOp );
                }
            }
        }
    CATCH_ALL
        {             
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpen );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpen );
    }
                
//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawSignals( )
//
// Draws signals.
//
// Parameters:
//
// pwl   - the window layout
// hdc   - the HDC
// nView - the view number  
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawSignals( const QWindowLayout *pwl, HDC hdc, int nView )
    {                                       
    LOGBRUSH lb;
    HPEN hpenBuy  ( NULL );
    HPEN hpenSell ( NULL );
    HBRUSH hbrBuy ( NULL );
    HBRUSH hbrSell( NULL );
    POINT  rgpt[ 3 ];
    int y;
    
    SaveDC( hdc );
    TRY      
        {
        if ( pwl->m_rgvl[ nView ].GetRule( ) )
            {
            hpenBuy = CreatePen( PS_SOLID, m_xLines, _rgclr[ _ptpl->m_genopts.GetBuyColor( ) ] );
            if ( hpenBuy == NULL )
                THROW_EXCEPTION( exGdi );
                
            hpenSell = CreatePen( PS_SOLID, m_xLines, _rgclr[ _ptpl->m_genopts.GetSellColor( ) ] );
            if ( hpenSell == NULL )
                THROW_EXCEPTION( exGdi );
                
            lmemset( &lb, 0, sizeof( lb ) );
            lb.lbStyle = BS_SOLID;
            lb.lbColor = _rgclr[ _ptpl->m_genopts.GetBuyColor( ) ];
            hbrBuy = CreateBrushIndirect( &lb );
            if ( hbrBuy == NULL )
                THROW_EXCEPTION( exGdi );

            lb.lbColor = _rgclr[ _ptpl->m_genopts.GetSellColor( ) ];
            hbrSell = CreateBrushIndirect( &lb );
            if ( hbrSell == NULL )
                THROW_EXCEPTION( exGdi );

            // if there's an attached rule...
            QSignal *psg = (QSignal *) m_rgviewinfo[ nView ].sglist.GetHead( );
                
            // we have to advance the signal to get the first one that
            // is on the screen.  Otherwise, the first signal is left
            // off the screen, and no signals show up.
            while ( psg && psg->GetDate( ) < m_pqqtFirst->GetDate( ) )
                {                            
                psg = (QSignal *) m_rgviewinfo[ nView ].sglist.GetNext( psg );
                }
            
            QQt *pqqt;
            int x, n;
            int dxIncr = (int) pwl->GetDxIncr( );
            ASSERT( dxIncr > 0 );
            for ( x = m_rgviewinfo[ nView ].rcGrid.left + dxIncr, n = 0, pqqt = m_pqqtFirst;
                  pqqt && n < m_nMax;
                  x += dxIncr, n++, pqqt = _pdoc->GetNext( pqqt ) )
                {
                if ( !psg )
                    {     
                    // we've reached the end of our signals
                    break;
                    }
                if ( psg->GetDate( ) == pqqt->GetDate( ) )
                    {
                    // Draw the actual signal                       
                    if ( psg->GetSg( ) == QSignal::sgBuy )
                        {
                        SelectObject( hdc, hpenBuy );
                        SelectObject( hdc, hbrBuy );
                        // upwards pointing triangle, y is at base
                        if ( pwl->m_rgvl[ nView ].GetHLCV( ) )
                            y = Scale( nView, pqqt->GetLo( ) ) - m_dySpace - m_dyHeight; 
                        else
                            y = m_rgviewinfo[ nView ].rcGrid.bottom + m_dySpace;
                        
                        y = max( m_rgviewinfo[ nView ].rcGrid.bottom + m_dySpace, y );

                        rgpt[ 0 ].x = x - m_dxBase;
                        rgpt[ 0 ].y = y;
                        rgpt[ 1 ].x = x + m_dxBase;
                        rgpt[ 1 ].y = y;
                        rgpt[ 2 ].x = x;
                        rgpt[ 2 ].y = y + m_dyHeight;
                        Polygon( hdc, rgpt, 3 );
                        }
                    else
                        {
                        SelectObject( hdc, hpenSell );
                        SelectObject( hdc, hbrSell );
                        // downwards pointing triangle, y is at base (at the top)
                        if ( pwl->m_rgvl[ nView ].GetHLCV( ) )
                            y = Scale( nView, pqqt->GetHi( ) ) + m_dySpace + m_dyHeight; 
                        else
                            y = m_rgviewinfo[ nView ].rcGrid.top - m_dySpace;
                        
                        y = min( m_rgviewinfo[ nView ].rcGrid.top - m_dySpace, y );
                        rgpt[ 0 ].x = x - m_dxBase;
                        rgpt[ 0 ].y = y;
                        rgpt[ 1 ].x = x + m_dxBase;
                        rgpt[ 1 ].y = y;
                        rgpt[ 2 ].x = x;
                        rgpt[ 2 ].y = y - m_dyHeight;
                        Polygon( hdc, rgpt, 3 );
                        }
                    psg = (QSignal *) m_rgviewinfo[ nView ].sglist.GetNext( psg );
                    }
                }
            }
        }    
    CATCH_ALL
        {    
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpenBuy );
        SafeDeleteObject( hpenSell );
        SafeDeleteObject( hbrBuy );
        SafeDeleteObject( hbrSell );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpenBuy  );
    SafeDeleteObject( hpenSell );
    SafeDeleteObject( hbrBuy   );
    SafeDeleteObject( hbrSell  );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawDates( )
//
// This routine will draw the dates at the bottom of the screen.
//
// Parameters:
//     pwl - The window layout.
//     hdc - The device context.
//
// Returns:
//    void.
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawDates( const QWindowLayout *pwl, HDC hdc, int nView )
    {
    if ( _ptpl->m_genopts.GetDateType( ) == dtNone )
        return;

    SaveDC( hdc );
    TRY
        {
        QQt *pqqt;
        QQt *pqqtPrev;
        int x, y, n;
        int dxIncr = (int) pwl->GetDxIncr( );
        ASSERT( dxIncr > 0 );

        y = m_rgviewinfo[ nView ].rcGrid.bottom;
        for ( x = m_rgviewinfo[ nView ].rcGrid.left + dxIncr,
              n = 0, 
              pqqtPrev = NULL,
              pqqt = m_pqqtFirst;

              pqqt && n < m_nMax;
    
              x += dxIncr,
              n++, 
              pqqtPrev = pqqt, pqqt = _pdoc->GetNext( pqqt ) )
            {
            if ( IsDrawDate( pqqtPrev, pqqt ) )
                {
                // draw the date here
                if ( pqqt->GetDate( ).GetMonth( ) == 1 ) 
                    {
                    CHAR sz[ 3 ];
                    // Draw the year here
                    int year = ( pqqt->GetDate( ).GetYear( ) % 100 );
                    wsprintf( sz, "%02d", year );
                    TextOut( hdc, x, y, sz, lstrlen( sz ) );
                    }
                else
                    {
                    // Draw the month here
                    static CHAR szMonths[ ] = "JFMAMJJASOND";
                    CHAR sz[ 2 ];
                    sz[ 0 ] = szMonths[ pqqt->GetDate( ).GetMonth( ) - 1 ];
                    sz[ 1 ] = '\0';
                    TextOut( hdc, x, y, sz, lstrlen( sz ) );
                    }
                }
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::IsDrawDate( )
//
// This routine will determine if it is appropriate
// to draw the dates, and their date lines.
//
// Parameters:
//     pqqtPrev - The previous quote.
//     pqqt     - The current quote.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::IsDrawDate( const QQt *pqqtPrev, const QQt *pqqt )
    {
    DateType dt = _ptpl->m_genopts.GetDateType( );
    if ( dt == dtNone )
        return FALSE;

    if ( pqqtPrev == NULL )
        return FALSE;

    ASSERT( pqqt != NULL );
    if ( dt == dtYears )
        {
        if ( pqqtPrev->GetDate( ).GetYear( ) != pqqt->GetDate( ).GetYear( ) )
            return TRUE;
        }
    else
        {
        if ( pqqtPrev->GetDate( ).GetMonth( ) != pqqt->GetDate( ).GetMonth( ) )
            return TRUE;
        }
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnDrawGrids( )
//
// Draws grids.
//
// Parameters:
//
// pwl   - the window layout
// hdc   - the HDC
// nView - the view number  
//
// Returns: void
//////////////////////////////////////////////////////////////////////////////
void QView::OnDrawGrids( const QWindowLayout *pwl, HDC hdc, int nView )
    {   
    HPEN  hpenSolid( NULL );
    HPEN  hpenDot  ( NULL );
    
    SaveDC( hdc );
    TRY
        {
        hpenSolid = CreatePen( PS_SOLID, m_xLines, _rgclr[ _ptpl->m_genopts.GetGridsColor( ) ] );
        if ( hpenSolid == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
            
        hpenDot = CreatePen( PS_SOLID, m_xLines, _rgclr[ _ptpl->m_genopts.GetGridsColor( ) ] );
        if ( hpenDot == NULL )
            {
            THROW_EXCEPTION( exGdi );
            }
        
        TextOut( hdc, 
            m_rgviewinfo[ nView ].ptTitle.x, 
            m_rgviewinfo[ nView ].ptTitle.y, 
            m_rgviewinfo[ nView ].sTitle,
            m_rgviewinfo[ nView ].sTitle.GetLength( ) );
        
        SelectObject( hdc, hpenSolid );
        SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
        Rectangle( hdc, 
                   m_rgviewinfo[ nView ].rcGrid.left, 
                   m_rgviewinfo[ nView ].rcGrid.top, 
                   m_rgviewinfo[ nView ].rcGrid.right, 
                   m_rgviewinfo[ nView ].rcGrid.bottom );
        
        // Get font width
        TEXTMETRIC tm;
        GetTextMetrics( hdc, &tm );
        int dxChar = tm.tmAveCharWidth;
        
        if ( m_rgviewinfo[ nView ].fAvail )
            {   
            int nGrids = (int) pwl->m_rgvl[ nView ].GetGrids( );
            ASSERT( nGrids > 0 );
            if ( pwl->m_rgvl[ nView ].GetHorz( ) )
                {    
                DWORD lw;
                NUM   num;
                int   i;
                int   x;    
                int   y;
                int   height;
                
                SelectObject( hdc, hpenDot );
                for ( num = m_rgnumMinAdj[ nView ], i = 0;
                      i <= nGrids;
                      num += m_rgnumStepAdj[ nView ], i++ )
                    {
                    CHAR sz[ _cbBufferMax + 1 ];
                    sprintf( sz, "%-6.3f", num );
                    lw = GetTextExtent( hdc, sz, lstrlen( sz ) );
                    height = HIWORD( lw );
                    x = m_rgviewinfo[ nView ].rcGrid.right + dxChar;
                    y = m_rgrgnScales[ nView ][ i ] + height / 2;
                    TextOut( hdc, x, y, sz, lstrlen( sz ) );
                    if ( ( i != 0 ) && ( i != nGrids ) )
                        {
                        hline( hdc, 
                               m_rgrgnScales[ nView ][ i ], 
                               m_rgviewinfo[ nView ].rcGrid.left,
                               m_rgviewinfo[ nView ].rcGrid.right );
                        }
                    }
                }
            NUM numMaxAdj = m_rgnumMinAdj[ nView ] + m_rgnumStepAdj[ nView ] * nGrids;
            if ( m_rgnumMinAdj[ nView ] < 0.0 && numMaxAdj > 0.0 )
                {
                int y = Scale( nView, 0.0 );
                SelectObject( hdc, hpenSolid );
                MoveTo( hdc, m_rgviewinfo[ nView ].rcGrid.left,  y );
                LineTo( hdc, m_rgviewinfo[ nView ].rcGrid.right, y );
                }
            }
        else
            {
            DWORD lw;
            int   width;
            int   height;
            int   x;
            int   y;
            QString s = GetString( IDS_UNAVAIL );
            
            lw = GetTextExtent( hdc, s, s.GetLength( ) );
            width  = LOWORD( lw );
            height = HIWORD( lw );
            x  = ( m_rgviewinfo[ nView ].rcGrid.right + m_rgviewinfo[ nView ].rcGrid.left ) / 2;
            x -= width / 2;
            y  = ( m_rgviewinfo[ nView ].rcGrid.top + m_rgviewinfo[ nView ].rcGrid.bottom ) / 2;
            y += height / 2;
            TextOut( hdc, x, y, s, s.GetLength( ) );
            }
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hpenSolid );
        SafeDeleteObject( hpenDot );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hpenSolid );
    SafeDeleteObject( hpenDot );
    }
//////////////////////////////////////////////////////////////////////////////
// QView::OnReSize( )
//
// Perform processing when the view is resized.
//////////////////////////////////////////////////////////////////////////////
void QView::OnReSize( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd )
    {
    OnReSizeGrids ( pwl, hwnd, hdc, rcClient, fScrollToEnd );
    OnReSizePrices( pwl, hwnd, hdc, rcClient, fScrollToEnd );
    OnReSizeV     ( pwl, hwnd, hdc, rcClient, fScrollToEnd );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::AdjustDown( )
//
// Round down based on numAdj.
//////////////////////////////////////////////////////////////////////////////
void QView::AdjustDown( NUM *pnumMin, const NUM numAdj )
    {
    *pnumMin /= numAdj;
    *pnumMin = (NUM) floor( *pnumMin );
    *pnumMin *= numAdj;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::CalculateRange( )
//
// Calculate the range of the view indexed on nIndex.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::CalculateRange( const QWindowLayout *pwl, int nIndex )
    {
    BOOL   fRet = FALSE;
    const  QViewLayout &vl    = pwl->m_rgvl  [ nIndex ];
    const  ViewInfo &viewinfo = m_rgviewinfo [ nIndex ];
    NUM   &m_numMinAbs        = m_rgnumMinAbs[ nIndex ];
    NUM   &m_numMaxAbs        = m_rgnumMaxAbs[ nIndex ];
    QQt   *pqqt = NULL;
    QQt   *pqqtFirst; 
    QIndicatorName *pname;
    int    n;
    int    nNorm;
    int    nFirst;
    int    nLast;
    
    m_numMinAbs =  NUM_MAX;
    m_numMaxAbs = -NUM_MAX;

    if ( _ptpl->m_genopts.Scaling( m_fPrintView ? 
         QGeneralOptions::ePrint : QGeneralOptions::eScreen ) == QGeneralOptions::eByPage )
        {
        pqqtFirst = m_pqqtFirst;
        nFirst    = m_nFirst;
        nLast     = min( m_nFirst + m_nMax, (int) _pdoc->GetCount( ) );
        }
    else
        {
        pqqtFirst = _pdoc->GetFirst( );
        nFirst    = 0;
        nLast     = _pdoc->GetCount( );
        }

    if ( vl.GetHLCV( ) )
        {
        for ( n = nFirst, pqqt = pqqtFirst; 
              n < nLast;
              n++, pqqt = _pdoc->GetNext( pqqt ) )
            {
            ASSERT( pqqt != NULL );
            if ( pqqt->GetHi( ) > m_numMaxAbs )
                {
                m_numMaxAbs = pqqt->GetHi( );
                }
            if ( pqqt->GetLo( ) < m_numMinAbs )
                {
                m_numMinAbs = pqqt->GetLo( );
                }
            fRet = TRUE;
            }
        }
    
    ASSERT( viewinfo.pdict != 0 );
    for ( pname = (QIndicatorName *) viewinfo.pdict->GetHead( );
          pname;
          pname = (QIndicatorName *) viewinfo.pdict->GetNext( pname ) )
        {
        const QCache *pcache = gcache.pcache( (QString) *pname, 0 );
        if ( !pcache )
            {
            break;
            }
        for ( n = nFirst; n < nLast; n++ )
            {
            nNorm = n - pcache->m_dataset.nOffset;
            ASSERT( (long) nNorm < (long) pcache->m_dataset.nPoints );
            if ( nNorm >= 0 )
                {
                if ( pcache->m_dataset.lpnum[ nNorm ] > m_numMaxAbs )
                    {
                    m_numMaxAbs = pcache->m_dataset.lpnum[ nNorm ];
                    }
                if ( pcache->m_dataset.lpnum[ nNorm ] < m_numMinAbs )
                    {
                    m_numMinAbs = pcache->m_dataset.lpnum[ nNorm ];
                    }
                fRet = TRUE;
                }
            }
        for ( n = 0; n < (int) pcache->m_datasetHorz.nPoints; n++ )
            {
            if ( pcache->m_datasetHorz.lpnum[ n ] > m_numMaxAbs )
                {
                m_numMaxAbs = pcache->m_datasetHorz.lpnum[ n ];
                }
            if ( pcache->m_datasetHorz.lpnum[ n ] < m_numMinAbs )
                {
                m_numMinAbs = pcache->m_datasetHorz.lpnum[ n ];
                }
            fRet = TRUE;
            }
        }
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::CalculateVolumeRange( )
//
// Calculate the volume range.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::CalculateVolumeRange( )
    {
    int  nFirst;
    int  nLast;
    int  n;
    QQt *pqqt;
    QQt *pqqtFirst;
    NUM  numVlMax = 1.0;

    if ( _ptpl->m_genopts.Scaling( m_fPrintView ? 
         QGeneralOptions::ePrint : QGeneralOptions::eScreen ) == QGeneralOptions::eByPage )
        {
        pqqtFirst = m_pqqtFirst;
        nFirst    = m_nFirst;
        nLast     = min( m_nFirst + m_nMax, (int) _pdoc->GetCount( ) );
        }
    else
        {
        pqqtFirst = _pdoc->GetFirst( );
        nFirst    = 0;
        nLast     = _pdoc->GetCount( );
        }

    for ( n = nFirst, pqqt = pqqtFirst; 
          n < nLast; 
          n++, pqqt = _pdoc->GetNext( pqqt ) )
        {
        ASSERT( pqqt != NULL );
        if ( pqqt->GetVl( ) > numVlMax )
            {
            numVlMax = pqqt->GetVl( );
            }
        }

    if ( numVlMax > 0.0 )
        {
        m_numVlMax = numVlMax;
        return TRUE;
        }
    // can't have negative volumes
    return FALSE;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnReSizeV( )
//
// Perform processing when the view is resized (WRT the volume).
//////////////////////////////////////////////////////////////////////////////
void QView::OnReSizeV( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd )
    {
    TASKLIST
        {
        for ( int i = 0; i < _cViewLayouts; i++ )
            {
            if ( !m_rgviewinfo[ i ].fPresent  ||
                 !pwl->m_rgvl[ i ].GetHLCV( ) ||
                 !m_rgviewinfo[ i ].fAvail )
                {
                continue;
                }

            const RECT &rcGrid = m_rgviewinfo[ i ].rcGrid;
            NUM  &m_numScaleVl = m_rgnumScaleVl[ i ];

            // This is not optimal, but it is simple
            if ( !CalculateVolumeRange( ) )
                {
                m_rgviewinfo[ i ].fAvail = FALSE;
                continue;
                }

            m_numScaleVl = ( rcGrid.top - rcGrid.bottom ) / m_numVlMax;
            }
        }
    ENDTASKLIST
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnReSizePrices( )
//
// Perform processing when the view is resized (WRT the prices).
//////////////////////////////////////////////////////////////////////////////
void QView::OnReSizePrices( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd )
    {
    NUM  rgnumMin[ _cViewLayouts ];
    NUM  rgnumMax[ _cViewLayouts ];
    NUM  num;
    int  i;
    int  j;
    
    TASKLIST
        {
        // if no data available, kill right away
        if ( !_pdoc->GetCount( ) )
            {
            for ( i = 0; i < _cViewLayouts; ++i )
                {
                m_rgviewinfo[ i ].fAvail = FALSE;
                }
            }
        
        /////////////////
        // Initialization
        lmemset( &m_rgnumMinAbs,  0, sizeof( m_rgnumMinAbs  ) );
        lmemset( &m_rgnumMaxAbs,  0, sizeof( m_rgnumMaxAbs  ) );
        lmemset( &m_rgnumScale,   0, sizeof( m_rgnumScale   ) );
        lmemset( &m_rgnumMinAdj,  0, sizeof( m_rgnumMinAdj  ) );
        lmemset( &m_rgnumStepAdj, 0, sizeof( m_rgnumStepAdj ) );

        for ( i = 0; i < _cViewLayouts; i++ )
            {
            if ( !m_rgviewinfo[ i ].fPresent ||
                 !m_rgviewinfo[ i ].fAvail )
                {
                continue;
                }
            
            const RECT &rcGrid = m_rgviewinfo[ i ].rcGrid;
            const int  &nGrids  = (int) pwl->m_rgvl[ i ].GetGrids( );
            ASSERT( nGrids > 0 );

            int *&m_rgnScales  = m_rgrgnScales [ i ];
            NUM  &numMin       = rgnumMin      [ i ];
            NUM  &numMax       = rgnumMax      [ i ];
            NUM  &m_numMinAbs  = m_rgnumMinAbs [ i ];
            NUM  &m_numMaxAbs  = m_rgnumMaxAbs [ i ];
            NUM  &m_numScale   = m_rgnumScale  [ i ];
            NUM  &m_numMinAdj  = m_rgnumMinAdj [ i ];
            NUM  &m_numStepAdj = m_rgnumStepAdj[ i ];
            
            if ( !CalculateRange( pwl, i ) )
                {
                m_rgviewinfo[ i ].fAvail = FALSE;
                continue;
                }

            if ( pwl->m_rgvl[ i ].GetRoundScale( ) )
                {
                for ( j = 0; 
                      j < sizeof( m_rgnum ) / sizeof( m_rgnum[ 0 ] ); 
                      j++ )
                    {
                    numMin = m_numMinAbs;
                    AdjustDown( &numMin, m_rgnum[ j ] );
 
                    if ( numMin + m_rgnum[ j ] * nGrids >= m_numMaxAbs )
                        {
                        m_numMinAdj = numMin;
                        m_numStepAdj = m_rgnum[ j ];
                        numMax = m_numStepAdj * nGrids + numMin;
                        break; // j
                        }
                    }
                if ( j == sizeof( m_rgnum ) / sizeof( m_rgnum[ 0 ] ) )
                    {
                    m_rgviewinfo[ i ].fAvail = FALSE;
                    continue;
                    }
                }
            else
                {
                numMin       = m_numMinAbs;
                numMax       = m_numMaxAbs;
                m_numMinAdj  = numMin;
                m_numStepAdj = ( numMax - m_numMinAdj ) / nGrids;
                }

            if ( m_rgpfnScale[ i ] == ScaleLog &&
                 numMin <= 0.0 )
                {
                m_rgviewinfo[ i ].fAvail = FALSE;
                continue;
                }

            if ( numMax == numMin )
                {
                // This can also divide by 0, below
                m_rgviewinfo[ i ].fAvail = FALSE;
                continue;
                }

            m_numScale = ( rcGrid.top - rcGrid.bottom ) /
                         ( m_rgpfnScale[ i ]( numMax ) - m_rgpfnScale[ i ]( numMin ) );

            for ( j = 0, num = m_numMinAdj;
                  j <= nGrids;
                  j++, num += m_numStepAdj )
                {
                m_rgnScales[ j ] = Scale( i, num );
                }
            }
        }
    ENDTASKLIST
    }

//////////////////////////////////////////////////////////////////////////////
// QView::Scale( )
//
// Scales a number onto the grid.  This routine assumes the following
// members have been initialized:
//
// m_rgviewinfo
// m_rgnumMinAdj
// m_rgnumScale
// m_rgpfnScale
//
// This implies you cannot use this routine until these have been 
// calculated.
//////////////////////////////////////////////////////////////////////////////
int QView::Scale( int nView, NUM num )
    {
    RECT &rcGrid      = m_rgviewinfo[ nView ].rcGrid;
    NUM  &m_numScale  = m_rgnumScale [ nView ];
    NUM  &m_numMinAdj = m_rgnumMinAdj[ nView ];

    return rcGrid.bottom + (int)
           ( m_numScale * ( m_rgpfnScale[ nView ]( num ) - m_rgpfnScale[ nView ]( m_numMinAdj ) ) );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnReSizeGrids( )
//
// Perform processing when the view is resized (WRT the grids).
//
// Parmeters:
//
// pwl      - the window layout
// hwnd     - HWND, if a screen; NULL o/w
// hdc      - the HDC
// rcClient - the client RC
//
// Returns:
//
// void
//////////////////////////////////////////////////////////////////////////////
void QView::OnReSizeGrids( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd )
    {
    HFONT hfont( NULL );
    
    SaveDC( hdc );
    TRY
        {
        LOGFONT    lf;
        lmemset( &lf, 0, sizeof( lf ) );
        int dyFont          = _ptpl->m_genopts.GetFontSize( );
        lf.lfWeight         = FW_NORMAL;
        lf.lfHeight         = dyFont;
        lstrcpy( lf.lfFaceName, _pszFontName );
        hfont               = CreateFontIndirect( &lf );
        SelectObject( hdc, hfont );
        
        //////////////////////////////
        // Rebuild the points and rc's
        
        // dyGridsOnly: the size of the client window, minus
        // a dyfont for each title, minus one extra dyfont at
        // the bottom
        int dyGridsOnly = rcClient.top - rcClient.bottom - dyFont * m_nViews;
        dyGridsOnly    -= dyFont;
        int yCur        = rcClient.top;
        
        TEXTMETRIC tm;
        GetTextMetrics( hdc, &tm );
        int dxChar = tm.tmAveCharWidth;
        int dyChar = tm.tmHeight + tm.tmExternalLeading;
        for( int i = 0; i < _cViewLayouts; i++ )
            {         
            if ( !m_rgviewinfo[ i ].fPresent )
                {
                continue;
                }

            ////////////////////////////
            // Calculate the title point
            m_rgviewinfo[ i ].ptTitle.x = rcClient.left + dxChar;
            m_rgviewinfo[ i ].ptTitle.y = yCur;
            yCur -= dyFont;

            ///////////////////
            // Calculate the rc
            int dyRect  = (int) ( ( (long) pwl->m_rgvl[ i ].GetPercent( ) * (long) dyGridsOnly ) / 100L );
            int yBottom = yCur - dyRect;
            SetRect( &m_rgviewinfo[ i ].rcGrid, 
                     rcClient.left + dxChar, 
                     yCur, 
                     rcClient.right - dxChar * ( m_nCharsRight + 1 ), 
                     yBottom );
            yCur = yBottom;
            }

        ///////////////////////
        // Calculate the m_nMax
        for( i = 0; i < _cViewLayouts; i++ )
            {         
            if ( !m_rgviewinfo[ i ].fPresent )
                {
                continue;
                }
            break; // i will now index the present view
            }
        ASSERT( i < _cViewLayouts );

        int dxIncr = (int) pwl->GetDxIncr( );
        ASSERT( dxIncr > 0 );
        m_nMax = ( m_rgviewinfo[ i ].rcGrid.right - m_rgviewinfo[ i ].rcGrid.left ) / dxIncr - 1;
        if ( hwnd )
            {
            SetScrollRange( hwnd, SB_HORZ, 0, _pdoc->GetCount( ) - 1, FALSE );
            }
        if ( fScrollToEnd )
            end( hwnd );
        }
    CATCH_ALL
        {
        RestoreDC( hdc, -1 );
        SafeDeleteObject( hfont );
        THROW_LAST( );
        }
    END_CATCH
    RestoreDC( hdc, -1 );
    SafeDeleteObject( hfont );
    }

//////////////////////////////////////////////////////////////////////////////
// QView::operator--( )
//
// Convenient shorthand notation to decrement m_nFirst and m_pqqtFirst.
//////////////////////////////////////////////////////////////////////////////
void QView::operator--( int )
    {
    ASSERT( m_nFirst > 0 );
    ASSERT( (long) m_nFirst <= (long) _pdoc->GetCount( ) - 1 );

    m_pqqtFirst = _pdoc->GetPrev( m_pqqtFirst );
    m_nFirst--;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::operator++( )
//
// Convenient shorthand notation to increment m_nFirst and m_pqqtFirst.
//////////////////////////////////////////////////////////////////////////////
void QView::operator++( int )
    {
    ASSERT( m_nFirst >= 0 );
    ASSERT( (long) m_nFirst < (long) _pdoc->GetCount( ) - 1 );

    m_pqqtFirst = _pdoc->GetNext( m_pqqtFirst );
    m_nFirst++;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::preparePage( )
//
// This routine performs the following actions to prepare a page
// for printing:
//     -sets the DC mapping modes, viewports, etc.
//     -fills in the PrintStruct structure ps
//     -calls OnReSize using the calculated RC's,
//      and using the fScrollToEnd parameter.
//////////////////////////////////////////////////////////////////////////////
void QView::preparePage( PrintStruct &ps, BOOL fScrollToEnd )
    {
    TRY
        {
        SetMapMode( ps.hdc, MM_LOENGLISH );
        int dxPage = GetDeviceCaps( ps.hdc, HORZRES );
        int dyPage = GetDeviceCaps( ps.hdc, VERTRES );
        SetRect( &ps.rcPage, 0, 0, dxPage, dyPage ); 
        SetViewportOrg( ps.hdc, 0, ps.rcPage.bottom );
        DPtoLP( ps.hdc, (LPPOINT) &ps.rcPage, 2 );

        // MM_LOENGLISH is in 1/100's of an inch
        int top    = (int) ( ps.numTop    * (NUM) 100 );
        int left   = (int) ( ps.numLeft   * (NUM) 100 );
        int bottom = (int) ( ps.numBottom * (NUM) 100 );
        int right  = (int) ( ps.numRight  * (NUM) 100 );

        if ( top < 0 || bottom < 0 || left < 0 || right < 0 )
                THROW_EXCEPTION( exGeneric );
                
        ps.rcPage.left   += left;
        ps.rcPage.right  -= right;
        ps.rcPage.top    -= top;
        ps.rcPage.bottom += bottom;

        // Also, leave room for the file Title.
        // Note: the file title is drawn separately,
        // and invisible, the main VIEW code.
        ps.rcPage.top -= ( 2 * _ptpl->m_genopts.GetFontSize( ) );

        // MM_LOENGLISH grows upwards: bottom must < top
        if ( ps.rcPage.left >= ps.rcPage.right || ps.rcPage.bottom >= ps.rcPage.top )
            THROW_EXCEPTION( exGeneric );

        OnReSize( ps.pwl, NULL, ps.hdc, ps.rcPage, fScrollToEnd );
        }
    CATCH_ALL
        {
        THROW_LAST( );
        }
    END_CATCH
    }

//////////////////////////////////////////////////////////////////////////////
// QView::iterate( )
//
// This routine will call a user-defined function for
// every page of the view.
//
// preparePage( ) must be called before iterate( ) can be used.
//////////////////////////////////////////////////////////////////////////////
void QView::iterate( HWND hwnd, void (*pfn)( QView *, int nPage, PrintStruct& ), PrintStruct &ps )
    {
    int nPage( 1 );

    while ( TRUE )
        {
        pfn( this, nPage, ps );
        if ( (long) ( m_nFirst + m_nMax ) >= (long) ( _pdoc->GetCount( ) ) )
            break;
        for ( int i = 0; i < m_nMax; i++ )
            (*this)++;
        nPage++;
        preparePage( ps, FALSE );
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QView::gotopos( )
//
// This routine goes to the indicated scroll bar position.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::gotopos( HWND hwnd, int nPos )
    {
    if ( !_pdoc->GetCount( ) )
        return FALSE;
    ASSERT( nPos >= 0 );
    ASSERT( (long) nPos < (long) _pdoc->GetCount( ) );
    
    // go home
    m_pqqtFirst = _pdoc->GetFirst( );
    m_nFirst    = 0;        
    
    while ( nPos > 0 )
        {
        (*this)++;
        nPos--;
        }
    if ( hwnd )        
        SetScrollPos( hwnd, SB_HORZ, m_nFirst, TRUE );
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::home( )
//
// This routine sets the view pointers to home.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::home( HWND hwnd )
    {
    if ( !_pdoc->GetCount( ) )
        return FALSE;
    ASSERT( _pdoc->GetCount( ) > 0u );

    m_pqqtFirst = _pdoc->GetFirst( );
    m_nFirst    = 0;        
    if ( hwnd )
        SetScrollPos( hwnd, SB_HORZ, m_nFirst, TRUE );
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::end( )
//
// This routine sets the view pointers to end.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::end( HWND hwnd )
    {
    if ( !_pdoc->GetCount( ) )
        return FALSE;
    ASSERT( _pdoc->GetCount( ) > 0u );

    int n = m_nMax - 1;
    
    m_nFirst    = _pdoc->GetCount( ) - 1;
    m_pqqtFirst = _pdoc->GetLast( );

    while ( n > 0 && m_nFirst > 0 )
        {
        (*this)--;
        n--;
        }
    if ( hwnd )
        SetScrollPos( hwnd, SB_HORZ, m_nFirst, TRUE );
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::left( )
//
// This routine sets the view pointers 1/m_nShift of a screen to the left.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::left( HWND hwnd )
    {
    if ( !_pdoc->GetCount( ) )
        return FALSE;
    BOOL fRet = FALSE;
    ASSERT( _pdoc->GetCount( ) > 0u );
    
    int n = max( m_nMax / m_nShift, 1 );
    
    while ( n > 0 && m_nFirst > 0 )
        {
        (*this)--;
        n--;          
        fRet = TRUE;
        }           
    if ( hwnd )
        SetScrollPos( hwnd, SB_HORZ, m_nFirst, TRUE );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::right( )
//
// This routine sets the view pointers 1/m_nShift of a screen to the right.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::right( HWND hwnd )
    {           
    if ( !_pdoc->GetCount( ) )
        return FALSE;
    BOOL fRet = FALSE;
    ASSERT( _pdoc->GetCount( ) > 0u );
    
    int n = max( m_nMax / m_nShift, 1 );
    
    while ( n > 0 && (long) m_nFirst < (long) _pdoc->GetCount( ) - 1 )
        {
        (*this)++;
        n--;          
        fRet = TRUE;
        }           
    if ( hwnd )
        SetScrollPos( hwnd, SB_HORZ, m_nFirst, TRUE );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::pageleft( )
//
// This routine sets the view pointers one page left.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::pageleft( HWND hwnd )
    {           
    if ( !_pdoc->GetCount( ) )
        return FALSE;
    BOOL fRet = FALSE;
    ASSERT( _pdoc->GetCount( ) > 0u );
    
    int n = m_nMax;
    
    while ( n > 0 && m_nFirst > 0 )
        {
        (*this)--;
        n--;          
        fRet = TRUE;
        }           
    if ( hwnd )
        SetScrollPos( hwnd, SB_HORZ, m_nFirst, TRUE );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::pageright( )
//
// This routine sets the view pointers one page right.
//////////////////////////////////////////////////////////////////////////////
BOOL QView::pageright( HWND hwnd )
    {           
    if ( !_pdoc->GetCount( ) )
        return FALSE;
    BOOL fRet = FALSE;
    ASSERT( _pdoc->GetCount( ) > 0u );
    
    int n = m_nMax;
    
    while ( n > 0 && (long) m_nFirst < (long) _pdoc->GetCount( ) - 1 )
        {
        (*this)++;
        n--;          
        fRet = TRUE;
        }           
    if ( hwnd )
        SetScrollPos( hwnd, SB_HORZ, m_nFirst, TRUE );
    return fRet;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::OnReBuild( )
//
// Rebuild the view including all cached indicators.
//////////////////////////////////////////////////////////////////////////////
void QView::OnReBuild( const QWindowLayout *pwl )
    {
    Cleanup( );
    ////////////////////////////
    // Rebuild the fPresent flag
    for( int i = 0; i < _cViewLayouts; i++ )
        {
        if ( pwl->m_rgvl[ i ].GetPercent( ) )
            {
            m_rgviewinfo[ i ].fPresent = TRUE;
            m_nViews++;
            }
        }
    ASSERT( m_nViews > 0 );

    /////////////////////
    // Rebuild the titles
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        if ( !m_rgviewinfo[ i ].fPresent || !m_rgviewinfo[ i ].fAvail )
            {
            continue;
            }
        m_rgviewinfo[ i ].sTitle = pwl->m_rgvl[ i ].GetTitle( );
        }
    
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        if ( !m_rgviewinfo[ i ].fPresent || !m_rgviewinfo[ i ].fAvail )
            {
            continue;
            }
        
        // keep track of the dictionary of included indicators
        m_rgviewinfo[ i ].pdict = &pwl->m_rgvl[ i ].m_dict;
        
        // pretend to access the cache.  What we want to avoid is
        // ending up in OnDrawPrices with an invalid cache.
        for ( QIndicatorName *pname = (QIndicatorName *) m_rgviewinfo[ i ].pdict->GetHead( );
              pname;
              pname = (QIndicatorName *) m_rgviewinfo[ i ].pdict->GetNext( pname ) )
            {
            if ( !gcache.pcache( (QString) *pname, 0 ) )
                {
                m_rgviewinfo[ i ].fAvail = FALSE;
                }
            }  
        }
        
    /////////////////////////////
    // Handle the scaling pointer
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        if ( !m_rgviewinfo[ i ].fPresent || !m_rgviewinfo[ i ].fAvail )
            {
            continue;
            }

        if ( pwl->m_rgvl[ i ].GetLogScale( ) )
            {
            m_rgpfnScale[ i ] = ScaleLog;
            }
        else
            {
            m_rgpfnScale[ i ] = ScaleLinear;
            }
    
        int nGrids = pwl->m_rgvl[ i ].GetGrids( );
        m_rgrgnScales[ i ] = new int[ nGrids + 1 ];
        if ( !m_rgrgnScales[ i ] )
            {                                
            // The view will be destroyed as well, but we have to do this:
            m_rgviewinfo[ i ].fAvail = FALSE;
            THROW_EXCEPTION( exMemory );
            }
        }
    
    ///////////////////
    // Handle the Rules
    //
    // Note: may wish to consider using the cache
    // here, rather than calling prule->Evaluate( ) directly
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        if ( !m_rgviewinfo[ i ].fPresent || !m_rgviewinfo[ i ].fAvail )
            {
            continue;
            }
        
        const CHAR *pch = pwl->m_rgvl[ i ].GetRule( );
        if ( pch )
            {
            // if there's an attached rule...
            QRule *prule = _ptpl->m_rulelist.Find( pch );
            ASSERT( prule != NULL );
            if ( !prule->Evaluate( m_rgviewinfo[ i ].sglist, FALSE, FALSE, 0 ) )
                {
                m_rgviewinfo[ i ].fAvail = FALSE;
                }
            }
        }

    ////////////////////////
    // Handle the Conditions
    for ( i = 0; i < _cViewLayouts; i++ )
        {
        if ( !m_rgviewinfo[ i ].fPresent || !m_rgviewinfo[ i ].fAvail )
            continue;
            
        const CHAR *psz = pwl->m_rgvl[ i ].GetCondition( );
        if ( psz )
            {
            // pretend to access the cache.  What we want to avoid is
            // ending up in OnDrawPrices with an invalid cache.
            if ( !gcache.pcondcache( psz, 0 ) )
                m_rgviewinfo[ i ].fAvail = FALSE;
            }
        }
    }

//////////////////////////////////////////////////////////////////////////////
// QView::GetLogicalBorders( )
//
// This routine returns (in logical units) the left and right borders
// of the view.  If the quotes don't fill up the entire view, the
// right border returned is where the quotes end.
//////////////////////////////////////////////////////////////////////////////
void QView::GetLogicalBorders( const QWindowLayout *pwl, int *pxLeft, int *pxRight )
    {
    int i;
    int xLeft;
    int xRight;
    int dxIncr;
    int nQuotes;

    /////////////////
    // Initialization
    dxIncr = (int) pwl->GetDxIncr( );

    for ( i = 0; i < _cViewLayouts; i++ )
        {
        if ( !m_rgviewinfo[ i ].fPresent )
            {
            continue;
            }
        break; // i will now index the present view
        }
    ASSERT( i < _cViewLayouts );

    xLeft   = m_rgviewinfo[ i ].rcGrid.left + dxIncr;
    nQuotes = min( (int) _pdoc->GetCount( ) - m_nFirst, m_nMax );
    xRight  = xLeft + nQuotes * dxIncr;
    ASSERT( xLeft <= xRight );
    *pxLeft  = xLeft;
    *pxRight = xRight;
    }

//////////////////////////////////////////////////////////////////////////////
// QView::GetFirstPqqt( )
//
// This routine returns the first quote pointer.
//////////////////////////////////////////////////////////////////////////////
QQt *QView::GetFirstPqqt( )
    {
    return m_pqqtFirst;
    }

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
// QView::AssertValid( )
//
// Standard AssertValid routine.
//////////////////////////////////////////////////////////////////////////////
void QView::AssertValid( ) const
    {
    QObject::AssertValid( );
    for ( int i = 0; i < _cViewLayouts; i++ )
        {
        m_rgviewinfo[ i ].sTitle.AssertValid( );
        m_rgviewinfo[ i ].sglist.AssertValid( );
        if ( m_rgviewinfo[ i ].pdict )
            {
            m_rgviewinfo[ i ].pdict->AssertValid( );
            }
        }
    }
#endif
