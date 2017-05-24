// $Id: RDBCemp.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//     This file is part of the RDBC
//     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
/////////////////////////////////////////////////////////////////////
//
//      RDBCemp -  Demo macro
//
//    DESCRIPTION
//
//      This is a sample macro to demostrate how to retrieve data
//      from Oracle database and draw simple graphs.
//
//    REQUIREMENTS
//
//      o ORACLE demo table EMP. To create this demo table for MySQL  run: 
//
//          $ mysql test -u username -p < demobld.mysql 
//  
//////////////////////////////////////////////////////////////////////
//
// usage:
//  
// root[] gSystem->Load("libRDBC.so");    // load library
// root[] .L RDBCemp.C                    // load macro
// root[] RDBCemp(dsn,[usr],[pwd]);       // execute the function from macro
//  
//Begin_Html
/*
<img src="oemp.gif">
*/
//End_Html

#ifndef __CINT__
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
#include <TROOT.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TText.h>
#include <TStyle.h>
#include <TInterpreter.h>
#include <stdlib.h>
#endif

//___________________________________________________________________
void DrawGraph(TGraph* graph, TString* labels, const TString& xname,
               Float_t xmin,Float_t xmax,Float_t ymin,Float_t ymax)
{
   //
   
   gPad->SetFrameFillColor(18);    
   gStyle->SetOptStat(0);
   Int_t nrDivisions = graph->GetN();
 
   TH1F *frame = new TH1F(graph->GetName(),graph->GetTitle(),2,xmin,xmax+1);
   frame->SetMinimum(ymin);
   frame->SetMaximum(ymax);
   frame->SetLabelOffset(10);
   frame->SetLabelSize(0.0);
   frame->SetNdivisions(0);   
   frame->SetTitleOffset(1.1,"X");
   frame->SetXTitle((char*)xname.Data());
   frame->SetTitleOffset(1.1,"Y");
   frame->Draw();
    
   TText *t = new TText();
   t->SetTextAlign(22);
   t->SetTextSize(0.022);
 
   Float_t span = xmax-xmin;
   Float_t step = Float_t(span/nrDivisions);
   Float_t dist = TMath::Abs(ymax-ymin);

   for (Int_t j=0; j<nrDivisions;j++) {
      Float_t xpos = step+xmin+j*step;
      Float_t ypos = ymin-0.05*dist;
      t->DrawText(xpos,ypos,labels[j].Data());
   }
   
//-- Finally plot the graph
   graph->SetFillColor(rand()%149);
   graph->SetLineWidth(1.0);
   graph->Draw("B");  
}
      
//___________________________________________________________________
Int_t RDBCemp( const Text_t* dsn, 
               const Text_t* usr="scott", 
               const Text_t* pwd="tiger" )
{
   //
   
   TString str;

   // set error handler
   TSQL::SetHandler("Catch(TSQLException*)");
  
   str = "Connecting to dsn="; str += dsn; 
   str += ", uid="; str += usr; 
   str += ", pwd="; str += pwd; 
   str += " ...";
   printf("%s\n",str.Data());
   
   TSQLConnection* gConn = gSQLDriverManager->GetConnection(dsn,usr,pwd);
   if(!gConn) return -1;

   printf("\t\t\t DONE.\n");

   TSQLStatement* stmt = gConn->CreateStatement();
   TSQLResultSet* rs = stmt->ExecuteQuery("select * from EMP");
   if(!rs) return -1;
   
   const Int_t nrows = 14;
   Float_t sal[nrows];
   Float_t empno[nrows];
   Float_t mgr[nrows];
   Float_t comm[nrows];
   Float_t row[nrows];
   TString ename[nrows];
   
   for(int i=0; i<nrows; i++) {
      rs->Next();    // iterate rows
      empno[i] = rs->GetInt(1); // empno   
      ename[i] = rs->GetString(2); // ename
      mgr[i] = rs->GetFloat(4); // mgr
      sal[i] = rs->GetFloat(6); // sal
      comm[i] = rs->GetFloat(7); // comm
      row[i] = rs->GetRow();
   }
   
   TGraph* gr; // 
   TCanvas* c = new TCanvas("EMP","EMP table",900,600); 
   c->Divide(2,2);
  
   Float_t xmin = 0;
   Float_t xmax = nrows;
   Float_t ymin;
   Float_t ymax;
   Int_t ind;   

   c->cd(1);
   gPad->SetFillColor(38);
   gPad->SetToolTipText("This is EMPNO distribution");
   ind = TMath::LocMin(nrows,empno);
   ymin = 7000; //empno[ind];
   ind = TMath::LocMax(nrows,empno);
   ymax = empno[ind]*1.05;
   gr = new TGraph(nrows,row,empno);
   gr->SetName("empno");
   gr->SetTitle("empno");
   DrawGraph(gr,ename,"ename",xmin,xmax,ymin,ymax);
   
   c->cd(2);
   gPad->SetFillColor(30);
   gPad->SetToolTipText("This is SAL distribution");   
   ind = TMath::LocMin(nrows,sal);
   ymin = sal[ind]*0.8;
   ind = TMath::LocMax(nrows,sal);
   ymax = sal[ind]*1.05;
   gr = new TGraph(nrows,row,sal);
   gr->SetName("sal");
   gr->SetTitle("sal");
   DrawGraph(gr,ename,"sal",xmin,xmax,ymin,ymax);
   
   c->cd(3);
   gPad->SetFillColor(47);
   gPad->SetToolTipText("This is MGR distribution");   
   ind = TMath::LocMin(nrows,mgr);
   ymin = mgr[ind]*0.9;
   ymin = 7000;
   ind = TMath::LocMax(nrows,mgr);
   ymax = mgr[ind]*1.05;
   gr = new TGraph(nrows,row,mgr);
   gr->SetName("mgr");
   gr->SetTitle("mgr");
   DrawGraph(gr,ename,"ename",xmin,xmax,ymin,ymax);
   
   c->cd(4);
   gPad->SetFillColor(42);
   gPad->SetToolTipText("This is COMM distribution");   
   ind = TMath::LocMin(nrows,comm);
   ymin = comm[ind]*0.9;
   ind = TMath::LocMax(nrows,comm);
   ymax = comm[ind]*1.05;
   gr = new TGraph(nrows,row,comm);
   gr->SetName("comm");
   gr->SetTitle("comm");
   DrawGraph(gr,ename,"ename",xmin,xmax,ymin,ymax); 
//   gConn->Close();
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
#include <TApplication.h>

//---- Main program ------------------------------------------------------------

TROOT root("RDBCemp", "EMP table graphs");

int main(int argc, char **argv)
{
   if(argc!=3 && argc!=4) {
      cerr << "Usage: " << argv[0] << " url username" << endl
           << "or     " << argv[0] << " url username password" << endl;
      return 0;
   }

   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      cerr << argv[0] << "cannot run in batch mode" << endl;
      return 1;
   }

   gSystem->Load("libRDBC");
   Int_t  ret;

   if(argc==3) ret= RDBCemp(argv[1],argv[2],"");
   else  ret = RDBCemp(argv[1],argv[2],argv[3]);
   
   if(ret) return ret;    // failure

   theApp.Run();
   return 0;
}
#endif
