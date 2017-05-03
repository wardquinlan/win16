#ifndef _VIEW_HXX
#define _VIEW_HXX

typedef NUM (*PFNSCALE)( NUM NUM );
extern  NUM ScaleLinear( NUM num );
extern  NUM ScaleLog( NUM num );

struct ViewInfo
    {
    BOOL                        fPresent;
    BOOL                        fAvail; 
    POINT                       ptTitle;
    QString                     sTitle;
    RECT                        rcGrid;
    const QIndicatorDictionary *pdict;
    QSignalList                 sglist;
    };

class QView : public QObject
    {
public:
    QView( const CHAR *pszName, BOOL fPrintView = FALSE );
    virtual ~QView( );
    virtual void Cleanup( );

    virtual void OnDraw   ( const QWindowLayout *pwl, HDC hdc );
    virtual void OnReSize ( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd );
    virtual void OnReBuild( const QWindowLayout *pwl );

    // messages to inform self of scrolling
    virtual BOOL home     ( HWND hwnd );
    virtual BOOL end      ( HWND hwnd );
    virtual BOOL left     ( HWND hwnd );
    virtual BOOL right    ( HWND hwnd );
    virtual BOOL pageleft ( HWND hwnd );
    virtual BOOL pageright( HWND hwnd );
    virtual BOOL gotopos  ( HWND hwnd, int nPos );
    
    void iterate    ( HWND hwnd, void (*pfn)( QView *, int nPage, PrintStruct& ), PrintStruct &ps );
    void preparePage( PrintStruct &ps, BOOL fScrollToEnd );

    void GetLogicalBorders( const QWindowLayout *pwl, int *pxLeft, int *pxRight );
    QQt *GetFirstPqqt( );

    #ifdef DEBUG
    virtual void AssertValid( ) const;  
    #endif

private:
    // Private functions
    void     AdjustDown( NUM *pnumMin, const NUM numAdj );
    int      Scale( int nView, NUM num );
    BOOL     CalculateRange( const QWindowLayout *pwl, int nIndex );
    BOOL     CalculateVolumeRange( );
    
    void     OnReSizeGrids ( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd );
    void     OnReSizePrices( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd );
    void     OnReSizeV     ( const QWindowLayout *pwl, HWND hwnd, HDC hdc, const RECT &rcClient, BOOL fScrollToEnd );

    void     OnDrawGrids         ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawPrices        ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawV             ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawIndicators    ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawHorizontalLine( const QWindowLayout *pwl, HDC hdc, int nView, 
             const QCache *pcache, const QIndicatorTemplate *pit ); 
    void     OnDrawLineIndicator ( const QWindowLayout *pwl, HDC hdc, int nView, 
             const QCache *pcache, const QIndicatorTemplate *pit );
    void     OnDrawBarIndicator  ( const QWindowLayout *pwl, HDC hdc, int nView, 
             const QCache *pcache, const QIndicatorTemplate *pit );
    void     OnDrawPointIndicator( const QWindowLayout *pwl, HDC hdc, int nView, 
             const QCache *pcache, const QIndicatorTemplate *pit );
    void     OnDrawHLC           ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawSignals       ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawDateLines     ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawConditions    ( const QWindowLayout *pwl, HDC hdc, int nView );
    void     OnDrawDates         ( const QWindowLayout *pwl, HDC hdc, int nView );
    BOOL     IsDrawDate          ( const QQt *pqtPrev, const QQt *pqt );
    void     hline               ( HDC hdc, int y, int x1, int x2 );
    void     vline               ( HDC hdc, int x, int y1, int y2 );
    int      m_nViews;
    
    void operator--( int );
    void operator++( int );

    ViewInfo m_rgviewinfo  [ _cViewLayouts ];
    NUM      m_rgnumMinAbs [ _cViewLayouts ];
    NUM      m_rgnumMaxAbs [ _cViewLayouts ];
    NUM      m_rgnumScale  [ _cViewLayouts ];
    NUM      m_rgnumMinAdj [ _cViewLayouts ];
    NUM      m_rgnumStepAdj[ _cViewLayouts ];
    int     *m_rgrgnScales [ _cViewLayouts ];
    PFNSCALE m_rgpfnScale  [ _cViewLayouts ];

    const int  m_xLines;
    const BOOL m_fPrintView;
    static const int m_nShift;

    QQt     *m_pqqtFirst;
    int      m_nMax;
    int      m_nFirst;

    NUM      m_numVlMax; // can only be one volume
    NUM      m_rgnumScaleVl[ _cViewLayouts ];

    static   const NUM m_rgnum[];
    static   const UINT m_nCharsRight;
    static   const NUM  m_numVlFactor;
    static   const int  m_dxBase;   // 1/2 base  of signal triangles
    static   const int  m_dyHeight; // height of signal triangles
    static   const int  m_dySpace;  // space between signal & quote
    static   const int  m_dLen;     // Length of a dotted line
    static   const int  m_dSpc;     // Space between dotted lines
    };

#endif
