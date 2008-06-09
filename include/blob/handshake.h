/*
 * include/blob/handshake.h
 * 
 * header file for handshake.c
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

#ifndef HANDSHAKE_H
#define HANDSHAKE_H

/* shakehand  with BP's PIN  */
#define GPIO_BP_RDY            0       /* BP_RDY     */
#define GPIO_BB_WDI            13      /* BB_WDI     */
#define GPIO_BB_WDI2           3       /* BB_WDI2    */
#define GPIO_BB_RESET          116      /* BB_RESET   */
#define GPIO_MCU_INT_SW        57      /* MCU_INT_SW */
#define BB_RESET_TIMEOUT	0x00080000
void restart_bp(void);
BOOL check_bb_wdi2_is_low(void);
/* Structure Note:
Different products have different hw init functions, but main() just
calls handshake(). You have to assign a products implementation in 
its product macro scopes*/

#ifdef BARBADOS

#define handshake(x) 		/* barbados don't do handshake in blob */

#endif

#ifdef MARTINIQUE

#define handshake(x) 		/* martinique don't do handshake in blob */

#endif

#ifdef HAINAN
u32 hainan_handshake(u32 x);
#define handshake(x)  hainan_handshake(u32 x)

#endif 





#endif /* end of file */
