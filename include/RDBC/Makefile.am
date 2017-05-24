# $Id: Makefile.am,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
#
#     This file is part of the RDBC
#     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
#

#pkgincludedir=@includedir@/@PACKAGE@

pkginclude_HEADERS = \
config.h \
TSQL.h \
TSQLCallableStatement.h \
TSQLConnection.h \
TSQLDatabaseMetaData.h \
TSQLDriverInfo.h \
TSQLDriverManager.h \
TSQLImporter.h \
TSQLImportClient.h \
TSQLPreparedStatement.h \
TSQLResultSet.h \
TSQLResultSetMetaData.h \
TSQLStatement.h \
TRDBCServer.h \
TSQLTypes.h \
TSQLUrl.h
 
config.h: $(CONFIG_HEADER)
	sed -e 's/#\([^ ][^ ]*\) \([^ ][^ ]*\)/#\1 RDBC_\2/g' >> $@ < $(CONFIG_HEADER)

CLEANFILES=config.h

# nuke our generated config file
dist-hook:
	rm -f $(distdir)/config.h

uninstall-local:
	test -d $(DESTDIR)$(pkgincludedir) && \
	rmdir $(DESTDIR)$(pkgincludedir) || true
