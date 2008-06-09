/*
 * include/blob/boot_modes.h
 *
 * header file for boot_modes.c
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
 *
 * 2005-JUN-06 - (Motorola)init draft
 *
*/
#ifndef BOOT_MODES_H
#define BOOT_MODES_H
BOOL check_pass_through_flag(void);
void enter_pass_through_mode(void);
void enter_simple_pass_through_mode(void);
BOOL check_flash_flag(void);
BOOL check_valid_code(void);
void usb_gpio_pass_through_init(void);
BOOL check_manual_flash_mode(void);
void enter_manual_flash_mode(u32 data);
void enter_tcmd_flash_mode(u32 data);


#endif /* end of file */
