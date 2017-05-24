// $Id: TSQLUrl.cxx,v 1.2 2009/10/09 12:28:48 pinkenbu Exp $
//*-- Author : Valeriy Onuchin 14/02/2000
//

////////////////////////////////////////////////////////////////////
//
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
//     TSQLConnection TSQLDriverManager 
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLUrl.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLDriverInfo.h>
#include <iostream>
#include <TString.h>
#include <TList.h>
#include <TSystem.h>
#include <TNamed.h>
#include <TRefCnt.h>
#include <cstdlib>

ClassImpQ(TSQLUrl)

/////////////////////////////////////////////////////////////////////
//
//  TSQLUrlParser - auxilary class used for parsing of URL string
//     It has several subclasses, i.e. OracleParser,MySQLParser etc.
//     which handle database specific cases.  
//
//
/////////////////////////////////////////////////////////////////////
class TSQLUrlParser: public TQObject, public TRefCnt
{
friend class TSQLUrl;

protected:

   enum EErrorBits {
      kErrProtocol   = BIT(0),   // wrong protocol 
      kErrSubProtocol= BIT(1),   // wrong subprotocol 
      kErrDriver     = BIT(2),   // wrong or not existent driver
      kErrHost       = BIT(3),   // wrong host
      kErrPort       = BIT(4),   // wrong port
      kErrFile       = BIT(5),   // wrong file/database 
      kErrDatabase   = BIT(5),   // wrong file/database 
      kErrAnchor     = BIT(6),   // wrong anchor 
      kErrOption     = BIT(7)    // wrong option
   };

   TString  fDriver;    // driver name  
   TString  fFullUrl;   // full url string 
   TString  fDSN;       // DataSourceName 
   TString  fProtocol;  // protocol: oracle, mysql, etc
   TString  fSubProtocol;  // sub-protocol:odbc, thin, etc
   TString  fHost;      // remote host
   TString  fFile;      // remote object
   TString  fAnchor;    // anchor in object
   TString  fOptions;   // options (after ?)
   TString  fDescription; // description
   TString  fPort;      // port through which to contact remote server  
   Int_t    fStatus;    // O if url is valid , non-zero otherwise
   Bool_t   fDynamicDSN;   // kTRUE if DSN is created at dynamically
         
protected:
   virtual TString DefaultProtocol() { return "odbc"; }
   virtual TString DefaultSubProtocol() { return "odbc"; }
   virtual TString DefaultDriver() { return ""; }
   virtual TString DefaultHost() { return "localhost"; }
   virtual TString DefaultDatabase() { return ""; }
   virtual TString DefaultPort() { return ""; }
   virtual TString DefaultOptions() { return ""; }
   virtual TString DefaultAnchor() { return ""; }
   virtual TString DefaultDescription() { return ""; }
   virtual TString DefaultDriverPath() { return ""; }

   virtual Bool_t  ValidProtocol(const TString&);
   virtual Bool_t  ValidSubProtocol(const TString&);
   virtual Bool_t  ValidDriver(const TString&);
   virtual Bool_t  ValidHost(const TString&);
   virtual Bool_t  ValidDatabase(const TString&);
   virtual Bool_t  ValidPort(const TString&);
   virtual Bool_t  ValidOption(const TString&);
   virtual Bool_t  ValidAnchor(const TString&);
   virtual void    Validate();
   virtual void    SetDefaults();
   virtual void    SetFullUrl();
public:
   TSQLUrlParser():TQObject(),TRefCnt() { }
   TSQLUrlParser(const TString& url);
   TSQLUrlParser& operator=(const TSQLUrlParser& url);
   void Throw( TSQLException* e ) { Emit("Throw(TSQLException*)",(long)e); } //*SIGNAL*
   virtual Bool_t IsValid() { return fStatus==0; }
};

/////////////////////////////////////////////////////////////////////
class Tokenizer 
{
private:
   TString fString;     // string to tokenize
   TString fDelim;      // delimter
   Ssiz_t  fDL;         // delim length 
   
public:
   ~Tokenizer() {}

   Tokenizer(const TString& str,const TString& delim) 
   { 
      SetString(str);
      SetDelim(delim); 
   } 

   Tokenizer(const TString& str) 
   { 
      SetString(str);
   } 

   TString GetString() const { return fString; }
   void SetString(const TString& str) 
   { 
      // force new TString w/ separate reference counting
      // so internal Tokenizer actions (effective modification of fData)
      // by Forw/Back don't disturb const TString& passed in to ctor
      const char* strCopy = str.Data();
      fString = TString(strCopy);
   }

   void SetDelim(const TString& delim)
   {
      fDelim = delim;
      fDL = fDelim.Length();
   }

   TString Skip(Int_t n)
   {
      fString = fString(n,fString.Length()-n);
      return fString;
   }

   TString Before()
   {
      // Returns string before delim

      Int_t idx = fString.First(fDelim.Data());

      if(idx!=kNPOS) {
         return fString(0,idx);
      } else {
         return fString;
      }
   }

   TString After()
   {
      // Returns string after delim

      Int_t idx = fString.First(fDelim.Data());

      if(idx!=kNPOS) {
         return fString(idx+fDL,fString.Length());
      } else {
         return fString;
      }
   }
   
   TString Forw(char term=0)
   {
      //  - Looks for fDelim string which is before "term char"
      // - Returns string before fDelim. 
      // - Cuts fString after fDelim.

      TString str;

      Int_t idx1 = fString.First(term);

      if(idx1!=kNPOS) {
         str = fString(0,idx1);
      } else {
         str = fString;
         idx1 = 0;
      }
      
      Int_t idx = str.Index(fDelim.Data());

      if(idx!=kNPOS) {
         fString = fString(idx+fDL,fString.Length());
         return str(0,idx);
      } else {
         return fString;
      }
   }   

   TString Back(char term=0)
   {
      // - Looks for fDelim string which is after "term char"
      // - Returns string after fDelim. 
      // - Cuts fString before fDelim.

      TString str;

      Int_t idx1 = fString.Last(term);

      if(idx1!=kNPOS && idx1<fString.Length()) {
         str = fString(idx1,fString.Length());
      } else {
         str = fString;
         idx1 = 0;
      }

      Int_t idx = str.Index(fDelim.Data());

      if(idx!=kNPOS) {
         fString = fString(0,idx+idx1);
         return str(idx+fDL,str.Length());
      } else {
         return "";
      }
   }
};

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLUrlParser::TSQLUrlParser(const TString& url):TQObject(),TRefCnt()
{
   //

   TString str;
   fDynamicDSN = kTRUE;
   fStatus = 0;

   Tokenizer parser(url);

   // protocol
   parser.SetDelim(":");
   fProtocol = parser.Forw('/');

   if(fProtocol.IsNull()) {   // dsn-like  
      fFullUrl = url;
      fDSN = url;
      fProtocol = DefaultProtocol();
      fDynamicDSN = kFALSE;
      return;
   }

   fProtocol.ToLower();

   // subprotocol
   parser.SetDelim(":");       // 
   fSubProtocol = parser.Forw('/');   // 
   fSubProtocol.ToLower();

   parser.SetDelim("://");
   fDriver= parser.Forw('@');

   if(fDriver.BeginsWith("//")) {  
      fDriver="";
      parser.Skip(2);
   }

   parser.SetDelim("?");
   fOptions = parser.Back(':');

   parser.SetDelim("#");
   fAnchor = parser.Back(':');   //

   if(fAnchor.IsNull()) fAnchor = DefaultAnchor(); 

   // host + port + file
   parser.SetDelim("/");
   fFile = parser.Back(':');

   if(parser.GetString().Contains(":")) {
      parser.SetDelim(":"); 
      fPort = parser.After();
      fHost = parser.Before();
   } else {
      fHost = parser.GetString();
   }

   if(fProtocol=="file") { // small fix
      if( fHost != "localhost") {
      // See if "host" is actually an environmental variable
        const char* host  = 0;
        if ( fHost.BeginsWith("$") ) host = getenv(fHost.Data()+1);
        if ( host ) { 
          fFile.Prepend("/");
          fFile.Prepend(host);
        }
        else        fFile = "/" + fHost + fFile;
        fHost = "localhost";
      }
   }
}

//___________________________________________________________________
void TSQLUrlParser::SetFullUrl()
{
   //

   if(fDynamicDSN) { //
      fFullUrl = fProtocol + ":" + fSubProtocol + ":";
      fFullUrl += fDriver + "://";
      if(!(fHost=="localhost" && fPort.IsNull())) fFullUrl += fHost; 
      fFullUrl += fPort.IsNull() ? "" : ":"; 
      fFullUrl += fPort;
      if((fHost=="localhost"&& fPort.IsNull())) fFullUrl +=  "/";
      fFullUrl += fFile; 
      fFullUrl += fOptions.IsNull() ? "" : "?";
      fFullUrl += fOptions;
   }
}

//___________________________________________________________________
void TSQLUrlParser::SetDefaults()
{
   // default settings

   if(fSubProtocol.IsNull()) fSubProtocol = DefaultSubProtocol();
   if(fDriver.IsNull()) fDriver = DefaultDriver();
   if(fOptions.IsNull()) fOptions = DefaultOptions();
   if(fDescription.IsNull()) fDescription = DefaultDescription();
   if(fFile.IsNull()) fFile = DefaultDatabase();
   if(fPort.IsNull()) fPort = DefaultPort();
   if(fHost.IsNull()) fHost = DefaultHost();
}

//___________________________________________________________________
void TSQLUrlParser::Validate()
{
   //

   TString str;

   if(!ValidProtocol(fProtocol)) {
      str = "Wrong format of protocol: ";
      str += fProtocol;
      fProtocol="none";
      Throw(new TSQLException(str,""));
      fStatus = fStatus | kErrProtocol;
   }

   if(!ValidSubProtocol(fSubProtocol)) {
      str = "Wrong format of sub-protocol: ";
      str += fSubProtocol;
      fSubProtocol="none";
      Throw(new TSQLException(str,""));
      fStatus = fStatus | kErrSubProtocol;
   }

   if(!ValidDriver(fDriver)) {
      str = "Wrong Driver : ";
      str += fDriver;
      fDriver="none";
      Throw(new TSQLException(str,""));
      fStatus = fStatus | kErrDriver;
   }
 
   if(!ValidHost(fHost)) {
      str = "Wrong format of host: ";
      str += fHost;
      fHost="none";
      Throw(new TSQLException(str,""));
      fStatus = fStatus | kErrHost;
   } 

   if(!ValidDatabase(fFile)) {
      str = "Wrong format of file/database: ";
      str += fFile;
      fFile="none";
      Throw(new TSQLException(str,""));
      fStatus = fStatus | kErrFile;
   }

   if(!ValidPort(fPort)) {
      str = "Wrong port: ";
      str += fPort;
      fPort="none";
      Throw(new TSQLException(str,""));
      fStatus = fStatus | kErrPort;
   }

   if(!ValidOption(fOptions)) {
      str = "Wrong format of option: ";
      str += fOptions;
      fOptions="none";
      Throw(new TSQLException(str,""));
      fStatus = fStatus | kErrOption;
   }

   if(fDSN.IsNull()) { // 
//    unixODBC imposes a 32 char limit on this (SQL_MAX_DSN_LENGTH)
//    make each name distinct to avoid ODBCINI caching issues
      static Int_t nConnect = 0;
      fDSN = fProtocol + Form("%d-%d",gSystem->GetPid(),++nConnect);
   }
}

//___________________________________________________________________
TSQLUrlParser& TSQLUrlParser::operator=(const TSQLUrlParser& url)
{
   //
 
   if (this != &url) {
      fDriver = url.fDriver; 
      fFullUrl = url.fFullUrl;
      fDSN = url.fDSN;
      fProtocol = url.fProtocol;
      fSubProtocol = url.fSubProtocol;
      fHost = url.fHost;
      fFile = url.fFile;
      fAnchor = url.fAnchor;
      fOptions = url.fOptions;
      fDescription = url.fDescription;
      fPort = url.fPort;
      fStatus = url.fStatus;
      fDynamicDSN = url.fDynamicDSN;
   }
   return *this;
}     

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidProtocol(const TString& prot) 
{ 
   //

   const char* str = prot.Data();
 
   for (Ssiz_t i = 0; i < prot.Length(); i++) {
      if( !isalnum(str[i]) && str[i]!='-' ) return kFALSE;
   }
   return kTRUE; 
}

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidSubProtocol(const TString& prot) 
{ 
   //
   const char* str = prot.Data();
 
   for (Ssiz_t i = 0; i < prot.Length(); i++) {
      if( !isalnum(str[i]) && str[i]!='-' ) return kFALSE;
   }
   return kTRUE; 
}

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidDriver(const TString& drvname) 
{ 
   //

   Char_t* chrStr =0;
   TList* li = TSQLDriverManager::GetDrivers();
   TSQLDriverInfo* drv;
   TString str;
   TNamed* obj;
   TString key;
   TString value;
   TString so = "."; so += gSystem->GetSoExt();
   TString str2;
   TString name = chrStr = gSystem->ExpandPathName(drvname.Data());
   delete [] chrStr;

   if(li) {
      TIter next(li);

      while((drv=(TSQLDriverInfo*)next())) {
         str = drv->GetDescription();
         if(str==name) return kTRUE;
         li=drv->GetAttributes();
         TIter nexta(li);
      
         while((obj=(TNamed*)nexta())) {
            str=obj->GetName();
            str.ReplaceAll(" ","");
            str.ReplaceAll("\t","");
            Int_t i = str.Index("=");
            key = str(0,i);
            value = str(i+1,str.Length()-i-1);
            key.ToLower();
            if(key=="driver" && value==name) return kTRUE;
         }
      } 
   }

   Bool_t af = gSystem->IsAbsoluteFileName(name.Data());

   if(!af) {
      if(!name.BeginsWith("lib")) name = "lib" + name;
      if(!name.EndsWith(so.Data())) name += so;

      str2 = chrStr = gSystem->ConcatFileName(DefaultDriverPath().Data(),name.Data());
      delete [] chrStr;
      if(!gSystem->AccessPathName(str2.Data()))  { fDriver = str2; return kTRUE; }

      str2 = chrStr = gSystem->ConcatFileName("/usr/lib",name.Data());
      delete [] chrStr;
      if(!gSystem->AccessPathName(str2.Data()))  { fDriver = str2; return kTRUE; }

      str2 = chrStr = gSystem->ConcatFileName("/usr/local/lib",name.Data());
      delete [] chrStr;
      if(!gSystem->AccessPathName(str2.Data()))   { fDriver = str2; return kTRUE; }

      TString rootsys = chrStr = gSystem->ExpandPathName("$ROOTSYS/lib");
      delete [] chrStr;

      str2 = chrStr = gSystem->ConcatFileName(rootsys.Data(),name.Data());
      delete [] chrStr;
      if(!gSystem->AccessPathName(str2.Data()))   { fDriver = str2; return kTRUE; }

      // check driver at $LD_LIBRARY_PATH
      TString ldlib = gSystem->Getenv("LD_LIBRARY_PATH");

      if(!ldlib.IsNull()) {
         Int_t sep = 0;
         Int_t prev = 0;
         TString sublib;

         while(1) {
            sep = ldlib.Index(":",prev);
            if(sep==kNPOS) break;
            sublib = ldlib(prev,sep-prev);


            if( (sublib=="/usr/lib") ||
                (sublib=="/usr/local/lib") ||
                (sublib==rootsys) ) {
               prev += sublib.Length()+1;  
               continue;
            }
            prev = sep+1;

            str2 = chrStr = gSystem->ConcatFileName(sublib.Data(),name.Data());
            delete [] chrStr;
            if(!gSystem->AccessPathName(str2.Data()))   { fDriver = str2; return kTRUE; }
         }
         sublib = ldlib(prev,ldlib.Length()-prev);
         str2 = chrStr = gSystem->ConcatFileName(sublib.Data(),name.Data());
         delete [] chrStr;
         if(!gSystem->AccessPathName(str2.Data()))   { fDriver = str2; return kTRUE; }        
      }

      return kFALSE;
   } else {
      if(!gSystem->AccessPathName(name.Data())) { 
         fDriver = name; 
         return kTRUE; 
      } else {
         fDriver = name;
         return kFALSE;  
      }
   }
   return kFALSE; 
}

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidHost(const TString& host) 
{ 
   //
   const char* str = host.Data();
   char prevch;

   for (Ssiz_t i = 0; i < host.Length(); i++) {
      if( (!isalnum(str[i]) && str[i]!='.' && str[i]!='-') || 
         (str[i]=='.' && prevch=='.') ) return kFALSE;
      prevch=str[i];
   }
   return kTRUE; 
}

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidDatabase(const TString& db) 
{ 
   //
  
   const char* str = db.Data();

   for (Ssiz_t i = 0; i < db.Length(); i++) {
      if( !isascii(str[i]) ) return kFALSE;
   }
   return kTRUE; 
}

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidPort(const TString& port) 
{ 
   //

   const char* str = port.Data();
   return atoi(str)!=0; 
}

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidOption(const TString& /* opt */) 
{ 
   //
   return kTRUE; 
}

//___________________________________________________________________
Bool_t TSQLUrlParser::ValidAnchor(const TString&) 
{ 
   //
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////
class OracleParser: public TSQLUrlParser
{
protected:
   TString fTNS;  // TNS name 

   virtual TString DefaultProtocol() { return "oracle"; }
   virtual TString DefaultSubProtocol() { return "odbc"; }
   virtual TString DefaultDriver();
   virtual TString DefaultHost() { return "localhost"; }
   virtual TString DefaultDatabase() { return "ORCL"; }
   virtual TString DefaultPort() { return "1521"; }
   virtual TString DefaultOptions();
   virtual TString DefaultDescription() { return "Connection via OpenLink ODBC driver to Oracle database"; }
public:
   OracleParser(const TString& url);
};

//___________________________________________________________________
OracleParser::OracleParser(const TString& url):TSQLUrlParser(url) 
{
   //

   if(fHost.IsNull()) {
      if(fFile.BeginsWith("@")) {
         Tokenizer parser(fFile);
         fFile = parser.Skip(1);
         if(fFile.Contains(":")) {
            parser.SetDelim(":");
            fTNS = "";
            fHost = parser.Forw(0);
            fPort = parser.Forw(0);
            fFile = parser.GetString();            
         } else {
            fTNS = parser.GetString();
            fHost = "localhost";
            fFile = fTNS;
         } 
      }
   }
}

//___________________________________________________________________
TString OracleParser::DefaultDriver()
{
   //

   if(fSubProtocol=="odbc") {
      return "oplodbc";
   } else {
      return "";
   }
}

//___________________________________________________________________
TString OracleParser::DefaultOptions()
{
   //

   if(fSubProtocol=="odbc") {
      return "ServerType=Oracle 8;"
             "Protocol=TCP/IP;"
             "ReadOnly=Yes;"
             "FetchBufferSize=100;"
             "UserName=scott;"
             "Password=tiger;";
   } else {
      return "";
   }
}

/////////////////////////////////////////////////////////////////////
class MySQLParser: public TSQLUrlParser
{
protected:
   virtual TString DefaultProtocol() { return "mysql"; }
   virtual TString DefaultSubProtocol() { return "odbc"; }
   virtual TString DefaultDriver();
   virtual TString DefaultHost() { return "localhost"; }
   virtual TString DefaultDatabase() { return "test"; }
   virtual TString DefaultPort() { return "3306"; }
   virtual TString DefaultOptions() { return "Trace=No;TraceFile=;"; }
   virtual TString DefaultDescription() { return "Connection via MyODBC driver to MySQL database"; }
//   virtual TString DefaultDriverPath() { return ""; }

public:
   MySQLParser(const TString& url):TSQLUrlParser(url) { }
};

//___________________________________________________________________
TString MySQLParser::DefaultDriver()
{
   //

   if(fSubProtocol=="odbc") {
      return "myodbc";
   } else {
      return "";
   }
}

/////////////////////////////////////////////////////////////////////
class PostgreSQLParser: public TSQLUrlParser
{
protected:
   virtual TString DefaultProtocol() { return "postgresql"; }
   virtual TString DefaultSubProtocol() { return "odbc"; }
   virtual TString DefaultDriver();
   virtual TString DefaultHost() { return "localhost"; }
   virtual TString DefaultDatabase() { return "test"; }
   virtual TString DefaultPort() { return "5432"; }
   virtual TString DefaultOptions() { return ""; }
   virtual TString DefaultDescription() { return "Connection via psqlodbc driver to PostgreSQL database"; }

public:
   PostgreSQLParser(const TString& url);
};

//___________________________________________________________________
PostgreSQLParser::PostgreSQLParser(const TString& url):TSQLUrlParser(url)  
{
}

//___________________________________________________________________
TString PostgreSQLParser::DefaultDriver()
{
  return "psqlodbc";
}

/////////////////////////////////////////////////////////////////////
class ConnectStringParser: public TSQLUrlParser
{
public:
   ConnectStringParser(const TString& fullurl) {
      Int_t i1 = fullurl.Index("=");
      Int_t i2 = fullurl.Index(";",i1);
      fFullUrl = fullurl;
      if(i2<0) {
         Destroyed();
         return;
      }
      fDSN = fullurl(i1+1,i2-i1-1); 
      fOptions = fullurl(i2+1,fullurl.Length()-i2-1);
      fDynamicDSN = kFALSE;
   }
};

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLUrl::TSQLUrl( const TString& url ):TObject(),TQObject()
{
   //

   TString fullurl(url.Data());
   fullurl.ReplaceAll(" ","");    // remove blanks 

   if (fullurl.BeginsWith("oracle:",TString::kIgnoreCase))  { fParser = new OracleParser(fullurl); goto exit; }
   if (fullurl.BeginsWith("mysql:",TString::kIgnoreCase))   { fParser = new MySQLParser(fullurl);  goto exit; }
   if (fullurl.BeginsWith("postgresql:",TString::kIgnoreCase)) { fParser = new PostgreSQLParser(fullurl); goto exit; }
//if (fullurl.BeginsWith("msql:",TString::kIgnoreCase))     { fParser = new mSQLParser(fullurl); goto exit; }
//if (fullurl.BeginsWith("informix:",TString::kIgnoreCase)) { fParser = new InformixParser(fullurl); goto exit; }
   if (fullurl.BeginsWith("dsn=",TString::kIgnoreCase))     { fParser = new ConnectStringParser(fullurl);  goto exit; }
   if(fullurl.BeginsWith("/")) fullurl = "file:" + fullurl;

// default:
   fParser = new TSQLUrlParser(fullurl);

exit:
   if(fParser->fDynamicDSN) {
      fParser->SetDefaults();
      fParser->Validate();
      fParser->SetFullUrl();

      if(!fParser->IsValid()) {
         //Throw(TSQLException*);
//      delete fParser;
   
      } else fParser->AddReference(); //update counter of references
   }
}

//___________________________________________________________________
TSQLUrl::TSQLUrl( const TString& dsn, const TString& description):
   TObject(),TQObject()
{
   // ctor.
   
   fParser = new TSQLUrlParser(dsn);
   fParser->fDescription = description;
   fParser->AddReference(); //update counter of references
}
   
//___________________________________________________________________
TSQLUrl::~TSQLUrl()
{
   // dtor.

   fParser->RemoveReference();  // decrease references
   if( fParser->References()<=0 ) delete fParser;
}

//___________________________________________________________________
TSQLUrl::TSQLUrl(const TSQLUrl& url) : TObject(), TQObject()
{
   //

   fParser = url.fParser; 
   fParser->AddReference(); //update counter of references 
}

//___________________________________________________________________
TSQLUrl& TSQLUrl::operator=(const TSQLUrl& url)
{
   //

   if(this != &url) {
      url.fParser->AddReference();
      fParser = url.fParser;
   }
   return *this;  
}
//___________________________________________________________________
TString TSQLUrl::GetDSN() const
{
   // Return DataSourceName

   return fParser->fDSN;  //
}

//___________________________________________________________________
TString TSQLUrl::GetUrl() const  
{
   // Return full URL string. 

   return fParser->fFullUrl;  //
}
//___________________________________________________________________
TString TSQLUrl::GetProtocol() const 
{
   // protocol: oracle, mysql, postgresql

   return fParser->fProtocol;
}

//___________________________________________________________________
TString TSQLUrl::GetSubProtocol() const 
{
   // subprotocol (driver type) : odbc, oci8, thin etc 

   return fParser->fSubProtocol;
}

//___________________________________________________________________
TString TSQLUrl::GetDriver() const
{
   // driver library name: myodbc, oplodbc, odbcpsql  or full path to driver
   //                /usr/lib/libmyodbc.so.1   

   return fParser->fDriver;
}

//___________________________________________________________________
TString TSQLUrl::GetHost() const
{
   // remote host

   return fParser->fHost;
}

//___________________________________________________________________
TString TSQLUrl::GetFile() const
{
   // remote object: file or database

   return fParser->fFile;
}

//___________________________________________________________________
TString TSQLUrl::GetAnchor() const
{
   // anchor in object

   return fParser->fAnchor;
}

//___________________________________________________________________
TString TSQLUrl::GetOptions() const
{
   // options (after ?) 

   return fParser->fOptions;
}

//___________________________________________________________________
TString TSQLUrl::GetDescription() const
{
   // return description string

   return fParser->fDescription;
}

//___________________________________________________________________
Int_t TSQLUrl::GetPort() const
{
   //  port through which to contact remote server

   const char* str = (const char*)fParser->fPort;
   return atoi(str);
}

//___________________________________________________________________
Bool_t TSQLUrl::IsValid() const
{
   // 

   return fParser->fStatus == 0;
}

//___________________________________________________________________
Bool_t TSQLUrl::IsDynamicDSN() const
{
   // 

   return fParser->fDynamicDSN;
}

//___________________________________________________________________
void TSQLUrl::AddOption(const TString& opt)
{
   // 

   int l = fParser->fOptions.Length();

   if(l) fParser->fFullUrl += "?";

   if(!l || ( l && !fParser->fOptions.Last(';'))) fParser->fOptions += ";";
   fParser->fOptions += opt + ";";
   fParser->fFullUrl += opt + ";";
}

//___________________________________________________________________
const char *TSQLUrl::GetName() const
{
   //  

   return GetDSN().Data();
}

//___________________________________________________________________
const char *TSQLUrl::GetTitle() const
{
   // 

   return GetDescription().Data();
}

//___________________________________________________________________
void TSQLUrl::ls(Option_t *option) const
{
   // pritnts url

   Print(option);
}

//___________________________________________________________________
void TSQLUrl::Print(Option_t *option) const
{
   // pritnts url to stdout or to file with name=option

   FILE* fd;
   if(!option || !strlen(option) || !IsDynamicDSN()) fd = stdout;
   else fd = fopen(option,"w"); 

   if(!fd) {
//      Throw( new TSQLException(Form("TSQLUrl:: Failed to open file: %s",option)) );
      return;
   }

   if(!fParser->fDSN.IsNull()) fprintf(fd,"[%s]\n", fParser->fDSN.Data()); 
   if(!fParser->fDescription.IsNull()) fprintf(fd,"Description\t\t= %s\n", fParser->fDescription.Data());
   if(!fParser->fProtocol.IsNull() && (fd == stdout) ) fprintf(fd,"RDBC Protocol\t\t= %s\n", fParser->fProtocol.Data()); 
   if(!fParser->fSubProtocol.IsNull() && (fd == stdout) ) fprintf(fd,"RDBC SubProtocol\t\t= %s\n", fParser->fSubProtocol.Data()); 
   if(!fParser->fDriver.IsNull()) fprintf(fd,"Driver\t\t= %s\n", fParser->fDriver.Data()); 
   if(!fParser->fHost.IsNull()) fprintf(fd,"Server\t\t= %s\n", fParser->fHost.Data()); 
   if(!fParser->fPort.IsNull()) fprintf(fd,"Port\t\t= %s\n", fParser->fPort.Data());  
   if(!fParser->fFile.IsNull()) fprintf(fd,"Database\t\t= %s\n", fParser->fFile.Data()); 
   if(!fParser->fAnchor.IsNull() && (fd == stdout) ) fprintf(fd,"Anchor\t\t= %s\n", fParser->fAnchor.Data()); 
   if(!fParser->fOptions.IsNull()) { 
      TString str(fParser->fOptions.Data());
      str.ReplaceAll(";","\n");
      str.ReplaceAll("&","\n");
      str.ReplaceAll("=","\t\t= ");
      fprintf(fd,"%s",str.Data());
   }
   fprintf( fd,"\n" );
   if(fd != stdout) fclose(fd);
}

