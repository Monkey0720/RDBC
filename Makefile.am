# $Id: Makefile.am,v 1.5 2010/12/08 21:17:25 bbannier Exp $
#
#     This file is part of the RDBC
#     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>

EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL INSTALL.local README BUGS TODO RDBC.spec.in

AUTOMAKE_OPTIONS=foreign

SUBDIRS = include macros rdbc odbc

.PHONY: rpm html changelog snapshot

ROOTEXE       = "root.exe -b -q "
MAKECHANGELOG = admin/makechangelog.sh
MAKEHTML      = "$(ROOTEXE) admin/makehtml.C"
MAKELOGHTML   = "$(ROOTEXE) admin/makeloghtml.C"

#
# quick&dirty script to produce rpms
rpm:
	$(MAKE) dist
	-mv $(distdir).tar.gz /usr/src/redhat/SOURCES/
	-rpm -ba @PACKAGE@.spec
	-mv /usr/src/redhat/SRPMS/$(distdir)-1.src.rpm ./$(distdir)-1.src.rpm
	-mv /usr/src/redhat/RPMS/i386/$(distdir)-1.i386.rpm ./$(distdir)_root@ROOT_VER@_gcc@GCC_VER@.i386.rpm

changelog:
	@$(MAKECHANGELOG)

html: changelog
	@$(MAKEHTML)
	@$(MAKELOGHTML)
