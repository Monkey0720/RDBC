// $Id: ODBCConnection.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_ODBCConnection_h
#define RDBC_ODBCConnection_h

//
//  ODBCConnection - connection to data base via ODBC
//

#ifndef RDBC_TSQLConnection_h
#include <RDBC/TSQLConnection.h>
#endif

////////////////////////////////////////////////////////////////////
class ODBCConnection: public TSQLConnection
{
private:
   virtual ~ODBCConnection(); 
public:
   Bool_t   GetAutoCommit(); 
   void     SetAutoCommit( Bool_t autoCommit = kTRUE );   
   TString  GetCatalog(); 
   void     SetCatalog( const TString& catalog );
   Bool_t   IsReadOnly(); 
   void     SetReadOnly( Bool_t readOnly = kTRUE );
   Bool_t   GetTrace();
   void     SetTrace( Bool_t trace = kTRUE );
   void     SetTraceFile( const TString& filename );
   TString  GetTraceFile();
   Int_t    GetTransactionIsolation(); 
   void     SetTransactionIsolation( Int_t level );
   TString  NativeSQL( const TString& sql ); 
   void     Close();
   void     Commit();
   void     Rollback();
   TSQLStatement* CreateStatement(); 
   TSQLStatement* CreateStatement( Int_t resultSetType, 
                                   Int_t resultSetConcurrency );
   TSQLPreparedStatement* PrepareStatement( const TString& sql );
   TSQLPreparedStatement* PrepareStatement( const TString& sql,
                                            Int_t resultSetType,
                                            Int_t resultSetConcurrency );
   TSQLCallableStatement* PrepareCall( const TString& sql );
   TSQLCallableStatement* PrepareCall( const TString& sql,
                                       Int_t resultSetType,
                                       Int_t resultSetConcurrency );
   TSQLDatabaseMetaData* GetMetaData();
   Bool_t HasBatchSupport();

///  methods below are private ...
public:
    
   ODBCConnection( const TString& url, const TString& username, 
                   const TString& password ); 
   ODBCConnection( const TString& connectString );

   static Int_t   GetLoginTimeout();
   static void    SetLoginTimeout( Int_t seconds );
   static void    Shutdown();
   static TList*  RefreshDataSources(TList* datasoures);
   static TList*  RefreshDrivers(TList* drivers);  

ClassDef(ODBCConnection,0)//A connection to a specific database. 
};

#endif // RDBC_ODBCConnection_h
