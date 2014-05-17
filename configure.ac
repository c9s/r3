AC_INIT([r3], 1.0)
AC_PREREQ([2.64])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
LT_INIT
AC_PROG_CC
AC_PROG_CC_STDC

# AM_PATH_CHECK()
AM_PATH_CHECK(,[have_check="yes"],
  AC_MSG_WARN([Check not found; cannot run unit tests!])
  [have_check="no"])
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

AC_CHECK_HEADERS([stdlib.h string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strndup strstr])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(DEPS, [libpcre check gvc])
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)
AC_CONFIG_FILES([
  r3.pc
  Makefile
  src/Makefile
  tests/Makefile
])
AC_OUTPUT
