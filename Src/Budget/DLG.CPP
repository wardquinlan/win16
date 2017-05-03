#include <qobject.hxx>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "id1.hxx"
#include "budget.hxx"
#include "profile.hxx"
#include "dlg.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

static BOOL stringIsDigit( const CHAR *psz );

BOOL FAR PASCAL _export NewWindowProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QAccount *pacc;
    CHAR szAccount[ cbAccountMax + 1 ];
    int nCount;
    static HWND hwndAccount;

    switch( message )
        {
    case WM_INITDIALOG:
        TRY
            {
            hwndAccount = GetDlgItem( hDlg, IDC_ACCOUNT );
            pacc = _pdoc->GetGeneralAccount( );
            ASSERT( pacc != NULL );
            ASSERT( pacc->GetName( ) != NULL );
            ASSERT( lstrlen( pacc->GetName( ) ) <= cbAccountMax );
            SendMessage( hwndAccount, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) pacc->GetName( ) );
            for ( pacc = _pdoc->GetHead( );
                  pacc;
                  pacc = _pdoc->GetNext( pacc ) )
                {
                ASSERT( pacc->GetName( ) != NULL );
                ASSERT( lstrlen( pacc->GetName( ) ) <= cbAccountMax );
                SendMessage( hwndAccount, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) pacc->GetName( ) );
                }
            nCount = (int) SendMessage( hwndAccount, CB_GETCOUNT, 0, 0L );
            if ( nCount > 0 )
                {
                // Set to the first account
                SendMessage( hwndAccount, CB_SETCURSEL, 0, 0L );
                }
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hDlg );
            EndDialog( hDlg, FALSE );
            }
        END_CATCH
        return TRUE;
        
    case WM_COMMAND:
        switch( wParam )
            {
        case IDC_SUMMARY:
            TRY
                {
                OpenSummaryWindow( );
                SetFocus( hwndAccount );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hDlg );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;
            
        case IDC_ACCOUNT2:
            TRY
                {
                FExtractEditValue( hwndAccount, szAccount, sizeof( szAccount ) );
                OpenWindow( szAccount );
                SetFocus( hwndAccount );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hDlg );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export MacroProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QTime        tDate;
    QString      sDate;

    CHAR szMacro [ cbAccountMax + 1 ];
    CHAR szDate  [ cbDateMax    + 1 ];
    CHAR szDesc  [ cbDescMax    + 1 ];
    
    int  nMonth, nDay, nYear;

    static HWND hwndMacro;
    static HWND hwndDate;
    static HWND hwndDesc;

    switch( message )
        {
    case WM_INITDIALOG:
        hwndMacro = GetDlgItem( hDlg, IDC_MACRO );
        hwndDate  = GetDlgItem( hDlg, IDC_DATE  );
        hwndDesc  = GetDlgItem( hDlg, IDC_DESC  );
        SendMessage( hwndMacro, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _szMacroA );
        SendMessage( hwndMacro, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _szMacroB );
        SendMessage( hwndMacro, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _szMacroC );
        SendMessage( hwndMacro, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) _szMacroD );
        SendMessage( hwndMacro, CB_SETCURSEL, 0, 0L );
        TRY
            {
            tDate.Format( 32, "%m/%d/%y" );
            sDate = tDate;
            SetWindowText( hwndDate, sDate );
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hDlg );
            EndDialog( hDlg, FALSE );
            }
        END_CATCH
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            FExtractEditValue( hwndMacro, szMacro, sizeof( szMacro ) );
            FExtractEditValue( hwndDesc, szDesc, sizeof( szDesc ) );
            if ( !FExtractEditValue( hwndDate, szDate, sizeof( szDate ) ) ||
                 !FParseDate( szDate, &nMonth, &nDay, &nYear ) )
                {
                MessageBox( hDlg, "The date is invalid.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndDate );
                SelectEdit( hwndDate );
                return TRUE;
                }
            tDate.Set( nYear, nMonth, nDay, 0, 0, 0 );
            if ( lstrcmp( szMacro, _szMacroA ) == 0 )
                ExecuteMacro( hDlg, _szMacroA, _szMacroAPath, tDate, szDesc );
            else if ( lstrcmp( szMacro, _szMacroB ) == 0 )
                ExecuteMacro( hDlg, _szMacroB, _szMacroBPath, tDate, szDesc );
            else if ( lstrcmp( szMacro, _szMacroC ) == 0 )
                ExecuteMacro( hDlg, _szMacroC, _szMacroCPath, tDate, szDesc );
            else if ( lstrcmp( szMacro, _szMacroD ) == 0 )
                ExecuteMacro( hDlg, _szMacroD, _szMacroDPath, tDate, szDesc );
            else
                ASSERT( FALSE );
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            return TRUE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export AboutProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    CHAR szBuffer[ cbBufferMax + 1 ];
    int  cb;

    switch ( message )
        {
    case WM_INITDIALOG:
#ifdef DEBUG

#if ( FDATA == 1 )
        cb = wsprintf( szBuffer, "LD Budget Version %s", (LPCSTR) _vBudget );
#else
        cb = wsprintf( szBuffer, "MD Budget Version %s", (LPCSTR) _vBudget );
#endif

#else
        cb = wsprintf( szBuffer, "Budget Version %s", (LPCSTR) _vBudget );
#endif
        ASSERT( cb < sizeof( szBuffer ) );
        SetWindowText( GetDlgItem( hDlg, IDC_VERSION1 ), szBuffer );
#ifdef DEBUG
        cb = wsprintf( szBuffer, "QObject Version %s", (LPCSTR) _vQObject );
        ASSERT( cb < sizeof( szBuffer ) );
        SetWindowText( GetDlgItem( hDlg, IDC_VERSION2 ), szBuffer );
#endif
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
        case IDCANCEL:
            EndDialog( hDlg, TRUE );
            return TRUE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export CreateProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QTransaction tx;
    QTime        tDate;
    QString      sDate;
    CHAR szAccount[ cbAccountMax + 1 ];
    CHAR szDate [ cbDateMax + 1 ];
    CHAR szAmount[ cbAmountMax + 1 ];
    static HWND hwndAccount;
    static HWND hwndDate;
    static HWND hwndAmount;
    static BOOL fRet;
    Money monAmount;
    int nMonth;
    int nDay;
    int nYear;
    CHAR *pch;
    
    switch ( message )
        {
    case WM_INITDIALOG:
        hwndAccount = GetDlgItem( hDlg, IDC_ACCOUNT );
        hwndDate    = GetDlgItem( hDlg, IDC_DATE );
        hwndAmount  = GetDlgItem( hDlg, IDC_AMOUNT );
        SendMessage( hwndAccount, EM_LIMITTEXT, cbAccountMax, 0L );
        SendMessage( hwndDate, EM_LIMITTEXT, cbDateMax, 0L );
        SendMessage( hwndAmount, EM_LIMITTEXT, cbAmountMax, 0L );
        TRY
            {
            tDate.Format( 32, "%m/%d/%y" );
            sDate = tDate;
            SetWindowText( hwndDate, sDate );
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hDlg );
            EndDialog( hDlg, FALSE );
            }
        END_CATCH
        fRet = FALSE;
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            if ( !FExtractEditValue( hwndAccount, szAccount, sizeof( szAccount ) ) )
                {
                MessageBox( hDlg, "The account name is invalid.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndAccount );
                SelectEdit( hwndAccount );
                return TRUE;
                }
            if ( _pdoc->FindAccount( szAccount ) != NULL ||
                 lstrcmpi( szAccount, _szGeneral ) == 0 )
                {
                // This account already exists
                MessageBox( hDlg, "That account already exists.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndAccount );
                SelectEdit( hwndAccount );
                return TRUE;
                }
            if ( !isalpha( *szAccount ) )
                {
                MessageBox( hDlg, "Account names must start with an alpha character.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndAccount );
                SelectEdit( hwndAccount );
                return TRUE;
                }
            for ( pch = szAccount; *pch; pch++ )
                {
                if ( isspace( *pch ) )
                    {
                    MessageBox( hDlg, "Account names cannot contain white space characters.", _szAppName, MB_ICONEXCLAMATION );
                    SetFocus( hwndAccount );
                    SelectEdit( hwndAccount );
                    return TRUE;
                    }
                }
            if ( !FExtractEditValue( hwndDate, szDate, sizeof( szDate ) ) ||
                 !FParseDate( szDate, &nMonth, &nDay, &nYear ) )
                {
                MessageBox( hDlg, "The date is invalid.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndDate );
                SelectEdit( hwndDate );
                return TRUE;
                }
            if ( FExtractEditValue( hwndAmount, szAmount, sizeof( szAmount ) ) )
                {
                monAmount = Money( szAmount );
                }
            else
                {
                monAmount = 0L;
                }
            TRY
                {
                tDate.Set( nYear, nMonth, nDay, 0, 0, 0 );
                tx.SetDate( tDate );
                tx.SetDescription( "Initial Balance" );
                tx.SetAmount( monAmount );
                _pdoc->AddAccount( szAccount );
                if ( !_pdoc->AddTransaction( szAccount, tx ) )
                    MessageBox( hDlg, "The account was created, but the initial transaction could not be added.", _szAppName, MB_ICONEXCLAMATION );

                UpdateAllViews( );
                _pdoc->SetModifiedFlag( );
                ClearEditBox( hwndAccount );
                ClearEditBox( hwndAmount );
                SetFocus( hwndAccount );
                fRet = TRUE;
                }
            CATCH( exMemory )
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hDlg );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                MessageBox( hDlg, "The operation has failed.", _szAppName, MB_OK );
                }
            END_CATCH
            // do not end the dialog yet!!
            return TRUE;
            
        case IDCANCEL:
            // Close dialog (ie. not 'Cancel').
            EndDialog( hDlg, fRet );
            return TRUE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export XFerProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QTime        tDate;
    QString      sDate;

    const QAccount *pacc;
    int  nCount;

    CHAR szDate  [ cbDateMax    + 1 ];
    CHAR szAmount[ cbAmountMax  + 1 ];
    CHAR szDesc  [ cbDescMax    + 1 ];
    CHAR szFrom  [ cbAccountMax + 1 ];
    CHAR szTo    [ cbAccountMax + 1 ];

    static HWND hwndFrom;
    static HWND hwndTo;
    static HWND hwndDate;
    static HWND hwndAmount;
    static HWND hwndDesc;
    static BOOL fRet;

    int    nMonth;
    int    nDay;
    int    nYear;
    Money  monAmount;
    
    switch( message )
        {
    case WM_INITDIALOG:
        hwndFrom   = GetDlgItem( hDlg, IDC_FROM   );
        hwndTo     = GetDlgItem( hDlg, IDC_TO     );
        hwndDate   = GetDlgItem( hDlg, IDC_DATE   );
        hwndAmount = GetDlgItem( hDlg, IDC_AMOUNT );
        hwndDesc   = GetDlgItem( hDlg, IDC_DESC   );
        SendMessage( hwndDate,   EM_LIMITTEXT, cbDateMax,   0L );
        SendMessage( hwndDesc,   EM_LIMITTEXT, cbDescMax,   0L );
        SendMessage( hwndAmount, EM_LIMITTEXT, cbAmountMax, 0L );
        for ( pacc = _pdoc->GetHead( );
              pacc;
              pacc = _pdoc->GetNext( pacc ) )
            {
            ASSERT( pacc->GetName( ) != NULL );
            ASSERT( lstrlen( pacc->GetName( ) ) <= cbAccountMax );
            
            SendMessage( hwndFrom, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) pacc->GetName( ) );
            nCount = (int) SendMessage( hwndFrom, CB_GETCOUNT, 0, 0L );
            ASSERT( nCount > 0 );
            // Set to the first account
            SendMessage( hwndFrom, CB_SETCURSEL, 0, 0L );
            
            SendMessage( hwndTo, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) pacc->GetName( ) );
            nCount = (int) SendMessage( hwndTo, CB_GETCOUNT, 0, 0L );
            ASSERT( nCount > 0 );
            // Set to the first account
            SendMessage( hwndTo, CB_SETCURSEL, 0, 0L );
            }
        
        TRY
            {
            tDate.Format( 32, "%m/%d/%y" );
            sDate = tDate;
            SetWindowText( hwndDate, sDate );
            SelectEdit( hwndDate );
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hDlg );
            EndDialog( hDlg, FALSE );
            }
        END_CATCH
        fRet = FALSE;
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            GetText( hwndFrom, szFrom, sizeof( szFrom ) );
            GetText( hwndTo,   szTo,   sizeof( szTo   ) );
            if ( !FExtractEditValue( hwndDate, szDate, sizeof( szDate ) ) ||
                 !FParseDate( szDate, &nMonth, &nDay, &nYear ) )
                {
                MessageBox( hDlg, "The date is invalid.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndDate );
                SelectEdit( hwndDate );
                return TRUE;
                }
            if ( !FExtractEditValue( hwndAmount, szAmount, sizeof( szAmount ) ) )
                {
                MessageBox( hDlg, "The amount is invalid.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndAmount );
                SelectEdit( hwndAmount );
                return TRUE;
                }
            monAmount = Money( szAmount );
            
            // It is legal to have a blank description
            FExtractEditValue( hwndDesc, szDesc, sizeof( szDesc ) );
            TRY
                {
                tDate.Set( nYear, nMonth, nDay, 0, 0, 0 );
                if ( _pdoc->TransferTransaction( szFrom, szTo, tDate, monAmount, szDesc ) )
                    {
                    _pdoc->SetModifiedFlag( );
                    UpdateAllViews( );
                    ClearEditBox( hwndAmount );
                    ClearEditBox( hwndDesc );
                    SetFocus( hwndDate );
                    SelectEdit( hwndDate );
                    }
                else
                    {
                    MessageBox( hDlg, "The transfer could not be performed.", _szAppName, MB_ICONEXCLAMATION );
                    SetFocus( hwndDate );
                    SelectEdit( hwndDate );
                    }
                }
            CATCH( exMemory )
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hDlg );
                EndDialog( hDlg, FALSE );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                MessageBox( hDlg, "The operation has failed.", _szAppName, MB_OK );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, fRet );
            return TRUE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export TransactionsProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QTransaction tx;
    QString sDate;
    QTime   tDate;
    CHAR szDate [ cbDateMax + 1 ];
    CHAR szAmount[ cbAmountMax + 1 ];
    CHAR szDesc[ cbDescMax + 1 ];
    CHAR szAccount[ cbAccountMax + 1 ];
    static HWND hwndAccount;
    static HWND hwndDate;
    static HWND hwndDesc;
    static HWND hwndAmount;
    const QAccount *pacc;
    int nCount;
    int nMonth;
    int nDay;
    int nYear;
    Money monAmount;
    BOOL fDeposit;

    switch ( message )
        {
    case WM_INITDIALOG:
        hwndAccount = GetDlgItem( hDlg, IDC_ACCOUNT );
        hwndDate    = GetDlgItem( hDlg, IDC_DATE );
        hwndDesc    = GetDlgItem( hDlg, IDC_DESC );
        hwndAmount  = GetDlgItem( hDlg, IDC_AMOUNT );

        SendMessage( hwndDate, EM_LIMITTEXT,   cbDateMax, 0L );
        SendMessage( hwndDesc, EM_LIMITTEXT,   cbDescMax, 0L );
        SendMessage( hwndAmount, EM_LIMITTEXT, cbAmountMax, 0L );
        
        MenuController::setEditTransactionsMode( TRUE );

        for ( pacc = _pdoc->GetHead( );
              pacc;
              pacc = _pdoc->GetNext( pacc ) )
            {
            ASSERT( pacc->GetName( ) != NULL );
            ASSERT( lstrlen( pacc->GetName( ) ) <= cbAccountMax );
            SendMessage( hwndAccount, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) pacc->GetName( ) );
            nCount = (int) SendMessage( hwndAccount, CB_GETCOUNT, 0, 0L );
            ASSERT( nCount > 0 );
            // Set to the first account
            SendMessage( hwndAccount, CB_SETCURSEL, 0, 0L );
            }
        TRY
            {
            tDate.Format( 32, "%m/%d/%y" );
            sDate = tDate;
            SetWindowText( hwndDate, sDate );
            SelectEdit( hwndDate );
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hDlg );
            PostMessage( hDlg, WM_CLOSE, 0, 0L );
            }
        END_CATCH
        return TRUE;

    case WM_CLOSE:
        MenuController::setEditTransactionsMode( FALSE );
        DestroyWindow( hDlg );
        _hDlgModeless = 0;
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDC_DEPOSIT:
        case IDC_WITHDRAW:
            fDeposit = ( wParam == IDC_DEPOSIT );
            GetText( hwndAccount, szAccount, sizeof( szAccount ) );
            if ( !FExtractEditValue( hwndDate, szDate, sizeof( szDate ) ) ||
                 !FParseDate( szDate, &nMonth, &nDay, &nYear ) )
                {
                MessageBox( hDlg, "The date is invalid.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndDate );
                SelectEdit( hwndDate );
                return TRUE;
                }
            if ( !FExtractEditValue( hwndAmount, szAmount, sizeof( szAmount ) ) )
                {
                MessageBox( hDlg, "The amount is invalid.", _szAppName, MB_ICONEXCLAMATION );
                SetFocus( hwndAmount );
                SelectEdit( hwndAmount );
                return TRUE;
                }
            monAmount = Money( szAmount );
            if ( !fDeposit )
                {
                monAmount = -monAmount;
                }
            // It is legal to have a blank description
            FExtractEditValue( hwndDesc, szDesc, sizeof( szDesc ) );

            TRY
                {
                tDate.Set( nYear, nMonth, nDay, 0, 0, 0 );
                tx.SetDate( tDate );
                tx.SetDescription( szDesc );
                tx.SetAmount( monAmount );
                if ( _pdoc->AddTransaction( szAccount, tx ) )
                    {
                    UpdateAllViews( );
                    ClearEditBox( hwndAmount );
                    ClearEditBox( hwndDesc );
                    SetFocus( hwndAccount );
                    _pdoc->SetModifiedFlag( );
                    }
                else
                    {
                    MessageBox( hDlg, "The transaction could not be added.", _szAppName, MB_ICONEXCLAMATION );
                    SetFocus( hwndDate );
                    SelectEdit( hwndDate );
                    }
                }
            CATCH( exMemory )
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hDlg );
                PostMessage( hDlg, WM_CLOSE, 0, 0L );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                MessageBox( hDlg, "The operation has failed.", _szAppName, MB_OK );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            PostMessage( hDlg, WM_CLOSE, 0, 0L );
            return TRUE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export ConfirmProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    QString sDate;
    QTime   tDate;
    const ConfirmStruct *pcf;
    const QTxb *ptxb;
    CHAR  sz[ cbBufferMax + 1 ];

    static HWND hwndConfirmText;
    static HWND hwndSeq; 
    static HWND hwndDate;
    static HWND hwndDepositText;
    static HWND hwndDeposit;
    static HWND hwndBalanceText;
    static HWND hwndBalance;

    switch( message )
        {
    case WM_INITDIALOG:
        TRY
            {
            pcf  = (const ConfirmStruct *) lParam;
            ASSERT( pcf != NULL );
            ptxb = _pdoc->txbFromSeq( pcf->seq_ );
            if ( ptxb )
                {
                hwndConfirmText = GetDlgItem( hDlg, IDC_CONFIRMTEXT );
                hwndSeq         = GetDlgItem( hDlg, IDC_SEQ );
                hwndDate        = GetDlgItem( hDlg, IDC_DATE );
                hwndDepositText = GetDlgItem( hDlg, IDC_DEPOSITTEXT );
                hwndDeposit     = GetDlgItem( hDlg, IDC_DEPOSIT );
                hwndBalanceText = GetDlgItem( hDlg, IDC_BALANCETEXT );
                hwndBalance     = GetDlgItem( hDlg, IDC_BALANCE );

                if ( pcf->t_ == ConfirmStruct::Delete )
                    SetWindowText( hwndConfirmText, "Please confirm your intention to DELETE the following transaction:" );
                else
                    SetWindowText( hwndConfirmText, "Please confirm your intention to BALANCE the following transaction:" );

                sprintf( sz, "%ld", ptxb->GetSeq( ) );
                SetWindowText( hwndSeq, sz );

                tDate = ptxb->GetTransaction( ).GetDate( );
                tDate.Format( cbTransactionsFormatMax, _szTransactionsFormat );
                sDate = tDate;
                SetWindowText( hwndDate, sDate );

                Money amt = ptxb->GetTransaction( ).GetAmount( );
                if ( amt >= 0 )
                    {
                    SetWindowText( hwndDepositText, "Deposit:" );
                    SetWindowText( hwndDeposit, amt.asString( ) );
                    }
                else
                    {
                    SetWindowText( hwndDepositText, "Withdraw:" );
                    amt = -amt;
                    SetWindowText( hwndDeposit, amt.asString( ) );
                    }

                if ( pcf->t_ == ConfirmStruct::Balance )
                    {
                    const QTxb *ptxbGeneral = _pdoc->txbFromSeqGeneral( pcf->seq_ );
                    ASSERT( ptxbGeneral != NULL );
                    ASSERT( ptxb->GetTransaction( ).GetDate( )   == ptxbGeneral->GetTransaction( ).GetDate( ) );
                    ASSERT( ptxb->GetTransaction( ).GetAmount( ) == ptxbGeneral->GetTransaction( ).GetAmount( ) );
                    ASSERT( ptxb->GetSeq( ) == ptxbGeneral->GetSeq( ) );
                    
                    SetWindowText( hwndBalanceText, "General Balance:" );
                    Money balance = ptxbGeneral->GetBalance( );
                    SetWindowText( hwndBalance, balance.asString( ) );
                    }
                }
            else
                EndDialog( hDlg, cfError );
            }
        CATCH_ALL
            {
            Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
            ResourcesMessageBox( hDlg );
            EndDialog( hDlg, cfError );
            }
        END_CATCH
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            EndDialog( hDlg, cfYes );
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, cfNo );
            return TRUE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export BalanceProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND hwndSeq;
    CHAR szSeq[ cbBufferMax + 1 ];

    switch ( message )
        {
    case WM_INITDIALOG:
        hwndSeq = GetDlgItem( hDlg, IDC_SEQ );
        SendMessage( hwndSeq, EM_LIMITTEXT, cbBufferMax, 0L );
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                GetText( hwndSeq, szSeq, sizeof( szSeq ) );
                if ( !stringIsDigit( szSeq ) )
                    *szSeq = '\0';
                long seq = atol( szSeq );
                int rc = Confirm( hDlg, ConfirmStruct::Balance, seq );
                
                if ( rc == cfYes )
                    {
                    if ( _pdoc->Balance( seq ) )
                        {
                        UpdateAllViews( );
                        ClearEditBox( hwndSeq );
                        _pdoc->SetModifiedFlag( );
                        }
                    else
                        MessageBox( hDlg, "Balancing failed for that transaction number.", _szAppName, MB_ICONEXCLAMATION );
                    }
                else if ( rc == cfError )
                    MessageBox( hDlg, "Balancing failed for that transaction number.", _szAppName, MB_ICONEXCLAMATION );
                
                SetFocus( hwndSeq );
                SelectEdit( hwndSeq );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hDlg );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;

        case IDCANCEL:
            EndDialog( hDlg, TRUE );
            return FALSE;
            }
        }
    return FALSE;
    }

BOOL FAR PASCAL _export DeleteProc( HWND hDlg, UINT message, UINT wParam, LONG lParam )
    {
    static HWND hwndSeq;
    CHAR szSeq[ cbBufferMax + 1 ];
    
    switch ( message )
        {
    case WM_INITDIALOG:
        hwndSeq = GetDlgItem( hDlg, IDC_SEQ );
        SendMessage( hwndSeq, EM_LIMITTEXT, cbBufferMax, 0L );
        return TRUE;

    case WM_COMMAND:
        switch( wParam )
            {
        case IDOK:
            TRY
                {
                GetText( hwndSeq, szSeq, sizeof( szSeq ) );
                if ( !stringIsDigit( szSeq ) )
                    *szSeq = '\0';
                long seq = atol( szSeq );
                int rc = Confirm( hDlg, ConfirmStruct::Delete, seq );

                if ( rc == cfYes )
                    {
                    if ( _pdoc->DeleteTransaction( seq ) )
                        {
                        UpdateAllViews( );
                        ClearEditBox( hwndSeq );
                        _pdoc->SetModifiedFlag( );
                        }
                    else
                        MessageBox( hDlg, "The transaction could not be deleted.", _szAppName, MB_ICONEXCLAMATION );
                    }
                else if ( rc == cfError )
                    {
                    MessageBox( hDlg, "The transaction could not be deleted.", _szAppName, MB_ICONEXCLAMATION );
                    }
                SetFocus( hwndSeq );
                SelectEdit( hwndSeq );
                }
            CATCH_ALL
                {
                Trace( TR_NORMAL, _szOperationFailed, THIS_FILE, __LINE__ );
                ResourcesMessageBox( hDlg );
                EndDialog( hDlg, FALSE );
                }
            END_CATCH
            return TRUE;
            
        case IDCANCEL:
            EndDialog( hDlg, TRUE );
            return FALSE;
            }
        }
    return FALSE;
    }

BOOL FExtractEditValue( HWND hwnd, CHAR *pszEdit, int cbEdit )
    {
    BOOL fRet = FALSE;
    TASKLIST
        {
        SendMessage( hwnd, WM_GETTEXT, cbEdit, (LONG)(LPSTR) pszEdit );
        if ( !*pszEdit )
            {
            QUIT;
            }
        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

BOOL FParseDate( CHAR *szDate, int *pmonth, int *pday, int *pyear )
    {
    static CHAR szTokens[] = "/";
    CHAR *pch;
    BOOL fRet = FALSE;

    TASKLIST
        {
        pch = strtok( szDate, szTokens );
        if ( !pch )
            {
            QUIT;
            }
        *pmonth = atoi( pch );
        if ( *pmonth < 1 || *pmonth > 12 )
            {
            QUIT;
            }
        pch = strtok( NULL, szTokens );
        if ( !pch )
            {
            QUIT;
            }
        *pday = atoi( pch );
        pch = strtok( NULL, szTokens );
        if ( !pch )
            {
            QUIT;
            }
        *pyear = atoi( pch );
        pch = strtok( NULL, szTokens );
        if ( pch )
            {
            // can't let the guy keep going!
            QUIT;
            }
        if ( *pyear < 70 )
            {
            // assume he means 20xx
            *pyear += 2000;
            }
        else if ( *pyear < 100 )
            {
            // assume he means 19xx
            *pyear += 1900;
            }
        // else, accept year as it is

        fRet = TRUE;
        }
    ENDTASKLIST
    return fRet;
    }

static BOOL stringIsDigit( const CHAR *psz )
    {
    int i;

    for ( i = 0; i < lstrlen( psz ); ++i )
        if ( !isdigit( psz[ i ] ) )
            return FALSE;
        
    return TRUE;
    }
