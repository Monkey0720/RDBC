// $Id: TSQLPreparedStatement.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLPreparedStatement_h
#define RDBC_TSQLPreparedStatement_h

#ifndef RDBC_TSQLStatement_h
#include <RDBC/TSQLStatement.h>
#endif
#ifndef ROOT_TString
#include <TString.h>
#endif
#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

//
// An object that represents a precompiled SQL statement. 
//

////////////////////////////////////////////////////////////////////
class TSQLPreparedStatement: public TSQLStatement
{
protected:
  TSQLPreparedStatement( TSQLConnection* con,void* imp=0 ):TSQLStatement(con,imp) {}
               
public:
   virtual ~TSQLPreparedStatement() {}
   
   virtual void SetNull( Int_t parameterIndex, Int_t sqlType ) = 0;
   virtual void SetBoolean( Int_t parameterIndex,Bool_t x ) = 0;
   virtual void SetByte( Int_t parameterIndex,Char_t x ) = 0;
   virtual void SetShort( Int_t parameterIndex,Short_t x ) = 0;
   virtual void SetInt( Int_t parameterIndex,Int_t x ) = 0;
   virtual void SetLong(Int_t parameterIndex,Long_t x) = 0;
   virtual void SetFloat(Int_t parameterIndex,Float_t x) = 0;
   virtual void SetDouble( Int_t parameterIndex,Double_t x) = 0;
   virtual void SetString(Int_t parameterIndex, const TString& x) = 0;
   virtual void SetBytes(Int_t parameterIndex,const TArrayC& x) = 0;
   virtual void SetDate(Int_t parameterIndex,const TSQLDate& x) = 0;
   virtual void SetTime(Int_t parameterIndex,const TSQLTime& x) = 0;
   virtual void SetTimestamp( Int_t parameterIndex,const TSQLTimestamp& x ) = 0;
   virtual void SetAsciiStream(Int_t parameterIndex,TBuffer* x,Int_t length) = 0;
   virtual void SetBinaryStream( Int_t parameterIndex,TBuffer* x,Int_t length ) = 0;
   virtual void SetObject( Int_t parameterIndex,TObject* x ) = 0;
   virtual void ClearParameters() = 0;   
   
ClassDef(TSQLPreparedStatement,0)//represents a precompiled SQL statement
};

#endif // RDBC_TSQLPreparedStatement_h
