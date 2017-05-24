// $Id: MySQLPreparedStatement.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2000 
//

/////////////////////////////////////////////////////////////////////
//
// An object that represents a precompiled SQL statement. 
//
// A SQL statement is pre-compiled and stored in a 
// TSQLPreparedStatement object. This object can then be used to 
// efficiently  TSQLPreparedStatement::Execute() this statement 
// multiple times. 
//
// Note: The TSQLPreparedStatement::SetXXX methods for setting IN 
//    parameter values must specify types that are compatible with the 
//    defined SQL type of the input parameter. For instance, if the 
//    IN parameter has SQL type integer, then the method 
//    TSQLPreparedStatement::SetInt() should be used. 
//
// Example of TSQLPreparedStatement setting a parameter; 
// con is an active connection 
//
//
//  TSQLPreparedStatement* pstmt = 
//       con->PrepareStatement("UPDATE EMPLOYEES SET SALARY = ?  
//                               WHERE ID = ?");
//
//   pstmt->SetInt(2, 110592);
// 
//
// See also: 
//      TSQLConnection::PrepareStatement(const TString&), 
//      TSQLResultSet TSQLStatement TSQLCallableStatement
//
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLPreparedStatement.h>
#include <RDBC/TSQLResultSet.h>

#include <iostream>
#include <sstream>
#include <TList.h>


ClassImpQ(TSQLPreparedStatement)

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLPreparedStatement::TSQLPreparedStatement(TSQLConnection* con,
                                    void* imp):TSQLStatement(con,imp)
   
{
   // ctor.
}

//___________________________________________________________________
void TSQLPreparedStatement::SetNull( Int_t parameterIndex,Int_t sqlType )
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
   

}

//___________________________________________________________________
void TSQLPreparedStatement::SetBoolean( Int_t parameterIndex,Bool_t x )
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
 
}

//___________________________________________________________________
void TSQLPreparedStatement::SetByte( Int_t parameterIndex,Char_t x )
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
   
}

//___________________________________________________________________
void TSQLPreparedStatement::SetShort( Int_t parameterIndex,Short_t x )
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetInt( Int_t parameterIndex,Int_t x )
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetLong( Int_t parameterIndex,Long_t x )
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetFloat( Int_t parameterIndex,Float_t x )
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetDouble( Int_t parameterIndex,Double_t x )
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetString( Int_t parameterIndex, 
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetBytes( Int_t parameterIndex,
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetDate( Int_t parameterIndex,
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetTime( Int_t parameterIndex,
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetTimestamp( Int_t parameterIndex,
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetAsciiStream( Int_t parameterIndex,
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetBinaryStream( Int_t parameterIndex,
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

}

//___________________________________________________________________
void TSQLPreparedStatement::SetObject( Int_t parameterIndex,TObject* x )
{
   // Sets the designated parameter to the given ROOT object
   //
   //   Parameters:
   //         parameterIndex - the first parameter is 1, 
   //                          the second is 2, ...
   //         x - the ROOT object
   //   Throws:
   //         TSQLException - if a database access error occurs

   TBuffer *b = new TBuffer(TBuffer::kWrite);
   b->WriteObject(x);
   SetBinaryStream(parameterIndex,b,b->BufferSize());
   b->DetachBuffer();
   delete b;
}

//___________________________________________________________________
void TSQLPreparedStatement::ClearParameters()
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

}

//___________________________________________________________________
Bool_t TSQLPreparedStatement::Execute()
{
   // Executes any kind of SQL statement. Some prepared statements
   // return multiple results; the TSQLPreparedStatement::Execute() 
   // method handles these complex statements as well as the simpler 
   // form of statements handled by  TSQLPreparedStatement::ExecuteQuery()
   // and TSQLPreparedStatement::ExecuteUpdate()
   //
   //    Throws:
   //          TSQLException - if a database access error occurs
   // 
   //    Return kTRUE if the result is a TSQLResultSet, 
   //           kFALSE if it's an  update count or unknown.
   //
   //    See Also: 
   //          TSQLStatement::Execute(const TString&)

}

//___________________________________________________________________
Int_t TSQLPreparedStatement::ExecuteUpdate()
{
   // Executes the SQL INSERT, UPDATE or DELETE statement in this
   // TSQLPreparedStatement object. In addition, SQL statements that
   // return nothing, such as SQL DDL statements, can be executed.
   // 
   //   Returns:
   //         either the row count for INSERT, UPDATE or DELETE
   //         statements; or 0 for SQL statements that return nothing
   //   Throws:
   //         TSQLException - if a database access error occurs

}

//___________________________________________________________________
TSQLResultSet* TSQLPreparedStatement::ExecuteQuery()
{
   //   Executes the SQL query in this TSQLPreparedStatement object 
   //   and returns the result set generated by the query.
   //
   //   Returns:
   //         a TSQLResultSet that contains the data produced by the
   //         query; never null
   //   Throws:
   //        TSQLException - if a database access error occurs

}
