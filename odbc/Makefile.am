# $Id: Makefile.am,v 1.5 2012/10/17 09:47:41 bbannier Exp $ 
#
#     This file is part of the RDBC
#     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>

EXTRA_DIST = README

AUTOMAKE_OPTIONS=foreign

ODBCCXX_DIR = @ODBCXX_DIR@

lib_LTLIBRARIES=libRDBCodbc.la

libRDBCodbc_la_LIBADD=-L$(ODBCXX_DIR)/lib/ -lodbc++

INCDIR=$(top_srcdir)/include/RDBC
AM_CPPFLAGS = -I$(top_srcdir)/include

RDBCodbcDict = RDBCodbc_Dict.cc RDBCodbcDict.h
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

pkginclude_HEADERS = \
  $(RDBCodbc_H)

  ROOTDICTS = \
    $(libRDBCodbc_la_BUILT_SOURCES) 

if  MAKEROOT6
pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  RDBCodbc_Dict_rdict.pcm
endif

libRDBCodbc_la_SOURCES= \
  $(ROOTDICTS) \
  ODBCCallableStatement.cxx \
  ODBCConnection.cxx \
  ODBCDatabaseMetaData.cxx \
  ODBCPreparedStatement.cxx \
  ODBCResultSet.cxx \
  ODBCResultSetMetaData.cxx \
  ODBCStatement.cxx


$(RDBCodbcDict): $(RDBCodbc_H) $(LINKDEF) Makefile
	@echo Generating dictionary ... 
	$(ROOTCINT) -f $@ @CINTDEFS@  -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $(RDBCodbc_H) $(LINKDEF)

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

#	@echo Compiling libodbc++ sources ...
#	cd $(topdir)/$(ODBCXX_DIR); $(MAKE)

dist-hook:
	rm -f $(distdir)/*Dict*
