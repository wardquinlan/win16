#ifndef _TEXTVIEW_HXX
#define _TEXTVIEW_HXX

#include "scrview.hxx"

class QTextView : public ScrollView
{
public:
    QTextView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids );
    virtual ~QTextView( );
    virtual void Cleanup( );
    
    virtual void draw( HDC hdc, const RECT &rcPaint ) const;
    virtual void drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const;
    virtual void saveText( FILE *pfile );

    // Get First visible element
    const QQt *getFirst( ) const
    {
        return (QQt *) first_;
    }

private:
    HFONT hfont_;
    int   aveWidth_;
};

#endif

