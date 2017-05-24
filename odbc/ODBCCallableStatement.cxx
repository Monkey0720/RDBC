// $Id: ODBCCallableStatement.cxx,v 1.2 2007/02/28 21:33:39 phnxbld Exp $
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
// A TSQLCallableStatement extends the functionality of a 
// TSQLPreparedStatement, by allowing output parameters.
//
// The ODBC escapes for calling stored procedures and functions 
// should be used. A procedure call is prepared like this:
//
// TSQLCallableStatement* cstmt = 
//             con->PrepareCall("{call my_procedure(?,?,?)}");
//
// 
// And for a function call (a procedure that returns a value), the
// following syntax should be used:
//
// TSQLCallableStatement* cstmt=
//                   con->PrepareCall("{?=call my_function(?,?)}"); 
// 
// All parameters in a TSQLCallableStatement  are treated
// as input/output parameters, unless they are registered as
// output-only parameters with registerOutParameter(). Note that
// output-only parameters must be registered with their proper
// SQL type prior to executing a TSQLCallableStatement.
//
// The interface used to execute SQL stored procedures. It provides a
// stored procedure SQL escape that allows stored procedures to be 
// called in a standard way for all RDBMSs. This escape syntax has 
// one form that includes a result parameter and one that does not. 
// If used, the result parameter must be registered as an OUT parameter.
//  The other parameters can be used for input, output or both. 
///
// Parameters are referred to sequentially, by number. 
// The first parameter is 1. 
//
//        {?= call ?procedure-name?[?arg1?,?arg2?, ...]}
//        {call ?procedure-name?[?arg1?,?arg2?, ...]}
//      
// IN parameter values are set using the set methods inherited from
// TSQLPreparedStatement. The type of all OUT parameters must be 
// registered prior to executing the stored procedure; their values
// are retrieved after execution via the get methods provided here. 
//
// A TSQLCallableStatement can return one TSQLResultSet or multiple 
// TSQLResultSet objets. Multiple TSQLResultSet objects are handled 
// using operations inherited from TSQLStatement. 
//
// For maximum portability, a call's TSQLResultSet objects and update 
// counts should be processed prior to getting the values of output 
// parameters. 
//
// See also: 
//     TSQLConnection::PrepareCall(TString), TSQLResultSet
//     TSQLStatement TSQLPreparedStatement 
//
// Note: 
//       - Callable statments not supported by MySQL.
//       - I failed to use with OpenLink ODBC driver to Oracle
//
/////////////////////////////////////////////////////////////////////

#include <sstream>
#include "ODBCCallableStatement.h"
#include <RDBC/odbc++/statement.h>
#include <RDBC/odbc++/preparedstatement.h>
#include <RDBC/odbc++/callablestatement.h>
#include <iostream>
#include "ODBCResultSet.h"
#include <RDBC/odbc++/resultset.h>
#include <TList.h>

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0)
#include <TBufferFile.h>
#endif


using namespace odbc;

ClassImpQ(ODBCCallableStatement)

  /////////////////////////////////////////////////////////////////////
  //___________________________________________________________________
  ODBCCallableStatement::ODBCCallableStatement(TSQLConnection* con,void* imp):
    TSQLCallableStatement(con,imp)
   
{
  // ctor
}

//___________________________________________________________________
ODBCCallableStatement::~ODBCCallableStatement()
{
  // dtor

  odbc::CallableStatement* imp = (odbc::CallableStatement*)fImp;
 
  try { 
    if(imp) delete  imp;   
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }

  // implementation part of fCurrentResult is deleted with statement
  if(fCurrentResult) ((ODBCResultSet*)fCurrentResult)->fImp = 0;
  fImp = 0;
}

//___________________________________________________________________
void ODBCCallableStatement::RegisterOutParameter(Int_t parameterIndex,
                                                 Int_t sqlType)
{
  //  Registers the OUT parameter in ordinal position parameterIndex to
  //  the type sqlType. All OUT parameters must be registered before
  //  a stored procedure is executed. 
  //
  //  The type specified by sqlType for an OUT parameter determines
  //  the  type that must be used in the get method to read the value
  //  of that parameter. 
  //
  //  Parameters:
  //
  //      parameterIndex - the first parameter is 1, 
  //                       the second is 2, and so on
  //      sqlType - the type code defined by ESQLTypes (see TSQLTypes.h)
  //                If the parameter is of type kNumeric, 
  //                the version of registerOutParameter that 
  //                accepts a scale value should be used.
  //  Throws:
  //      TSQLException - if a database access error occurs
  //  See Also: 
  //      TSQLTypes.h 
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

  if(!fImp) { Destroyed(); return; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
      
  try {   
    stmt->registerOutParameter( parameterIndex,sqlType );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::RegisterOutParameter(Int_t parameterIndex,
                                                 Int_t sqlType,
                                                 Int_t scale)
{
  //  Registers the parameter in ordinal position parameterIndex to be
  //  of type sqlType. This method must be called before a stored
  //  procedure is executed. 
  //
  //  The type specified by sqlType for an OUT parameter determines
  //  the  type that must be used in the get method to read the value
  //  of that parameter. 
  //
  //  This version of registerOutParameter should be used when the
  //  parameter is of type kNUMERIC.
  //  
  //  Parameters:
  //     parameterIndex -  the first parameter is 1, 
  //                       the second is 2, and so on
  //     sqlType -  SQL type code defined in TSQLTypes.h
  //     scale -    the desired number of digits to the right of the
  //                decimal point. It must be greater than or equal 
  //                to zero.
  //  Throws:
  //      TSQLException - if a database access error occurs
  //  See Also: 
  //      TSQLTypes.h
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

  if(!fImp) { Destroyed(); return; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
      
  try {
    stmt->registerOutParameter( parameterIndex,sqlType,scale );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
Bool_t ODBCCallableStatement::WasNull()
{
  // Indicates whether or not the last OUT parameter read had 
  // the value of SQL NULL. Note that this method should be 
  // called only after calling the get method; otherwise, there 
  // is no value to use in determining whether it is null or not.
  //  
  //  Returns:
  //      kTRUE if the last parameter read was SQL NULL; 
  //      kFALSE otherwise.
  //  Throws:
  //      TSQLException - if a database access error occurs

  Bool_t return_value = kFALSE;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    return_value = stmt->wasNull();

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return kFALSE;
  }   
  return return_value;
}

//___________________________________________________________________
TString ODBCCallableStatement::GetString( Int_t parameterIndex )
{
  // Retrieves the value of a  parameter as a TString. 
  //
  // For the fixed-length type kCHAR, the TString object returned 
  // has exactly the same value the kCHAR value had in the database,
  // including any padding added by the database.
  //
  //  Parameters:
  //      parameterIndex - the first parameter is 1, 
  //                       the second is 2,  and so on
  //  Returns:
  //      the parameter value. If the value is SQL NULL, 
  //                           the result is null.
  //  Throws:
  //      TSQLException - if a database access error occurs
   
  TString str;
   
  if(!fImp) { Destroyed(); return str; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    ODBCXX_STRING s = stmt->getString(parameterIndex);
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
Bool_t ODBCCallableStatement::GetBoolean( Int_t parameterIndex )
{
  //  Gets the value of a parameter as a Bool_t 
  //  
  //  Parameters:
  //       parameterIndex -  the first parameter is 1, 
  //                         the second is 2, and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is kFALSE.
  //  Throws:
  //      TSQLException - if a database access error occurs

  Bool_t return_value = kFALSE;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    return_value = stmt->getBoolean(parameterIndex);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return kFALSE;
  }
  return return_value;
}

//___________________________________________________________________
Char_t ODBCCallableStatement::GetByte( Int_t parameterIndex )
{
  // Gets the value of a parameter as a byte .
  //
  //  Parameters:
  //      parameterIndex - the first parameter is 1, 
  //                       the second is 2, and so on
  //  Returns:
  //      the parameter value. If the value is SQL NULL, 
  //      the result is 0.
  //  Throws:
  //      TSQLException - if a database access error occurs
   
  Char_t return_value = 0;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
    
  try {
    return_value = stmt->getByte(parameterIndex);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }   
  return return_value;
}

//___________________________________________________________________
Short_t ODBCCallableStatement::GetShort( Int_t parameterIndex )
{
  // Gets the value of a parameter as a Short_t .
  //  
  //  Parameters:
  //       parameterIndex - the first parameter is 1, 
  //                        the second is 2, and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is 0.
  //   Throws:
  //       TSQLException - if a database access error occurs

  Short_t return_value = 0;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    return_value = stmt->getShort(parameterIndex);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }
  return return_value;
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetInt( Int_t parameterIndex )
{
  // Gets the value of a parameter as an Int_t .
  //
  //  Parameters:
  //       parameterIndex -  the first parameter is 1, the second is 2,
  //                         and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is 0.
  //  Throws:
  //       TSQLException - if a database access error occurs

  Int_t return_value = 0;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 

  try {
    return_value = stmt->getInt(parameterIndex);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }   
  return return_value;
}

//___________________________________________________________________
Long_t ODBCCallableStatement::GetLong( Int_t parameterIndex )
{
  // Gets the value of a parameter as a Long_t .
  //
  //  Parameters:
  //       parameterIndex - the first parameter is 1, 
  //                       the second is 2, and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is 0.
  //  Throws:
  //       TSQLException - if a database access error occurs

  Long_t return_value = 0;

  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    return_value = stmt->getLong(parameterIndex);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }   
  return return_value;
}

//___________________________________________________________________
Float_t ODBCCallableStatement::GetFloat( Int_t parameterIndex )
{
  // Gets the value of a parameter as a Float_t .
  //
  // Parameters:
  //       parameterIndex - the first parameter is 1, 
  //                       the second is 2, and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is 0.
  //  Throws:
  //       TSQLException - if a database access error occurs

  Float_t return_value = 0;

  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    return_value = stmt->getFloat(parameterIndex);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }   
  return return_value;
}

//___________________________________________________________________
Double_t ODBCCallableStatement::GetDouble( Int_t parameterIndex )
{
  // Gets the value of a parameter as a Double_t .
  //
  //  Parameters:
  //       parameterIndex - the first parameter is 1, 
  //                       the second is 2, and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is 0.
  //  Throws:
  //       TSQLException - if a database access error occurs

  Double_t return_value = 0;

  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    return_value = stmt->getDouble(parameterIndex);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }   
  return return_value;
}

//___________________________________________________________________
TArrayC ODBCCallableStatement::GetBytes( Int_t parameterIndex )
{
  // Gets the value of a parameter as an array of byte values.
  //
  //  Parameters:
  //       parameterIndex -  the first parameter is 1, 
  //                         the second is 2, and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is null.
  //  Throws:
  //       TSQLException - if a database access error occurs

  TArrayC array;
   
  if(!fImp) { Destroyed(); return array; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try { 
    ODBCXX_BYTES b = stmt->getBytes(parameterIndex);
      
    array.Set( (Int_t)ODBCXX_BYTES_SIZE(b),
	       (Char_t*)ODBCXX_BYTES_DATA(b) );
   
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return TArrayC();
  }
  return array;
}

//___________________________________________________________________
TSQLDate ODBCCallableStatement::GetDate( Int_t parameterIndex )
{
  //  Gets the value of a parameter as a TSQLDate object.
  //
  //  Parameters:
  //       parameterIndex -  the first parameter is 1, 
  //                         the second is 2, and so on
  //  Returns:
  //       the parameter value. If the value is SQL NULL, 
  //       the result is null.
  //  Throws:
  //       TSQLException - if a database access error occurs

  TSQLDate return_value;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 

  try {
    odbc::Date dt = stmt->getDate(parameterIndex);

    return_value = TSQLDate( dt.getYear(),
			     dt.getMonth(),
			     dt.getDay() );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return TSQLDate();
  }
  return return_value;
}

//___________________________________________________________________
TSQLTime ODBCCallableStatement::GetTime( Int_t parameterIndex )
{
  // Get the value of a parameter as a TSQLTime object.
  // 
  // Parameters:
  //      parameterIndex -  the first parameter is 1, 
  //                        the second is 2, and so on
  // Returns:
  //      the parameter value. If the value is SQL NULL, 
  //      the result is null.
  // Throws:
  //      TSQLException - if a database access error occurs

  TSQLTime return_value;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    odbc::Time tm = stmt->getTime(parameterIndex);

    return_value = TSQLTime( tm.getHour(),
			     tm.getMinute(),
			     tm.getSecond() );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return TSQLTime();
  }
  return return_value;
}

//___________________________________________________________________
TSQLTimestamp ODBCCallableStatement::GetTimestamp( Int_t parameterIndex )
{
  // Gets the value of a parameter as a TSQLTimestamp object.
  //  
  // Parameters:
  //      parameterIndex -  the first parameter is 1, 
  //                        the second is 2, and so on
  // Returns:
  //      the parameter value. If the value is SQL NULL, 
  //      the result is null.
  // Throws:
  //      TSQLException - if a database access error occurs
   
  TSQLTimestamp return_value;
   
  if(!fImp) { Destroyed(); return return_value; } 
  odbc::CallableStatement* stmt=(odbc::CallableStatement*)fImp; 
   
  try {
    odbc::Timestamp tmstmp = stmt->getTimestamp(parameterIndex);

    return_value = TSQLTimestamp( tmstmp.getYear(),
				  tmstmp.getMonth(),
				  tmstmp.getDay(), 
				  tmstmp.getHour(),
				  tmstmp.getMinute(),
				  tmstmp.getSecond(),
				  tmstmp.getNanos() );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return TSQLTimestamp();
  }
  return return_value;
}

///////////////// include from ODBCPreparedStatement.cxx /////////////
//___________________________________________________________________
void ODBCCallableStatement::SetNull( Int_t parameterIndex,Int_t sqlType )
{
  // Sets the designated parameter to SQL NULL. 
  //
  //   Note: You must specify the parameter's SQL type.
  //
  //   Parameters:
  //          parameterIndex - the first parameter is 1, 
  //                           the second is 2, ...
  //          sqlType - the SQL type code defined in TSQLTypes
  //   Throws:
  //          TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
     
  try {
    imp->setNull(parameterIndex,sqlType);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetBoolean( Int_t parameterIndex,Bool_t x )
{
  // Sets the designated parameter to a Bool_t value. The
  // driver converts this to an SQL BIT value when it sends it to
  // the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setBoolean(parameterIndex,x); 

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetByte( Int_t parameterIndex,Char_t x )
{
  // Sets the designated parameter to a  byte value. The
  // driver converts this to an SQL TINYINT value when it sends
  // it to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {   
    imp->setByte(parameterIndex,x); 

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetShort( Int_t parameterIndex,Short_t x )
{
  // Sets the designated parameter to a  short value. The
  //   driver converts this to an SQL SMALLINT value when it sends
  //   it to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                            the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs

  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setShort(parameterIndex,x);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetInt( Int_t parameterIndex,Int_t x )
{
  // Sets the designated parameter to a  int value. The
  // driver converts this to an SQL INTEGER value when it sends
  // it to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setInt(parameterIndex,x);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetLong( Int_t parameterIndex,Long_t x )
{
  // Sets the designated parameter to a  long value. The
  // driver converts this to an SQL BIGINT value when it sends it
  // to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setLong(parameterIndex,x);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetFloat( Int_t parameterIndex,Float_t x )
{
  // Sets the designated parameter to a  float value. The
  // driver converts this to an SQL FLOAT value when it sends it
  // to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setFloat(parameterIndex,x);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetDouble( Int_t parameterIndex,Double_t x )
{
  // Sets the designated parameter to a  double value. The
  // driver converts this to an SQL DOUBLE value when it sends it
  // to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setDouble(parameterIndex,x);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetString( Int_t parameterIndex, 
                                       const TString& x )
{
  //  Sets the designated parameter to a  TString value. The
  //  driver converts this to an SQL VARCHAR or LONGVARCHAR value
  //  (depending on the argument's size relative to the driver's
  //  limits on VARCHARs) when it sends it to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setString( parameterIndex, ODBCXX_STRING_C(x.Data()) );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetBytes( Int_t parameterIndex,
                                      const TArrayC& x )
{
  // Sets the designated parameter to a  array of bytes. The
  // driver converts this to an SQL VARBINARY or LONGVARBINARY
  // (depending on the argument's size relative to the driver's
  // limits on VARBINARYs) when it sends it to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs

  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->setBytes( parameterIndex,
		   ODBCXX_BYTES_C(x.GetArray(),x.GetSize()) );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetDate( Int_t parameterIndex,
                                     const TSQLDate& x )
{
  //  Sets the designated parameter to a TSQLDate value. The
  //  driver converts this to an SQL DATE value when it sends it
  //  to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    odbc::Date dt( x.GetYear(),
		   x.GetMonth(),
		   x.GetDay() );

    imp->setDate(parameterIndex,dt);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetTime( Int_t parameterIndex,
                                     const TSQLTime& x )
{
  // Sets the designated parameter to a TSQLTime value. The
  // driver converts this to an SQL TIME value when it sends it
  // to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    odbc::Time tm( x.GetHour(),
		   x.GetMinute(),
		   x.GetSecond() );

    imp->setTime(parameterIndex,tm);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetTimestamp( Int_t parameterIndex,
                                          const TSQLTimestamp& x )
{
  // Sets the designated parameter to a TSQLTimestamp value.
  // The driver converts this to an SQL TIMESTAMP value when it
  // sends it to the database.
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1,
  //                          the second is 2, ...
  //         x - the parameter value
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    odbc::Timestamp tmstmp( x.GetYear(),
			    x.GetMonth(),
			    x.GetDay(),
			    x.GetHour(),
			    x.GetMinute(),
			    x.GetSecond(),
			    x.GetNanos() );

    imp->setTimestamp(parameterIndex,tmstmp);

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetAsciiStream( Int_t parameterIndex,
                                            TBuffer* x,
                                            Int_t length )
{
  // Sets the designated parameter to the given input stream,
  // which will have the specified number of bytes. When a very
  // large ASCII value is input to a LONGVARCHAR parameter, it
  // may be more practical to send it via a TBuffer
  // will read the data from the stream as needed, until it
  // reaches end-of-file. The  driver will do any necessary
  // conversion from ASCII to the database char format. 
  //
  //    Parameters:
  //          parameterIndex - the first parameter is 1, 
  //                           the second is 2, ...
  //          x - the  input stream that contains the ASCII
  //              parameter value
  //          length - the number of bytes in the stream,
  //                   total size of buffer is by default. 
  //    Throws:
  //          TSQLException - if a database access error occurs

  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    Int_t xl = x->BufferSize()>length ? length : x->BufferSize();
    std::istringstream* s = new std::istringstream( x->Buffer() ); 
    imp->setAsciiStream( parameterIndex,(std::istream*)s,xl ); 

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetBinaryStream( Int_t parameterIndex,
                                             TBuffer* x,
                                             Int_t length )
{
  // Sets the designated parameter to the given input stream,
  // which will have the specified number of bytes. When a very
  // large binary value is input to a LONGVARBINARY parameter, it
  // may be more practical to send it via a TBuffer.
  // will read the data from the stream as needed, until it
  // reaches end-of-file. 
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the input tream which contains the binary
  //                  parameter value
  //         length - the number of bytes in the stream
  //                   total size of buffer is by default. 
  //   Throws:
  //         TSQLException - if a database access error occurs

  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    Int_t xl = x->BufferSize()>length ? length : x->BufferSize();
    std::string a(x->Buffer(),xl);
    std::istream* s = new std::istringstream(a);
    imp->setBinaryStream( parameterIndex,s,xl );

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetObject( Int_t parameterIndex,TObject* x )
{
  // Sets the designated parameter to the given ROOT object
  //
  //   Parameters:
  //         parameterIndex - the first parameter is 1, 
  //                          the second is 2, ...
  //         x - the ROOT object
  //   Throws:
  //         TSQLException - if a database access error occurs
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0)
  TBuffer *b = new TBufferFile(TBuffer::kWrite);
#else
  TBuffer *b = new TBuffer(TBuffer::kWrite);
#endif
  b->WriteObject(x);
  SetBinaryStream(parameterIndex,b,b->BufferSize());
  b->DetachBuffer();
  delete b;
}

//___________________________________________________________________
void ODBCCallableStatement::ClearParameters()
{
  // Clears the current parameter values immediately. 
  //
  //  In general, parameter values remain in force for repeated
  //  use of a TSQLStatement. Setting a parameter value 
  //  automatically clears its previous value. However, in some 
  //  cases it is useful to immediately release the resources used 
  //  by the current parameter values; this can be done by calling
  //  ClearParameters().
  //   
  //   Throws:
  //         TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed();   return; } 
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
   
  try {
    imp->clearParameters();

  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

////////////////// include from ODBCStatement.cxx ///////////////////
//___________________________________________________________________
TSQLResultSet* ODBCCallableStatement::ExecuteQuery( const TString& sql )
{
  // Executes a SQL statement that returns a single TSQLResultSet
  //
  // This method also implicitly closes current TSQLResultSet 
  //
  // Returns:
  //       a TSLResultSet that contains the data produced by the query; 
  //       NULL - in case of error
  //
  //   Throws:
  //       TSQLException - if a database access error occurs

  if(!fImp) { Destroyed(); return 0; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;  
  odbc::ResultSet* rs = 0; 
  ClearWarnings();

  if(fCurrentResult)  delete fCurrentResult;

  try {
    if(!sql.IsNull()) {
      rs = stmt->executeQuery(ODBCXX_STRING_C(sql.Data()));
    } else {
      rs = imp->executeQuery();
    } 
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    if(rs) delete rs;
    fCurrentResult = 0;
    return 0;
  }
   
  return fCurrentResult = new ODBCResultSet(this,(void*)rs);;
}

//___________________________________________________________________
Bool_t ODBCCallableStatement::Execute( const TString& sql )
{
  // Executes a SQL statement that may return multiple results.
  // Under some (uncommon) situations a single SQL statement may 
  // return multiple result sets and/or update counts. Normally you 
  // can ignore this unless you are (1) executing a stored
  // procedure that you know may return multiple results or (2) you 
  // are dynamically executing an unknown SQL string. The methods 
  // execute, GetMoreResults(), GetResultSet(), and GetUpdateCount()
  // let you navigate through multiple results.
  //  The execute method executes a SQL statement and indicates the 
  // form of the first result. You can then use GetResultSet() or
  // GetUpdateCount() to retrieve the result, and GetMoreResults() 
  // to move to any subsequent result(s).
  //
  // Parameters:
  //          sql - any SQL statement
  // Returns:
  //          kTRUE if the next result is a TSQLResultSet; 
  //          kFALSE if it is an update count or there are no more
  //          results
  // Throws:
  //            TSQLException - if a database access error occurs
  // See Also: 
  //       GetResultSet(), GetUpdateCount(), GetMoreResults()
  
  if(!fImp) { Destroyed(); return kFALSE; }

  Bool_t return_value = kFALSE;
  ClearWarnings();
  odbc::Statement* stmt = (odbc::Statement*)fImp;
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;
 
  try {
    if(!sql.IsNull()) {
      return_value = (Bool_t)stmt->execute(ODBCXX_STRING_C(sql.Data()));
    } else {
      return_value = (Bool_t)imp->execute();
    } 
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return_value = kFALSE;
  }
  return return_value;
}

//___________________________________________________________________
Int_t ODBCCallableStatement::ExecuteUpdate( const TString& sql )
{
  // Executes an SQL INSERT, UPDATE or DELETE statement. 
  // In addition, SQL statements that return nothing, 
  // such as SQL DDL statements, can be executed.
  //
  //  Parameters:
  //      sql - a SQL INSERT, UPDATE or DELETE statement or 
  //            a SQL statement that  returns nothing
  //
  //  Returns:
  //      either the row count for INSERT, UPDATE or DELETE or 
  //      0 for SQL statements that return nothing
  //  Throws:
  //      TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return 0; }

  Int_t return_value = 0;
  ClearWarnings();
  odbc::Statement* stmt = (odbc::Statement*)fImp;
  odbc::PreparedStatement* imp = (odbc::PreparedStatement*)fImp;

  try {
    if(!sql.IsNull()) {
      return_value = (Bool_t)stmt->executeUpdate(ODBCXX_STRING_C(sql.Data()));
    } else {
      return_value = (Bool_t)imp->executeUpdate();
    } 
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return_value = 0;
  }
  return return_value;
}

//___________________________________________________________________
TSQLResultSet* ODBCCallableStatement::GetResultSet()
{
  // Returns the current result as a TSQLResultSet object. 
  // This method should be called only once per result.
  //
  // This method also implicitly closes any current TSQLResultSet 
  //   
  // Returns:
  //       the current result as a TSQLResultSet; null if the result 
  //       is an update count or there are no more results
  // Throws:
  //       TSQLException - if a database access error occurs
  // See Also: 
  //       Execute(const TString&)
   
  if(!fImp) { Destroyed(); return 0; }
  odbc::ResultSet* rs = 0;
  odbc::Statement* stmt = (odbc::Statement*)fImp;

  if(fCurrentResult)  delete fCurrentResult;

  try {     
    rs = stmt->getResultSet(); 
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    if(rs) delete rs;
    fCurrentResult = 0;
    return 0;
  }

  return fCurrentResult = new ODBCResultSet(this,(void*)rs); 
}   

//___________________________________________________________________
Int_t ODBCCallableStatement::GetUpdateCount()
{
  // Returns the current result as an update count; 
  // if there are no more results, -1 is returned.
  // This method should be called only once per result.
  //
  // Returns:
  //       the current result as an update count; -1 if it is a 
  //       TSQLResultSet or there are no more results
  // Throws:
  //       TSQLException - if a database access error occurs
  // See Also: 
  //       Execute(const TString&)
   
  if(!fImp) { Destroyed(); return 0; }

  Int_t return_value = 0;
  odbc::Statement* stmt = (odbc::Statement*)fImp;
      
  try {
    return_value = stmt->getUpdateCount();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return  0;
  }
  return return_value;
}

//___________________________________________________________________
Bool_t ODBCCallableStatement::GetMoreResults()
{
  // Moves to a ODBCStatement's next result. It returns kTRUE if 
  // this result is a TSQLResultSet. 
  // 
  // There are no more results when 
  //       (!GetMoreResults() && (GetUpdateCount() == -1)
  //
  // Returns:
  //    kTRUE if the next result is a TSQLResultSet; 
  //    kFALSE if it is an update count or there are no more results
  //
  // Throws:
  //       TSQLException - if a database access error occurs
  // See Also: 
  //       Execute(const TString&)

  Bool_t return_value = kFALSE;
   
  if(!fImp) { Destroyed(); return return_value; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
      
  try {
    return_value = (Bool_t)stmt->getMoreResults();      
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return  kFALSE;
  }
  return return_value;
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetMaxFieldSize()
{
  // Returns the maximum number of bytes allowed for any column 
  // value. This limit is the maximum number of bytes that can be
  // returned for any column value. The limit applies only to 
  // kBINARY, kVARBINARY, kLONGVARBINARY, kCHAR, kVARCHAR, and 
  // kLONGVARCHAR columns (see TSQLTypes.h). If the limit is exceeded, 
  // the excess data  is silently discarded.
  //
  // Returns:
  //    the current max column size limit; zero means unlimited
  // Throws:
  //    TSQLException - if a database access error occurs

  if(!fImp) { Destroyed(); return 0; }

  Int_t return_value = 0;
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    return_value = stmt->getMaxFieldSize();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }
  return return_value;
}

//___________________________________________________________________
void ODBCCallableStatement::SetMaxFieldSize( Int_t max )
{
  // Sets the limit for the maximum number of bytes in a column to 
  // the given number of bytes. This is the maximum number of bytes 
  // that can be returned for any column value. This limit applies 
  // only to kBINARY, kVARBINARY, kLONGVARBINARY, kCHAR, kVARCHAR,
  // and kLONGVARCHAR fields (see TSQLTypes.h) . If the limit is exceeded, 
  // the excess  data is silently discarded. For maximum portability, 
  // use values greater than 256.
  //
  // Parameters:
  //       max - the new max column size limit; zero means unlimited
  // Throws:
  //       TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    stmt->setMaxFieldSize(max);   
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetMaxRows()
{
  // Retrieves the maximum number of rows that a TSQLResultSet can 
  // contain. If the limit is exceeded, the excess rows are silently 
  // dropped.
  //
  // Returns:
  //       the current max row limit; zero means unlimited
  // Throws:
  //       TSQLException - if a database access error occurs

  if(!fImp) { Destroyed(); return 0; }

  Int_t return_value = 0;
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    return_value = stmt->getMaxRows();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }
  return return_value;
}

//___________________________________________________________________
void ODBCCallableStatement::SetMaxRows( Int_t max )
{
  // Sets the limit for the maximum number of rows that any 
  // TSQLResultSet can contain to the given number. If the limit is 
  // exceeded, the excess rows are silently dropped.
  //
  // Parameters:
  //       max - the new max rows limit; zero means unlimited
  // Throws:
  //       TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    stmt->setMaxRows(max);  
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetEscapeProcessing( Bool_t enable )
{
  // Sets escape processing on or off. If escape scanning is on 
  // (the default), the driver will do escape substitution before 
  // sending the SQL to the database.
  // 
  // Note:
  //    Since prepared statements have usually been parsed prior to 
  //    making this call, disabling escape processing for prepared 
  //    statements will have no effect.
  //
  // Parameters:
  //       enable - kTRUE to enable; kFALSE to disable
  // Throws:
  //       TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    stmt->setEscapeProcessing(enable);
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
Bool_t ODBCCallableStatement::GetEscapeProcessing()
{
  //  Returns if escape processing is on or off. 
  // If escape scanning is on (the default), the driver will do escape 
  // substitution before  sending the SQL to the database.
  // 
  // Note:
  //    Since prepared statements have usually been parsed prior to 
  //    making this call, disabling escape processing for prepared 
  //    statements will have no effect.
  //
  // Parameters:
  //       enable - kTRUE to enable; kFALSE to disable
  // Throws:
  //       TSQLException - if a database access error occurs

  if(!fImp) { Destroyed(); return kFALSE; }   

  Bool_t return_value = kFALSE;
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    return_value = stmt->getEscapeProcessing();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return kFALSE;
  }
  return return_value;
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetQueryTimeout()
{
  // Retrieves the number of seconds the driver will wait for a
  // ODBCStatement to execute. If the limit is exceeded, a 
  // TSQLException is thrown.
  //
  // Returns:
  //    the current query timeout limit in seconds; zero means
  //    unlimited
  // Throws:
  //    TSQLException - if a database access error occurs
   
  Int_t return_value = 0;
   
  if(!fImp) { Destroyed(); return return_value; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
      
  try {
    return_value = stmt->getQueryTimeout();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }
  return return_value;
}

//___________________________________________________________________
void ODBCCallableStatement::SetQueryTimeout( Int_t seconds )
{
  // Sets the number of seconds the driver will wait for a 
  // ODBCStatement to execute to the given number of seconds. 
  // If the limit is exceeded, a TSQLException is thrown.
  //
  // Parameters:
  //          seconds - the new query timeout limit in seconds; 
  //          zero means unlimited
  // Throws:
  //          TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    stmt->setQueryTimeout(seconds);
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::Cancel() 
{
  // Cancels this statement object if both the DBMS and driver 
  // support aborting an SQL statement. This method can be used by 
  // one thread to cancel a statement that is being executed by 
  // another thread.
  //
  // Throws:
  //       TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    stmt->cancel();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::Close()
{
  // Avoid using this method. Use delete ODBCStatement instead.
  //
  //  Note: When a ODBCStatement is closed, its current 
  //       TSQLResultSet,  if one exists, is also closed.
  //
  //     Throws:
  //      TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return; }
          
  try {    
    if(fCurrentResult)  { 
      delete fCurrentResult;
      fCurrentResult = 0;
    }
    ClearBatch();
    SafeDelete(fBatches);

    odbc::Statement* imp = (odbc::Statement*)fImp;
    if(imp) delete  imp;
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
  fImp = 0;
  Destroyed();
}

//___________________________________________________________________
void ODBCCallableStatement::SetCursorName( const TString& name )
{
  // Defines the SQL cursor name that will be used by subsequent
  // ODBCStatement execute methods. This name can then be used in 
  // SQL positioned update/delete statements to identify the 
  // current row in the TSQLResultSet generated by this statement. 
  // If the database doesn't support positioned update/delete, 
  // this method is a noop. To insure that a cursor has the proper 
  // isolation level to support updates, the cursor's SELECT 
  // statement should be of the form 'SELECT FOR UPDATE ...'. If
  // the 'FOR UPDATE' phrase is omitted, positioned updates may 
  // fail. 
  //
  // Note: By definition, positioned update/delete execution must 
  //    be done by a different ODBCStatement than the one which 
  //    generated the TSQLResultSet being used for positioning.
  //    Also, cursor names must be unique within a connection.
  //
  // Parameters:
  //       name - the new cursor name, which must be unique within
  //       a connection
  // Throws:
  //       TSQLException - if a database access error occurs
   
  if(!fImp) { Destroyed(); return; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    stmt->setCursorName(ODBCXX_STRING_C(name.Data()));
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
  }
}

//___________________________________________________________________
void ODBCCallableStatement::SetFetchDirection( Int_t /* direction */ )
{
  // Gives the driver a hint as to the direction in which the
  // rows in a result set will be processed. The hint applies only 
  // to result sets created using this statement object. 
  // The default value is TSQLResultSet::kTYPE_FORWARD_ONLY
  //
  // Note that this method sets the default fetch direction for 
  // result sets generated by this statement object.
  //
  // Parameters:
  //    direction - the initial direction for processing rows
  // Throws:
  //    TSQLException - if a database access error occurs or the 
  //                   given direction is not one of 
  // 
   
  if(!fImp) { Destroyed(); return; }
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetFetchDirection()
{
  // Retrieves the direction for fetching rows from database
  // tables that is the default for result sets generated from this 
  // statement object. If this statement object has not set 
  // a fetch direction by calling the method SetFetchDirection(), 
  // the return value is implementation-specific.
  //
  // Returns:
  //       the default fetch direction for result sets generated 
  //       from this statement object
  // Throws:
  //       TSQLException - if a database access error occurs

  return 0;
}

//___________________________________________________________________
void ODBCCallableStatement::SetFetchSize( Int_t /* rows */ )
{
  // Gives the driver a hint as to the number of rows that
  // should be fetched from the database when more rows are needed. 
  // The number of rows specified affects only result sets created 
  // using this statement. If the value specified is zero, then the
  // hint is ignored. The default value is zero.
  //
  // Parameters:
  //       rows - the number of rows to fetch
  // Throws:
  //       TSQLException - if a database access error occurs, or 
  //       the condition 0 <= rows <= GetMaxRows() is not satisfied.
   
  if(!fImp) { Destroyed(); return; }
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetFetchSize()
{
  // Retrieves the number of result set rows that is the default 
  // fetch size for result sets generated from this ODBCStatement 
  // object. If this statement object has not set a fetch size
  // by calling the method SetFetchSize(), the return value is
  // implementation-specific.
  //
  // Returns:
  //       the default fetch size for result sets generated from 
  //       this statement object
  // Throws:
  //       TSQLException - if a database access error occurs
   
  Int_t return_value = 0;
   
  if(!fImp) { Destroyed(); return return_value; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    return_value = stmt->getFetchSize();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }
  return return_value;
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetResultSetConcurrency()
{
  // Retrieves the result set concurrency.
  //
  // enum EResultSetConcurrency{
  //       kCONCUR_READ_ONLY,
  //       kCONCUR_UPDATABLE
  //    };

  Int_t return_value = 0;
   
  if(!fImp) { Destroyed(); return return_value; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    return_value = stmt->getResultSetConcurrency();
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }   
  return return_value;
}

//___________________________________________________________________
Int_t ODBCCallableStatement::GetResultSetType()
{
  // Determine the result set type.
  //
  // enum EResultSetType{
  //       kTYPE_FORWARD_ONLY,
  //       kTYPE_SCROLL_INSENSITIVE,
  //       kTYPE_SCROLL_SENSITIVE
  //       };
  //
     
  Int_t return_value = 0;
   
  if(!fImp) { Destroyed(); return return_value; }
  odbc::Statement* stmt = (odbc::Statement*)fImp;
   
  try {
    return_value = stmt->getResultSetType(); 
  } catch(odbc::SQLException& e) {
    Throw( new TSQLException( ODBCXX_STRING_CSTR(e.getMessage()),
			      ODBCXX_STRING_CSTR(e.getSQLState()),
			      e.getErrorCode()) );
    return 0;
  }
  return return_value;
}

//___________________________________________________________________
void ODBCCallableStatement::AddBatch( const TString& /* sql */ )
{
  // Adds a SQL command to the current batch of commmands for
  // the statement. This method is optional.
  //
  // Parameters:
  //       sql - typically this is a static SQL INSERT or UPDATE 
  //       statement
  // Throws:
  //       TSQLException - if a database access error occurs, or 
  //       the  driver does not support batch statements

}

//___________________________________________________________________
void ODBCCallableStatement::ClearBatch()
{
  // Makes the set of commands in the current batch empty. This
  // method is optional.
  //
  // Throws:
  //       TSQLException - if a database access error occurs or 
  //       the driver does not support batch statements

}

//___________________________________________________________________
Int_t* ODBCCallableStatement::ExecuteBatch()
{
  // Submits a batch of commands to the database for execution.
  // This method is optional.
  //
  // Returns:
  //       an array of update counts containing one element for 
  //       each command in the batch. The array is ordered 
  //       according  to the order in which commands were inserted 
  //       into the  batch.
  //
  // Throws:
  //       TSQLException - if a database access error occurs or 
  //       the driver  does not support batch statements

  return 0;
}
