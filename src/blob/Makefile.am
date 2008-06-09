# -*- makefile -*-
#
# Makefile.am
#
# Copyright (C) 2005 Motorola Inc.
# Copyright (C) 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
#
# $Id: Makefile.am,v 1.15 2002/01/07 15:00:14 erikm Exp $
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
# 2005-Jun-06 - (Motorola) changed for ezx platform
#

bin_PROGRAMS	= \
	blob-start-elf32 \
	blob-start \
	blob-start-chain-elf32 \
	blob-start-chain \
	blob-rest-elf32 \
	blob-rest \
	blob \
	blob-chain


INCLUDES		+= \
	-I${top_builddir}/include \
	-I${top_srcdir}/include


# ---- Built sources -------------------------------------------------

BUILT_SOURCES = \
	rest-ld-script


rest-ld-script: rest-ld-script.in
	$(CC) -x c-header -undef -nostdinc ${INCLUDES} -E $< | sed 's/^#.*//' > $@




# ---- Blob first stage loader ---------------------------------------

# WARNING: start.S *must* be the first file, otherwise the target will
# be linked in the wrong order!
blob_start_elf32_SOURCES	= \
	start.S \
	memsetup.S \
	ledasm.S \
	testmem.S

blob_start_elf32_LDFLAGS	+= \
	-Wl,-T,${srcdir}/start-ld-script

blob_start_elf32_LDADD	+= \
	-lgcc


blob_start_SOURCES		=


blob-start: blob-start-elf32
	$(OBJCOPY) $(OCFLAGS) $< $@




# ---- Blob first stage chain loader --------------------------------

# WARNING: chain.S *must* be the first file, otherwise the target will
# be linked in the wrong order!
blob_start_chain_elf32_SOURCES	= \
	chain.S \
	ledasm.S

blob_start_chain_elf32_DEPENDENCIES =\
	start-ld-script

blob_start_chain_elf32_LDFLAGS	+= \
	-Wl,-T,${srcdir}/start-ld-script

blob_start_chain_elf32_LDADD	+= \
	-lgcc


blob_start_chain_SOURCES		=


blob-start-chain: blob-start-chain-elf32
	$(OBJCOPY) $(OCFLAGS) $< $@




# ---- Blob second stage ---------------------------------------------

# WARNING: trampoline.S *must* be the first file, otherwise the target
# will be linked in the wrong order!
blob_rest_elf32_SOURCES = \
	trampoline.S \
	flashasm.S \
	stack.S \
	testmem2.S \
	bootldrpart.c \
	commands.c \
	flash.c \
	initcalls.c \
	linux.c \
	main.c \
	memory.c \
	param_block.c \
	partition.c \
	reboot.c \
	printlcd_8bit.c \
	uucodec.c \
	xmodem.c


# conditionally compiled sources
EXTRA_blob_rest_elf32_SOURCES = \
	chkmem.c \
	clock.c \
	debug.c \
	amd32.c \
	intel16.c \
	intel32.c \
	nullflash.c \
	assabet.c \
	brutus.c \
	badge4.c \
	clart.c \
	h3600.c \
	idr.c \
	jornada720.c \
	lart.c \
	nesa.c \
	pleb.c \
	shannon.c \
	system3.c


blob_rest_elf32_DEPENDENCIES = \
	@BLOB_PLATFORM_OBJ@ \
	@BLOB_FLASH_OBJS@ \
	@BLOB_USB_OBJS@ \
	@BLOB_ETH_OBJS@ \
	@CHKMEM@ \
	@DEBUG@ \
	@CLOCK@ \
	${top_builddir}/src/lib/libblob.a \
	rest-ld-script


blob_rest_elf32_LDFLAGS	+= \
	-Wl,-T,rest-ld-script


blob_rest_elf32_LDADD	+= \
	@BLOB_PLATFORM_OBJ@ \
	@BLOB_FLASH_OBJS@ \
	@BLOB_USB_OBJS@ \
	@BLOB_ETH_OBJS@ \
	@CHKMEM@ \
	@DEBUG@ \
	@CLOCK@ \
	-L${top_builddir}/src/lib -lblob\
	-lgcc


blob_rest_SOURCES	=


blob-rest: blob-rest-elf32
	$(OBJCOPY) $(OCFLAGS) $< $@




# ---- Final blob binary ---------------------------------------------

blob_SOURCES	=


blob: blob-start blob-rest
	rm -f $@
	dd if=blob-start of=$@ bs=1k conv=sync
	dd if=blob-rest of=$@ bs=1k seek=4 conv=sync
	chmod +x $@




# ---- Final blob chain loader binary --------------------------------

blob_chain_SOURCES	=

blob-chain: blob-start-chain blob-rest
	rm -f $@
	dd if=blob-start-chain of=$@ bs=1k conv=sync
	dd if=blob-rest of=$@ bs=1k seek=1
	chmod +x $@




# ---- Automake administrativia --------------------------------------

EXTRA_DIST	= \
	start-ld-script \
	rest-ld-script.in


CLEANFILES	= ${srcdir}/*~ rest-ld-script


DISTCLEANFILES	= ${builddir}/.deps/*.P
