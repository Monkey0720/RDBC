TBINS := $(subst .C,,$(wildcard *.C))
BINCXXFILES =  $(wildcard *.C)
COMPLEXBIN = $(TBINS)


SCRIPT_DIR := $(shell if [ -d $(SRT_PRIVATE_CONTEXT)/RDBC/test/ ] ; then \
echo $(SRT_PRIVATE_CONTEXT)/RDBC/test/ ; \
else echo $(SRT_PUBLIC_CONTEXT)/RDBC/test; \
fi)

INC_PACK = $(SRT_PRIVATE_CONTEXT)/$(PACKAGE)
INC_mysql = $(INC_PACK)/mysql

override CPPFLAGS += -I$(INC_mysql) \
			 -DSTANDALONE




BINLIBS = -l$(PACKAGE) \
          $(MINOSLIBS)  \
	-lLeakChecker -lValidity -lConventions -lRDBC -lRDBCodbc -lodbcpp \
	 $(ROOTLIBS) $(ROOTGLIBS)


TEST_% : $(bindir)%
	@echo '<**testing**>' $*
	@${SCRIPT_DIR}/runit.pl $* $(bindir) $(curdir) $(workdir)

REF_% : $(bindir)%
	@echo '<**reference**>' $*
	@${SCRIPT_DIR}/runit.pl $* $(bindir) $(curdir) $(curdir)

do_test: $(foreach i,$(TBINS), TEST_$(i))
do_ref: $(foreach i,$(TBINS), REF_$(i))

test: do_test
ref: do_ref
foo:
	@echo workdir is $(workdir)
	@echo curdir is $(curdir)
	@echo bindir is $(bindir)
	@echo BINCXXFILES  is $(BINCXXFILES)
	@echo TBINS  is $(TBINS)
	@echo PACKAGE  is $(PACKAGE)
	@echo INC_RDBC is $(INC_RDBC)
	@echo INC_odbcplusplus is $(INC_odbcplusplus)
	@echo SCRIPT_DIR is $(SCRIPT_DIR)



############################################################
include SoftRelTools/standard.mk
include SoftRelTools/arch_spec_root.mk
include SoftRelTools/arch_spec_MySQL.mk
include SoftRelTools/arch_spec_sigc++.mk
