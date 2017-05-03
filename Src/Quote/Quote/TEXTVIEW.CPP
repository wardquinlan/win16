#include <qobject.hxx>
#include <stdio.h>
#include <statpak.h>
#include <boxctrl.h>
#include "types.hxx"
#include "template.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "textview.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

#define clrDkGray RGB( 0x40, 0x40, 0x40 )
#define clrRed    RGB( 0xFF, 0x00, 0x00 )
#define clrGreen  RGB( 0x00, 0x80, 0x00 )
static const CHAR szSpacer[ ] = "  ";

//////////////////////////////////////////////////////////////////////////////
// QTextView methods
//////////////////////////////////////////////////////////////////////////////

QTextView::QTextView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids )
    :ScrollView( hwnd, model, dyTitle, dyLines, hpenGrids )
{
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
    f = ScrollView::addColumn( "Date",    aveWidth_ * ( 1 + lstrlen( "XX/XX/XXXX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Close",   aveWidth_ * ( lstrlen( "XXXXXXX.XX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Chg",     aveWidth_ * ( lstrlen( "XXXXXXX.XX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Chg%",    aveWidth_ * ( lstrlen( "XXXXXX.XX%" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Open",    aveWidth_ * ( lstrlen( "XXXXXXX.XX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "High",    aveWidth_ * ( lstrlen( "XXXXXXX.XX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Low",     aveWidth_ * ( lstrlen( "XXXXXXX.XX" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Volume",  0 );
    ASSERT( f );
}

QTextView::~QTextView( )
{
    Cleanup( );
}

void QTextView::Cleanup( )
{
    DeleteObject( hfont_ );
}

void QTextView::draw( HDC hdc, const RECT &rcPaint ) const
{
    SetBkMode( hdc, TRANSPARENT );
    SelectObject( hdc, hfont_ );
    
    ScrollView::draw( hdc, rcPaint );
}

void QTextView::drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const
{
    RECT rcDraw;
    int  x, w;

    // get the element
    QQt *pqqt = (QQt *) elem;
    CHAR sz[ _cbBufferMax + 1 ];

    // Draw the horizontal grid lines
    SelectObject( hdc, hpenGrids_ );
    Rectangle( hdc, rcModelElement.left,  rcModelElement.bottom,
                    rcModelElement.right, rcModelElement.top + 1 );

    // Draw the Date column
    SetTextColor( hdc, clrDkGray );
    sprintf( sz, "%02d/%02d/%4d", pqqt->GetDate( ).GetMonth( ),
                                  pqqt->GetDate( ).GetDay( ), 
                                  pqqt->GetDate( ).GetYear( ) );
    ScrollView::getColumn( 0, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );
    
    // Draw the Close column
    sprintf( sz, "%.2f", pqqt->GetCl( ) );
    ScrollView::getColumn( 1, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );

    // Draw the Change columns
    QQt *pqqtPrev = _pdoc->GetPrev( pqqt );
    if ( pqqtPrev )
        {
        NUM numChange = pqqt->GetCl( ) - pqqtPrev->GetCl( );
        if ( numChange > (NUM) 0 )
            {
            SetTextColor( hdc, clrGreen );
            sprintf( sz, "+%.2f", numChange );
            }
        else if ( numChange < (NUM) 0 )
            {
            SetTextColor( hdc, clrRed );
            sprintf( sz, "-%.2f", -numChange );
            }
        else
            {
            sprintf( sz, "%.2f", numChange );
            }
        ScrollView::getColumn( 2, x, w );
        SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
        DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );

        NUM numPct = numChange / pqqtPrev->GetCl( ) * (NUM) 100;
        if ( numPct > (NUM) 0 )
            sprintf( sz, "+%.2f%%", numPct );
        else if ( numPct < (NUM) 0 )
            sprintf( sz, "-%.2f%%", -numPct );
        else
            sprintf( sz, "%.2f%%", numPct );
        
        ScrollView::getColumn( 3, x, w );
        SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
        DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );
        }

    // Draw the Open column
    SetTextColor( hdc, clrDkGray );
    sprintf( sz, "%.2f", pqqt->GetOp( ) );
    ScrollView::getColumn( 4, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );

    // Draw the High column
    SetTextColor( hdc, clrDkGray );
    sprintf( sz, "%.2f", pqqt->GetHi( ) );
    ScrollView::getColumn( 5, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );

    // Draw the Low column
    sprintf( sz, "%.2f", pqqt->GetLo( ) );
    ScrollView::getColumn( 6, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );

    // Draw the Volume column
    sprintf( sz, "%.0f", pqqt->GetVl( ) );
    ScrollView::getColumn( 7, x, w );
    int width = aveWidth_ * ( lstrlen( "XXXXXXXXXX" ) );
    SetRect( &rcDraw, x, rcModelElement.top, x + width, rcModelElement.bottom );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER );
}

void QTextView::saveText( FILE *pfile )
    {
    // Not supported
    ASSERT( FALSE );
    }
