# $Id: Makefile.am,v 1.5 2012/10/17 09:47:41 bbannier Exp $ 
#
#     This file is part of the RDBC
#     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>

EXTRA_DIST = README

AUTOMAKE_OPTIONS=foreign

ODBCCXX_DIR = @ODBCXX_DIR@

lib_LTLIBRARIES=libRDBCodbc.la

libRDBCodbc_la_LDFLAGS= \
   -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
   -release $(LT_RELEASE)

libRDBCodbc_la_LIBADD=-L$(ODBCXX_DIR)/lib/ -lodbc++

INCDIR=$(top_srcdir)/include/RDBC
INCLUDES= -I$(top_srcdir)/include

RDBCodbcDict = RDBCodbcDict.cxx RDBCodbcDict.h
LINKDEF = $(srcdir)/LinkDef.h

libRDBCodbc_la_BUILT_SOURCES = $(RDBCodbcDict)

CLEANFILES=$(RDBCodbcDict)

RDBCodbc_H = \
$(srcdir)/ODBCCallableStatement.h \
$(srcdir)/ODBCConnection.h \
$(srcdir)/ODBCDatabaseMetaData.h \
$(srcdir)/ODBCPreparedStatement.h \
$(srcdir)/ODBCResultSet.h \
$(srcdir)/ODBCResultSetMetaData.h \
$(srcdir)/ODBCStatement.h

libRDBCodbc_la_SOURCES= \
$(libRDBCodbc_la_BUILT_SOURCES) \
ODBCCallableStatement.cxx \
ODBCConnection.cxx \
ODBCDatabaseMetaData.cxx \
ODBCPreparedStatement.cxx \
ODBCResultSet.cxx \
ODBCResultSetMetaData.cxx \
ODBCStatement.cxx


$(RDBCodbcDict): $(RDBCodbc_H) $(LINKDEF) Makefile
	@echo Generating dictionary ... 
	$(ROOTCINT) -f RDBCodbcDict.cxx -c $(INCLUDES) $(RDBCodbc_H) $(LINKDEF)

#	@echo Compiling libodbc++ sources ...
#	cd $(topdir)/$(ODBCXX_DIR); $(MAKE)

dist-hook:
	rm -f $(distdir)/*Dict*
