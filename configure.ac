AC_INIT([r3], 1.3.3)
AC_PREREQ([2.64])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_INSTALL

# older debian
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

AC_CHECK_HEADERS([stdlib.h string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strndup strstr])
PKG_PROG_PKG_CONFIG



AC_ARG_ENABLE([gcov],
  [AS_HELP_STRING([--enable-gcov],
    [use Gcov to test the test suite])],
    [],
    [enable_gcov=no])
AM_CONDITIONAL([COND_GCOV],[test '!' "$enable_gcov" = no])



AC_ARG_WITH([malloc],
    AS_HELP_STRING([--without-malloc], [Use the default malloc]))

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

# AM_CONDITIONAL(USE_JEMALLOC, test "x$found_jemalloc" = "xyes")
# AC_DEFINE(USE_JEMALLOC, test "x$found_jemalloc" = "xyes" , "use jemalloc")


PKG_CHECK_MODULES(DEPS, [libpcre])
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)


AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[enable debug]))
if test "x$enable_debug" = "xyes"; then
   AC_DEFINE(ENABLE_DEBUG, 1, "debug")
fi
AM_CONDITIONAL(ENABLE_DEBUG, test "x$enable_debug" = "xyes")



AC_ARG_ENABLE(graphviz, AS_HELP_STRING([--enable-graphviz],[enable graphviz support]))
if test "x$enable_graphviz" = "xyes" ; then
   PKG_CHECK_MODULES(GVC_DEPS, [libgvc])
   AC_SUBST(GVC_DEPS_CFLAGS)
   AC_SUBST(GVC_DEPS_LIBS)
   AC_DEFINE(ENABLE_GRAPHVIZ, 1, "whether graphviz is enable")
fi
AM_CONDITIONAL(ENABLE_GRAPHVIZ, test "x$enable_graphviz" = "xyes")




AC_ARG_ENABLE(json, AS_HELP_STRING([--enable-json],[enable json encoder]))
if test "x$enable_json" = "xyes"; then
    PKG_CHECK_MODULES(JSONC, [json-c])
    AC_SUBST(JSONC_CFLAGS)
    AC_SUBST(JSONC_LIBS)
    AC_DEFINE(ENABLE_JSON, 1, [enable json])
fi
AM_CONDITIONAL(ENABLE_JSON, test "x$enable_json" = "xyes")


# This does not work because configure does not look into /opt/local/include...
# AC_CHECK_HEADERS([check.h],[ enable_check=yes ],[ enable_check=unset ])


AC_ARG_ENABLE(check,
              AS_HELP_STRING([--enable-check],
                             [enable unit testing]),
              , enable_check=unset)
if test "x$enable_check" != "xunset" ; then
PKG_CHECK_MODULES(CHECK,[check >= 0.9.4],:,[
  ifdef([AM_PATH_CHECK],
  [AM_PATH_CHECK(,[have_check="yes"])],
  AC_MSG_WARN([Check not found; cannot run unit tests!])
  [have_check="no"])]
])
fi
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

AC_CONFIG_FILES([
  r3.pc
  Makefile
  3rdparty/Makefile
  src/Makefile
  tests/Makefile
  examples/Makefile
])
AC_OUTPUT
