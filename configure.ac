AC_INIT([r3], 1.0)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
LT_INIT
AC_PROG_CC

# AM_PATH_CHECK()
AM_PATH_CHECK(,[have_check="yes"],
  AC_MSG_WARN([Check not found; cannot run unit tests!])
  [have_check="no"])
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(DEPS, [libpcre check])
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)
AC_CONFIG_FILES(Makefile src/Makefile tests/Makefile)
AC_OUTPUT
