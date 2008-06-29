/*
 * linux.c: support functions for booting a kernel
 *
 * Copyright (C) 2003, 2005 Motorola Inc.
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
 * 2003-Jan-16 - (Motorola) Remove setup of initrd and ramdisk tags
 * 2005-Dec-14 - (Motorola) Added EZX parameter
 */

#ident "$Id: linux.c,v 1.8 2002/01/06 15:46:17 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/linux.h>
#include <blob/arch.h>
#include <blob/command.h>
#include <blob/init.h>
#include <blob/main.h>
#include <blob/memory.h>
#include <blob/util.h>
#include <blob/serial.h>
#include <blob/pxa.h>

#include <asm-arm/setup.h>

static void setup_start_tag(void);
static void setup_memory_tags(void);
static void setup_commandline_tag(int argc, char *argv[]);
static void setup_ramdisk_tag(void);
static void setup_initrd_tag(void);
static void setup_end_tag(void);
#ifdef MBM			//mbm module
static void setup_ezx_tag(void);
#endif
static struct tag *params;
extern int cmd_flag;

int boot_linux(int argc, char *argv[])
{
	void (*theKernel) (int zero, int arch) =
	    (void (*)(int, int))KERNEL_RAM_BASE;

	setup_start_tag();
	setup_memory_tags();
	setup_commandline_tag(argc, argv);
#if 0
	setup_initrd_tag();
	setup_ramdisk_tag();
#endif
#ifdef MBM			//mbm module
	setup_ezx_tag();
#endif //need to restore for all products 0311
	setup_end_tag();

	/* we assume that the kernel is in place */
	SerialOutputString("\nStarting kernel ...\n\n");
	serial_flush_output();

	/* disable subsystems that want to be disabled before kernel boot */
	exit_subsystems();

	/* start kernel */
	theKernel(0, ARCH_NUMBER);

	SerialOutputString
	    ("Hey, the kernel returned! This should not happen.\n");
	printlcd("Hey, the kernel returned! This should not happen.\n");
	while (1) ;
	return 0;
}

static char boothelp[] = "boot [kernel options]\n"
    "Boot Linux with optional kernel options\n";

__commandlist(boot_linux, "boot", boothelp);

static void setup_start_tag(void)
{
	params = (struct tag *)BOOT_PARAMS;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size(tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next(params);
}

#ifdef MBM			//mbm module
//???fix me: This address should be same as in start.S, potential 
//confilicts may be happened if someone use this address in other places
//it assert a0000000+12 now, see lubbock.h for other flags in this area

#define POWERUP_REASON_ADDR 0xa000000c
u32 *powup_reason = (u32 *) POWERUP_REASON_ADDR;
void setup_ezx_tag(void)
{
//  params->hdr.tag = ATAG_EZX;
//  params->hdr.size = tag_size(tag_ezx);
//  params->u.ezx.pow_up_reason=*powup_reason;
//  params = tag_next(params);

}
#endif //mbm module
static void setup_memory_tags(void)
{
	int i;

	for (i = 0; i < NUM_MEM_AREAS; i++) {
		if (memory_map[i].used) {
			params->hdr.tag = ATAG_MEM;
			params->hdr.size = tag_size(tag_mem32);

			params->u.mem.start = memory_map[i].start;
			params->u.mem.size = memory_map[i].len;

			params = tag_next(params);
		}
#if defined( NEPONSET )
		// When neponset board is present dont report
		// the 2nd ram area to the kernel. This area
		// is on the neponset board and would require
		// NUMA support in the kernel.
		break;
#endif
	}
}

static void setup_commandline_tag(int argc, char *argv[])
{
	char *p;
	int i;

	/* initialise commandline */
	params->u.cmdline.cmdline[0] = '\0';

	/* copy default commandline from parameter block */
	if (cmd_flag == 1)
		strlcpy(params->u.cmdline.cmdline, blob_status.cmdline,
			COMMAND_LINE_SIZE);

	/* copy commandline */
	if (argc >= 2) {
		p = params->u.cmdline.cmdline;

		for (i = 1; i < argc; i++) {
			strlcpy(p, argv[i], COMMAND_LINE_SIZE);
			p += strlen(p);
			*p++ = ' ';
		}

		p--;
		*p = '\0';

		/* technically spoken we should limit the length of
		 * the kernel command line to COMMAND_LINE_SIZE
		 * characters, but the kernel won't copy longer
		 * strings anyway, so we don't care over here.
		 */
	}

	if (strlen(params->u.cmdline.cmdline) > 0) {
		params->hdr.tag = ATAG_CMDLINE;
		params->hdr.size = (sizeof(struct tag_header) +
				    strlen(params->u.cmdline.cmdline) + 1 +
				    4) >> 2;

		params = tag_next(params);
	}
}

static void setup_initrd_tag(void)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD;
	params->hdr.size = tag_size(tag_initrd);

	params->u.initrd.start = RAMDISK_RAM_BASE;
	params->u.initrd.size = RAMDISK_FLASH_LEN;

	params = tag_next(params);
}

static void setup_ramdisk_tag(void)
{
	/* an ATAG_RAMDISK node tells the kernel how large the
	 * decompressed ramdisk will become.
	 */
	params->hdr.tag = ATAG_RAMDISK;
	params->hdr.size = tag_size(tag_ramdisk);

	params->u.ramdisk.start = 0;
	params->u.ramdisk.size = RAMDISK_SIZE;
	params->u.ramdisk.flags = 1;	/* automatically load ramdisk */

	params = tag_next(params);
}

static void setup_end_tag(void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}
