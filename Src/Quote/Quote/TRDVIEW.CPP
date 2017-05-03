#include <qobject.hxx>
#include <stdio.h>
#include <statpak.h>
#include <boxctrl.h>
#include "types.hxx"
#include "template.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "perfeng.hxx"
#include "trdview.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

#define clrDkGray RGB( 0x40, 0x40, 0x40 )
#define clrRed    RGB( 0xFF, 0x00, 0x00 )
#define clrGreen  RGB( 0x00, 0x80, 0x00 )
static const CHAR szSpacer[ ] = "  ";

//////////////////////////////////////////////////////////////////////////////
// QTradeView methods
//////////////////////////////////////////////////////////////////////////////

QTradeView::QTradeView( BOOL fPerformanceTester, HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids )
    :ScrollView( hwnd, model, dyTitle, dyLines, hpenGrids )
{
    fPerformanceTester_ = fPerformanceTester;
    // Prepare the LOGFONT structure
    LOGFONT lf;
    lmemset( &lf, 0, sizeof( lf ) );
    lf.lfWeight         = FW_NORMAL;
    lf.lfHeight         = dyLines;
    lstrcpy( lf.lfFaceName, _pszFontName );
    
    // Create the font used by this view
    hfont_ = CreateFontIndirect( &lf );
    ASSERT( hfont_ );
    
    // Get average character width
    BOOL f;
    HDC hdc;
    TEXTMETRIC tm;
    HDC hfontOld;
    lmemset( &tm, 0, sizeof( tm ) );
    hdc = GetDC( hwnd );
    hfontOld = SelectObject( hdc, hfont_ );
    f = GetTextMetrics( hdc, &tm );
    ASSERT( f );
    SelectObject( hdc, hfontOld );
    ReleaseDC( hwnd, hdc );
    aveWidth_ = tm.tmAveCharWidth;

    // Create the columns used by this view
    f = ScrollView::addColumn( "Security",   aveWidth_ * ( lstrlen( "XXXXXXXXXX.XXX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Date",       aveWidth_ * ( lstrlen( "    XX/XX/XXXX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Signal",     aveWidth_ * ( lstrlen( "XXXXXXXXXXXXXX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Price",      aveWidth_ * ( lstrlen( "XXXXXXXXXXX.XX" ) ) );
    ASSERT( f );

    if ( fPerformanceTester_ )
        {
        f = ScrollView::addColumn( "Bars",       aveWidth_ * ( lstrlen( "XXXXXXXXXXXXXX" ) ) );
        ASSERT( f );
        }

    f = ScrollView::addColumn( "Net Profit", aveWidth_ * ( lstrlen( "XXXXXXXXXXX.XX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( ( fPerformanceTester_ ? "Drawdown" : "Cash Balance" ), 0 );
    ASSERT( f );
}

QTradeView::~QTradeView( )
{
    Cleanup( );
}

void QTradeView::Cleanup( )
{
    DeleteObject( hfont_ );
}

void QTradeView::draw( HDC hdc, const RECT &rcPaint ) const
{
    SetBkMode( hdc, TRANSPARENT );
    SelectObject( hdc, hfont_ );
    
    ScrollView::draw( hdc, rcPaint );
}

void QTradeView::drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const
{
    RECT rcDraw;
    int  x, w;
    int  col;
    
    // get the element
    QTrade *pTrade = (QTrade *) elem;
    CHAR sz[ _cbBufferMax + 1 ];

    // Draw the horizontal grid lines
    SelectObject( hdc, hpenGrids_ );
    Rectangle( hdc, rcModelElement.left,  rcModelElement.bottom,
                    rcModelElement.right, rcModelElement.top + 1 );

    // Draw the Security column
    SetTextColor( hdc, clrDkGray );
    ScrollView::getColumn( 0, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    const CHAR *pszSecurity = (const CHAR *) pTrade->security( );
    DrawText( hdc, pszSecurity, lstrlen( pszSecurity ), &rcDraw, DT_RIGHT | DT_VCENTER );

    // Draw the Date column
    sprintf( sz, "%02d/%02d/%4d", pTrade->date( ).GetMonth( ),
                                  pTrade->date( ).GetDay( ), 
                                  pTrade->date( ).GetYear( ) );
    ScrollView::getColumn( 1, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );
    
    // Draw the Type column
    ScrollView::getColumn( 2, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    const CHAR *pszType = ( pTrade->signalType( ) == QSignal::sgBuy ? "Buy" : "Sell" );
    DrawText( hdc, pszType, lstrlen( pszType ), &rcDraw, DT_RIGHT | DT_VCENTER );

    // Draw the Price column
    ScrollView::getColumn( 3, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    sprintf( sz, "%.2f", pTrade->price( ) );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );

    if ( pTrade->signalType( ) == QSignal::sgSell )
    {
        if ( fPerformanceTester_ )
        {
            // Draw the Bars column
            ScrollView::getColumn( 4, x, w );
            SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
            sprintf( sz, "%lu", pTrade->barsSinceEntry( ) );
            DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );
        }

        // Draw the Profit column
        if ( pTrade->profit( ) > (NUM) 0 )
        {
            SetTextColor( hdc, clrGreen );
            sprintf( sz, "+%.2f", pTrade->profit( ) );
        }
        else if ( pTrade->profit( ) < (NUM) 0 )
        {
            SetTextColor( hdc, clrRed );
            sprintf( sz, "-%.2f", -pTrade->profit( ) );
        }
        else
        {
            sprintf( sz, "%.2f", pTrade->profit( ) );
        }

        col = ( fPerformanceTester_ ? 5 : 4 );
        ScrollView::getColumn( col, x, w );
        SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
        DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );
    }

    // Draw the Drawdown/cash balance column
    SetTextColor( hdc, clrDkGray );
    col = ( fPerformanceTester_ ? 6 : 5 );
    ScrollView::getColumn( col, x, w );
    int width = aveWidth_ * ( lstrlen( "XXXXXXXXXXX.XX" ) );
    SetRect( &rcDraw, x, rcModelElement.top, x + width, rcModelElement.bottom );
    if ( fPerformanceTester_ )
        sprintf( sz, "%.2f", pTrade->drawdown( ) );
    else
        sprintf( sz, "%.2f", pTrade->cashBalance( ) );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );
}

void QTradeView::saveText( FILE *pfile )
    {
    CHAR sz[ _cbBufferMax + 1 ];
    QTrade *pTrade;

    if ( fPerformanceTester_ )
    {
        fprintf( pfile, "%14s%14s%14s%14s%14s%14s%14s\n", "Security", "Date", "Signal", "Price", "Bars", "Net Profit", "Drawdown" );
        fprintf( pfile, "==================================================================================================\n" );
    }
    else
    {
        fprintf( pfile, "%14s%14s%14s%14s%14s%14s\n", "Security", "Date", "Signal", "Price", "Net Profit", "Cash Balance" );
        fprintf( pfile, "====================================================================================\n" );
    }
    
    for ( pTrade = (QTrade *) model_->GetHead( );
          pTrade;
          pTrade = (QTrade *) model_->GetNext( pTrade ) )
        {
        fprintf( pfile, "%14s", (const CHAR *) pTrade->security( ) );
        fprintf( pfile, "    %02d/%02d/%4d", 
            pTrade->date( ).GetMonth( ),
            pTrade->date( ).GetDay( ), 
            pTrade->date( ).GetYear( ) );
        const CHAR *pszType = ( pTrade->signalType( ) == QSignal::sgBuy ? "Buy" : "Sell" );
        fprintf( pfile, "%14s", pszType );
        fprintf( pfile, "%14.2f", pTrade->price( ) );

        if ( pTrade->signalType( ) == QSignal::sgBuy )
            {
            if ( fPerformanceTester_ )
                fprintf( pfile, "              " ); // bars since entry
            fprintf( pfile, "              " ); // net profit
            }
        else
            {
            if ( fPerformanceTester_ )
                fprintf( pfile, "%14lu", pTrade->barsSinceEntry( ) );

            if ( pTrade->profit( ) > (NUM) 0 )
                sprintf( sz, "+%.2f", pTrade->profit( ) );
            else if ( pTrade->profit( ) < (NUM) 0 )
                sprintf( sz, "-%.2f", -pTrade->profit( ) );
            else
                fprintf( pfile, "%14.2f", pTrade->profit( ) );
                
            fprintf( pfile, "%14s", sz );
            }

        if ( fPerformanceTester_ )
            fprintf( pfile, "%14.2f", pTrade->drawdown( ) );
        else
            fprintf( pfile, "%14.2f", pTrade->cashBalance( ) );
        fprintf( pfile, "\n" );
        }
    }

