// $Id: TSQLImporter.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 20/03/2001
//

////////////////////////////////////////////////////////////////////
//
// TSQLImporter
//
////////////////////////////////////////////////////////////////////
#include <RDBC/TSQLImporter.h>
#include <RDBC/TSQLImportClient.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLUrl.h>
#include <RDBC/TSQLStatement.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLDriverManager.h>

ClassImpQ(TSQLImporter)
/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLImporter::TSQLImporter():TQObject()
{
   // ctor

   fConnection = 0;
   fClient = 0;
   fStatus = 400; // not OK
}

//___________________________________________________________________
void TSQLImporter::LoadTable(const TString& url)
{
   //

   fStatus = 400;
   TSQLStatement* stmt = 0;

   fClient = new TSQLImportClient(url);

   // catch exceptions from client and re-emmit exception
   fClient->Init();

   if(!fClient->IsValid()) {  // failed to create client
      fStatus = fClient->GetStatus();
      TSQLException* e = fClient->GetException();
       if(e) {
	 // Create a copy of the exception; the owning client is about to be destroyed.
        TSQLException* clientException = new TSQLException();
        *clientException = *e;
        Throw(clientException);
       }
      if(fClient) delete fClient;
      fClient = 0;
      return;
   }

   TString cols = fClient->GetColumns();   // read column names,types
   TString table = fClient->GetTableName();
   TString file = fClient->GetLocalFile();

   TString query;
   TString query1;
   Int_t update_count;

   stmt = fConnection->CreateStatement();

   if(!stmt) {
      Throw(new TSQLException("Failed to create SQL statement","",HTTP_NOT_ACCEPTABLE));
      if(fClient) delete fClient;
      Destroyed();
      fClient = 0;
      fStatus = HTTP_NOT_ACCEPTABLE;
      return;
   }

   query1 =  "CREATE TEMPORARY TABLE ";
   query1 += table + "(" + cols + ")";

   update_count = stmt->ExecuteUpdate(query1);

   query =  "LOAD DATA ";
   query += fClient->GetLocal() + " INFILE '";
   query += file;
   query += "' INTO TABLE ";
   query += table;
   query += " FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '";
   query += '\"';
   query += "'";
//   query += " ESCAPED BY '\\'";
//   query += " LINES TERMINATED BY '\n'";

   if(fClient->GetSkipLines()) {
      query += " IGNORE ";
      query += Form("%d LINES",fClient->GetSkipLines());
   }

   update_count = stmt->ExecuteUpdate(query);

   if(!update_count) {  //
      Throw(new TSQLException(Form("Wrong format:\n%s\n%s",query1.Data(),query.Data()),"",HTTP_NOT_ACCEPTABLE));
      Destroyed();
      SafeDelete(fClient);
      fStatus = HTTP_NOT_ACCEPTABLE;
   }

   fStatus = HTTP_OK;
   if(stmt) delete stmt;
   return;
}

//___________________________________________________________________
void TSQLImporter::LoadCatalog(const TString& url)
{
   //

   LoadTable(url);   // load

   if( (fStatus!=HTTP_OK) || !fClient ) {
      return;
   }

   TSQLStatement* stmt = fConnection->CreateStatement();
   TString table = fClient->GetTableName();
   TString query;

//   query = "ALTER TABLE " + table;
//   query += " ADD table_name varchar(64)";

   query = "SELECT * FROM " + table;

   TSQLResultSet* rs = stmt->ExecuteQuery(query);

   if(!rs) {
      fStatus = HTTP_NOT_ACCEPTABLE;
      return;
   } 

   if(fClient) {
      delete fClient;
      fClient = 0;
   }

   while(rs->Next()) {
      table = rs->GetString(1); // first column is URL/file
      LoadTable(table);
   }

   if(fClient) {
      delete fClient;
      fClient = 0;
   }

   fStatus = HTTP_OK; 
   if(stmt) delete stmt;
   return;   
}

//___________________________________________________________________
Int_t TSQLImporter::Import(const TString& url,TSQLConnection* con)
{
   // import data from url to con
 
   fStatus = 400;

   if(!con) {
      fConnection = 0;
      //Throw(new TSQLException("Connection not defined","",HTTP_FORBIDDEN));
      return fStatus = HTTP_FORBIDDEN;
   }

   fConnection = con;
   TString ext = strrchr(url.Data(),'.');  // get file extention

   if( (ext==".cat") || (ext==".db") ) LoadCatalog(url);
   else LoadTable(url); 

   return fStatus = HTTP_OK;
}

//___________________________________________________________________
Bool_t TSQLImporter::IsValid() const
{
   // 

   return (fStatus < 400);
}

//___________________________________________________________________
TSQLImporter::~TSQLImporter()
{
   // dtor

   SafeDelete(fClient); 
}

