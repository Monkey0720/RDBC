// $Id: TSQLDatabaseMetaData.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2000 
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

ClassImpQ(TSQLDatabaseMetaData)
            
/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLDatabaseMetaData::TSQLDatabaseMetaData(TSQLConnection* connection,
                                           void* imp):TSQL(imp)
{
   // constructor
   
   fConnection = connection;
}

//___________________________________________________________________
TSQLDatabaseMetaData::~TSQLDatabaseMetaData()
{
   // destructor will be called when fConnection is deleted 

   fConnection = 0;
   fImp = 0;
}
