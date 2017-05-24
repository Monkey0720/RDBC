// $Id: TSQLUrl.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLUrl_h
#define RDBC_TSQLUrl_h

//
//  URL string parser
//

#ifndef ROOT_TQObject
#include "TQObject.h"
#endif
#ifndef ROOT_TObject
#include "TObject.h"
#endif

class TSQLException;
class TSQLUrlParser;
/////////////////////////////////////////////////////////////////////
class TSQLUrl: public TObject, public TQObject
{
private:
   TSQLUrlParser* fParser;    // URL parser

public: 
   TSQLUrl() { fParser=0; }
   TSQLUrl(const TString& url);
   TSQLUrl(const TString& dsn,const TString& description);
   TSQLUrl(const TSQLUrl &url);
   virtual ~TSQLUrl();

   TSQLUrl& operator=(const TSQLUrl& url);
   operator const char*() { TString str = GetUrl(); return str.Data(); }
   TSQLUrl& operator=(const char *s) { TSQLUrl url(s); *this=url; return *this; }

   TString  GetDSN() const; 
   TString  GetUrl() const;
   TString  GetProtocol() const;
   TString  GetSubProtocol() const;
   TString  GetDriver() const;  
   TString  GetHost() const;
   TString  GetFile() const;
   TString  GetAnchor() const;
   TString  GetOptions() const;
   TString  GetDescription() const;
   Int_t    GetPort() const;
   Bool_t   IsValid() const;
   Bool_t   IsDynamicDSN() const;
   void     AddOption(const TString& opt);

   virtual void  Print(Option_t *option="") const;
   virtual void  ls(Option_t *option="") const;
   virtual const char *GetName() const;
   virtual const char *GetTitle() const;

   void Throw( TSQLException* e ) { Emit("Throw(TSQLException*)",(long)e); } //*SIGNAL*

ClassDef(TSQLUrl,0)// URL string parser
};

#endif // RDBC_TSQLUrl_h
