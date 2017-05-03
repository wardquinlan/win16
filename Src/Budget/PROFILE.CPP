#include <qobject.hxx>
#include "budget.hxx"
#include "profile.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

static const CHAR FAR *_lpszFile = "budget.ini";

// Section Identifiers
static CHAR _szView[]        = "view";

///////////////////
// View Identifiers
static CHAR _szDyfont[] = "dyfont";
int  _dyfont;

static CHAR _szOpenGeneral[ ] = "opengeneral";
BOOL _fOpenGeneral;

CHAR _szMacroA[ cbAccountMax + 1 ] = "macroA";
CHAR _szMacroAPath[ cbPathMax + 1 ];

CHAR _szMacroB[ cbAccountMax + 1 ] = "macroB";
CHAR _szMacroBPath[ cbPathMax + 1 ];

CHAR _szMacroC[ cbAccountMax + 1 ] = "macroC";
CHAR _szMacroCPath[ cbPathMax + 1 ];

CHAR _szMacroD[ cbAccountMax + 1 ] = "macroD";
CHAR _szMacroDPath[ cbPathMax + 1 ];

void ProfileInit( const CHAR FAR *lpszCmdLine )
    {
    UINT nTmp;

    if ( lpszCmdLine && *lpszCmdLine )
        {
        _lpszFile = lpszCmdLine;
        }

    ////////////////////////////////////////////////////////////////////
    // View Identifiers

    _dyfont = GetPrivateProfileInt( _szView, _szDyfont, 14, _lpszFile );
    if ( _dyfont <= 0 )
        {
        _dyfont = 14;
        }
    
    // 0 = FALSE  1 = TRUE
    nTmp = GetPrivateProfileInt( _szView, _szOpenGeneral, 1, _lpszFile );
    _fOpenGeneral = ( nTmp == 1u );

    GetPrivateProfileString( _szView, _szMacroA, "", _szMacroAPath, cbPathMax, _lpszFile );
    
    GetPrivateProfileString( _szView, _szMacroB, "", _szMacroBPath, cbPathMax, _lpszFile );
    
    GetPrivateProfileString( _szView, _szMacroC, "", _szMacroCPath, cbPathMax, _lpszFile );

    GetPrivateProfileString( _szView, _szMacroD, "", _szMacroDPath, cbPathMax, _lpszFile );
    }

