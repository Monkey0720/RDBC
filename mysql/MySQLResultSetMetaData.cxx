// $Id: MySQLResultSetMetaData.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 26/02/2001
//
// RDBC driver to MySQL database implemented with MySQL C API.
//
// ++ The code consists of some parts stolen from "mm JDBC" and  
//    "MyODBC ODBC" drivers and other mysql-related open sources.
//

////////////////////////////////////////////////////////////////////
//
// An object that can be used to find out about the 
// types and properties of the columns in a TSQLResultSet. 
//
// See also:
//    TSQLResultSet TSQLDatabaseMetaData
//
////////////////////////////////////////////////////////////////////
   
#include <RDBC/TSQLResultSetMetaData.h>
#include <RDBC/TSQLResultSet.h>
#include "MySQLResultSetPrivate.h"

ClassImpQ(TSQLResultSetMetaData)


///////////////////////////////////////////////////////////////////// 
//___________________________________________________________________   
TSQLResultSetMetaData::TSQLResultSetMetaData( TSQLResultSet* rs,
                                              void* imp ):TSQL(imp)
{
   // ctor  
   
   fResultSet = rs;

   // initiate some structures in  MySQLResultSetPrivate* imp

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;
   Int_t cols = imp->fMYSQL_RES->field_count;

   imp->fColumnPrecisions = new Int_t[cols];
   imp->fColumnTypes = new Int_t[cols];
   imp->fColumnTypeNames = new TString[cols];

   for(int i=0; i<cols; i++) {
      imp->fColumnPrecisions[i] = max(imp->fMYSQL_RES->fields[i].length, imp->fMYSQL_RES->fields[i].max_length);

      switch(imp->fMYSQL_RES->fields[i].type) {
      case FIELD_TYPE_DECIMAL: 
         imp->fColumnTypes[i] = kDECIMAL;
         imp->fColumnTypeNames[i] = "decimal";
         break;
      case FIELD_TYPE_CHAR:
         imp->fColumnTypeNames[i] = "tinyint";
         if(imp->fMYSQL_RES->fields[i].flags & UNSIGNED_FLAG) imp->fColumnTypeNames[i] += " unsigned";
         imp->fColumnTypes[i] = kTINYINT;
         break;
      case FIELD_TYPE_SHORT:
         imp->fColumnTypeNames[i] = "smallint";
         if(imp->fMYSQL_RES->fields[i].flags & UNSIGNED_FLAG) imp->fColumnTypeNames[i] += " unsigned";
         imp->fColumnTypes[i] = kSMALLINT;
         break;
      case FIELD_TYPE_INT24:
         imp->fColumnTypeNames[i] = "mediumint";
         if(imp->fMYSQL_RES->fields[i].flags & UNSIGNED_FLAG) imp->fColumnTypeNames[i] += " unsigned";
         imp->fColumnTypes[i] = kINTEGER;
         break;
      case FIELD_TYPE_LONG:
         imp->fColumnTypeNames[i] = "integer";
         if(imp->fMYSQL_RES->fields[i].flags & UNSIGNED_FLAG) imp->fColumnTypeNames[i] += " unsigned";
         imp->fColumnTypes[i] = kINTEGER;
         break;
      case FIELD_TYPE_LONGLONG:
         imp->fColumnTypeNames[i] = "bigint";
         if(imp->fMYSQL_RES->fields[i].flags & UNSIGNED_FLAG) imp->fColumnTypeNames[i] += " unsigned";
         imp->fColumnTypes[i] = ? kINTEGER : kBIGINT;
!!    return (stmt->dbc->flag & FLAG_NO_BIGINT) 
      case FIELD_TYPE_FLOAT:
         imp->fColumnTypeNames[i] = "float";
         if(imp->fMYSQL_RES->fields[i].flags & UNSIGNED_FLAG) imp->fColumnTypeNames[i] += " unsigned";
         imp->fColumnTypes[i] = kREAL;
         break;
      case FIELD_TYPE_DOUBLE:
         imp->fColumnTypeNames[i] = "double";
         if(imp->fMYSQL_RES->fields[i].flags & UNSIGNED_FLAG) imp->fColumnTypeNames[i] += " unsigned";
         imp->fColumnTypes[i] = kDOUBLE;
         break;
      case FIELD_TYPE_NULL:
         imp->fColumnTypeNames[i] = "null";
         imp->fColumnTypes[i] = kVARCHAR;
         break;
      case FIELD_TYPE_YEAR:
         imp->fColumnTypeNames[i] = "year";
         imp->fColumnTypes[i] = kSMALLINT;
         break;
      case FIELD_TYPE_TIMESTAMP:
         imp->fColumnTypeNames[i] = "timestamp";
         imp->fColumnPrecisions[i] = 19;
         imp->fColumnTypes[i] = kTIMESTAMP;
         break;
      case FIELD_TYPE_DATETIME:
         imp->fColumnTypeNames[i] = "datetime";
         imp->fColumnPrecisions[i] =  = 19;
         imp->fColumnTypes[i] = kTIMESTAMP;
         break;
      case FIELD_TYPE_NEWDATE:
      case FIELD_TYPE_DATE:
         imp->fColumnTypeNames[i] = "date";
         imp->fColumnPrecisions[i] = 10;
         imp->fColumnTypes[i] = kDATE;
         break;
      case FIELD_TYPE_TIME:
         imp->fColumnTypeNames[i] = "time";
         imp->fColumnPrecisions[i] = 8;
         imp->fColumnTypes[i] = kTIME;
         break;
      case FIELD_TYPE_STRING:
         imp->fColumnTypeNames[i] = "char";
         imp->fColumnTypes[i] = kCHAR;
         break;
      case FIELD_TYPE_VAR_STRING:
         imp->fColumnTypeNames[i] = "varchar";
         imp->fColumnTypes[i] = kVARCHAR;
         break;
      case FIELD_TYPE_TINY_BLOB:
         imp->fColumnTypeNames[i] = (imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? "tinyblob" : "tinytext");
         if (stmt->dbc->flag & (FLAG_FIELD_LENGTH | FLAG_SAFE)) imp->fColumnPrecisions[i] = 255;
         imp->fColumnTypes[i] = (imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? kLONGVARBINARY : kLONGVARCHAR;
      case FIELD_TYPE_BLOB:
         imp->fColumnTypeNames[i] = (imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? "blob" : "text");
         if (stmt->dbc->flag & (FLAG_FIELD_LENGTH | FLAG_SAFE))  imp->fColumnPrecisions[i] = 65535;
         imp->fColumnTypes[i] = (imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? kLONGVARBINARY : kLONGVARCHAR;
      case FIELD_TYPE_MEDIUM_BLOB:
         imp->fColumnTypeNames[i] = ((imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? "mediumblob" : "mediumtext"));
         if (stmt->dbc->flag & (FLAG_FIELD_LENGTH | FLAG_SAFE))imp->fColumnPrecisions[i] = (1L << 24)-1L;
         imp->fColumnTypes[i] = (imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? kLONGVARBINARY : kLONGVARCHAR;
      case FIELD_TYPE_LONG_BLOB:
         imp->fColumnTypeNames[i] = ((imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? "longblob": "longtext"));
         if (stmt->dbc->flag & (FLAG_FIELD_LENGTH | FLAG_SAFE)) imp->fColumnPrecisions[i] = = INT_MAX32;
         imp->fColumnTypes[i] = (imp->fMYSQL_RES->fields[i].flags & BINARY_FLAG) ? kLONGVARBINARY : kLONGVARCHAR;
      case FIELD_TYPE_ENUM:
         imp->fColumnTypeNames[i] = "enum";
         imp->fColumnTypes[i] = kCHAR;
         break;
      case FIELD_TYPE_SET:
         imp->fColumnTypeNames[i] = "set";
         imp->fColumnTypes[i] = kCHAR;
         break;
      }
   } 
}

//___________________________________________________________________
TSQLResultSetMetaData::~TSQLResultSetMetaData()
{
   // dtor  

   fResultSet = 0;
   fImp = 0;
}
                         
//___________________________________________________________________
Int_t TSQLResultSetMetaData::GetColumnCount()
{                   
   // Returns the number of columns in this TSQLResultSet
   // If there are no columns in the result set, zero is
   // returned.
   //
   //  Returns:
   //       the number of columns
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;
   return (imp->fMYSQL_RES->field_count);
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsAutoIncrement( Int_t column )
{                       
   // Indicates whether the column is automatically numbered, 
   // thus read-only.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE - the column's data type is an auto increment data
   //               type
   //       kFALSE - the  column's data type is not an auto increment
   //                data type or the column does not contain numeric 
   //                data
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }

   return (imp->MYSQL_RES->fields[column-1].flags & AUTO_INCREMENT_FLAG);
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsCaseSensitive( Int_t column )
{                        
   // Indicates whether a column's case sensitive for collations
   // and comparisons.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;
   
   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }

   return (imp->MYSQL_RES->fields[column-1].flags & BINARY_FLAG); // true if not binary
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsSearchable( Int_t column )                     
{
   // Indicates whether the column can be used in a 'WHERE' clause.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }

   return kTRUE;  // MyODBC always returns SQL_SEARCHABLE
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsCurrency( Int_t column )
{
   // Indicates whether the column is a cash value ( money data type ).
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }

   return kFALSE; // MyODBC always returns 0
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsNullable( Int_t column )               
{
   // Indicates the nullability of values in the designated column.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       the nullability status of the given column; 
   //       one of columnNoNulls, columnNullable or 
   //       columnNullableUnknown
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }
   
   return ((imp->MYSQL_RES->fields[column-1].flags & (NOT_NULL_FLAG | AUTO_INCREMENT_FLAG)) != NOT_NULL_FLAG);
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsSigned( Int_t column )                 
{
   // Indicates whether values in the column are signed numbers
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }

   retrurn !(imp->MYSQL_RES->fields[column-1].flags & UNSIGNED_FLAG);  
}

//___________________________________________________________________
Int_t TSQLResultSetMetaData::GetColumnDisplaySize( Int_t column )
{                         
   // Indicates the column's normal max width in chars.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       the normal maximum number of characteimp allowed as 
   //       the width of the designated column
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return 0;
   }

   return imp->fColumnPrecisions[column-1];
}

//___________________________________________________________________
TString TSQLResultSetMetaData::GetColumnLabel( Int_t column )
{         
   // Gets the suggested column title for use in printouts 
   // and displays.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       the suggested column title
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return "";
   }

   return imp->MYSQL_RES->fields[column-1].name;
}

//___________________________________________________________________
TString TSQLResultSetMetaData::GetColumnName( Int_t column )
{                     
   // Gets a column's name.
   //
   //   Parameters:
   //        column - the first column is 1, the second is 2, ...
   //   Returns:
   //        column name
   //   Throws:
   //        TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return "";
   }

   return imp->MYSQL_RES->fields[column-1].name;
}

//___________________________________________________________________
TString TSQLResultSetMetaData::GetSchemaName( Int_t column )
{                     
   // Gets a column's table's schema.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       schema name or "" if not applicable
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return "";
   }

   return "";  // do it like MyODBC, i.e. return nothing  ...
}

//___________________________________________________________________
Int_t TSQLResultSetMetaData::GetPrecision( Int_t column )
{  
   // Gets a column's number of decimal digits.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       precIsion
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return 0;
   }

   return imp->fColumnPrecisions[column-1];
}

//___________________________________________________________________
Int_t TSQLResultSetMetaData::GetScale( Int_t column )             
{
   // Gets a column's number of digits to right of the decimal point.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       scale
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return 0;
   }

   return (imp->MYSQL_RES->fields[column-1].decimals);
}

//___________________________________________________________________
TString TSQLResultSetMetaData::GetTableName( Int_t column )
{                    
   // Gets a column's table name.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       table name or "" if not applicable
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return ("");
   }

   return (imp->MYSQL_RES->fields[column-1].table ? imp->MYSQL_RES->fields[column-1].table : ""); 
}

//___________________________________________________________________
TString TSQLResultSetMetaData::GetCatalogName( Int_t column )
{                      
   // Gets a column's table's catalog name.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       column name or "" if not applicable.
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return "";
   }

   return "";  // do it like MyODBC, i.e.return nothing  ( could be retrived from TSQLStatement ) 
}

//___________________________________________________________________
Int_t TSQLResultSetMetaData::GetColumnType( Int_t column )
{                  
   // Retrieves a column's SQL type. 
   //
   // enum ESQLTypes { 
   //       kBIGINT = -5,
   //       kBINARY = -2,
   //       kBIT = -7,
   //       kCHAR = 1,
   // #ifdef ODBC_VER_LESS_30 
   //       kDATE = 9,
   //       kTIME = 10,
   //       kTIMESTAMP = 11,
   // #endif     
   //       kDATE = 91,
   //       kTIME = 92,
   //       kTIMESTAMP = 93,
   //       kSMALLINT = 5,
   //       kDECIMAL = 3,
   //       kDOUBLE = 8,
   //       kFLOAT = 6,
   //       kINTEGER = 4,
   //       kLONGVARBINARY = -4,
   //       kLONGVARCHAR = -1,
   //       kNUMERIC = 2,
   //       kREAL = 7,
   //       kTINYINT = -6,
   //       kVARBINARY = -3,
   //       kVARCHAR  = 12 
   // };
   //   
   // Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       SQL type from TSQLTypes
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return 0;
   }

   return imp->fColumnTypes[column-1];
}

//___________________________________________________________________
TString TSQLResultSetMetaData::GetColumnTypeName( Int_t column )
{
   // Retrieves a column's database-specific type name.
   // See TSQLTypes.h
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       type name used by the database. If the column type is 
   //       a user-defined type, then a fully-qualified type name 
   //       is returned.
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return "";
   }

   return imp->fColumnTypeNames[column-1];
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsReadOnly( Int_t column )                   
{
   // Indicates whether a column Is definitely not writable.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return (kTRUE);
   }

   return (!(imp->MYSQL_RES->fields[column-1].table && imp->MYSQL_RES->fields[column-1].table[0]));
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsWritable( Int_t column )                   
{
   // Indicates whether it is possible for a write on the column 
   // to succeed.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }

   return (imp->MYSQL_RES->fields[column-1].table && imp->MYSQL_RES->fields[column-1].table[0]);
}

//___________________________________________________________________
Bool_t TSQLResultSetMetaData::IsDefinitelyWritable( Int_t column )
{                             
   // Indicates whether a write on the column will definitely succeed.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs
   //
   // Comment: the same as TSQLResultSetMetaData::IsWritable()

   MySQLResultSetPrivate* imp = (MySQLResultSetPrivate*)fImp;

   if( (UInt_t)(column-1) >= imp->fMYSQL_RES->field_count ) {
      Throw(new TSQLException(Form("Invalid column number ( %d > %d )",column,imp->fMYSQL_RES->field_count+1),"S1002",);
      return kFALSE;
   }

   return (imp->MYSQL_RES->fields[column-1].table && imp->MYSQL_RES->fields[column-1].table[0]);
}
