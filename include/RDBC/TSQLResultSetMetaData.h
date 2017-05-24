// $Id: TSQLResultSetMetaData.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLResultSetMetaData_h
#define RDBC_TSQLResultSetMetaData_h

//
// TSQLResultSetMetaData class - provides meta data about a result set 
//

#ifndef RDBC_TSQL_h
#include <RDBC/TSQL.h>
#endif

class TSQLResultSet;
////////////////////////////////////////////////////////////////////
class TSQLResultSetMetaData: public TSQL
{
friend class TSQLResultSet;

protected:
   TSQLResultSet* fResultSet;    // parent result set

   TSQLResultSetMetaData( TSQLResultSet* rs, void* imp=0 );
   virtual ~TSQLResultSetMetaData();

public:   
   virtual Int_t     GetColumnCount() = 0;
   virtual Bool_t    IsAutoIncrement(Int_t column) = 0;
   virtual Bool_t    IsCaseSensitive(Int_t column) = 0;
   virtual Bool_t    IsSearchable(Int_t column) = 0;                     
   virtual Bool_t    IsCurrency(Int_t column) = 0;
   virtual Bool_t    IsNullable(Int_t column) = 0;               
   virtual Bool_t    IsSigned(Int_t column) = 0;  
   virtual Int_t     GetColumnDisplaySize(Int_t column) = 0;
   virtual TString   GetColumnLabel(Int_t column) = 0;
   virtual TString   GetColumnName(Int_t column) = 0;
   virtual TString   GetSchemaName(Int_t column) = 0;
   virtual Int_t     GetPrecision(Int_t column) = 0;
   virtual Int_t     GetScale(Int_t column) = 0;             
   virtual TString   GetTableName(Int_t column) = 0;
   virtual TString   GetCatalogName(Int_t column) = 0;
   virtual Int_t     GetColumnType(Int_t column) = 0;
   virtual TString   GetColumnTypeName(Int_t column) = 0; 
   virtual Bool_t    IsReadOnly(Int_t column) = 0;                   
   virtual Bool_t    IsWritable(Int_t column) = 0;
   virtual Bool_t    IsDefinitelyWritable(Int_t column) = 0;
 
ClassDef(TSQLResultSetMetaData,0)// provides meta data about a result set 
};

#endif // RDBC_TSQLResultSetMetaData_h 
