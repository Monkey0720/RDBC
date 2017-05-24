
#include <TError.h>
#include <TString.h>
#include <TFile.h>
#include <TF1.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLDatabaseMetaData.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLResultSetMetaData.h>
#include <RDBC/TSQLPreparedStatement.h>
#include <RDBC/TSQLCallableStatement.h>
#include <RDBC/TSQLTypes.h>
extern "C" {
#include <stdlib.h>
};
//___________________________________________________________________
Int_t RDBCTestInt(const Text_t* dsn,
               const Text_t* usr="",
               const Text_t* pwd="")
{
   //    Open a connection...
   TSQLConnection* myConnection = NULL;
   if(usr!="" && pwd !=""){
     if(getenv("VERBOSE"))
       printf( "connecting with: dsn= %s usr=%s pwd=%s\n",dsn,usr,pwd);
     myConnection = TSQLDriverManager::GetConnection( dsn, usr,  pwd );
   } else{
     if(getenv("VERBOSE"))
       printf( "connecting with: dsn= %s \n",dsn);
     myConnection = TSQLDriverManager::GetConnection( dsn );
   }

  if(!myConnection) {
     printf( "failed to connect: dsn= %s usr=%s pwd=%s\n",dsn,usr,pwd);
     printf("exiting...\n");
     return -1;  // return on error
   }else
     printf("connected!!!\n");
   TSQLStatement* stmt = myConnection->CreateStatement();

   stmt->ExecuteUpdate( "drop table float_table" );
   stmt->ExecuteUpdate( "create table float_table (a_float float not null)" );
   TSQLPreparedStatement* pstmt = 
     myConnection->PrepareStatement("insert into float_table (a_float) values(?)");
   
   float some_float = 1.2345678;	
   pstmt->SetFloat(1,some_float);
   pstmt->ExecuteUpdate(""); 
   
   some_float = 3.3456789;

   pstmt->SetFloat(1,some_float);
   pstmt->ExecuteUpdate(""); 
   
   TSQLResultSet* rs = stmt->ExecuteQuery("select a_float from float_table order by a_float");
   rs->Next(); // goto the first row
   float my_float = rs->GetFloat(1); 
   printf ("retrieved: %f\n",my_float);	
   rs->Next(); // goto the next row
   my_float = rs->GetFloat(1); 
   printf ("retrieved: %f\n",my_float);	


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

TROOT root("RDBCTestInt","Test RDBC TSQLDriverManager and TSQLConnection");

int main(int argc, char **argv)
{

   gSystem->Load("libRDBC");
   Int_t ret = -1;

   if(argc < 2 || argc > 4){
     printf ("usage: RDBCTestInt dsn [usr] [password]\n");
     return ret;
   }

   if(argc==2) 
     ret=RDBCTestInt(argv[1]);
   if(argc==3) 
     ret=RDBCTestInt(argv[1],argv[2]);
   if(argc==4)
     ret=RDBCTestInt(argv[1],argv[2],argv[3]);

   return ret;
}
#endif
