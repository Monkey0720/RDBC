// $Id: TSQLStatement.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLStatement_h
#define RDBC_TSQLStatement_h

//
// Implementation of SQL Statements
//

#ifndef RDBC_TSQL_h
#include <RDBC/TSQL.h>
#endif
#ifndef ROOT_TObject
#include "TObject.h"
#endif

class TSQLResultSet;
class TSQLConnection; 
class TList;
////////////////////////////////////////////////////////////////////
class TSQLStatement: public TObject, public TSQL
{
protected:
   TList*   fBatches;               // list of batches
   TSQLResultSet* fCurrentResult;   // current result set   
   TSQLConnection* fConnection;     // connection that owns this

   TSQLStatement( TSQLConnection* con, void* imp=0 );
   
public:
   // override TObject's 
   virtual void Execute (const char *, const char *, int *) {}
   virtual void Execute (TMethod *, TObjArray *, int *) {}

   virtual ~TSQLStatement();
   virtual TSQLResultSet* ExecuteQuery( const TString& sql="" ) = 0;
   virtual Int_t  ExecuteUpdate( const TString& sql="" ) = 0;
   virtual Bool_t Execute( const TString& sql="" ) = 0;
   virtual TSQLResultSet*  GetResultSet() = 0;
   virtual Bool_t GetMoreResults() = 0;
   virtual void   Close() = 0;
   virtual void   Cancel() = 0;
   virtual Int_t  GetMaxFieldSize() = 0;
   virtual void   SetMaxFieldSize( Int_t max ) = 0;
   virtual Int_t  GetMaxRows() = 0;
   virtual void   SetMaxRows( Int_t max ) = 0;
   virtual Bool_t GetEscapeProcessing() = 0;
   virtual void   SetEscapeProcessing( Bool_t enable = kTRUE ) = 0;
   virtual Int_t  GetQueryTimeout() = 0;
   virtual void   SetQueryTimeout( Int_t seconds ) = 0;  
   virtual Int_t  GetUpdateCount() = 0;
   virtual void   SetCursorName( const TString& name ) = 0;
   virtual void   AddBatch( const TString& sql ) = 0; 
   virtual void   ClearBatch() = 0;
   virtual Int_t* ExecuteBatch() = 0;
   virtual Int_t  GetFetchDirection() = 0;
   virtual void   SetFetchDirection( Int_t direction ) = 0;    
   virtual Int_t  GetFetchSize() = 0; 
   virtual void   SetFetchSize(Int_t rows) = 0;
   virtual Int_t  GetResultSetConcurrency() = 0; 
   virtual Int_t  GetResultSetType() = 0;

   TSQLConnection* GetConnection() const { return fConnection; }

ClassDef(TSQLStatement,0)//Class used for executing a SQL statement and obtaining the results produced by it
};

#endif // RDBC_TSQLStatement
