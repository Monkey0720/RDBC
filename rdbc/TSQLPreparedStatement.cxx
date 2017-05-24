// $Id: TSQLPreparedStatement.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
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


ClassImpQ(TSQLPreparedStatement)
