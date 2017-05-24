// $Id: MySQLResultSetPrivate.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_MySQLResultSetPrivate_h
#define RDBC_MySQLResultSetPrivate_h

#include <mysql.h>

//
// Very private MySQL part of TSQLResultSet
//

/////////////////////////////////////////////////////////////////////
class MySQLResultSetPrivate
{
friend class TSQLResultSet;
friend class TSQLResultSetMetaData;

private:
   Int_t    fCurrentRow;   // Cursor to current row
   Bool_t   fWasNull;      // for WasNull()
   Bool_t   fReallyResult; // for ExecuteUpdate() vs. Execute()
   
   Bool_t   fOnInsertRow;  //
   Bool_t   fDoingUpdates; //     

   Long_t   fUpdateID;     // for AUTO_INCREMENT
   Long_t   fUpdateCount;  // How many rows did we update?

   MYSQL_RES*  fMYSQL_RES;       // result  set 
   Int_t*      fColumnPrecisions;//  
   Int_t*      fColumnTypes;     // 
   TString*    fColumnTypeNames; //

   void Close() {
      mysql_free_result(fMYSQL_RES);
 
      if(fColumnPrecisions) delete fColumnPrecisions;
      if(fColumnTypes) delete fColumnTypes;
      if(fColumnTypeNames) delete fColumnTypeNames;
      
      fMYSQL_RES = 0;
      fCurrentRow = 0;
      fColumnPrecisions = 0; 
      fColumnTypes = 0; 
      fColumnTypeNames = 0;
   }
};

#endif // RDBC_MySQLResultSetPrivate_h
