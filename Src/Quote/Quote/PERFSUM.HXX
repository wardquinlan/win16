#ifndef _PERFSUM_HXX
#define _PERFSUM_HXX

#include "scrview.hxx"

class QPerformanceSummary : public ScrollView
{
public:
    QPerformanceSummary( HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids );
    virtual ~QPerformanceSummary( );
    virtual void Cleanup( );
    
    virtual void draw( HDC hdc, const RECT &rcPaint ) const;
    virtual void drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const;
    virtual void saveText( FILE *pfile );

private:
    HFONT hfont_;
    HBRUSH hbr_;
    int   aveWidth_;
};

#define PERFSUM_NAME_WIDTH 26

#endif

