/*
 * memsetup.h: include file for memory setup
 *
 * $Id: memsetup.h,v 1.4 2001/12/27 18:27:37 erikm Exp $
 *
 * Copyright (C) 2001 Stefan Eletzhofer
 *						<stefan.eletzhofer@www.eletztrick.net>
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
/*
 * Documentation:
 * Intel Corporation, "Intel StrongARM SA-1110 Microprocessor
 *     Developer's Manual", April 1999
 */

/* $Id: memsetup.h,v 1.4 2001/12/27 18:27:37 erikm Exp $ */

#ifndef BLOB_MEMSETUP_H
#define BLOB_MEMSETUP_H

/**********************************************************************
 *  Memory Config Register Indices
 *  based on 0xA0000000
 */
#define	MDCNFG	0x0
#define MDCAS00	0x04
#define MDCAS01	0x08
#define MDCAS02	0x0c
#define MCS0	0x10
#define MCS1	0x14
#define	MECR	0x18
#define MDREFR	0x1C
#define MDCAS20	0x20
#define MDCAS21	0x24
#define MDCAS22	0x28
#define MCS2	0x2C
#define	SMCNFG	0x30

/**********************************************************************
 *  MDCNFG masks
 */

#define MDCNFG_BANK0_ENABLE (1 << 0)
#define MDCNFG_BANK1_ENABLE (1 << 1)
#define MDCNFG_DTIM0_SDRAM  (1 << 2)
#define MDCNFG_DWID0_32B    (0 << 3)
#define MDCNFG_DWID0_16B    (1 << 3)
#define MDCNFG_DRAC0(n_)    (((n_) & 7) << 4)
#define MDCNFG_TRP0(n_)     (((n_) & 0xF) << 8)
#define MDCNFG_TDL0(n_)     (((n_) & 3) << 12)
#define MDCNFG_TWR0(n_)     (((n_) & 3) << 14)

/**********************************************************************
 *  MDREFR masks
 */
#define MDREFR_TRASR(X)		(X & (0x0000000f))
#define MDREFR_DRI(X)		((X & (0x00000fff)) << 4 )
#define MDREFR_E0PIN		(1 << 16)
#define MDREFR_K0RUN		(1 << 17)
#define MDREFR_K0DB2		(1 << 18)
#define MDREFR_E1PIN		(1 << 20)
#define MDREFR_K1RUN		(1 << 21)
#define MDREFR_K1DB2		(1 << 22)
#define MDREFR_K2RUN		(1 << 25)
#define MDREFR_K2DB2		(1 << 26)
#define MDREFR_EAPD		(1 << 28)
#define MDREFR_KAPD		(1 << 29)
#define MDREFR_SLFRSH		(1 << 31)

/**********************************************************************
 *  Static Memory Config
 */
#undef MSC_RDF
#undef MSC_RDN
#undef MSC_RRR

#define MSC_RT_ROMFLASH         0
#define MSC_RT_SRAM_012         1
#define MSC_RT_VARLAT_345       1
#define MSC_RT_BURST4           2
#define MSC_RT_BURST8           3

#define MSC_RBW32               (0 << 2)
#define MSC_RBW16               (1 << 2)

#define MSC_RDF(n_)             (((n_)&0x1f)<<3)
#define MSC_RDN(n_)             (((n_)&0x1f)<<8)
#define MSC_RRR(n_)             (((n_)&0x7)<<13)

#endif
