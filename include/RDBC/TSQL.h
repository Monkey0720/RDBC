// $Id: TSQL.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQL_h
#define RDBC_TSQL_h

//  base class for TSQL
//

#ifndef ROOT_TQObject
#include <TQObject.h>
#endif
#ifndef ROOT_TRefCnt
#include <TRefCnt.h>
#endif
#ifndef  RDBC_TSQLTypes_h
#include <RDBC/TSQLTypes.h>
#endif


/////////////////////////////////////////////////////////////////////
class TSQLException: public TObject, public TRefCnt
{
private:
   TString fReason;     // description of this message
   TString fSqlState;   // SQLLSTATE value
   Int_t   fErrorCode;  // error code of this exception

public:
   TSQLException(const TString& reason ="", 
                 const TString& sqlState ="",
                 Int_t vendorCode =0)
      :TObject(), fReason(reason), 
       fSqlState(sqlState),fErrorCode(vendorCode) {}
       
   virtual ~TSQLException() {}
   Int_t GetErrorCode() const { return fErrorCode; }
   const TString& GetSQLState() const { return fSqlState;}    
   const TString& GetMessage() const { return fReason; }
   
      // TObject methods
   virtual const char* GetName() const { return fReason.Data(); }
   virtual void Print(Option_t *option="") const;      
   virtual void ls(Option_t *option="") const { Print(option); }
   
ClassDef(TSQLException,0)// An exception that provides information on a database access error.
};


/////////////////////////////////////////////////////////////////////
class TSQL:public TQObject
{
protected:
   TList*   fWarnings; // a list of warnings
   void*    fImp; // "hidden" implementation part

public:
   TSQL(void* imp=0); 
   virtual ~TSQL();

   virtual void Throw( TSQLException* e ); // *SIGNAL*   

   TList* GetWarnings() const { return fWarnings; }
   void   ClearWarnings();
   virtual Bool_t IsValid() const { return fImp!=0; }

   static Bool_t SetHandler(const TString& handler);
   static Bool_t UnsetHandler(const TString& handler="");
      
ClassDef(TSQL,0)// base class, provides exception handling
};

#endif //  RDBC_TSQL_h
