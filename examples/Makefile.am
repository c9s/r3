AM_CFLAGS=$(DEPS_CFLAGS) $(GVC_DEPS_CFLAGS) -I$(top_builddir)/include -Wall -std=c99
AM_CXXFLAGS=$(DEPS_CFLAGS) $(GVC_DEPS_CFLAGS) -I$(top_builddir)/include -Wall
AM_LDFLAGS=$(DEPS_LIBS) $(GVC_DEPS_LIBS) $(top_builddir)/libr3.la

noinst_PROGRAMS = simple simple_cpp
simple_SOURCES = simple.c
simple_cpp_SOURCES = simple_cpp.cpp
