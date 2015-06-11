#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([analytics-kitten-tcasm-local.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_FC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])
AC_CHECK_LIB([m], [round])
AC_CHECK_LIB([mpi], [mpi_init])
# FIXME: Replace `main' with a function in `-lxpmem':
AC_ARG_WITH([xpmem],
			[AS_HELP_STRING([--with-xpmem],
							[path to nonstandard location for xpmem library])],
							[
							LDFLAGS="-L$withval/lib $LDFLAGS"
							CFLAGS="-I$withval/include $CFLAGS"
							],
							[])
AC_CHECK_LIB([xpmem], [xpmem_get], [], [AC_MSG_ERROR([xpmem required but not found])])

AC_ARG_WITH([hobbes],
			[AS_HELP_STRING([--with-hobbes],
							[path to nonstandard location for the hobbes library])],
							[
							LDFLAGS="-L$withval $LDFLAGS"
							CFLAGS="-I$withval $CFLAGS"
							],
							[])
AC_CHECK_LIB([hobbes], [xemem_get], [], [AC_MSG_ERROR([hobbes required but not found])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h xpmem.h xemem.h]) #, [], [AC_MSG_ERROR([required header not found]])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MMAP
AC_CHECK_FUNCS([munmap])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
