// $Id: RDBCfirst.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//     This file is part of the RDBC
//     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
/////////////////////////////////////////////////////////////////////
//
// This example corresponds to the following JDBC program
//
////////////////////////////////////////////////////////////////////////////////////
//
// A sample JDBC program.
//
// import                            java.sql.*; 
//
// //**************************************************************************** 
// //* JDBCExample                                          ;                     * 
// //**************************************************************************** 
//
//    public class
//    JDBCExample
//    {
//
//    //**************************************************************************** 
//    //* main                                                                      * 
//    //**************************************************************************** 
//
//    public static void
//    main( String args[] )
//    throws Exception
//    {
//       //    Find the class...
//       Class.forName( "weblogic.jdbc.oci.Driver" );
//
//       //    Open a connection...
//       Connection myConnection = DriverManager.getConnection(
//             "jdbc:weblogic:oracle:tcp-loopback.world", 
//             "scott", 
//             "tiger" );
//
//       //    Create a statement...
//       Statement myStatement = myConnection.createStatement();
//
//       //    Execute a query...
//       try
//       {
//          //    Execute the query...
//          myStatement.execute( "select * from emp" );
//
//          //    Get the result set...
//          ResultSet mySet = myStatement.getResultSet();
//
//          //    Advance to the next row...
//          while ( mySet.next() )
//          { 
//             //    Get the data...
//             int empno = mySet.getInt( "empno" );
//             String ename = mySet.getString( "ename" );
//             long salary = mySet.getLong( "sal" );
//
//             //    Print it all out...
//             System.out.println( Integer.toString( empno ) + " - " +
//                   ename + " - " + Integer.toString( sal ) );
//          } 
//       }
//       catch ( SQLException e )
//       {
//          System.out.println( "SQL Error: " + e.toString() );
//       }
//
//       //    Close everything up...
//       myStatement.close(); 
//       myConnection.close(); 
//    }
//
// } 
//
////////////////////////////////////////////////////////////////////////////////
//    REQUIREMENTS
//
//      o ORACLE database user SCOTT exists with demo table EMP
////////////////////////////////////////////////////////////////////////////////////
//
// Usage:
//  
// root[] gSystem->Load("libRDBC.so");    // load library
// root[] .L RDBCfirst.C++                // compile & load macro
// root[] RDBCfirst(dsn,[usr],[pwd]);     // execute function from macro
//
//

#include <TError.h>
#include <TString.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLDatabaseMetaData.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLResultSetMetaData.h>
#include <RDBC/TSQLPreparedStatement.h>
#include <RDBC/TSQLCallableStatement.h>
#include <RDBC/TSQLTypes.h>

//___________________________________________________________________
Int_t RDBCfirst(const Text_t* dsn,
               const Text_t* usr="scott",
               const Text_t* pwd="tiger")
{
   //    Open a connection...
   TSQLConnection* myConnection = TSQLDriverManager::GetConnection( dsn, 
                                                                    usr, 
                                                                    pwd );
   if(!myConnection) return -1;  // return on error
      
   //    Create a statement...
   TSQLStatement* myStatement = myConnection->CreateStatement();
   if(!myStatement) return -1;   // return on error
   
   //   Set exception handler 
   TSQL::SetHandler("Catch(TSQLException*)");
   
   //    Execute the query...
   Bool_t success = myStatement->Execute( "select * from EMP" );
   if(!success) return -1; // return on error
   
   //    Get the result set...
   TSQLResultSet* mySet = myStatement->GetResultSet();
   if(!mySet) return -1; // return on error

   //    Advance to the next row...
   while ( mySet->Next() ) { 
                            
      //    Get the data...
      int empno = mySet->GetInt( "empno" );
      TString ename = mySet->GetString( "ename" );
      long salary = mySet->GetLong( "sal" );

      //    Print it all out...
      printf( "%d - %s - %ld\n",empno,ename.Data(),salary );
   } 
      
   //    Close everything up...
   myConnection->Close();
   return 0;
}

//___________________________________________________________________
void Catch(TSQLException* e)
{
   // handle exceptions
   
   TString str = e->GetMessage();
   printf("SQL Error: %s\n",str.Data()); 
}


//////////////////////////// Main program ////////////////////////////////////
#ifdef STANDALONE

#include <TROOT.h>
#include <TSystem.h>
#include <iostream>

//---- Main program ------------------------------------------------------------

TROOT root("RDBCfirst", "My first program with RDBC");

int main(int argc, char **argv)
{
   if(argc!=3 && argc!=4) {
      cerr << "Usage: " << argv[0] << " url username" << endl
           << "or     " << argv[0] << " url username password" << endl;
      return 0;
   }

   gSystem->Load("libRDBC");
   Int_t ret;

   if(argc==3) ret=RDBCfirst(argv[1],argv[2],"");
   else  ret=RDBCfirst(argv[1],argv[2],argv[3]);

   return ret;
}
#endif
