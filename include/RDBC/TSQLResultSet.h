// $Id: TSQLResultSet.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLResultSet_h
#define RDBC_TSQLResultSet_h

//
// TSQLResultSet class - provides access to a table of data
//

#ifndef RDBC_TSQL_h
#include <RDBC/TSQL.h>
#endif
#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif
#ifndef ROOT_TSQLResult
#include <TSQLResult.h>
#endif

class TSQLStatement;
class TList;
class TSQLResultSetMetaData;
class TBuffer;
class TTree;
class RDBCRow;
////////////////////////////////////////////////////////////////////

enum EResultSetConcurrency{
   kCONCUR_READ_ONLY,
   kCONCUR_UPDATABLE
};

enum EResultSetType{
   kTYPE_FORWARD_ONLY,
   kTYPE_SCROLL_INSENSITIVE,
   kTYPE_SCROLL_SENSITIVE
};  

////////////////////////////////////////////////////////////////////
class TSQLResultSet: public TSQLResult, public TSQL
{
friend class TSQLStatement;

protected:
   RDBCRow* fRow;  // row == TSQLRow
   TSQLStatement* fStatement; // parent statement
   TSQLResultSetMetaData* fMetaData; // meta data

   TSQLResultSet( TSQLStatement* stmt=0, void* imp=0 );

public:
   virtual ~TSQLResultSet();
   virtual Bool_t    WasNull() = 0;
   virtual TString   GetString( Int_t columnIndex ) = 0;
   virtual Bool_t    GetBoolean( Int_t columnIndex ) = 0;
   virtual Char_t    GetByte( Int_t columnIndex ) = 0;
   virtual Short_t   GetShort( Int_t columnIndex ) = 0; 
   virtual Int_t     GetInt( Int_t columnIndex ) = 0;
   virtual Long_t    GetLong( Int_t columnIndex ) = 0;
   virtual Float_t   GetFloat( Int_t columnIndex ) = 0;
   virtual Double_t  GetDouble( Int_t columnIndex ) = 0;
   virtual TArrayC   GetBytes( Int_t columnIndex ) = 0;
   virtual TSQLDate  GetDate( Int_t columnIndex ) = 0; 
   virtual TSQLTime  GetTime( Int_t columnIndex ) = 0; 
   virtual TSQLTimestamp GetTimestamp( Int_t columnIndex ) = 0;   
   virtual TBuffer*  GetAsciiStream( Int_t columnIndex ) = 0;  
   virtual TBuffer*  GetBinaryStream( Int_t columnIndex ) = 0; 
   virtual TObject*  GetObject( Int_t columnIndex ) = 0;  
   virtual TString   GetString( const TString& columnName ) = 0; 
   virtual Bool_t    GetBoolean( const TString& columnName ) = 0;
   virtual Char_t    GetByte( const TString& columnName ) = 0;
   virtual Short_t   GetShort( const TString& columnName ) = 0;
   virtual Int_t     GetInt( const TString& columnName ) = 0;
   virtual Long_t    GetLong( const TString& columnName ) = 0;
   virtual Float_t   GetFloat( const TString& columnName ) = 0;
   virtual Double_t  GetDouble( const TString& columnName ) = 0;
   virtual TArrayC   GetBytes( const TString& columnName ) = 0;
   virtual TSQLDate  GetDate( const TString& columnName ) = 0; 
   virtual TSQLTime  GetTime( const TString& columnName ) = 0; 
   virtual TSQLTimestamp GetTimestamp( const TString& columnName ) = 0;
   virtual TBuffer*  GetAsciiStream( const TString& columnName ) = 0;  
   virtual TBuffer*  GetBinaryStream( const TString& columnName ) = 0; 
   virtual TObject*  GetObject( const TString& columnName ) = 0;
   virtual TString   GetCursorName() = 0;
   virtual Int_t     FindColumn( const TString& columnName ) = 0;
   virtual Bool_t    IsBeforeFirst() = 0;
   virtual Bool_t    IsAfterLast() = 0;
   virtual Bool_t    IsFirst() = 0;
   virtual Bool_t    IsLast() = 0;
   virtual void      BeforeFirst() = 0;
   virtual void      AfterLast() = 0;
   virtual void      SetFetchDirection( Int_t direction ) = 0;
   virtual Int_t     GetFetchDirection() = 0;
   virtual Int_t     GetRow() = 0;
   virtual void      SetFetchSize( Int_t rows ) = 0;
   virtual Int_t     GetFetchSize() = 0;
   virtual Int_t     GetType() = 0;
   virtual Int_t     GetConcurrency() = 0;
   virtual void      UpdateNull( Int_t columnIndex ) = 0;
   virtual void      UpdateBoolean( Int_t columnIndex,Bool_t x ) = 0;
   virtual void      UpdateByte( Int_t columnIndex,Char_t x ) = 0;
   virtual void      UpdateShort( Int_t columnIndex,Short_t x ) = 0;
   virtual void      UpdateInt( Int_t columnIndex,Int_t x ) = 0;
   virtual void      UpdateLong( Int_t columnIndex,Long_t x ) = 0;
   virtual void      UpdateFloat( Int_t columnIndex, Float_t x ) = 0;
   virtual void      UpdateDouble( Int_t columnIndex,Double_t x ) = 0;
   virtual void      UpdateString( Int_t columnIndex,const TString& x ) = 0;
   virtual void      UpdateBytes( Int_t columnIndex,const TArrayC& x ) = 0;
   virtual void      UpdateDate( Int_t columnIndex,const TSQLDate& x ) = 0;
   virtual void      UpdateTime( Int_t columnIndex,const TSQLTime& x ) = 0;
   virtual void      UpdateTimestamp( Int_t columnIndex,const TSQLTimestamp& x ) = 0;
   virtual void      UpdateAsciiStream( Int_t columnIndex,TBuffer* x,Int_t length ) = 0;
   virtual void      UpdateBinaryStream( Int_t columnIndex,TBuffer* x,Int_t length ) = 0;
   virtual void      UpdateObject( Int_t columnIndex,TObject* x ) = 0;
   virtual void      UpdateNull( const TString& columnName ) = 0;
   virtual void      UpdateBoolean( const TString& columnName, Bool_t x ) = 0;
   virtual void      UpdateByte( const TString& columnName,Char_t x ) = 0;
   virtual void      UpdateShort( const TString& columnName,Short_t x ) = 0;
   virtual void      UpdateInt( const TString& columnName,Int_t x ) = 0;
   virtual void      UpdateLong( const TString& columnName,Long_t x ) = 0;
   virtual void      UpdateFloat( const TString& columnName,Float_t x ) = 0;
   virtual void      UpdateDouble( const TString& columnName,Double_t x ) = 0;
   virtual void      UpdateString( const TString& columnName,const TString& x ) = 0;
   virtual void      UpdateBytes( const TString& columnName,const TArrayC& x ) = 0;
   virtual void      UpdateDate( const TString& columnName,const TSQLDate& x ) = 0;
   virtual void      UpdateTime( const TString& columnName, const TSQLTime& x ) = 0;
   virtual void      UpdateTimestamp( const TString& columnName,const TSQLTimestamp& x ) = 0;
   virtual void      UpdateAsciiStream( const TString& columnName,TBuffer* x,Int_t length ) = 0;
   virtual void      UpdateBinaryStream( const TString& columnName,TBuffer* x,Int_t length ) = 0;
   virtual void      UpdateObject( const TString& columnName,TObject* x ) = 0;
   virtual Bool_t    RowUpdated() = 0;
   virtual Bool_t    RowInserted() = 0;
   virtual Bool_t    RowDeleted() = 0;
   virtual void      InsertRow() = 0;
   virtual void      UpdateRow() = 0;
   virtual void      DeleteRow() = 0;
   virtual void      RefreshRow() = 0;
   virtual void      CancelRowUpdates() = 0;
   virtual void      MoveToInsertRow() = 0;
   virtual void      MoveToCurrentRow() = 0;
   virtual void      Close(const Option_t *option="") = 0;
   virtual TSQLRow*  Next() = 0;
   virtual TSQLRow*  Previous() = 0; 
   virtual TSQLRow*  First() = 0;
   virtual TSQLRow*  Last() = 0;
   virtual TSQLRow*  Absolute( Int_t row ) = 0;
   virtual TSQLRow*  Relative( Int_t rows ) = 0;
   virtual TSQLResultSetMetaData* GetMetaData() = 0;

   virtual TSQLStatement* GetStatement() const { return fStatement; }
   virtual Int_t     GetFieldCount();
   virtual const char *GetFieldName(Int_t field);
   virtual void Print(Option_t *option="") const;
   virtual TTree* Tree(Int_t begin=0,Int_t end=0);

ClassDef(TSQLResultSet,0) //provides access to a table of data
};

#endif // RDBC_TSQLResultSet_h 
