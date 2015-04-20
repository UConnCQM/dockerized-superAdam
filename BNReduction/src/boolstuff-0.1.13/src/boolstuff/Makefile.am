# $Id: Makefile.am,v 1.13 2004/12/31 04:50:30 sarrazip Exp $

API = 0.1
# When this is changed, some following variables name must be changed also.

lib_LTLIBRARIES = libboolstuff-0.1.la

libboolstuff_0_1_la_LDFLAGS = -version-info 0:0:0 -no-undefined
#
# Interface 0 is the initial unstable development interface.
# Interface 1 will be the first frozen interface.

libboolstuff_0_1_la_SOURCES = \
	BoolExpr.h \
	BoolExprParser.cpp \
	BoolExprParser.h \
	c-api.cpp \
	c-api.h
#
# BoolExpr.cpp is not mentioned here because it is only meant to be
# included by BoolExpr.h

libboolstuff_0_1_la_CXXFLAGS = -I$(top_srcdir)/src


# Install the header files in (typically) /usr/include/boolstuff:
#
pkgincludedir = $(includedir)/$(PACKAGE)-$(API)/$(PACKAGE)
pkginclude_HEADERS = \
	BoolExpr.h \
	BoolExpr.cpp \
	BoolExprParser.h \
	c-api.h


# Define the test programs:
#
check_PROGRAMS = checkdnf checkc

checkdnf_SOURCES = checkdnf.cpp
checkdnf_CXXFLAGS = -I$(top_srcdir)/src
checkdnf_DEPENDENCIES = lib$(PACKAGE)-$(API).la
checkdnf_LDADD = -l$(PACKAGE)-$(API)

checkc_SOURCES = checkc.c
nodist_checkc_SOURCES = force-cxx-linking.cpp
checkc_CFLAGS = -I$(top_srcdir)/src
checkc_DEPENDENCIES = lib$(PACKAGE)-$(API).la
checkc_LDADD = -l$(PACKAGE)-$(API)

force-cxx-linking.cpp:
	echo 'void dummy();' > $@

check:
	$(PERL) -cw $(srcdir)/check-test-cases.pl
	$(PERL) $(srcdir)/check-test-cases.pl \
		./checkdnf $(srcdir)/test-cases.txt
	@echo ""
	./checkc

doc:
	doxygen boolstuff.dox
	@echo "HTML documentation should now be in 'html' subdirectory."

EXTRA_DIST = \
	check-test-cases.pl \
	test-cases.txt \
	$(PACKAGE).dox

CLEANFILES = force-cxx-linking.cpp

MAINTAINERCLEANFILES = Makefile.in

