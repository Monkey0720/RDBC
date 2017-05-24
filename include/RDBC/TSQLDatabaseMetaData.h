// $Id: TSQLDatabaseMetaData.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLDatabaseMetaData_h
#define RDBC_TSQLDatabaseMetaData_h

//
// Provides several tons of information about a data source
//

#ifndef RDBC_TSQL_h
#include <RDBC/TSQL.h>
#endif

class TSQLResultSet;
class TSQLConnection;
////////////////////////////////////////////////////////////////////
class TSQLDatabaseMetaData: public TSQL
{
friend class TSQLConnection;

protected:
   TSQLConnection* fConnection; // connection that produced this metadata object
   TString  fUrl; // url/dsn

   TSQLDatabaseMetaData( TSQLConnection* connection,void* imp=0 );
   virtual ~TSQLDatabaseMetaData();
  
   void SetURL(const TString& url) { fUrl=url; } 
   void Set(TSQLConnection* connection,void* imp=0) 
                           { fConnection=connection; fImp=imp; }
public:
    enum EMetaDataConstants{
      kBestRowTemporary          = 0,  // SQL_SCOPE_CURROW
      kBestRowTransaction        = 1,  // SQL_SCOPE_TRANSACTION
      kBestRowSession            = 2,  // SQL_SCOPE_SESSION
 
      kBestRowUnknown            = 0,  // SQL_PC_UNKNOWN,
      kBestRowPseudo             = 2,  // SQL_PC_PSEUDO,
      kBestRowNotPseudo          = 1,  // SQL_PC_NOT_PSEUDO
 
      kVersionColumnNotPseudo    = 1,  // SQL_PC_NOT_PSEUDO,
      kVersionColumnPseudo       = 2,  // SQL_PC_PSEUDO,
      kVersionColumnUnknown      = 0,  // SQL_PC_UNKNOWN

      kTypeNoNulls               = 0,  // SQL_NO_NULLS,
      kTypeNullable              = 1,  // SQL_NULLABLE,
      kTypeNullableUnknown       = 2,  // SQL_NULLABLE_UNKNOWN

      kColumnNoNulls             = 0,  // SQL_NO_NULLS,
      kColumnNullable            = 1,  // SQL_NULLABLE,
      kColumnNullableUnknown     = 2,  // SQL_NULLABLE_UNKNOWN

      kTypePredNone              = 0,  // SQL_UNSEARCHABLE,
      kTypePredChar              = 1,  // SQL_LIKE_ONLY,
      kTypePredBasic             = 2,  // SQL_ALL_EXCEPT_LIKE,
      kTypeSearchable            = 3,  // SQL_SEARCHABLE

      kImportedKeyCascade        = 0x00000020L,    // SQL_CASCADE,
      kImportedKeySetNull        = 2,              // SQL_SET_NULL,
      kImportedKeySetDefault     = 4,              // SQL_SET_DEFAULT,
      kImportedKeyNoAction       = 3,              // SQL_NO_ACTION,
      kImportedKeyRestrict       = 1,              // SQL_RESTRICT

      kImportedKeyInitiallyDeferred    = 5,  // SQL_INITIALLY_DEFERRED,
      kImportedKeyInitiallyImmediate   = 6,  // SQL_INITIALLY_IMMEDIATE,
      kImportedKeyNotDeferrable        = 7,  // SQL_NOT_DEFERRABLE

      kTableIndexClustered       = 1,  // SQL_INDEX_CLUSTERED,
      kTableIndexHashed          = 2,  // SQL_INDEX_HASHED,
      kTableIndexOther           = 3,  // SQL_INDEX_OTHER,
      kTableIndexStatistic       = 0,  // SQL_TABLE_STAT

      kProcedureColumnIn         = 1, // SQL_PARAM_INPUT,
      kProcedureColumnInOut      = 2, // SQL_PARAM_INPUT_OUTPUT,
      kProcedureColumnOut        = 4, // SQL_PARAM_OUTPUT,
      kProcedureColumnResult     = 3, // SQL_RESULT_COL,
      kProcedureColumnReturn     = 5, // SQL_RETURN_VALUE,
      kProcedureColumnUnknown    = 0, // SQL_PARAM_TYPE_UNKNOWN

      kProcedureNoNulls          = 0,  // SQL_NO_NULLS,
      kProcedureNullable         = 1,  // SQL_NULLABLE,
      kProcedureNullableUnknown  = 2,  // SQL_NULLABLE_UNKNOWN

      kProcedureReturnsResult    = 2,  // SQL_PT_FUNCTION,
      kProcedureNoResult         = 1,  // SQL_PT_PROCEDURE,
      kProcedureResultUnknown    = 0   // SQL_PT_UNKNOWN
    };

public:
   virtual Bool_t    AllProceduresAreCallable() = 0;
   virtual Bool_t    AllTablesAreSelectable() = 0;
   virtual TString   GetUserName() = 0;
   virtual Bool_t    IsReadOnly() = 0;
   virtual Bool_t    NullsAreSortedHigh() = 0;
   virtual Bool_t    NullsAreSortedLow() = 0;
   virtual Bool_t    NullsAreSortedAtStart() = 0;
   virtual Bool_t    NullsAreSortedAtEnd() = 0;
   virtual TString   GetDatabaseProductName() = 0;
   virtual TString   GetDatabaseProductVersion() = 0;
   virtual TString   GetDriverName() = 0;
   virtual TString   GetDriverVersion() = 0;
   virtual Int_t     GetDriverMajorVersion() = 0;
   virtual Int_t     GetDriverMinorVersion() = 0;
   virtual Bool_t    UsesLocalFiles() = 0;
   virtual Bool_t    UsesLocalFilePerTable() = 0;
   virtual Bool_t    SupportsMixedCaseIdentifiers() = 0;
   virtual Bool_t    StoresUpperCaseIdentifiers() = 0;
   virtual Bool_t    StoresLowerCaseIdentifiers() = 0;
   virtual Bool_t    StoresMixedCaseIdentifiers() = 0;
   virtual Bool_t    SupportsMixedCaseQuotedIdentifiers() = 0;
   virtual Bool_t    StoresUpperCaseQuotedIdentifiers() = 0;
   virtual Bool_t    StoresLowerCaseQuotedIdentifiers() = 0;
   virtual Bool_t    StoresMixedCaseQuotedIdentifiers() = 0;
   virtual TString   GetIdentifierQuoteString() = 0;
   virtual TString   GetSQLKeywords() = 0;
   virtual TString   GetNumericFunctions() = 0;
   virtual TString   GetStringFunctions() = 0;
   virtual TString   GetSystemFunctions() = 0;
   virtual TString   GetTimeDateFunctions() = 0;
   virtual TString   GetSearchStringEscape() = 0;
   virtual TString   GetExtraNameCharacters() = 0;
   virtual Bool_t    SupportsAlterTableWithAddColumn() = 0;
   virtual Bool_t    SupportsAlterTableWithDropColumn() = 0;
   virtual Bool_t    SupportsColumnAliasing() = 0;
   virtual Bool_t    NullPlusNonNullIsNull() = 0;
   virtual Bool_t    SupportsConvert() = 0;
   virtual Bool_t    SupportsConvert(Int_t fromType,Int_t toType) = 0;
   virtual Bool_t    SupportsTableCorrelationNames() = 0;
   virtual Bool_t    SupportsDifferentTableCorrelationNames() = 0;
   virtual Bool_t    SupportsExpressionsInOrderBy() = 0;
   virtual Bool_t    SupportsOrderByUnrelated() = 0;
   virtual Bool_t    SupportsGroupBy() = 0;
   virtual Bool_t    SupportsGroupByUnrelated() = 0;
   virtual Bool_t    SupportsGroupByBeyondSelect() = 0;
   virtual Bool_t    SupportsLikeEscapeClause() = 0;
   virtual Bool_t    SupportsMultipleResultSets() = 0;
   virtual Bool_t    SupportsMultipleTransactions() = 0;
   virtual Bool_t    SupportsNonNullableColumns() = 0;
   virtual Bool_t    SupportsMinimumSQLGrammar() = 0;
   virtual Bool_t    SupportsCoreSQLGrammar() = 0;
   virtual Bool_t    SupportsExtendedSQLGrammar() = 0;
   virtual Bool_t    SupportsANSI92EntryLevelSQL() = 0;
   virtual Bool_t    SupportsANSI92IntermediateSQL() = 0;
   virtual Bool_t    SupportsANSI92FullSQL() = 0;
   virtual Bool_t    SupportsIntegrityEnhancementFacility() = 0;
   virtual Bool_t    SupportsOuterJoins() = 0;
   virtual Bool_t    SupportsFullOuterJoins() = 0;
   virtual Bool_t    SupportsLimitedOuterJoins() = 0;
   virtual TString   GetSchemaTerm() = 0;
   virtual TString   GetTableTerm() = 0;   
   virtual TString   GetProcedureTerm() = 0;
   virtual TString   GetCatalogTerm() = 0;
   virtual Bool_t    IsCatalogAtStart() = 0;
   virtual TString   GetCatalogSeparator() = 0;
   virtual Bool_t    SupportsSchemasInDataManipulation() = 0;
   virtual Bool_t    SupportsSchemasInProcedureCalls() = 0;
   virtual Bool_t    SupportsSchemasInTableDefinitions() = 0;
   virtual Bool_t    SupportsSchemasInIndexDefinitions() = 0;
   virtual Bool_t    SupportsSchemasInPrivilegeDefinitions() = 0;
   virtual Bool_t    SupportsCatalogsInDataManipulation() = 0;
   virtual Bool_t    SupportsCatalogsInProcedureCalls() = 0;
   virtual Bool_t    SupportsCatalogsInTableDefinitions() = 0;
   virtual Bool_t    SupportsCatalogsInIndexDefinitions() = 0;
   virtual Bool_t    SupportsCatalogsInPrivilegeDefinitions() = 0;
   virtual Bool_t    SupportsPositionedDelete() = 0;
   virtual Bool_t    SupportsPositionedUpdate() = 0;
   virtual Bool_t    SupportsSelectForUpdate() = 0;
   virtual Bool_t    SupportsStoredProcedures() = 0;
   virtual Bool_t    SupportsSubqueriesInComparisons() = 0;
   virtual Bool_t    SupportsSubqueriesInExists() = 0;
   virtual Bool_t    SupportsSubqueriesInIns() = 0;
   virtual Bool_t    SupportsSubqueriesInQuantifieds() = 0;
   virtual Bool_t    SupportsCorrelatedSubqueries() = 0;
   virtual Bool_t    SupportsUnion() = 0;
   virtual Bool_t    SupportsUnionAll() = 0;
   virtual Bool_t    SupportsOpenCursorsAcrossRollback() = 0;
   virtual Bool_t    SupportsOpenStatementsAcrossCommit() = 0;
   virtual Bool_t    SupportsOpenStatementsAcrossRollback() = 0;
   virtual Int_t     GetMaxBinaryLiteralLength() = 0;
   virtual Int_t     GetMaxCharLiteralLength() = 0;
   virtual Int_t     GetMaxColumnNameLength() = 0;
   virtual Int_t     GetMaxColumnsInGroupBy() = 0;
   virtual Int_t     GetMaxColumnsInIndex() = 0;
   virtual Int_t     GetMaxColumnsInOrderBy() = 0;
   virtual Int_t     GetMaxColumnsInSelect() = 0;
   virtual Int_t     GetMaxColumnsInTable() = 0;
   virtual Int_t     GetMaxConnections() = 0;
   virtual Int_t     GetMaxCursorNameLength() = 0;
   virtual Int_t     GetMaxIndexLength() = 0;
   virtual Int_t     GetMaxSchemaNameLength() = 0;
   virtual Int_t     GetMaxProcedureNameLength() = 0;
   virtual Int_t     GetMaxCatalogNameLength() = 0;
   virtual Int_t     GetMaxRowSize() = 0;
   virtual Bool_t    DoesMaxRowSizeIncludeBlobs() = 0;
   virtual Int_t     GetMaxStatementLength() = 0;
   virtual Int_t     GetMaxStatements() = 0;
   virtual Int_t     GetMaxTableNameLength() = 0;
   virtual Int_t     GetMaxTablesInSelect() = 0;
   virtual Int_t     GetMaxUserNameLength() = 0;
   virtual Int_t     GetDefaultTransactionIsolation() = 0;
   virtual Bool_t    SupportsTransactions() = 0;
   virtual Bool_t    SupportsTransactionIsolationLevel(Int_t level) = 0;
   virtual Bool_t    SupportsDataDefinitionAndDataManipulationTransactions() = 0;
   virtual Bool_t    SupportsDataManipulationTransactionsOnly() = 0;
   virtual Bool_t    DataDefinitionCausesTransactionCommit() = 0;
   virtual Bool_t    DataDefinitionIgnoredInTransactions() = 0;
   virtual Bool_t    SupportsResultSetType(Int_t type) = 0;
   virtual Bool_t    SupportsResultSetConcurrency( Int_t type,Int_t concurrency ) = 0;
   virtual Bool_t    OwnUpdatesAreVisible(Int_t type) = 0;
   virtual Bool_t    OwnDeletesAreVisible(Int_t type) = 0;
   virtual Bool_t    OwnInsertsAreVisible(Int_t type) = 0;
   virtual Bool_t    OthersUpdatesAreVisible(Int_t type) = 0;
   virtual Bool_t    OthersDeletesAreVisible(Int_t type) = 0;
   virtual Bool_t    OthersInsertsAreVisible(Int_t type) = 0;
   virtual Bool_t    UpdatesAreDetected(Int_t type) = 0;
   virtual Bool_t    DeletesAreDetected(Int_t type) = 0;
   virtual Bool_t    InsertsAreDetected(Int_t type) = 0;
   virtual Bool_t    SupportsBatchUpdates() = 0;
   virtual TSQLResultSet* GetProcedures( const TString& catalog,
                                       const TString& schemaPattern,
                                       const TString& procedureNamePattern ) = 0;
   virtual TSQLResultSet* GetProcedureColumns( const TString& catalog,
                                             const TString& schemaPattern,
                                             const TString& procedureNamePattern,
                                             const TString& columnNamePattern ) = 0;
   virtual TSQLResultSet* GetTables( const TString& catalog,
                                 const TString& schemaPattern,
                                 const TString& tableNamePattern,
                                 const TString& types ) = 0;
   virtual TSQLResultSet* GetSchemas() = 0;
   virtual TSQLResultSet* GetCatalogs() = 0;
   virtual TSQLResultSet* GetTableTypes() = 0;
   virtual TSQLResultSet* GetColumns( const TString& catalog,
                                    const TString& schemaPattern,
                                    const TString& tableNamePattern,
                                    const TString& columnNamePattern ) = 0;
   virtual TSQLResultSet* GetColumnPrivileges( const TString& catalog,
                                             const TString& schema,
                                             const TString& table,
                                             const TString& columnNamePattern ) = 0;
   virtual TSQLResultSet* GetTablePrivileges( const TString& catalog,
                                             const TString& schemaPattern,
                                             const TString& tableNamePattern ) = 0;
   virtual TSQLResultSet* GetBestRowIdentifier( const TString& catalog,
                                                const TString& schema,
                                                const TString& table,
                                                Int_t scope,
                                                Bool_t nullable ) = 0;
   virtual TSQLResultSet* GetVersionColumns( const TString& catalog,
                                     const TString& schema,
                                     const TString& table ) = 0;
   virtual TSQLResultSet* GetPrimaryKeys( const TString& catalog,
                                  const TString& schema,
                                  const TString& table) = 0;
   virtual TSQLResultSet* GetImportedKeys( const TString& catalog,
                                   const TString& schema,
                                   const TString& table ) = 0;
   virtual TSQLResultSet* GetExportedKeys(  const TString& catalog,
                                          const TString& schema,
                                          const TString& table ) = 0;
   virtual TSQLResultSet* GetCrossReference( const TString& primaryCatalog,
                                          const TString& primarySchema,
                                          const TString& primaryTable,
                                          const TString& foreignCatalog,
                                          const TString& foreignSchema,
                                          const TString& foreignTable ) = 0;
   virtual TSQLResultSet* GetTypeInfo() = 0;
   virtual TSQLResultSet* GetIndexInfo( const TString& catalog,
                                       const TString& schema,
                                       const TString& table,
                                       Bool_t unique,
                                       Bool_t approximate ) = 0;

   TSQLConnection* GetConnection() const { return fConnection; }
   virtual TString GetURL() const { return fUrl; }

ClassDef(TSQLDatabaseMetaData,0) // Provides information about the database as a whole
};

#endif // RDBC_TSQLDatabaseMetaData_h
