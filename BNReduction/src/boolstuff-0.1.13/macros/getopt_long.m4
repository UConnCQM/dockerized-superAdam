# Defines an automake conditional `HAVE_GETOPT_LONG' depending on
# whether getopt_long() from GNU getopt is available or not.  If it
# is, defines the preprocessor identifier `HAVE_GETOPT_LONG' to 1.

AC_PREREQ(2.50)

AC_DEFUN([GETOPT_LONG],
[
	AC_CHECK_HEADER([getopt.h])
	AC_CHECK_FUNCS([getopt_long], HAVE_GETOPT_LONG=1, HAVE_GETOPT_LONG=0)
	AM_CONDITIONAL(HAVE_GETOPT_LONG, [test "$HAVE_GETOPT_LONG" = 1])
])


