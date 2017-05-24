// $Id: ODBCResultSetMetaData.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_ODBCResultSetMetaData_h
#define RDBC_ODBCResultSetMetaData_h

//
// ODBCResultSetMetaData class - provides meta data about a result set 
//

#ifndef RDBC_TSQLResultSetMetaData_h
#include <RDBC/TSQLResultSetMetaData.h>
#endif

class TSQLResultSet;
////////////////////////////////////////////////////////////////////
class ODBCResultSetMetaData: public TSQLResultSetMetaData
{
friend class ODBCResultSet;

protected:

   ODBCResultSetMetaData( TSQLResultSet* rs, void* imp=0 );
   virtual ~ODBCResultSetMetaData();

public:   
   Int_t    GetColumnCount();
   Bool_t   IsAutoIncrement(Int_t column);
   Bool_t   IsCaseSensitive(Int_t column);
   Bool_t   IsSearchable(Int_t column);                     
   Bool_t   IsCurrency(Int_t column);
   Bool_t   IsNullable(Int_t column);               
   Bool_t   IsSigned(Int_t column);  
   Int_t    GetColumnDisplaySize(Int_t column);
   TString  GetColumnLabel(Int_t column);
   TString  GetColumnName(Int_t column);
   TString  GetSchemaName(Int_t column);
   Int_t    GetPrecision(Int_t column);
   Int_t    GetScale(Int_t column);             
   TString  GetTableName(Int_t column);
   TString  GetCatalogName(Int_t column);
   Int_t    GetColumnType(Int_t column);
   TString  GetColumnTypeName(Int_t column); 
   Bool_t   IsReadOnly(Int_t column);                   
   Bool_t   IsWritable(Int_t column);
   Bool_t   IsDefinitelyWritable(Int_t column);
 
ClassDef(ODBCResultSetMetaData,0) // provides meta data about a result set  
};

#endif // RDBC_ODBCResultSetMetaData_h 
