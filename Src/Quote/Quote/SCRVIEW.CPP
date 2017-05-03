#include <qobject.hxx>
#include <stdio.h>
#include <boxctrl.h>
#include <statpak.h>
#include "resource.hxx"
#include "types.hxx"
#include "template.hxx"
#include "tools.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "scrview.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

// Title color
#define CLRTITLE RGB( 0x00, 0x00, 0xFF )

// Adjust to application instance accordingly...
extern HANDLE _hInstance;

ScrollView::ScrollView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids )
    :hwnd_( hwnd ), 
     model_( model ), 
     first_( model->GetHead( ) ),
     lines_( 0 ),
     dyTitle_( dyTitle ),
     dyLines_( dyLines ),
     scrollRange_( 0 ),
     scrollPos_( 0 ),
     nColumns_( 0 ),
     hpenGrids_( hpenGrids )
{
    ASSERT( hwnd_ );
    ASSERT( model_ != 0 );
    
    lmemset( &rcClient_, 0, sizeof( rcClient_ ) );
    lmemset( &rgcol_, 0, sizeof( rgcol_ ) );
}

ScrollView::~ScrollView( )
{
    Cleanup( );
}

void ScrollView::Cleanup( )
{
    int i;
    for ( i = 0; i < nColumns_; ++i )
    {
        if ( rgcol_[ i ].hwnd_ )
            DestroyWindow( rgcol_[ i ].hwnd_ );
    }
    nColumns_ = 0;
}

void ScrollView::top( )
{
    first_     = model_->GetHead( );
    scrollPos_ = 0;
    
    SetScrollPos( hwnd_, SB_VERT, scrollPos_, TRUE );
    InvalidateRect( hwnd_, NULL, TRUE );
}

void ScrollView::bottom( )
{
    first_     = model_->GetTail( );
    scrollPos_ = scrollRange_;
    
    int iters = min( ( (int) model_->GetCount( ) ), lines_ ) - 1;
    while ( iters > 0 )
    {
        first_ = model_->GetPrev( first_ );
        --scrollPos_;
        --iters;
    }

    SetScrollPos( hwnd_, SB_VERT, scrollPos_, TRUE );
    InvalidateRect( hwnd_, NULL, TRUE );
}

void ScrollView::gotoPos( int pos )
{
    ASSERT( pos >= 0 );
    ASSERT( pos <= scrollRange_ );

    first_     = model_->GetHead( );
    scrollPos_ = 0;

    int iters = pos;
    while ( iters > 0 )
    {
        first_ = model_->GetNext( first_ );
        ++scrollPos_;
        --iters;
    }
    
    SetScrollPos( hwnd_, SB_VERT, scrollPos_, TRUE );
    InvalidateRect( hwnd_, NULL, TRUE );
}

void ScrollView::up( )
{
    RECT rcScroll;
    
    int iters = min( scrollPos_, 1 );
    if ( iters > 0 )
    {
        first_ = model_->GetPrev( first_ );
        --scrollPos_;
        --iters;

        if ( lines_ > 1 )
        {
            int y = dyTitle_ + ( lines_ - 1 ) * dyLines_;
            SetRect( &rcScroll, 0, dyTitle_, rcClient_.right, y );
            ScrollWindow( hwnd_, 0, +dyLines_, &rcScroll, NULL );
        }
        else
        {
            InvalidateRect( hwnd_, NULL, TRUE );
        }
    }
    
    SetScrollPos( hwnd_, SB_VERT, scrollPos_, TRUE );
}

void ScrollView::pgUp( )
{
    int iters = min( scrollPos_, lines_ );
    while ( iters > 0 )
    {
        first_ = model_->GetPrev( first_ );
        --scrollPos_;
        --iters;
    }
    
    SetScrollPos( hwnd_, SB_VERT, scrollPos_, TRUE );
    InvalidateRect( hwnd_, NULL, TRUE );
}

void ScrollView::down( )
{
    RECT rcScroll;
    
    int iters = min( scrollRange_ - scrollPos_, 1 );
    if ( iters > 0 )
    {
        first_ = model_->GetNext( first_ );
        ++scrollPos_;
        --iters;
    
        if ( lines_ > 1 )
        {
            int y = dyTitle_ + lines_ * dyLines_;
            SetRect( &rcScroll, 0, dyTitle_ + dyLines_, rcClient_.right, y );
            ScrollWindow( hwnd_, 0, -dyLines_, &rcScroll, NULL );
        }
        else
        {
            InvalidateRect( hwnd_, NULL, TRUE );
        }
    }
    
    SetScrollPos( hwnd_, SB_VERT, scrollPos_, TRUE );
}

void ScrollView::pgDown( )
{
    int iters = min( ( scrollRange_ - scrollPos_ ), lines_ );
    while ( iters > 0 )
    {
        first_ = model_->GetNext( first_ );
        ++scrollPos_;
        --iters;
    }
    
    SetScrollPos( hwnd_, SB_VERT, scrollPos_, TRUE );
    InvalidateRect( hwnd_, NULL, TRUE );
}

void ScrollView::resize( )
{
    int i;

    GetClientRect( hwnd_, &rcClient_ );

    lines_       = ( rcClient_.bottom - rcClient_.top - dyTitle_ ) / dyLines_;
    scrollRange_ = max( ( (int) model_->GetCount( ) ) - 1, 0 );
    
    // if being called after a change to the model, there is
    // a possibility that scrollPos_ > scrollRange_.  Clients
    // of this interface will have to call another call
    // such as bottom( ) to compensate for this
    SetScrollRange( hwnd_, SB_VERT, 0, scrollRange_, TRUE );

    // Resize the (n-1)th column, as it is variable-width
    if ( nColumns_ > 0 )
        rgcol_[ nColumns_ - 1 ].w_ = rcClient_.right - rgcol_[ nColumns_ - 1 ].x_;

    // Adjust the child boxctrl windows
    for ( i = 0; i < nColumns_; ++i )
        MoveWindow( rgcol_[ i ].hwnd_, rgcol_[ i ].x_, 0, rgcol_[ i ].w_, dyTitle_, TRUE );
}

void ScrollView::draw( HDC hdc, const RECT &rcPaint ) const
{
    int y;
    int i;
    int line;
    const ModelElement *elem;
    HPEN hpenOld;
    
    for ( y = dyTitle_,
          line = 0,
          elem = first_;

          line < lines_ && elem;

          y += dyLines_, ++line, elem = model_->GetNext( elem ) )
    {
        RECT rcSrc;
        RECT rcDst;

        SetRect( &rcSrc, 0, y, rcClient_.right, y + dyLines_ );
        if ( IntersectRect( &rcDst, &rcSrc, &rcPaint ) )
            drawModelElement( hdc, elem, rcSrc );
    }

    // draw the grid lines (for the first n - 1 columns)
    hpenOld = SelectObject( hdc, hpenGrids_ );
    for ( i = 0; i < nColumns_ - 1; ++i )
    {
        int x = rgcol_[ i ].x_ + rgcol_[ i ].w_;
        MoveTo( hdc, x, rcClient_.top + dyTitle_ );
        LineTo( hdc, x, rcClient_.bottom );
    }
    SelectObject( hdc, hpenOld );
}

BOOL ScrollView::addColumn( const CHAR *szTitle, int width )
{
    if ( nColumns_ == SCR_NCOLUMNS )
        return FALSE;

    rgcol_[ nColumns_ ].hwnd_ = 
        CreateWindow( GetString( IDS_BOXCTRL ), szTitle, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, 
            hwnd_, nColumns_, _hInstance, NULL );
    ASSERT( rgcol_[ nColumns_ ].hwnd_ );
    
    // Set title box attributes
    SetBoxColor  ( rgcol_[ nColumns_ ].hwnd_, CLRTITLE );
    SetBoxJustify( rgcol_[ nColumns_ ].hwnd_, BOX_CENTER );
    SetBoxState  ( rgcol_[ nColumns_ ].hwnd_, BOX_OUTSIDE );
    
    // Set box ctrl font size
    HDC hdc = GetDC( rgcol_[ nColumns_ ].hwnd_ );
    SetBoxFontHeight( rgcol_[ nColumns_ ].hwnd_, LoEnglishToDeviceY( hdc, _ptpl->m_genopts.GetFontSize( ) ) );
    ReleaseDC( rgcol_[ nColumns_ ].hwnd_, hdc );
    
    rgcol_[ nColumns_ ].x_ = ( nColumns_ > 0 ? rgcol_[ nColumns_ - 1 ].x_ + rgcol_[ nColumns_ - 1 ].w_ : 0 );
    rgcol_[ nColumns_ ].w_ = width;
    ++nColumns_;

    return TRUE;
}

VOID ScrollView::getColumn( int column, int &x, int &w ) const
{
    ASSERT( column >= 0 );
    ASSERT( column < nColumns_ );
    
    x = rgcol_[ column ].x_;
    w = rgcol_[ column ].w_;
}
