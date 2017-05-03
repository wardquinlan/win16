#include <qobject.hxx>
#include <stdio.h>
#include "budget.hxx"
#include "doc.hxx"
#include "view.hxx"
#include "profile.hxx"
#include "boxctrl.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

static const CHAR szSpacer[ ] = "  ";

//////////////////////////////////////////////////////////////////////////////
// QBudgetView methods
//////////////////////////////////////////////////////////////////////////////

QBudgetView::QBudgetView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines )
    :ScrollView( hwnd, model, dyTitle, dyLines, _hpenLtGray )
{
    HDC hdc;
    HDC hfontOld;
    BOOL f;
    TEXTMETRIC tm;
    int aveWidth;
    
    // Get average character width
    lmemset( &tm, 0, sizeof( tm ) );
    hdc = GetDC( hwnd );
    hfontOld = SelectObject( hdc, _hfontNormal );
    f = GetTextMetrics( hdc, &tm );
    ASSERT( f );
    SelectObject( hdc, hfontOld );
    ReleaseDC( hwnd, hdc );
    aveWidth = tm.tmAveCharWidth;

    f = ScrollView::addColumn( "Num",     aveWidth * ( 1 + lstrlen( "12345" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Date",    aveWidth * ( 3 + lstrlen( "Jan 00, 0000" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Credit",  aveWidth * ( 3 + lstrlen( "00000.00" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Debit",   aveWidth * ( 3 + lstrlen( "00000.00" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Balance", aveWidth * ( 3 + lstrlen( "(000000.00)" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Description", 0 );
    ASSERT( f );
}

QBudgetView::~QBudgetView( )
{
    Cleanup( );
}

void QBudgetView::Cleanup( )
{
}

void QBudgetView::draw( HDC hdc, const RECT &rcPaint ) const
{
    SetBkMode( hdc, TRANSPARENT );
    SelectObject( hdc, _hfontNormal );
    
    ScrollView::draw( hdc, rcPaint );
}

void QBudgetView::drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const
{
    CHAR sz[ cbBufferMax + 1 ];
    RECT rcDraw;
    int x, w;
    COLORREF clrText;

    // get the element
    QTxb *ptxb = (QTxb *) elem;

    // draw the element rectangle
    if ( ptxb->GetSeq( ) <= _pdoc->getHighestSeqBalance( ) )
    {
        clrText = clrGray;
        SelectObject( hdc, _hbrFill );
    }
    else
    {
        clrText = clrBlack;
        SelectObject( hdc, GetStockObject( HOLLOW_BRUSH ) );
    }
    
    SelectObject( hdc, _hpenLtGray );
    Rectangle( hdc, rcModelElement.left,  rcModelElement.bottom, 
                    rcModelElement.right, rcModelElement.top + 1 );
    
    // Draw the Sequence Number Column
    SetTextColor( hdc, clrGray );
    ScrollView::getColumn( 0, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    sprintf( sz, "%ld", ptxb->GetSeq( ) );
    DrawText( hdc, sz, lstrlen( sz ), &rcDraw, DT_RIGHT | DT_VCENTER | DT_SINGLELINE );

    // Draw the Date column
    SetTextColor( hdc, clrText );
    ScrollView::getColumn( 1, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    QTime tDate = ptxb->GetTransaction( ).GetDate( );
    tDate.Format( cbTransactionsFormatMax, _szTransactionsFormat );
    QString sDate = (QString) tDate;
    DrawText( hdc, sDate, sDate.GetLength( ), &rcDraw, DT_RIGHT | DT_VCENTER | DT_SINGLELINE );

    // Draw the Credit/Debit columns
    Money monAmount = ptxb->GetTransaction( ).GetAmount( );
    BOOL fDeposit = ( monAmount >= 0 );
    if ( !fDeposit )
        monAmount = -monAmount;
    ScrollView::getColumn( fDeposit ? 2 : 3, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    DrawText( hdc, monAmount.asString( ), lstrlen( monAmount.asString( ) ), &rcDraw, DT_RIGHT | DT_VCENTER | DT_SINGLELINE );

    // Draw the Balance column
    ScrollView::getColumn( 4, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    Money monBalance = ptxb->GetBalance( );
    QString sBalance = monBalance.asString( );
    SetTextColor( hdc, ( monBalance >= 0 ? clrText : clrRed ) );
    DrawText( hdc, sBalance, sBalance.GetLength( ), &rcDraw, DT_RIGHT | DT_VCENTER | DT_SINGLELINE );

    // Draw the Description column
    SetTextColor( hdc, clrText );
    ScrollView::getColumn( 5, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    QString sDesc = QString( szSpacer ) + ptxb->GetTransaction( ).GetDescription( );
    DrawText( hdc, sDesc, sDesc.GetLength( ), &rcDraw, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
}

//////////////////////////////////////////////////////////////////////////////
// QSummaryView methods
//////////////////////////////////////////////////////////////////////////////

QSummaryView::QSummaryView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines )
    :ScrollView( hwnd, model, dyTitle, dyLines, _hpenLtGray )
{
    HDC hdc;
    HDC hfontOld;
    BOOL f;
    TEXTMETRIC tm;
    
    // Get average character width
    lmemset( &tm, 0, sizeof( tm ) );
    hdc = GetDC( hwnd );
    hfontOld = SelectObject( hdc, _hfontNormal );
    f = GetTextMetrics( hdc, &tm );
    ASSERT( f );
    SelectObject( hdc, hfontOld );
    ReleaseDC( hwnd, hdc );
    aveWidth_ = tm.tmAveCharWidth;

    f = ScrollView::addColumn( "Account", aveWidth_ * ( 3 + lstrlen( szSpacer ) + cbAccountMax ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Date",    aveWidth_ * ( 3 + lstrlen( "Jan 00, 0000" ) ) );
    ASSERT( f );
    f = ScrollView::addColumn( "Balance", 0 );
    ASSERT( f );
}

QSummaryView::~QSummaryView( )
{
    Cleanup( );
}

void QSummaryView::Cleanup( )
{
}

void QSummaryView::draw( HDC hdc, const RECT &rcPaint ) const
{
    SetBkMode( hdc, TRANSPARENT );
    SelectObject( hdc, _hfontNormal );
    
    ScrollView::draw( hdc, rcPaint );
}

void QSummaryView::drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const
{
    RECT rcDraw;
    int x, w;

    // get the element
    QAccount *pacc = (QAccount *) elem;

    // draw the element rectangle
    SelectObject( hdc, _hpenLtGray );
    SelectObject( hdc, GetStockObject( HOLLOW_BRUSH ) );
    Rectangle( hdc, rcModelElement.left,  rcModelElement.bottom, 
                    rcModelElement.right, rcModelElement.top + 1 );
    

    SetTextColor( hdc, clrBlack );

    // Draw the Account column
    ScrollView::getColumn( 0, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    QString sAcc = QString( szSpacer ) + pacc->GetName( );
    DrawText( hdc, sAcc, sAcc.GetLength( ), &rcDraw, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

    // get the TXB to display; if NULL then don't draw anything else
    QTxb *ptxb = pacc->GetTail( );
    if ( !ptxb )
        return;
    
    // Draw the Date column
    ScrollView::getColumn( 1, x, w );
    SetRect( &rcDraw, x, rcModelElement.top, x + w, rcModelElement.bottom );
    QTime tDate = ptxb->GetTransaction( ).GetDate( );
    tDate.Format( cbTransactionsFormatMax, _szTransactionsFormat );
    QString sDate = (QString) tDate;
    DrawText( hdc, sDate, sDate.GetLength( ), &rcDraw, DT_RIGHT | DT_VCENTER | DT_SINGLELINE );
    
    // Draw the Balance column
    ScrollView::getColumn( 2, x, w );
    int width = aveWidth_ * ( 3 + lstrlen( "(000000.00)" ) );
    SetRect( &rcDraw, x, rcModelElement.top, x + width, rcModelElement.bottom );
    Money monBalance = ptxb->GetBalance( );
    QString sBalance = monBalance.asString( );
    SetTextColor( hdc, ( monBalance >= 0 ? clrBlack : clrRed ) );
    DrawText( hdc, sBalance, sBalance.GetLength( ), &rcDraw, DT_RIGHT | DT_VCENTER | DT_SINGLELINE );
}


