# Makefile
#


include ../Makefile.config

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = -I. \
-I$(SMTLIB2_PREFIX) \
-I$(Z3HEAD_PREFIX) \

ICXXFLAGS = \
-I$(LIBVATA_INCLUDE)

LDFLAGS = \
-L$(SMTLIB2_PREFIX) \
-L$(Z3LIB_PREFIX) \
# -L$(LIBVATA_LIB)

#DEBUG=_debug

#---------------------------------------
# Files
#---------------------------------------

CCINC = bilia_vector.h \
        bilia_option.h \
	bilia_types.h bilia_vars.h bilia_form.h \
	bilia_sat.h \
	bilia_entl.h \
	bilia.h smtlib2bilia.h \
	bilia_nnf.h bilia_z3_api.h\
 	bilia_rewrite.h bilia_step2.h bilia_step3.h bilia_step2_UF.h bilia_step3_UF.h bilia2smt.h bilia_optim_bag.h bilia_var_check.h

CCSRC = bilia_types.c \
	bilia_vars.c \
	bilia_form.c \
        bilia_option.c \
	bilia_sat.c \
 bilia_entl.c \
	bilia.c smtlib2bilia.c bilia_nnf.c\
	bilia_rewrite.c bilia_step2.c bilia_step3.c bilia_step2_UF.c bilia_step3_UF.c bilia2smt.c bilia_optim_bag.c bilia_var_check.c\
	bilia_z3_api.c \
 bilia-dp.c



#---------------------------------------
# Rules
#---------------------------------------

SHARED_LIBS = -lsmtlib2parser \
	-lm \
	-lz3
	# -lvata

all: bats


bats: $(subst .c,$(DEBUG).o,$(CCSRC))
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^ \
	$(LDFLAGS) \
	$(SHARED_LIBS)

clean:
	rm -f *.o *~ *-out.txt *.dot result.txt

distclean: clean
	rm bats

install: bats
	mv bats $(HOME)/bin

indent: $(CCINC) $(CCSRC)
	indent -nut $^

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .c .h .a .o .so .cc

# libvata_bilia_iface.o: libvata_bilia_iface.cc $(CCINC) $(DEPS)
# 	$(CXX) $(CXXFLAGS) $(ICXXFLAGS) -c -o $@ $<
#
# libvata_bilia_iface_debug.o: libvata_bilia_iface.cc $(CCINC) $(DEPS)
# 	$(CXX) $(CXXFLAGS_DEBUG) $(ICXXFLAGS) -c -o $@ $<

%.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) $(ICFLAGS) -c -o $@ $<

%_debug.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -c -o $@ $<
