// $Id: TSQLDriverManager.cxx,v 1.3 2012/09/07 16:00:43 bbannier Exp $
//*-- Author : Valeriy Onuchin 14/02/2000
//

////////////////////////////////////////////////////////////////////
//
// The basic service for managing a set of drivers and providing 
// access to database. To create connection use GetConnection() method. 
// The connection string can be specified by DSN or URL.
// The default format of URL string is:
//
// protocol:[subprotocol:][driver:]//host[:port]/[database][?options]
//
// where:
//    protocol - DBMS type , e.g. mysql,oracle, etc.
//    subprotocol - driver type , currently: "odbc"
//    driver - driver name ( it could be alias name defined in /etc/odbcinst ) 
//             or path to the driver lib, e.g. myodbc - /usr/local/lib/libmyodbc.so
//
//             If absolute pathname is not specified, driver manager will try to
//             find "lib[driver].so" in the following directories"
//                /usr/lib,
//                /usr/local/lib,
//                $ROOTSYS/lib, 
//
//    host - host name or IP address of database server
//    port - port number
//    database - name of database
//    options - string key=value's separated by ';' or '&'     
//
//  Example: "mysql:odbc:myodbc://myhost:3306/test?Trace=Yes;TraceFile=qq.log"
//
//  You can do the same via "mysql://myhost/test?Trace=Yes;TraceFile=qq.log"
//  The missing parts of connect string are set to default values.
// 
//    For MySQL these values are:
//
//    subprotcol = odbc
//    driver = /usr/local/lib/libmyodbc.so 
//    port = 3306
//    database = test
//    options = "Trace=;TraceFile="  
//
//
// See also: 
//     TSQLConnection TSQLDriverInfo 
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLUrl.h>
#include <TList.h>
#include <TSystem.h>
#include <TClassTable.h>
#include <iostream>
#include <TROOT.h>
#include <TString.h>
#include <stdlib.h>
#include <TSysEvtHandler.h>
#include <TApplication.h>
#include <TInterpreter.h>
#include <RDBC/TSQLImporter.h>

ClassImpQ(TSQLDriverManager)
/////////////////////////////////////////////////////////////////////
TList* gDataSources;                // list of data sources
TList* gDrivers;                    // list of drivers
TList* gConnections;                // list of connections
TString gRDBClibPath;               // path to RDBC library
TSignalHandler* gSIGTERMhandler;    // SIGTERM handler
TSignalHandler* gSIGQUIThandler;    // SIGQUIT handler
Bool_t gHandlersInitiated = kFALSE; // kTRUE if handlers are initiated
/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLDriverManager* InitGlobalDriverManager()
{

   if (!gSQLDriverManager) gSQLDriverManager = new TSQLDriverManager();
   return gSQLDriverManager;
}

TSQLDriverManager* gSQLDriverManager = InitGlobalDriverManager(); // 
/////////////////////////////////////////////////////////////////////
//
// Handler of "program terminate" signals == SIGTERM, SIGQUIT
//
/////////////////////////////////////////////////////////////////////
class SIGTERMhandler: public TSignalHandler
{
public:
   SIGTERMhandler(ESignals sig) : TSignalHandler(sig, kFALSE) { }

   Bool_t  Notify() {
      delete gSQLDriverManager;
      gApplication->Terminate(0);
      return kTRUE;
   }
};

/////////////////////////////////////////////////////////////////////
//
// TSQLini is used for "Dynamic DSN connections" 
//
/////////////////////////////////////////////////////////////////////
class TSQLini: public TObject 
{
private:
   TString  fOldIni;    // old   $ODBCINI 
   TString  fNewIni;    // temp  $ODBCINI 
  
public:
   TSQLini(TSQLUrl* url);
   ~TSQLini();
};

//___________________________________________________________________
TSQLini::TSQLini(TSQLUrl* url)
{
   // - save current $ODBCINI
   // - redifine $ODBCINI to temporary file
   // - write TSQLUrl to temporary $ODBCINI file

   TString homedir = "/tmp"; // gSystem->HomeDirectory();   //
   fOldIni = gSystem->Getenv("ODBCINI");
   fNewIni = homedir + Form("/.odbcini.%d",gSystem->GetPid()); // temporary
   gSystem->Setenv("ODBCINI",fNewIni.Data());

   url->Print(fNewIni.Data()); 
}

//___________________________________________________________________
TSQLini::~TSQLini()
{
   // deallocate resources

   gSystem->Setenv("ODBCINI",fOldIni.Data());   // restore old $ODBCINI

   if(gSystem->Unlink(fNewIni.Data())) { // failed to delete file 
      TString message  = "Failed to delete file: ";
      message +=  fNewIni;
      Warning("Restore ODBCINI",message.Data());   
   }
}

TSQLini* gODBCini = 0;
/////////////////////////////////////////////////////////////////////
Bool_t InitRDBCpath()
{
   // define pathname to RDBC library
   
   gRDBClibPath = gInterpreter->GetSharedLibs();
   int i1 = gRDBClibPath.Index("libRDBC.");
   if ( i1 < 0 ) return kFALSE;
   TString str = gRDBClibPath(0,i1);
   int i2 = str.Last(' ');
   str = gRDBClibPath(i2+1,gRDBClibPath.Length()-i2-1);
   i2 = str.Index(' ');
   
   if(i2 == kNPOS) gRDBClibPath = str;
   else gRDBClibPath=str(0,i2-1);
   
   gRDBClibPath = gSystem->DirName(gRDBClibPath.Data()); 
   return !gRDBClibPath.IsNull();
}

static Bool_t dummy = InitRDBCpath();
/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
void InitHandlers()
{
   // All handlers are initated just before the first connection, but
   // after allocation of main resources ( gSystem, gROOT etc. )

   if(gHandlersInitiated) return;

   if(gSystem) gSystem->RemoveOnExit(gSQLDriverManager); // remove on exit
   if(gInterpreter) gInterpreter->SaveGlobalsContext();  // proof against gROOT->Reset()

   // catch program termination signals SIGTERM ( aka 'kill -15 pid', 'kill -TERM pid')
   // and SIGQUIT ( ^\ ) to remove gSQLDriverManager ( deallocate resources )
 
   gSIGTERMhandler = new SIGTERMhandler(kSigTermination);
   gSIGQUIThandler = new SIGTERMhandler(kSigQuit); 
   if(gSystem) gSystem->AddSignalHandler(gSIGTERMhandler);
   if(gSystem) gSystem->AddSignalHandler(gSIGQUIThandler);

   if(TClassTable::GetDict("TSQL")) {
      InitRDBCpath();
   }

   gHandlersInitiated = kTRUE;
}

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLDriverManager::TSQLDriverManager():TSQL(0)
{
   // contructor. 

   if(!gSQLDriverManager) {
      gSQLDriverManager = this;
      gDataSources  = new TList(); // list of data sources
      gDrivers = new TList();     // list of drivers
      gConnections = new TList();     // list of drivers
   } else {
      Destroyed();   //
   } 
}
   
//___________________________________________________________________
TSQLDriverManager::~TSQLDriverManager()
{
   // destructor
   //
   //  - deallocate/close all connections
   //  - free global enviroment  
   
   if(gDebug) Warning("~TSQLDriverManager()","Shutting down DriverManager");
   Shutdown();
   Destroyed();   //   
}

//___________________________________________________________________
void TSQLDriverManager::Shutdown()
{
   // Should be called before an application is to exit

   gConnections->Delete();
   gDrivers->Delete();
   gDataSources->Delete();
   delete gDrivers;
   delete gDataSources;
   delete gConnections;
   gDrivers = 0;
   gDataSources = 0;
   gConnections = 0;

   if(gODBCini) delete gODBCini;
   gSQLDriverManager = 0;      
}

//___________________________________________________________________
TList* TSQLDriverManager::GetDrivers()
{
   // Fetch a list of all of currently loaded drivers
   // to which the current caller has access. 

   if(!TClassTable::GetDict("ODBCConnection"))  {
      gSQLDriverManager->Throw(new TSQLException(
            "TSQLDriverManager::GetDrivers:RDBC-ODBC driver not loaded"));
      return 0;
   }

   return gDrivers = (TList*)gROOT->ProcessLineFast(
      Form("ODBCConnection::RefreshDrivers((TList*)%ul)",gDrivers));
}

//___________________________________________________________________
TList* TSQLDriverManager::GetDataSources()
{
   // Fetch a list of of all available data sources

   if(!TClassTable::GetDict("ODBCConnection"))  {
      gSQLDriverManager->Throw(new TSQLException(
         "TSQLDriverManager::GetDataSources:RDBC-ODBC driver not loaded"));
      return 0;
   }
 
   return gDataSources = (TList*)gROOT->ProcessLineFast(
      Form("ODBCConnection::RefreshDataSources((TList*)%ul)",gDataSources));
}

////// EnterPassword() noecho mode for password typing ///////////////
#ifndef WIN32
// #ifdef HAVE_TERMIOS_H  
//   #include        <termios.h>
//   #define TERMIO  struct termios
// #else
//   #ifdef HAVE_TERMIO_H 
   #include        <termio.h>
   #define TERMIO  struct termio
// #endif   

//___________________________________________________________________
TString EnterPassword()
{
   // - type promt to "Enter password:" 
   // - when password string is entered it's not echoed on the screen 

   TERMIO org,tmp;
   TString pswd;
   const int bufsiz = 128;
   char passtr[bufsiz];

   if(isatty(fileno(stdout))) {
      fputs("Enter password:",stdout);
      fflush(stdout);
   }
   ioctl(fileno(stdin), (int) TCGETA, &org);
   tmp = org;
   tmp.c_lflag &= ~(ECHO | ISIG | ICANON);
   tmp.c_cc[VMIN] = 1;
   tmp.c_cc[VTIME]= 0;
   ioctl(fileno(stdin),(int) TCSETA, &tmp);

   fgets(passtr,bufsiz,stdin); // get password string ( mysql use it's own func )

   pswd = passtr;
   pswd.Chop();   // chop last char ( either eol,'\n','\r' )
   ioctl(fileno(stdin),(int) TCSETA, &org); 

   if (isatty(fileno(stdout))) fputc('\n',stdout);
   return pswd;
}

#else // WIN32 case
 #include <conio.h>

//___________________________________________________________________
TString EnterPassword()
{
   // code stolen from mysql libmysl/get_password.c ... not tested

   char to[80];
   char *pos=to,*end = to+sizeof(to)-1;
   int i = 0;

   fprintf(stdout,"Enter password: ");

   for (;;) {
      char tmp;
      tmp = _getch();

      if (tmp == '\b' || (int) tmp == 127) {
         if (pos != to) {
            _cputs("\b \b");
            pos--;
            continue;
         }
      }
      if (tmp == '\n' || tmp == '\r' || tmp == 3) break;
      if (iscntrl(tmp) || pos == end) continue;      
      _cputs("*");
      *(pos++) = tmp;
   }
  
   while (pos != to && isspace(pos[-1]) == ' ') pos--; // Allow dummy space at end

   *pos=0;
   _cputs("\n");
  
   return to;
}
#endif  //  WIN32
//////////////////// end of EnterPassword //////////////////////////

//___________________________________________________________________
TSQLConnection* TSQLDriverManager::GetConnection( const TString& url, 
                                                  const TString& user, 
                                                  const TString& password )
{
   // Attempts to establish a connection to the given Data Source 
   // Name (DSN). The TSQLDriverManager attempts to select an appropriate 
   // driver  from  the set of registered drivers.
   //
   // Parameters:
   //    url   - DataSourceName string
   //    user  - the database user on whose behalf the TSQLConnection
   //            is being made
   //    password - the user's password
   //
   // Returns:
   //    a TSQLConnection to the data source
   //
   // Throws:
   //    TSQLException - if a database access error occurs
   //  

   if(!gHandlersInitiated) InitHandlers();
   gSQLDriverManager->GetWarnings()->Delete(); // clear previous warnings

   TSQLConnection* con = 0;  
   TSQLUrl sqlurl(url);

   if(!sqlurl.IsValid()) {
      gSQLDriverManager->Throw(new TSQLException(
             Form("TSQLDriverManager::GetConnection:URL %s is not valid:-\n"
                  "    Protocol : %s  SubProtocol: %s Driver: %s Port: %d",
                  url.Data(), 
                  sqlurl.GetProtocol().Data(),
                  sqlurl.GetSubProtocol().Data(),
                  sqlurl.GetDriver().Data(),
                  sqlurl.GetPort()
             )));
      return 0;
   }

   TString pswd = password;

   if(pswd.IsNull()) pswd = EnterPassword();

   // anchor is used to specify import table/file/tree for opened connection
   TString anchor = sqlurl.GetAnchor();

   if( sqlurl.GetProtocol()=="file" ||
       sqlurl.GetProtocol()=="http" ||
       sqlurl.GetProtocol()=="ftp" ) {
      anchor = sqlurl;
      sqlurl = "mysql:odbc://localhost/test";   //  localhost mysql as proxy server
   }

   sqlurl.AddOption("User=" + user);   // 
   con = (TSQLConnection*)gSQLDriverManager->GetConnections()->FindObject(sqlurl.GetUrl().Data());

   if(con) {         // connection exists
      con->AddReference();
      goto exit;
   }

   if( (sqlurl.GetProtocol()=="odbc" || sqlurl.GetSubProtocol()=="odbc") ) {   
      if(!TClassTable::GetDict("ODBCConnection"))  {
         gSQLDriverManager->Throw(new TSQLException(
               "TSQLDriverManager::GetConnection:RDBC-ODBC driver not loaded"));
         return 0;
      }

      TString dsn = sqlurl.GetDSN();

      if(gODBCini) { delete gODBCini; gODBCini = 0; }  
      if(sqlurl.IsDynamicDSN()) gODBCini = new TSQLini(&sqlurl); // 
   
      con = (TSQLConnection*) gROOT->ProcessLineFast(Form(
         "new ODBCConnection(\"%s\",\"%s\",\"%s\")",dsn.Data(),user.Data(),pswd.Data()));
 
      if(gODBCini) { delete gODBCini; gODBCini = 0; }      
      goto exit;
   }

exit:
   if ( con && !con->fImp ) {
      delete con;
      con = 0;
      return 0;
   }
   if(!con) return con;
   TSQLImporter* importer = 0;

   if(!anchor.IsNull()) {
  
      importer = new TSQLImporter();
      importer->Connect("Throw(TSQLException*)","TSQLDriverManager",gSQLDriverManager,"Throw(TSQLException*)");
      importer->Import(anchor,con);

      if(!importer->IsValid() || gSQLDriverManager->GetWarnings()->GetSize()) {
         delete importer;
         delete con;
         return 0;
      }
   }

   if(con->fImp) {            // fImp==0 in case of error 
      if(con->References()==1) { 
         gSQLDriverManager->GetConnections()->Add(con);
         con->SetURL(sqlurl.GetUrl()); // == set the name of connection
      }
   } else {
      delete con;     
      return 0;
   }
   if(importer) delete importer;

   return con;   
}

//___________________________________________________________________
TSQLConnection* TSQLDriverManager::GetConnection( const TString& connectString )
{
   // Attempts to establish a connection to the given database 
   // by specified connection string. This string is simply
   // a series of keyword/value pairs, searated by semicolons,
   // that contains information used to establish the connection.
   // The TSQLDriverManager attempts to select an appropriate driver
   // from the set of registered drivers.
   //
   // Parameters:
   //    connectString usually something like:
   //                   "dsn=minos;uid=scott;pwd=tiger"
   // Returns:
   //    a connection to the data source
   // Throws:
   //    TSQLException - if a database access error occurs

   if(!gHandlersInitiated) InitHandlers();
   gSQLDriverManager->GetWarnings()->Delete(); // clear previous warnings

   if(gODBCini) { delete gODBCini; gODBCini = 0; }  
   TSQLConnection* con = 0;
   con = (TSQLConnection*)gSQLDriverManager->GetConnections()->FindObject(connectString.Data());

   if(con) {         // connection exists
      con->AddReference();
      goto exit;
   }

   if(!TClassTable::GetDict("ODBCConnection"))  {
      gSQLDriverManager->Throw(new TSQLException(
         "TSQLDriverManager::GetConnection:RDBC-ODBC driver not loaded"));
      return 0;
   }

   con = (TSQLConnection*) gROOT->ProcessLineFast(Form(
         "new ODBCConnection(\"%s\")",connectString.Data()));
exit:
   if(con->fImp) {            // fImp==0 in case of error 
      if(con->References()==1) { 
         gSQLDriverManager->GetConnections()->Add(con);
         con->SetURL(connectString.Data());
      }
   } else {
      delete con;     
      return 0;
   } 
   return con;    
}

//___________________________________________________________________
void TSQLDriverManager::SetLoginTimeout( Int_t seconds )
{
   // Sets the maximum time in seconds that a driver will wait while
   // attempting to connect to a database. Set to 0 to disable. 
   //
   // Parameters:
   //       seconds - the login time limit in seconds

   if(TClassTable::GetDict("ODBCConnection")) 
      gROOT->ProcessLineFast(Form("ODBCConnection::SetLoginTimeout(%d)",seconds));
}

//___________________________________________________________________
Int_t TSQLDriverManager::GetLoginTimeout()
{
   // Gets the maximum time in seconds that a driver can wait when
   // attempting to log in to a database. 
   //
   // Returns:
   //       the driver login time limit in seconds, or 0 if disabled.

   return TClassTable::GetDict("ODBCConnection") ?
            (Int_t) gROOT->ProcessLineFast("ODBCConnection::GetLoginTimeout()") : 0;
}

//___________________________________________________________________
TList* TSQLDriverManager::GetConnections()
{
   // Fetch a list of all of currently opened connections. 

   return gConnections;
}
