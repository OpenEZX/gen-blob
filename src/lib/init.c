/*
 * init.c: Support for init and exit lists
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: init.c,v 1.3 2002/01/03 16:07:18 erikm Exp $
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

/* $Id: init.c,v 1.3 2002/01/03 16:07:18 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/errno.h>
#include <blob/error.h>
#include <blob/init.h>
#include <blob/serial.h>
#include <blob/types.h>


/* int and exit list start and end. filled in by the linker */
extern u32 __initlist_start;
extern u32 __initlist_end;
extern u32 __exitlist_start;
extern u32 __exitlist_end;


static void call_funcs(initlist_t *start, initlist_t *end, 
		       u32 magic, int level)
{
	initlist_t *item;

	for(item = start; item != end; item++) {
		if(item->magic != magic) {
			printerror(EMAGIC, NULL);
#ifdef BLOB_DEBUG
			printerrprefix();
			SerialOutputString("Address = 0x");
			SerialOutputHex((u32)item);
			serial_write('\n');
#endif
			return;
		}
		
		if(item->level == level) {
			/* call function */
			item->callback();
		}
	}
}


void init_subsystems(void)
{
	int i;
	
	/* call all subsystem init functions */
	for(i = INIT_LEVEL_MIN; i <= INIT_LEVEL_MAX; i++)
		call_funcs((initlist_t *)&__initlist_start, 
			   (initlist_t *)&__initlist_end,
			   INIT_MAGIC, i);

}


void exit_subsystems(void)
{
	int i;
	
	/* call all subsystem exit functions */
	for(i = INIT_LEVEL_MAX; i >= INIT_LEVEL_MIN; i--)
		call_funcs((initlist_t *)&__exitlist_start, 
			   (initlist_t *)&__exitlist_end,
			   EXIT_MAGIC, i);
}
