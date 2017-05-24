// $Id: RDBCscroll.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//     This file is part of the RDBC
//     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
/////////////////////////////////////////////////////////////////////
//
// This example based on libodbc++/tests/scroll.cpp 
//
///*
//  This should work with almost any almost-compliant database out there,
//  providing it supports scrollable cursors.
// */
//
//////////////////////////////////////////////////////////////////////
//
// Usage:
//  
// root[] gSystem->Load("libRDBC.so");    // load library
// root[] .L RDBCscroll.C                 // load macro
// root[] RDBCscroll(dsn,usr,psw);        // execute the function from macro
//  
//
//////////////////////////////////////////////////////////////////////
//
//    REQUIREMENTS
//
//      o You must have create table/drop table rights
//

#ifndef __CINT__   
// g++ -c -W RDBCscroll.C -I$ROOTSYS/include

#include <TError.h>
#include <TString.h>
#include <TSQLDriverManager.h>
#include <TSQLConnection.h>
#include <TSQLDatabaseMetaData.h>
#include <TSQLResultSet.h>
#include <TSQLResultSetMetaData.h>
#include <TSQLPreparedStatement.h>
#include <TROOT.h>
#include <TInterpreter.h>
#endif // __CINT__

const TString tableName = "odbcxx_test";
const Int_t tableRows = 1000;

//__________________________________________________________________
void CreateStuff(TSQLConnection* con)
{
   // create table
   
   TSQLStatement* stmt = con->CreateStatement();
   TString str = "create table ";
   str += tableName;
   str += "( id integer not null primary key, ";
   str += "name varchar(40) not null)";
   stmt->ExecuteUpdate(str.Data());
   
   str = "Table ";
   str += tableName;
   str += " created.";
   printf("%s \n",str.Data());
     
   delete stmt;
}

//___________________________________________________________________
void DropStuff(TSQLConnection* con)
{
   // Drops the database objects.
   
   TSQLStatement* stmt=con->CreateStatement();

   TString str = "drop table ";
   str += tableName;
   stmt->ExecuteUpdate(str.Data());
   
   str = "Dropped table ";
   str +=  tableName;
   printf("%s\n",str.Data());
   
   delete stmt;
}

//__________________________________________________________________
TString MakeName(Int_t n)
{
   TString str;
   char number[10];
   
   sprintf(number,"%d",n);
   str  = "This is row number ";
   str += number;
   return str;
}

//__________________________________________________________________
void Populate(TSQLConnection* con)
{
   //
   
   TString str = "insert into ";
   str += tableName;
   str += " (id,name) values(?,?)";
    
   TSQLPreparedStatement* pstmt = con->PrepareStatement(str.Data());

   for(int i=0; i<tableRows; i++) {
      pstmt->SetInt(1,i);
      pstmt->SetString(2,MakeName(i));
      pstmt->ExecuteUpdate();
   }
   
   delete pstmt;
   con->Commit();
   
   printf("Inserted %d rows\n",tableRows);
}

//__________________________________________________________________
void Compare(TSQLConnection* con)
{
   // decide whether we should use a scroll insensitive 
   // or a scroll sensitive cursor
  
   int rstype;
   int rsconc;
   TString str;
   TString name;
   
   TSQLDatabaseMetaData* md = con->GetMetaData();

   if(md->SupportsResultSetType(kTYPE_SCROLL_INSENSITIVE)) {
      rstype = kTYPE_SCROLL_INSENSITIVE;
   } else if(md->SupportsResultSetType(kTYPE_SCROLL_SENSITIVE)) {
      rstype = kTYPE_SCROLL_SENSITIVE;
   } else {
      printf("Skipping compare, data source does not support scrollable cursors\n"); 
      return;
   }

   if(md->SupportsResultSetConcurrency(rstype,kCONCUR_READ_ONLY)) {
      // this is all we need
      rsconc = kCONCUR_READ_ONLY;
   } else {
      rsconc = kCONCUR_UPDATABLE;
   }
  
   TSQLStatement* stmt = con->CreateStatement(rstype,rsconc);
   str = "select id,name from ";
   str += tableName;
   TSQLResultSet* rs = stmt->ExecuteQuery( str );
   if(!rs) return;
      
   Assert(rs->IsBeforeFirst());
   Assert(rs->First());
   Assert(!rs->IsBeforeFirst());
   Assert(rs->IsFirst());
    
   Assert(rs->Last());
   Assert(rs->IsLast());
   Assert(!rs->IsAfterLast());
   rs->AfterLast();
   Assert(rs->IsAfterLast());

   Assert(rs->Previous());
   Assert(rs->IsLast());
    
   printf("Positioned on the last row (%d)\n", rs->GetRow());
   
   int i = tableRows;
  
   do {
      i--;
      name = MakeName(i);
      Assert(rs->GetInt(1) == i);
      Assert(rs->GetString(2) == name);
   } while(rs->Previous());
   
   Assert(i==0);
   Assert(rs->IsBeforeFirst());
    
   printf("%d rows checked with expected values.\n",tableRows);;

   delete stmt; //will kill rs
}

//__________________________________________________________________
Int_t RDBCscroll( const Text_t* dsn, 
                  const Text_t* usr, 
                  const Text_t* pwd )
{
   //
 
   TSQLConnection* con;
   TString str;
  
   // connect to error handler
   TSQL::SetHandler("Throw(TSQLException*)"); 
        
   str = "Connecting to dsn="; str += dsn; 
   str += ", uid="; str += usr; 
        str += ", pwd="; str += pwd; 
   str += " ...";
   printf("%s\n",str.Data());
   
   con = gSQLDriverManager->GetConnection(dsn,usr,pwd);
   
   if(!con) return -1;  // failed to connect 

   printf("\t\t\t DONE.\n");

   // we don't want autocommit
   if( con->GetMetaData()->SupportsTransactions() ) {
      con->SetAutoCommit(kFALSE);
   }

   DropStuff(con);
   CreateStuff(con);    
   Populate(con);
   Compare(con);
   con->Commit();   
   
   DropStuff(con);
   con->Commit();
   con->Close();
   return 0;
}

//___________________________________________________________________
void Catch(TSQLException* e)
{
   // handle exceptions
   
   TString str = e->GetMessage(); 
   printf("SQL Error: %s\n",str.Data()); 
}
