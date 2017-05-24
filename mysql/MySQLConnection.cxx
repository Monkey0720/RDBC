// $Id: MySQLConnection.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 22/02/2001 
//
// RDBC driver to MySQL database implemented with MySQL C API.
//
// ++ The code consists of some parts stolen from mm JDBC and  
//    MyODBC ODBC drivers and other mysql-related open sources.
//

/////////////////////////////////////////////////////////////////////
//
//
// A connection (session) with a specific database. Within the context 
// of a TSQLConnection, SQL statements are executed and results are 
// returned.  A TSQLConnection's database is able to provide information 
// describing its tables, its supported SQL grammar, its stored procedures, 
// the capabilities of this connection, and so on. This information is
// obtained with the GetMetaData() method. 
//
// See also:
//    TSQLDriverManager::GetConnection(TString)
//    TSQLStatement TSQLPreparedStatement TSQLCallableStatement
//    TSQLResultSet TSQLDatabaseMetaData
//
///////////////////////////////////////////////////////////////////////
// 
// A transaction is a recoverable sequence of SQL operations grouped
// as a single unit. The initiation and termination of transaction 
// define the following points of data consistency within an 
// application process; either all SQL operations within a transaction 
// are applied to the data source (committed), or the effects of all
// SQL operations within a transaction are completely "undone"
// (rolled back). 
//   RDBC provides the following modes of transaction processing 
// that determine haw and when transactions are to be committed (or,
// if possible rolled back):
// 
//    - Auto-commit mode
//    - Manual-commit mode 
//
//  defined by TSQLConnection::SetAutoCommit( Bool_t autoCommit )
//
///////////////////////////////////////////////////////////////////
//
// In multi-user database system, transactions can occur 
// simultaneously, and each transaction has potential to interfere
// with another one. When transactions are not isolated from each
// other in multi-user enviroments, the following three types of
// events can occur:
//___________________________________________________________________
//
// Dirty Read: 
//
//    Transaction 1 changes a row. Transaction 2 reads
// the changed row before Transacion 1 commits the change. If
// Transaction 1 rolls back the change, Transacion 2 will have
// read a row that is considered to have never existed
//
//___________________________________________________________________
//
// Nonrepeatable Read:
//
//    Transaction 1 reads a row. Transaction 2 updates or deletes
// that row and commits the change. If Transaction 1 attempts
// to reread the row, it will receive different row values or 
//  discover that the row has been deleted.
//
//___________________________________________________________________
//
// Phantom: 
//
//    Transaction 1  reads a set of rows that satisfy some search
// criteria. Transaction 2 generates one or more rows ( either 
// through inserts or updates ) that match the search criteria
// If transacion 1 re-executes the statement that reads the rows,
// it receives a different set of rows.
//
//////////////////////////////////////////////////////////////////////
//
// ODBC defines four levels of transaction isolation that can,
// prevent all, some, or none of these events from occurring. 
// They are:
//___________________________________________________________________
//
// kTRANSACTION_NONE
//
// Indicates that transactions are not supported.
//
//___________________________________________________________________
//
// kTRANSACTION_READ_UNCOMMITTED
// 
// Dirty reads, non-repeatable reads and phantom reads can occur.
// This level allows a row changed by one transaction to be read 
// by another transaction before any changes in that row have been 
// committed (a "dirty read"). If any of the changes are rolled back,
// the second transaction will have retrieved an invalid row.
//
//___________________________________________________________________
//
// kTRANSACTION_READ_COMMITTED
//
//
// Dirty reads are prevented; non-repeatable reads and phantom reads
// can occur. This level only prohibits a transaction from reading a 
// row with uncommitted changes in it.
//
//___________________________________________________________________
//
// kTRANSACTION_REPEATABLE_READ
//
// Dirty reads and non-repeatable reads are prevented; phantom reads 
// can occur. This level prohibits a transaction from reading a row 
// with uncommitted changes in it, and it also prohibits the situation 
// where one transaction reads a row, a second transaction alters the 
// row, and the first transaction rereads the row, getting different 
// values the second   time (a "non-repeatable read").
//
//___________________________________________________________________
//
// kTRANSACTION_SERIALIZABLE
//
// Dirty reads, non-repeatable reads and phantom reads are prevented.
// This level includes the prohibitions in kTRANSACTION_REPEATABLE_READ
// and further prohibits the situation where one transaction reads 
// all rows that satisfy a WHERE condition, a second transaction 
// inserts a row that satisfies that WHERE condition, and the first 
// transaction rereads for the same condition, retrieving the 
// additional "phantom" row in the second read.
//
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLStatement.h>
#include <RDBC/TSQLPreparedStatement.h>
#include <RDBC/TSQLCallableStatement.h>
#include <RDBC/TSQLDatabaseMetaData.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLResultSet.h>
#include <TList.h>
#include <TNamed.h>

#include "MySQLConnectionPrivate.h"

ClassImpQ(TSQLConnection)


///////////////////////////////////////////////////////////////////// 
//___________________________________________________________________
TSQLConnection::TSQLConnection( void* imp ):TSQL(imp)
{
   // private constructor called by 
 
   fImp = new ConnectionPrivate();
   fListOfStatements = new TList();
   fMetaData = new TSQLDatabaseMetaData(this,fImp)
}

//___________________________________________________________________
TSQLConnection::TSQLConnection( const TString&  connectString ):TSQL(0)
{
   // Attempts to establish a connection to the given database 
   // by specified connection string. This string is simply
   // a series of keyword/value pairs, searated by semicolons,
   // that contains information used to establish the connection.
   // The TSQLDriverManager attempts to select an appropriate driver
   // from the set of registered drivers.
   //
   // Parameters:
   //    connectString usually something like:
   //                   "dsn=minos;uid=scott;pwd=tiger"
   //
   // Throws:
   //    TSQLException - if a database access error occurs

   fImp = new ConnectionPrivate();
   fMetaData = new TSQLDatabaseMetaData(this,fImp)
   fListOfStatements = new TList();
}

//___________________________________________________________________
TSQLConnection::TSQLConnection( const TString& url, 
                                const TString& username, 
                                const TString& password ): TSQL(0)
{
   // Attempts to establish a connection to the given Data Source Name (DSN). 
   // The TSQLDriverManager attempts to select an appropriate driver  from  
   // the set of registered drivers.
   //
   // Parameters:
   //    url      - URL string
   //    username - the database user on whose behalf the TSQLConnection
   //                is being made
   //    password - the user's password
   //
   // Throws:
   //    TSQLException - if a database access error occurs
   //

   MySQLConnectionPrivate* imp = new MySQLConnectionPrivate();
   fImp = imp;
     
   imp->fUrl = url;

//   imp->fMYSQL.options.compress = compress;
//   imp->fMYSQL.options.connect_timeout=connect_timeout;
//   locked = kTRUE; mysql.options.my_cnf_file="my";
 
   if( mysql_real_connect( imp->fMYSQL, url.GetHost().Data(), 
                                 username.Data(), password.Data(), 
                                 url.GetDatabase().Data(),
                                 url.GetPort(),  const char *unix_socket, unsigned int client_flag) compress,
       connect_timeout,socket_name, client_flag))
   {
      imp->fLocked = kFALSE;
      imp->fIsConnected = kTRUE;
   
   } else {
      imp->fLocked = kFALSE;
      imp->fIsConnected = kFALSE;
      Throw(new TSQLException(mysql_error(imp->fMYSQL),"",mysql_errno(imp->fMYSQL)));  //
      return;
   }

   fMetaData = new TSQLDatabaseMetaData(this,fImp);
   fListOfStatements = new TList(); 
}

//___________________________________________________________________
TSQLConnection::~TSQLConnection()
{
   // Destructor:
   //
   // - deallocate all statements produced by this connection
   // - disconnect this connection 

   Close();
}

//___________________________________________________________________
TSQLStatement* TSQLConnection::CreateStatement()
{ 
   // Creates a TSQLStatement object for sending SQL statements 
   // to the database. SQL statements without parameters are 
   // normally executed using TSQLStatement objects. If the
   // same SQL statement is executed many times, it is more 
   // efficient to use a TSQLPreparedStatement. TSQLResultSet s 
   // created using the returned TSQLStatement will have 
   // forward-only type, and read-only concurrency, by default.
   //
   // Returns:
   //      a new TSQLStatement object
   //      zero - in case of  error  
   // Throws:
   //       TSQLException - if a database access error occurs     

   TSQLStatement* stmt = new TSQLStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt;
}

//___________________________________________________________________
TSQLStatement* TSQLConnection::CreateStatement( Int_t resultSetType,
                                          Int_t resultSetConcurrency )
{ 
   // Creates a TSQLStatement object that will generate TSQLResultSet
   // objects with the given type and concurrency. This method is the 
   // same as the CreateStatement() method above, but it allows the 
   // default result set type and result set concurrency type to be 
   // overridden.
   //
   // Parameters:
   //       resultSetType - a result set type; 
   //                       see TSQLResultSet::kTYPE_XXX
   //       resultSetConcurrency - a concurrency type; 
   //                       see TSQLResultSet::kCONCUR_XXX
   // Returns:
   //       a new TSQLStatement object
   // Throws:
   //       TSQLException - if a database access error occurs

   TSQLStatement* stmt = new TSQLStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt; 
}

//___________________________________________________________________
TSQLPreparedStatement* TSQLConnection::PrepareStatement(
                                                const TString& sql )
{
   // Creates a TSQLPreparedStatement object for sending 
   // parameterized SQL statements to the database. A SQL statement 
   // with or without IN parameters can be pre-compiled and stored 
   // in a TSQLPreparedStatement object. This object can then be 
   // used to efficiently execute this statement multiple times. 
   //
   // Note: This method is optimized for handling parametric SQL
   //       statements that benefit from precompilation.
   //       If the driver supports precompilation, the method 
   //       PrepareStatement() will send the statement to the database 
   //       for precompilation. Some drivers may not support precompilation. 
   //       In this case, the statement may not be sent to the database 
   //       until the TSQLPreparedStatement is executed. This has no direct
   //       effect on users; however, it does affect which method throws 
   //       certain TSQLException s. Result sets created using the returned 
   //       TSQLPreparedStatement will have forward-only type and read-only
   //       concurrency, by default.
   //
   // Parameters:
   //       sql - a SQL statement that may contain one or more '?' 
   //       IN parameter placeholders
   // Returns:
   //       a new TSQLPreparedStatement object containing the 
   //       pre-compiled statement
   // Throws:
   //       TSQLException - if a database access error occurs

   TSQLPreparedStatement* stmt = new TSQLPreparedStatement(this,imp); 
   fListOfStatements->Add(stmt);
   return stmt;
}

//___________________________________________________________________
TSQLCallableStatement* TSQLConnection::PrepareCall( const TString& sql )
{  
   // Creates a TSQLCallableStatement object for calling database 
   // stored procedures. The TSQLCallableStatement provides methods 
   // for setting up its IN and OUT parameters, and methods for 
   // executing the call to a stored procedure. 
   //
   // Note: This method is optimized for handling stored procedure 
   //       call statements. Some drivers may send the call statement 
   //       to the database when the method PrepareCall() is done; 
   //       others may wait until the TSQLCallableStatement is 
   //       executed. This has no direct effect on users; however, 
   //       it does affect which method throws certain SQLExceptions. 
   //       Result sets created using the returned 
   //       TSQLCallableStatement will have forward-only type and 
   //       read-only concurrency, by default.
   // Parameters:
   //       sql - a SQL statement that may contain one or more '?'
   //             parameter placeholders. Typically this statement is
   //             a function call escape string.
   // Returns:
   //       a new TSQLCallableStatement object containing the 
   //       pre-compiled SQL statement
   // Throws:
   //       TSQLException - if a database access error occurs

   Throw( new TSQLSQLException("Callable statements not supported.", "S1C00",4000); // 
   return 0;
}

//___________________________________________________________________
TSQLPreparedStatement* TSQLConnection::PrepareStatement(
                                          const TString& sql,
                                          Int_t resultSetType, 
                                          Int_t resultSetConcurrency )
{
   // Creates a TSQLPreparedStatement object that will generate
   // TSQLResultSet objects with the given type and concurrency. 
   // This method is the same as the PrepareStatement() method above,
   // but it allows the default result set type and result set 
   // concurrency type to be overridden.
   //   
   // Parameters:
   //       resultSetType - a result set type; 
   //                       see TSQLResultSet::kTYPE_XXX
   //       resultSetConcurrency - a concurrency type; 
   //                              see TSQLResultSet::kCONCUR_XXX
   // Returns:
   //       a new TSQLPreparedStatement object containing the 
   //       pre-compiled SQL statement
   // Throws:
   //       TSQLException - if a database access error occurs

   TSQLPreparedStatement* stmt = new TSQLPreparedStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt;
}

//___________________________________________________________________
TSQLCallableStatement* TSQLConnection::PrepareCall( 
                                          const TString& sql, 
                                          Int_t resultSetType, 
                                          Int_t resultSetConcurrency )
{
   // Creates a TSQLCallableStatement object that will generate
   // TSQLResultSet objects with the given type and concurrency. 
   // This method is the same as the PrepareCall() method above,
   // but it allows the default result set type and result set 
   // concurrency type to be overridden.
   //
   // Parameters:
   //       resultSetType - a result set type; 
   //                         see TSQLResultSet::kTYPE_XXX
   //       resultSetConcurrency - a concurrency type;
   //                         see TSQLResultSet::kCONCUR_XXX
   //
   // Returns:
   //       a new TSQLCallableStatement object containing the 
   //       pre-compiled SQL statement
   // Throws:
   //       TSQLException - if a database access error occurs

   Throw( new TSQLSQLException("Callable statments not suppoted.", "S1C00",4000); // 
   return 0;
}

//___________________________________________________________________
TString TSQLConnection::NativeSQL( const TString& sql )
{
   // Converts the given SQL statement into the system's native SQL
   // grammar. A driver may convert the sql grammar into its system's
   // native SQL grammar prior to sending it; this method returns 
   // the native form of the statement that the driver would have 
   // sent.
   //
   // Parameters:
   //       sql - a SQL statement that may contain one or more '?'
   //             parameter placeholders
   // Returns:
   //       the native form of this statement
   // Throws:
   //       TSQLException - if a database access error occurs

   return sql;
}

//___________________________________________________________________
void TSQLConnection::SetAutoCommit( Bool_t autoCommit )
{
   // Sets this connection's auto-commit mode. If a connection is in
   // auto-commit mode, then all its SQL statements will be executed 
   // and committed as individual transactions. Otherwise, its SQL 
   // statements are grouped into transactions that are terminated 
   // by a call to either the method commit or the method rollback. 
   // By default, new connections are in auto-commit mode. The commit
   // occurs when the statement completes or the next execute occurs,
   // whichever comes first. In the case of statements returning a 
   // TSQLResultSet, the statement completes when the last row
   // of the TSQLResultSet has been retrieved or the TSQLResultSet 
   // has been closed. In advanced cases, a single statement may 
   // return multiple results as well as output parameter values. 
   // In these cases the commit occurs when all results and output 
   // parameter values have been retrieved.
   //
   // Parameters:
   //       autoCommit - kTRUE enables auto-commit; 
   //                    kFALSE disables auto-commit.
   // Throws:
   //       TSQLException - if a database access error occurs
   
   if (autoCommit == kFALSE) {
      Throw( new TSQLException("Cannot disable AUTO_COMMIT", "08003",)); //
   }
}

//___________________________________________________________________
Bool_t TSQLConnection::GetAutoCommit()
{ 
   // Gets the current auto-commit state.
   //   
   // Returns:
   //       the current state of auto-commit mode
   // Throws:
   //       TSQLException - if a database access error occurs
   // See Also: 
   //       SetAutoCommit(Bool_t)

   return  kTRUE;
}

//___________________________________________________________________
void TSQLConnection::Commit()
{
   // Makes all changes made since the previous commit/rollback
   // permanent and releases any database locks currently held by 
   // the TSQLConnection. This method should be used only when 
   // auto-commit mode has been disabled.
   //
   // Throws:
   //       TSQLException - if a database access error occurs
   // See Also: 
   //       SetAutoCommit(Bool_t)

   if( IsClosed() ) {
      Throw( new TSQLException("Commit attempt on closed connection.", "08003", ); //
   }
}

//___________________________________________________________________
void TSQLConnection::Rollback()
{ 
   // Drops all changes made since the previous commit/rollback and
   // releases any database locks currently held by this TSQLConnection. 
   // This method should be used only when auto-commit has been disabled.
   //
   // Throws:
   //       TSQLException - if a database access error occurs
   // See Also: 
   //       SetAutoCommit(Bool_t)

   if( IsClosed() ) {
      Throw( new TSQLException("[RDBC]Rollback attempt on closed connection.", "08003", ); //
   }
}

//___________________________________________________________________
void TSQLConnection::Close()
{
   // Releases a TSQLConnection's database and resources immediately
   // instead of waiting for them to be automatically released. 
   //
   // Note: A TSQLConnection is automatically closed when it is 
   //       garbage collected. Certain fatal errors also result in 
   //       a closed TSQLConnection.
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) { 
      Throw(new TSQLException("[RDBC]Close attempt on closed connection.","08003",); //
      return;
   }

   TList* li = TSQLDriverManager::GetConnections();
   li->Remove(this);

   if(fMetaData) delete fMetaData;

   if(fListOfStatements) {    // deallocate all statements
      fListOfStatements->Delete();
      delete fListOfStatements;
   }

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;
   mysql_close(imp->fMYSQL);
   fImp = 0;
   Destroyed();
   return;
}

//___________________________________________________________________
Bool_t TSQLConnection::IsClosed()
{
   // Tests to see if a TSQLConnection is closed.
   // 
   // Returns:
   //       kTRUE if the connection is closed; 
   //       kFALSE if it's still open
   // Throws:
   //       TSQLException - if a database access error occurs
 
   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;

   if(!imp->fIsClosed) {
         // Test the connection
         rs = execSQL(gPING_COMMAND, -1);
   }
   
   imp->fIsClosed = !rs;   // kFALSE if rs==0 
   return imp->fIsClosed;
}

//___________________________________________________________________
TSQLDatabaseMetaData* TSQLConnection::GetMetaData()
{
   // Gets the metadata regarding this connection's database. 
   // A TSQLConnection's database is able to provide information 
   // describing its tables, its supported SQL grammar, its
   // stored procedures, the capabilities of this connection,
   // and so on. This information is made available through a 
   // TSQLDatabaseMetaData object.
   //
   // Returns:
   //       a TSQLDatabaseMetaData object for this TSQLConnection
   // Throws:
   //       TSQLException - if a database access error occurs
 
   return fMetaData;
}

//___________________________________________________________________
void TSQLConnection::SetReadOnly( Bool_t readOnly )
{
   // Puts this connection in read-only mode as a hint to enable
   // database optimizations. 
   //
   // Note: This method cannot be called while in the middle of a
   // transaction.
   //
   // Parameters:
   //       readOnly - kTRUE enables read-only mode; 
   //                  kFALSE disables read-only mode.
   // Throws:
   //       TSQLException - if a database access error occurs

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;

   if(IsClosed()) { 
      Throw(new TSQLException("SetReadOnly attempt on closed connection.","08003",); //
      return;
   }

   imp->fReadOnly = readOnly;
}

//___________________________________________________________________
Bool_t TSQLConnection::IsReadOnly()
{
   // Tests to see if the connection is in read-only mode.
   //   
   // Returns:
   //       kTRUE if connection is read-only and kFALSE otherwise
   // Throws:
   //       TSQLException - if a database access error occurs

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;
   return imp->fReadOnly;
}

//___________________________________________________________________
void TSQLConnection::SetCatalog( const TString& catalog )
{ 
   // Sets a catalog name in order to select a subspace of this
   // TSQLConnection's database in which to work. If the driver 
   // does not support catalogs, it will silently ignore this 
   // request.
   //
   // Throws:
   //       TSQLException - if a database access error occurs

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;

   if(IsClosed()) { 
      Throw(new TSQLException("SetCatalog attempt on closed connection.","08003",); //
      return;
   }

   if( !mysql_select_db(imp->fMYSQL,catalog) ) {
      Throw( new TSQLException("SetCatalog attempt failed","",) );
      return;
   }  
   imp->fDatabase = catalog;
}

//___________________________________________________________________
TString TSQLConnection::GetCatalog()
{
   // Returns the TSQLConnection's current catalog name.
   //
   // Returns:
   //       the current catalog name or null string
   // Throws:
   //       TSQLException - if a database access error occurs

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;

   if(IsClosed()) { 
      Throw(new TSQLException("GetCatalog attempt on closed connection.","08003",); //
      return "";
   }

   return imp->fDatabase; 
}

//___________________________________________________________________
void TSQLConnection::SetTransactionIsolation( Int_t level )
{
   // Attempts to change the transaction isolation level to the one
   // given. The constants defined in the interface TSQLConnection 
   // are the possible transaction isolation levels. 
   //
   // Note: This method cannot be called while in the middle of a
   //       transaction.
   //
   // Parameters:
   //       level - one of the kTRANSACTION_XXX isolation values with 
   //               the exception of kTRANSACTION_NONE; 
   //               some databases may not support other values
   // Throws:
   //        TSQLException - if a database access error occurs
   // 
   // See Also: 
   // TSQLDatabaseMetaData::SupportsTransactionIsolationLevel(Int_t)

   if(IsClosed()) { 
      Throw(new TSQLException("Transaction Isolation Levels are not supported.","S1C00",4000); //
   }
   return;
}

//___________________________________________________________________
Int_t TSQLConnection::GetTransactionIsolation()
{ 
   // Gets this TSQLConnection's current transaction isolation level.
   //
   // Returns:
   //       the current kTRANSACTION_XXX mode value
   // Throws:
   //       TSQLException - if a database access error occurs
  
   return kTRANSACTION_SERIALIZABLE;
}

//___________________________________________________________________
Bool_t TSQLConnection::GetTrace()
{
   // Returns kTRUE if tracing is enabled on this connection 

   Throw(new TSQLException("Tracing is not enable","S1C00",4000));
   return kFALSE;
}

//___________________________________________________________________
void TSQLConnection::SetTrace( Bool_t on )
{
   // Sets tracing on or off
  
   Throw(new TSQLException("Tracing is not enable","S1C00",4000));
}

//___________________________________________________________________
TString TSQLConnection::GetTraceFile()
{
   // Returns the file tracing is currently written to  
   
   Throw(new TSQLException("Tracing is not enable","S1C00",4000));   
   return "";
}

//___________________________________________________________________
void TSQLConnection::SetTraceFile( const TString& fn )
{
   // Sets the file tracing is written to    

   Throw(new TSQLException("Tracing is not enable","S1C00",4000));
}

//___________________________________________________________________
Bool_t TSQLConnection::HasBatchSupport()
{
   // Returns kTRUE if batch are supported
   //
   // Throws:
   //       TSQLException - if a database access error occurs
  
   return kFALSE; // not ... for a moment
}

//___________________________________________________________________
void TSQLConnection::SetURL(const TString& url)
{
   // sets URL string

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;
   imp->fURL = url;
   fMetaData->SetURL(url);
}

//////////// functions used to print out dbmetadata /////////////////
//___________________________________________________________________
const char* s(Bool_t b)
{
   //
   
   return b?"Yes":"No";
}

//___________________________________________________________________
TString maybeQuote(const TString& str, const TString& qc)
{
   //
   
   TString res; 
   if(qc!=" ")  res = qc + str + qc;
   else  res = str;
   return res;
}

//___________________________________________________________________
void rsInfo( TSQLDatabaseMetaData* md, 
             Int_t rsType, const char* name)
{
   //  prints result set info    
  
   if(md->SupportsResultSetType(rsType)) {
      printf("%s\n",name);

      if(md->SupportsResultSetConcurrency(rsType,kCONCUR_READ_ONLY)) {
              printf("  + kCONCUR_READ_ONLY\n");
      }

      if(md->SupportsResultSetConcurrency(rsType,kCONCUR_UPDATABLE)) {
              printf("  + kCONCUR_UPDATABLE\n");
      }

      if(md->OwnInsertsAreVisible(rsType)) {
              printf("    Own inserts are visible\n");
      }

      if(md->OwnUpdatesAreVisible(rsType)) {
              printf("    Own updates are visible\n");
      }
      if(md->OwnDeletesAreVisible(rsType)) {
              printf("    Own deletes are visible\n");
      }
   }
}


//___________________________________________________________________
struct {
   int id;
   const char* name;
   } levels[] = {
      { kTRANSACTION_READ_UNCOMMITTED, "kTRANSACTION_READ_UNCOMMITTED" },
      { kTRANSACTION_READ_COMMITTED, "kTRANSACTION_READ_COMMITTED" },
      { kTRANSACTION_REPEATABLE_READ, "kTRANSACTION_REPEATABLE_READ" },
      { kTRANSACTION_SERIALIZABLE, "kTRANSACTION_SERIALIZABLE" },
      { 0,NULL }
   };
   
//___________________________________________________________________
void transactionInfo(TSQLDatabaseMetaData* md)
{
   //prints out transaction info
  
   TString str;

   str = s(md->SupportsTransactions());
   printf("Supports transactions           : %s\n",str.Data()); 

   printf("\nTransaction support \n");
   printf("---------------------------------------------------\n");

   if(!md->SupportsTransactions()) {
      printf("This datasource does not support transactions.\n\n");
      return;
   }

   int defIsolation=md->GetDefaultTransactionIsolation();

   for(int i=0; levels[i].name!=NULL; i++) {
      if(md->SupportsTransactionIsolationLevel(levels[i].id)) {
         str = " +";
         str += levels[i].name;
         str += (levels[i].id==defIsolation) ? " (default)" :"";
         printf("%s\n",str.Data());
      }
   }

   // the longest method name I've ever seen!
   if(md->SupportsDataDefinitionAndDataManipulationTransactions()) {
      printf("  Both DML and DDL can be used within a transaction\n");
   } else if(md->SupportsDataManipulationTransactionsOnly()) {
      printf("  Only DML can be used within a transaction\n");
   } else if(md->DataDefinitionCausesTransactionCommit()) {
      printf("  DDL causes commit\n");
   } else if(md->DataDefinitionIgnoredInTransactions()) {
      printf("  DDL is ignored in transactions\n");
   }
}

//___________________________________________________________________
void catalogInfo(TSQLDatabaseMetaData* md)
{
   //

   Bool_t cdml = md->SupportsCatalogsInDataManipulation();
   Bool_t cproc = md->SupportsCatalogsInProcedureCalls();
   Bool_t ctd = md->SupportsCatalogsInTableDefinitions();
   Bool_t cid = md->SupportsCatalogsInIndexDefinitions();
   Bool_t cpd = md->SupportsCatalogsInPrivilegeDefinitions();
   Bool_t hasCatalogs = cdml || cproc || ctd || cid || cpd;
  
   if(hasCatalogs) {
      printf("Supported catalog uses\n");
      printf("--------------------------------------------------- \n");
      printf("Data manipulation    : %s\n",s(cdml) );
      printf("Procedure calls      : %s\n",s(cproc) );
      printf("Table definitions    : %s\n",s(ctd) );
      printf("Index definitions    : %s\n",s(cid) );
      printf("Privilege definitions: %s\n",s(cpd) );
   } else {
      printf("This datasource does not support catalogs\n");
   }
   printf("\n");
 
   TString id = md->GetTableTerm();
   if(hasCatalogs) {
      TString catSep;
      TString catTerm;
      catSep  = md->GetCatalogSeparator();
      catTerm = md->GetCatalogTerm(); 
      if(md->IsCatalogAtStart()) {
         id =  catTerm + catSep + id;
      } else {
              id = id + catSep + catTerm;
      }
   }
   printf("Preferred table identifier format: %s\n",id.Data());
}

//___________________________________________________________________
void schemaInfo(TSQLDatabaseMetaData* md)
{
   //

   Bool_t sdml = md->SupportsSchemasInDataManipulation();
   Bool_t sproc = md->SupportsSchemasInProcedureCalls();
   Bool_t std = md->SupportsSchemasInTableDefinitions();
   Bool_t sid = md->SupportsSchemasInIndexDefinitions();
   Bool_t spd = md->SupportsSchemasInPrivilegeDefinitions();
   Bool_t hasSchemas=sdml || sproc || std || sid || spd;

   if(hasSchemas) {
      printf("Supported schema uses\n");
      printf("--------------------------------------------------- \n");
      printf("Data manipulation    : %s\n",s(sdml) );
      printf("Procedure calls      : %s\n",s(sproc) );
      printf("Table definitions    : %s\n",s(std) );
      printf("Index definitions    : %s\n",s(sid) );
      printf("Privilege definitions: %s\n",s(spd) );
   } else {
      printf("This datasource does not support schemas\n");
   }
   printf("\n");
  
   TString idq = md->GetIdentifierQuoteString();
   // space means no quoting supported

   TString id = md->GetTableTerm();
}

//___________________________________________________________________
void productInfo(TSQLDatabaseMetaData* md)
{
   //

   TString str;

   str = md->GetDatabaseProductName();
   printf("Product name                    : %s\n",str.Data()); 
   
   str = md->GetDatabaseProductVersion(); 
   printf("Product version                 : %s\n",str.Data()); 
}

//___________________________________________________________________
void driverInfo(TSQLDatabaseMetaData* md)
{
   //

   TString str;

   str = md->GetDriverName(); 
        printf("Driver name                     : %s\n",str.Data());
   
   str = md->GetDriverVersion();
        printf("Driver version                  : %s\n",str.Data()); 
}

//___________________________________________________________________
void funcInfo(TSQLDatabaseMetaData* md)
{
   //

   TString str;

   str =  md->GetSystemFunctions();
   printf("Supported system functions \n");
   printf("--------------------------------------------------- \n");
   printf("%s\n\n",str.Data());
   
   printf("Supported string functions\n");
   printf("--------------------------------------------------- \n");
   str = md->GetStringFunctions();
   printf("%s\n\n",str.Data());
   
   printf("Supported time/date functions\n");
   printf("--------------------------------------------------- \n");
   str = md->GetTimeDateFunctions();
   printf("%s\n\n",str.Data());
      
   printf("Supported numeric functions\n");
   printf("---------------------------------------------------\n");
   str = md->GetNumericFunctions();
   printf("%s\n\n",str.Data());
}

//___________________________________________________________________
void keywordInfo(TSQLDatabaseMetaData* md)
{
   //

   TString str;

   printf("Non-ODBC SQL keywords\n");
   printf("--------------------------------------------------- \n");
   str = md->GetSQLKeywords();
   printf("%s\n\n",str.Data());
}

//___________________________________________________________________
void TSQLConnection::Print(Option_t *option) const
{
   // Prints out information about this connection
   //
   // If option contains: 
   //
   //          'r' - prints result set info
   //          't' - prints out transaction info
   //          'p' - prints out product info
   //          'd' - prints out driver info
   //          'f' - prints out supported functions
   //          'k' - prints out Non-ODBC SQL keywords
   //          'c' - prints out catalog info
   //          's' - prints out shema info
   //          'a' - prints out everything

   TString opt = option;
   opt.ToLower();

   printf("Connection to :\t%s\n",fMetaData->GetURL().Data());
   printf("=================================================== \n\n");
   
   if(opt.Contains("p") || opt.Contains("a")) {
      productInfo(fMetaData);
      printf("=================================================== \n\n");
   }
   if(opt.Contains("d") || opt.Contains("a")) {
      driverInfo(fMetaData);
      printf("=================================================== \n\n");
   }
   if(opt.Contains("t") || opt.Contains("a")) {
      transactionInfo(fMetaData);
      printf("=================================================== \n\n");
   }
   if(opt.Contains("f") || opt.Contains("a")) {
      funcInfo(fMetaData);
      printf("=================================================== \n\n");
   }
   if(opt.Contains("k") || opt.Contains("a")) {
      keywordInfo(fMetaData);
      printf("=================================================== \n\n");
   }

   if(opt.Contains("r") || opt.Contains("a")) {
      printf("Supported TSQLResultSet types\n");
      printf("--------------------------------------------------- \n");
      rsInfo(fMetaData,kTYPE_FORWARD_ONLY,"kTYPE_FORWARD_ONLY");
      rsInfo(fMetaData,kTYPE_SCROLL_INSENSITIVE,"kTYPE_SCROLL_INSENSITIVE");
      rsInfo(fMetaData,kTYPE_SCROLL_SENSITIVE,"kTYPE_SCROLL_SENSITIVE");
      printf("\n=================================================== \n");
   }
   if(opt.Contains("c") || opt.Contains("a")) {
      catalogInfo(fMetaData);
      printf("=================================================== \n\n");
   }
   if(opt.Contains("s") || opt.Contains("a")) {
      schemaInfo(fMetaData);
      printf("=================================================== \n\n");
   }
}

//___________________________________________________________________
const char* TSQLConnection::GetName() const
{
   // returns dsn/url

   return fMetaData->GetURL().Data();
}

//___________________________________________________________________  
const char* TSQLConnection::GetTitle() const
{
   // returns nothing 

   return "";
}

/////////// private methods used by TSQLDriverManager ///////////////

//___________________________________________________________________
void TSQLConnection::SetLoginTimeout( Int_t seconds )
{
   // Sets the maximum time in seconds that a driver will wait while
   // attempting to connect to a database. Set to 0 to disable. 
   //
   // Parameters:
   //       seconds - the login time limit in seconds

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;
   imp->fLoginTimeout = seconds >= 0 ? seconds : 0;
}

//___________________________________________________________________
Int_t TSQLConnection::GetLoginTimeout()
{
   // Gets the maximum time in seconds that a driver can wait when
   // attempting to log in to a database. 
   //
   // Returns:
   //       the driver login time limit in seconds, or 0 if disabled.

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;
   return imp->fLoginTimeout;
}

//___________________________________________________________________
void TSQLConnection::Shutdown()
{
   // Should be called before an application is to exit

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp;

   Bool_t suc = !(mysql_shutdown(imp->fMYSQL));
 //  if(!suc) Throw MysqlBadQuery(error());
   return;   
}

//___________________________________________________________________
TList* TSQLConnection::RefreshDrivers(TList* gDrivers)
{
   // Fetch a list of all of currently loaded drivers
   // to which the current caller has access. 
   
   if(!gDrivers) return 0;

   return  gDrivers;   
}

//___________________________________________________________________
TList* TSQLConnection::RefreshDataSources(TList* gDataSources)
{
   // Fetch a list of of all available data sources ( TSQLUrl objects)
 
   if(!gDataSources) return 0;

   gDataSources->Delete(); // remove all 
   TSQLUrl* url;
 
   return  gDataSources;
}
