#ifndef _TRADEVIEW_HXX
#define _TRADEVIEW_HXX

#include "scrview.hxx"

class QTradeView : public ScrollView
{
public:
    QTradeView( BOOL fSimulator, HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids );
    virtual ~QTradeView( );
    virtual void Cleanup( );
    
    virtual void draw( HDC hdc, const RECT &rcPaint ) const;
    virtual void drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const;
    virtual void saveText( FILE *pfile );

private:
    BOOL  fPerformanceTester_;
    HFONT hfont_;
    int   aveWidth_;
};

#endif

