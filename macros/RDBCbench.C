// $Id: RDBCbench.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//     This file is part of the RDBC
//     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
/////////////////////////////////////////////////////////////////////
//
//  This is a sample implementation of the 
//  Transaction Processing Performance  Council Benchmark B 
//  coded in Java and ANSI SQL2. 
//
//  This is ROOT adaptation of JAVA code from  
//    http://www.worldserver.com/mm.mysql/
//
//
/////////////////////////////////////////////////////////////////////
//
// usage:
// 
// root[] gSystem->Load("libRDBC.so");    // load library
// root[] .L RDBCbench.C                  // load macro
// root[] bench(dsn,usr,psw,[option],[tpc],[clients]); // execute
//
//    dsn          Data Source Name
//    usr          user name
//    psw          password
//    tpc          transactions per client
//    clients      number of simultaneous clients
//
//    option string can contain:
//       -v           verbose error messages
//       -init        initialize the tables
// 
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLStatement.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLResultSet.h>
#include <TTimer.h>
#include <TStopwatch.h>
#include <TString.h>
#include <TRandom.h>
#include <RQ_OBJECT.h>
#include <TSystem.h>

int tps       =       1;  // the tps scaling factor: here it is 1 
int nbranches =       1;   // number of branches in 1 tps db 
int ntellers  =      10;   // number of tellers in  1 tps db
int naccounts =  100000;   // number of accounts in 1 tps db 
int nhistory  =  864000;   // number of history recs in 1 tps db
int gClients  =  10;       // default number of clients
int gTxnPerClient = 100;   // default number transactions per client

int failed_transactions = 0;
int transaction_count = 0;
Bool_t gVerbose = kFALSE;
long start_time = 0;    
enum {  kTELLER, kBRANCH, kACCOUNT };


/////////////////////////////////////////////////////////////////////
class RDBCBench 
{
friend class ClientThread;

RQ_OBJECT()

private: 
   TSQLConnection*   fConn;      // DataBase connection
   TList*            fListOfClients;   // client threads
   TStopwatch        fTimer; // benchmark timer

   void  StopThreads();
   void  CreateDatabase();
   void  DropDatabase();
   int   DoOne(int bid, int tid, int aid, int delta);
   int   GetRandomID(int type);
   int   GetRandomInt(int lo, int hi);

public:
   RDBCBench(TSQLConnection* conn, Bool_t init);
   ~RDBCBench() { delete fListOfClients; delete fConn; }

   void  ReportDone();  //*SIGNAL*
};

/////////////////////////////////////////////////////////////////////
class ClientThread
{
private:
   Int_t       fNtrans; //
   RDBCBench*  fParent; //
   TTimer*     fThread; //

public:
   ClientThread(int number_of_txns,RDBCBench* p);
   ~ClientThread() { }
   
   void Run();
   void Start() { fThread->Start(10); }  // 100 msec period 
   TTimer* GetThread() const { return fThread; }
};

//___________________________________________________________________
ClientThread::ClientThread(int number_of_txns,RDBCBench* p) 
{
   //

   fNtrans = number_of_txns;
   fThread = new TTimer(10);   
   fParent = p;
   fThread->Connect("Timeout()","ClientThread",this,"Run()");
}

//___________________________________________________________________
void ClientThread::Run()
{
   //

   while (fNtrans-- > 0) {
         
      int account = fParent->GetRandomID(kACCOUNT);
      int branch  = fParent->GetRandomID(kBRANCH);
      int teller  = fParent->GetRandomID(kTELLER);
      int delta   = fParent->GetRandomInt(0,1000);
                
      fParent->DoOne(account, branch, teller, delta);
      transaction_count++;
   }
   fParent->ReportDone();
}

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________  
RDBCBench::RDBCBench(TSQLConnection* conn, Bool_t init)
{
   //
   
   fConn = conn;
  
   if (init) {
      DropDatabase();
      printf("Initializing dataset...");
      CreateDatabase();
      printf("done.\n");
   }
     
   fListOfClients = new TList();
         
   for (int i = 0; i < gClients; i++) {
      ClientThread* client = new ClientThread(gTxnPerClient,this);
      fListOfClients->Add(client->GetThread());
      client->Start();
   }

   printf("\n* Starting Benchmark Run *\n");
   fTimer.Start(kTRUE);
}

//___________________________________________________________________    
void RDBCBench::ReportDone()
{
   //

   gClients--;
        
   if (gClients == 0) {
      fTimer.Stop();
      Double_t rtime = fTimer.RealTime();
      Double_t ctime = fTimer.CpuTime();
      
      printf("* Benchmark finished *");
      printf("\n\n* Benchmark Report *\n" );
      printf("-------------------------------------------------\n");
      printf("Time to execute %d  transactions: Real time %.3f, CPU time %.3f seconds\n", 
      transaction_count,rtime,ctime );
//      printf("Max/Min memory usage: " + MemoryWatcher.max + " / " + MemoryWatcher.min + " kb");
      printf("%d / %d  failed to complete.\n",failed_transactions,transaction_count);
      printf("Transaction rate: %f txn/sec (real time).\n\n",(transaction_count - failed_transactions) / rtime);
      StopThreads();
      Emit("ReportDone()");   // emit signal
      gSystem->Exit(0);
   }
}

//___________________________________________________________________  
void RDBCBench::StopThreads()
{
   //

   fListOfClients->Delete();
}

//___________________________________________________________________  
int RDBCBench::GetRandomInt(int lo, int hi)
{
   //

   int ret = 0;  
   ret = gRandom->Integer(hi-lo);
   ret += lo;
   return ret;
}

//___________________________________________________________________ 
int RDBCBench::GetRandomID(int type)
{
   //

   int min, max, num;
   max = min = num = 0;

   switch(type) {
   case kTELLER:
      num = ntellers*tps;
      break;
   case kBRANCH:
      num = nbranches*tps;
      break;
   case kACCOUNT:
      num = naccounts*tps;
      break;
   }
 
   max = min + num;
   return (GetRandomInt(min, max));
}

//___________________________________________________________________  
void RDBCBench::CreateDatabase()
{
   //  Creates and Initializes a scaled database.     
   //     

   TSQLStatement* stmt = fConn->CreateStatement();
    
   TString query;

   query = "CREATE TABLE branches (";
   query+= "Bid         INT NOT NULL, PRIMARY KEY(Bid), ";
   query+= "Bbalance    INT,";
   query+= "filler      CHAR(88))"; /* pad to 100 bytes */ 

   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();
         
   query = "CREATE TABLE tellers ( "; 
   query+= "Tid         INT NOT NULL, PRIMARY KEY(Tid),"; 
   query+= "Bid         INT,";
   query+= "Tbalance    INT,";
   query+= "filler      CHAR(84))"; /* pad to 100 bytes */
            
   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();
            
   query = "CREATE TABLE accounts ( ";
   query+= "Aid         INT NOT NULL, PRIMARY KEY(Aid), "; 
   query+= "Bid         INT, ";
   query+= "Abalance    INT, ";
   query+= "filler      CHAR(84))";      /* pad to 100 bytes */
            
   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();
            
   query = "CREATE TABLE history ( ";
   query+= "Tid         INT, "; 
   query+= "Bid         INT, ";
   query+= "Aid         INT, ";
   query+= "delta       INT, ";
   query+= "time        TIMESTAMP, ";
   query+= "filler      CHAR(22))"; /* pad to 50 bytes  */
            
   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();
        
   // prime database using TPC BM B scaling rules.  
   //  Note that for each branch and teller:
   //      branch_id = teller_id  / ntellers
   //      branch_id = account_id / naccounts
   //
  
   char str[1028];
 
   for (int i = 0; i < nbranches * tps; i++) {
      sprintf(str,"INSERT INTO branches(Bid,Bbalance) VALUES ( %d ,0)",i);
      stmt->ExecuteUpdate(str);
      stmt->ClearWarnings();
   }
   
   for (int i = 0; i < ntellers * tps; i++) {
      sprintf(str,"INSERT INTO tellers(Tid,Bid,Tbalance) VALUES (%d,%d,0)",i,i / ntellers);
      stmt->ExecuteUpdate(str);
      stmt->ClearWarnings();
   }
   
   for (int i = 0; i < naccounts*tps; i++) {
      sprintf(str,"INSERT INTO accounts(Aid,Bid,Abalance) VALUES (%d,%d,0)",i,i / naccounts);
      stmt->ExecuteUpdate(str);
      stmt->ClearWarnings();
   }

   delete stmt;
}

//___________________________________________________________________  
void RDBCBench::DropDatabase()
{
   //   Drops database.     
   //     

   TSQLStatement* stmt = fConn->CreateStatement();
  
   TSQL::UnsetHandler();   // 
   TString str;
   str = "DROP TABLE branches";
   stmt->ExecuteUpdate(str);

   str = "DROP TABLE tellers";
   stmt->ExecuteUpdate(str);

   str = "DROP TABLE accounts";
   stmt->ExecuteUpdate(str);

   str = "DROP TABLE history";
   stmt->ExecuteUpdate(str);

   delete stmt;
   TSQL::SetHandler("Catch(TSQLException*)");   // set default error handler
}

//___________________________________________________________________
int RDBCBench::DoOne(int aid, int bid, int tid, int delta)
{
   // Executes a single TPC BM B transaction

 
   TSQLStatement* stmt = fConn->CreateStatement();
              
   char query[1028];
 
   TSQL::SetHandler("CatchFailedTransaction(TSQLException*)");

   sprintf(query,"UPDATE accounts SET   Abalance = Abalance + %d WHERE   Aid = %d",delta,aid);
   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();
                   
   sprintf(query,"SELECT Abalance FROM  accounts  WHERE  Aid = %d",aid);
   TSQLResultSet* rs = stmt->ExecuteQuery(query);
   stmt->ClearWarnings();
                
   int aBalance = 0;
                
   while (rs->Next()) {
      aBalance = rs->GetInt(1);
   }

   delete stmt;
   stmt = fConn->CreateStatement();

   sprintf(query,"UPDATE tellers  SET  Tbalance = Tbalance + %d WHERE  Tid = %d",delta,tid);               
   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();

   sprintf(query,"UPDATE branches SET  Bbalance = Bbalance + %d WHERE  Bid = %d",delta,bid);
   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();

   sprintf(query,"INSERT INTO history(Tid, Bid, Aid, delta) VALUES (%d,%d,%d,%d)", tid,bid,aid,delta);      
   stmt->ExecuteUpdate(query);
   stmt->ClearWarnings();
   
   TSQL::SetHandler("Catch(TSQLException*)");   // set back default handler
   delete stmt;
   return aBalance;
}

//___________________________________________________________________
void CatchFailedTransaction(TSQLException* e)
{
   //
   TString str = e->GetMessage();

   if (gVerbose) {
      printf("Transaction failed: %s\n",str.Data());
//      e->printStackTrace();
   } 
   failed_transactions++;
}

/////////////////////////////////////////////////////////////////////
void bench( const TString& dsn="",const TString& usr="",
            const TString& psw="",const TString& option="",
            const TString& tpc_str="100", const TString& clients_str="10")
{
   // main program:  
   //       creates a 1-tps database:  i.e. 1 branch, 10 tellers,...
   //       runs one TPC BM B transaction
   //

   Bool_t initialize_dataset = kFALSE;
   failed_transactions = 0;
   transaction_count = 0;

   Int_t tpc = atoi(tpc_str.Data());
   Int_t clients = atoi(clients_str.Data());  

   if(clients>0) gClients = clients;
   if(tpc>0) gTxnPerClient = tpc;
 
   if(option.Contains("-init")) initialize_dataset = kTRUE;
   else if(option.Contains("-v")) gVerbose = kTRUE;

   printf("\n usage: obench(dsn,usr,psw,[option],[tpc],[clients])\n");
   printf("\n");
   printf("dsn          Data Source Name\n");
   printf("usr          user name\n");
   printf("psw          password\n");
   printf("tpc          transactions per client\n");
   printf("clients      number of simultaneous clients\n"); 
   printf("option string:\n");
   printf("-v           verbose error messages\n");
   printf("-init        initialize the tables\n");
      
   printf("\n*********************************************************\n");
   printf("* RDBCBench v1.0                                        *\n");
   printf("*********************************************************\n");
   printf("\n");
   printf("DSN:   %s\n",dsn.Data());
   printf("User:  %s\n",usr.Data());
   printf("\n");
   printf("Number of clients: %d\n",gClients);
   printf("Number of transactions per client: %d\n",gTxnPerClient);
   printf("\n");
      
   TSQL::SetHandler("Catch(TSQLException*)");   // set default error handler
   TSQLConnection* conn = gSQLDriverManager->GetConnection(dsn,usr,psw);
   RDBCBench* rdbc_bench = new RDBCBench(conn, initialize_dataset);
   // memory leak here: 
   //    rdbc_bench should be deleted after the end of test 
   //    It can be done by connecting "ReportDone()" signal to
   //    some garbage collector

   return;
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
#include <TApplication.h>

//---- Main program ------------------------------------------------------------

TROOT root("RDBCbench", "RDBC benchmark program");

int main(int argc, char **argv)
{
   if(argc < 3) {
      cerr << "Usage: " << argv[0] << " url username" << endl
           << "or     " << argv[0] << " url username password [option] [tpc] [clients]" << endl;
      return 0;
   }

   TApplication theApp("App", &argc, argv);

   gSystem->Load("libRDBC");
   gSystem->Load("./RDBCbench_C");

   if(argc==3) bench(argv[1],argv[2],"");
   else if(argc==4)  bench(argv[1],argv[2],argv[3]);
   else if(argc==5)  bench(argv[1],argv[2],argv[3],argv[4]);
   else if(argc==6)  bench(argv[1],argv[2],argv[3],argv[4],argv[5]);
   else if(argc==7)  bench(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);

   theApp.Run();
   return 0;
}
#endif
