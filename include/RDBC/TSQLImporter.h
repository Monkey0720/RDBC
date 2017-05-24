// $Id: TSQLImporter.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLImporter_h
#define RDBC_TSQLImporter_h

//
//  TSQLImporter: class used for data import
//

#ifndef ROOT_TQObject
#include <TQObject.h>
#endif

class TSQLImportClient;
class TSQLConnection;
class TSQLException;
/////////////////////////////////////////////////////////////////////
class TSQLImporter: public TQObject
{
protected:
   TSQLConnection* fConnection;  // where to import data (server)
   TSQLImportClient* fClient;    // what to import (client)

private:
   void  LoadCatalog(const TString& url);
   void  LoadTable(const TString& url);
   Int_t fStatus; // status of import procedure, fStatus < 400 status is OK

public:
   TSQLImporter();
   TSQLImporter(const TString& url,TSQLConnection* con) { Import(url,con); } 
   virtual ~TSQLImporter();   
   virtual Int_t  Import(const TString& url,TSQLConnection* con);
   virtual Bool_t IsValid() const;
   void Throw( TSQLException* e ) { Emit("Throw(TSQLException*)",(long)e); } //*SIGNAL*

ClassDef(TSQLImporter,0)// class used for data import into database
};

#endif // RDBC_TSQLImporter_h
