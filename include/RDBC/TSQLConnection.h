// $Id: TSQLConnection.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLConnection_h
#define RDBC_TSQLConnection_h

//
//  TSQLConnection - connection to DataBase
//

#ifndef RDBC_TSQL_h
#include <RDBC/TSQL.h>
#endif
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TRefCnt
#include "TRefCnt.h"
#endif

class TSQLStatement;
class TSQLPreparedStatement;
class TSQLCallableStatement;
class TSQLDriverManager;
class TSQLDatabaseMetaData;
class TList;
 
enum ETransactionIsolation {  
   kTRANSACTION_NONE = 0,
   kTRANSACTION_READ_COMMITTED = 0x00000001L,
   kTRANSACTION_READ_UNCOMMITTED = 0x00000002L,
   kTRANSACTION_REPEATABLE_READ = 0x00000004L,  
   kTRANSACTION_SERIALIZABLE = 0x00000008L,
   kTRANSACTION_VERSIONING  = 0x00000010L     
};

////////////////////////////////////////////////////////////////////
class TSQLConnection: public TObject, public TSQL, public TRefCnt
{
friend class TSQLDriverManager;

protected:
   TSQLDatabaseMetaData* fMetaData; // database metadata
   TList*   fListOfStatements;      // list of SQL statemens

   TSQLConnection( const TString& url, const TString& username, 
                   const TString& password ); 
   TSQLConnection( const TString& connectString );
   
   void SetURL(const TString& url);
   virtual ~TSQLConnection();

public:
   virtual Bool_t   GetAutoCommit() = 0; 
   virtual void     SetAutoCommit( Bool_t autoCommit = kTRUE ) = 0;   
   virtual TString  GetCatalog() = 0; 
   virtual void     SetCatalog( const TString& catalog ) = 0;
   virtual Bool_t   IsClosed(); 
   virtual Bool_t   IsReadOnly() = 0; 
   virtual void     SetReadOnly( Bool_t readOnly = kTRUE ) = 0;
   virtual Bool_t   GetTrace() = 0;
   virtual void     SetTrace( Bool_t trace = kTRUE ) = 0;
   virtual void     SetTraceFile( const TString& filename ) = 0;
   virtual TString  GetTraceFile() = 0;
   virtual Int_t    GetTransactionIsolation() = 0; 
   virtual void     SetTransactionIsolation( Int_t level ) = 0;
   virtual TString  NativeSQL( const TString& sql ) = 0; 
   virtual void     Commit() = 0;
   virtual void     Rollback() = 0;
   virtual TSQLStatement* CreateStatement() = 0; 
   virtual TSQLStatement* CreateStatement( Int_t resultSetType, 
                                           Int_t resultSetConcurrency ) = 0;
   virtual TSQLPreparedStatement* PrepareStatement( const TString& sql ) = 0;
   virtual TSQLPreparedStatement* PrepareStatement( const TString& sql,
                                                    Int_t resultSetType,
                                                    Int_t resultSetConcurrency )  = 0;
   virtual TSQLCallableStatement* PrepareCall( const TString& sql ) = 0;
   virtual TSQLCallableStatement* PrepareCall( const TString& sql,
                                               Int_t resultSetType,
                                               Int_t resultSetConcurrency ) = 0;
   virtual TSQLDatabaseMetaData* GetMetaData() = 0;
   virtual Bool_t HasBatchSupport() = 0;

   virtual void Close();
   TList* GetListOfStatements() const { return fListOfStatements; }   
   virtual const char *GetName() const;
   virtual const char *GetTitle() const;
   virtual void  Print(Option_t *option="") const;
   virtual void  ls(Option_t *option="") const { Print(option); }
   
ClassDef(TSQLConnection,0)//A connection to a specific database. 
};

#endif // RDBC_TSQLConnection_h
