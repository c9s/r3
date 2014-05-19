lib_LTLIBRARIES = libr3.la
# lib_LIBRARIES = libr3.a
libr3_la_SOURCES = node.c edge.c str.c token.c

libr3_la_LIBADD=$(DEPS_LIBS)
AM_CFLAGS=$(DEPS_CFLAGS) -I$(top_builddir) -I$(top_builddir)/include -Wall

if ENABLE_DEBUG
AM_CFLAGS += -g -fprofile-arcs -ftest-coverage
endif

if ENABLE_GRAPHVIZ
libr3_la_SOURCES += gvc.c
libr3_la_LIBADD += $(GVC_DEPS_LIBS)
AM_CFLAGS += $(GVC_DEPS_CFLAGS)
endif

# AM_CFLAGS=$(DEPS_CFLAGS)
