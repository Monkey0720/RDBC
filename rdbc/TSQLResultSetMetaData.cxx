// $Id: TSQLResultSetMetaData.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2000 
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

ClassImpQ(TSQLResultSetMetaData)

///////////////////////////////////////////////////////////////////// 
//___________________________________________________________________   
TSQLResultSetMetaData::TSQLResultSetMetaData( TSQLResultSet* rs,
                                              void* imp ):TSQL(imp)
{
   // ctor  
   
   fResultSet = rs;
}

//___________________________________________________________________
TSQLResultSetMetaData::~TSQLResultSetMetaData()
{
   // dtor  

   fResultSet = 0;
}
