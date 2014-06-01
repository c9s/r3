AM_CFLAGS=$(DEPS_CFLAGS) $(GVC_DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include -I$(top_builddir)/3rdparty  -Wall -std=c99
AM_LDFLAGS=$(DEPS_LIBS) $(GVC_DEPS_LIBS)

noinst_LTLIBRARIES = libr3ext.la
libr3ext_la_SOURCES = zmalloc.c
libr3ext_la_LIBADD=$(DEPS_LIBS)
# noinst_LIBRARIES = libr3ext.la
libr3ext_la_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/3rdparty  -Wall -std=c99

noinst_HEADERS = \
	zmalloc.h \
	$(NULL)


