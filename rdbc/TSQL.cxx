// $Id: TSQL.cxx,v 1.2 2007/02/28 21:32:46 phnxbld Exp $
//*-- Author : Valeriy Onuchin 14/02/2000 
//

////////////////////////////////////////////////////////////////////
//
//  Base class for RDBC.  It provides error and exception handling.
//
// =============================================================================
//______________________________________________________________________________
//*-*-*-*-*-*-*-*-*-*-*   A simple RDBC example    *-*-*-*-*-*-*-*-*-**-*-*-*-*-*
//            ===========================================
//___________________________________________________________________
//void RDBCexample(const Text_t* dsn="minos")
//{
//   //   Set exception handler function
//   TSQL::SetHandler("Catch(TSQLException*)");
//
//  TSQLConnection* myConnection = TSQLDriverManager::GetConnection( dsn, 
//                                                                   "scott", 
//                                                                   "tiger" );
//   if(!myConnection) return;  // return on error
//      
//   //    Create a statement...
//   TSQLStatement* myStatement = myConnection->CreateStatement();
//   if(!myStatement) return;   // return on error
//      
//   //    Execute the query...
//   Bool_t success = myStatement->Execute( "select * from emp" );
//   if(!success) return; // return on error
//   
//   //    Get the result set...
//   TSQLResultSet* mySet = myStatement->GetResultSet();
//   if(!mySet) return; // return on error
//
//   //    Advance to the next row...
//   while ( mySet->Next() ) { 
//                            
//     //    Get the data...
//      int empno = mySet->GetInt( "empno" );
//      TString ename = mySet->GetString( "ename" );
//      long salary = mySet->GetLong( "sal" );
//
//      //    Print it all out...
//      printf( "%d - %s - %dn",empno,ename.Data(),salary );
//   } 
//      
//   //    Close everything up...
//   delete myStatement; 
//   delete myConnection; 
//}
// 
//___________________________________________________________________
//void Catch(TSQLException* e)
//{
//   // exception handle function
//   
//   TString str = e->GetMessage();
//   printf("SQL Error: %sn",str.Data()); 
//}
//
//
////////////////////////////////////////////////////////////////////
//
// See also:
//    TSQLException  TSQLStatement TSQLPreparedStatement  TSQLResultSet
//    TSQLCallableStatement TSQLResultSet TSQLDatabaseMetaData
//    TSQLDriverManager TSQLResultSetMetaData  TSQLWarning TSQLDate 
//    TSQLTime TSQLTimestamp
//
////////////////////////////////////////////////////////////////////

#include <RDBC/TSQL.h>
#include <TClass.h>

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0)
#include <TList.h>
#endif

ClassImpQ(TSQL)
ClassImp(TSQLException)

/////////////////////////////////////////////////////////////////////
void TSQLException::Print(Option_t * /* option */) const
{
   // dump information about this exception 

   printf( "%s %s %d\n",
             GetMessage().Data(),
             GetSQLState().Data(),
             GetErrorCode() );
}

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQL::TSQL(void* imp):TQObject()
{
   // ctor

   fWarnings = new TList();   
   fImp = imp;
}

//___________________________________________________________________
TSQL::~TSQL()
{
   // dtor
   
   ClearWarnings();
   delete fWarnings;
}

//___________________________________________________________________
void TSQL::Throw( TSQLException* e )
{
   // exception handling service 
   // 
   // Warning: shared exceptions are not allowed yet

   if(gDebug) {
      printf("%s:\t",IsA()->GetName());
      e->Print();
   }

   fWarnings->Add(e);
   e->AddReference(); 
   Emit("Throw(TSQLException*)",(long)e);
}

//___________________________________________________________________
void TSQL::ClearWarnings()
{
   // Clears all warnings reported for this TSQL object. 
   
   fWarnings->Delete();
}

//___________________________________________________________________
Bool_t TSQL::SetHandler(const TString& handler)
{
   // connect any TSQLxxx object to handler function 
   
   Bool_t failed = kFALSE;
   
   UnsetHandler();
      
   failed |= !TQObject::Connect("TSQL","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLCallableStatement","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLConnection","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLDatabaseMetaData","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLDriverManager","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLPreparedStatement","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLResultSet","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLResultSetMetaData","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   failed |= !TQObject::Connect("TSQLStatement","Throw(TSQLException*)",
                     (const Text_t *)0,(void*)0,handler.Data());
   
   return !failed;
}

//___________________________________________________________________
Bool_t TSQL::UnsetHandler(const TString& handler)
{
   // disconnect everything from handler function
   
   const Text_t* slot;
   Bool_t failed = kFALSE;
   
   if(handler.IsNull()) {
      slot = 0;
   } else {
      slot = handler.Data();
   }
      
   failed |= !TQObject::Disconnect( "TSQL",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLCallableStatement",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLConnection",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLDatabaseMetaData",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLDriverManager",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLPreparedStatement",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLResultSet",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLResultSetMetaData",
                                    "Throw(TSQLException*)",0,slot );
   failed |= !TQObject::Disconnect( "TSQLStatement",
                                    "Throw(TSQLException*)",0,slot );
   return !failed;   
}

