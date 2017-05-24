// $Id: ODBCResultSet.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_ODBCResultSet_h
#define RDBC_ODBCResultSet_h

//
// ODBCResultSet class - provides access to a table of data
//

#ifndef RDBC_TSQLResultSet_h
#include <RDBC/TSQLResultSet.h>
#endif



////////////////////////////////////////////////////////////////////
class ODBCResultSet: public TSQLResultSet
{
friend class ODBCStatement;
friend class ODBCPreparedStatement;
friend class ODBCCallableStatement;
friend class ODBCDatabaseMetaData;

protected:

   ODBCResultSet( TSQLStatement* stmt=0, void* imp=0 );   
         
public:   
   virtual ~ODBCResultSet();
   Bool_t   WasNull();
   TString  GetString( Int_t columnIndex );
   Bool_t   GetBoolean( Int_t columnIndex );
   Char_t   GetByte( Int_t columnIndex );
   Short_t  GetShort( Int_t columnIndex ); 
   Int_t    GetInt( Int_t columnIndex );
   Long_t   GetLong( Int_t columnIndex );
   Float_t  GetFloat( Int_t columnIndex );
   Double_t GetDouble( Int_t columnIndex );
   TArrayC  GetBytes( Int_t columnIndex );
   TSQLDate GetDate( Int_t columnIndex );   
   TSQLTime GetTime( Int_t columnIndex );      
   TSQLTimestamp GetTimestamp( Int_t columnIndex );        
   TBuffer* GetAsciiStream( Int_t columnIndex );    
   TBuffer* GetBinaryStream( Int_t columnIndex ); 
   TObject* GetObject( Int_t columnIndex );  
   TString  GetString( const TString& columnName ); 
   Bool_t   GetBoolean( const TString& columnName );
   Char_t   GetByte( const TString& columnName );
   Short_t  GetShort( const TString& columnName );
   Int_t    GetInt( const TString& columnName );
   Long_t   GetLong( const TString& columnName );
   Float_t  GetFloat( const TString& columnName );
   Double_t GetDouble( const TString& columnName );
   TArrayC  GetBytes( const TString& columnName );
   TSQLDate GetDate( const TString& columnName );   
   TSQLTime GetTime( const TString& columnName );      
   TSQLTimestamp GetTimestamp( const TString& columnName );
   TBuffer* GetAsciiStream( const TString& columnName );    
   TBuffer* GetBinaryStream( const TString& columnName ); 
   TObject* GetObject( const TString& columnName );
   TString  GetCursorName();
   TSQLResultSetMetaData* GetMetaData();
   Int_t  FindColumn( const TString& columnName );
   Bool_t IsBeforeFirst();
   Bool_t IsAfterLast();
   Bool_t IsFirst();
   Bool_t IsLast();
   void   BeforeFirst();
   void   AfterLast();
   void   SetFetchDirection( Int_t direction );
   Int_t  GetFetchDirection();
   Int_t  GetRow();
   void   SetFetchSize( Int_t rows );
   Int_t  GetFetchSize();
   Int_t  GetType();
   Int_t  GetConcurrency();
   void UpdateNull( Int_t columnIndex );
   void UpdateBoolean( Int_t columnIndex,Bool_t x );
   void UpdateByte( Int_t columnIndex,Char_t x );
   void UpdateShort( Int_t columnIndex,Short_t x );
   void UpdateInt( Int_t columnIndex,Int_t x );
   void UpdateLong( Int_t columnIndex,Long_t x );
   void UpdateFloat( Int_t columnIndex, Float_t x );
   void UpdateDouble( Int_t columnIndex,Double_t x );
   void UpdateString( Int_t columnIndex,const TString& x );
   void UpdateBytes( Int_t columnIndex,const TArrayC& x );
   void UpdateDate( Int_t columnIndex,const TSQLDate& x );
   void UpdateTime( Int_t columnIndex,const TSQLTime& x );
   void UpdateTimestamp( Int_t columnIndex,const TSQLTimestamp& x );
   void UpdateAsciiStream( Int_t columnIndex,TBuffer* x,Int_t length );
   void UpdateBinaryStream( Int_t columnIndex,TBuffer* x,Int_t length );
   void UpdateObject( Int_t columnIndex,TObject* x );
   void UpdateNull( const TString& columnName );
   void UpdateBoolean( const TString& columnName, Bool_t x );
   void UpdateByte( const TString& columnName,Char_t x );
   void UpdateShort( const TString& columnName,Short_t x );
   void UpdateInt( const TString& columnName,Int_t x );
   void UpdateLong( const TString& columnName,Long_t x );
   void UpdateFloat( const TString& columnName,Float_t x );
   void UpdateDouble( const TString& columnName,Double_t x );
   void UpdateString( const TString& columnName,const TString& x );
   void UpdateBytes( const TString& columnName,const TArrayC& x );
   void UpdateDate( const TString& columnName,const TSQLDate& x );
   void UpdateTime( const TString& columnName, const TSQLTime& x );
   void UpdateTimestamp( const TString& columnName,const TSQLTimestamp& x );
   void UpdateAsciiStream( const TString& columnName,TBuffer* x,Int_t length );
   void UpdateBinaryStream( const TString& columnName,TBuffer* x,Int_t length );
   void UpdateObject( const TString& columnName,TObject* x );
   Bool_t RowUpdated();
   Bool_t RowInserted();
   Bool_t RowDeleted();      
   void InsertRow();
   void UpdateRow();
   void DeleteRow();
   void RefreshRow();
   void CancelRowUpdates();
   void MoveToInsertRow();
   void MoveToCurrentRow();

   void     Close(Option_t *option="");
   TSQLRow* Next();
   TSQLRow* Previous();   
   TSQLRow* First();
   TSQLRow* Last();
   TSQLRow* Absolute( Int_t row );
   TSQLRow* Relative( Int_t rows );

ClassDef(ODBCResultSet,0) //provides access to a table of data   
};

#endif // RDBC_ODBCResultSet_h 
