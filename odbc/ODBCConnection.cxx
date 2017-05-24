// $Id: ODBCConnection.cxx,v 1.2 2010/09/17 14:55:49 phnxbld Exp $
//*-- Author : Valeriy Onuchin 14/02/2000 
//

/**************************************************************************

   ROOT wrappers of libodbc++ library
    
   Copyright (C) 1999-2000 Manush Dodunekov <manush@stendahls.net>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

**************************************************************************/

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

#include "ODBCConnection.h"
#include "ODBCStatement.h"
#include "ODBCPreparedStatement.h"
#include "ODBCCallableStatement.h"
#include "ODBCDatabaseMetaData.h"
#include "ODBCResultSet.h"
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLUrl.h>
#include <RDBC/TSQLDriverInfo.h>
#include <TList.h>
#include <TNamed.h>
#include <RDBC/odbc++/connection.h>
#include <RDBC/odbc++/statement.h>
#include <RDBC/odbc++/resultset.h>
#include <RDBC/odbc++/preparedstatement.h>
#include <RDBC/odbc++/callablestatement.h>
#include <RDBC/odbc++/drivermanager.h>

using namespace odbc;
using namespace std;

ClassImpQ(ODBCConnection)

///////////////////////////////////////////////////////////////////// 
//___________________________________________________________________
ODBCConnection::ODBCConnection( const TString&  connectString ):
               TSQLConnection(connectString)
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

   odbc::Connection* imp = 0;
   odbc::DatabaseMetaData* md = 0; 

   try {
      imp = odbc::DriverManager::getConnection( 
                                   ODBCXX_STRING_C(connectString) );
      fImp = imp;
      if(imp) md = imp->getMetaData();
    } catch(odbc::SQLException& e) {
      gSQLDriverManager->Throw( new TSQLException( 
                                    ODBCXX_STRING_CSTR(e.getMessage()),
                                    ODBCXX_STRING_CSTR(e.getSQLState()),
                                    e.getErrorCode()) );
   // failed to connect => clean all
      if(imp) delete imp;
      fImp = 0;
      return;
   }

   if(!fMetaData) fMetaData = new ODBCDatabaseMetaData(this,md);
}

//___________________________________________________________________
ODBCConnection::ODBCConnection( const TString& dsn, 
                                const TString& username, 
                                const TString& password ): 
                                TSQLConnection(dsn,username,password)
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
   
   odbc::Connection* imp = 0;
   odbc::DatabaseMetaData* md = 0; 

   try {
      imp = odbc::DriverManager::getConnection( 
                                   ODBCXX_STRING_C(dsn),
                                   ODBCXX_STRING_C(username),
                                   ODBCXX_STRING_C(password) );
      fImp = imp;
      if(imp) md = imp->getMetaData();
    } catch(odbc::SQLException& e) {
      gSQLDriverManager->Throw( new TSQLException( 
                                    ODBCXX_STRING_CSTR(e.getMessage()),
                                    ODBCXX_STRING_CSTR(e.getSQLState()),
                                    e.getErrorCode()) );
   // failed to connect => clean all
      if(imp) delete imp;
      fImp = 0;
      return;
   }
   if(!fMetaData) fMetaData = new ODBCDatabaseMetaData(this,md);
}

//___________________________________________________________________
ODBCConnection::~ODBCConnection()
{
   // Destructor:
   //
   // - deallocate all statements produced by this connection
   // - disconnect this connection 

   if(IsClosed()) return;

   odbc::Connection* con = (odbc::Connection*)fImp;  

   if(fListOfStatements) {    // deallocate all statements
      fListOfStatements->Delete();
      delete fListOfStatements;
   }

   fListOfStatements = 0;

   try {
      if(con) delete con;
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
  
   if(fMetaData) delete ((ODBCDatabaseMetaData*)fMetaData);

   fImp = 0;
   fMetaData = 0;
}

//___________________________________________________________________
TSQLStatement* ODBCConnection::CreateStatement()
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
   //      a new ODBCStatement object
   //      zero - in case of  error  
   // Throws:
   //       TSQLException - if a database access error occurs     

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }

   ClearWarnings();

   TSQLStatement* stmt = 0;
   odbc::Connection* con = (odbc::Connection*)fImp;
   odbc::Statement* imp = 0;

   try {      
      imp = con->createStatement();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      if(imp) delete imp;
      return 0;
   }
   stmt = new ODBCStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt; 
}

//___________________________________________________________________
TSQLStatement* ODBCConnection::CreateStatement( Int_t resultSetType,
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
   //       a new ODBCStatement object
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }
   ClearWarnings();

   TSQLStatement* stmt = 0;  
   odbc::Connection* con = (odbc::Connection*)fImp;
   odbc::Statement* imp = 0;

   try {      
      imp = con->createStatement( resultSetType,resultSetConcurrency );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      if(imp) delete imp;
      return 0;
   }
   stmt = new ODBCStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt; 
}

//___________________________________________________________________
TSQLPreparedStatement* ODBCConnection::PrepareStatement( const TString& sql )
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
   //       a new ODBCPreparedStatement object containing the 
   //       pre-compiled statement
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }

   ClearWarnings();

   ODBCPreparedStatement* stmt = 0;
   odbc::Connection* con = (odbc::Connection*)fImp;
   odbc::PreparedStatement* imp = 0;

   try {      
      imp = con->prepareStatement( ODBCXX_STRING_C(sql.Data()) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      if(imp) delete imp;
      return 0; 
   }
   stmt = new ODBCPreparedStatement(this,imp); 
   fListOfStatements->Add(stmt);
   return stmt;
}

//___________________________________________________________________
TSQLCallableStatement* ODBCConnection::PrepareCall( const TString& sql )
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
   //       a new ODBCCallableStatement object containing the 
   //       pre-compiled SQL statement
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }

   ClearWarnings();

   ODBCCallableStatement* stmt = 0;
   odbc::Connection* con = (odbc::Connection*)fImp;
   odbc::CallableStatement* imp = 0;

   try {      
      imp = con->prepareCall( ODBCXX_STRING_C(sql.Data()) );      
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      if(imp) delete imp;
      return 0;
   }
   stmt = new ODBCCallableStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt;
}

//___________________________________________________________________
TSQLPreparedStatement* ODBCConnection::PrepareStatement( const TString& sql,
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
   //       a new ODBCPreparedStatement object containing the 
   //       pre-compiled SQL statement
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }

   ClearWarnings();

   ODBCPreparedStatement* stmt = 0; 
   odbc::Connection* con = (odbc::Connection*)fImp;
   odbc::PreparedStatement* imp =0;

   try {      
      imp = con->prepareStatement( ODBCXX_STRING_C(sql.Data()),
                                   resultSetType,
                                   resultSetConcurrency );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      if(imp) delete imp;
      return 0;
   }
   stmt =  new ODBCPreparedStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt;
}

//___________________________________________________________________
TSQLCallableStatement* ODBCConnection::PrepareCall( const TString& sql, 
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
   //       a new ODBCCallableStatement object containing the 
   //       pre-compiled SQL statement
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }

   ClearWarnings();

   ODBCCallableStatement* stmt = 0;
   odbc::Connection* con = (odbc::Connection*)fImp;
   odbc::CallableStatement* imp = 0; 

   try {      
      imp= con->prepareCall( ODBCXX_STRING_C(sql.Data()),
                             resultSetType,
                             resultSetConcurrency );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      if(imp) delete imp;
      return 0;
   }
   stmt =  new ODBCCallableStatement(this,imp);
   fListOfStatements->Add(stmt);
   return stmt;
}

//___________________________________________________________________
TString ODBCConnection::NativeSQL( const TString& sql )
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

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return "0";
   }

   ClearWarnings();

   TString str;
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {
      ODBCXX_STRING s = con->nativeSQL( ODBCXX_STRING_C(sql.Data()) );
      str = ODBCXX_STRING_CSTR(s);         
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }     
   return str;
}

//___________________________________________________________________
void ODBCConnection::SetAutoCommit( Bool_t autoCommit )
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

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      con->setAutoCommit(autoCommit);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
Bool_t ODBCConnection::GetAutoCommit()
{ 
   // Gets the current auto-commit state.
   //   
   // Returns:
   //       the current state of auto-commit mode
   // Throws:
   //       TSQLException - if a database access error occurs
   // See Also: 
   //       SetAutoCommit(Bool_t)

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }

   Bool_t return_value = kFALSE;    
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      return_value = con->getAutoCommit();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }
   return return_value;
}

//___________________________________________________________________
void ODBCConnection::Commit()
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

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }
  
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      con->commit();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
void ODBCConnection::Rollback()
{ 
   // Drops all changes made since the previous commit/rollback and
   // releases any database locks currently held by this TSQLConnection. 
   // This method should be used only when auto-commit has been disabled.
   //
   // Throws:
   //       TSQLException - if a database access error occurs
   // See Also: 
   //       SetAutoCommit(Bool_t)

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }

   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      con->rollback();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
void ODBCConnection::Close()
{
   // Releases a TSQLConnection's database and resources immediately
   // instead of waiting for them to be automatically released. 
   //
   // Throws:
   //       TSQLException - if a database access error occurs

   TSQLConnection::Close();
   if(!IsClosed()) return;    // connection is in use

   odbc::Connection* con = (odbc::Connection*)fImp;

   try {
      if(con) delete con;  // !!!!
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }  

   if(fMetaData) delete ((ODBCDatabaseMetaData*)fMetaData);
   fMetaData = 0;
   fImp = 0;
}

//___________________________________________________________________
TSQLDatabaseMetaData* ODBCConnection::GetMetaData()
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

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }

   odbc::DatabaseMetaData* md = 0; 
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      md = con->getMetaData();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) ); 
      return 0;
   }   
   if(fMetaData) ((ODBCDatabaseMetaData*)fMetaData)->Set(this,md);
   return fMetaData;
}

//___________________________________________________________________
void ODBCConnection::SetReadOnly( Bool_t readOnly )
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

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   } 
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {
      con->setReadOnly(readOnly);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
Bool_t ODBCConnection::IsReadOnly()
{
   // Tests to see if the connection is in read-only mode.
   //   
   // Returns:
   //       kTRUE if connection is read-only and kFALSE otherwise
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return kTRUE;
   }

   Bool_t return_value = kTRUE;   
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
		return_value = con->isReadOnly();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kTRUE;
   }
	return return_value;
}

//___________________________________________________________________
void ODBCConnection::SetCatalog( const TString& catalog )
{ 
   // Sets a catalog name in order to select a subspace of this
   // TSQLConnection's database in which to work. If the driver 
   // does not support catalogs, it will silently ignore this 
   // request.
   //
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }

   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      con->setCatalog( ODBCXX_STRING_C(catalog.Data()) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
TString ODBCConnection::GetCatalog()
{
   // Returns the TSQLConnection's current catalog name.
   //
   // Returns:
   //       the current catalog name or null string
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return "0";
   }

   TString str;
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {       
      str = ODBCXX_STRING_CSTR( con->getCatalog() );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }
   return str;
}

//___________________________________________________________________
void ODBCConnection::SetTransactionIsolation( Int_t level )
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
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }

   try {      
      // con->setTransactionIsolation(level);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
Int_t ODBCConnection::GetTransactionIsolation()
{ 
   // Gets this TSQLConnection's current transaction isolation level.
   //
   // Returns:
   //       the current kTRANSACTION_XXX mode value
   // Throws:
   //       TSQLException - if a database access error occurs

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return 0;
   }
   
   Int_t return_value = 0; 
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      return_value = con->getTransactionIsolation(); 

   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return 0;
   }
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCConnection::GetTrace()
{
   // Returns kTRUE if tracing is enabled on this connection 

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return kFALSE;
   }

   Bool_t return_value = kFALSE;
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      return_value = con->getTrace();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }
   return return_value;
}

//___________________________________________________________________
void ODBCConnection::SetTrace( Bool_t on )
{
   // Sets tracing on or off
   
   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }

   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      con->setTrace(on);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
TString ODBCConnection::GetTraceFile()
{
   // Returns the file tracing is currently written to  
   
   TString str;

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return str;
   }
   
   if(!fImp) { Destroyed();   return str; }       
   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      str = ODBCXX_STRING_CSTR( con->getTraceFile() );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }
   return str;
}

//___________________________________________________________________
void ODBCConnection::SetTraceFile( const TString& fn )
{
   // Sets the file racing is written to    

   if(IsClosed()) {
      Throw( new TSQLException( "Connection is closed","",0) );
      return;
   }

   odbc::Connection* con = (odbc::Connection*)fImp;

   try {      
      con->setTraceFile( ODBCXX_STRING_C(fn.Data()) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}

//___________________________________________________________________
Bool_t ODBCConnection::HasBatchSupport()
{
   // Returns kTRUE if batch are supported
   //
   // Throws:
   //       TSQLException - if a database access error occurs
  
   return kFALSE; 
}

/////////// private methods used by TSQLDriverManager ///////////////

//___________________________________________________________________
void ODBCConnection::SetLoginTimeout( Int_t seconds )
{
   // Sets the maximum time in seconds that a driver will wait while
   // attempting to connect to a database. Set to 0 to disable. 
   //
   // Parameters:
   //       seconds - the login time limit in seconds

   odbc::DriverManager::setLoginTimeout( seconds );
}

//___________________________________________________________________
Int_t ODBCConnection::GetLoginTimeout()
{
   // Gets the maximum time in seconds that a driver can wait when
   // attempting to log in to a database. 
   //
   // Returns:
   //       the driver login time limit in seconds, or 0 if disabled.

   return odbc::DriverManager::getLoginTimeout();
}

//___________________________________________________________________
void ODBCConnection::Shutdown()
{
   // Should be called before an application is to exit

   try {
      odbc::DriverManager::shutdown();
   } catch(odbc::SQLException& e) {
      gSQLDriverManager->Throw( new TSQLException( 
                                    ODBCXX_STRING_CSTR(e.getMessage()),
                                    ODBCXX_STRING_CSTR(e.getSQLState()),
                                    e.getErrorCode()) );
   }  
}

//___________________________________________________________________
TList* ODBCConnection::RefreshDrivers(TList* gDrivers)
{
   // Fetch a list of all of currently loaded drivers
   // to which the current caller has access. 
   
   if(!gDrivers) return 0;

   gDrivers->Delete();

   TSQLDriverInfo* driver; 
   TNamed* attribute;
   TList* attributeList;
   
   try {
      odbc::DriverList* list = odbc::DriverManager::getDrivers();
      
      for( odbc::DriverList::iterator i=list->begin();
           i != list->end(); i++) {

         TString description = ODBCXX_STRING_CSTR((*i)->getDescription());
         const vector<ODBCXX_STRING>& attrs=(*i)->getAttributes();
         
         attributeList = new TList();
            
         for( vector<ODBCXX_STRING>::const_iterator x=attrs.begin(); 
               x!=attrs.end(); x++) {
           
            attribute = new TNamed( ODBCXX_STRING_CSTR((*x)), "attribute" );
            attributeList->Add(attribute);
         }
         driver = new TSQLDriverInfo(description,attributeList);
         gDrivers->Add(driver);   
      }   
   } catch(odbc::SQLException& e) {
      gSQLDriverManager->Throw( new TSQLException( 
                              ODBCXX_STRING_CSTR(e.getMessage()),
                              ODBCXX_STRING_CSTR(e.getSQLState()),
                              e.getErrorCode()) );
   }
   return  gDrivers;   
}

//___________________________________________________________________
TList* ODBCConnection::RefreshDataSources(TList* gDataSources)
{
   // Fetch a list of of all available data sources ( TSQLUrl objects)
 
   if(!gDataSources) return 0;

   gDataSources->Delete(); // remove all 
   TSQLUrl* url;
         
   try {
      odbc::DataSourceList* list = 
            odbc::DriverManager::getDataSources();
   
      for( odbc::DataSourceList::iterator i=list->begin();
         i != list->end(); i++) {

         odbc::DataSource* ds = (*i);
         url = new TSQLUrl(ODBCXX_STRING_CSTR(ds->getName()),
                           ODBCXX_STRING_CSTR(ds->getDescription()));
         gDataSources->Add(url);   
      }   
   } catch(odbc::SQLException& e) {
      gSQLDriverManager->Throw( new TSQLException( 
                              ODBCXX_STRING_CSTR(e.getMessage()),
                              ODBCXX_STRING_CSTR(e.getSQLState()),
                              e.getErrorCode()) );
   }
   return  gDataSources;
}
