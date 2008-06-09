# -*- makefile -*-
#
# $Id: Makefile.am,v 1.2 2001/09/18 19:45:29 erikm Exp $
#

bin_PROGRAMS	= \
	mkparamblock

mkparamblock_SOURCES	= \
	mkparamblock.c

mkparamblock: mkparamblock.c
	gcc -Wall -O2 -I${top_builddir}/include -I${top_srcdir}/include -o $@ $<

CLEANFILE	= mkparamblock *~
