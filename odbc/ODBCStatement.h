// $Id: ODBCStatement.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_ODBCStatement_h
#define RDBC_ODBCStatement_h

//
// ODBC implementation of SQL Statements
//

#ifndef RDBC_TSQLStatement_h
#include <RDBC/TSQLStatement.h>
#endif

////////////////////////////////////////////////////////////////////
class ODBCStatement: public TSQLStatement
{
friend class ODBCConnection;
friend class ODBCResultSet;

protected:

   ODBCStatement( TSQLConnection* con, void* imp=0 );

public:
   
   virtual ~ODBCStatement();   
   using TSQLStatement::Execute;  // unhide (TMethod*, TObjArray*, int*)
   Bool_t Execute( const TString& sql="" );
   TSQLResultSet* ExecuteQuery( const TString& sql="" );
   Int_t  ExecuteUpdate( const TString& sql="" );
   TSQLResultSet* GetResultSet();
   Bool_t GetMoreResults();
   void   Close();
   void   Cancel();
   Int_t  GetMaxFieldSize();
   void   SetMaxFieldSize( Int_t max );
   Int_t  GetMaxRows();
   void   SetMaxRows( Int_t max );
   Bool_t GetEscapeProcessing();
   void   SetEscapeProcessing( Bool_t enable = kTRUE );
   Int_t  GetQueryTimeout();
   void   SetQueryTimeout( Int_t seconds );  
   Int_t  GetUpdateCount();
   void   SetCursorName( const TString& name );
   void   AddBatch( const TString& sql );
   void   ClearBatch();
   Int_t* ExecuteBatch();
   Int_t  GetFetchDirection();
   void   SetFetchDirection( Int_t direction );    
   Int_t  GetFetchSize(); 
   void   SetFetchSize(Int_t rows);
   Int_t  GetResultSetConcurrency(); 
   Int_t  GetResultSetType();

ClassDef(ODBCStatement,0)//Class used for executing a SQL statement and obtaining the results produced by it
};

#endif // RDBC_ODBCStatement_h
