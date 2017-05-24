// $Id: ODBCCallableStatement.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_ODBCCallableStatement_h
#define RDBC_ODBCCallableStatement_h

#ifndef RDBC_TSQLCallableStatement_h
#include <RDBC/TSQLCallableStatement.h>
#endif

//
// A prepared statement suited for stored procedure calls
//

////////////////////////////////////////////////////////////////////
class ODBCCallableStatement: public TSQLCallableStatement
{
friend class ODBCConnection;

protected:
  ODBCCallableStatement( TSQLConnection* con,void* imp=0 );                

public:
   virtual ~ODBCCallableStatement();
   
   void RegisterOutParameter( Int_t parameterIndex,Int_t sqlType );
   void RegisterOutParameter( Int_t parameterIndex,Int_t sqlType,Int_t scale );   
   Bool_t   WasNull();
   TString  GetString(Int_t parameterIndex);
   Bool_t   GetBoolean(Int_t parameterIndex);
   Char_t   GetByte(Int_t parameterIndex);
   Short_t  GetShort(Int_t parameterIndex);
   Int_t    GetInt(Int_t parameterIndex);
   Long_t   GetLong(Int_t parameterIndex);
   Float_t  GetFloat(Int_t parameterIndex);
   Double_t GetDouble(Int_t parameterIndex);   
   TArrayC  GetBytes(Int_t parameterIndex);
   TSQLDate GetDate(Int_t parameterIndex);
   TSQLTime GetTime(Int_t parameterIndex);
   TSQLTimestamp GetTimestamp(Int_t parameterIndex);

   // TSQLPreparedStatement methods
   void SetNull( Int_t parameterIndex, Int_t sqlType );
   void SetBoolean( Int_t parameterIndex,Bool_t x );
   void SetByte( Int_t parameterIndex,Char_t x );
   void SetShort( Int_t parameterIndex,Short_t x );
   void SetInt( Int_t parameterIndex,Int_t x );
   void SetLong(Int_t parameterIndex,Long_t x);
   void SetFloat(Int_t parameterIndex,Float_t x);
   void SetDouble( Int_t parameterIndex,Double_t x);
   void SetString(Int_t parameterIndex, const TString& x);
   void SetBytes(Int_t parameterIndex,const TArrayC& x);
   void SetDate(Int_t parameterIndex,const TSQLDate& x);
   void SetTime(Int_t parameterIndex,const TSQLTime& x);
   void SetTimestamp( Int_t parameterIndex,const TSQLTimestamp& x );
   void SetAsciiStream(Int_t parameterIndex,TBuffer* x,Int_t length);
   void SetBinaryStream( Int_t parameterIndex,TBuffer* x,Int_t length );
   void SetObject( Int_t parameterIndex,TObject* x );
   void ClearParameters();

   // TSQLStatement methods
   using TSQLStatement::Execute;  // unhide (TMethod*, TObjArray*, int*)
   virtual Bool_t Execute( const TString& sql="" );
   virtual TSQLResultSet* ExecuteQuery( const TString& sql="" );
   virtual Int_t  ExecuteUpdate( const TString& sql="" );
   TSQLResultSet* GetResultSet();
   Bool_t GetMoreResults();
   void   Close();
   void   Cancel();
   Int_t  GetMaxFieldSize();
   void   SetMaxFieldSize( Int_t max );
   Int_t  GetMaxRows();
   void   SetMaxRows( Int_t max );
   Bool_t GetEscapeProcessing();
   void   SetEscapeProcessing( Bool_t enable = kTRUE );
   Int_t  GetQueryTimeout();
   void   SetQueryTimeout( Int_t seconds );  
   Int_t  GetUpdateCount();
   void   SetCursorName( const TString& name );
   void   AddBatch( const TString& sql );
   void   ClearBatch();
   Int_t* ExecuteBatch();
   Int_t  GetFetchDirection();
   void   SetFetchDirection( Int_t direction );    
   Int_t  GetFetchSize(); 
   void   SetFetchSize(Int_t rows);
   Int_t  GetResultSetConcurrency(); 
   Int_t  GetResultSetType();

ClassDef(ODBCCallableStatement,0)//A prepared statement suited for stored procedure calls
};

#endif // RDBC_ODBCCallableStatement_h
