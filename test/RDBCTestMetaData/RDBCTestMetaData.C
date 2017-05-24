
#include <TError.h>
#include <TString.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLDatabaseMetaData.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLResultSetMetaData.h>
#include <RDBC/TSQLPreparedStatement.h>
#include <RDBC/TSQLCallableStatement.h>
#include <RDBC/TSQLTypes.h>
#include <string>
extern "C" {
#include <stdlib.h>
};

//A quick test to make sure TSQLResultSetMetaData::GetTables() works
//and  TSQLResultSetMetaData::GetColumnName()  and ::GetColumnTypeName()
//work OK
//Author dbox@fnal.gov
//Date 1/6/03
//
//___________________________________________________________________
Int_t RDBCTestMetaData(const Text_t* dsn, const Text_t* schema,
		       const Text_t* usr="",
		       const Text_t* pwd="")
{
  //    Open a connection...
  TSQLConnection* myConnection = NULL;
  if(usr!="" && pwd !=""){
    if(getenv("VERBOSE"))
      printf( "connecting with: dsn= %s usr=%s pwd=%s\n",dsn,usr,pwd);
    myConnection = TSQLDriverManager::GetConnection( dsn, usr,  pwd );
  } else{
    if(getenv("VERBOSE"))
      printf( "connecting with: dsn= %s \n",dsn);
    myConnection = TSQLDriverManager::GetConnection( dsn );
  }

  if(!myConnection) {

    printf( "failed to connect: dsn= %s usr=%s pwd=%s\n",dsn,usr,pwd);
    printf("exiting...\n");
    return -1;  // return on error
  }else
    printf("connected!!!\n");

  TSQLDatabaseMetaData *md = myConnection->GetMetaData();







  TString schm=schema;
  TString catalog=schema;
  TString tab_pattern="";
  TString tab_type="TABLE";
  TString tableName="";
  TSQLResultSet *rs = md->GetTables(catalog,schm,tab_pattern,tab_type);
  while(rs->Next()){
    tableName = rs->GetString(3);
    printf("%s\n",tableName.Data());
  }

  printf("Bool_t AllProceduresAreCallable():%d\n", 
	 md->AllProceduresAreCallable());
  printf("Bool_t AllTablesAreSelectable():%d\n",
	 md->AllTablesAreSelectable());
  printf("TString GetUserName():%s\n", md->GetUserName().Data());
  printf("Bool_t IsReadOnly():%d\n", md->IsReadOnly());
  printf("Bool_t NullsAreSortedHigh():%d\n", md->NullsAreSortedHigh());
  printf("Bool_t NullsAreSortedLow():%d\n", md->NullsAreSortedLow());
  printf("Bool_t NullsAreSortedAtStart():%d\n", md->NullsAreSortedAtStart());
  printf("Bool_t NullsAreSortedAtEnd():%d\n", md->NullsAreSortedAtEnd());
  printf("TString GetDatabaseProductName():%s\n", 
	 md->GetDatabaseProductName().Data());
  printf("TString GetDatabaseProductVersion():%s\n", 
	 md->GetDatabaseProductVersion().Data());
  printf("TString GetDriverName():%s\n", md->GetDriverName().Data());
  printf("TString GetDriverVersion():%s\n", md->GetDriverVersion().Data());
  printf("Int_t GetDriverMajorVersion():%d\n", md->GetDriverMajorVersion());
  printf("Int_t GetDriverMinorVersion():%d\n", md->GetDriverMinorVersion());
  printf("Bool_t UsesLocalFiles():%d\n", md->UsesLocalFiles());
  printf("Bool_t UsesLocalFilePerTable():%d\n", md->UsesLocalFilePerTable());
  printf("Bool_t SupportsMixedCaseIdentifiers():%d\n", 
	 md->SupportsMixedCaseIdentifiers());
  printf("Bool_t StoresUpperCaseIdentifiers():%d\n", 
	 md->StoresUpperCaseIdentifiers());
  printf("Bool_t StoresLowerCaseIdentifiers():%d\n", 
	 md->StoresLowerCaseIdentifiers());
  printf("Bool_t StoresMixedCaseIdentifiers():%d\n", 
	 md->StoresMixedCaseIdentifiers());
  printf("Bool_t SupportsMixedCaseQuotedIdentifiers():%d\n", 
	 md->SupportsMixedCaseQuotedIdentifiers());
  printf("Bool_t StoresUpperCaseQuotedIdentifiers():%d\n", 
	 md->StoresUpperCaseQuotedIdentifiers());
  printf("Bool_t StoresLowerCaseQuotedIdentifiers():%d\n", 
	 md->StoresLowerCaseQuotedIdentifiers());
  printf("Bool_t StoresMixedCaseQuotedIdentifiers():%d\n", 
	 md->StoresMixedCaseQuotedIdentifiers());
  printf("TString GetIdentifierQuoteString():%s\n", 
	 md->GetIdentifierQuoteString().Data());
  printf("TString GetSQLKeywords():%s\n", 
	 md->GetSQLKeywords().Data());
  printf("TString GetNumericFunctions():%s\n", 
	 md->GetNumericFunctions().Data());
  printf("TString GetStringFunctions():%s\n", 
	 md->GetStringFunctions().Data());
  printf("TString GetSystemFunctions():%s\n", 
	 md->GetSystemFunctions().Data());
  printf("TString GetTimeDateFunctions():%s\n", 
	 md->GetTimeDateFunctions().Data());
  printf("TString GetSearchStringEscape():%s\n", 
	 md->GetSearchStringEscape().Data());
  printf("TString GetExtraNameCharacters():%s\n", 
	 md->GetExtraNameCharacters().Data());
  printf("Bool_t SupportsAlterTableWithAddColumn():%d\n", 
	 md->SupportsAlterTableWithAddColumn());
  printf("Bool_t SupportsAlterTableWithDropColumn():%d\n", 
	 md->SupportsAlterTableWithDropColumn());
  printf("Bool_t SupportsColumnAliasing():%d\n", 
	 md->SupportsColumnAliasing());
  printf("Bool_t NullPlusNonNullIsNull():%d\n", 
	 md->NullPlusNonNullIsNull());
  printf("Bool_t SupportsConvert():%d\n", md->SupportsConvert());
  //printf("Bool_t SupportsConvert(Int_t:%d\n", md->SupportsConvert(Int_t);
  printf("Bool_t SupportsTableCorrelationNames():%d\n", 
	 md->SupportsTableCorrelationNames());
  printf("Bool_t SupportsDifferentTableCorrelationNames():%d\n", 
	 md->SupportsDifferentTableCorrelationNames());
  printf("Bool_t SupportsExpressionsInOrderBy():%d\n", 
	 md->SupportsExpressionsInOrderBy());
  printf("Bool_t SupportsOrderByUnrelated():%d\n", 
	 md->SupportsOrderByUnrelated());
  printf("Bool_t SupportsGroupBy():%d\n", 
	 md->SupportsGroupBy());
  printf("Bool_t SupportsGroupByUnrelated():%d\n", 
	 md->SupportsGroupByUnrelated());
  printf("Bool_t SupportsGroupByBeyondSelect():%d\n", 
	 md->SupportsGroupByBeyondSelect());
  printf("Bool_t SupportsLikeEscapeClause():%d\n", 
	 md->SupportsLikeEscapeClause());
  printf("Bool_t SupportsMultipleResultSets():%d\n", 
	 md->SupportsMultipleResultSets());
  printf("Bool_t SupportsMultipleTransactions():%d\n", 
	 md->SupportsMultipleTransactions());
  printf("Bool_t SupportsNonNullableColumns():%d\n", 
	 md->SupportsNonNullableColumns());
  printf("Bool_t SupportsMinimumSQLGrammar():%d\n", 
	 md->SupportsMinimumSQLGrammar());
  printf("Bool_t SupportsCoreSQLGrammar():%d\n", 
	 md->SupportsCoreSQLGrammar());
  printf("Bool_t SupportsExtendedSQLGrammar():%d\n", 
	 md->SupportsExtendedSQLGrammar());
  printf("Bool_t SupportsANSI92EntryLevelSQL():%d\n", 
	 md->SupportsANSI92EntryLevelSQL());
  printf("Bool_t SupportsANSI92IntermediateSQL():%d\n", 
	 md->SupportsANSI92IntermediateSQL());
  printf("Bool_t SupportsANSI92FullSQL():%d\n", 
	 md->SupportsANSI92FullSQL());
  printf("Bool_t SupportsIntegrityEnhancementFacility():%d\n", 
	 md->SupportsIntegrityEnhancementFacility());
  printf("Bool_t SupportsOuterJoins():%d\n", 
	 md->SupportsOuterJoins());
  printf("Bool_t SupportsFullOuterJoins():%d\n", 
	 md->SupportsFullOuterJoins());
  printf("Bool_t SupportsLimitedOuterJoins():%d\n", 
	 md->SupportsLimitedOuterJoins());
  printf("TString GetSchemaTerm():%s\n", 
	 md->GetSchemaTerm().Data());
  printf("TString GetTableTerm():%s\n", 
	 md->GetTableTerm().Data());
  printf("TString GetProcedureTerm():%s\n", 
	 md->GetProcedureTerm().Data());
  printf("TString GetCatalogTerm():%s\n", 
	 md->GetCatalogTerm().Data());
  printf("Bool_t IsCatalogAtStart():%d\n", 
	 md->IsCatalogAtStart());
  printf("TString GetCatalogSeparator():%s\n", 
	 md->GetCatalogSeparator().Data());
  printf("Bool_t SupportsSchemasInDataManipulation():%d\n", 
	 md->SupportsSchemasInDataManipulation());
  printf("Bool_t SupportsSchemasInProcedureCalls():%d\n", 
	 md->SupportsSchemasInProcedureCalls());
  printf("Bool_t SupportsSchemasInTableDefinitions():%d\n", 
	 md->SupportsSchemasInTableDefinitions());
  printf("Bool_t SupportsSchemasInIndexDefinitions():%d\n", 
	 md->SupportsSchemasInIndexDefinitions());
  printf("Bool_t SupportsSchemasInPrivilegeDefinitions():%d\n", 
	 md->SupportsSchemasInPrivilegeDefinitions());
  printf("Bool_t SupportsCatalogsInDataManipulation():%d\n", 
	 md->SupportsCatalogsInDataManipulation());
  printf("Bool_t SupportsCatalogsInProcedureCalls():%d\n", 
	 md->SupportsCatalogsInProcedureCalls());
  printf("Bool_t SupportsCatalogsInTableDefinitions():%d\n", 
	 md->SupportsCatalogsInTableDefinitions());
  printf("Bool_t SupportsCatalogsInIndexDefinitions():%d\n", 
	 md->SupportsCatalogsInIndexDefinitions());
  printf("Bool_t SupportsCatalogsInPrivilegeDefinitions():%d\n", 
	 md->SupportsCatalogsInPrivilegeDefinitions());
  printf("Bool_t SupportsPositionedDelete():%d\n", 
	 md->SupportsPositionedDelete());
  printf("Bool_t SupportsPositionedUpdate():%d\n", 
	 md->SupportsPositionedUpdate());
  printf("Bool_t SupportsSelectForUpdate():%d\n", 
	 md->SupportsSelectForUpdate());
  printf("Bool_t SupportsStoredProcedures():%d\n", 
	 md->SupportsStoredProcedures());
  printf("Bool_t SupportsSubqueriesInComparisons():%d\n", 
	 md->SupportsSubqueriesInComparisons());
  printf("Bool_t SupportsSubqueriesInExists():%d\n", 
	 md->SupportsSubqueriesInExists());
  printf("Bool_t SupportsSubqueriesInIns():%d\n", 
	 md->SupportsSubqueriesInIns());
  printf("Bool_t SupportsSubqueriesInQuantifieds():%d\n", 
	 md->SupportsSubqueriesInQuantifieds());
  printf("Bool_t SupportsCorrelatedSubqueries():%d\n", 
	 md->SupportsCorrelatedSubqueries());
  printf("Bool_t SupportsUnion():%d\n", 
	 md->SupportsUnion());
  printf("Bool_t SupportsUnionAll():%d\n", 
	 md->SupportsUnionAll());
  printf("Bool_t SupportsOpenCursorsAcrossRollback():%d\n", 
	 md->SupportsOpenCursorsAcrossRollback());
  printf("Bool_t SupportsOpenStatementsAcrossCommit():%d\n", 
	 md->SupportsOpenStatementsAcrossCommit());
  printf("Bool_t SupportsOpenStatementsAcrossRollback():%d\n", 
	 md->SupportsOpenStatementsAcrossRollback());
  printf("Int_t GetMaxBinaryLiteralLength():%d\n", 
	 md->GetMaxBinaryLiteralLength());
  printf("Int_t GetMaxCharLiteralLength():%d\n", 
	 md->GetMaxCharLiteralLength());
  printf("Int_t GetMaxColumnNameLength():%d\n", 
	 md->GetMaxColumnNameLength());
  printf("Int_t GetMaxColumnsInGroupBy():%d\n", 
	 md->GetMaxColumnsInGroupBy());
  printf("Int_t GetMaxColumnsInIndex():%d\n", 
	 md->GetMaxColumnsInIndex());
  printf("Int_t GetMaxColumnsInOrderBy():%d\n", 
	 md->GetMaxColumnsInOrderBy());
  printf("Int_t GetMaxColumnsInSelect():%d\n", 
	 md->GetMaxColumnsInSelect());
  printf("Int_t GetMaxColumnsInTable():%d\n", 
	 md->GetMaxColumnsInTable());
  printf("Int_t GetMaxConnections():%d\n", 
	 md->GetMaxConnections());
  printf("Int_t GetMaxCursorNameLength():%d\n", 
	 md->GetMaxCursorNameLength());
  printf("Int_t GetMaxIndexLength():%d\n", 
	 md->GetMaxIndexLength());
  printf("Int_t GetMaxSchemaNameLength():%d\n", 
	 md->GetMaxSchemaNameLength());
  printf("Int_t GetMaxProcedureNameLength():%d\n", 
	 md->GetMaxProcedureNameLength());
  printf("Int_t GetMaxCatalogNameLength():%d\n", 
	 md->GetMaxCatalogNameLength());
  printf("Int_t GetMaxRowSize():%d\n", md->GetMaxRowSize());
  printf("Bool_t DoesMaxRowSizeIncludeBlobs():%d\n", 
	 md->DoesMaxRowSizeIncludeBlobs());
  printf("Int_t GetMaxStatementLength():%d\n", 
	 md->GetMaxStatementLength());
  printf("Int_t GetMaxStatements():%d\n", 
	 md->GetMaxStatements());
  printf("Int_t GetMaxTableNameLength():%d\n",
	 md->GetMaxTableNameLength());
  printf("Int_t GetMaxTablesInSelect():%d\n", 
	 md->GetMaxTablesInSelect());
  printf("Int_t GetMaxUserNameLength():%d\n",
	 md->GetMaxUserNameLength());
  printf("Int_t GetDefaultTransactionIsolation():%d\n", 
	 md->GetDefaultTransactionIsolation());
  printf("Bool_t SupportsTransactions():%d\n", 
	 md->SupportsTransactions());
		
  printf("Bool_t SupportsDataDefinitionAndDataManipulationTransactions():%d\n", 
	 md->SupportsDataDefinitionAndDataManipulationTransactions());
		
  printf("Bool_t SupportsDataManipulationTransactionsOnly():%d\n", 
	 md->SupportsDataManipulationTransactionsOnly());
  printf("Bool_t DataDefinitionCausesTransactionCommit():%d\n", 
	 md->DataDefinitionCausesTransactionCommit());
  printf("Bool_t DataDefinitionIgnoredInTransactions():%d\n", 
	 md->DataDefinitionIgnoredInTransactions());
  
  printf("looking at columns of %s\n",tableName.Data());
  TString query = "select * from ";
  query += tableName + " where 1=0;";
  TSQLStatement* stmt = myConnection->CreateStatement();
  TSQLResultSet *rs2 =stmt->ExecuteQuery( query);
   
  TSQLResultSetMetaData *rsmd = rs2->GetMetaData();
  int numCols = rsmd->GetColumnCount();

  for(int i=1; i<= numCols; i++)
    {
       
      printf( "col name= '%s' type = '%s' \n",
	      rsmd->GetColumnName(i).Data(), 
	      rsmd->GetColumnTypeName(i).Data() );
       
    }
   

  myConnection->Close();
  return 0;
}

//___________________________________________________________________
void Catch(TSQLException* e)
{ 
  // handle exceptions
   
  TString str = e->GetMessage();
  printf("SQL Error: %s\n",str.Data()); 
}


//////////////////////////// Main program ////////////////////////////////////
#ifdef STANDALONE

#include <TROOT.h>
#include <TSystem.h>
#include <iostream>

//---- Main program ------------------------------------------------------------

TROOT root("RDBCTestMetaData","Test RDBC TSQLDriverManager and TSQLConnection");

int main(int argc, char **argv)
{

  gSystem->Load("libRDBC");
  Int_t ret = -1;
  /* 
     printf("arg0:'%s'\n",argv[0]);
     printf("arg1:'%s'\n",argv[1]);
     printf("arg2:'%s'\n",argv[2]);
     printf("arg3:'%s'\n",argv[3]);
     printf("arg4:'%s'\n",argv[4]);
  */


  if(argc != 3 && argc != 5){
    printf ("usage: RDBCTestMetaData dsn schema [usr] [password]\n");
    return ret;
  }

  
  if(argc==3) 
    ret=RDBCTestMetaData(argv[1],argv[2]);
  if(argc==5)
    ret=RDBCTestMetaData(argv[1],argv[2],argv[3], argv[4]);

  return ret;
}
#endif
