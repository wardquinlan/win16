//////////////////////////////////////////////////////////////////////////////
// SAR.HXX
//
// Parabolic-SAR Indicator
//
// This indicator is for use with the Quote package.
//
// This is an implementation of the Parabolic-SAR
// indicator, developed by J. Welles Wilder.
//////////////////////////////////////////////////////////////////////////////

#define IDD_SAR      100
#define IDC_ACCEL    200
#define IDC_ACCELMAX 201

#ifndef RC_INVOKED
extern "C" 
    {
    int  FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine );  
    int  FAR PASCAL _export WEP( int nParam );
    BOOL FAR PASCAL _export SarOptionsDlgProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    };
#endif
