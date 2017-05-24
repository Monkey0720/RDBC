#     This file is part of the RDBC
#     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
#
#     This directory contains some RDBC test macros.

data_DATA = \
README.RDBC \
RDBCmysql.C \
RDBCbench.C \
RDBCform.C \
RDBCconsumer.C \
RDBCemp.C \
RDBCfirst.C \
RDBCproducer.C \
RDBCscroll.C \
demobld.mysql 
            
datadir = @prefix@/macros/@PACKAGE@/

EXTRA_DIST = $(data_DATA)

uninstall-local:
	test -d $(datadir) && \
  	rmdir $(datadir) || true
