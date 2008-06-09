/*
 * sa1111.h: wrapper file to include asm/arch-sa1100/SA-1111.h
 *
 * Copyright (C) 2001 Stefan Eletzhofer <stefan.eletzhofer@www.eletztrick.net>
 *
 * $Id: sa1111.h,v 1.4 2002/01/07 17:54:14 seletz Exp $
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

#ident "$Id: sa1111.h,v 1.4 2002/01/07 17:54:14 seletz Exp $"

#ifndef BLOB_SA1111_H
#define BLOB_SA1111_H

#include <blob/arch.h>

#ifndef SA1111_BASE
# error "Define SA1111_BASE address in architecture specific include file"
#endif

#define SA1111_p2v(x) (x)
#define SA1101_p2v(PhAdd)  (PhAdd)
#define io_p2v(x) (x)
#define __REG(x)      (*((volatile u32 *)io_p2v(x)))
#define __REGP(x)      (*((volatile u32 *)io_p2v(x)))

/* Tell SA-1111.h to shut up; we're including it anyway. Nyah nyah ;-) */
#define __ASM_ARCH_HARDWARE_H
#include <asm-arm/arch-sa1100/SA-1111.h>

#endif
