#include <qobject.hxx>
#include <stdio.h>
#include <statpak.h>
#include <boxctrl.h>
#include "types.hxx"
#include "template.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "perfeng.hxx"
#include "perfsum.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

#define clrBlack  RGB( 0x00, 0x00, 0x00 )
#define clrLtGray RGB( 0xC0, 0xC0, 0xC0 )
#define clrGray   RGB( 0x80, 0x80, 0x80 )
#define clrDkGray RGB( 0x40, 0x40, 0x40 )
#define clrRed    RGB( 0xFF, 0x00, 0x00 )
#define clrGreen  RGB( 0x00, 0x80, 0x00 )
static const CHAR szSpacer[ ] = "  ";

//////////////////////////////////////////////////////////////////////////////
// QPerformanceSummary methods
//////////////////////////////////////////////////////////////////////////////

QPerformanceSummary::QPerformanceSummary( HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids )
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
    
    // Create the separator brush
    LOGBRUSH lb;
    lmemset( &lb, 0, sizeof( lb ) );
    lb.lbStyle = BS_SOLID;
    lb.lbColor = clrLtGray;
    hbr_ = CreateBrushIndirect( &lb );

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
    f = ScrollView::addColumn( "", aveWidth_ * PERFSUM_NAME_WIDTH );
    ASSERT( f );
    f = ScrollView::addColumn( "", 0 );
    ASSERT( f );
}

QPerformanceSummary::~QPerformanceSummary( )
{
    Cleanup( );
}

void QPerformanceSummary::Cleanup( )
{
    DeleteObject( hfont_ );
    DeleteObject( hbr_ );
}

void QPerformanceSummary::draw( HDC hdc, const RECT &rcPaint ) const
{
    SetBkMode( hdc, TRANSPARENT );
    SelectObject( hdc, hfont_ );
    
    ScrollView::draw( hdc, rcPaint );
}

void QPerformanceSummary::drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const
{
    RECT rcDraw;
    int  x, w;

    // get the element
    NVPair *pair = (NVPair *) elem;

    // Draw the separator and the horizontal grid lines
    SelectObject( hdc, hpenGrids_ );
    if ( pair->separator( ) )
        SelectObject( hdc, hbr_ );
    else
        SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    Rectangle( hdc, rcModelElement.left,  rcModelElement.bottom,
                    rcModelElement.right, rcModelElement.top + 1 );

    // Draw the Name column
    if ( pair->metadata( ) )
        SetTextColor( hdc, clrGreen );
    else if ( pair->selected( ) )
        SetTextColor( hdc, clrRed );
    else
        SetTextColor( hdc, clrDkGray );
    ScrollView::getColumn( 0, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, pair->name( ), lstrlen( pair->name( ) ), &rcDraw, DT_LEFT | DT_VCENTER );
                          
    // Draw the Value column
    ScrollView::getColumn( 1, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, pair->value( ), lstrlen( pair->value( ) ), &rcDraw, DT_LEFT | DT_VCENTER );
}

void QPerformanceSummary::saveText( FILE *pfile )
    {
    NVPair *pair;

    for ( pair = (NVPair *) model_->GetHead( );
          pair;
          pair = (NVPair *) model_->GetNext( pair ) )
        {
        if ( pair->separator( ) )
            fprintf( pfile, "%30s%25s\n", "==============================", "=========================" );
        else
            fprintf( pfile, "%-30s%25s\n", pair->name( ), pair->value( ) );
        }
    }

