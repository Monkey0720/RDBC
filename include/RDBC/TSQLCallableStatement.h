// $Id: TSQLCallableStatement.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLCallableStatement_h
#define RDBC_TSQLCallableStatement_h

#ifndef RDBC_TSQLPreparedStatement_h
#include <RDBC/TSQLPreparedStatement.h>
#endif

//
// A prepared statement suited for stored procedure calls
//

////////////////////////////////////////////////////////////////////
class TSQLCallableStatement: public TSQLPreparedStatement
{
protected:
  TSQLCallableStatement( TSQLConnection* con,void* imp=0 ):TSQLPreparedStatement(con,imp) {}

public:
   virtual ~TSQLCallableStatement() {}
   
   virtual void RegisterOutParameter( Int_t parameterIndex,Int_t sqlType ) = 0;
   virtual void RegisterOutParameter( Int_t parameterIndex,Int_t sqlType,Int_t scale ) = 0;   
   virtual Bool_t   WasNull() = 0;
   virtual TString  GetString(Int_t parameterIndex) = 0;
   virtual Bool_t   GetBoolean(Int_t parameterIndex) = 0;
   virtual Char_t   GetByte(Int_t parameterIndex) = 0;
   virtual Short_t  GetShort(Int_t parameterIndex) = 0;
   virtual Int_t    GetInt(Int_t parameterIndex) = 0;
   virtual Long_t   GetLong(Int_t parameterIndex) = 0;
   virtual Float_t  GetFloat(Int_t parameterIndex) = 0;
   virtual Double_t GetDouble(Int_t parameterIndex) = 0;   
   virtual TArrayC  GetBytes(Int_t parameterIndex) = 0;
   virtual TSQLDate GetDate(Int_t parameterIndex) = 0;
   virtual TSQLTime GetTime(Int_t parameterIndex) = 0;
   virtual TSQLTimestamp GetTimestamp(Int_t parameterIndex) = 0;

ClassDef(TSQLCallableStatement,0)//A prepared statement suited for stored procedure calls
};

#endif // RDBC_TSQLCallableStatement_h
