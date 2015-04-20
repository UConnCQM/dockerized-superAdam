dnl  AC_COMPILE_WARNINGS
dnl
dnl  Set the maximum warning verbosity according to compiler
dnl  used. Currently supports g++ and gcc. This macro must be put after
dnl  AC_PROG_CC and AC_PROG_CXX in configure.in
dnl
dnl  Author: Loic Dachary
dnl
dnl  Source:
dnl  http://www.gnu.org/software/ac-archive/htmldoc/ac_compile_warnings.html

AC_DEFUN([AC_COMPILE_WARNINGS],
[AC_MSG_CHECKING(maximum warning verbosity option)
if test -n "$CXX"
then
  if test "$GXX" = "yes"
  then
    ac_compile_warnings_opt='-Wall'
  fi
  CXXFLAGS="$CXXFLAGS $ac_compile_warnings_opt"
  ac_compile_warnings_msg="$ac_compile_warnings_opt for C++"
fi

if test -n "$CC"
then
  if test "$GCC" = "yes"
  then
    ac_compile_warnings_opt='-Wall'
  fi
  CFLAGS="$CFLAGS $ac_compile_warnings_opt"
  ac_compile_warnings_msg="$ac_compile_warnings_msg $ac_compile_warnings_opt for C"
fi
AC_MSG_RESULT($ac_compile_warnings_msg)
unset ac_compile_warnings_msg
unset ac_compile_warnings_opt
])
