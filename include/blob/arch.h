/*
 * arch.h: Architecture specific defines
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: arch.h,v 1.4 2001/12/27 18:27:37 erikm Exp $
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

/* $Id: arch.h,v 1.4 2001/12/27 18:27:37 erikm Exp $ */

#ifndef BLOB_ARCH_H
#define BLOB_ARCH_H




/* memory start and end */
#define MEMORY_START		(0xA0000000)
#define MEMORY_END		(0xA4000000)




/* architecture specific include files */
#if defined ASSABET
# include <blob/arch/assabet.h>
#elif defined LUBBOCK
# include <blob/arch/lubbock.h>
#elif defined BADGE4
# include <blob/arch/badge4.h>
#elif defined BRUTUS
# include <blob/arch/brutus.h>
#elif defined CLART
# include <blob/arch/clart.h>
#elif defined H3600
# include <blob/arch/h3600.h>
#elif defined IDR
# include <blob/arch/idr.h>
#elif defined JORNADA720
# include <blob/arch/jornada720.h>
#elif defined LART
# include <blob/arch/lart.h>
#elif defined NESA
# include <blob/arch/nesa.h>
#elif defined PLEB
# include <blob/arch/pleb.h>
#elif defined SHANNON
# include <blob/arch/shannon.h>
#elif defined PT_SYSTEM3
# include <blob/arch/system3.h>
#else
# error "Please add an architecture specific include file"
#endif




#endif
