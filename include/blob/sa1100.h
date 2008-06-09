/*
 * sa1100.h: wrapper file to include asm/arch-sa1100/SA-1100.h
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

#ident "$Id: sa1100.h,v 1.2 2001/11/07 12:25:03 seletz Exp $"

#ifndef BLOB_SA1100_H
#define BLOB_SA1100_H

#define io_p2v(x) (x)
#define __REG(x)      (*((volatile u32 *)io_p2v(x)))

/* Tell SA-1100.h to shut up; we're including it anyway. Nyah nyah ;-) */
#define __ASM_ARCH_HARDWARE_H
#include <asm-arm/arch-sa1100/SA-1100.h>

#endif
