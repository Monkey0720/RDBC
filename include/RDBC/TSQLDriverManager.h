// $Id: TSQLDriverManager.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLDriverManager_h
#define RDBC_TSQLDriverManager_h

//
//  TSQLDriverManager - "plug-in" loader
//

#ifndef RDBC_TSQL_h
#include <RDBC/TSQL.h>
#endif

#ifndef ROOT_TObject
#include "TObject.h"
#endif

class TSQLConnection;
/////////////////////////////////////////////////////////////////////
class TSQLDriverManager: public TObject, public TSQL
{
friend TSQLDriverManager* InitGlobalDriverManager();

private:
   TSQLDriverManager(const TSQLDriverManager&)     // forbid
     : TObject(), TSQL() {}  // forbid
   void operator=(const TSQLDriverManager&) {}     // forbid   
   TSQLDriverManager();    // forbid

public:
   ~TSQLDriverManager();
   static TSQLConnection* GetConnection( const TString& connectString ); 
   static TSQLConnection* GetConnection( const TString& dsn, 
                                         const TString& user, 
                                         const TString& password="");
   static Int_t   GetLoginTimeout();
   static void    SetLoginTimeout( Int_t seconds );
   static void    Shutdown();
   static TList*  GetDataSources();
   static TList*  GetDrivers();  
   static TList*  GetConnections();
   const char*    GetName() const { return "gSQLDriverManager"; }

ClassDef(TSQLDriverManager,0)//The basic service for managing a set of drivers. 
};

R__EXTERN TSQLDriverManager* gSQLDriverManager;

#endif // RDBC_TSQLDriverManager_h                 
