// $Id: TSQLImportClient.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 21/03/2001

/////////////////////////////////////////////////////////////////////
//
// TSQLImportClient
//
/////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLUrl.h>
#include <RDBC/TSQL.h>
#include <RDBC/TSQLImportClient.h>
#include <TString.h>
#include <TSystem.h>
#include <stdlib.h>
#include <stdio.h>
#include <TUrl.h>
#include <TSocket.h>

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLImportClient::TSQLImportClient(const TString& url)
{
   // ctor.

   fUrl = new TSQLUrl(url);
   fException = 0;
   fStatus = 0;
   TString host(fUrl->GetHost().Data());
   TString str(fUrl->GetFile().Data());
   fLocalFile = str;

   if(host=="localhost")  {
      fMustDeleteLocalFile = kFALSE;
   } else {
      fMustDeleteLocalFile = kTRUE;
      fLocalFile = Form("/tmp/%s%d",gSystem->BaseName(fUrl->GetFile().Data()),gSystem->GetPid());
      GET(url);   // download 
   }

   fSkipLines = 1; // default , first line is a header describes the columns
}

//___________________________________________________________________
TSQLImportClient::~TSQLImportClient()
{
   // dtor.

   Clean();
}

//___________________________________________________________________
void TSQLImportClient::Clean()
{
   //

   if(fMustDeleteLocalFile) {
      gSystem->Unlink(fLocalFile.Data());
   }
   if(fException) delete fException;
   if(fUrl) delete fUrl; 
}

//___________________________________________________________________
TString Validate(const TString& str)
{
   // internal use static func.
   // 
   // - Does validation of string as coulmn names/types, very primitive so far.
   // - Returns corrected column string 

   TString ret = str.Strip(TString::kBoth);
   Int_t spidx = 0;
   const char* s = ret.Data();
   
   if(s[0]=='\"' || s[strlen(s)-1]=='\"' ) {
      TString quote('\"');
      ret.ReplaceAll(quote,"");
      goto exit;
   }
   if( ret.IsNull() ||
      ((ret.Length()==1) && !isalpha(s[0]) ) ) return "wrong format";

   for (Ssiz_t i = 0; i < ret.Length(); i++) {
      if( !isalnum(s[i]) && !isspace(s[i]) && 
         s[i]!=')' && s[i]!='(' && s[i]!=',' &&
         s[i]!='_' && s[i]!='-' ) {
         return "wrong format";
      }
      if(isspace(s[i])) spidx = i;
   }
   
exit:
   if(!spidx) ret += " TEXT NOT NULL";  
   return ret;   
}

//___________________________________________________________________
void TSQLImportClient::GET(const TString& url)
{
   // Download url into local temporary file
 
   TString str;
   const Int_t buflen=8192;
   static char buf[buflen];

   TString filename = url;
   filename.ReplaceAll(" ","");

   TUrl u(filename);
 
   TSocket s(u.GetHost(), u.GetPort());

   if (!s.IsValid()) {
      fStatus = HTTP_FORBIDDEN;
      return;
   }

   TString msg = Form("GET %s HTTP/1.0\015\012\015\012", u.GetFile());
   s.SendRaw(msg.Data(), msg.Length());

   while(s.RecvRaw(buf, buflen)>0) {         
      str += buf; 
      memset(buf,0,buflen);
   }
   s.Close();

   // cutoff HTTP header
   Int_t idx;
   idx = str.Index("\015\012\015\012");
   if(idx!=kNPOS) str = str(idx+4,str.Length()-idx-4);

   FILE* fd = fopen(fLocalFile.Data(),"w");
   if(!fd) fStatus = HTTP_FORBIDDEN;

   idx = fwrite(str.Data(),str.Length(),1,fd);
   if(idx!=1) {
      fStatus = HTTP_FORBIDDEN;
   }
   fclose(fd);
   return;
}

//___________________________________________________________________
Int_t TSQLImportClient::Init()
{
   // - read first line from local file 
   // - determine column names and types

   //usused: Bool_t isValid = kTRUE;
   FILE* fd;
   TString str;

   fTableName = TString(gSystem->BaseName(fLocalFile.Data()));
   TString ext = strrchr(fTableName.Data(),'.');

   if(!ext.IsNull()) {
      Int_t pidx = fTableName.Index(ext.Data());
      if(pidx>1) {
         fTableName =  fTableName(0,pidx);
      }

      fTableName.ReplaceAll(".","_");
   }

   if(gSystem->AccessPathName(fLocalFile.Data())) {
      fStatus = HTTP_NOT_FOUND;
      str = "File ";
      str += fLocalFile + " not found";
      fException = new TSQLException(str,"",fStatus);
      return fStatus;
   }

   fd = fopen(fLocalFile.Data(),"r");

   if( !fd ) {
      fclose(fd);
      fStatus = HTTP_FORBIDDEN;
      str = "You don't have read permission to ";
      str += fLocalFile;
      fException = new TSQLException(str,"",fStatus);
      return fStatus;
   }
   
   const Int_t buflen=8192;
   char buf[buflen];

   fgets(buf,buflen,fd);  // read first line         
   str = buf;

   if(str.IsNull()) {
      fclose(fd); // empty file
      fStatus = HTTP_NOT_ACCEPTABLE;
      str = "File ";
      str += fLocalFile + " is empty";
      fException = new TSQLException(str,"",fStatus);
      return fStatus;
   }
   str.Chop(); // cut off \n

   TString tmp;
   Int_t i,k;
   Int_t ncols = 0;
   Bool_t wrongFormat = kFALSE;

   for( i=k=0; (i=str.Index(",",i))>0; k=i++ ) {
      ncols++;
      tmp = Validate(str(!k?0:k+1,!k?i:i-k-1));
      wrongFormat = wrongFormat || tmp.IsNull() || (tmp=="wrong format"); 
      if(!wrongFormat) fColumns +=  tmp + ",";
   } 

   ncols++;
   tmp = Validate(str(k+(ncols>1),str.Length())); // the rest of string

   wrongFormat = wrongFormat || (tmp=="wrong format"); 
   if(!wrongFormat)  fColumns += tmp;
   else {
      fColumns = "";
      for(i=1; i<ncols; i++) fColumns += Form("C%d TEXT NOT NULL,",i);
      fColumns += Form("C%d TEXT NOT NULL",ncols);
      fSkipLines = 0;
   }

   fclose(fd);
   return fStatus = HTTP_OK;
}
