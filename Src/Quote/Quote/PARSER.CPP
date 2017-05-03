//////////////////////////////////////////////////////////////////////////////
// PARSER.CPP
//
// This file contains the template class methods
//
// Copyright (c) Ward Quinlan, 2002
//////////////////////////////////////////////////////////////////////////////
#include <qobject.hxx>
#include <windowsx.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <statpak.h>
#include "types.hxx"
#include "resource.hxx"
#include "ot.hxx"
#include "template.hxx"
#include "doc.hxx"
#include "quote.hxx"
#include "tools.hxx"

#ifdef DEBUG
static const CHAR THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// QParser::m_szTokenDelimiters[ ]
//
// QParser token delimiters.
//////////////////////////////////////////////////////////////////////////////
const CHAR QParser::m_szTokenDelimiters[ ] = " \t\r\n";

//////////////////////////////////////////////////////////////////////////////
// QParser::program( )
//
// This parse routine, based on the parse2 program, will parse a single
// line of input, indexed at offset i.
//
// Grammar:
// ========
// 
// program    ::= rule
//
// rule       ::= condition<end-of-input>
//
// condition  ::= condition and comparison
//            ::= condition or  comparison
//            ::= comparison
//
// comparison ::= expr <  expr
//            ::= expr <= expr
//            ::= expr >  expr
//            ::= expr >= expr
//            ::= expr == expr
//            ::= expr != expr
//            ::= [condition]
//            ::= $false
//            ::= $true
//
// expr       ::= expr + term
//            ::= expr - term
//            ::= term
//
// term       ::= term * primary
//            ::= term / primary
//            ::= primary
//
// primary    ::= <floating point number>
//            ::= #<indicator-name>
//            ::= $<native-name>
//            ::= ~<rule-name>
//            ::= +primary
//            ::= -primary
//            ::= (expr)
//
// Note the more precedence an operation has
// the 'lower' it goes in the grammar.
// Unary minus has very high precedence; hence,
// it goes in primary( ).
//
//
// Parameters:
// ===========
//
// i      - The current index from the first Quote.
// iter   - The token list iterator object.
// fValue - The returns value of the parse expression.
//
// Returns: 
// ========
//
// QParser::rcOk         - on success
// QParser::rcUnAvail    - on data unavailable for given offset
// QParser::rcFatalError - on fatal errors e.g., syntax errors
//////////////////////////////////////////////////////////////////////////////
QParser::Rc QParser::program( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel,
                              BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail )
    {
    return rule( i, iter, fValue, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::rule( )
//
// Parses rules, according to parse2.
//////////////////////////////////////////////////////////////////////////////
QParser::Rc QParser::rule( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel,
                           BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail )
    {
    Rc rc = condition( i, iter, fValue, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
    
    if ( rc != rcOk )
        {
        return rc;
        }
        
    QToken *ptk = iter( ); // read a token
    if ( ptk )
        {
        Trace( TR_NORMAL, "rule: Unexpected token at end-of-rule.\n" );
        return rcFatalError;
        }            
    return rcOk;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::condition( )
//
// Parses conditions, according to parse2.
//////////////////////////////////////////////////////////////////////////////
QParser::Rc QParser::condition( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel,
                                BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail )
    {   
    QToken *ptk;    
    BOOL    fValue2;
    Rc      rc = comparison( i, iter, fValue, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
        
    if ( rc != rcOk )
        {
        return rc;
        }
        
    while ( 1 )
        {
        ptk = iter.Peek( );
        if ( !ptk )
            {
            break;
            }
        if ( ptk->GetTk( ) == QToken::tkAnd )
            {
            iter( ); // eat the 'and'
            rc = comparison( i, iter, fValue2, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
            if ( rc != rcOk )
                {
                return rc;
                }
            fValue = fValue && fValue2;
            }
        else if ( ptk->GetTk( ) == QToken::tkOr )
            {
            iter( ); // eat the 'or'
            rc = comparison( i, iter, fValue2, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
            if ( rc != rcOk )
                {
                return rc;
                }
            fValue = fValue || fValue2;
            }
        else
            {
            break;
            }
        }
    return rcOk;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::comparison( )
//
// Parses comparisons, according to parse2.
//////////////////////////////////////////////////////////////////////////////
QParser::Rc QParser::comparison( UINT i, QTokenIter &iter, BOOL &fValue, int nRecurseLevel,
                                 BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail )
    {                                                                
    NUM     a;
    NUM     b;
    QToken *ptk = iter.Peek( );
    Rc      rc;
    
    if ( !ptk )
        {
        Trace( TR_NORMAL, "comparison: Expecting token.\n" );
        return rcFatalError;
        }
    
    if ( ptk->GetTk( ) == QToken::tkLBracket )
        {
        iter( ); // eat the '['
        rc = condition( i, iter, fValue, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
        if ( rc != rcOk )
            {
            return rc;
            }
        ptk = iter( ); // eat the ']'
        if ( !ptk || ptk->GetTk( ) != QToken::tkRBracket )
            {
            Trace( TR_NORMAL, "comparison: Expecting ']' character.\n" );
            return rcFatalError;;
            }
        return rcOk;
        }
    
    if ( ptk->GetTk( ) == QToken::tkNative )
        {
        if ( lstrcmp( ptk->GetName( ), "false" ) == 0 )
            {
            iter( ); // eat the token
            fValue = FALSE;
            return rcOk;
            }
        else if ( lstrcmp( ptk->GetName( ), "true" ) == 0 )
            {
            iter( ); // eat the token
            fValue = TRUE;
            return rcOk;
            }
        // else, fall through
        }

    rc = expr( i, iter, a, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
    if ( rc != rcOk )
        {
        return rc;
        }
    
    ptk = iter.Peek( );
    if ( !ptk )
        {
        Trace( TR_NORMAL, "comparison: Expecting token.\n" );
        return rcFatalError;
        }
    
    switch( ptk->GetTk( ) )
        {
    case QToken::tkLess:
    case QToken::tkGreater:
    case QToken::tkLessEqual:
    case QToken::tkGreaterEqual:
    case QToken::tkEqual:
    case QToken::tkNotEqual:
        iter( ); // read the token
        rc = expr( i, iter, b, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
        if ( rc != rcOk )
            {
            return rc;
            }
        break;
    default:
        Trace( TR_NORMAL, "comparison: expecting comparison.\n" );
        return rcFatalError;
        } // switch
        
    switch( ptk->GetTk( ) )
        {
    case QToken::tkLess:
        fValue = ( a < b );
        return rcOk;
    case QToken::tkGreater:
        fValue = ( a > b );
        return rcOk;
    case QToken::tkLessEqual:
        fValue = ( a <= b );
        return rcOk;
    case QToken::tkGreaterEqual:
        fValue = ( a >=b );
        return rcOk;
    case QToken::tkEqual:
        fValue = ( a == b );
        return rcOk;
    case QToken::tkNotEqual:
        fValue = ( a != b );
        return rcOk;
        }    
    ASSERT( FALSE );
    return rcFatalError;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::expr( )
//
// Parses expressions, according to parse2.
//////////////////////////////////////////////////////////////////////////////
QParser::Rc QParser::expr( UINT i, QTokenIter &iter, NUM &numValue, int nRecurseLevel,
                           BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail )
    {
    QToken *ptk;
    NUM     a;      
    NUM     b;
    Rc      rc = term( i, iter, a, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
    
    if ( rc != rcOk )
        {
        return rc;
        }
    
    while ( 1 )
        {
        ptk = iter.Peek( );
        if ( !ptk )
            {     
            // an expression could end on end-of-input
            break;
            }
        if ( ptk->GetTk( ) == QToken::tkPlus )
            {    
            iter( ); // read the token
            rc = term( i, iter, b, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
            if ( rc != rcOk )
                {
                return rc;
                }
            a += b;
            }
        else if ( ptk->GetTk( ) == QToken::tkMinus )
            {
            iter( ); // read the token
            rc = term( i, iter, b, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
            if ( rc != rcOk )
                {
                return rc;
                }
            a -= b;
            }
        else
            {
            break;
            }
        }       
    numValue = a;
    return rcOk;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::term( )
//
// Parses terms, according to parse2.
//////////////////////////////////////////////////////////////////////////////
QParser::Rc QParser::term( UINT i, QTokenIter &iter, NUM &numValue, int nRecurseLevel,
                           BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail )
    {    
    QToken *ptk;
    NUM     a;
    NUM     b;
    Rc      rc = primary( i, iter, a, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
    
    if ( rc != rcOk )
        {
        return rc;
        }
    
    while ( 1 )
        {       
        ptk = iter.Peek( );
        if ( !ptk )
            {     
            // a term could end of end-of-input
            break;
            }
        if ( ptk->GetTk( ) == QToken::tkTimes )
            {   
            iter( ); // read the token
            rc = primary( i, iter, b, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
            if ( rc != rcOk )
                {            
                return rc;
                }
            a *= b;
            }
        else if ( ptk->GetTk( ) == QToken::tkDivide )
            {
            iter( ); // read the token
            rc = primary( i, iter, b, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
            if ( rc != rcOk )
                {            
                return rc;
                }
            if ( b == (NUM) 0.0 )
                {
                Trace( TR_NORMAL, "term: Divide by zero.\n" );
                return rcFatalError;
                }
            a /= b;
            }
        else
            {
            break;
            }
        }    
    numValue = a;
    return rcOk;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::primary( )
//
// Parses primaries, according to parse2.
//////////////////////////////////////////////////////////////////////////////
QParser::Rc QParser::primary( UINT i, QTokenIter &iter, NUM &numValue, int nRecurseLevel,
                              BOOL fScalarFunc, NUM numEntry, UINT iEntry, NUM numTrail )
    {            
    CHAR    szFunction[ _cbFuncNameMax  + 1 ];
    CHAR    szFloat   [ _cbFloatMax     + 1 ];
    NUM     a;
    QToken *ptk = iter.Peek( );
    Rc      rc;
    
    if ( !ptk )
        {
        Trace( TR_NORMAL, "primary: Expecting primary.\n" );
        return rcFatalError;
        }            
    
    switch( ptk->GetTk( ) )
        {
    case QToken::tkFloat:
        iter( ); // read the token
        numValue = ptk->GetValue( );
        return rcOk;
    
    case QToken::tkMinus:
        iter( ); // read the token
        rc = primary( i, iter, a, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
        if ( rc != rcOk )
            {
            return rc;
            }
        numValue = -a;
        return rcOk;
    
    case QToken::tkPlus:
        iter( ); // read the token
        return primary( i, iter, numValue, nRecurseLevel, fScalarFunc, numEntry, iEntry, numTrail );
    
    case QToken::tkLParen:
        iter( ); // read the token
        rc = expr( i, iter, a, nRecurseLevel, fScalarFunc, numEntry, iEntry );
        if ( rc != rcOk )
            {
            return rc;
            }
        ptk = iter( );
        if ( !ptk || ptk->GetTk( ) != QToken::tkRParen )
            {
            Trace( TR_NORMAL, "primary: expecting right paren.\n" );
            return rcFatalError;
            }
        numValue = a;
        return rcOk;
        
    case QToken::tkScalarFunc:
        {          
        BOOL fStopFunction = FALSE;
        NUM  numStopValue = 0;
        if ( !fScalarFunc )
            {
            Trace( TR_NORMAL, "primary: scalar function unsupported here.\n" );
            return rcFatalError;
            }
        
        iter( ); // read the token
        
        int cbFunc = lstrlen( ptk->GetName( ) );
        if ( cbFunc > _cbFuncNameMax )
            return rcFatalError;
        lmemset( szFunction, 0, sizeof( szFunction ) );
        lstrcpy( szFunction, ptk->GetName( ) );
        fStopFunction = ( lstrcmp( szFunction, "mms" ) == 0 ||
                          lstrcmp( szFunction, "trs" ) == 0 );
                
        // read the left paren
        ptk = iter( );
        if ( !ptk || ptk->GetTk( ) != QToken::tkLParen )
            {
            Trace( TR_NORMAL, "primary: expecting left paren.\n" );
            return rcFatalError;
            }
        
        // read the parameter if a stop function
        if ( fStopFunction )
            {
            ptk = iter( );
            if ( !ptk || ptk->GetTk( ) != QToken::tkFloat )
                {
                Trace( TR_NORMAL, "primary: expecting stop parameter.\n" );
                return rcFatalError;
                }
            numStopValue = ptk->GetValue( );
            }

        // read the right paren
        ptk = iter( );
        if ( !ptk || ptk->GetTk( ) != QToken::tkRParen )
            {
            Trace( TR_NORMAL, "primary: expecting right paren.\n" );
            return rcFatalError;
            }
        
        if ( lstrcmp( szFunction, "etp" ) == 0 )
            {
            numValue = numEntry;
            Trace( TR_NORMAL, "etp[%u] = %f\n", i, numValue );
            }
        else if ( lstrcmp( szFunction, "bsi" ) == 0 )
            {                
            ASSERT( i > iEntry );
            numValue = (NUM) ( i - iEntry );
            Trace( TR_NORMAL, "bsi[%u] = %f\n", i, numValue );
            }
        else if ( lstrcmp( szFunction, "mms" ) == 0 )
            {
            numValue = numEntry * numStopValue;
            Trace( TR_NORMAL, "mms[%u] = %f\n", i, numValue );
            }
        else if ( lstrcmp( szFunction, "trs" ) == 0 )
            {
            numValue = numTrail * numStopValue;
            Trace( TR_NORMAL, "trs[%u] = %f\n", i, numValue );
            }
        else
            {
            Trace( TR_NORMAL, "primary: unsupported scalar function.\n" );
            return rcFatalError;
            }
        return rcOk;
        }
        
    case QToken::tkFunc:
        {
        int cbFunc = 0;
        BOOL fValid = FALSE;
        iter( ); // read the token
        
        lmemset( szFunction, 0, sizeof( szFunction ) );
        lstrcpy( szFunction, "@" );
        cbFunc = 1;

        cbFunc += lstrlen( ptk->GetName( ) );
        if ( cbFunc > _cbFuncNameMax )
            return rcFatalError;
        lstrcat( szFunction, ptk->GetName( ) );

        // read the left paren
        ptk = iter( );
        if ( !ptk || ptk->GetTk( ) != QToken::tkLParen )
            {
            Trace( TR_NORMAL, "primary: expecting left paren.\n" );
            return rcFatalError;
            }
        
        // read the first parameter/right bracket
        ptk = iter( );
        while ( ptk )
            {
            if ( ptk->GetTk( ) == QToken::tkRParen )
                {
                // This case will handle empty functions
                fValid = TRUE;
                break;
                }
            
            if ( ptk->GetTk( ) == QToken::tkNative || 
                 ptk->GetTk( ) == QToken::tkIndicator )
                {
                cbFunc += 2;
                if ( cbFunc > _cbFuncNameMax )
                    break;
                lstrcat( szFunction, ptk->GetTk( ) == QToken::tkNative ? "@$" : "@#" );
                
                cbFunc += lstrlen( ptk->GetName( ) );
                if ( cbFunc > _cbFuncNameMax )
                    break;
                lstrcat( szFunction, ptk->GetName( ) );
                }
            else if ( ptk->GetTk( ) == QToken::tkFloat )
                {
                sprintf( szFloat, "@%f", ptk->GetValue( ) );
                cbFunc += lstrlen( szFloat );
                if ( cbFunc > _cbFuncNameMax )
                    break;
                lstrcat( szFunction, szFloat );
                }
            else
                {
                // Error case
                break;
                }

            // read the comma/right bracket
            ptk = iter( );
            if ( !ptk )
                break;

            if ( ptk->GetTk( ) == QToken::tkRParen )
                {
                fValid = TRUE;
                break;
                }
            
            if ( ptk->GetTk( ) != QToken::tkComma )
                {
                // Error case
                break;
                }
            
            // read the next parameter/right bracket
            ptk = iter( );
            }
        if ( !fValid )
            {
            Trace( TR_NORMAL, "primary: fatal error evaluating function.\n" );
            return rcFatalError;
            }
        
        const QCache *pcache = gcache.pcache( szFunction, nRecurseLevel );
        if ( !pcache )
            {
            return rcFatalError;
            }
        long j = (long) i - (long) pcache->m_dataset.nOffset;
        if ( j < 0 )
            {
            return rcUnAvail;
            }
        ASSERT( j < (long) pcache->m_dataset.nPoints );
        numValue = pcache->m_dataset.lpnum[ j ];
        return rcOk;
        }

    case QToken::tkNative:
        iter( ); // read the token
        ASSERT( ptk->GetName( ) != NULL );
        if ( lstrcmp( _pdoc->m_szCl, ptk->GetName( ) ) == 0 )
            {            
            ASSERT( i < gcache.cl( ).nPoints );
            numValue = gcache.cl( ).lpnum[ i ];
            }
        else if ( lstrcmp( _pdoc->m_szOp, ptk->GetName( ) ) == 0 )
            {
            ASSERT( i < gcache.hi( ).nPoints );
            numValue = gcache.op( ).lpnum[ i ];
            }
        else if ( lstrcmp( _pdoc->m_szHi, ptk->GetName( ) ) == 0 )
            {
            ASSERT( i < gcache.hi( ).nPoints );
            numValue = gcache.hi( ).lpnum[ i ];
            }
        else if ( lstrcmp( _pdoc->m_szLo, ptk->GetName( ) ) == 0 )
            {
            ASSERT( i < gcache.lo( ).nPoints );
            numValue = gcache.lo( ).lpnum[ i ];
            }
        else if ( lstrcmp( _pdoc->m_szVl, ptk->GetName( ) ) == 0 )
            {
            ASSERT( i < gcache.vl( ).nPoints );
            numValue = gcache.vl( ).lpnum[ i ];
            }
        else if ( lstrcmp( _pdoc->m_szX, ptk->GetName( ) ) == 0 )
            {
            ASSERT( i < gcache.x( ).nPoints );
            numValue = gcache.x( ).lpnum[ i ];
            }
        else if ( lstrcmp( _pdoc->m_szCount, ptk->GetName( ) ) == 0 )
            {
            ASSERT( i < gcache.count( ).nPoints );
            numValue = gcache.count( ).lpnum[ i ];
            }
        else
            {
            return rcFatalError;
            }
        return rcOk;
        
    case QToken::tkIndicator:
        {
        iter( ); // read the token
        const QCache *pcache = (QCache *) gcache.pcache( ptk->GetName( ), nRecurseLevel );
        if( !pcache )
            {
            return rcFatalError;
            }
        long j = (long) i - (long) pcache->m_dataset.nOffset;
        if ( j < 0 )
            {
            return rcUnAvail;
            }
        ASSERT( j < (long) pcache->m_dataset.nPoints );
        numValue = pcache->m_dataset.lpnum[ j ];
        return rcOk;
        }

    case QToken::tkCond:
        {
        iter( ); // read the token
        const QCondCache *pcondcache = (QCondCache *) gcache.pcondcache( ptk->GetName( ), nRecurseLevel );
        if ( !pcondcache )
            {
            return rcFatalError;
            }
        long j = (long) i - (long) pcondcache->DataSet( ).nOffset;
        if ( j < 0 )
            {
            return rcUnAvail;
            }
        ASSERT( j < (long) pcondcache->DataSet( ).nPoints );
        numValue = pcondcache->DataSet( ).lpnum[ j ];
        return rcOk;
        }

    case QToken::tkRule:
        {
        WORD n;
        iter( ); // read the token
        const QRuleCache *prulecache = (QRuleCache *) gcache.prulecache( ptk->GetName( ), nRecurseLevel );
        if ( !prulecache )
            {
            return rcFatalError;
            }
        // If rule A uses rule B, then rule A can't have an
        // offset of < than that of rule B
        if ( i < prulecache->DataSet( ).nOffset )
            {
            return rcUnAvail;
            }
        n = i - prulecache->DataSet( ).nOffset;
        ASSERT( n < prulecache->DataSet( ).nPoints );
        numValue = prulecache->DataSet( ).lpnum[ n ];
        ASSERT( numValue == (NUM) 1 || numValue == (NUM) 0 );
        return rcOk;
        }
        
        } // switch
    
    Trace( TR_NORMAL, "primary: unexpected primary.\n" );
    return rcFatalError;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::PostAnalyze( )
//
// This member will perform 'post-analysis'
//////////////////////////////////////////////////////////////////////////////
BOOL QParser::PostAnalyze( QTokenList &list )
    {
    BOOL fError( FALSE );

    for ( QToken *ptk = (QToken *) list.GetHead( );
          ptk;
          ptk = (QToken *) list.GetNext( ptk ) )
        {
        if ( ptk->GetTk( ) == QToken::tkConst )
            {
            QConstant *pconst = _ptpl->m_constlist.Find( ptk->GetName( ) );
            if ( pconst == NULL )
                {
                fError = TRUE;
                break;
                }
            ptk->SetTk( QToken::tkFloat );
            ptk->SetValue( pconst->GetValue( ) );
            }
        }

    return !fError;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::Analyze( )
//
// This member will perform lexical analysis.
//
// Parameters:
//     list        - Token list to fill in.
//     sTokens     - String of the expression.
//     fFullParser - If FALSE, only enables expressions and below.
//                   If TRUE, enables full parser, including comparisons.
//
// Returns: 
//     TRUE If the analysis occurred successfully.
//     FALSE otherwise.
//////////////////////////////////////////////////////////////////////////////
BOOL QParser::Analyze( QTokenList &list, const QString &sTokens, BOOL fFullParser )
    {               
    BOOL fError( FALSE );
    BOOL fAdded( FALSE );
    CHAR *psz      = 0;
    CHAR *pszToken = 0;
    QToken *ptk    = 0;
    
    TRY
        {
        psz = new CHAR[ sTokens.GetLength( ) + 1 ];
        if ( !psz )
            {
            THROW_EXCEPTION( exMemory );
            }
        lstrcpy( psz, sTokens );
                
        pszToken = strtok( psz, m_szTokenDelimiters );
        while ( !fError && pszToken )
            {
            while ( !fError && *pszToken )
                {
                fAdded = FALSE;
                ptk = new QToken;
                if ( !ptk )
                    {
                    THROW_EXCEPTION( exMemory );
                    }
                if ( *pszToken == '[' )
                    {
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkLBracket );
                        pszToken++;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == ']' )
                    {
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkRBracket );
                        pszToken++;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == ',' )
                    {
                    ptk->SetTk( QToken::tkComma );
                    pszToken++;
                    }
                else if ( *pszToken == '(' )
                    {          
                    ptk->SetTk( QToken::tkLParen );
                    pszToken++;
                    }
                else if ( *pszToken == ')' )
                    {          
                    ptk->SetTk( QToken::tkRParen );
                    pszToken++;
                    }
                else if ( *pszToken == '+' )
                    {          
                    ptk->SetTk( QToken::tkPlus );
                    pszToken++;
                    }
                else if ( *pszToken == '-' )
                    {                            
                    ptk->SetTk( QToken::tkMinus );
                    pszToken++;
                    }
                else if ( *pszToken == '*' )
                    {          
                    ptk->SetTk( QToken::tkTimes );
                    pszToken++;
                    }
                else if ( *pszToken == '/' )
                    {          
                    ptk->SetTk( QToken::tkDivide );
                    pszToken++;
                    }
                // These 2 have to appear before '<' and '>'
                else if ( memcmp( pszToken, "<=", 2 ) == 0 )
                    {
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkLessEqual );
                        pszToken += 2;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( memcmp( pszToken, ">=", 2 ) == 0 )
                    {
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkGreaterEqual );
                        pszToken += 2;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '<' )
                    {          
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkLess );
                        pszToken++;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '>' )
                    {          
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkGreater );
                        pszToken++;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '#' )
                    {    
                    ptk->SetTk( QToken::tkIndicator );
                    pszToken++; // consume the '#' character
                    UINT cb = 0;
                    if ( ReadSymbol( ptk, pszToken, cb ) )
                        {
                        pszToken += cb;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '$' )
                    {
                    ptk->SetTk( QToken::tkNative );
                    pszToken++; // consume the '$' character
                    UINT cb = 0;
                    if ( ReadSymbol( ptk, pszToken, cb ) )
                        {
                        pszToken += cb;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '^' )
                    {
                    ptk->SetTk( QToken::tkConst );
                    pszToken++; // consume the '^' character
                    UINT cb = 0;
                    if ( ReadSymbol( ptk, pszToken, cb ) )
                        {
                        pszToken += cb;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '@' )
                    {
                    pszToken++; // consume the '@' character
                    UINT cb = 0;
                    if ( ReadSymbol( ptk, pszToken, cb ) )
                        {
                        if ( lstrcmp( ptk->GetName( ), "etp" ) == 0 ||
                             lstrcmp( ptk->GetName( ), "bsi" ) == 0 ||
                             lstrcmp( ptk->GetName( ), "mms" ) == 0 ||
                             lstrcmp( ptk->GetName( ), "trs" ) == 0 )
                            {
                            ptk->SetTk( QToken::tkScalarFunc );
                            }
                        else
                            {
                            ptk->SetTk( QToken::tkFunc );
                            }
                        pszToken += cb;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '~' )
                    {
                    ptk->SetTk( QToken::tkRule );
                    pszToken++; // consume the '~' character
                    UINT cb = 0;
                    if ( ReadSymbol( ptk, pszToken, cb ) )
                        {
                        pszToken += cb;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( *pszToken == '?' )
                    {
                    ptk->SetTk( QToken::tkCond );
                    pszToken++; // consume the '?' character
                    UINT cb = 0;
                    if ( ReadSymbol( ptk, pszToken, cb ) )
                        {
                        pszToken += cb;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( memcmp( pszToken, "==", 2 ) == 0 )
                    {                              
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkEqual );
                        pszToken += 2;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( memcmp( pszToken, "!=", 2 ) == 0 )
                    {
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkNotEqual );
                        pszToken += 2;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( memcmp( pszToken, "and", 3 ) == 0 )
                    {
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkAnd );
                        pszToken += 3;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( memcmp( pszToken, "or", 2 ) == 0 )
                    {
                    if ( fFullParser )
                        {
                        ptk->SetTk( QToken::tkOr );
                        pszToken += 2;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else if ( isdigit( *pszToken ) || *pszToken == '.' )
                    {
                    ptk->SetTk( QToken::tkFloat );
                    UINT cb = 0;
                    if ( ReadValue( ptk, pszToken, cb ) )
                        {
                        pszToken += cb;
                        }
                    else
                        {
                        fError = TRUE;
                        }
                    }
                else
                    {
                    // error case
                    fError = TRUE;
                    }             
                if ( !fError )
                    {
                    list.AddTail( ptk );
                    fAdded = TRUE;
                    }
                } // while
            if ( !fError )
                {
                pszToken = strtok( NULL, m_szTokenDelimiters );
                }
            } // while
        
        // Perform post-analysis
        if ( !fError )
            {
            fError = !QParser::PostAnalyze( list );
            }
        }
    CATCH_ALL
        {
        delete psz;
        if ( !fAdded )
            {
            delete ptk;
            }
        THROW_LAST( );
        }
    END_CATCH
    delete psz;
    if ( !fAdded )
        {
        delete ptk;
        }
    return !fError;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::ReadSymbol( )
//
// This routine will read a symbol into the passed token.
//////////////////////////////////////////////////////////////////////////////
BOOL QParser::ReadSymbol( QToken *ptk, const CHAR *pszToken, UINT &cb )
    {              
    CHAR szName[ _cbNameMax + 1 ];
    UINT n( 0 );              
    
    lmemset( szName, 0, sizeof( szName ) );
    while ( isalnum( *pszToken ) || *pszToken == '_' )
        {
        if ( n >= _cbNameMax )
            {
            return FALSE;
            }
        szName[ n ] = *pszToken;
        pszToken++; // increment our local copy
        n++;
        }
    ASSERT( (UINT) lstrlen( szName ) == n );
        
    if ( !*szName )
        {
        // zero-length token
        return FALSE;
        }
    
    ptk->SetName( szName );
    cb = n;
    return TRUE;
    }

//////////////////////////////////////////////////////////////////////////////
// QParser::ReadValue( )
//
// This routine will read a floating-point value into the passed token.
//////////////////////////////////////////////////////////////////////////////
BOOL QParser::ReadValue( QToken *ptk, const CHAR *pszToken, UINT &cb )
    {                                                           
    CHAR szValue[ _cbNameMax + 1 ]; // let values have a max. size of names.
    UINT n( 0 );
    UINT nDecimalPoints( 0 );
    
    lmemset( szValue, 0, sizeof( szValue ) );
    while ( TRUE )
        {
        if ( *pszToken == '.' )
            {
            nDecimalPoints++;
            }
        else if ( !isdigit( *pszToken ) )
            {
            break;
            }
        
        if ( n >= _cbNameMax )
            {
            return FALSE;
            }
        
        szValue[ n ] = *pszToken;
        pszToken++; // increment our local copy
        n++;
        }   
    ASSERT( (UINT) lstrlen( szValue ) == n );
    
    if ( !*szValue )
        {
        // zero-length token
        return FALSE;
        }
    
    if ( nDecimalPoints > 1 )
        {
        // can't have more than 1 decimal point
        return FALSE;
        }
    
    if ( szValue[ n - 1 ] == '.' )
        {
        // decimal can't be at the end
        return FALSE;
        }
            
    ptk->SetValue( (NUM) atof( szValue ) );
    cb = n;
    return TRUE;
    }
