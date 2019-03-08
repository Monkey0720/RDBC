# $Id: Makefile.am,v 1.4 2010/12/08 21:17:26 bbannier Exp $ 
#
#     This file is part of the RDBC
#     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>

EXTRA_DIST = README

AUTOMAKE_OPTIONS  = foreign

lib_LTLIBRARIES=libRDBC.la


AM_CPPFLAGS = -I$(top_srcdir)/include

RDBCDict = RDBC_Dict.cc RDBCDict.h
LINKDEF = $(srcdir)/LinkDef.h

libRDBC_la_BUILT_SOURCES = $(RDBCDict)

CLEANFILES=$(RDBCDict)

RDBC_H = \
  $(top_srcdir)/include/RDBC/TSQL.h \
  $(top_srcdir)/include/RDBC/TSQLCallableStatement.h \
  $(top_srcdir)/include/RDBC/TSQLConnection.h \
  $(top_srcdir)/include/RDBC/TSQLDatabaseMetaData.h \
  $(top_srcdir)/include/RDBC/TSQLDriverInfo.h \
  $(top_srcdir)/include/RDBC/TSQLDriverManager.h \
  $(top_srcdir)/include/RDBC/TSQLImporter.h \
  $(top_srcdir)/include/RDBC/TSQLImportClient.h \
  $(top_srcdir)/include/RDBC/TSQLPreparedStatement.h \
  $(top_srcdir)/include/RDBC/TSQLResultSet.h \
  $(top_srcdir)/include/RDBC/TSQLResultSetMetaData.h \
  $(top_srcdir)/include/RDBC/TSQLStatement.h \
  $(top_srcdir)/include/RDBC/TRDBCServer.h \
  $(top_srcdir)/include/RDBC/TSQLTypes.h \
  $(top_srcdir)/include/RDBC/TSQLUrl.h

ROOTDICTS = \
  $(libRDBC_la_BUILT_SOURCES) 

if MAKEROOT6
pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  RDBC_Dict_rdict.pcm
endif

libRDBC_la_SOURCES= \
  $(ROOTDICTS) \
  $(srcdir)/TSQL.cxx \
  $(srcdir)/TSQLCallableStatement.cxx \
  $(srcdir)/TSQLConnection.cxx \
  $(srcdir)/TSQLDatabaseMetaData.cxx \
  $(srcdir)/TSQLDriverInfo.cxx \
  $(srcdir)/TSQLDriverManager.cxx \
  $(srcdir)/TSQLPreparedStatement.cxx \
  $(srcdir)/TSQLResultSet.cxx \
  $(srcdir)/TSQLResultSetMetaData.cxx \
  $(srcdir)/TSQLStatement.cxx \
  $(srcdir)/TRDBCServer.cxx \
  $(srcdir)/TSQLTypes.cxx \
  $(srcdir)/TSQLUrl.cxx \
  $(srcdir)/TSQLImporter.cxx \
  $(srcdir)/TSQLImportClient.cxx


$(RDBCDict): $(RDBC_H) $(LINKDEF) Makefile
	@echo Generating dictionary ... 
	$(ROOTCINT) -f $@ @CINTDEFS@  -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $(RDBC_H) $(LINKDEF)

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;
