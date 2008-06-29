/*
 *  Boot Menu
 *  
 *  Copyright (C) 2007 Alex Zhang <celeber2@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef __MENU_H__
#define __MENU_H__

#define grub_memcpy memcpy 
#define grub_strlen strlen
#define grub_memset memset

typedef size_t grub_size_t;
typedef char * grub_file_t;

typedef struct __menu_entry {
	int id;
	char *title;
	char *kernel;
	char *param;
	int machid;
	struct __menu_entry *next;
} menu_entry_t;

typedef struct __menu {
	int curr_entry;
	int num;
	int default_machid;
	menu_entry_t *entry;
} menu_t;

typedef struct __cmd_t {
	int type;
	char *cmd;
} cmd_t;

enum {
	TYPE_CMD_UNKNOWN,
	TYPE_CMD_TITLE,
	TYPE_CMD_KERNEL,
	TYPE_CMD_PARAM,
	TYPE_CMD_MACHID,
};


#endif // __MENU_H__
