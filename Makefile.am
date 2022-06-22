ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I./src
AM_CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic

SUBDIRS = src

bin_PROGRAMS = ham
ham_SOURCES = src/ham.cpp
ham_LDADD = src/libham.la

dist_doc_DATA = README.md
