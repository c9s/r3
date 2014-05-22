SUBDIRS=src tests

ACLOCAL_AMFLAGS=-I m4

r3_includedir = $(includedir)/r3
r3_include_HEADERS = \
	include/r3.h \
	include/r3_define.h \
	include/r3_list.h \
	include/r3_str.h \
	include/str_array.h \
	include/zmalloc.h \
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
