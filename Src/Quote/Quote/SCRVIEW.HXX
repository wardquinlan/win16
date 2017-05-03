#ifndef _SCRVIEW_HXX
#define _SCRVIEW_HXX

#include <stdio.h>

// Definition of the Model
typedef class QVirtualList Model;
typedef class QListObject  ModelElement;

struct Column
{
    HWND hwnd_; // child window
    int  x_;    // x-component
    int  w_;    // width
};

class ScrollView : public QObject
{
public:
    ScrollView( HWND hwnd, const Model *model, const int dyTitle, const int dyLines, HPEN hpenGrids );
    virtual ~ScrollView( ) = 0;
    virtual void Cleanup( );

    virtual void top( );
    virtual void bottom( );
    virtual void up( );
    virtual void down( );
    virtual void pgUp( );
    virtual void pgDown( );
    virtual void gotoPos( int pos );
    
    virtual void draw( HDC hdc, const RECT &rcPaint ) const;
    virtual void drawModelElement( HDC hdc, const ModelElement *elem, const RECT &rcModelElement ) const = 0;
    virtual void resize( );
    virtual void saveText( FILE *pfile ) = 0;

protected:
    const RECT &clientRect( ) const { return rcClient_; }
    BOOL  addColumn( const CHAR *szTitle, int width );
    void  getColumn( int column, int &x, int &w ) const;

    const HWND hwnd_;             // associated HWND
    const HPEN hpenGrids_;        // gridline pen
    const Model *model_;          // associated model
    const ModelElement *first_;   // first visible element on view

private:
    RECT  rcClient_;              // client area

    int lines_;                   // number of lines in window
    const int dyTitle_;           // height (in device units) of the title
    const int dyLines_;           // height (in device units) of one line

                                  // scrolling: goes from 0 to ( #model - 1 )
                                  // (both range and position)
    int scrollRange_;             // scroll range
    int scrollPos_;               // current scroll position

    #define SCR_NCOLUMNS 8
    int nColumns_;                // number of columns
    Column rgcol_[ SCR_NCOLUMNS ];// the columns
};

#endif
