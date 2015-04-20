# $Id: configure.ac,v 1.24 2010/02/14 19:38:12 sarrazip Exp $

AC_PREREQ(2.50)
AC_INIT(src/boolstuff/BoolExpr.h)
AC_CANONICAL_HOST
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(boolstuff, 0.1.13)
API=`echo $VERSION | cut -d . -f -2`; AC_SUBST(API)
RELEASE=`echo $VERSION | cut -d . -f 3`; AC_SUBST(RELEASE)

PACKAGE_FULL_NAME="BoolStuff"
PACKAGE_SUMMARY_EN="Disjunctive Normal Form boolean expression library"
PACKAGE_SUMMARY_FR="Librairie calculant la forme normale disjonctive booléenne"
AC_SUBST(PACKAGE_FULL_NAME)
AC_SUBST(PACKAGE_SUMMARY_EN)
AC_SUBST(PACKAGE_SUMMARY_FR)


AC_PROG_CXX
AC_COMPILE_WARNINGS

dnl  For `checkc' target in src/boolstuff/Makefile.am.
dnl  Avoids this warning:
dnl  compiling `checkc.c' with per-target flags requires `AM_PROG_CC_C_O' in `configure.ac'
AM_PROG_CC_C_O

AC_LANG_CPLUSPLUS

AC_DISABLE_STATIC
AM_PROG_LIBTOOL		dnl This must come after AC_DISABLE_STATIC.


# The application has a test suite that is written in Perl.
#
AC_PROG_PERL_MODULES(IPC::Open2)


# Check for getopt_long() but don't fail if it is not available:
#
GETOPT_LONG


# Check for <sstream> and <stdiostream.h>:
#
AC_CXX_HAVE_STD
if test "_$ac_cv_cxx_have_std" = _no; then
	AC_MSG_ERROR([this program requires the ISO C++ standard library
and expects it to be in the std namespace])
fi
AC_CXX_HAVE_SSTREAM
if test "_$ac_cv_cxx_have_sstream" = _no; then
	AC_MSG_ERROR([this program requires the <sstream> C++ header])
fi


AC_CONFIG_FILES([
	Makefile
	boolstuff-0.1.spec
	boolstuff-0.1.pc
	src/Makefile
	src/boolstuff/Makefile
	src/commands/Makefile
	examples/Makefile
])
AC_OUTPUT
