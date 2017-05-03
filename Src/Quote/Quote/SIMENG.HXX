//////////////////////////////////////////////////////////////////////////////
// SIMENG.HXX
//
// This file generates simulator test results.
//
// Copyright (c) Ward Quinlan, 2002
//////////////////////////////////////////////////////////////////////////////
#ifndef _SIMENG_HXX
#define _SIMENG_HXX

//////////////////////////////////////////////////////////////////////////////
// Simulator Request structure
//////////////////////////////////////////////////////////////////////////////
struct SimulatorRequest
    {
    // Parent Window
    HWND hwndParent;

    // Rule Name
    QString sRuleName;

    // Performance Test Type
    enum PerformanceType
        {
        eLoadedCompany,
        eAllCompanies
        };
    PerformanceType type;

    // Initial Account Size, $
    NUM numInitialAccount;

    // Position Size, $
    NUM numPosition;

    // Commission, $
    NUM numComm;

    // File Extension Filter
    QString sFilter;

    SimulatorRequest( )
        : hwndParent( 0 ),
          type( eLoadedCompany ),
          numInitialAccount( 0.00 ),
          numPosition( 0.00 ),
          numComm( 0.00 )
        {
        }
    
    ~SimulatorRequest( )
        {
        Cleanup( );
        }
    
    void Cleanup( )
        {
        sRuleName.Cleanup( );
        sFilter.Cleanup( );
        }
    };

//////////////////////////////////////////////////////////////////////////////
// Simulator Results 'class' (structure)
//////////////////////////////////////////////////////////////////////////////
class SimulatorResults : public QListObject
    {
public:
    SimulatorResults( )
        :m_numPosition( 0 )
        {
        }

    ~SimulatorResults( )
        {
        Cleanup( );
        }

    void Cleanup( )
        {
        m_sName.Cleanup( );
        }
    
private:
    QString m_sName;
    NUM     m_numPosition;
    };
    
//////////////////////////////////////////////////////////////////////////////
// class SimulatorResultsList
//
// This class defines the results list collection
//////////////////////////////////////////////////////////////////////////////
class SimulatorResultsList : public QVirtualList
    {
public:
    SimulatorResultsList( )
        {
        }

    ~SimulatorResultsList( )
        {
        Cleanup( );
        }

    virtual void Cleanup( );

    void InsertResults( const SimulatorRequest &req, SimulatorResults *pres );
    
#ifdef DEBUG
    void AssertValid( ) const;
#endif

private:
    virtual void Abstract( )
        {
        }
    };

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous
//////////////////////////////////////////////////////////////////////////////
extern void Simulator( const SimulatorRequest &req );

#endif

