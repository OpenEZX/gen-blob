# -*- makefile -*-
#
# Makefile.am
#
# Copyright (C) 2005 Motorola Inc.
# Copyright (C) 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
#
# $Id: Makefile.am,v 1.9 2002/01/06 18:59:40 erikm Exp $
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# 2005-Dec-14 - (Motorola) Modified for ezx platform
#

noinst_LIBRARIES	= \
	libblob.a


libblob_a_SOURCES	= \
	command.c \
	error.c \
	icache.c \
	init.c \
	led.c \
	md5.c \
	md5support.c \
	memcpy.c \
	reboot.c \
	serial.c \
	serial-pxa.c \
	strncpy.c \
	strlen.c \
	strncmp.c \
	strtou32.c \
	terminal.c \
	time.c \
	util.c 

INCLUDES		+= \
	-I${top_builddir}/include \
	-I${top_srcdir}/include



CLEANFILES	= ${srcdir}/*~


DISTCLEANFILES	= ${builddir}/.deps/*.P
