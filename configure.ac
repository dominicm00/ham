AC_PREREQ([2.69])
AC_INIT([Ham], [0.1.0], [dom@dominicm.dev], [ham],
  [https://github.com/dominicm00/ham])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([Jamfile])
AC_CONFIG_FILES([
  Makefile
  ])

AC_PROG_CXX

AM_PROG_AR

LT_INIT([disable-shared])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AM_SILENT_RULES([yes])

dnl TODO: fix warnings
dnl AC_SUBST([warnings], ["-Wall -Wextra -Wpedantic -Werror"])
AC_SUBST([warnings], ["-Wall -Wextra -Wpedantic"])

AC_OUTPUT
