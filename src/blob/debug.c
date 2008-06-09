/*
 * debug.c: Debugging command functions
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

#ident "$Id: debug.c,v 1.9 2002/01/06 15:44:23 erikm Exp $"

/**********************************************************************
 * Includes
 */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/errno.h>
#include <blob/error.h>
#include <blob/types.h>
#include <blob/util.h>
#include <blob/serial.h>
#include <blob/command.h>


/**********************************************************************
 * defines
 */

#define CHKMEM_DEBUG BLOB_DEBUG

/* this will send a cold shiver through erik's spine ... */
#define	ERR( x )		{ ret = x; goto DONE; }

/* more readable IMHO */
#define MEM( x )        (*((u32 *)x))

/**********************************************************************
 * prototypes
 */

static void perror( int errno, char *func );

/*********************************************************************
 * CmdMemcpy
 *
 * AUTOR:		SELETZ
 * REVISED:
 *
 * Command wrapper for memcpy utility function.
 *
 */
static int CmdMemcpy( int argc, char *argv[] )
{
	int	ret = 0;
	u32	src	= 0L;
	u32	dest	= 0L;
	u32	len	= 0L;

	if ( argc < 4 )				ERR( -EINVAL );

	ret = strtou32( argv[1], &src );
	if ( ret < 0 )				ERR( -EINVAL );

	ret = strtou32( argv[2], &dest );
	if ( ret < 0 )				ERR( -EINVAL );

	ret = strtou32( argv[3], &len );
	if ( ret < 0 )				ERR( -EINVAL );

	/* check alignment of src and dest */
	if((src & 0x03) || (dest & 0x03))
		ERR(-EALIGN);

	/* counted in words */
	if ( len & 0x00000003 ) {
		len = ( len >> 2 ) + 1;
	} else {
		len = len >> 2;
	}

	SerialOutputString("\n### Now copying 0x");
	SerialOutputHex(len);
	SerialOutputString(" words from 0x");
	SerialOutputHex((int)src);
	SerialOutputString(" to 0x");
	SerialOutputHex((int)dest);
	serial_write('\n');

	MyMemCpy( (u32 *)dest, (const u32 *)src, len);

	SerialOutputString(" done\n");

	ret = 0;
DONE:
	if ( ret != 0 ) {
		perror( ret, __FUNCTION__ );
	}
	return ret;
}

static char memcpyhelp[] = "memcpy src dst len\n"
"copy len bytes from src to dst\n";
__commandlist(CmdMemcpy, "memcpy", memcpyhelp);


/*********************************************************************
 * Poke
 *
 * AUTOR:	Stefan Eletzhofer
 * REVISED:
 *
 * Poke values to memory
 *
 */
static int Poke( int argc, char *argv[] )
{
	int		ret				= 0;
	u32		address;
	u32		value;
	char		type = 'w';

	if ( argc < 3 ) 			ERR( -EINVAL );

	ret = strtou32(argv[1], &address);
	if ( ret < 0 )				ERR( -EINVAL );

	ret = strtou32(argv[2], &value);
	if ( ret < 0 )				ERR( -EINVAL );

	if ( argc >= 3 ) {
		type = argv[3][0];
	}

#if CHKMEM_DEBUG
	SerialOutputString("adr=0x");
	SerialOutputHex(address);
	SerialOutputString(" val=0x");
	SerialOutputHex(value);
	SerialOutputString(" type=");
	serial_write(type);
	serial_write('\n');
#endif

	/* check memory alignment */
	switch(type | 0x20) {
	case 'b':
		/* bytes don't have alignment restrictions */
		break;

	case 'h':
		/* half word accesses should be aligned on half words */
		if(address & 0x01)
			ERR(-EALIGN);

		break;

	case 'w':
		/* word accesses should be aligned on word boundaries */
		if(address & 0x03)
			ERR(-EALIGN);
		
		break;

	default:
		/* hmm, invalid type */
		ERR( -EINVAL );
		break;
	}

	/* write the value to memory */
	switch( type | 0x20 ) {
	case 'b':
		*((u8 *)address) = (u8)(value & 0xff);
		break;

	case 'h':
		*((u16 *)address) = (u16)(value & 0xffff);
		break;

	case 'w':
		*((u32 *)address) = value;
		break;

	default:
		/* this should not happen */
		ERR( -EINVAL );
		break;
	}


	ret = 0;
DONE:
	if ( ret != 0 ) {
		perror( ret, __FUNCTION__ );
	}
	return ret;
}

static char pokehelp[] = "poke address value [b|h|w]\n"
"b = byte, h = half word, w = word (default is w)\n";
__commandlist(Poke, "poke", pokehelp );

/*********************************************************************
 * Peek
 *
 * AUTHOR:	Stefan Eletzhofer
 * REVISED:
 *
 * Peeks values from memory
 *
 */
static int Peek( int argc, char *argv[] )
{
	int		ret				= 0;
	u32		address;
	u32		value;
	char		type	= 'w';

	if ( argc < 2 ) 			ERR( -EINVAL );

	ret = strtou32(argv[1], &address);
	if ( ret < 0 )				ERR( -EINVAL );

	if ( argc >= 2 ) {
		type = argv[2][0];
	}

#if CHKMEM_DEBUG
	SerialOutputString("adr=0x");
	SerialOutputHex(address);
	SerialOutputString(" type=");
	serial_write(type);
	serial_write('\n');
#endif

	/* check memory alignment */
	switch(type | 0x20) {
	case 'b':
		/* bytes don't have alignment restrictions */
		break;

	case 'h':
		/* half word accesses should be aligned on half words */
		if(address & 0x01)
			ERR(-EALIGN);

		break;

	case 'w':
		/* word accesses should be aligned on word boundaries */
		if(address & 0x03)
			ERR(-EALIGN);
		
		break;

	default:
		/* hmm, invalid type */
		ERR( -EINVAL );
		break;
	}

	/* read value from memory */
	switch( type | 0x20 ) {
	case 'b':
		value = (*((u8 *)address)) & 0xff;
		break;

	case 'h':
		value = (*((u16 *)address))& 0xffff;
		break;

	case 'w':
		value = (*((u32 *)address));
		break;

	default:
		/* this should not happen */
		ERR(-EINVAL);
		break;
	}

	serial_write(type);
	serial_write(' ');
	SerialOutputHex(value);
	SerialOutputString("\n");

	ret = 0;
DONE:
	if ( ret != 0 ) {
		perror( ret, __FUNCTION__ );
	}
	return ret;
}

static char peekhelp[] = "peek address [b|h|w]\n"
"b = byte, h = half word, w = word (default is w)\n";
__commandlist(Peek, "peek", peekhelp );

/*********************************************************************
 * dump
 *
 * AUTHOR: Tim Riker
 * REVISED:
 *
 * dumps memory
 *
 */
static int dump( int argc, char *argv[] )
{
	int		ret = 0;
	u32		address;
	u32		endaddress;
	u32		tmpaddress;
	u32		value;

	if ( argc < 2 )
		ERR( -EINVAL );

	ret = strtou32(argv[1], &address);
	if ( ret < 0 )
		ERR( -EINVAL );

	if ( argc == 3 ) {
		ret = strtou32(argv[2], &endaddress);
		if ( ret < 0 )
			ERR( -EINVAL );
	} else
		endaddress = address + 0x80;


	/* check alignment of address and endaddress */
	if((address & 0x03) || (endaddress & 0x03))
		ERR(-EALIGN);

	/* print it */
	for ( ; address < endaddress; address += 0x10) {
		SerialOutputHex(address);
		SerialOutputString(": ");
		for (tmpaddress = address; tmpaddress < address + 0x10; tmpaddress += 4) {
			value = (*((u32 *)tmpaddress));
			SerialOutputHex(value);
			serial_write(' ');
		}
		for (tmpaddress = address; tmpaddress < address + 0x10; tmpaddress++) {
			value = (*((u8 *)tmpaddress)) & 0xff;
			if ((value >= ' ') && (value <= '~'))
				serial_write(value);
			else
				serial_write('.');
		}
		serial_write('\n');
	}

	ret = 0;
DONE:
	if ( ret != 0 ) {
		perror( ret, __FUNCTION__ );
	}
	return ret;
}

static char dumphelp[] = "dump address [endAddress]\n";
__commandlist(dump, "dump", dumphelp );

/*********************************************************************
 * BitChange
 *
 * AUTOR:		SELETZ
 * REVISED:
 *
 * Modifies bits of an given memory location
 *
 */
static int BitChange( int argc, char *argv[] )
{
	int		ret		= 0;
	u32		adr		= 0L;
	u32		value	= 0L;

	if ( argc < 4 )				ERR( -EINVAL );

	ret = strtou32( argv[1], &adr );
	if ( ret < 0 )				ERR( -EINVAL );

	ret = strtou32( argv[2], &value );
	if ( ret < 0 )				ERR( -EINVAL );

	/* check memory alignment */
	if(adr &= 0x03)
		ERR(-EALIGN);

	SerialOutputHex( MEM( adr ) );

	switch ( argv[3][0] & (~0x20) ) {
		case 'A':
			MEM( adr ) &= value;
			break;
		case 'S':
		case 'O':
			MEM( adr ) |= value;
			break;
		case 'X':
			MEM( adr ) ^= value;
			break;
		case 'C':
			MEM( adr ) &= ~value;
			break;
		default:
			ERR( -EINVAL );
			break;
	}

	SerialOutputString( "->" );
	SerialOutputHex( MEM( adr ) );
	SerialOutputString( "\n" );

	ret = 0;
DONE:
	if ( ret != 0 ) {
		perror( ret, __FUNCTION__ );
	}
	return ret;
}

static char bitchghelp[] = "bitchg address value {and|or|xor|set|clear}\n";
__commandlist(BitChange, "bitchg", bitchghelp );

/*********************************************************************
 * Call
 *
 * AUTHOR:		Christopher Hoover <ch@hpl.hp.com>
 * REVISED:
 *
 * Jumps to an arbitrary address.
 *
 */
static int Call( int argc, char *argv[] )
{
	int		ret		= 0;
	u32 adr, a = 0, b = 0, c = 0, d = 0;
	int (*called_fn)(u32, u32, u32, u32);

	if ( argc < 2  || argc > 6 )		ERR( -EINVAL );

	ret = strtou32( argv[1], &adr );
	if ( ret < 0 )				ERR( -EINVAL );

	if ( argc >= 3 ) {
		ret = strtou32( argv[2], &a );
		if ( ret < 0 )			ERR( -EINVAL );
	}
	if ( argc >= 4 ) {
		ret = strtou32( argv[2], &b );
		if ( ret < 0 )			ERR( -EINVAL );
	}
	if ( argc >= 5 ) {
		ret = strtou32( argv[2], &c );
		if ( ret < 0 )			ERR( -EINVAL );
	}
	if ( argc >= 6) {
		ret = strtou32( argv[2], &d );
		if ( ret < 0 )			ERR( -EINVAL );
	}

	SerialOutputString("Calling function at 0x");
	SerialOutputHex(adr);
	SerialOutputString(" ...\n");
	serial_flush_output();

	called_fn = (int (*)(u32, u32, u32, u32))adr;

	ret = called_fn(a, b, c, d);
	
	SerialOutputString("\nReturn value: 0x");
	SerialOutputHex((u32)ret);
	serial_write('\n');

	ret = 0;

DONE:
	if ( ret != 0 ) {
		perror( ret, __FUNCTION__ );
	}
	return ret;
}

static char callhelp[] = "call address [arg0 [arg1 [arg2 [arg3]]]]\n"
"Call function at <address> with optional arguments\n";

__commandlist(Call, "call", callhelp);


/**********************************************************************
 * static functions
 */

static void perror( int errno, char *func )
{
	printerrprefix();

	if ( errno < 0 )
		errno = -errno;

	if ( func != NULL) {
		SerialOutputString(func);
		SerialOutputString(": ");
	}
	SerialOutputString(strerror(errno));

	serial_write('\n');
}
