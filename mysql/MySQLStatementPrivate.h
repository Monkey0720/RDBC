// $Id: MySQLStatementPrivate.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
$Id: MySQLStatementPrivate.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_MySQLStatementPrivate_h
#define RDBC_MySQLStatementPrivate_h

//
// Very private MySQL part of TSQLStatement
//

#include <mysql.h>
#include <TString.h>

/////////////////////////////////////////////////////////////////////
class MySQLStatementPrivate
{
friend class TSQLStatement;
friend class TSQLPreparedStatement;

protected:
   UInt_t   fBindType;
   UInt_t   fRowsInSet;    //

   ULong_t  fMaxLength;    //
   Ulong_t  fMaxRows;      //
   Int_t    fQueryTimeout;   // The timeout for a query (not implemented)
   TString  fCatalog;
   TString  fQuery;     // last executed query

   Int_t    fCursorType;   //
};

/////////////////////////////////////////////////////////////////////
class MySQLPreparedStatementPrivate: public MySQLStatementPrivate
{
friend class TSQLStatement;
friend class TSQLPreparedStatement;

private:
   UInt_t   fBindType;
   UInt_t   fRowsInSet;    //

   ULong_t  fMaxLength;    //
   Ulong_t  fMaxRows;      //
   Int_t    fQueryTimeout;   // The timeout for a query (not implemented)
   TString  fCatalog;
   TString  fQuery;     // last executed query

   Int_t    fCursorType;   //
};


#endif // RDBC_MySQLStatementPrivate_h
