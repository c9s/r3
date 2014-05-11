AC_INIT([r2],[1.4],[yoanlin93@gmail.com],[r2],[http://github.com/c9s/r2])
# AC_CONFIG_AUX_DIR(config)
# AC_CONFIG_SRCDIR(Front/mona.cpp)
# AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([config.h])

CFLAGS=$CXXFLAGS
AC_SUBST(CFLAGS)

AC_HEADER_STDC

AC_CHECK_HEADERS(stdlib.h string.h sys/time.h unistd.h
                 ,,AC_MSG_ERROR(required header file not found))

dnl ========== checks for typedefs, structures, and compiler characteristics ==========

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_OUTPUT
