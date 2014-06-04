SUBDIRS=3rdparty src . tests examples

lib_LTLIBRARIES = libr3.la
libr3_la_SOURCES =
libr3_la_LIBADD = 3rdparty/libr3ext.la src/libr3core.la
libr3_la_LDFLAGS =

AM_CFLAGS=$(DEPS_CFLAGS) $(GVC_DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include -I$(top_builddir)/3rdparty -Wall -std=c99
AM_LDFLAGS=$(DEPS_LIBS) $(GVC_DEPS_LIBS)


ACLOCAL_AMFLAGS=-I m4

if ENABLE_DEBUG
AM_CFLAGS += -ggdb -fprofile-arcs -ftest-coverage
endif

if USE_JEMALLOC
AM_LDFLAGS += -ljemalloc
endif

r3_includedir = $(includedir)/r3
r3_include_HEADERS = \
	include/r3.h \
	include/r3_define.h \
	include/r3_list.h \
	include/r3_str.h \
	include/r3_gvc.h \
	include/r3_json.h \
	include/str_array.h \
	include/r3.hpp \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = r3.pc

EXTRA_DIST = \
	autogen.sh \
	bench.html \
	demo.c \
	gen_routes.rb \
	HACKING.md \
	LICENSE \
	README.md \
	$(NULL)
