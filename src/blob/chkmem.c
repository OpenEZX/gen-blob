/*-------------------------------------------------------------------------
 * Filename:      chkmem.c
 * Version:       $Id: chkmem.c,v 1.6 2001/10/29 11:49:48 seletz Exp $
 * Copyright:     Copyright (C) 2001, Stefan Eletzhofer
 * Author:        Stefan Eletzhofer <stefan.eletzhofer@www.eletztrick.net>
 * Description:   memory test functions
 * Created at:    Mit Sep 26 19:20:24 CEST 2001
 * Modified by:   
 * Modified at:  
 *-----------------------------------------------------------------------*/
/*
 * chkmem.c: Utility to test memory integrity
 *
 * Copyright (C) 2001 Stefan Eletzhofer <stefan.eletzhofer@www.eletztrick.net>
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

/* $Id: chkmem.c,v 1.6 2001/10/29 11:49:48 seletz Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/command.h>
#include <blob/types.h>
#include <blob/serial.h>
#include <blob/time.h>
#include <blob/util.h>

#include <blob/memory.h>

/*********************************************************************/
/**  DEFINES *********************************************************/
/*********************************************************************/

/* define this to 1 for debug */
#define CHKMEM_DEBUG		1

/* show every X bytes of memory during test */
#define CHKMEM_SHOWEVERY	(1<<14)

/* more readable IMHO */
#define MEM( x )        (*((u32 *)x))

#define MAKE32FROM8(X)	(u32) (X | X << 8 | X << 16 | X << 24)

#define	CHKMEM_ERR		(-1)
#define CHKMEM_OK		(0)

#if CHKMEM_DEBUG
#	define SHOWFUNC()		SerialOutputString("chkmem: method: "__FUNCTION__ "\n" ); \
							SerialOutputString("	p1=0x"); \
							SerialOutputHex((u32)bp1); \
							SerialOutputString(" p2=0x"); \
							SerialOutputHex((u32)bp2); \
							SerialOutputString(" count=0x"); \
							SerialOutputHex((u32)count); \
							SerialOutputString("\n");
#else
#	define SHOWFUNC()		SerialOutputString("chkmem: method: "__FUNCTION__ "\n" );
#endif

#define SKIPBLOBMEM( STARTADR ) while ( STARTADR < (BLOB_RAM_BASE + 0x00100000) ) STARTADR++;

#define CHKMEM_MAXERR	64
#define CHKMEM_PUSHERR( ADR ) { chkmem_errlist[ chkmem_errs % CHKMEM_MAXERR ] = ADR; \
							chkmem_errs++; }

/*********************************************************************/
/**  MODULE GLOBALS  *************************************************/
/*********************************************************************/

static u32 showevery;

/* list of failed adresses */
static u32 chkmem_errlist[CHKMEM_MAXERR];
static int chkmem_errs;

/*********************************************************************/
/**  TYPES   *********************************************************/
/*********************************************************************/

typedef int (*memtestfunc_t) (u32, u32, u32, u32 *);

/*********************************************************************/
/**  FORWARDS  *******************************************************/
/*********************************************************************/

static int ChkMemErr(void);
static int ChkMemMovInv(u32 start, u32 end, u32 pattern, u32 * badadr);
static int ChkMemAdrTst(u32 start, u32 end, u32 pattern, u32 * badadr);
static int ChkMemHardcore(u32 start, u32 end, u32 pattern, u32 * badadr);

static void showrun(u32 run);
static void showadr(volatile u32 * adr);

/* Charles Cazabon test methods */
static int test_or_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_and_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_seqinc_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_checkerboard_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_solidbits_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_blockseq_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_walkbits_comparison(u32 * bp1, u32 * bp2, u32 count, int mode);
static int test_bitspread_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_bitflip_comparison(u32 * bp1, u32 * bp2, u32 count);
static int test_stuck_address(u32 * bp1, u32 * bp2, u32 count);

/*********************************************************************/
/** EXPORTED FUNCTIONS ***********************************************/
/*********************************************************************/

/*********************************************************************
 * ChkMem
 *
 * AUTOR:	Stefan Eletzhofer
 * REVISED:
 *
 * Command entry, memory test method dispatcher
 *
 */
int ChkMem(int argc, char *argv[])
{
	memtestfunc_t method;
	int area;
	u32 start = 0L;
	u32 end = 0L;
	u32 badaddr = 0L;
	u32 repcount = 0L;

	/* check args */
	if (argc < 2) {
		SerialOutputString("*** not enough arguments\n");
		return CHKMEM_ERR;
	}

	/* reset error counter */
	chkmem_errs = 0;

	/* get verbosity level */
	showevery = CHKMEM_SHOWEVERY;
	if (argc > 2) {
		if (strtou32(argv[2], &showevery) < 0) {
			SerialOutputString("*** not a value\n");
			return CHKMEM_ERR;
		}

		if (showevery > 0) {
			showevery = 1 << showevery;
		} else {
			/* never show address */
			showevery = 0xffffffff;
		}
	}

	/* get repeat count */
	repcount = 1;
	if (argc > 3) {
		if (strtou32(argv[3], &repcount) < 0) {
			SerialOutputString("*** not a value\n");
			return CHKMEM_ERR;
		}
	}

	SerialOutputString(argv[0]);
	SerialOutputString(": display every 0x");
	SerialOutputHex(showevery);
	SerialOutputString(" bytes\n");

	/* set memory test method */
	switch (*argv[1]) {
	case '0':
		method = ChkMemMovInv;
		break;
	case '1':
		method = ChkMemAdrTst;
		break;
	case '2':
		method = ChkMemHardcore;
		break;
	default:
		SerialOutputString("*** unknown method\n");
		return CHKMEM_ERR;
		break;
	}

	while (repcount--) {
		/* test all known memory areas */
		for (area = 0; area < NUM_MEM_AREAS; area++) {
			if (memory_map[area].used) {
				start = memory_map[area].start;
				end = start + memory_map[area].len;

				if (method(start, end, 0x5555aaaa, &badaddr) !=
				    CHKMEM_OK) {
					CHKMEM_PUSHERR(badaddr);
				}
			}
		}
	}

	if (chkmem_errs == 0) {
		SerialOutputString("\n*** no error found\n");
	} else {
		ChkMemErr();
	}

	return CHKMEM_OK;
}
static char chkmemhelp[] =
    "chkmem [method] {verbosity:1..F} {repeat-count}\nmethod=0: move-inverse test\n"
    "method=1: address test\n" "method=2: hardcore test\n"
    "verbosity: display every 2^n address during test\n";
__commandlist(ChkMem, "chkmem", chkmemhelp);

/*********************************************************************/
/** STATIC FUNCTIONS  ************************************************/
/*********************************************************************/

/*********************************************************************
 * showrun
 *
 * AUTOR:	SELETZ
 * REVISED:
 *
 * Shows current memory test run
 *
 */
static void showrun(u32 run)
{
	SerialOutputString("\r\nrun ");
	SerialOutputHex(run);
	SerialOutputString("\n");
}

/*********************************************************************
 * showadr
 *
 * AUTOR:	SELETZ
 * REVISED:
 *
 * display <adr> every <showevery> bytes.
 *
 */
static void showadr(volatile u32 * adr)
{
	if (((u32) adr) % showevery == 0) {
		SerialOutputString("\r");
		SerialOutputHex((u32) adr);
	}

}

/*********************************************************************
 * ChkMemErr
 *
 * AUTOR:		Stefan Eletzhofer
 * REVISED:
 *
 * Reports memory check errors
 *
 */
static int ChkMemErr(void)
{
	int i;

	SerialOutputString("\n*** memory errors:\n");

	for (i = 0; i < chkmem_errs % CHKMEM_MAXERR; i++) {
		SerialOutputHex(i);
		SerialOutputString(": 0x");
		SerialOutputHex(chkmem_errlist[i]);
		SerialOutputString("\n");
	}

	return CHKMEM_OK;
}

/*********************************************************************
 * ChkMemMovInv
 *
 * AUTOR:	Stefan Eletzhofer
 * REVISED:
 *
 * Moving-Inverse Memory Test
 *
 * Test method (from GNU/memtest86 utility):
 * 1) Fill memory with a pattern
 * 2) Starting at the lowest address
 * 2a check that the pattern has not changed
 * 2b write the patterns complement
 * 2c increment the address
 * repeat 2a - 2c
 * 3) Starting at the highest address
 * 3a check that the pattern has not changed
 * 3b write the patterns complement
 * 3c decrement the address
 * repeat 3a - 3c
 *
 * returns 1 if memory failure, returns failed
 * address in badadr
 *
 */
static int ChkMemMovInv(u32 start, u32 end, u32 pattern, u32 * badadr)
{
	int ret = 1;
	register u32 p;
	register u32 tst;

	SerialOutputString("\nchkmem: move-inverse method\n");

	SKIPBLOBMEM(start);

#if CHKMEM_DEBUG
	SerialOutputString("ChkMem: start(0x");
	SerialOutputHex(start);
	SerialOutputString(") - end(0x");
	SerialOutputHex(end);
	SerialOutputString(")\n");
#endif

#if CHKMEM_DEBUG
	SerialOutputString("ChkMem: fillup\n");
#endif

	/* fill mem with pattern */
	p = start;
	while (p < end) {
		MEM(p) = pattern;
		barrier();
		p += 4;
	}

#if CHKMEM_DEBUG
	SerialOutputString("\rChkMem: bottom-up\n");
#endif

	/* bottom-up test */
	p = start;
	while (p < end) {
		showadr((u32 *) p);
		tst = MEM(p);
		if (tst != pattern) {
			goto DONE;
		}
		MEM(p) = ~pattern;
		barrier();

		p += 4;
	}

	pattern = ~pattern;

#if CHKMEM_DEBUG
	SerialOutputString("\rChkMem: top-down\n");
#endif

	/* top-down test */
	p = end - 4;
	while (p >= start) {
		showadr((u32 *) p);
		tst = MEM(p);
		if (tst != pattern) {
			goto DONE;
		}
		MEM(p) = ~pattern;
		barrier();

		p -= 4;
	}

	/* no error if we reach this point */
	ret = 0;
      DONE:
	if (ret != 0 && badadr) {
		*badadr = p;
	}
	return ret;
}

/*********************************************************************
 * ChkMemAdrTst
 *
 * AUTOR:	Stefan Eletzhofer
 * REVISED:
 *
 * Writes every memory location with its adress, then checks address
 *
 * returns 1 if memory failure, returns failed
 * address in badadr
 *
 */
static int ChkMemAdrTst(u32 start, u32 end, u32 pattern, u32 * badadr)
{
	int ret = 1;
	register u32 p;
	register u32 tst;

	SerialOutputString("\nchkmem: address test method\n");

	SKIPBLOBMEM(start);

#if CHKMEM_DEBUG
	SerialOutputString("ChkMem: start(0x");
	SerialOutputHex(start);
	SerialOutputString(") - end(0x");
	SerialOutputHex(end);
	SerialOutputString(")\n");
#endif

#if CHKMEM_DEBUG
	SerialOutputString("ChkMem: fillup\n");
#endif

	/* fill mem with pattern */
	p = start;
	while (p < end) {
		MEM(p) = p;
		barrier();

		p += 4;
	}

#if CHKMEM_DEBUG
	SerialOutputString("\rChkMem: bottom-up\n");
#endif

	/* bottom-up test */
	p = start;
	while (p < end) {
		showadr((u32 *) p);
		tst = MEM(p);
		if (tst != p) {
			goto DONE;
		}

		p += 4;
	}

	/* no error if we reach this point */
	ret = 0;
      DONE:
	if (ret != 0 && badadr) {
		*badadr = p;
	}
	return ret;
}

/*********************************************************************
 * ChkMemHardcore
 *
 * AUTOR:	Stefan Eletzhofer
 * REVISED:
 *
 * Hardcore memory test. Test methods based on memtest
 * by Charles Cazabon <memtest@discworld.dyndns.org>
 *
 * returns 1 if memory failure, returns failed
 * address in badadr
 *
 */
static int ChkMemHardcore(u32 start, u32 end, u32 pattern, u32 * badadr)
{
	register u32 count;

	SerialOutputString("\nchkmem: hardcore test method\n");

	SKIPBLOBMEM(start);

#if CHKMEM_DEBUG
	SerialOutputString("ChkMem: start(0x");
	SerialOutputHex(start);
	SerialOutputString(") - end(0x");
	SerialOutputHex(end);
	SerialOutputString(")\n");
#endif

	count = end - start;
	SerialOutputHex(count);
	SerialOutputString("\n");
	count = (count >> 1);
	SerialOutputHex(count);
	SerialOutputString("\n");

	test_or_comparison((u32 *) start, (u32 *) (start + count), count >> 2);

	test_and_comparison((u32 *) start, (u32 *) (start + count), count >> 2);

	test_seqinc_comparison((u32 *) start, (u32 *) (start + count),
			       count >> 2);

	test_checkerboard_comparison((u32 *) start, (u32 *) (start + count),
				     count >> 2);

	test_solidbits_comparison((u32 *) start, (u32 *) (start + count),
				  count >> 2);

	test_blockseq_comparison((u32 *) start, (u32 *) (start + count),
				 count >> 2);

	test_walkbits_comparison((u32 *) start, (u32 *) (start + count),
				 count >> 2, 0);

	test_walkbits_comparison((u32 *) start, (u32 *) (start + count),
				 count >> 2, 1);

	test_bitspread_comparison((u32 *) start, (u32 *) (start + count),
				  count >> 2);

	test_bitflip_comparison((u32 *) start, (u32 *) (start + count),
				count >> 2);

	test_stuck_address((u32 *) start, (u32 *) (start + count), count >> 2);

	/* no error if we reach this point */
	if (badadr) {
		*badadr = 0L;
	}
	return 0;
}

/*********************************************************************/
/** MEMTESTER FUNCTIONS  *********************************************/
/*********************************************************************/

/**********************************************************************
 * Original Authors of following memory test functions:
 *
 * Charles Cazabon <memtest@discworld.dyndns.org>
 *
 * Copyright © 1999 Simon Kirby.
 * Version 2 Copyright © 1999 Charles Cazabon.
 *
 */

int test_verify_success(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i;

	for (i = 0; i < count; i++, p1++, p2++) {

		if (MEM(p1) != MEM(p2)) {

			SerialOutputString("\nchkmem: contents differ:\n");
			SerialOutputHex((u32) p1);
			SerialOutputString("\n");
			SerialOutputHex((u32) p2);
			SerialOutputString("\n");

			CHKMEM_PUSHERR((u32) p1);
			CHKMEM_PUSHERR((u32) p2);
		}
	}
	return (CHKMEM_OK);
}

int
 test_or_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i;
	u32 q = 0xdeadbeef;

	SHOWFUNC();

	for (i = 0; i < count; i++) {
		showadr(p2);
		MEM(p1++) |= q;
		barrier();
		MEM(p2++) |= q;
		barrier();
	}
	return (test_verify_success(bp1, bp2, count));
}

int test_and_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i;
	u32 q = 0xdeadbeef;

	SHOWFUNC();

	for (i = 0; i < count; i++) {
		showadr(p1);
		MEM(p1++) &= q;
		barrier();
		MEM(p2++) &= q;
		barrier();
	}
	return (test_verify_success(bp1, bp2, count));
}

int test_seqinc_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i;
	u32 q = 0xdeadbeef;

	SHOWFUNC();

	for (i = 0; i < count; i++) {
		showadr(p1);
		MEM(p1++) = MEM(p2++) = (i + q);
		barrier();
	}
	return (test_verify_success(bp1, bp2, count));
}

int test_solidbits_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 q, i, j;

	SHOWFUNC();

	for (j = 0; j < 64; j++) {

		showrun(j);

		q = (j % 2) == 0 ? 0xFFFFFFFF : 0x00000000;
		p1 = (volatile u32 *)bp1;
		p2 = (volatile u32 *)bp2;
		for (i = 0; i < count; i++) {
			showadr(p1);
			MEM(p1++) = MEM(p2++) = (i % 2) == 0 ? q : ~q;
			barrier();
		}

		if (test_verify_success(bp1, bp2, count) == CHKMEM_ERR) {
			return (CHKMEM_ERR);
		}
	}
	return (CHKMEM_OK);
}

int test_checkerboard_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 q, i, j;

	SHOWFUNC();

	for (j = 0; j < 64; j++) {
		showrun(j);
		q = (j % 2) == 0 ? 0x55555555 : 0xAAAAAAAA;
		p1 = (volatile u32 *)bp1;
		p2 = (volatile u32 *)bp2;
		for (i = 0; i < count; i++) {
			showadr(p1);
			MEM(p1++) = MEM(p2++) = (i % 2) == 0 ? q : ~q;
			barrier();
		}

		if (test_verify_success(bp1, bp2, count) == CHKMEM_ERR) {
			return (CHKMEM_ERR);
		}
	}
	return (CHKMEM_OK);
}

int test_blockseq_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i, j;

	SHOWFUNC();

	for (j = 0; j < 256; j++) {
		showrun(j);
		p1 = (volatile u32 *)bp1;
		p2 = (volatile u32 *)bp2;
		for (i = 0; i < count; i++) {
			showadr(p1);
			MEM(p1++) = MEM(p2++) = MAKE32FROM8(j);
			barrier();
		}

		if (test_verify_success(bp1, bp2, count) == CHKMEM_ERR) {
			return (CHKMEM_ERR);
		}
	}
	return (CHKMEM_OK);
}

int test_walkbits_comparison(u32 * bp1, u32 * bp2, u32 count, int m)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i, j;

	SHOWFUNC();

	for (j = 0; j < 64; j++) {
		showrun(j);
		p1 = (volatile u32 *)bp1;
		p2 = (volatile u32 *)bp2;
		for (i = 0; i < count; i++) {
			if (j < 32) {	/* Walk it up. */
				showadr(p1);
				MEM(p1++) = MEM(p2++) =
				    (m ==
				     0) ? 0x00000001 << j : 0xFFFFFFFF ^
				    (0x00000001 << j);
				barrier();
			} else {	/* Walk it back down. */

				MEM(p1++) = MEM(p2++) = (m == 0)
				    ? 0x00000001 << (64 - j - 1)
				    : 0xFFFFFFFF ^ (0x00000001 << (64 - j - 1));
				barrier();
			}
		}

		if (test_verify_success(bp1, bp2, count) == CHKMEM_ERR) {
			return (CHKMEM_ERR);
		}
	}
	return (CHKMEM_OK);
}

int test_bitspread_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i, j;

	SHOWFUNC();

	for (j = 0; j < 64; j++) {
		showrun(j);
		p1 = (volatile u32 *)bp1;
		p2 = (volatile u32 *)bp2;
		for (i = 0; i < count; i++) {
			showadr(p1);
			if (j < 32) {	/* Walk it up. */
				MEM(p1++) = MEM(p2++) = (i % 2 == 0)
				    ? (0x00000001 << j) | (0x00000001 <<
							   (j + 2))
				    : 0xFFFFFFFF ^ ((0x00000001 << j)
						    | (0x00000001 << (j + 2)));
				barrier();
			} else {	/* Walk it back down. */

				MEM(p1++) = MEM(p2++) = (i % 2 == 0)
				    ? (0x00000001 << (63 - j)) | (0x00000001 <<
								  (65 - j))
				    : 0xFFFFFFFF ^ (0x00000001 << (63 - j)
						    | (0x00000001 << (65 - j)));
				barrier();
			}
		}

		if (test_verify_success(bp1, bp2, count) == CHKMEM_ERR) {
			return (CHKMEM_ERR);
		}
	}
	return (CHKMEM_OK);
}

int test_bitflip_comparison(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1 = (volatile u32 *)bp1;
	volatile u32 *p2 = (volatile u32 *)bp2;
	u32 i, j, k;
	u32 q;

	SHOWFUNC();

	for (k = 0; k < 32; k++) {
		showrun(k * 8);
		q = 0x00000001 << k;

		for (j = 0; j < 8; j++) {
			q = ~q;
			p1 = (volatile u32 *)bp1;
			p2 = (volatile u32 *)bp2;
			for (i = 0; i < count; i++) {
				showadr(p1);
				MEM(p1++) = MEM(p2++) = (i % 2) == 0 ? q : ~q;
				barrier();
			}

			if (test_verify_success(bp1, bp2, count) == CHKMEM_ERR) {
				return (CHKMEM_ERR);
			}
		}

	}
	return (CHKMEM_OK);
}

int test_stuck_address(u32 * bp1, u32 * bp2, u32 count)
{
	volatile u32 *p1;
	/* second argument is not used; just gives it a compatible signature. */
	u32 i, j;

	SHOWFUNC();

	count <<= 1;
	for (j = 0; j < 16; j++) {
		showrun(j);
		p1 = (volatile u32 *)bp1;
		for (i = 0; i < count; i++) {
			showadr(p1);
			MEM(p1++) = ((j + i) % 2) == 0 ? (u32) p1 : ~((u32) p1);
			barrier();
		}
		p1 = (volatile u32 *)bp1;
		for (i = 0; i < count; i++, p1++) {
			showadr(p1);
			if (*p1 !=
			    (((j + i) % 2) == 0 ? (u32) p1 : ~((u32) p1))) {
				return (CHKMEM_ERR);
			}
		}
	}
	return (CHKMEM_OK);
}
