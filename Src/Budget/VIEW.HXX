#ifndef _VIEW_HXX
#define _VIEW_HXX

#include "scrview.hxx"

#if ( FDATA == 1 )
#define GWL_VIEWPTR 0 // offset of the view pointer
#define cbWindowExtra ( GWL_VIEWPTR + sizeof( DWORD ) )
#else
#define GWW_VIEWPTR 0 // offset of the view pointer
#define cbWindowExtra ( GWW_VIEWPTR + sizeof( WORD ) ) 
#endif

class QBudgetView : public ScrollView
{
public:
    QBudgetView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines );
    virtual ~QBudgetView( );
    virtual void Cleanup( );
    
    virtual void draw( HDC hdc, const RECT &rcPaint ) const;
    virtual void drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const;
};

class QSummaryView : public ScrollView
{
public:
    QSummaryView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines );
    virtual ~QSummaryView( );
    virtual void Cleanup( );

    virtual void draw( HDC hdc, const RECT &rcPaint ) const;
    virtual void drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const;

private:
    int aveWidth_;
};

#endif

