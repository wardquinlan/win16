#ifndef _PROFILE_HXX
#define _PROFILE_HXX

extern int  _dyfont;
extern BOOL _fOpenGeneral;

extern CHAR _szMacroAPath[ cbPathMax + 1 ];
extern CHAR _szMacroBPath[ cbPathMax + 1 ];
extern CHAR _szMacroCPath[ cbPathMax + 1 ];
extern CHAR _szMacroDPath[ cbPathMax + 1 ];
extern CHAR _szMacroA[ cbAccountMax + 1 ];
extern CHAR _szMacroB[ cbAccountMax + 1 ];
extern CHAR _szMacroC[ cbAccountMax + 1 ];
extern CHAR _szMacroD[ cbAccountMax + 1 ];

extern void ProfileInit( const CHAR FAR *lpszCmdLine );

#endif
