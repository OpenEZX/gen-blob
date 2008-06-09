/*-------------------------------------------------------------------------
 * Filename:      param_block.c
 * Version:       $Id: param_block.c,v 1.5 2002/01/03 16:07:18 erikm Exp $
 * Copyright:     Copyright (C) 2001, Russ Dill
 * Author:        Russ Dill <Russ.Dill@asu.edu>
 * Description:   Paramater block tags for blob
 * Created at:    Thu Aug 30 15:20:26 MST 2001
 *-----------------------------------------------------------------------*/
/*
 * param_block.c: Paramater block tags for blob
 * Adapted from linux/arch/arm/kernel/setup.c
 *
 * Copyright (C) 2001 Russ Dill <Russ.Dill@asu.edu>
 * Copyright (C) 1995-2000 Russell King
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

#ident "$Id: param_block.c,v 1.5 2002/01/03 16:07:18 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/types.h>
#include <blob/param_block.h>
#include <blob/main.h>
#include <blob/util.h>
#include <blob/sa1100.h>



/*
 *  Paramater Tag parsing.
 *
 * The first tag must be a PTAG_CORE tag for the list to be recognised 
 * The list is terminated with a zero-length tag (this tag is not parsed 
 * in any way).
 */
static int parse_ptag_core(const struct ptag *ptag)
{
	blob_status.terminalSpeed = ptag->u.core.terminal;
	serial_init(blob_status.terminalSpeed);
	return 0;
}

__ptagtable(PTAG_CORE, parse_ptag_core);

static int parse_ptag_ramdisk(const struct ptag *ptag)
{
	blob_status.load_ramdisk = ptag->u.ramdisk.flags & 1;
	return 0;
}

__ptagtable(PTAG_RAMDISK, parse_ptag_ramdisk);

static int parse_ptag_bootdelay(const struct ptag *ptag)
{
	blob_status.boot_delay = ptag->u.bootdelay.delay;
	return 0;
}

__ptagtable(PTAG_BOOTDELAY, parse_ptag_bootdelay);

static int parse_ptag_cmdline(const struct ptag *ptag)
{
	strlcpy(blob_status.cmdline, ptag->u.cmdline.cmdline, 
		COMMAND_LINE_SIZE);

	return 0;
}

__ptagtable(PTAG_CMDLINE, parse_ptag_cmdline);

static int parse_ptag_baud(const struct ptag *ptag)
{
	blob_status.downloadSpeed = ptag->u.baud.download;
	return 0;
}

__ptagtable(PTAG_BAUD, parse_ptag_baud);

static int parse_ptag_gpio(const struct ptag *ptag)
{
	GPDR &= ~ptag->u.gpio.mask;
	if ((GPSR & ptag->u.gpio.mask) == ptag->u.gpio.level) 
		return -1;
	else return 0;
}

__ptagtable(PTAG_GPIO, parse_ptag_gpio);

/*
 * Scan the tag table for this tag, and call its parse function.
 * The tag table is built by the linker from all the __ptagtable
 * declarations.
 */
int parse_ptag(const struct ptag *ptag, u32 *conf)
{
	extern struct ptagtable __ptagtable_begin, __ptagtable_end;
	struct ptagtable *t;

	for (t = &__ptagtable_begin; t < &__ptagtable_end; t++)
		if (ptag->hdr.ptag == t->ptag &&
		   ((*conf & ptag->hdr.conf_mask) == ptag->hdr.conf)) {
			if (t->parse(ptag) == -1) {
				*conf |= ptag->hdr.fail_set_mask;
				*conf &= ~ptag->hdr.fail_clear_mask;
			}
			break;
		}

	return t < &__ptagtable_end;
}

/*
 * Parse all tags in the list
 */
void parse_ptags(void *arg, u32 *conf)
{
#ifdef PARAM_START
	struct ptag *t	= (struct ptag *) arg;
	if (t->hdr.ptag == PTAG_CORE) {
		for (; t->hdr.size; t = ptag_next(t)) {
			if (t < (struct ptag *) PARAM_START || 
			    t > (struct ptag *) (PARAM_START + PARAM_LEN) ||
			    t->hdr.size > 0x7FFFFFFF) {
				return; /* corrupt tags */
			}
			if (!parse_ptag(t, conf)) { /* Ignoring unrecognised tag */ }
		}
	}
#endif
}




