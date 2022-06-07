ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I./src @warnings@

SUBDIRS = src

bin_PROGRAMS = ham
ham_SOURCES = src/ham.cpp
ham_LDADD = src/libham.la

dist_doc_DATA = README.md
