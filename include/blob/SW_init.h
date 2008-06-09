/*
 * include/blob/SW_init.h
 *
 * header file for SW_init.c
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
 * 2005-Jun-06 - (Motorla) init draft
 *
 */
#ifndef SW_INIT_H
#define SW_INIT_H

/* Structure Note:
Different products have different sw init functions, but main() just
calls sw_init(). You have to assign a products implementation in 
its product macro scopes*/

#ifdef BARBADOS
u32 barbados_sw_init(u32 x);
#define sw_init(x) barbados_sw_init(x)

#endif

#ifdef MARTINIQUE
u32 martinique_sw_init(u32 x);
#define sw_init(x) martinique_sw_init(x)

#endif

#ifdef HAINAN
u32 hainan_sw_init(u32 x);
#define sw_init(x) hainan_sw_init(x)


#endif 

#ifdef SUMATRA
u32 sumatra_sw_init(u32 x);
#define sw_init(x) sumatra_sw_init(x)


#endif 

#endif /* end of file */

