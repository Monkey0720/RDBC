// $Id: TSQLDriverInfo.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLDriverInfo_h
#define RDBC_TSQLDriverInfo_h

//
// TSQLDriverInfo - info about driver
//

#ifndef RDBC_TSQL_h
#include <RDBC/TSQL.h>
#endif

#ifndef ROOT_TObject
#include "TObject.h"
#endif

/////////////////////////////////////////////////////////////////////
class TSQLDriverInfo: public TObject
{
private:
   TList*   fAttributes;       // list of key=value
   TString  fDescription;     // name of driver

public:
   TSQLDriverInfo( const TString& description, TList* attributes );
    ~TSQLDriverInfo();
 
   TString  GetDescription() const { return fDescription; }
   TList*   GetAttributes() const { return fAttributes; }
   virtual void  Print(const Option_t* opt="") const;    
   virtual void  ls(const Option_t* opt="") const { Print(opt); }
       
ClassDef(TSQLDriverInfo,0) // Info about driver
};

#endif // RDBC_TSQLDriverInfo_h
