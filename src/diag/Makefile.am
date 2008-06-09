# -*- makefile -*-
#
# Makefile.am
#
# Copyright (C) 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
#
# $Id: Makefile.am,v 1.7 2002/01/07 17:04:02 seletz Exp $
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


bin_PROGRAMS	= \
	diag-elf32 \
	diag


INCLUDES		+= \
	-I${top_builddir}/include \
	-I${top_srcdir}/include


# WARNING: start.S *must* be the first file, otherwise the target will
# be linked in the wrong order!
diag_elf32_SOURCES	= \
	start.S \
	stack.S \
	command_hist.c \
	commands.c \
	initcalls.c \
	regs-sa11x0.c \
	main.c

EXTRA_diag_elf32_SOURCES		= \
	lcd.c \
	assabet.c \
	badge4.c \
	brutus.c \
	clart.c \
	h3600.c \
	idr.c \
	jornada720.c \
	lart.c \
	nesa.c \
	pleb.c \
	shannon.c \
	system3.c

diag_elf32_DEPENDENCIES	= \
	@LCD@ \
	@DIAG_PLATFORM_OBJ@ \
	ld-script

diag_elf32_LDFLAGS	+= \
	-Wl,-T,${srcdir}/ld-script


diag_elf32_LDADD	+= \
	@LCD@ \
	@DIAG_PLATFORM_OBJ@ \
	-L${top_builddir}/src/lib -lblob \
	-lgcc


diag_SOURCES		=


diag: diag-elf32
	$(OBJCOPY) $(OCFLAGS) $< $@


EXTRA_DIST	= \
	ld-script


CLEANFILES	= ${srcdir}/*~


DISTCLEANFILES	= ${builddir}/.deps/*.P
