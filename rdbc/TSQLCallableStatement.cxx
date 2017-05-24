// $Id: TSQLCallableStatement.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2000 
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
//       - I failed to use with OpenLink ODBC driver to Oracle
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLCallableStatement.h>

ClassImpQ(TSQLCallableStatement)

