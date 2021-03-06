// $Id: MySQLDatabaseMetaData.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 26/02/2001 
//
// RDBC driver to MySQL database implemented with MySQL C API.
//
// ++ The code consists of some parts stolen from mm JDBC and  
//    MyODBC ODBC drivers and other mysql-related open sources.
//

/////////////////////////////////////////////////////////////////////
//
// Comprehensive information about the database as a whole. 
//
// Many of the methods here return lists of information in the form 
// of TSQLResultSet objects. You can use the normal TSQLResultSet 
// methods  such as TSQLResultSet::GetString() and 
// TSQLResultSet::GetInt() to retrieve the data from 
// these  TSQLResultSet s. If a given form of metadata is not available, 
// these methods should throw an TSQLException. 
//
// Some of these methods take arguments that are string patterns. 
// These arguments all have names such as fooPattern. Within a 
// pattern string, "%" means match any substring of 0 or more 
// characters, and "_" means match any one character. Only metadata 
// entries matching the search pattern are returned. If a search 
// pattern argument is set to a null ref, that argument's criteria 
// will be dropped from the search. 
//
//
/////////////////////////////////////////////////////////////////////
// 
//       Constants:
//
//___________________________________________________________________
// kProcedureResultUnknown
//
//      A possible value for column PROCEDURE_TYPE in the 
//      TSQLResultSet  object returned by the method GetProcedures(). 
//
//      Indicates that it is not known whether the procedure 
//      returns a result.
//
//___________________________________________________________________
// kProcedureNoResult
//
//      A possible value for column PROCEDURE_TYPE in the 
//      TSQLResultSet object returned by the  method GetProcedures(). 
//
//      Indicates that the procedure does not return a result.
//
//___________________________________________________________________
// kProcedureReturnsResult
//
//      A possible value for column PROCEDURE_TYPE in the 
//      TSQLResultSet object returned by the  method GetProcedures(). 
//
//      Indicates that the procedure returns a result.
//
//___________________________________________________________________
// kProcedureColumnUnknown
//
//     Indicates that type of the column is unknown. A possible value 
//     for the column  COLUMN_TYPE in the TSQLResultSet returned by 
//     the method GetProcedureColumns().
//
//___________________________________________________________________
//  kProcedureColumnIn
//
//    Indicates that the column stores IN parameters. A possible 
//    value for the column  COLUMN_TYPE in the TSLQResultSet returned 
//    by the method GetProcedureColumns().
//
//___________________________________________________________________
// kProcedureColumnInOut
//
//     Indicates that the column stores INOUT parameters. A possible 
//    value for the column COLUMN_TYPE in the TSQLResultSet returned 
//    by the method GetProcedureColumns().
//
//___________________________________________________________________
// kProcedureColumnOut
//
//      Indicates that the column stores OUT parameters. A possible 
//      value for the column kCOLUMN_TYPE in the TSQLResultSet 
//      returned by the method GetProcedureColumns().
//
//___________________________________________________________________
// kProcedureColumnReturn
//
//      Indicates that the column stores return values. A possible 
//      value for the column COLUMN_TYPE in the TSQLResultSet returned 
//      by the method GetProcedureColumns().
//
//___________________________________________________________________
// kProcedureColumnResult
//
//      Indicates that the column stores results. A possible value 
//      for the column kCOLUMN_TYPE in the TSQLResultSet returned 
//      by the  method GetProcedureColumns().
//
//___________________________________________________________________
// kProcedureNoNulls
//
//      Indicates that NULL values are not allowed. A possible value 
//      for the column NULLABLE in the TSQLResultSet returned by the 
//      method GetProcedureColumns().
//
//___________________________________________________________________
// kProcedureNullable
//
//      Indicates that NULL values are allowed. A possible value for 
//      the column NULLABLE in  the TSQLResultSet returned by 
//      the method  GetProcedureColumns().
//
//___________________________________________________________________
// kProcedureNullableUnknown
//
//      Indicates that whether NULL values are allowed is unknown.
//      A possible value for the column NULLABLE in the TSQLResultSet 
//      returned by the method GetProcedureColumns().
//
//___________________________________________________________________
// kColumnNoNulls
//
//      Indicates that the column might not allow NULL values. 
//      A possible value for the column NULLABLE in the TSQLResultSet 
//      returned by the method GetColumns().
//
//___________________________________________________________________
// kColumnNullable
//
//      Indicates that the column definitely allows NULL values. 
//      A possible value for the column NULLABLE in the TSQLResultSet 
//      returned by the method GetColumns().
//
//___________________________________________________________________
//  kColumnNullableUnknown
//
//      Indicates that the nullability of columns is unknown. 
//      A possible value for the column NULLABLE in the TSQLResultSet 
//      returned by the method GetColumns().
//
//___________________________________________________________________
// kBestRowTemporary
//
//      Indicates that the scope of the best row identifier is very 
//      temporary, lasting only  while the row is being used. 
//      A possible  value for the column SCOPE in the TSQLResultSet 
//      object returned  by the method GetBestRowIdentifier().
//
//___________________________________________________________________
// kBestRowTransaction
//
//      Indicates that the scope of the best row identifier is the 
//      remainder of the current  transaction. A possible value for 
//      the column SCOPE in the TSQLResultSet object returned by
//      the method GetBestRowIdentifier().
//
//___________________________________________________________________
// kBestRowSession
//
//      Indicates that the scope of the best row identifier is the 
//      remainder of the current  session. A possible value for the 
//      column SCOPE in the TSQLResultSet object returned by 
//      the method GetBestRowIdentifier().
//
//___________________________________________________________________
// kBestRowUnknown
//
//      Indicates that the best row identifier may or may not be 
//      a pseudo column. A possible value for the column 
//      PSEUDO_COLUMN in the TSQLResultSet object returned by 
//      the method GetBestRowIdentifier().
//
//___________________________________________________________________
// kBestRowNotPseudo
//
//      Indicates that the best row identifier is NOT a pseudo 
//      column. A possible value for the column PSEUDO_COLUMN in 
//      the TSQLResultSet object returned by the method 
//      GetBestRowIdentifier().
//
//___________________________________________________________________
// kBestRowPseudo
//
//      Indicates that the best row identifier is a pseudo column. 
//      A possible value for the column PSEUDO_COLUMN in the 
//      TSQLResultSet object returned by the method  
//      GetBestRowIdentifier().
//
//___________________________________________________________________
// kVersionColumnUnknown
//
//      Indicates that this version column may or may not be a pseudo 
//      column. A possible value for the column PSEUDO_COLUMN in 
//      the TSQLResultSet object returned by the method 
//      GetVersionColumns().
//
//___________________________________________________________________
// kVersionColumnNotPseudo
//
//      Indicates that this version column is NOT a pseudo column. 
//      A possible value for the column PSEUDO_COLUMN in the 
//      TSQLResultSet object returned by the method GetVersionColumns().
//
//___________________________________________________________________
// kVersionColumnPseudo
//
//      Indicates that this version column is a pseudo column. 
//      A possible value for the column PSEUDO_COLUMN in the 
//      TSQLResultSet object returned by the method GetVersionColumns().
//
//___________________________________________________________________
// kImportedKeyCascade
//
//      A possible value for the columns UPDATE_RULE and 
//    DELETE_RULE in the TSQLResultSet objects returned by the 
//    methods GetImportedKeys(), GetExportedKeys(), 
//    and GetCrossReference(). 
//
//      For the column UPDATE_RULE, it indicates that when the 
//      primary key is updated, the foreign key (imported key) 
//      is changed to agree with it. 
//
//      For the column DELETE_RULE, it indicates that when the 
//      primary key is deleted, rows that imported that key 
//      are deleted.
//
//___________________________________________________________________
// kImportedKeyRestrict
//
//      A possible value for the columns UPDATE_RULE and DELETE_RULE 
//    in the TSQLResultSet objects returned by the methods 
//    GetImportedKeys(), GetExportedKeys(), and GetCrossReference(). 
//
//      For the column UPDATE_RULE, it indicates that a primary 
//    key may not be updated if it has been imported by another 
//    table as a foreign key. 
//
//      For the column DELETE_RULE, it indicates that a primary 
//      key may not be deleted if it has been imported by another
//     table as a foreign key.
//
//___________________________________________________________________
// kImportedKeySetNull
//
//      A possible value for the columns UPDATE_RULE and DELETE_RULE 
//    in the TSQLResultSet objects returned by the methods 
//    GetImportedKeys(), GetExportedKeys(), and GetCrossReference(). 
//
//      For the columns UPDATE_RULE and DELETE_RULE, it indicates 
//      that when the primary key is  updated or deleted, the 
//      foreign key (imported key) is changed to NULL.
//
//___________________________________________________________________
// kImportedKeyNoAction
//
//      A possible value for the columns UPDATE_RULE and DELETE_RULE 
//      in the TSQLResultSet objects returned by the methods 
//      GetImportedKeys(), GetExportedKeys(), and GetCrossReference(). 
//
//      For the columns UPDATE_RULE and DELETE_RULE, it indicates 
//      that if the primary key has been imported, it cannot be 
//     updated or deleted.
//
//___________________________________________________________________
// kImportedKeySetDefault
//
//      A possible value for the columns UPDATE_RULE and DELETE_RULE
//      in the TSQLResultSet objects returned by the methods 
//      GetImportedKeys(), GetExportedKeys(), and GetCrossReference(). 
//
//      For the columns UPDATE_RULE and DELETE_RULE, it indicates 
//      that if the primary key is updated or deleted, 
//      the foreign key (imported key) is set to the default value.
//
//___________________________________________________________________
// kImportedKeyInitiallyDeferred
//
//      A possible value for the column DEFERRABILITY in 
//     the TSQLResultSet objects returned by the methods 
//     GetImportedKeys(), GetExportedKeys(), and GetCrossReference(). 
//
//      Indicates deferrability. See SQL-92 for a definition.
//
//___________________________________________________________________
// kImportedKeyInitiallyImmediate
//
//      A possible value for the column DEFERRABILITY in 
//    the TSQLResultSet objects returned by the methods 
//    GetImportedKeys(), GetExportedKeys(), and GetCrossReference(). 
//
//      Indicates deferrability. See SQL-92 for a definition.
//
//___________________________________________________________________
// kImportedKeyNotDeferrable
//
//      A possible value for the column DEFERRABILITY in 
//      the TSQLResultSet objects returned by the methods 
//      GetImportedKeys(), GetExportedKeys(), and GetCrossReference(). 
//
//      Indicates deferrability. See SQL-92 for a definition.
//
//___________________________________________________________________
// kTypeNoNulls
//
//      A possible value for column NULLABLE in the TSQLResultSet 
//      object returned by the method GetTypeInfo(). 
//
//      Indicates that a NULL value is NOT allowed for 
//      this data type.
//
//___________________________________________________________________
// kTypeNullable
//
//      A possible value for column NULLABLE in the TSQLResultSet 
//      object returned by the method GetTypeInfo(). 
//
//      Indicates that a NULL value is allowed for this data type.
//
//___________________________________________________________________
// kTypeNullableUnknown
//
//      A possible value for column NULLABLE in the TSQLResultSet 
//      object returned by the method GetTypeInfo(). 
//
//      Indicates that it is not known whether a NULL value 
//      is allowed for this data type.
//
//___________________________________________________________________
// kTypePredNone
//
//      A possible value for column SEARCHABLE in the TSQLResultSet 
//      object returned by the method GetTypeInfo(). 
//
//      Indicates that WHERE search clauses are not supported 
//      for this type.
//
//___________________________________________________________________
// kTypePredChar
//
//      A possible value for column SEARCHABLE in the TSQLResultSet 
//      object returned by the method GetTypeInfo(). 
//
//      Indicates that the only WHERE search clause that can be 
//      based on this type is WHERE . . .LIKE.
//
//___________________________________________________________________
// kTypePredBasic
//
//      A possible value for column SEARCHABLE in the TSQLResultSet 
//    object returned by the method GetTypeInfo(). 
//
//      Indicates that one can base all WHERE search clauses 
//      except WHERE . . .LIKE on this data type.
//
//___________________________________________________________________
// kTypeSearchable
//
//      A possible value for column SEARCHABLE in the TSQLResultSet 
//      object returned by the method GetTypeInfo(). 
//
//      Indicates that all WHERE search clauses can be based 
//      on this type.
//
//___________________________________________________________________
// kTableIndexStatistic
//
//      A possible value for column TYPE in the TSQLResultSet 
//    object returned by the method GetIndexInfo(). 
//
//      Indicates that this column contains table statistics 
//      that are returned in conjunction with a table's index 
//      descriptions.
//
//___________________________________________________________________
// kTableIndexClustered
//
//      A possible value for column TYPE in the TSQLResultSet object 
//      returned by the method GetIndexInfo(). 
//
//      Indicates that this table index is a clustered index.
//
//___________________________________________________________________
// kTableIndexHashed
//
//      A possible value for column TYPE in the TSQLResultSet object 
//      returned by the method GetIndexInfo(). 
//
//      Indicates that this table index is a hashed index.
//
//___________________________________________________________________
// kTableIndexOther
//
//      A possible value for column TYPE in the TSQLResultSet object 
//      returned by the method GetIndexInfo(). 
//
//      Indicates that this table index is not a clustered index, 
//      a hashed index, or table statistics; it is something 
//      other than these.
//
//  See also:
//       TSQLTypes TSQLConnection
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLDatabaseMetaData.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/configure.h>   //


#include "MySQLConnectionPrivate.h"

ClassImpQ(TSQLDatabaseMetaData)

// allowed extra chars ( stolen from MyODBC-2.50.36/info.c )
UChar_t gAllowedChars[]= {
  '\307','\374','\351','\342','\344','\340','\345','\347','\352','\353',
  '\350','\357','\356','\354','\304','\305','\311','\346','\306','\364',
  '\366','\362','\373','\371','\377','\326','\334','\341','\355','\363',
  '\372','\361','\321',0};

// from <http://mysql.com/doc///L/e/Legal_names.html> && mysql_com.h
const Int_t gNameLength = NAME_LEN; // Field/table name length
const Int_t gMaxDatabaseNameLength = HOSTNAME_LENGTH;
const Int_t gMaxTableNameLength = NAME_LEN;
const Int_t gMaxColumnNameLength = NAME_LEN;
const Int_t gMaxUserNameLength = USERNAME_LENGTH;
const Int_t gMaxAliasNameLength = 64;
const Int_t gMaxIndexLength = 256;
const Int_t gMaxLiteralLength = 0;  // == 4294967295 (2^32 - 1) - NET_HEADER_SIZE - COMP_HEADER_SIZE

MySQLResultSetPrivate* gResult = 0;  // result set used in methods which return TSQLResultSets
 
/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLDatabaseMetaData::TSQLDatabaseMetaData(TSQLConnection* connection,
                                           void* imp):TSQL(imp)
{
   // constructor
   
   fConnection = connection;
   if(!gResult) gResult =  = new MySQLResultSetPrivate();
}

//___________________________________________________________________
TSQLDatabaseMetaData::~TSQLDatabaseMetaData()
{
   // destructor will be called when fConnection is deleted 

   fConnection = 0;
   fImp = 0;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::AllProceduresAreCallable()
{
   // Can all the procedures returned by GetProcedures be 
   // called by the current user?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::AllTablesAreSelectable()
{
   // Can all the tables returned by GetTable() be SELECTed by 
   // the current user?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetUserName()
{
   // What's our user name as known to the database?
   //
   //   Returns:
   //        our database user name
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Identifies the user name in a particular database
   // (this name can be different from the login name)
   //
   
   TString str;
   TSQLStatement* stmt = fConnection->CreateSatement();
   TSQLResultSet* rs = stmt->ExecuteQuery("select substring_index(USER(),\"@\",1)");
   rs->Next();
   str = rs->GetString(1);
   delete stmt; 
   return str;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::IsReadOnly()
{
   // Is the database in read-only mode?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // This characteristic pertains only to the data source itself; 
   // it is not a characteristic of the driver that enables access 
   // to the data source. A driver that is read/write can be used 
   // with a data source that is read-only.
   // 
   // If a driver is read-only, all its data sources must be read-only
   // and must return kTRUE for this method
   //

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::NullsAreSortedHigh()
{
   // Are NULL values sorted high?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // Identifies where NULL values are sorted in a result data set
   //

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::NullsAreSortedLow()
{
   // Are NULL values sorted low?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // Identifies where NULL values are sorted in a result data set
   //   

   return !NullsAreSortedHigh();
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::NullsAreSortedAtStart()
{
   // Are NULL values sorted at the start regardless of sort order?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // Identifies where NULL values are sorted in a result data set
   //   

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::NullsAreSortedAtEnd()
{
   // Are NULL values sorted at the end regardless of sort order?
   //
   //  Returns:
   //        kTRUE if so; kFALSE otherwise
   //  Throws:
   //        TSQLException - if a database access error occurs
   //
   // Identifies where NULL values are sorted in a result data set
   //
   
   return kFALSE;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetDatabaseProductName()
{
   // What's the name of this database product?
   //
   //   Returns:
   //         database product name
   //   Throws:
   //         TSQLException - if a database access error occurs

   return "MySQL";
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetDatabaseProductVersion()
{
   // What's the version of this database product?
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // This information returned in a string that has format
   // mm.vv.rrrr, where mm is the major version number,
   // vv is the minor version number, and rrrr is the release
   // virsion number. 
   //
   //   Returns:
   //         database version
   //   Throws:
   //         TSQLException - if a database access error occurs

   MySQLConnectionPrivate* imp = (MySQLConnectionPrivate*)fImp; 
   return mysql_get_server_info(imp->fMYSQL);
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetDriverName()
{
   // What's the name of this ODBC driver?
   //
   //   Returns:
   //         driver name
   //   Throws:
   //         TSQLException - if a database access error occurs

   return "VO MySQL Driver";
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetDriverVersion()
{
   // What's the version of this ODBC driver?
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // This information returned in a string that has format
   // mm.vv.rrrr, where mm is the major version number,
   // vv is the minor version number, and rrrr is the release
   // virsion number 
   //   
   //   Returns:
   //         driver version
   //   Throws:
   //         TSQLException - if a database access error occurs

   TString str = RDBC_VERSION;
   return str;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetDriverMajorVersion()
{
   // What's this driver's major version number?
   //
   //   Returns:
   //         driver major version

   Int_t return_value = 0;
   return return_value;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetDriverMinorVersion()
{
   // What's this driver's minor version number?
   //
   //   Returns:
   //         driver minor version number

   Int_t return_value = 0;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::UsesLocalFiles()
{
   // Does the database store tables in a local file?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // kFALSE means that driver is a not single-tier driver
   // thereore files are not supported

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::UsesLocalFilePerTable()
{
   // Does the database use a file for each table?
   //
   //   Returns:
   //         kTRUE if the database uses a local file for each table
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // The single-tier driver treats files in a data source as
   // tables
   //

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsMixedCaseIdentifiers()
{
   // Does the database treat mixed case unquoted SQL identifiers 
   // as case sensitive and as a result store them in mixed case? 
   // A SQL-92 complient driver will always return kFALSE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::StoresUpperCaseIdentifiers()
{
   // Does the database treat mixed case unquoted SQL identifiers 
   // as case insensitive and store them in upper case?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::StoresLowerCaseIdentifiers()
{
   // Does the database treat mixed case unquoted SQL identifiers 
   // as case insensitive and store them in lower case?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

  return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::StoresMixedCaseIdentifiers()
{
   // Does the database treat mixed case unquoted SQL identifiers 
   // as case insensitive and store them in mixed case?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsMixedCaseQuotedIdentifiers()
{
   // Does the database treat mixed case quoted SQL identifiers 
   // as case sensitive and as a result store them in mixed case? 
   // A SQL-92 entry level-conformant driver will always 
   // return kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::StoresUpperCaseQuotedIdentifiers()
{
   // Does the database treat mixed case quoted SQL identifiers 
   // as case insensitive and store them in upper case?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::StoresLowerCaseQuotedIdentifiers()
{
   // Does the database treat mixed case quoted SQL identifiers 
   // as case insensitive and store them in lower case?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::StoresMixedCaseQuotedIdentifiers()
{
   // Does the database treat mixed case quoted SQL identifiers 
   // as case insensitive and store them in mixed case?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetIdentifierQuoteString()
{
   // What's the string used to quote SQL identifiers? 
   // This returns a space " " if identifier quoting isn't supported. 
   // A SQL-92 Full level-conformant  driver always uses a double
   // quote character (").
   //
   //   Returns:
   //         the quoting string
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Identifies the character tht is to be used as the starting and
   // ending delimeter of a quoted(delimetered) identifier in SQL
   // statements. (Identifiers passed in ODBC function parameters
   // do not need to be quoted) This character can also be used for
   // quoting catalog function parameters when ...
   // 
   // If the data source does not support quoted identifiers,
   // a blank string (" ") is returned
   //

   return using323() ? "`" : " ";
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetSQLKeywords()
{
   // Gets a comma-separated list of all a database's SQL keywords 
   // that are NOT also SQL92 keywords.
   //
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   //   See also <http://mysql.com/doc///E/x/Extensions_to_ANSI.html>

   // stolen from MyODBC-2.50.36/info.c
   TString str =  "UNIQUE,ZEROFILL,UNSIGNED,BIGINT,BLOB,TINYBLOB,MEDIMUMBLOB,"
                  "LONGBLOB,MEDIUMINT,PROCEDURE,SHOW,LIMIT,DEFAULT,TABLES,"
                  "REGEXP,RLIKE,KEYS,TINYTEXT,MEDIUMTEXT";
   return str;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetNumericFunctions()
{
   // Gets a comma-separated list of math functions. 
   // These are the X/Open CLI math function names used in the ODBC 
   // function escape clause.
   //
   //   Returns:
   //         the list
   //    Throws:
   //         TSQLException - if a database access error occurs
  
   TString str =  "ABS,ACOS,ASIN,ATAN,ATAN2,BIT_COUNT,CEILING,COS,COT,"
                  "DEGREES,EXP,FLOOR,LOG,LOG10,MAX,MIN,MOD,PI,POW,POWER,"
                  "RADIANS,RAND,ROUND,SIN,SQRT,TAN,TRUNCATE";
   return str;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetStringFunctions()
{
   // Gets a comma-separated list of string functions. 
   // These are the X/Open CLI string function names used in the 
   // ODBC function escape clause.
   //
   //   Returns:
   //         the list
   //   Throws:
   //         TSQLException - if a database access error occurs

   TString str =  "ASCII,CHAR,CHAR_LENGTH,CHARACTER_LENGTH,CONCAT,"
                  "ELT,FIELD,FIND_IN_SET,INSERT,INSTR,INTERVAL,"
                  "LCASE,LEFT,LENGTH,LOCATE,LOWER,LTRIM,MID,POSITION,"
                  "OCTET_LENGTH,REPEAT,REPLACE,REVERSE,RIGHT,RTRIM,"
                  "SPACE,SOUNDEX,SUBSTRING,SUBSTRING_INDEX,TRIM,"
                  "UCASE,UPPER";  
   return str;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetSystemFunctions()
{
   // Gets a comma-separated list of system functions. 
   // These are the X/Open CLI system function names used in the 
   // ODBC function escape clause.
   //
   //   Returns:
   //         the list
   //   Throws:
   //         TSQLException - if a database access error occurs

   TString str =  "DATABASE,USER,SYSTEM_USER,SESSION_USER,"
                  "PASSWORD,ENCRYPT,LAST_INSERT_ID,VERSION";
   return str;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetTimeDateFunctions()
{
   // Gets a comma-separated list of time and date functions.
   //
   //   Returns:
   //         the list
   //   Throws:
   //         TSQLException - if a database access error occurs

   TString str =  "DAYOFWEEK,WEEKDAY,DAYOFMONTH,DAYOFYEAR,MONTH,"
                  "DAYNAME,MONTHNAME,QUARTER,WEEK,YEAR,HOUR,MINUTE,"
                  "SECOND,PERIOD_ADD,PERIOD_DIFF,TO_DAYS,FROM_DAYS,"
                  "DATE_FORMAT,TIME_FORMAT,CURDATE,CURRENT_DATE,"
                  "CURTIME,CURRENT_TIME,NOW,SYSDATE,CURRENT_TIMESTAMP,"
                  "UNIX_TIMESTAMP,FROM_UNIXTIME,SEC_TO_TIME,TIME_TO_SEC";

   return str;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetSearchStringEscape()
{
   // Gets the string that can be used to escape wildcard characters. 
   // This is the string that can be used to escape '_' or '%' in 
   // the string pattern style catalog search parameters. 
   //
   //   The '_' character represents any single character. 
   //
   //   The '%' character represents any sequence of zero or more 
   //    characters.
   //
   //   Returns:
   //         the string used to escape wildcard characters
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // If driver does not provide a search-pattern escape character,
   // there is no specified limit, or if limit is unknown, an
   // empt string (" ") is returned.

   return "\\";
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetExtraNameCharacters()
{
   // Gets all the "extra" characters that can be used in unquoted 
   // identifier names ( those beyond a-z, A-Z, 0-9 and _ ),
   // for example, a table, column or index name.
   //
   //   Returns:
   //         the string containing the extra characters
   //   Throws:
   //         TSQLException - if a database access error occurs

   return (char*)gAllowedChars;  //
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsAlterTableWithAddColumn()
{
   // Is "ALTER TABLE" with add column supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsAlterTableWithDropColumn()
{
   // Is "ALTER TABLE" with drop column supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsColumnAliasing()
{
   // Is column aliasing supported? 
   //
   //  If so, the SQL "AS" clause can be used to provide names 
   // for computed columns or to provide alias names for columns 
   // as required. A SQL-92 entry level-conformant driver always 
   // returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::NullPlusNonNullIsNull()
{
   // Are concatenations between NULL and non-NULL values NULL? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsConvert()
{
   // Is the CONVERT function between SQL types supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

  return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsConvert( Int_t fromType,
                                              Int_t toType )
{
   // Is CONVERT between the given SQL types supported?
   //
   //   Parameters:
   //         fromType - the type to convert from
   //         toType - the type to convert to
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         TSQLTypes.h
   //
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //   
   // Identifiers whether the specified data type conversion
   // is supported by the data source and the CONVERT()
   // scalar function. For example, to find out if a data
   // source supports the conversion of a SQL_INTEGER data
   // type to an SQL_BIGINT data type, an application calls
   // SupportsConvert(kINTEGER,kBIGINT)
   // If the resulting value is kTRUE, the data type conversion
   // is supported.
 
   switch (fromType) {

      // The char/binary types can be converted to pretty much anything.
      case kCHAR:
      case kVARCHAR:
      case kLONGVARCHAR:
      case kBINARY:
      case kVARBINARY:
      case kLONGVARBINARY:
      
      switch (toType) {
         case kDECIMAL:
         case kNUMERIC:
         case kREAL:
         case kTINYINT:
         case kSMALLINT:
         case kINTEGER:
         case kBIGINT: 
         case kFLOAT:
         case kDOUBLE:
         case kCHAR:
         case kVARCHAR:
         case kLONGVARCHAR:
         case kBINARY:
         case kVARBINARY:
         case kLONGVARBINARY:
         case kOTHER:
         case kDATE:
         case kTIME:
         case kTIMESTAMP:
            return kTRUE;
         default:
            return kFALSE;
      }
          
      // We don't handle the BIT type yet.
      case kBIT:
         return kFALSE;

      // The numeric types. Basically they can convert
      // among themselves, and with char/binary types.
  
      case kDECIMAL:
      case kNUMERIC:
      case kREAL:
      case kTINYINT:
      case kSMALLINT:
      case kINTEGER:
      case kBIGINT:
      case kFLOAT:
      case kDOUBLE:
      
      switch (toType) {
         case kDECIMAL:
         case kNUMERIC:
         case kREAL:
         case kTINYINT:
         case kSMALLINT:
         case kINTEGER:
         case kBIGINT: 
         case kFLOAT:
         case kDOUBLE:
         case kCHAR:
         case kVARCHAR:
         case kLONGVARCHAR:
         case kBINARY:
         case kVARBINARY:
         case kLONGVARBINARY: 
            return kTRUE;
         default:
            return kFALSE;
      }
          
      // MySQL doesn't support a NULL type.
      case kNULL:
         return kFALSE;

      case kOTHER:
      
      switch (toType) {
         case kCHAR:
         case kVARCHAR:
         case kLONGVARCHAR:
         case kBINARY:
         case kVARBINARY:
         case kLONGVARBINARY:
            return kTRUE;
         default:
            return kFALSE;
      }
         
      // Dates can be converted to char/binary types.
      case kDATE:
      
      switch (toType) {
         case kCHAR:
         case kVARCHAR:
         case kLONGVARCHAR:
         case kBINARY:
         case kVARBINARY:
         case kLONGVARBINARY:
            return kTRUE;
         default:
            return kFALSE;
      }
          
      // Time can be converted to char/binary types
      case kTIME:
         switch (toType) {
         case kCHAR:
         case kVARCHAR:
         case kLONGVARCHAR:
         case kBINARY:
         case kVARBINARY:
         case kLONGVARBINARY:
            return kTRUE;
         default:
            return kFALSE;
         }

      // Timestamp can be converted to char/binary types
      // and date/time types (with loss of precision).
      case kTIMESTAMP:
         switch (toType) {
         case kCHAR:
         case kVARCHAR:
         case kLONGVARCHAR:
         case kBINARY:
         case kVARBINARY:
         case kLONGVARBINARY:
         case kTIME:
         case kDATE:
            return kTRUE;
         default:
            return kFALSE;
      }
        
      // We shouldn't get here!
      default:
         return kFALSE; // not sure
   }   
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsTableCorrelationNames()
{
   // Are table correlation names supported? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsDifferentTableCorrelationNames()
{
   // If table correlation names are supported, are they restricted
   // to be different from the names of the tables?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsExpressionsInOrderBy()
{
   // Are expressions in "ORDER BY" lists supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsOrderByUnrelated()
{
   // Can an "ORDER BY" clause use columns not in the 
   // SELECT statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsGroupBy()
{
   // Is some form of "GROUP BY" clause supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsGroupByUnrelated()
{
   // Can a "GROUP BY" clause use columns not in the SELECT?
   //
   // For example, 
   //    SELECT DEPT,SALARY FROM EMPLOYEE GROUP BY DEPT,AGE
   // 
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsGroupByBeyondSelect()
{
   // Can a "GROUP BY" clause add columns not in the SELECT provided 
   // it specifies all the columns in the SELECT?
   //
   // For example, 
   //    SELECT DEPT,MAX(SALARY) FROM EMPLOYEE GROUP BY DEPT,AGE
   // 
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsLikeEscapeClause()
{
   // Is the escape character in "LIKE" clauses supported? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   // 

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsMultipleResultSets()
{
   // Are multiple TSQLResultSets from a single execute supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsMultipleTransactions()
{
   // Can we have multiple transactions open at once (on different
   // connections)?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsNonNullableColumns()
{
   // Can columns be defined as non-nullable? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Indicates whether the data source supports the "NOT NULL"
   // column constraint in "CREATE TABLE" SQL statements
   //

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsMinimumSQLGrammar()
{
   // Is the ODBC Minimum SQL grammar supported? 
   // All SQL-92 entry level-conformant drivers must return kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsCoreSQLGrammar()
{
   // Is the ODBC Core SQL grammar supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsExtendedSQLGrammar()
{
   // Is the Extended SQL grammar supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsANSI92EntryLevelSQL()
{
   // Is the ANSI92 entry level SQL grammar supported? 
   // All SQL-92 entry level-conformant drivers must return kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;  // MyODBC says "Lie to make things work"
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsANSI92IntermediateSQL()
{
   // Is the ANSI92 intermediate SQL grammar supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;  // MyODBC says "Lie to make things work"
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsANSI92FullSQL()
{
   // Is the ANSI92 full SQL grammar supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;  // MyODBC says "Lie to make things work"
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsIntegrityEnhancementFacility()
{
   // Is the SQL Integrity Enhancement Facility supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsOuterJoins()
{
   // Is some form of outer join supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsFullOuterJoins()
{
   // Are full nested outer joins supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsLimitedOuterJoins()
{
   // Is there limited support for outer joins? (This will be kTRUE 
   // if SupportFullOuterJoins is kTRUE.)
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetSchemaTerm()
{
   // What's the database vendor's preferred term for "schema"?
   //
   // For example "owner",Authorization ID", or "Schema"
   // this string can be in upper, lower , or mixed case.
   //
   //  An SQL-92 entry level-conformant driver returns "schema"
   //
   //   Returns:
   //         the vendor term
   //   Throws:
   //         TSQLException - if a database access error occurs

   return "owner";
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetTableTerm()
{
   // What's the database vendor's preferred term for "table"
   //
   //  An SQL-92 entry level-conformant driver returns "schema"
   //
   //   Returns:
   //         the vendor term
   //   Throws:
   //         TSQLException - if a database access error occurs

   return "table";
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetProcedureTerm()
{
   // What's the database vendor's preferred term for a
   // stored procedure?
   //
   // For example,"database procedure","stored procedure",
   // "procedure","package", or "stored query"
   //
   //   Returns:
   //         the vendor term
   //   Throws:
   //         TSQLException - if a database access error occurs

   return "procedure";
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetCatalogTerm()
{
   // What's the database vendor's preferred term for "catalog"?
   //
   // Can be for example "directory" or "database"
   // This string can be in upper, lower , or mixed case.
   // I catalogs are not supported by the data source, an empty
   // string ("") is returned.
   //
   //   Returns:
   //         the vendor term
   //   Throws:
   //         TSQLException - if a database access error occurs

   return "database";
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::IsCatalogAtStart()
{
   // Does a catalog appear at the start of a qualified table name? 
   // (Otherwise it appears at the end)
   //
   // For example MySQL would say kTRUE, 
   // dBASE driver also returns kTRUE because the directory (catalog)
   // name is at the start of the table name, a in \EMPDATA\EMP.DBF
   // An ORACLE Server driver returns kFALSE, because the catalog is
   // at the end of the table name as in ADMIN.EMP@EMPDATA
   // 
   //   Returns:
   //         kTRUE if it appears at the start
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
TString TSQLDatabaseMetaData::GetCatalogSeparator()
{
   // What's the separator between catalog and table name?
   //
   // For example ORACLE would return a "@", while MySQL would say "." 
   // If catalogs are not supported by the data source, an empty 
   // string ("") is returned.
   //
   // An SQL-92 Full level conformant driver returns "."
   //
   //   Returns:
   //         the separator string
   //   Throws:
   //         TSQLException - if a database access error occurs

   return ".";
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSchemasInDataManipulation()
{
   // Can a schema name be used in a data manipulation statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Returns kTRUE if schemas are supported in all Data
   // Manipulation Language (DML) statements:
   //    SELECT,INSERT,UPDATE,DELETE, and , if supported
   //    SELECT FOR UPDATE and positioned UPDATE and DELETE
   //    statements.
   //
   // An SQL-92 entry level-conformant driver returns kTRUE.
 
   Bool_t return_value = kFALSE; 
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSchemasInProcedureCalls()
{
   // Can a schema name be used in a procedure call statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSchemasInTableDefinitions()
{
   // Can a schema name be used in a table definition statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   // 
   // Returns kTRUE if schemas are supported in all table
   // definition statements: CREATE TABLE,CREATE VIEW,
   // ALTER TABLE, DROP TABLE and DROP VIEW
   //
   // An SQL-92 entry level-conformant driver returns kTRUE.

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSchemasInIndexDefinitions()
{
   // Can a schema name be used in an index definition statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   
   // Returns kTRUE if schemas are supported in all index
   // definition statements: CREATE INDEX and DROP INDEX

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSchemasInPrivilegeDefinitions()
{
   // Can a schema name be used in a privilege definition statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // Returns kTRUE if schemas are supported in all privelege
   // definition statements: GRANT and REVOKE
   //
   // An SQL-92 entry level-conformant driver returns kTRUE.

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsCatalogsInDataManipulation()
{
   // Can a catalog name be used in a data manipulation statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Returns kTRUE if catalog names can be used in all Data
   // Manipulation Language (DML) statements:
   //    SELECT,INSERT,UPDATE,DELETE, and , if supported
   //    SELECT FOR UPDATE and positioned UPDATE and DELETE
   //    statements.
   //
   // An SQL-92 Full level-conformant driver returns kTRUE

       // Servers before 3.22 could not do this
        if (_Conn.getServerMajorVersion() >= 3) { // newer than version 3?
            if (_Conn.getServerMajorVersion() == 3) {
                if (_Conn.getServerMinorVersion() >= 22) { // minor 22?
                    return true;
                }
                else {
                    return false; // Old version 3
                }
            }
            else {
                return true; // newer than version 3.22
            }
        }
        else {
            return false; // older than version 3
        }

}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsCatalogsInProcedureCalls()
{
   // Can a catalog name be used in a procedure call statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // An SQL-92 Full level-conformant driver returns kTRUE

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsCatalogsInTableDefinitions()
{
   // Can a catalog name be used in a table definition statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   // 
   // Returns kTRUE if catalog names can be used in all table
   // definition statements: CREATE TABLE,CREATE VIEW,
   // ALTER TABLE, DROP TABLE and DROP VIEW

   Bool_t return_value = kFALSE; 
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsCatalogsInIndexDefinitions()
{
   // Can a catalog name be used in an index definition statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   
   // Returns kTRUE if catalog names can be used in all index
   // definition statements: CREATE INDEX and DROP INDEX
   //
   // An SQL-92 Full level-conformant driver returns kTRUE.

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsCatalogsInPrivilegeDefinitions()
{
   // Can a catalog name be used in a privilege definition statement?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // Returns kTRUE if catalog names can be used  in all privelege
   // definition statements: GRANT and REVOKE
   //
   // An SQL-92 Full level-conformant driver returns kTRUE

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsPositionedDelete()
{
   // Is positioned DELETE supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsPositionedUpdate()
{
   // Is positioned UPDATE supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSelectForUpdate()
{
   // Is SELECT for UPDATE supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsStoredProcedures()
{
   // Are stored procedure calls using the stored procedure escape 
   // syntax supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSubqueriesInComparisons()
{
   // Are subqueries in comparison expressions supported? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSubqueriesInExists()
{
   // Are subqueries in EXISTS expressions supported? 
   // A SQL-92 entry level-conformant driver always  returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSubqueriesInIns()
{
   // Are subqueries in IN statements supported? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsSubqueriesInQuantifieds()
{
   // Are subqueries in quantified expressions supported? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsCorrelatedSubqueries()
{
   // Are correlated subqueries supported? 
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsUnion()
{
   // Is SQL UNION supported?
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsUnionAll()
{
   // Is SQL UNION ALL supported?
   // A SQL-92 entry level-conformant driver always returns kTRUE.
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsOpenCursorsAcrossCommit()
{
   // Can cursors remain open across commits?
   // 
   // Returns kTRUE if the data source and the driver can handle
   // open cursors (eg. TSQLResultSets) across a commit, or kFALSE 
   // if they are invalidated.
   //
   //   Returns:
   //         kTRUE if cursors always remain open; 
   //         kFALSE if they might not remain open
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Returns kTRUE if cursors and access plans for prepared SQL 
   // statements reamin as they were before the COMMIT operation
   // ( see TSQLConnection::Commit() method ) was performed. 
   // The application can continue to fetch data or it can close 
   // cursor  and re-execute SQL statement without having to 
   // re-prepare it.
   //
    
   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsOpenCursorsAcrossRollback()
{
   // Can cursors remain open across rollbacks?
   // 
   // Returns kTRUE if the data source and the driver can handle
   // open cursors (eg. TSQLResultSet s) across a rollback, or kFALSE 
   // if they are invalidated.
   //
   //   Returns:
   //         kTRUE if cursors always remain open; 
   //         kFALSE if they might not remain open
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Returns kTRUE if cursors and access plans for prepared SQL 
   // statements reamin as they were before the ROLLBACK operation
   // ( see TSQLConnection::Rollback() method ) was performed. 
   // The application can continue to fetch data or it can close 
   // cursor  and re-execute SQL statement without having to 
   // re-prepare it.
   //
   
   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsOpenStatementsAcrossCommit()
{
   // Can statements remain open across commits?
   //
   // Returns kTRUE if the data source and the driver can handle
   // open statements across a commit, or kFALSE if
   // they are invalidated.   
   //
   //   Returns:
   //         kTRUE if statements always remain open; 
   //         kFALSE if they might not remain open
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsOpenStatementsAcrossRollback()
{
   // Can statements remain open across rollbacks?
   //
   // Returns kTRUE if the data source and the driver can handle
   // open statements across a rollback, or kFALSE if
   // they are invalidated.   
   //
   //   Returns:
   //         kTRUE if statements always remain open; 
   //         kFALSE if they might not remain open
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxBinaryLiteralLength()
{
   // How many hex characters can you have in an inline binary 
   // literal (number of hexadecimal characters, exluding the 
   // literal prefix and suffix ) ? For example the binary
   // literal 0xFFAA has a length of 4.
   //
   //   Returns:
   //         max binary literal length in hex characters; a result 
   //         of zero means that thereis no limit or the limit is 
   //         not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return gMaxLiteralLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxCharLiteralLength()
{
   // What's the max length for a character literal
   // (number of characters, exluding the literal prefix
   // and suffix ) ?
   //
   //   Returns:
   //          max literal length; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return gMaxLiteralLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxColumnNameLength()
{
   // What's the limit on column name length?
   //
   //    Returns:
   //          max column name length; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // See also <http://mysql.com/doc///L/e/Legal_names.html>

   return gMaxColumnNameLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxColumnsInGroupBy()
{
   // What's the maximum number of columns in a "GROUP BY" clause?
   //
   //   Returns:
   //          max number of columns; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return 16;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxColumnsInIndex()
{
   // What's the maximum number of columns allowed in an index?
   //
   //   Returns:
   //          max number of columns; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   //  See also <http://mysql.com/doc///M/u/Multiple-column_indexes.html>

   return 16;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxColumnsInOrderBy()
{
   // What's the maximum number of columns in an "ORDER BY" clause?
   //
   //   Returns:
   //          max number of columns; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return 16;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxColumnsInSelect()
{
   // What's the maximum number of columns in a "SELECT" list?
   //
   //   Returns:
   //          max number of columns; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return 256;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxColumnsInTable()
{
   // What's the maximum number of columns in a table?
   //
   //   Returns:
   //          max number of columns; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //          TSQLException - if a database access error occurs

   return 32;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxConnections()
{
   // How many active connections can we have at a time to this 
   // database? This value can reflect a limitation imposed  by
   // either the driver or the data source.
   //
   //   Returns:
   //          max number of active connections; a result of zero 
   //          means that there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return 0;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxCursorNameLength()
{
  // What's the maximum cursor name length?
  //
  //    Returns:
  //           max cursor name length in bytes; a result of zero means 
  //           that there is no limit or the limit is not known
  //    Throws:
  //          TSQLException - if a database access error occurs

   return gMaxCursorNameLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxIndexLength()
{
   // What's the maximum length of an index (in bytes)?
   //
   //   Returns:
   //          max index length in bytes; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // See also, <http://mysql.com/doc///I/n/Indexes.html>

   return gMaxIndexLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxSchemaNameLength()
{
   // What's the maximum length allowed for a schema name?
   //
   //   Returns:
   //          max name length in bytes; a result of zero means that 
   //          there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return 0;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxProcedureNameLength()
{
   //  What's the maximum length of a procedure name?
   //
   //    Returns:
   //          max name length in bytes; a result of zero means that 
   //         there is no limit or the limit is not known
   //    Throws:
   //          TSQLException - if a database access error occurs

   return 0;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxCatalogNameLength()
{
   // What's the maximum length of a catalog name?
   //
   //   Returns:
   //         max name length in bytes; a result of zero means 
   //         that there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   //   See also, <http://mysql.com/doc///L/e/Legal_names.html>

   return gMaxDatabaseNameLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxRowSize()
{
   // What's the maximum length of a single row?
   //
   //   Returns:
   //         max row size in bytes; a result of zero means 
   //         that there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return 0;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::DoesMaxRowSizeIncludeBlobs()
{
   // Did GetMaxRowSize() include LONGVARCHAR and LONGVARBINARY blobs?
   //
   // Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   
   return kTRUE;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxStatementLength()
{
   // What's the maximum length of a SQL statement?
   //
   //   Returns:
   //         max length in bytes; a result of zero means that there 
   //         is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return MySQLdefs::kMAXBUF - 4; // Max buffer - header
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxStatements()
{
   // How many active statements can we have open at one time to this 
   // database?
   //
   //   Returns:
   //        the maximum number of statements that can be open at 
   //        one time; a result of zero means that there is no limit
   //        or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Identifies the maximum number of active SQL statements
   // the driver can support for a connection. A statement is defined
   // as active if it has results pending, with the term results
   // meaning ows from a SELECT operation or rows affected by an 
   // INSERT, UPDATE, or DELETE operation (such as a row count),
   // or if it is in a "Need Data" state. This value can reflect a
   // limitation imposed by either the  driver or the data source
   //

   return 0;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxTableNameLength()
{
   // What's the maximum length of a table name?
   //
   //   Returns:
   //         max name length in bytes; a result of zero means that 
   //         there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // See also, <http://mysql.com/doc///L/e/Legal_names.html>

   return gMaxTableNameLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxTablesInSelect()
{
   // What's the maximum number of tables in a SELECT statement?
   //
   //   Returns:
   //         the maximum number of tables allowed in a SELECT 
   //         statement; a result of zero means that there is no 
   //         limit  or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   // ... more comments based on  "ODBC 3.5 Developer's Guide"
   //
   // Identifies the maximum number of tables allowed in the
   // FROM clause of a SELECT statement.
   //

   return 32; // ?? odbc driver returns 32
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetMaxUserNameLength()
{
   // What's the maximum length of a user name?
   //
   //   Returns:
   //         max user name length in bytes; a result of zero means 
   //        that there is no limit or the limit is not known
   //   Throws:
   //         TSQLException - if a database access error occurs

   return gNameLength;
}

//___________________________________________________________________
Int_t TSQLDatabaseMetaData::GetDefaultTransactionIsolation()
{
   // What's the database's default transaction isolation level? 
   // The values are defined in TSQLConnection.
   //
   //   Returns:
   //         the default isolation level
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         TSQLConnection

   return kTRANSACTION_NONE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsTransactions()
{
   // Are transactions supported? If not, invoking the method commit 
   // is a noop and the isolation level is TRANSACTION_NONE.
   //
   //   Returns:
   //         kTRUE if transactions are supported; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   
   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsTransactionIsolationLevel(
                                                         Int_t level )
{
   // Does this database support the given transaction isolation 
   // level?
   //
   //   Parameters:
   //         level - the values are defined in TSQLConnection
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         TSQLConnection

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsDataDefinitionAndDataManipulationTransactions()
{
   // Are both data definition and data manipulation statements 
   // within a transaction supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsDataManipulationTransactionsOnly()
{
   // Are only data manipulation statements within a transaction 
   // supported?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::DataDefinitionCausesTransactionCommit()
{
   // Does a data definition statement within a transaction force 
   // the transaction to commit?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::DataDefinitionIgnoredInTransactions()
{
   // Is a data definition statement within a transaction ignored?
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetProcedures(
                                 const TString& catalog,
                                 const TString& schemaPattern,
                                 const TString& procedureNamePattern )
{
   // Gets a description of the stored procedures available in 
   // a catalog. 
   //
   //   Only procedure descriptions matching the schema and procedure 
   // name criteria are returned. They are ordered by PROCEDURE_SCHEM,
   // and PROCEDURE_NAME. 
   //
   //   Each procedure description has the the following columns:
   // 
   //     1.PROCEDURE_CAT string => procedure catalog (may be null) 
   //
   //     2.PROCEDURE_SCHEM string => procedure schema (may be null) 
   //
   //     3.PROCEDURE_NAME string => procedure name 
   //
   //     4.reserved for future use 
   //     5.reserved for future use 
   //     6.reserved for future use 
   //
   //     7.REMARKS string => explanatory comment on the procedure 
   //
   //     8.PROCEDURE_TYPE short => kind of procedure:
   // 
   //               kProcedureResultUnknown - May return a result 
   //               kProcedureNoResult - Does not return a result 
   //               kProcedureReturnsResult - Returns a result 
   //
   //   Parameters:
   //         catalog -  a catalog name; 
   //                   ""   - retrieves those without a catalog; 
   //         schemaPattern - a schema name pattern; 
   //                         ""  - retrieves  those without a schema
   //         procedureNamePattern - a procedure name pattern
   //
   //   Returns:
   //         TSQLResultSet - each row is a procedure description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetSearchStringEscape()

   Throw( new TSQLException("MyODBC doesn't support this yet","08002",4000) );
   return 0;
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetProcedureColumns(
                                 const TString& catalog,
                                 const TString& schemaPattern,
                                 const TString& procedureNamePattern,
                                 const TString& columnNamePattern )
{
   // Gets a description of a catalog's stored procedure parameters 
   // and result columns. 
   //
   //   Only descriptions matching the schema, procedure and 
   // parameter name criteria are returned. They are ordered by 
   // PROCEDURE_SCHEM and PROCEDURE_NAME. Within this, the return
   // value, if any, is first. Next are the parameter descriptions 
   // in call order. The column descriptions follow in column number
   // order. 
   //
   //   Each row in the TSQLResultSet is a parameter description or 
   // column description with the following fields: 
   //
   //      1.PROCEDURE_CAT string => procedure catalog (may be null) 
   //
   //      2.PROCEDURE_SCHEM string => procedure schema (may be null) 
   //
   //      3.PROCEDURE_NAME string => procedure name 
   //
   //      4.COLUMN_NAME string => column/parameter name 
   //
   //      5.COLUMN_TYPE Short => kind of column/parameter:
   // 
   //          kProcedureColumnUnknown - nobody knows 
   //          kProcedureColumnIn - IN parameter 
   //          kProcedureColumnInOut - INOUT parameter 
   //          kProcedureColumnOut - OUT parameter 
   //          kProcedureColumnReturn - procedure return value 
   //          kProcedureColumnResult - result column in TSQLResultSet 
   //
   //     6.DATA_TYPE short => SQL type from TSQLTypes 
   //
   //     7.TYPE_NAME string => SQL type name, for a UDT type 
   //                            the type name is fully qualified 
   //
   //     8.PRECISION int => precision 
   //
   //     9.LENGTH int => length in bytes of data 
   //
   //     10.SCALE short => scale 
   //
   //     11.RADIX short => radix 
   //
   //     12.NULLABLE short => can it contain NULL? 
   //
   //          kProcedureNoNulls - does not allow NULL values 
   //          kProcedureNullable - allows NULL values 
   //          kProcedureNullableUnknown - nullability unknown 
   //
   //     13.REMARKS string => comment describing parameter/column 
   //
   //   Note: Some databases may not return the column descriptions 
   //          for a procedure. Additional columns beyond REMARKS 
   //          can be defined by the database.
   //
   //   Parameters:
   //         catalog -  a catalog name; 
   //                      ""    -  retrieves those without a catalog; 
   //         schemaPattern - a schema name pattern; 
   //                         ""  - retrieves those without a schema
   //         procedureNamePattern - a procedure name pattern
   //         columnNamePattern - a column name pattern
   //
   //   Returns:
   //         TSQLResultSet - each row describes a stored procedure 
   //                         parameter or column
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetSearchStringEscape()

   Throw( new TSQLException("MyODBC doesn't support this yet","08002",4000) );
   return 0;
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetTables(
                                    const TString& catalog,
                                    const TString& schemaPattern,
                                    const TString& tableNamePattern,
                                    const TString& types )
{
   // Gets a description of tables available in a catalog. 
   //
   //   Only table descriptions matching the catalog, schema, 
   // table name and type criteria are returned. They are ordered 
   // by TABLE_TYPE, TABLE_SCHEM and TABLE_NAME. 
   //
   //   Each table description has the following columns:
   // 
   //      1.TABLE_CAT string => table catalog (may be null) 
   //
   //      2.TABLE_SCHEM string => table schema (may be null) 
   //
   //      3.TABLE_NAME string => table name 
   //
   //      4.TABLE_TYPE string => table type. Typical types are:
   // 
   //                   "TABLE", "VIEW", "SYSTEM TABLE", 
   //                   "GLOBAL TEMPORARY", "LOCAL TEMPORARY", 
   //                   "ALIAS", "SYNONYM". 
   //
   //      5.REMARKS string => explanatory comment on the table 
   //
   // Note: Some databases may not return information for all tables.
   //
   //   Parameters:
   //          catalog -   a catalog name; 
   //                      ""   - retrieves those without a catalog; 
   //          schemaPattern - a schema name pattern; 
   //                      ""   - retrieves those without a schema
   //          tableNamePattern - a table name pattern
   //          types -  a list of table types to include; 
   //                   null returns all types
   //
   //   Returns:
   //         TSQLResultSet - each row is a table description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //          GetSearchStringEscape()

   static MYSQL_FIELD SQLTABLES_fields[] = {
      {"TABLE_CAT","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0},
      {"TABLE_SHEM","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0},
      {"TABLE_NAME","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,NAME_LEN,0},
      {"TABLE_TYPE","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,5,0},
      {"REMARKS","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,11}};

   const Int_t SQLTABLES_FIELDS = array_elements(SQLTABLES_values);

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;
   TString db = con->fURL->GetDatabase();   

   if( !catalog.IsNull() && schemaPattern.IsNull() &&  tableNamePattern.IsNull() ) {
      gResult->fMYSQL_RES = mysql_list_dbs(con->fMYSQL,catalog);
   
      if(!gResult->fMYSQL_RES) {
         Throw( new TSQLException("Could not read database names","S1000",0) );
         return 0;
      }

      gResult->fMYSQL_RES->fields = SQLTABLES_fields;
      gResult->fMYSQL_RES->field_count = SQLTABLES_FIELDS;
      gResult->fMYSQL_RES->current_field = 0; 

      return new TSQLResultSet(0,gResult);   
   }

   if( catalog.IsNull() && schemaPattern == "%" &&  tableNamePattern.IsNull() ) {
      gResult->fMYSQL_RES = (MYSQL_RES*) my_malloc(sizeof(MYSQL_RES),MYF(MY_ZEROFILL));
      gResult->fMYSQL_RES->row_count = 1;
      gResult->fMYSQL_RES->fields = SQLTABLES_fields;
      gResult->fMYSQL_RES->field_count = SQLTABLES_FIELDS;
      gResult->fMYSQL_RES->current_field = 0; 

      return new TSQLResultSet(0,gResult);   
   }

   if( catalog.IsNull() && schemaPattern.IsNull() && 
      tableNamePattern.IsNull() && types=="%" ) {
      gResult->fMYSQL_RES = (MYSQL_RES*) my_malloc(sizeof(MYSQL_RES),MYF(MY_ZEROFILL));
      gResult->fMYSQL_RES->row_count = 1;
      gResult->fMYSQL_RES->fields = SQLTABLES_fields;
      gResult->fMYSQL_RES->field_count = SQLTABLES_FIELDS;
      gResult->fMYSQL_RES->current_field = 0; 

      return new TSQLResultSet(0,gResult); 
   } 

   // Return empty set if unknown tbale type or if catalog or schemaPattern is used 

   if( (!types.IsNull() && !types.Contains("TABLE") ) ||
       (!catalog.IsNull() && catalog != "%" && catalog != db) ||
       (!schemaPattern.IsNull() && schemaPattern != "%" && catalog != db) ) {
      gResult->fMYSQL_RES = (MYSQL_RES*) my_malloc(sizeof(MYSQL_RES),MYF(MY_ZEROFILL));
      gResult->fMYSQL_RES->row_count = 0;
      gResult->fMYSQL_RES->fields = SQLTABLES_fields;
      gResult->fMYSQL_RES->field_count = SQLTABLES_FIELDS;
      gResult->fMYSQL_RES->current_field = 0; 

      return new TSQLResultSet(0,gResult); 
   }

   // this was a normal request for tables.  Return matching tables 
   
   gResult->fMYSQL_RES = mysql_list_tables(con->fMYSQL,tableNamePattern);
   
   if(!gResult->fMYSQL_RES) {
      Throw( new TSQLException("Could not read table names","S1000",0) );
      return 0;
   }
  
   gResult->fMYSQL_RES->fields = SQLTABLES_fields;
   gResult->fMYSQL_RES->field_count = SQLTABLES_FIELDS;
   gResult->fMYSQL_RES->current_field = 0; 

   return new TSQLResultSet(0,gResult);
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetSchemas()
{
   // Gets the schema names available in this database. The results 
   // are ordered by schema name. 
   //
   //   The schema column is: 
   //      1.TABLE_SCHEM string => schema name 
   //
   //   Returns:
   //        TSQLResultSet -  each row has a single string column 
   //                         that is a schema name
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   //  Note: result the same as GetTables("%","","","");

   static MYSQL_FIELD SCHEMA_fields[] = {
      {"TABLE_SHEM","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0}};

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   gResult->fMYSQL_RES = (MYSQL_RES*) my_malloc(sizeof(MYSQL_RES),MYF(MY_ZEROFILL));
   gResult->fMYSQL_RES->row_count = 1;
   gResult->fMYSQL_RES->fields = SCHEMA_fields;
   gResult->fMYSQL_RES->field_count = 1;
   gResult->fMYSQL_RES->current_field = 0; 

   return new TSQLResultSet(0,gResult);  
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetCatalogs()
{
   // Gets the catalog names available in this database. The results 
   // are ordered by catalog name. 
   //
   //   The catalog column is: 
   //      1.TABLE_CAT string => catalog name 
   //
   //   Returns:
   //         TSQLResultSet - each row has a single string column 
   //                         that is a catalog name
   //   Throws:
   //         TSQLException - if a database access error occurs

   static MYSQL_FIELD CATALOG_fields[] = {
      {"TABLE_CAT","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0}};

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   gResult->fMYSQL_RES = mysql_list_dbs(con->fMYSQL,"%");
   
   if(!gResult->fMYSQL_RES) {
      Throw( new TSQLException("Could not read database names","S1000",0) );
      return 0;
   }

   gResult->fMYSQL_RES->fields = CATALOG_fields;
   gResult->fMYSQL_RES->field_count = 1;
   gResult->fMYSQL_RES->current_field = 0; 

   return new TSQLResultSet(0,gResult);   
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetTableTypes()
{
   // Gets the table types available in this database. The results 
   // are ordered by table type. 
   //
   //   The table type is:
   //      1.TABLE_TYPE string => table type. Typical types are:
   // 
   //                   "TABLE", "VIEW", "SYSTEM TABLE", 
   //                   "GLOBAL TEMPORARY","LOCAL TEMPORARY", 
   //                   "ALIAS", "SYNONYM". 
   //
   //   Returns:
   //         TSQLResultSet - each row has a single string column 
   //                         that is a table type
   //   Throws:
   //         TSQLException - if a database access error occurs

   static MYSQL_FIELD TTYPE_fields[] = {
      {"TABLE_TYPE","Catalog",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0}};

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp; 

   gResult->fMYSQL_RES = (MYSQL_RES*) my_malloc(sizeof(MYSQL_RES),MYF(MY_ZEROFILL));
   gResult->fMYSQL_RES->row_count = 1;
   gResult->fMYSQL_RES->fields = TTYPE_fields;
   gResult->fMYSQL_RES->field_count = 1;
   gResult->fMYSQL_RES->current_field = 0; 

   return new TSQLResultSet(0,gResult);
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetColumns(
                                    const TString& catalog,
                                    const TString& schemaPattern,
                                    const TString& tableNamePattern,
                                    const TString& columnNamePattern )
{
   // Gets a description of table columns available in the specified 
   // catalog. 
   //
   //   Only column descriptions matching the catalog, schema, 
   // table and column name criteria are returned. They are ordered 
   // by TABLE_SCHEM, TABLE_NAME and ORDINAL_POSITION. 
   //
   //   Each column description has the following columns: 
   //
   //      1.TABLE_CAT string => table catalog (may be null) 
   //
   //      2.TABLE_SCHEM string => table schema (may be null) 
   //
   //      3.TABLE_NAME string => table name 
   //
   //      4.COLUMN_NAME string => column name 
   //
   //      5.DATA_TYPE short => SQL type from TSQLTypes 
   //
   //      6.TYPE_NAME string => Data source dependent type name, 
   //                   for a UDT the type name is fully qualified 
   //
   //      7.COLUMN_SIZE int => column size. For char or date types 
   //                   this is the maximum number of characters, 
   //                   for numeric or decimal types this is precision. 
   //
   //      8.BUFFER_LENGTH is not used. 
   //
   //      9.DECIMAL_DIGITS int => the number of fractional digits 
   //
   //     10.NUM_PREC_RADIX int => Radix (typically either 10 or 2) 
   //
   //     11.NULLABLE int => is NULL allowed? 
   //
   //               kColumnNoNulls - might not allow NULL values 
   //               kColumnNullable - definitely allows NULL values 
   //               kColumnNullableUnknown - nullability unknown 
   //
   //     12.REMARKS string => comment describing column (may be null) 
   //
   //     13.COLUMN_DEF string => default value (may be null) 
   //
   //     14.SQL_DATA_TYPE int => unused 
   //
   //     15.SQL_DATETIME_SUB int => unused 
   //
   //     16.CHAR_OCTET_LENGTH int => for char types the maximum 
   //                                  number of bytes in the column 
   //
   //     17.ORDINAL_POSITION int => index of column in 
   //                                  table  (starting at 1) 
   //
   //     18.IS_NULLABLE string => "NO" means column definitely 
   //                                  does not allow NULL values;
   //                             "YES" means the column might allow 
   //                                  NULL values. 
   //                             An empty string means nobody knows. 
   //
   //   Parameters:
   //         catalog - a catalog name; 
   //                   ""   -  retrieves those without a catalog; 
   //         schemaPattern - a schema name pattern; 
   //                         ""  - retrieves those without a schema
   //         tableNamePattern - a table name pattern
   //         columnNamePattern - a column name pattern
   //
   //   Returns:
   //         TSQLResultSet - each row is a column description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetSearchStringEscape()

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   return new TSQLResultSet(0,gResult);
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetColumnPrivileges(
                                     const TString& catalog,
                                     const TString& schema,
                                     const TString& table,
                                     const TString& columnNamePattern )
{
   // Gets a description of the access rights for a table's columns. 
   //
   //   Only privileges matching the column name criteria are 
   // returned. They are ordered by COLUMN_NAME and PRIVILEGE. 
   //
   //   Each privilige description has the following columns:
   // 
   //      1.TABLE_CAT string => table catalog (may be null) 
   //
   //      2.TABLE_SCHEM string => table schema (may be null) 
   //
   //      3.TABLE_NAME string => table name 
   //
   //      4.COLUMN_NAME string => column name 
   //
   //      5.GRANTOR => grantor of access (may be null) 
   //
   //      6.GRANTEE string => grantee of access 
   //
   //      7.PRIVILEGE string => name of access (SELECT, INSERT, 
   //                            UPDATE, REFRENCES, ...)
   // 
   //      8.IS_GRANTABLE string => "YES" if grantee is permitted 
   //                                     to grant to others; 
   //                               "NO"  if not; 
   //                               null  if unknown 
   //
   //   Parameters:
   //         catalog - a catalog name; 
   //                   ""   - retrieves those without a catalog; 
   //         schema - a schema name; 
   //                   ""  - retrieves those without a schema
   //         table - a table name
   //         columnNamePattern - a column name pattern
   //
   //   Returns:
   //         TSQLResultSet - each row is a column privilege 
   //                         description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetSearchStringEscape()

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   Throw( new TSQLException("MyODBC doesn't support this yet","08002",4000) );
   return 0;
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetTablePrivileges(
                                    const TString& catalog,
                                    const TString& schemaPattern,
                                    const TString& tableNamePattern )
{
   // Gets a description of the access rights for each table 
   // available in a catalog. Note that a table privilege applies 
   // to one or more columns in the table. It would be wrong to 
   // assume that this priviledge applies to all columns (this may 
   // be true for some systems but is not true for all.) 
   //
   //   Only privileges matching the schema and table name criteria 
   // are returned. They are ordered by TABLE_SCHEM, TABLE_NAME, 
   // and PRIVILEGE. 
   //
   //   Each privilige description has the following columns:
   // 
   //      1.TABLE_CAT string => table catalog (may be null) 
   //
   //      2.TABLE_SCHEM string => table schema (may be null) 
   //
   //      3.TABLE_NAME string => table name 
   //
   //      4.GRANTOR => grantor of access (may be null) 
   //
   //      5.GRANTEE string => grantee of access 
   //
   //      6.PRIVILEGE string => name of access (SELECT, INSERT, 
   //                            UPDATE, REFRENCES, ...) 
   //
   //      7.IS_GRANTABLE string => "YES" -  if grantee is permitted 
   //                                        to grant to others; 
   //                               "NO"  -  if not; 
   //                               null  -  if unknown 
   //
   //   Parameters:
   //         catalog - a catalog name; 
   //                   ""   - retrieves those without a catalog; 
   //         schemaPattern - a schema name pattern; 
   //                   ""  - retrieves those without a schema
   //         tableNamePattern - a table name pattern
   //
   //   Returns:
   //         TSQLResultSet - each row is a table privilege 
   //                         description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetSearchStringEscape()

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   Throw( new TSQLException("MyODBC doesn't support this yet","08002",4000) );
   return 0;
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetBestRowIdentifier(
                                             const TString& catalog,
                                             const TString& schema,
                                             const TString& table,
                                             Int_t scope,
                                             Bool_t nullable )
{
   // Gets a description of a table's optimal set of columns that 
   // uniquely identifies a row. They are ordered by SCOPE. 
   //
   //   Each column description has the following columns: 
   //
   //      1.SCOPE short => actual scope of result
   // 
   //          kBestRowTemporary - very temporary, while using row    
   //          kBestRowTransaction - valid for remainder of current 
   //                               transaction 
   //          kBestRowSession - valid for remainder of current session 
   //
   //      2.COLUMN_NAME string => column name 
   //
   //      3.DATA_TYPE short => SQL data type from TSQLTypes 
   //
   //      4.TYPE_NAME string => Data source dependent type name, 
   //                            for a UDT the type name is fully 
   //                            qualified 
   //
   //      5.COLUMN_SIZE int => precision 
   //
   //      6.BUFFER_LENGTH int => not used 
   //
   //      7.DECIMAL_DIGITS short => scale 
   //
   //      8.PSEUDO_COLUMN short => is this a pseudo column like 
   //                               an Oracle ROWID
   //  
   //          kBestRowUnknown - may or may not  be pseudo column  
   //          kBestRowNotPseudo - is NOT a  pseudo column 
   //          kBestRowPseudo - is a pseudo column 
   //
   //   Parameters:
   //         catalog - a catalog name; 
   //                   ""  -  retrieves those without a catalog; 
   //         schema - a schema name; 
   //                  ""  - retrieves those without a schema
   //         table - a table name
   //         scope - the scope of interest; use same values as SCOPE
   //         nullable - include columns that are nullable?
   //
   //   Returns:
   //         TSQLResultSet - each row is a column description
   //   Throws:
   //         TSQLException - if a database access error occurs

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   return new TSQLResultSet(0,gResult);
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetVersionColumns(
                                             const TString& catalog,
                                             const TString& schema,
                                             const TString& table )
{
   // Gets a description of a table's columns that are automatically 
   // updated when any value in a row is updated. They are unordered. 
   //
   //   Each column description has the following columns: 
   //
   //      1.SCOPE short => is not used 
   //
   //      2.COLUMN_NAME string => column name 
   //
   //      3.DATA_TYPE short => SQL data type from TSQLTypes 
   //
   //      4.TYPE_NAME string => Data source dependent type name 
   //
   //      5.COLUMN_SIZE int => precision 
   //
   //      6.BUFFER_LENGTH int => length of column value in bytes 
   //
   //      7.DECIMAL_DIGITS short => scale 
   //
   //      8.PSEUDO_COLUMN short => is this a pseudo column like 
   //                               an Oracle ROWID
   //
   //          kVersionColumnUnknown - may or may not be pseudo column 
   //          kVersionColumnNotPseudo - is NOT a pseudo column 
   //          kVersionColumnPseudo - is a pseudo column 
   //
   //   Parameters:
   //          catalog - a catalog name; 
   //                   "" -     retrieves those without a catalog; 
   //          schema - a schema name; 
   //                   "" retrieves those without a schema
   //          table - a table name
   //
   //   Returns:
   //         TSQLResultSet - each row is a column description
   //   Throws:
   //         TSQLException - if a database access error occurs

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   Throw( new TSQLException("MyODBC doesn't support this yet","08002",4000) );
   return 0;
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetPrimaryKeys(
                                             const TString& catalog,
                                             const TString& schema,
                                             const TString& table )
{
   // Gets a description of a table's primary key columns. 
   // They are ordered by COLUMN_NAME.
   //
   //  Each primary key column description has the following columns:
   // 
   //      1.TABLE_CAT string => table catalog (may be null) 
   //
   //      2.TABLE_SCHEM string => table schema (may be null) 
   //
   //      3.TABLE_NAME string => table name 
   //
   //      4.COLUMN_NAME string => column name 
   //
   //      5.KEY_SEQ short => sequence number within primary key 
   //
   //      6.PK_NAME string => primary key name (may be null) 
   //
   //   Parameters:
   //         catalog - a catalog name; 
   //                   "" -   retrieves those without a catalog; 
   //         schema - a schema name; 
   //                   "" - retrieves those without  a schema
   //         table - a table name
   //
   //   Returns:
   //         TSQLResultSet - each row is a primary key column 
   //                         description
   //   Throws:
   //         TSQLException - if a database access error occurs

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;
 
   return new TSQLResultSet(0,gResult);
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetImportedKeys(
                                             const TString& catalog,
                                             const TString& schema,
                                             const TString& table )
{
   // Gets a description of the primary key columns that are 
   // referenced by a table's foreign key columns (the primary keys
   // imported by a table). They are ordered by PKTABLE_CAT,
   // PKTABLE_SCHEM, PKTABLE_NAME, and KEY_SEQ. 
   //
   //  Each primary key column description has the following columns: 
   //
   //    1.PKTABLE_CAT string => primary key table catalog being 
   //                            imported (may be null)
   //
   //    2.PKTABLE_SCHEM string => primary key table schema being
   //                             imported (may be null) 
   //
   //    3.PKTABLE_NAME string => primary key table name being 
   //                             imported 
   //
   //    4.PKCOLUMN_NAME string => primary key column name being    
   //                             imported 
   //
   //    5.FKTABLE_CAT string => foreign key table catalog 
   //                            (may be null) 
   //
   //    6.FKTABLE_SCHEM string => foreign key table schema 
   //                               (may be null) 
   //
   //    7.FKTABLE_NAME string => foreign key table name 
   //
   //    8.FKCOLUMN_NAME string => foreign key column name 
   //
   //    9.KEY_SEQ short => sequence number within foreign key 
   //
   //    10.UPDATE_RULE short =>  What happens to foreign key when 
   //                             primary is updated: 
   //
   //        kImportedNoAction -  do not allow update of primary key 
   //                             if it has been imported 
   //        kImportedKeyCascade -  change imported key to agree 
   //                               with primary key update 
   //        kImportedKeySetNull -  change imported key to NULL if 
   //                               its primary key has been updated 
   //        kImportedKeySetDefault - change imported key to default
   //                                  values if its primary key has 
   //                                  been updated 
   //        kImportedKeyRestrict - same as kImportedKeyNoAction 
   //
   //     11.DELETE_RULE short => What happens to the foreign key 
   //                             when primary is deleted:
   //   
   //        kImportedKeyNoAction - do not allow delete of primary
   //                               key if it has been imported 
   //        kImportedKeyCascade -  delete rows that import a 
   //                               deleted key 
   //        kImportedKeySetNull -  change imported key to NULL 
   //                               if its primary key has been deleted 
   //        kImportedKeyRestrict - same as kImportedKeyNoAction 
   //        kImportedKeySetDefault - change imported key to default
   //                               if its primary key has been deleted
   // 
   //     12.FK_NAME string => foreign key name (may be null) 
   //
   //     13.PK_NAME string => primary key name (may be null) 
   //
   //     14.DEFERRABILITY short => can the evaluation of foreign key
   //                            constraints be deferred until commit
   // 
   //       kImportedKeyInitiallyDeferred - see SQL92 for definition 
   //       kImportedKeyInitiallyImmediate - see SQL92 for definition 
   //       kImportedKeyNotDeferrable - see SQL92 for definition 
   //
   //   Parameters:
   //       catalog - a catalog name; 
   //                   ""    - retrieves those without a catalog; 
   //       schema - a schema name; 
   //                   ""   - retrieves those without a schema
   //       table - a table name
   //
   //   Returns:
   //       TSQLResultSet - each row is a primary key column description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetExportedKeys(const TString&,
   //                         const TString&,
   //                         const TString&)

   return GetCrossReference("","","",catalog,schema,table);
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetExportedKeys( 
                                             const TString& catalog,
                                             const TString& schema,
                                             const TString& table )
{
   // Gets a description of the foreign key columns that reference 
   // a table's primary key columns (the foreign keys exported by 
   // a table). They are ordered by FKTABLE_CAT, FKTABLE_SCHEM, 
   // FKTABLE_NAME, and KEY_SEQ. 
   //
   // Each foreign key column description has the following columns: 
   //
   //    1.PKTABLE_CAT string => primary key table catalog (may be null) 
   //
   //    2.PKTABLE_SCHEM string => primary key  table schema (may be null) 
   //
   //    3.PKTABLE_NAME string => primary key table name 
   //
   //    4.PKCOLUMN_NAME string => primary key column name 
   //
   //    5.FKTABLE_CAT string => foreign key table catalog (may be 
   //                            null) being exported (may be null) 
   //
   //    6.FKTABLE_SCHEM string => foreign key table schema (may be 
   //                              null) being exported (may be null) 
   //    7.FKTABLE_NAME string => foreign key table name being  exported 
   //
   //    8.FKCOLUMN_NAME string => foreign key column name being  exported 
   //
   //    9.KEY_SEQ short => sequence number within foreign key 
   //
   //    10.UPDATE_RULE short => What happens to foreign key when 
   //                            primary is updated: 
   //
   //          kImportedNoAction -  do not allow update of primary 
   //                               key if it has been imported 
   //          kImportedKeyCascade - change imported key to agree 
   //                               with primary key update 
   //          kImportedKeySetNull - change imported key to NULL 
   //                               if its primary key has been updated 
   //          kImportedKeySetDefault - change imported key to default 
   //                               values if its primary key has been 
   //                               updated 
   //          kImportedKeyRestrict - same as kImportedKeyNoAction 
   //
   //    11.DELETE_RULE short => What happens to the foreign key 
   //                            when primary is deleted:
   // 
   //          kImportedKeyNoAction -  do not allow delete of primary
   //                                  key if it has been imported 
   //          kImportedKeyCascade -   delete rows that import a 
   //                                  deleted key 
   //          kImportedKeySetNull -   change imported key to NULL if 
   //                                  its primary key has been deleted 
   //          kImportedKeyRestrict -  same as kImportedKeyNoAction 
   //          kImportedKeySetDefault - change imported key to default
   //                                  if its primary key has been 
   //                                  deleted 
   //
   //    12.FK_NAME string => foreign key name (may be null) 
   //
   //    13.PK_NAME string => primary key name (may be null) 
   //
   //    14.DEFERRABILITY short => can the evaluation of foreign key 
   //                            constraints be deferred until commit 
   //
   //          kImportedKeyInitiallyDeferred - see SQL92 for definition 
   //          kImportedKeyInitiallyImmediate - see SQL92 for definition 
   //          kImportedKeyNotDeferrable - see SQL92 for definition 
   //
   //   Parameters:
   //       catalog - a catalog name; 
   //                   ""    -  retrieves those without a  catalog; 
   //       schema - a schema name; 
   //                   ""    -  retrieves those without a schema
   //       table - a table name
   //
   //   Returns:
   //       TSQLResultSet -   each row is a foreign key column 
   //                         description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetImportedKeys( const TString&,
   //                          const TString&,
   //                          const TString& )

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;
  
   return GetCrossReference(catalog,schema,table,"","","");
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetCrossReference(
                                   const TString& primaryCatalog,
                                   const TString& primarySchema,
                                   const TString& primaryTable,
                                   const TString& foreignCatalog,
                                   const TString& foreignSchema,
                                   const TString& foreignTable )
{
   // Gets a description of the foreign key columns in the foreign 
   // key table that reference the primary key columns of the primary 
   // key table (describe how one table imports another's key.) 
   // This should normally return a single foreign key/primary key 
   // pair (most tables only import a foreign key from a table once.) 
   // They are ordered by FKTABLE_CAT, FKTABLE_SCHEM, FKTABLE_NAME, 
   // and KEY_SEQ. 
   //
   // Each foreign key column description has the following columns: 
   //
   //      1.PKTABLE_CAT string => primary key table catalog (may be null)
   // 
   //      2.PKTABLE_SCHEM string => primary key table schema (may be null)
   // 
   //      3.PKTABLE_NAME string => primary key table name 
   //
   //      4.PKCOLUMN_NAME string => primary key column name 
   //
   //      5.FKTABLE_CAT string =>  foreign key table catalog (may be null) 
   //                                being exported (may be null) 
   //
   //      6.FKTABLE_SCHEM string =>  foreign key table schema (may be null) 
   //                                 being exported (may be null) 
   //
   //      7.FKTABLE_NAME string =>  foreign key table name being exported 
   //
   //      8.FKCOLUMN_NAME string =>   foreign key column name 
   //                                  being exported 
   //
   //      9.KEY_SEQ short => sequence number within foreign key 
   //
   //      10.UPDATE_RULE short => What happens to foreign key when 
   //                            primary is updated: 
   //
   //          kImportedNoAction - do not allow update of primary key 
   //                               if it has been imported 
   //          kImportedKeyCascade - change imported key to agree 
   //                               with primary key update 
   //          kImportedKeySetNull - change imported key to NULL 
   //                               if its primary key has been updated 
   //          kImportedKeySetDefault - change imported key to 
   //                               default values if its primary key 
   //                               has been  updated 
   //          kImportedKeyRestrict - same as kImportedKeyNoAction 
   //
   //      11.DELETE_RULE short => What happens to the foreign key 
   //                               when primary is deleted
   // 
   //          kImportedKeyNoAction - do not allow delete of primary 
   //                               key if it has been imported 
   //          kImportedKeyCascade - delete rows that import 
   //                               a deleted  key 
   //          kImportedKeySetNull - change imported key to NULL if 
   //                               its primary key has been deleted 
   //          kImportedKeyRestrict - same as kImportedKeyNoAction 
   //          kImportedKeySetDefault - change imported key to 
   //                                  default if its primary key has 
   //                                  been deleted 
   //
   //     12.FK_NAME string => foreign key name (may be null) 
   //
   //     13.PK_NAME string => primary key name (may be null) 
   //
   //     14.DEFERRABILITY short => can the evaluation of foreign 
   //                               key constraints be deferred until
   //                               commit
   //
   //          kImportedKeyInitiallyDeferred - see SQL92 for definition 
   //          kImportedKeyInitiallyImmediate - see SQL92 for definition 
   //          kImportedKeyNotDeferrable - see SQL92 for definition 
   //
   //   Parameters:
   //         primaryCatalog -   a catalog name; 
   //                            "" - retrieves those without a catalog; 
   //         primarySchema - a schema name; 
   //                            "" - retrieves those  without a schema
   //         primaryTable - the table name that exports the key
   //         foreignCatalog - a catalog name; 
   //                            ""    - retrieves those without a catalog; 
   //         foreignSchema - a schema name; 
   //                            ""    - retrieves those without a schema
   //         foreignTable - the table name that imports the key
   //
   //   Returns:
   //         TSQLResultSet - each row is a foreign key column 
   //                         description
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         GetImportedKeys( const TString&,
   //                          const TString&,
   //                          const TString& )

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetTypeInfo()
{
   // Gets a description of all the standard SQL types supported 
   // by this database. They are ordered by DATA_TYPE and then by 
   // how closely the data type maps to the corresponding SQL type. 
   //
   //   Each type description has the following columns: 
   //
   //      1.TYPE_NAME string => Type name 
   //
   //      2.DATA_TYPE short => SQL data type from TSQLTypes 
   //
   //      3.PRECISION int => maximum precision 
   //
   //      4.LITERAL_PREFIX string => prefix used to quote a 
   //                                  literal  (may be null) 
   //
   //      5.LITERAL_SUFFIX string => suffix used to quote a 
   //                                  literal (may be null) 
   //
   //      6.CREATE_PARAMS string => parameters used in creating 
   //                                  the type (may be null) 
   //
   //      7.NULLABLE short => can you use NULL for this type?
   //
   //               kTypeNoNulls - does not allow NULL values 
   //               kTypeNullable - allows NULL values 
   //               kTypeNullableUnknown - nullability unknown 
   //
   //      8.CASE_SENSITIVE boolean=> is it case sensitive? 
   //
   //      9.SEARCHABLE short => can you use "WHERE" based on 
   //                            this type:
   // 
   //             kTypePredNone - No support 
   //             kTypePredChar - Only supported with WHERE .. LIKE 
   //             kTypePredBasic - Supported except for WHERE .. LIKE 
   //             kTypeSearchable - Supported for all WHERE .. 
   //
   //     10.UNSIGNED_ATTRIBUTE => is it unsigned? 
   //
   //     11.FIXED_PREC_SCALE => can it be a money value? 
   //
   //     12.AUTO_INCREMENT =>   can it be used for an auto-increment 
   //                            value? 
   //
   //     13.LOCAL_TYPE_NAME string => localized version of type 
   //                                  name (may be null)
   // 
   //     14.MINIMUM_SCALE short => minimum scale supported 
   //
   //     15.MAXIMUM_SCALE short => maximum scale supported 
   //
   //     16.SQL_DATA_TYPE int => unused 
   //
   //     17.SQL_DATETIME_SUB int => unused 
   //
   //     18.NUM_PREC_RADIX int => usually 2 or 10 
   //
   //   Returns:
   //         TSQLResultSet - each row is a SQL type description
   //   Throws:
   //         TSQLException - if a database access error occurs

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;
 
   return new TSQLResultSet(0,gResult);
}

//___________________________________________________________________
TSQLResultSet* TSQLDatabaseMetaData::GetIndexInfo(
                                             const TString& catalog,
                                             const TString& schema,
                                             const TString& table,
                                             Bool_t  unique,
                                             Bool_t approximate )
{
   // Gets a description of a table's indices and statistics. 
   // They are ordered by NON_UNIQUE, TYPE, INDEX_NAME, 
   // and ORDINAL_POSITION. 
   //
   //   Each index column description has the following columns: 
   //
   //      1.TABLE_CAT string => table catalog (may be null) 
   //
   //      2.TABLE_SCHEM string => table schema (may be null) 
   //
   //      3.TABLE_NAME string => table name 
   //
   //      4.NON_UNIQUE => Can index values be non-unique? 
   //                   kFALSE when TYPE is kTableIndexStatistic 
   //
   //      5.INDEX_QUALIFIER string => index catalog (may be null); 
   //                   null when TYPE is kTableIndexStatistic 
   //
   //      6.INDEX_NAME string => index name;  null when TYPE 
   //                             is kTableIndexStatistic 
   //
   //      7.TYPE short => index type: 
   //
   //          kTableIndexStatistic - this identifies table 
   //                                  statistics that are returned
   //                                  in conjuction with a table's 
   //                                  index descriptions 
   //
   //          kTableIndexClustered  - this is a clustered index  
   //          kTableIndexHashed -  this is a hashed index  
   //          kTableIndexOther -  this is some other style of index 
   //
   //      8.ORDINAL_POSITION short => column sequence number within 
   //                                  index; zero when TYPE is 
   //                                  kTableIndexStatistic 
   //
   //      9.COLUMN_NAME string => column name; null when TYPE is 
   //                                  kTableIndexStatistic
   // 
   //     10.ASC_OR_DESC string => column sort sequence,
   // 
   //                         "A" => ascending, 
   //                         "D" => descending, may be null if 
   //                                sort sequence is not supported; 
   //                         null => when TYPE is kTableIndexStatistic
   // 
   //     11.CARDINALITY int => When TYPE is kTableIndexStatistic, 
   //                         then this is the number of rows in 
   //                         the table;  otherwise, it is the number 
   //                         of unique values in the index.
   // 
   //     12.PAGES int => When TYPE is kTableIndexStatisic then 
   //                      this is the number of pages used for 
   //                      the table,  otherwise it is the number 
   //                      of pages used for the  current index.
   // 
   //     13.FILTER_CONDITION string => Filter condition, if any
   //                                  (may be null) 
   //
   //   Parameters:
   //         catalog -    a catalog name; 
   //                         ""    -  retrieves those without  a catalog; 
   //          schema -    a schema name;  
   //                         ""    -  retrieves those without a schema 
   //          table -     a table  name 
   //          unique -    when kTRUE, return only indices for unique 
   //                      values; when kFALSE, return indices 
   //                      regardless  of whether unique or not 
   //          approximate -  when kTRUE,  result is allowed to reflect 
   //                         approximate or out of  data values; 
   //                         when kFALSE, results are requested to  
   //                         be accurate
   //
   //   Returns:
   //         TSQLResultSet - each row is an index column description
   //   Throws:
   //         TSQLException - if a database access error occurs

   gResult->Close();
   MySQLConnectionPrivate* con = (MySQLConnectionPrivate*)fConnection->fImp;

   static MYSQL_FIELD SQLSTAT_fields[] = {
      {"TABLE_CAT","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0},
      {"TABLE_SCHEM","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0},
      {"TABLE_NAME","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,NAME_LEN,NOT_NULL_FLAG},
      {"NON_UNIQUE","MySQL_Stat",NULL,FIELD_TYPE_SHORT,1,1,NOT_NULL_FLAG},
      {"NDEX_QUALIFIER","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,0},
      {"INDEX_NAME","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,NAME_LEN},
      {"TYPE","MySQL_Stat",NULL,FIELD_TYPE_SHORT,1,1,NOT_NULL_FLAG},
      {"ORDINAL_POSITION","MySQL_Stat",NULL,FIELD_TYPE_SHORT,1,2,NOT_NULL_FLAG},
      {"COLUMN_NAME","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,NAME_LEN,NAME_LEN,NOT_NULL_FLAG},
      {"ASC_OR_DESC","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,1,1},
      {"CARDINALITY","MySQL_Stat",NULL,FIELD_TYPE_LONG,11,11},
      {"PAGES","MySQL_Stat",NULL,FIELD_TYPE_LONG,9,9},
      {"FILTER_CONDITION","MySQL_Stat",NULL,FIELD_TYPE_VAR_STRING,10,10},
   };

   const Int_t SQLSTAT_FIELDS=array_elements(SQLSTAT_fields);

   return new TSQLResultSet(0,gResult);
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsResultSetType( Int_t type )
{
   // Does the database support the given result set type?
   //
   //   Parameters:
   //         type - defined in TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         TSQLResultSet

   return ( (type==kTYPE_SCROLL_INSENSITIVE) || (type==kTYPE_FORWARD_ONLY) );
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsResultSetConcurrency( Int_t type,
                                                   Int_t concurrency )
{
   // Does the database support the concurrency type in combination 
   // with the given result set type?
   //
   //   Parameters:
   //         type - defined in TSQLResultSet
   //         concurrency - type defined in TSQLResultSet
   //
   //   Returns:
   //         kTRUE if so; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //   See Also: 
   //         TSQLConnection TSQLResultSet

   return (type == kTYPE_SCROLL_SENSITIVE &&
           concurrency == kCONCUR_READ_ONLY);
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::OwnUpdatesAreVisible( Int_t type )
{
   // Indicates whether a result set's own updates are visible.
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if updates are visible for the result set type; 
   //         kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   //    see also TSQLResultSet
   
   Bool_t return_value = kFALSE; 
   return return_value;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::OwnDeletesAreVisible( Int_t type )
{
   // Indicates whether a result set's own deletes are visible.
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //    Returns:
   //         kTRUE if deletes are visible for the result set type; 
   //         kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   //    see also TSQLResultSet

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::OwnInsertsAreVisible( Int_t type )
{
   // Indicates whether a result set's own inserts are visible.
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if inserts are visible for the result set type; 
   //         kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs
   //
   //    see also TSQLResultSet

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::OthersUpdatesAreVisible( Int_t type )
{
   // Indicates whether updates made by others are visible.
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX. 
   //
   //   Returns:
   //         kTRUE if updates made by others are visible for 
   //         the result set type; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

  return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::OthersDeletesAreVisible( Int_t type )
{
   // Indicates whether deletes made by others are visible.
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if deletes made by others are visible for the 
   //         result set type; kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

   return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::OthersInsertsAreVisible( Int_t type )
{
   // Indicates whether inserts made by others are visible.
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if updates are visible for the result set type
   //   Throws:
   //         TSQLException - if a database access error occurs

  return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::UpdatesAreDetected( Int_t type )
{
   // Indicates whether or not a visible row update  can be detected 
   // by calling the method TSQLResultSet::RowUpdated().
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if changes are detected by the result set type; 
   //          kFALSE otherwise
   //   Throws:
   //         TSQLException - if a database access error occurs

  return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::DeletesAreDetected( Int_t type )
{
   // Indicates whether or not a visible row delete 
   // can be detected by calling TSQLResultSet::RowDeleted(). 
   // If DeletesAreDetected() returns kFALSE, then deleted rows are
   // removed from the result set.
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if changes are detected by the resultset type
   //   Throws:
   //         TSQLException - if a database access error occurs

  return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::InsertsAreDetected( Int_t type )
{
   // Indicates whether or not a visible row insert 
   // can be detected by calling TSQLResultSet::RowInserted().
   //
   //   Parameters:
   //         resultset type, i.e. TSQLResultSet::kTYPE_XXX
   //
   //   Returns:
   //         kTRUE if changes are detected by the resultset type
   //   Throws:
   //         TSQLException - if a database access error occurs

  return kFALSE;
}

//___________________________________________________________________
Bool_t TSQLDatabaseMetaData::SupportsBatchUpdates()
{
   // Indicates whether the driver supports batch updates.
   //
   //   Returns:
   //         kTRUE if the driver supports batch updates; 
   //         kFALSE otherwise

   return kFALSE;
}
