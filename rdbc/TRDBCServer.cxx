// $Id: TRDBCServer.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2001
//

/////////////////////////////////////////////////////////////////////
//
//  TRDBCServer is implementation of TSQLServer with RDBC 
//
//  This class is experimental and not completed yet, 
//  as much as possible use  TSQLDriverManager instead.
//  

#include <RDBC/TRDBCServer.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLDatabaseMetaData.h>
#include <RDBC/TSQLStatement.h>
#include <iostream>

ClassImp(TRDBCServer)
/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TRDBCServer::TRDBCServer(const char *db, const char *uid, const char *pw)
{
   // Open a connection to a  DB server. The db arguments should be
   // of the form 
   // "protocol:[subprotocol]:[driver]://<host>[:<port>][/<database>]", e.g.:
   // "mysql:odbc:myodbc//pcroot.cern.ch:3456/test". 
   // The uid is the username and pw
   // the password that should be used for the connection.
   //

   fConnection = TSQLDriverManager::GetConnection(db,uid,pw);

   if(!fConnection) {
      Error("TRDBCServer", "connection to %s failed", db);
      return;
   } 
   TSQLDatabaseMetaData* md = fConnection->GetMetaData();
   fDbName = md->GetDatabaseProductName();
}

//___________________________________________________________________
TRDBCServer::~TRDBCServer()
{
   // Close connection to DB server
   
   if (IsConnected()) Close();
}

//______________________________________________________________________________
void TRDBCServer::Close(Option_t *)
{
   // Close connection to DB server.

   if (!fConnection) return;
   fConnection->Close();
   if(fConnection->IsClosed()) fConnection = 0;
}

//______________________________________________________________________________
TSQLResultSet* TRDBCServer::Query(const char *sql)
{
   // Execute SQL command.
   //
   // Returns a pointer to a TSQLResultSet object if successful, 0 otherwise.
   
   if (!IsConnected()) {
      Error("Query", "not connected");
      return 0;
   }
   return fConnection->CreateStatement()->ExecuteQuery(sql);
}

//___________________________________________________________________
Int_t TRDBCServer::SelectDataBase(const char* dbname)
{
   // Select/Change a database. Returns kTRUE if successful, 
   // kFALSE otherwise.
   //
   // Corresponds to MySQL statement "USE database"

   if(!IsConnected()) return 0;
  
  
   TSQLStatement* stmt = fConnection->CreateStatement();
   TString query = "USE ";
   query += dbname;

   if(!stmt->Execute(query)) return 0;
   delete stmt;
   return 1;
}

//______________________________________________________________________________
TSQLResultSet* TRDBCServer::GetDataBases(const char *wild)
{
   // List all available databases. Wild is for wildcarding "t%" list all
   // databases starting with "t".
   // Returns a pointer to a TSQLResultSet object if successful, 0 otherwise.
   
   if (!IsConnected()) {
      Error("GetDataBases", "not connected");
      return 0;
   }
   
   TString query;

   if( fDbName == "MySQL" ) {
      query= "show databases";
      if(wild) { query += " like ";  query += wild; }
      return Query(query);      
   } else {
      return 0;
   }
}

//______________________________________________________________________________
TSQLResultSet* TRDBCServer::GetTables(const char *dbname, const char *wild)
{
   // List all tables in the specified database. Wild is for wildcarding 
   // "t%" list all tables starting with "t".
   // Returns a pointer to a TSQLResultSet object if successful, 0 otherwise.

   if (!IsConnected()) {
      Error("GetTables", "not connected");
      return 0;
   }

   TString query;

   if( fDbName == "MySQL") {
      query = "SHOW TABLES";
   
      if(dbname) {
         if(SelectDataBase(dbname) != 0) {
            Error("GetTables", "no such database %s", dbname);
            return 0;
         }
         query += " from ";   query += dbname; 
      }

      if(wild) { query += " like ";  query += wild; }
      return Query(query);
   } else {
      return 0;
   }
}

//______________________________________________________________________________
TSQLResultSet* TRDBCServer::GetColumns( const char *dbname, const char *table,
                                        const char *wild )
{
   // List all columns in specified table in the specified database.
   // Wild is for wildcarding "t%" list all columns starting with "t".
   // Returns a pointer to a TSQLResultSet object if successful, 0 otherwise.
   // The result object must be deleted by the user.

   if (!IsConnected()) {
      Error("GetColumns", "not connected");
      return 0;
   }

   TString query;

   if ( dbname && (SelectDataBase(dbname) != 0) ) {
      Error("GetColumns", "no such database %s", dbname);
      return 0;
   }
   
   if(fDbName == "MySQL") {
      query = "SHOW COLUMNS FROM ";
      query += table;
   
      if (wild) { query += " LIKE "; query += wild; }
      return Query(query);
   } else {
      return 0;
   }
}

//______________________________________________________________________________
Int_t TRDBCServer::CreateDataBase(const char *dbname)
{
   // Create a database. Returns kTRUE if successful, kFALSE otherwise.
   
   if (!IsConnected()) {
      Error("CreateDataBase", "not connected");
      return kFALSE;
   }

   Bool_t res;
   TString query;
   TSQLStatement* stmt;

   query = "CREATE DATABASE ";   // standart SQL
   query += dbname;

   stmt = fConnection->CreateStatement();
   res = stmt->Execute(query);
   delete stmt;
   return res;
}

//______________________________________________________________________________
Int_t TRDBCServer::DropDataBase(const char *dbname)
{
   // Drop (i.e. delete) a database. Returns kTRUE if successful, kFALSE otherwise.
   
   if (!IsConnected()) {
      Error("DropDataBase", "not connected");
      return 0;
   }

   Bool_t res;
   TString query;
   TSQLStatement* stmt;

   query = "DROP DATABASE ";  // standart SQL
   query += dbname;

   stmt = fConnection->CreateStatement();
   res = stmt->Execute(query);
   delete stmt;
   return res;
}

//______________________________________________________________________________
Int_t TRDBCServer::Reload()
{
   // Reload permission tables. Returns kTRUE if successful, kFALSE
   // otherwise. User must have reload permissions.
   
   if (!IsConnected()) {
      Error("Reload", "not connected");
      return kFALSE;
   }
  
   Bool_t res;
   TSQLStatement* stmt;

   if(fDbName == "MySQL") {
      stmt = fConnection->CreateStatement();
      res = stmt->Execute("FLUSH PRIVILEGES");
      delete stmt;
      return res;
   } else {
      return kFALSE;
   }
}

//______________________________________________________________________________
Int_t TRDBCServer::Shutdown()
{
   // Shutdown the database server. Returns kTRUE if successful, kFALSE
   // otherwise. User must have shutdown permissions.

   if (!IsConnected()) {
      Error("Shutdown", "not connected");
      return kFALSE;
   }
   return kFALSE; // not implemented
}

//______________________________________________________________________________
const char* TRDBCServer::ServerInfo()
{
   // Returns a string that represents the server version number.
   //
   // Corresponds to:
   //
   //     mysql> SELECT VERSION();

   if (!IsConnected()) {
      Error("ServerInfo", "not connected");
      return 0;
   }
  
   static TString  str;
   TSQLResultSet* rs;

   if(fDbName == "MySQL") {

      rs = Query("SELECT VERSION()");
      if(!rs) return str.Data();

      if(!rs->Next()) return str.Data();
      str = rs->GetString(1);
      delete rs;
      return str.Data();
   } else {
      return str.Data(); 
   }
}


