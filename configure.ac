AC_INIT([r3], 1.0.0)
AC_PREREQ([2.64])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
LT_INIT
AC_PROG_CC
AC_PROG_CC_STDC

AC_CHECK_HEADERS([stdlib.h string.h sys/time.h stdint.h pthread.h])

PKG_CHECK_MODULES(CHECK,[check >= 0.9.4],:,[
  ifdef([AM_PATH_CHECK],
  [AM_PATH_CHECK(,[have_check="yes"])],
  AC_MSG_WARN([Check not found; cannot run unit tests!])
  [have_check="no"])]
])
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strndup strstr])



PKG_PROG_PKG_CONFIG




AC_ARG_WITH([malloc], AS_HELP_STRING([--without-malloc], [Use the default malloc]))

AS_IF([test "x$with_malloc" == "xjemalloc"],
    [AC_CHECK_HEADERS([jemalloc/jemalloc.h], [
        found_jemalloc=yes; break
     ])])

if test "x$found_jemalloc" == "xyes" ; then

AC_MSG_CHECKING([Checking jemalloc version])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <jemalloc/jemalloc.h>]],
                                    [[   
                                     #ifdef JEMALLOC_VERSION_MAJOR > 2
                                     return 0;
                                     #endif
                                     return 1;
                                     ]])],
                   [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([USE_JEMALLOC], 1, [Define to 1 if you have the PATH_MAX macro.])
                      have_jemalloc=yes
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([USE_JEMALLOC], 0, [Define to 1 if you have the PATH_MAX macro.])
                      have_jemalloc=no
                    ]
                 )
fi
AM_CONDITIONAL(USE_JEMALLOC, test "x$have_jemalloc" = "xyes")
AM_CONDITIONAL(ENABLE_GRAPHVIZ, test "x$enable_graphviz" = "xyes")

AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [enable debug]), [],[enable_debug=unset])
AM_CONDITIONAL(ENABLE_DEBUG, test "x$enable_debug" = "xyes")


AC_ARG_ENABLE(graphviz,
              AS_HELP_STRING([--enable-graphviz],
                             [enable graphviz support]),
              , enable_graphviz=unset)
if test "x$enable_graphviz" != "xunset" ; then
   PKG_CHECK_MODULES(GVC_DEPS, [libgvc])
   AC_SUBST(GVC_DEPS_CFLAGS)
   AC_SUBST(GVC_DEPS_LIBS)
fi
AM_CONDITIONAL(ENABLE_GRAPHVIZ, test "x$enable_graphviz" = "xyes")
AC_DEFINE(ENABLE_GRAPHVIZ, test "x$enable_graphviz" = "xyes", "whether graphviz is enable")


AC_ARG_ENABLE(stats, AS_HELP_STRING([--enable-stats], [enable statistics]), [] ,[enable_stats=unset])
AC_DEFINE(ENABLE_STATS,    test "x$enable_stats" = "xyes", "whether statistics is enable")




PKG_CHECK_MODULES(DEPS, [libpcre])
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)
AC_CONFIG_FILES([
  r3.pc
  Makefile
  src/Makefile
  tests/Makefile
])
AC_OUTPUT
