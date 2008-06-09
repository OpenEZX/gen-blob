/*
 * include/blob/HW_init.h
 *
 * header file for HW_init.c
 *
 * Copyright (C) 2005  Motorola
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
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
 * 2005-Jun-06 - (Motorola) init draft
 *
 */
#ifndef HW_INIT_H
#define HW_INIT_H

/* Structure Note:
Different products have different hw init functions, but main() just
calls hw_init(). You have to assign a products implementation in 
its product macro scopes*/

#ifdef BARBADOS
u32 barbados_hw_init(u32 x);
#define hw_init(x) barbados_hw_init(x)

#endif

#ifdef MARTINIQUE
u32 martinique_hw_init(u32 x);
#define hw_init(x) martinique_hw_init(x)

#endif
#ifdef HAINAN
u32 hainan_hw_init(u32 x);
#define hw_init(x) hainan_hw_init(x)


#endif 

#ifdef SUMATRA
u32 sumatra_hw_init(u32 x);
#define hw_init(x) sumatra_hw_init(x)


#endif 


#endif /* File end */

