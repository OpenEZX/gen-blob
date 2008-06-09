/*
 * linux.h: header file for linux.c
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ident "$Id: linux.h,v 1.4 2001/12/27 18:27:37 erikm Exp $"

#ifndef BLOB_LINUX_H
#define BLOB_LINUX_H


/* FIXME:
 * these numbers should be generated from arch/arm/tools/mach-types
 * with a hacked up version of arch/arm/tools/gen-mach-types 
 *
 * -- Erik
 */

#if defined ASSABET
# define ARCH_NUMBER (25)
#elif defined LUBBOCK
# define ARCH_NUMBER (89)
#elif defined BADGE4
# define ARCH_NUMBER (138)
#elif defined BRUTUS
# define ARCH_NUMBER (16)
#elif defined CLART
# define ARCH_NUMBER (68)
#elif defined H3600
# define ARCH_NUMBER (22)
#elif defined IDR
# define ARCH_NUMBER (147)
#elif defined JORNADA720
# define ARCH_NUMBER (48)
#elif defined LART
# define ARCH_NUMBER (27)
#elif defined NESA
# define ARCH_NUMBER (75)
#elif defined PLEB
# define ARCH_NUMBER (20)
#elif defined SHANNON
# define ARCH_NUMBER (97)
#elif defined PT_SYSTEM3
# define ARCH_NUMBER (112)
#else
#warning "FIXME: Calling the kernel with a generic SA1100 architecture code. YMMV!"
#define ARCH_NUMBER (18)
#endif




#endif
