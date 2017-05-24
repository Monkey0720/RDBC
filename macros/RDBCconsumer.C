// $Id: RDBCconsumer.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//     This file is part of the RDBC
//     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
/////////////////////////////////////////////////////////////////////
//
// Histogram consumer script. Create a canvas and 3 pads. Connect
// to database table, that was created by RDBCproducer.C.
// It reads the histograms from database and displays them
// in the pads.
//
//////////////////////////////////////////////////////////////////////
// 
//  1. Run another root session on another window(or computer)
//  2. Start the RDBCproducer.C script over there
//  3. Start RDBCconsumer script:
//  
//    root[] gSystem->Load("libRDBC.so");    // load library
//    root[] .L RDBCconsumer.C               // load macro
//    root[] RDBCconsumer(dsn,uid,pwd); 
//  
//Begin_Html
/*
<img src="oconsumer.gif">
*/
//End_Html
#ifndef __CINT__   
// g++ -c -W RDBCconsumer.C -I$ROOTSYS/include

#include <TError.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TROOT.h>

#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLResultSetMetaData.h>
#include <RDBC/TSQLPreparedStatement.h>

#endif // __CINT__

const TString tableName = "object_test";

// our histos
TH1F* hpx=0;
TH2F* hpxpy=0;
TProfile* hprof=0;

//__________________________________________________________________
void DropStuff(TSQLConnection* con)
{
   // Drops the database objects.
   
   TSQLStatement* stmt = con->CreateStatement();
   stmt->ExecuteUpdate("drop table " + tableName);
   printf("Dropped table %s\n",tableName.Data());
   delete stmt;
}
   
//__________________________________________________________________
Int_t RDBCconsumer( const Text_t* dsn, 
                 const Text_t* usr, 
                 const Text_t* pwd )
{
   // - connects to database 
   // - if table with histos exists ( implies that oproducer.C 
   //   script is running on another window/computer )
   //   periodically reads them out to display  
   
  TObject *to;
   TSQLConnection* con;
   TString str;
  
   // set error handler
   TSQL::SetHandler("Catch(TSQLException*)");
  
   str = "dsn="; str += dsn; 
   str += "; uid="; str += usr; 
   
   // con = gSQLDriverManager->GetConnection(dsn,usr,pwd);
   con = gSQLDriverManager->GetConnection(str);
   
   if(!con) return -1;  // failed to connect

   printf("\t\t\t DONE.\n");
   
   TCanvas *c1;
   TPad *pad1, *pad2, *pad3;      
   
   c1 = new TCanvas("c1","Reading objects from Database Example",200,10,700,780);
   pad1 = new TPad("pad1","This is pad1",0.02,0.52,0.98,0.98,21);
   pad2 = new TPad("pad2","This is pad2",0.02,0.02,0.48,0.48,21);
   pad3 = new TPad("pad3","This is pad3",0.52,0.02,0.98,0.48,21);
   pad1->Draw();
   pad2->Draw();
   pad3->Draw(); 
   
   TSQLStatement* stmt = con->CreateStatement(kTYPE_FORWARD_ONLY,
					      kCONCUR_READ_ONLY);

loop: // wait while oproducer.C script is not started
   TSQLResultSet* rs;
   rs = stmt->ExecuteQuery("select name, histos from " + tableName);
   
   if (!rs) {
     for(int i=0; i<50; i++) {
       gSystem->Sleep(100);   // sleep for 0.1 seconds
       gSystem->ProcessEvents();
     }
     printf("\n\n\t\t** producer not connected yet **\n\n");
     goto loop; 
   }   
           
   // Loop displaying the histograms. Once the producer stops this
   // script will break out of the loop.
   Double_t oldentries = 0;
   for (int j = 0; j < 100; j++) 
     {
       if(hpx) delete hpx;
       if(hpxpy) delete hpxpy;
       if(hprof) delete hprof;
       
       rs = stmt->ExecuteQuery("select name, histos from " + tableName);
       if (!rs) break;
       //       rs->BeforeFirst();
       while (rs->Next()) 
	 {
	   str = rs->GetString(1);
	   cout << str << endl;
	   to = rs->GetObject(2);
	   if(str=="hpx") hpx = (TH1F *)to;
	   else if(str=="hpxpy" ) hpxpy  = (TH2F *)to;
	   else if(str=="hprof") hprof  = (TProfile *)to;
	 }   
       if (hpx->GetEntries() == oldentries) break;
       oldentries = hpx->GetEntries();
       pad1->cd();
       if (hpx) hpx->Draw();
       pad2->cd();
       if (hprof) hprof->Draw();
       pad3->cd();
       if (hpxpy) hpxpy->Draw("cont");
       c1->Modified();
       c1->Update();
       
       gSystem->Sleep(100);   // sleep for 0.1 seconds
       //       if (gSystem->ProcessEvents()) break;
       delete rs;
     } 
   
   delete stmt;
   
   //   DropStuff(con);
   con->Commit();
   con->Close();
   return 0;
}
      
//___________________________________________________________________
void Catch(TSQLException* e)
{
   // handle exceptions
   
   TString str = e->GetMessage(); 
   printf("%s\n",str.Data());
}
