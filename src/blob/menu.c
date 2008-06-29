/*
 *  Boot Menu
 *  
 *  Copyright (C) 2007 Alex Zhang <celeber2@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <common.h>
#include <types.h>
#include "menu.h"

#define MAX_WORD_NR     10
#define MAX_FILE_SIZE   (2 * 1024)
#define MAX_HEAP_SIZE   (10 * 1024)

static char *file_content = NULL;
static int file_size = 0;
static char heap[MAX_HEAP_SIZE];
static char *p_heap = heap;

#if 0
#define debug printf
#else
#define debug(fmt, ...)
#endif

#if 1
char *grub_malloc(grub_size_t n)
{
	char *p = p_heap;
	p_heap += n;
	if (p_heap > (heap + MAX_HEAP_SIZE))
		return NULL;
	return p;
}

char *grub_realloc(char *old, grub_size_t n)
{
	char *p;
	p = grub_malloc(n);
	if (!p)
		return NULL;
	grub_memcpy(p, old, n / 2);	// FIXME
	return p;
}

void grub_free(char *p)
{
}
#else
#define grub_malloc malloc
#define grub_free free
#define grub_realloc realloc
#endif
int atoi(char *string)
{
	int res = 0;
	while (*string >= '0' && *string <= '9') {
		res *= 10;
		res += *string - '0';
		string++;
	}

	return res;
}

grub_file_t grub_fopen(char *name, char *type)
{
	file_content = grub_malloc(MAX_FILE_SIZE);
	if (!file_content)
		return 0;
	file_size = file_fat_read(name, file_content, MAX_FILE_SIZE);
	if (!file_size)
		return 0;
	return file_content;
}

void grub_fclose(grub_file_t file)
{
	grub_free(file);
}

grub_size_t grub_file_read(grub_file_t file, char *buf, grub_size_t len)
{
	grub_size_t n = 0;
	while (1) {
		if (file_size == 0)
			break;
		if (n == len)
			break;
		*buf++ = *file_content++;
		n++;
		file_size--;
	}
	return n;
}

int grub_isspace(int c)
{
	return (c == '\n' || c == '\r' || c == ' ' || c == '\t');
}

static char *get_line(grub_file_t file)
{
	char c;
	int pos = 0;
	int literal = 0;
	int comment = 0;
	char *cmdline;
	int max_len = 64;

	/* Initially locate some space.  */
	cmdline = grub_malloc(max_len);
	if (!cmdline)
		return 0;

	while (1) {
		if (grub_file_read(file, &c, 1) != 1)
			break;

		/* Skip all carriage returns.  */
		if (c == '\r')
			continue;

		/* Replace tabs with spaces.  */
		if (c == '\t')
			c = ' ';

		/* The previous is a backslash, then...  */
		if (literal) {
			/* If it is a newline, replace it with a space and continue.  */
			if (c == '\n') {
				c = ' ';

				/* Go back to overwrite the backslash.  */
				if (pos > 0)
					pos--;
			}

			literal = 0;
		}

		if (c == '\\')
			literal = 1;

		if (comment) {
			if (c == '\n')
				comment = 0;
		} else if (pos == 0) {
			if (c == '#')
				comment = 1;
			else if (!grub_isspace(c))
				cmdline[pos++] = c;
		} else {
			if (c == '\n')
				break;

			if (pos >= max_len) {
				char *old_cmdline = cmdline;
				max_len = max_len * 2;
				cmdline = grub_realloc(cmdline, max_len);
				if (!cmdline) {
					grub_free(old_cmdline);
					return 0;
				}
			}

			cmdline[pos++] = c;
		}
	}

	cmdline[pos] = '\0';

	/* If the buffer is empty, don't return anything at all.  */
	if (pos == 0) {
		grub_free(cmdline);
		cmdline = 0;
	}

	return cmdline;
}

int get_cmd_word(char *cmdline, char *word)
{
	int n = 0;
	char c;

	while (1) {
		c = *cmdline++;
		if (grub_isspace(c))
			break;
		if (c == '\0')
			break;
		*word++ = c;
		n++;
		if (n > MAX_WORD_NR - 1)
			break;
	}
	*word = '\0';
	return 0;
}

cmd_t cmd_set[] = {
	{TYPE_CMD_TITLE, "title"}
	,
	{TYPE_CMD_KERNEL, "kernel"}
	,
	{TYPE_CMD_PARAM, "param"}
	,
	{TYPE_CMD_MACHID, "machid"}
	,
};

#define NR_CMD_SET (sizeof(cmd_set) / sizeof(cmd_t))

int get_cmd_type(char *cmd)
{
	int i;
	int type = TYPE_CMD_UNKNOWN;

	for (i = 0; i < NR_CMD_SET; i++) {
		if (strcmp(cmd, cmd_set[i].cmd) == 0) {
			type = cmd_set[i].type;
			break;
		}
	}
	return type;
}

char *get_menu_entry_param(char *cmdline, char *cmd)
{
	char *ret = NULL;
	int n;
	char c;

	n = grub_strlen(cmd);
	cmdline += n;
	while (1) {
		c = *cmdline;
		if (c == '\0')
			return NULL;
		if (!grub_isspace(c))
			break;
		cmdline++;
	}
#if 0
	n = grub_strlen(cmdline);
	if (n < 0 || n == 0)
		return NULL;
	ret = grub_malloc(n + 1);
	if (!ret)
		return NULL;
	grub_memcpy(ret, cmdline, n);
	ret[n] = '\0';
#else
	ret = cmdline;
#endif
	debug("@: %s\n", ret);
	return ret;
}

int parse_conf_file(char *name, menu_t * menu)
{
	grub_file_t file;
	char *cmdline, *p;
	int type;
	char cmd[MAX_WORD_NR];
	menu_entry_t *new_entry, *curr_entry;
	int i = 0, num = 0;

	file = grub_fopen(name, "r");

	if (file == NULL) {
		printf("Cannot Open configuration file: %s\n", name);
		return 1;
	}

	new_entry = NULL;
	curr_entry = NULL;
	grub_memset(menu, 0, sizeof(menu_t));
	while (1) {
		cmdline = get_line(file);
		if (!cmdline)
			break;
		debug("> %s\n", cmdline);
		get_cmd_word(cmdline, &cmd);
		type = get_cmd_type(&cmd);
		debug(": %s, type = %d\n", &cmd, type);
		switch (type) {
		case TYPE_CMD_TITLE:
			new_entry = grub_malloc(sizeof(menu_entry_t));
			if (!new_entry)
				goto err;
			grub_memset(new_entry, 0, sizeof(menu_entry_t));
			if (curr_entry == NULL) {
				menu->entry = new_entry;
			} else {
				curr_entry->next = new_entry;
			}
			curr_entry = new_entry;
			curr_entry->id = num;
			num++;
			p = get_menu_entry_param(cmdline, &cmd);
			if (!p)
				goto err;
			curr_entry->title = p;
			break;
		case TYPE_CMD_KERNEL:
			if (!curr_entry)
				goto err;
			curr_entry->kernel =
			    get_menu_entry_param(cmdline, &cmd);
			break;
		case TYPE_CMD_PARAM:
			if (!curr_entry)
				goto err;
			curr_entry->param = get_menu_entry_param(cmdline, &cmd);
			break;
		case TYPE_CMD_MACHID:
			p = get_menu_entry_param(cmdline, &cmd);
			i = atoi(p);
			grub_free(p);
			if (curr_entry)
				curr_entry->machid = i;
			else
				menu->default_machid = i;
			break;
		case TYPE_CMD_UNKNOWN:
			break;	// FIXME
		default:
			goto err;
		}
		//      grub_free(cmdline);
	}
	menu->num = num;
	grub_fclose(file);
	return 0;
      err:
	debug("exit\n");
	grub_fclose(file);
	return -1;
}
