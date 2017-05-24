// $Id: ODBCPreparedStatement.h,v 1.2 2014/02/14 16:51:27 jinhuang Exp $

#ifndef RDBC_ODBCPreparedStatement_h
#define RDBC_ODBCPreparedStatement_h

#ifndef RDBC_TSQLPreparedStatement_h
#include <RDBC/TSQLPreparedStatement.h>
#endif

#ifndef __CINT__
#include <sstream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <vector>
#endif

//
// An object that represents a precompiled SQL statement. 
//

////////////////////////////////////////////////////////////////////
class ODBCPreparedStatement: public TSQLPreparedStatement
{
friend class ODBCConnection;

protected:
   ODBCPreparedStatement( TSQLConnection* con, void* imp=0 );

public:
   virtual ~ODBCPreparedStatement();
   
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

   using TSQLStatement::Execute;  // unhide (TMethod*, TObjArray*, int*)
   virtual Bool_t Execute( const TString& sql="" );
   virtual TSQLResultSet* ExecuteQuery( const TString& sql="" );
   virtual Int_t  ExecuteUpdate( const TString& sql="" );

   // TSQLStatement methods   
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
 
private:
#ifndef __CINT__
   std::vector < boost::shared_ptr<std::istream> > _vec_str_buf;
#endif

ClassDef(ODBCPreparedStatement,0)//represents a precompiled SQL statement
};

#endif // RDBC_ODBCPreparedStatement_h
