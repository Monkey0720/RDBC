// $Id: ODBCDatabaseMetaData.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_ODBCDatabaseMetaData_h
#define RDBC_ODBCDatabaseMetaData_h

//
// Provides several tons of information about a data source
//

#ifndef RDBC_TSQLDatabaseMetaData_h
#include <RDBC/TSQLDatabaseMetaData.h>
#endif

////////////////////////////////////////////////////////////////////
class ODBCDatabaseMetaData: public TSQLDatabaseMetaData
{
friend class ODBCConnection;

protected:

   ODBCDatabaseMetaData( TSQLConnection* connection,void* imp=0 );
   virtual ~ODBCDatabaseMetaData();
 
public:
   Bool_t AllProceduresAreCallable();
   Bool_t AllTablesAreSelectable();
   TString GetUserName();
   Bool_t IsReadOnly();
   Bool_t NullsAreSortedHigh();
   Bool_t NullsAreSortedLow();
   Bool_t NullsAreSortedAtStart();
   Bool_t NullsAreSortedAtEnd();
   TString GetDatabaseProductName();
   TString GetDatabaseProductVersion();
   TString GetDriverName();
   TString GetDriverVersion();
   Int_t GetDriverMajorVersion();
   Int_t GetDriverMinorVersion();
   Bool_t UsesLocalFiles();
   Bool_t UsesLocalFilePerTable();
   Bool_t SupportsMixedCaseIdentifiers();
   Bool_t StoresUpperCaseIdentifiers();
   Bool_t StoresLowerCaseIdentifiers();
   Bool_t StoresMixedCaseIdentifiers();
   Bool_t SupportsMixedCaseQuotedIdentifiers();
   Bool_t StoresUpperCaseQuotedIdentifiers();
   Bool_t StoresLowerCaseQuotedIdentifiers();
   Bool_t StoresMixedCaseQuotedIdentifiers();
   TString GetIdentifierQuoteString();
   TString GetSQLKeywords();
   TString GetNumericFunctions();
   TString GetStringFunctions();
   TString GetSystemFunctions();
   TString GetTimeDateFunctions();
   TString GetSearchStringEscape();
   TString GetExtraNameCharacters();
   Bool_t SupportsAlterTableWithAddColumn();
   Bool_t SupportsAlterTableWithDropColumn();
   Bool_t SupportsColumnAliasing();
   Bool_t NullPlusNonNullIsNull();
   Bool_t SupportsConvert();
   Bool_t SupportsConvert(Int_t fromType,Int_t toType);
   Bool_t SupportsTableCorrelationNames();
   Bool_t SupportsDifferentTableCorrelationNames();
   Bool_t SupportsExpressionsInOrderBy();
   Bool_t SupportsOrderByUnrelated();
   Bool_t SupportsGroupBy();
   Bool_t SupportsGroupByUnrelated();
   Bool_t SupportsGroupByBeyondSelect();
   Bool_t SupportsLikeEscapeClause();
   Bool_t SupportsMultipleResultSets();
   Bool_t SupportsMultipleTransactions();
   Bool_t SupportsNonNullableColumns();
   Bool_t SupportsMinimumSQLGrammar();
   Bool_t SupportsCoreSQLGrammar();
   Bool_t SupportsExtendedSQLGrammar();
   Bool_t SupportsANSI92EntryLevelSQL();
   Bool_t SupportsANSI92IntermediateSQL();
   Bool_t SupportsANSI92FullSQL();
   Bool_t SupportsIntegrityEnhancementFacility();
   Bool_t SupportsOuterJoins();
   Bool_t SupportsFullOuterJoins();
   Bool_t SupportsLimitedOuterJoins();
   TString GetSchemaTerm();
   TString GetTableTerm();   
   TString GetProcedureTerm();
   TString GetCatalogTerm();
   Bool_t IsCatalogAtStart();
   TString GetCatalogSeparator();
   Bool_t SupportsSchemasInDataManipulation();
   Bool_t SupportsSchemasInProcedureCalls();
   Bool_t SupportsSchemasInTableDefinitions();
   Bool_t SupportsSchemasInIndexDefinitions();
   Bool_t SupportsSchemasInPrivilegeDefinitions();
   Bool_t SupportsCatalogsInDataManipulation();
   Bool_t SupportsCatalogsInProcedureCalls();
   Bool_t SupportsCatalogsInTableDefinitions();
   Bool_t SupportsCatalogsInIndexDefinitions();
   Bool_t SupportsCatalogsInPrivilegeDefinitions();
   Bool_t SupportsPositionedDelete();
   Bool_t SupportsPositionedUpdate();
   Bool_t SupportsSelectForUpdate();
   Bool_t SupportsStoredProcedures();
   Bool_t SupportsSubqueriesInComparisons();
   Bool_t SupportsSubqueriesInExists();
   Bool_t SupportsSubqueriesInIns();
   Bool_t SupportsSubqueriesInQuantifieds();
   Bool_t SupportsCorrelatedSubqueries();
   Bool_t SupportsUnion();
   Bool_t SupportsUnionAll();
   Bool_t SupportsOpenCursorsAcrossCommit();
   Bool_t SupportsOpenCursorsAcrossRollback();
   Bool_t SupportsOpenStatementsAcrossCommit();
   Bool_t SupportsOpenStatementsAcrossRollback();
   Int_t GetMaxBinaryLiteralLength();
   Int_t GetMaxCharLiteralLength();
   Int_t GetMaxColumnNameLength();
   Int_t GetMaxColumnsInGroupBy();
   Int_t GetMaxColumnsInIndex();
   Int_t GetMaxColumnsInOrderBy();
   Int_t GetMaxColumnsInSelect();
   Int_t GetMaxColumnsInTable();
   Int_t GetMaxConnections();
   Int_t GetMaxCursorNameLength();
   Int_t GetMaxIndexLength();
   Int_t GetMaxSchemaNameLength();
   Int_t GetMaxProcedureNameLength();
   Int_t GetMaxCatalogNameLength();
   Int_t GetMaxRowSize();
   Bool_t DoesMaxRowSizeIncludeBlobs();
   Int_t GetMaxStatementLength();
   Int_t GetMaxStatements();
   Int_t GetMaxTableNameLength();
   Int_t GetMaxTablesInSelect();
   Int_t GetMaxUserNameLength();
   Int_t GetDefaultTransactionIsolation();
   Bool_t SupportsTransactions();
   Bool_t SupportsTransactionIsolationLevel(Int_t level);
   Bool_t SupportsDataDefinitionAndDataManipulationTransactions();
   Bool_t SupportsDataManipulationTransactionsOnly();
   Bool_t DataDefinitionCausesTransactionCommit();
   Bool_t DataDefinitionIgnoredInTransactions();
   TSQLResultSet* GetProcedures( const TString& catalog,
                                 const TString& schemaPattern,
                                 const TString& procedureNamePattern );
   TSQLResultSet* GetProcedureColumns( const TString& catalog,
                                       const TString& schemaPattern,
                                       const TString& procedureNamePattern,
                                       const TString& columnNamePattern );
   TSQLResultSet* GetTables( const TString& catalog,
                             const TString& schemaPattern,
                             const TString& tableNamePattern,
                             const TString& types );
   TSQLResultSet* GetSchemas();
   TSQLResultSet* GetCatalogs();
   TSQLResultSet* GetTableTypes();
   TSQLResultSet* GetColumns( const TString& catalog,
                              const TString& schemaPattern,
                              const TString& tableNamePattern,
                              const TString& columnNamePattern );
   TSQLResultSet* GetColumnPrivileges( const TString& catalog,
                                       const TString& schema,
                                       const TString& table,
                                       const TString& columnNamePattern );
   TSQLResultSet* GetTablePrivileges( const TString& catalog,
                                      const TString& schemaPattern,
                                      const TString& tableNamePattern );
   TSQLResultSet* GetBestRowIdentifier( const TString& catalog,
                                        const TString& schema,
                                        const TString& table,
                                        Int_t scope,
                                        Bool_t nullable );
   TSQLResultSet* GetVersionColumns( const TString& catalog,
                                     const TString& schema,
                                     const TString& table );
   TSQLResultSet* GetPrimaryKeys( const TString& catalog,
                                  const TString& schema,
                                  const TString& table);
   TSQLResultSet* GetImportedKeys( const TString& catalog,
                                   const TString& schema,
                                   const TString& table );
   TSQLResultSet* GetExportedKeys(  const TString& catalog,
                                    const TString& schema,
                                    const TString& table );
   TSQLResultSet* GetCrossReference( const TString& primaryCatalog,
                                     const TString& primarySchema,
                                     const TString& primaryTable,
                                     const TString& foreignCatalog,
                                     const TString& foreignSchema,
                                     const TString& foreignTable );
   TSQLResultSet* GetTypeInfo();
   TSQLResultSet* GetIndexInfo( const TString& catalog,
                                const TString& schema,
                                const TString& table,
                                Bool_t unique,
                                Bool_t approximate );
   Bool_t SupportsResultSetType(Int_t type);
   Bool_t SupportsResultSetConcurrency( Int_t type,Int_t concurrency );
   Bool_t OwnUpdatesAreVisible(Int_t type);
   Bool_t OwnDeletesAreVisible(Int_t type);
   Bool_t OwnInsertsAreVisible(Int_t type);
   Bool_t OthersUpdatesAreVisible(Int_t type);
   Bool_t OthersDeletesAreVisible(Int_t type);
   Bool_t OthersInsertsAreVisible(Int_t type);
   Bool_t UpdatesAreDetected(Int_t type);
   Bool_t DeletesAreDetected(Int_t type);
   Bool_t InsertsAreDetected(Int_t type);
   Bool_t SupportsBatchUpdates();

ClassDef(ODBCDatabaseMetaData,0) // Provides information about the database as a whole
};

#endif // RDBC_ODBCDatabaseMetaData_h
