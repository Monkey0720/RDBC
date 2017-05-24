// $Id: MySQLCallableStatement.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 04/03/2001
//

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
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLCallableStatement.h>

ClassImpQ(TSQLCallableStatement)

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLCallableStatement::TSQLCallableStatement(TSQLConnection* con,void* imp):
                        TSQLPreparedStatement(con,imp)
   
{

}

//___________________________________________________________________
void TSQLCallableStatement::RegisterOutParameter(Int_t parameterIndex,
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

}

//___________________________________________________________________
void TSQLCallableStatement::RegisterOutParameter(Int_t parameterIndex,
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

}

//___________________________________________________________________
Bool_t TSQLCallableStatement::WasNull()
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
   return return_value;
}

//___________________________________________________________________
TString TSQLCallableStatement::GetString( Int_t parameterIndex )
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
   return str;
}

//___________________________________________________________________
Bool_t TSQLCallableStatement::GetBoolean( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
Char_t TSQLCallableStatement::GetByte( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
Short_t TSQLCallableStatement::GetShort( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
Int_t TSQLCallableStatement::GetInt( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
Long_t TSQLCallableStatement::GetLong( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
Float_t TSQLCallableStatement::GetFloat( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
Double_t TSQLCallableStatement::GetDouble( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
TArrayC TSQLCallableStatement::GetBytes( Int_t parameterIndex )
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
   return array;
}

//___________________________________________________________________
TSQLDate TSQLCallableStatement::GetDate( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
TSQLTime TSQLCallableStatement::GetTime( Int_t parameterIndex )
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
   return return_value;
}

//___________________________________________________________________
TSQLTimestamp TSQLCallableStatement::GetTimestamp( Int_t parameterIndex )
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
   return return_value;
}
