AC_INIT([r3], 1.0.0)
AC_PREREQ([2.64])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
LT_INIT
AC_PROG_CC
AC_PROG_CC_STDC

# AM_PATH_CHECK()
PKG_CHECK_MODULES(CHECK,[check >= 0.9.4],:,[
  ifdef([AM_PATH_CHECK],
  [AM_PATH_CHECK(,[have_check="yes"])],
  AC_MSG_WARN([Check not found; cannot run unit tests!])
  [have_check="no"])]
])
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

AC_CHECK_HEADERS([stdlib.h string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strndup strstr])



PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE(graphviz,
              AS_HELP_STRING([--enable-graphviz],
                             [enable graphviz support]),
              , enable_graphviz=unset)
if test "x$enable_graphviz" != "xunset" ; then
   PKG_CHECK_MODULES(GVC_DEPS, [libgvc])
   AC_SUBST(GVC_DEPS_CFLAGS)
   AC_SUBST(GVC_DEPS_LIBS)
fi

AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug],
                             [enable debug]),
              , enable_debug=unset)

AC_ARG_WITH([malloc],
    AS_HELP_STRING([--without-malloc], [Use the default malloc]))

AS_IF([test "x$with_malloc" != "xno"],
    [AC_CHECK_HEADERS([jemalloc/jemalloc.h], [
        found_jemalloc=yes; break
     ])])
AM_CONDITIONAL(USE_JEMALLOC, test "x$found_jemalloc" = "xyes")
AC_DEFINE(USE_JEMALLOC, test "x$found_jemalloc" = "xyes" , "use jemalloc")

AM_CONDITIONAL(ENABLE_DEBUG, test "x$enable_debug" == "xyes")
AM_CONDITIONAL(ENABLE_GRAPHVIZ, test "x$enable_graphviz" == "xyes")
AC_DEFINE(ENABLE_GRAPHVIZ, test "x$enable_graphviz" == "xyes", "whether graphviz is enable")


PKG_CHECK_MODULES(DEPS, [libpcre check])
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)
AC_CONFIG_FILES([
  r3.pc
  Makefile
  src/Makefile
  tests/Makefile
])
AC_OUTPUT
