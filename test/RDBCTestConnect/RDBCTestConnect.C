
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
extern "C" {
#include <stdlib.h>
};
//___________________________________________________________________
Int_t RDBCTestConnect(const Text_t* dsn,
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

TROOT root("RDBCTestConnect","Test RDBC TSQLDriverManager and TSQLConnection");

int main(int argc, char **argv)
{

   gSystem->Load("libRDBC");
   Int_t ret = -1;

   if(argc < 2 || argc > 4){
     printf ("usage: RDBCTestConnect dsn [usr] [password]\n");
     return ret;
   }

   if(argc==2) 
     ret=RDBCTestConnect(argv[1]);
   if(argc==3) 
     ret=RDBCTestConnect(argv[1],argv[2]);
   if(argc==4)
     ret=RDBCTestConnect(argv[1],argv[2],argv[3]);

   return ret;
}
#endif
