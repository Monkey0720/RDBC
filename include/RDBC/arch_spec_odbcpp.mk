# arch_spec_odbcpp.mk
# Architecture/site specific makefile fragment
#   for inclusion by packages that use odbc++
#

RDBC_DIR := $(shell if [ -d $(SRT_PRIVATE_CONTEXT)/RDBC ] ; then \
echo $(SRT_PRIVATE_CONTEXT)/RDBC ; \
else echo $(SRT_PUBLIC_CONTEXT)/RDBC ; \
fi)
INC_RDBC = $(RDBC_DIR)/include/$(PACKAGE)
INC_odbc = $(RDBC_DIR)/odbc
INC_odbcplusplus = $(RDBC_DIR)/odbc++
INC_mysql = $(RDBC_DIR)/mysql

override CPPFLAGS += -I$(INC_RDBC) \
                        -I$(INC_odbc) \
                        -I$(INC_odbcplusplus) \
                        -I$(INC_mysql) \



include SoftRelTools/specialize_arch_spec.mk

override LDFLAGS   +=  -lodbc++ -lRDBC -lRDBCodbc
override MINOSLIBS +=  -lodbc++  -lRDBC -lRDBCodbc
