// $Id: TSQLConnection.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2000 
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
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLDatabaseMetaData.h>
#include <RDBC/TSQLDriverManager.h>
#include <TList.h>

ClassImpQ(TSQLConnection)

///////////////////////////////////////////////////////////////////// 
//___________________________________________________________________
TSQLConnection::TSQLConnection( const TString& /* connectString */ ):
TObject(),TSQL(),TRefCnt()
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

   fImp = 0;
   fMetaData = 0;
   fListOfStatements = new TList();
   AddReference();
}

//___________________________________________________________________
TSQLConnection::TSQLConnection( const TString& /* dsn */, 
                                const TString& /* username */, 
                                const TString& /* password */):
TObject(),TSQL(),TRefCnt()
{
   // Attempts to establish a connection to the given Data Source Name (DSN). 
   // The TSQLDriverManager attempts to select an appropriate driver  from  
   // the set of registered drivers.
   //
   // Parameters:
   //    dsn      - DataSourceName string
   //    username - the database user on whose behalf the TSQLConnection
   //                is being made
   //    password - the user's password
   //
   // Throws:
   //    TSQLException - if a database access error occurs
   //  
 
   fImp = 0;
   fMetaData = 0;
   fListOfStatements = new TList();
   AddReference();
}

//___________________________________________________________________
TSQLConnection::~TSQLConnection()
{  
   // dtor

   if(IsClosed()) return;
   TList* li = gSQLDriverManager->GetConnections();
   li->Remove(this);
}

//___________________________________________________________________
void TSQLConnection::Close()
{
   // Releases a TSQLConnection's database and resources immediately
   // instead of waiting for them to be automatically released. 
   //
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }

   if(RemoveReference()>0) return;

   TList* li = gSQLDriverManager->GetConnections();
   li->Remove(this);

   if(fListOfStatements) {    // deallocate all statements
      fListOfStatements->Delete();
      delete fListOfStatements;
   }

   fListOfStatements = 0;
   Destroyed();       
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

   return References()<=0;
}

//___________________________________________________________________
void TSQLConnection::SetURL(const TString& url)
{
   // sets URL string

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

   TSQLConnection* con = (TSQLConnection*)this;
   if(con->IsClosed()) return;

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

   TSQLConnection* con = (TSQLConnection*)this;
   if(con->IsClosed()) return 0;

   return fMetaData->GetURL().Data();
}

//___________________________________________________________________  
const char* TSQLConnection::GetTitle() const
{
   // returns nothing 

   TSQLConnection* con = (TSQLConnection*)this;
   if(con->IsClosed()) return 0;
   return "";
}
