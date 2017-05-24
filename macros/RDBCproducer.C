// $Id: RDBCproducer.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//     This file is part of the RDBC
//     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
/////////////////////////////////////////////////////////////////////
//
//  Histogram producer script. This script stores three histogram objects 
// in a datbase (a TH1F, a TH2F and a TProfile). It then fills, in an 
// infinite loop (so use ctrl-c to stop this script), the three histogram 
// objects with random numbers. Every 1000 fills  the objects are updated 
// in database.
// 
//  Use the oconsumer.C script to map this file and display the histograms.
//
//
//    REQUIREMENTS
//
//  You must have create table/drop table rights.
//
//////////////////////////////////////////////////////////////////////
//
// Usage:
//  
// root[] gSystem->Load("libRDBC.so");  // load library
// root[] .L RDBCproducer.C             // load macro
// root[] RDBCproducer(dsn,uid,pwd);    // execute the function from macro
//  

#ifndef __CINT__   
// g++ -c -W RDBCproducer.C -I$ROOTSYS/include

#include <TError.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TSQLDriverManager.h>
#include <TSQLConnection.h>
#include <TSQLResultSet.h>
#include <TSQLResultSetMetaData.h>
#include <TSQLPreparedStatement.h>

#endif // __CINT__

const TString tableName = "object_test";

// our histos
TH1F* hpx;
TH2F* hpxpy;
TProfile* hprof;

//__________________________________________________________________
void CreateStuff(TSQLConnection* con)
{
   // Creates the database objects needed
   
   // create our table
    TSQLStatement* stmt = con->CreateStatement();

    stmt->ExecuteUpdate( "delete from " + tableName);
    delete stmt;
   
   TSQLPreparedStatement* pstmt = con->PrepareStatement( 
                                       "insert into " +tableName+ 
                                       "(name,histos) values(?,?)" );
   
   hpx = new TH1F("hpx","This is the px distribution",100,-4,4);
   hpxpy = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
   hprof = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);
   
   // Set a fill color for the TH1F
   hpx->SetFillColor(48);
   
   Float_t px, py, pz;
   int ii = 0;
   
   // Endless loop filling histograms with random numbers
   for (ii = 0; ii < 1000; ii++) 
     {
       gRandom->Rannor(px,py);
       pz = px*px + py*py;
       hpx->Fill(px);
       hpxpy->Fill(px,py);
       hprof->Fill(px,pz);
     }

   pstmt->SetString(1,"hpx");
   pstmt->SetObject(2,hpx);
   pstmt->ExecuteUpdate();
   
   pstmt->SetString(1,"hpxpy");
   pstmt->SetObject(2,hpxpy);
   pstmt->ExecuteUpdate();
   
   pstmt->SetString(1,"hprof");
   pstmt->SetObject(2,hprof);
   pstmt->ExecuteUpdate();
   
   con->Commit();
   delete pstmt;
}

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
void Fill(TSQLConnection* con)
{
   // Endless loop filling histograms with random numbers
   
   TString str;

   TSQLStatement* stmt = 
     con->CreateStatement(kTYPE_SCROLL_SENSITIVE, kCONCUR_UPDATABLE);
   if (!stmt) return;
   
   con->Print("a");
   
   TSQLResultSet* rs;
         
   Float_t px, py, pz;
   int ii = 0;
   
   // Endless loop filling histograms with random numbers
   while (1) 
     {
       gRandom->Rannor(px,py);
       pz = px*px + py*py;
       hpx->Fill(px);
       hpxpy->Fill(px,py);
       hprof->Fill(px,pz);
       
       if ((ii % 1000) == 0) 
	 { // updates all objects in db 
	   rs = stmt->ExecuteQuery("select name, histos from "
				   + tableName);
         
	   if (!rs) break; // failed to select 
	   
	   while(rs->Next()) 
	     {
	       str = rs->GetString(1);
 	       if(str==hpx->GetName()) rs->UpdateObject(2,hpx);
 	       else if(str==hpxpy->GetName()) rs->UpdateObject(2,hpxpy);
 	       else if(str==hprof->GetName()) rs->UpdateObject(2,hprof);
	       rs->UpdateRow();
	     } 
	   con->Commit();
	   delete rs;        
	 }       
       ii++;
     }
   delete stmt;
}
      
//__________________________________________________________________
Int_t oproducer( const Text_t* dsn, 
                 const Text_t* usr, 
                 const Text_t* pwd )
{
   // 
   
   TSQLConnection* con;
   TString str;
  
   // set error handler
   TSQL::SetHandler("Catch(TSQLException*)");
  
   str = "dsn="; str += dsn; 
   str += "; uid="; str += usr; 
   
   // con = gSQLDriverManager->GetConnection(dsn,usr,pwd);
   con = gSQLDriverManager->GetConnection(str);

   gSystem->Sleep(5000);
   
   if(!con) return -1;  // failed to connect

   if( con->GetMetaData()->SupportsTransactions() ) {
      con->SetAutoCommit(kFALSE);
   }

   TSQL::UnsetHandler(); // ignore errors
   //  DropStuff(con);
   TSQL::SetHandler("Catch(TSQLException*)");
   CreateStuff(con);
   // Fill(con);        // endless loop here
   TSQL::UnsetHandler(); // ignore errors 
   // DropStuff(con);
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
