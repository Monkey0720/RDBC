// $Id: TSQLDriverInfo.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2000
//

#include <RDBC/TSQLDriverInfo.h>
#include <TNamed.h>
#include <TList.h>
#include <iostream>
using std::cout;
using std::endl;

ClassImpQ(TSQLDriverInfo)

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLDriverInfo::TSQLDriverInfo( const TString& description, 
                                TList* attributes )
{
   // constructor   

   fDescription = description;
   fAttributes = attributes;
} 

//___________________________________________________________________
TSQLDriverInfo::~TSQLDriverInfo()
{
   // destructor

   fAttributes->Delete();
   delete fAttributes;
}
      
//___________________________________________________________________
void TSQLDriverInfo::Print(const Option_t* /* opt */) const
{
   // print driver info

   TNamed* obj;

   cout << "\t" << fDescription << endl;
   
   TIter next(fAttributes);
   
   while((obj=(TNamed*)next())) {
      cout << "\t\t " << obj->GetName() << endl;
   }
}
