// $Id: ODBCResultSetMetaData.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
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

////////////////////////////////////////////////////////////////////
//
// An object that can be used to find out about the 
// types and properties of the columns in a TSQLResultSet. 
//
// See also:
//    TSQLResultSet TSQLDatabaseMetaData
//
////////////////////////////////////////////////////////////////////
   
#include "ODBCResultSetMetaData.h"
#include "ODBCResultSet.h"
#include <RDBC/odbc++/resultsetmetadata.h>

using namespace odbc;
 
ClassImpQ(ODBCResultSetMetaData)

///////////////////////////////////////////////////////////////////// 
//___________________________________________________________________   
ODBCResultSetMetaData::ODBCResultSetMetaData( TSQLResultSet* rs,void* imp ):
   TSQLResultSetMetaData(rs,imp)
{
   // ctor  
   
   fResultSet = rs;
}

//___________________________________________________________________
ODBCResultSetMetaData::~ODBCResultSetMetaData()
{
   // dtor  

   fResultSet = 0;
//   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   fImp = 0;
   
   try {         
    	//   
   	//   delete imp;  // !!! private dtor, deleted by result set 
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
   }
}
                         
//___________________________________________________________________
Int_t ODBCResultSetMetaData::GetColumnCount()
{                   
   // Returns the number of columns in this TSQLResultSet
   // If there are no columns in the result set, zero is
   // returned.
   //
   //  Returns:
   //       the number of columns
   //  Throws:
   //       TSQLException - if a database access error occurs

   Int_t return_value = 0;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
      
   try {      
      return_value = imp->getColumnCount();
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return 0;
   }
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsAutoIncrement( Int_t column )
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
   //
   // 
   
   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {      
      return_value = imp->isAutoIncrement(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }   
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsCaseSensitive( Int_t column )
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

   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {      
      return_value = imp->isCaseSensitive(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }   
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsSearchable( Int_t column )                     
{
   // Indicates whether the column can be used in a 'WHERE' clause.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->isSearchable(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }  
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsCurrency( Int_t column )
{
   // Indicates whether the column is a cash value ( money data
   // type ).
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->isCurrency(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }   
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsNullable( Int_t column )               
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

   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->isNullable(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }  
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsSigned( Int_t column )                 
{
   // Indicates whether values in the column are signed numbers
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->isSigned(column); 
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }   
   return return_value;
}

//___________________________________________________________________
Int_t ODBCResultSetMetaData::GetColumnDisplaySize( Int_t column )
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

   Int_t return_value = 0;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->getColumnDisplaySize(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return 0;
   }   
   return return_value;
}

//___________________________________________________________________
TString ODBCResultSetMetaData::GetColumnLabel( Int_t column )
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

   TString str;
   
   if(!fImp) { Destroyed(); return str; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      str = ODBCXX_STRING_CSTR( imp->getColumnLabel(column) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }   
   return str;
}

//___________________________________________________________________
TString ODBCResultSetMetaData::GetColumnName( Int_t column )
{                     
   // Gets a column's name.
   //
   //   Parameters:
   //        column - the first column is 1, the second is 2, ...
   //   Returns:
   //        column name
   //   Throws:
   //        TSQLException - if a database access error occurs

   TString str;
   
   if(!fImp) { Destroyed(); return str; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      str = ODBCXX_STRING_CSTR( imp->getColumnName(column) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }  
   return str;
}

//___________________________________________________________________
TString ODBCResultSetMetaData::GetSchemaName( Int_t column )
{                     
   // Gets a column's table's schema.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       schema name or "" if not applicable
   //  Throws:
   //       TSQLException - if a database access error occurs

   TString str;
   
   if(!fImp) { Destroyed(); return str; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      str = ODBCXX_STRING_CSTR( imp->getSchemaName(column) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }   
   return str;
}

//___________________________________________________________________
Int_t ODBCResultSetMetaData::GetPrecision( Int_t column )
{  
   // Gets a column's number of decimal digits.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       precIsion
   //  Throws:
   //       TSQLException - if a database access error occurs

   Int_t return_value = 0;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->getPrecision(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return 0;
   }   
   return return_value;
}

//___________________________________________________________________
Int_t ODBCResultSetMetaData::GetScale( Int_t column )             
{
   // Gets a column's number of digits to right of the decimal point.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       scale
   //  Throws:
   //       TSQLException - if a database access error occurs

   Int_t return_value = 0;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
      
   try {
      return_value = imp->getScale(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return 0;
   }   
   return return_value;
}

//___________________________________________________________________
TString ODBCResultSetMetaData::GetTableName( Int_t column )
{                    
   // Gets a column's table name.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       table name or "" if not applicable
   //  Throws:
   //       TSQLException - if a database access error occurs

   TString str;
   
   if(!fImp) { Destroyed(); return str; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      str = ODBCXX_STRING_CSTR( imp->getTableName(column) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }
   return str;
}

//___________________________________________________________________
TString ODBCResultSetMetaData::GetCatalogName( Int_t column )
{                      
   // Gets a column's table's catalog name.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       column name or "" if not applicable.
   //  Throws:
   //       TSQLException - if a database access error occurs

   TString str;
   
   if(!fImp) { Destroyed(); return str; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      str = ODBCXX_STRING_CSTR( imp->getCatalogName(column) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }  
   return str;
}

//___________________________________________________________________
Int_t ODBCResultSetMetaData::GetColumnType( Int_t column )
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

   Int_t return_value = 0;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
      
   try {      
      return_value =  imp->getColumnType(column); 
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return 0;
   }   
   return return_value;
}

//___________________________________________________________________
TString ODBCResultSetMetaData::GetColumnTypeName( Int_t column )
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

   TString str;
   
   if(!fImp) { Destroyed(); return str; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      str = ODBCXX_STRING_CSTR( imp->getColumnTypeName(column) );
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return "";
   }
   return str;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsReadOnly( Int_t column )                   
{
   // Indicates whether a column Is definitely not writable.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   Bool_t return_value = kTRUE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->isReadOnly(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kTRUE;
   }  
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsWritable( Int_t column )                   
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

   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
   
   try {
      return_value = imp->isWritable(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }   
   return return_value;
}

//___________________________________________________________________
Bool_t ODBCResultSetMetaData::IsDefinitelyWritable( Int_t column )
{                             
   // Indicates whether a write on the column will definitely succeed.
   //
   //  Parameters:
   //       column - the first column is 1, the second is 2, ...
   //  Returns:
   //       kTRUE if so
   //  Throws:
   //       TSQLException - if a database access error occurs

   Bool_t return_value = kFALSE;
   
   if(!fImp) { Destroyed(); return return_value; }
   odbc::ResultSetMetaData* imp = (odbc::ResultSetMetaData*)fImp;
      
   try {
      return_value = imp->isDefinitelyWritable(column);
   } catch(odbc::SQLException& e) {
      Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
                                ODBCXX_STRING_CSTR(e.getSQLState()),
                                e.getErrorCode()) );
      return kFALSE;
   }   
   return return_value;
}
