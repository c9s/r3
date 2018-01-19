dnl config.m4 for extension r3

PHP_ARG_WITH(r3, for r3 support,
[  --with-r3             Include r3 support])

dnl PHP_ARG_ENABLE(r3, whether to enable r3 support,
dnl Make sure that the comment is aligned:
dnl [  --enable-r3           Enable r3 support])

if test "$PHP_R3" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/r3/r3.h"
  if test -r $PHP_R3/$SEARCH_FOR; then
    R3_DIR=$PHP_R3
  else
    AC_MSG_CHECKING([for r3 files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        R3_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$R3_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the r3 distribution])
  fi

echo $R3_DIR
  dnl # --with-r3 -> add include path
  PHP_ADD_INCLUDE($R3_DIR/include)

  LIBNAME=r3
  LIBSYMBOL=r3_route_create

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $R3_DIR/lib, R3_SHARED_LIBADD)
    AC_DEFINE(HAVE_R3LIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong r3 lib version or lib not found])
  ],[
    -L$R3_DIR/lib -lm
  ])

  PHP_SUBST(R3_SHARED_LIBADD)

  PHP_NEW_EXTENSION(r3, [ct_helper.c hash.c php_expandable_mux.c php_r3.c r3_controller.c r3_functions.c r3_mux.c r3_persistent.c], $ext_shared)
fi
