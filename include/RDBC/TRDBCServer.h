// $Id: TRDBCServer.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2001
//

//
//  TRDBCServer class provides access to db server via RDBC.

#ifndef ROOT_TSQLServer
#include <TSQLServer.h>
#endif

#ifndef RDBC_TSQLResultSet_h
#include <RDBC/TSQLResultSet.h>
#endif

class TSQLConnection;
/////////////////////////////////////////////////////////////////////
class TRDBCServer: public TSQLServer
{
private:
   TSQLConnection* fConnection;  // current connection
   TString  fDbName; // database product name

public:
   TRDBCServer(const char *db, const char *uid, const char *pw);
   ~TRDBCServer();
    
   void           Close(Option_t *opt="");
   TSQLResultSet* Query(const char *sql);
   Int_t          SelectDataBase(const char *dbname);
   TSQLResultSet* GetDataBases(const char *wild = 0);
   TSQLResultSet* GetTables(const char *dbname, const char *wild = 0);
   TSQLResultSet* GetColumns(const char *dbname, 
                             const char *table, 
                             const char *wild = 0);
   Int_t          CreateDataBase(const char *dbname);
   Int_t          DropDataBase(const char *dbname);
   Int_t          Reload();
   Int_t          Shutdown();
   const char*    ServerInfo();
   TSQLConnection* GetConnection() const { return fConnection; }

ClassDef(TRDBCServer,0)  // Connection to SQL server via RDBC
};
