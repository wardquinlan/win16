//////////////////////////////////////////////////////////////////////////////
// DLG.HXX
//
// Dialog box include file.
//
// Copyright (c) Ward Quinlan, 1996
//////////////////////////////////////////////////////////////////////////////
#ifndef _DLG_HXX
#define _DLG_HXX

extern "C"
    {
    BOOL FAR PASCAL _export AddQuoteProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export DeleteQuoteProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export FontSizeProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export BrowserProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export ExternalIndicatorFileProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export NewWindowProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export ColorProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export AboutProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export LineWidthProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export TextFormatProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export CreateITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export CustomITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export ExternalITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export DeleteITProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export CreateWLProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export DeleteWLProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export CreateRuleProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export DeleteRuleProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export CreateCondProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export DeleteConstProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export CreateConstProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export DeleteCondProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export PerformanceProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export PrintProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export ReportScanResultsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export SaveResultsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export ScannerProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export SaveScanResultsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export IndValProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export ReportIndicatorValueResultsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    BOOL FAR PASCAL _export SimulatorProc( HWND hDlg, UINT message, UINT wParam, LONG lParam );
    }
#endif
