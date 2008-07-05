/*
 *  SD Card driver for pxa27x
 *  
 *  Copyright (C) 2007 Alex Zhang <celeber2@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <config.h>
#include <common.h>
#include <types.h>
#include <part.h>
#include <mmc.h>
#include <asm/errno.h>
#include <pxa.h>

#if 0
#define debug(fmt, arg...)
#else
#define debug printf
#define MMC_DEBUG
#endif

#if 0
#define info(fmt, arg...)
#else
#define info printf
#endif

#define CONFIG_PXA27X

ulong mmc_bread(int dev_num, ulong blknr, ulong blkcnt, uchar * dst);
struct mmci mmci = {
	.blkdev = {
		   .if_type = IF_TYPE_MMC,
		   .block_read = mmc_bread,
		   },
};

#define NULL  0
#define CFG_OP_COND (1 << 15)
//#define CFG_OP_COND 0 //(1 << 18)

#define MMC_RSP_PRESENT	(1 << 0)
#define MMC_RSP_136	(1 << 1)	/* 136 bit response */
#define MMC_RSP_CRC	(1 << 2)	/* expect valid crc */
#define MMC_RSP_BUSY	(1 << 3)	/* card may send busy */
#define MMC_RSP_OPCODE	(1 << 4)	/* response contains opcode */
#define MMC_CMD_MASK	(3 << 5)	/* command type */
#define MMC_CMD_AC	(0 << 5)
#define MMC_CMD_ADTC	(1 << 5)
#define MMC_CMD_BC	(2 << 5)
#define MMC_CMD_BCR	(3 << 5)

/*
 * These are the response types, and correspond to valid bit
 * patterns of the above flags.  One additional valid pattern
 * is all zeros, which means we don't expect a response.
 */
#define MMC_RSP_NONE	(0)
#define MMC_RSP_R1	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2	(MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3	(MMC_RSP_PRESENT)
#define MMC_RSP_R6	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define mmc_resp_type(flags)	(flags & (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC|MMC_RSP_BUSY|MMC_RSP_OPCODE))

#define CMDAT_DMAEN		(1 << 7)
#define CMDAT_INIT		(1 << 6)
#define CMDAT_BUSY		(1 << 5)
#define CMDAT_STREAM		(1 << 4)	/* 1 = stream */
#define CMDAT_WRITE		(1 << 3)	/* 1 = write */
#define CMDAT_DATAEN		(1 << 2)
#define CMDAT_RESP_NONE		(0 << 0)
#define CMDAT_RESP_SHORT	(1 << 0)
#define CMDAT_RESP_R2		(2 << 0)
#define CMDAT_RESP_R3		(3 << 0)

static int mmc_cmd(ushort cmd, ulong arg, ushort flags, ulong * resp)
{
	ulong status, v;
	int i;
	ushort cmdat = 0;

#if 0
	if (flags & CMDAT_INIT == CMDAT_INIT)
		cmdat = CMDAT_INIT;
	else {

		pxamci_setup_data(host, mrq->data);

		cmdat &= ~CMDAT_BUSY;
		cmdat |= CMDAT_DATAEN | CMDAT_DMAEN;
		if (mrq->data->flags & MMC_DATA_WRITE)
			cmdat |= CMDAT_WRITE;

		if (mrq->data->flags & MMC_DATA_STREAM)
			cmdat |= CMDAT_STREAM;

		if (flags & MMC_RSP_BUSY)
			cmdat |= CMDAT_BUSY;
#define RSP_TYPE(x)	((x) & ~(MMC_RSP_BUSY|MMC_RSP_OPCODE))
		switch (RSP_TYPE(mmc_resp_type(cmdat))) {
		case RSP_TYPE(MMC_RSP_R1):	/* r1, r1b, r6, r7 */
			cmdat |= CMDAT_RESP_SHORT;
			break;
		case RSP_TYPE(MMC_RSP_R3):
			cmdat |= CMDAT_RESP_R3;
			break;
		case RSP_TYPE(MMC_RSP_R2):
			cmdat |= CMDAT_RESP_R2;
			break;
		default:
			break;
		}
#else
	switch (cmd) {
	case 0:
		cmdat = 0x40;
		break;
	case 2:
		cmdat = 0x2;
		break;
	case 3:
		cmdat = 0x1;
		break;
	case 8:
		cmdat = 0x1;
		break;
	case 41:
		cmdat = 0x3;
		break;
	case 55:
		cmdat = 0x1;
		break;
	case 9:
		cmdat = 0x2;
		break;
	default:
		cmdat = flags;
	}
#endif
	debug("CMD%d: %x %x %x\n", cmd, arg, cmdat, flags);

	MMC_STRPCL = MMC_STRPCL_STOP_CLK;
	MMC_I_MASK = ~MMC_I_MASK_CLK_IS_OFF;
	while (!(MMC_I_REG & MMC_I_REG_CLK_IS_OFF)) ;
	MMC_CMD = cmd;
	MMC_ARGH = (ushort) (arg >> 16);
	MMC_ARGL = (ushort) (arg >> 0);
	MMC_CMDAT = cmdat;
	MMC_I_MASK = ~MMC_I_MASK_END_CMD_RES;
	MMC_STRPCL = MMC_STRPCL_START_CLK;
	while (!(MMC_I_REG & MMC_I_REG_END_CMD_RES)) ;

	status = MMC_STAT;
	debug("MMC status %x\n", status);
	if (status & MMC_STAT_TIME_OUT_RESPONSE) {
		debug("MMC_CMD TIMEOUT\n");
		return -1;
	}

	if (resp == NULL)
		return 0;

	v = MMC_RES & 0xffff;
	for (i = 0; i < 4; i++) {
		ulong w1 = MMC_RES & 0xffff;
		ulong w2 = MMC_RES & 0xffff;
		resp[i] = v << 24 | w1 << 8 | w2 >> 8;
		v = w2;
	}
#ifdef MMC_DEBUG
	debug("R%02d: %08X %08X\n     %08X %08X\n", cmd, resp[0], resp[1],
	      resp[2], resp[3]);
#endif // MMC_DEBUG
	return 0;
}

static int mmc_idle_cards(void)
{
	/* Reset and initialize all cards */
	mmc_cmd(0, 0, 0x40, NULL);
	udelay(10000);		/* Keep the bus idle for 74 clock cycles */
	mmc_cmd(8, 0x01aa, 0x75, NULL);
	return 0;
}

static int mmc_acmd(ushort cmd, ulong arg, ushort flags, ulong * resp)
{
	mmc_cmd(55, 0, 0x15, resp);
	udelay(1000);
	return mmc_cmd(cmd, arg, flags, resp);
}

int mmc_card_sd(void)
{
	return 1;		// FIXME
}

/*
 * OCR Bit positions to 10s of Vdd mV.
 */
static const unsigned short mmc_ocr_bit_to_vdd[] = {
	150, 155, 160, 165, 170, 180, 190, 200,
	210, 220, 230, 240, 250, 260, 270, 280,
	290, 300, 310, 320, 330, 340, 350, 360
};

static const unsigned int tran_exp[] = {
	10000, 100000, 1000000, 10000000,
	0, 0, 0, 0
};

static const unsigned char tran_mant[] = {
	0, 10, 12, 13, 15, 20, 25, 30,
	35, 40, 45, 50, 55, 60, 70, 80,
};

static const unsigned int tacc_exp[] = {
	1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
};

static const unsigned int tacc_mant[] = {
	0, 10, 12, 13, 15, 20, 25, 30,
	35, 40, 45, 50, 55, 60, 70, 80,
};

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const ulong __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		ulong __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
static void mmc_decode_cid(struct mmci *mmc)
{
	struct mmc_cid *cid = &mmc->cid;
	ulong *resp = &mmc->raw_cid;
	memset(cid, 0, sizeof(struct mmc_cid));

	if (mmc_card_sd()) {
		/*
		 * SD doesn't currently have a version field so we will
		 * have to assume we can parse this.
		 */
		cid->manfid = UNSTUFF_BITS(resp, 120, 8);
		cid->oemid = UNSTUFF_BITS(resp, 104, 16);
		cid->prod_name[0] = UNSTUFF_BITS(resp, 96, 8);
		cid->prod_name[1] = UNSTUFF_BITS(resp, 88, 8);
		cid->prod_name[2] = UNSTUFF_BITS(resp, 80, 8);
		cid->prod_name[3] = UNSTUFF_BITS(resp, 72, 8);
		cid->prod_name[4] = UNSTUFF_BITS(resp, 64, 8);
		cid->hwrev = UNSTUFF_BITS(resp, 60, 4);
		cid->fwrev = UNSTUFF_BITS(resp, 56, 4);
		cid->serial = UNSTUFF_BITS(resp, 24, 32);
		cid->year = UNSTUFF_BITS(resp, 12, 8);
		cid->month = UNSTUFF_BITS(resp, 8, 4);

		cid->year += 2000;	/* SD cards year offset */
	} else {
		/*
		 * The selection of the format here is based upon published
		 * specs from sandisk and from what people have reported.
		 */
		switch (mmc->csd.mmca_vsn) {
		case 0:	/* MMC v1.0 - v1.2 */
		case 1:	/* MMC v1.4 */
			cid->manfid = UNSTUFF_BITS(resp, 104, 24);
			cid->prod_name[0] = UNSTUFF_BITS(resp, 96, 8);
			cid->prod_name[1] = UNSTUFF_BITS(resp, 88, 8);
			cid->prod_name[2] = UNSTUFF_BITS(resp, 80, 8);
			cid->prod_name[3] = UNSTUFF_BITS(resp, 72, 8);
			cid->prod_name[4] = UNSTUFF_BITS(resp, 64, 8);
			cid->prod_name[5] = UNSTUFF_BITS(resp, 56, 8);
			cid->prod_name[6] = UNSTUFF_BITS(resp, 48, 8);
			cid->hwrev = UNSTUFF_BITS(resp, 44, 4);
			cid->fwrev = UNSTUFF_BITS(resp, 40, 4);
			cid->serial = UNSTUFF_BITS(resp, 16, 24);
			cid->month = UNSTUFF_BITS(resp, 12, 4);
			cid->year = UNSTUFF_BITS(resp, 8, 4) + 1997;
			break;

		case 2:	/* MMC v2.0 - v2.2 */
		case 3:	/* MMC v3.1 - v3.3 */
		case 4:	/* MMC v4 */
			cid->manfid = UNSTUFF_BITS(resp, 120, 8);
			cid->oemid = UNSTUFF_BITS(resp, 104, 16);
			cid->prod_name[0] = UNSTUFF_BITS(resp, 96, 8);
			cid->prod_name[1] = UNSTUFF_BITS(resp, 88, 8);
			cid->prod_name[2] = UNSTUFF_BITS(resp, 80, 8);
			cid->prod_name[3] = UNSTUFF_BITS(resp, 72, 8);
			cid->prod_name[4] = UNSTUFF_BITS(resp, 64, 8);
			cid->prod_name[5] = UNSTUFF_BITS(resp, 56, 8);
			cid->serial = UNSTUFF_BITS(resp, 16, 32);
			cid->month = UNSTUFF_BITS(resp, 12, 4);
			cid->year = UNSTUFF_BITS(resp, 8, 4) + 1997;
			break;

		default:
			printf("MMC: card has unknown MMCA version %d\n",
			       mmc->csd.mmca_vsn);
//                      mmc_card_set_bad(card);
			break;
		}
	}
//      printf("ALEX::::::mid=%02lX, oemid=%04lX, SN=%lu\n", cid->manfid, cid->oemid, cid->serial);
}

/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static void mmc_decode_csd(struct mmci *mmc)
{
	struct mmc_csd *csd = &mmc->csd;
	ulong *resp = &mmc->raw_csd;
	unsigned int e, m, csd_struct;

	if (mmc_card_sd()) {
		csd_struct = UNSTUFF_BITS(resp, 126, 2);

		switch (csd_struct) {
		case 0:
			m = UNSTUFF_BITS(resp, 115, 4);
			e = UNSTUFF_BITS(resp, 112, 3);
			csd->tacc_ns = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
			csd->tacc_clks = UNSTUFF_BITS(resp, 104, 8) * 100;

			m = UNSTUFF_BITS(resp, 99, 4);
			e = UNSTUFF_BITS(resp, 96, 3);
			csd->max_dtr = tran_exp[e] * tran_mant[m];
			csd->cmdclass = UNSTUFF_BITS(resp, 84, 12);

			e = UNSTUFF_BITS(resp, 47, 3);
			m = UNSTUFF_BITS(resp, 62, 12);
			csd->capacity = (1 + m) << (e + 2);

			csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
			csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
			csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
			csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
			csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
			csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
			csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
			break;
		case 1:
			/*
			 * This is a block-addressed SDHC card. Most
			 * interesting fields are unused and have fixed
			 * values. To avoid getting tripped by buggy cards,
			 * we assume those fixed values ourselves.
			 */
//                      mmc_card_set_blockaddr(card);

			csd->tacc_ns = 0;	/* Unused */
			csd->tacc_clks = 0;	/* Unused */

			m = UNSTUFF_BITS(resp, 99, 4);
			e = UNSTUFF_BITS(resp, 96, 3);
			csd->max_dtr = tran_exp[e] * tran_mant[m];
			csd->cmdclass = UNSTUFF_BITS(resp, 84, 12);

			m = UNSTUFF_BITS(resp, 48, 22);
			csd->capacity = (1 + m) << 10;

			csd->read_blkbits = 9;
			csd->read_partial = 0;
			csd->write_misalign = 0;
			csd->read_misalign = 0;
			csd->r2w_factor = 4;	/* Unused */
			csd->write_blkbits = 9;
			csd->write_partial = 0;
			break;
		default:
			printf("MMC: unrecognised CSD structure version %d\n",
			       csd_struct);
//                      mmc_card_set_bad(card);
			return;
		}
	} else {
		/*
		 * We only understand CSD structure v1.1 and v1.2.
		 * v1.2 has extra information in bits 15, 11 and 10.
		 */
		csd_struct = UNSTUFF_BITS(resp, 126, 2);
		if (csd_struct != 1 && csd_struct != 2) {
			printf("MMC: unrecognised CSD structure version %d\n",
			       csd_struct);
			//              mmc_card_set_bad(card);
			return;
		}

		csd->mmca_vsn = UNSTUFF_BITS(resp, 122, 4);
		m = UNSTUFF_BITS(resp, 115, 4);
		e = UNSTUFF_BITS(resp, 112, 3);
		csd->tacc_ns = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		csd->tacc_clks = UNSTUFF_BITS(resp, 104, 8) * 100;

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr = tran_exp[e] * tran_mant[m];
		csd->cmdclass = UNSTUFF_BITS(resp, 84, 12);

		e = UNSTUFF_BITS(resp, 47, 3);
		m = UNSTUFF_BITS(resp, 62, 12);
		csd->capacity = (1 + m) << (e + 2);

		csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
	}
//      printf("ALEX::::::struct Ver=1.%d, cmdclass=%d, RBL=%d, WBL=%d, capacity=%lu\n", csd_struct, csd->cmdclass, csd->read_blkbits, csd->write_blkbits, csd->capacity);
}

static void mmc_dump_cid(const struct mmc_cid *cid)
{
	info("MID:   %02lX\n", cid->manfid);
	info("OID:   %04lX\n", cid->oemid);
	info("name:  %s\n", cid->prod_name);
	info("Rev:   %lu.%lu\n", cid->hwrev, cid->fwrev);
	info("SN:    %lu\n", cid->serial);
	info("Date:  %02lu/%02lu\n", cid->year, cid->month);
}

static void mmc_dump_csd(const struct mmc_csd *csd)
{
//      debug("CSD Struct Ver: 1.%u\n", csd->mmca_vsn); /* CSD structure version */
	info("MMC Sys Spec Ver: %u\n", csd->mmca_vsn);	/* MMC System Spec version */
	info("CMD classes: %03x\n", csd->cmdclass);	/* Card Command Classes */
	info("Read Block len: %u\n", 1 << csd->read_blkbits);	/* Read Block Length */
	info("Partial reads:");
	if (csd->read_partial)
		info("YES\n");
	else
		info("NO\n");
	info("Write Block len: %u\n", 1 << csd->write_blkbits);	/* Write block length */
	info("Partial writes:");
	if (csd->write_partial)
		info("YES\n");
	else
		info("NO\n");
	info("Capacity: %uKiB\n", (csd->capacity) >> 1);

#if 0
	if (csd->wp_grp_enable)
		debug("Group WP: %u\n", csd->wp_grp_size + 1);
	else
		debug("Group WP:NO\n");
	debug("File format: %u/%u\n", csd->file_format_grp, csd->file_format);
	debug("WP:");		/* Write Protection */
	if (csd->perm_write_protect)
		debug(" permanent");
	if (csd->tmp_write_protect)
		debug(" temporary");
	debug('\n');
#endif
}

static int sd_init_card(struct mmci *mmc, int verbose)
{
	int i;
	int retries = 10;
	int ret;
	ulong resp[4];

      reset:
	mmc_idle_cards();
	for (i = 0; i < retries; i++) {
		ret = mmc_acmd(41, 0x00010000, 0x61, &resp);
		if (resp[0] & 0x80000000) {	/* POWER UP of clear BUSY BIT */
			debug("YEAHHHHH, SD initialized\n");
			break;
		}
		udelay(200000);
	}
	if (i == retries) {
		printf("CMD41 Timeout\n");
		return -1;
	}
	udelay(10000);

	ret = mmc_cmd(2, 0, 0x67, &mmc->raw_cid);
	if (ret < 0)
		return ret;

	/* Get RCA of the card that responded */
	ret = mmc_cmd(3, 0, 0x75, &resp);
	if (ret < 0)
		return ret;
	mmc->rca = resp[0] >> 16;
	if (verbose)
		printf("SD Card detected:\n(RCA = %u)\n", mmc->rca);
	return 0;
}

/* MMC interface */

#define GPIO_MMC_CLK            32
#define GPIO_MMC_DATA0          92
//#define GPIO_MMC_WP           107
#define GPIO_MMC_DATA1          109
#define GPIO_MMC_DATA2          110
#define GPIO_MMC_DATA3          111
#define GPIO_MMC_CMD            112
#define GPIO_MMC_DETECT         GPIO_MMC_DATA3

/* interface function */
#define GPIO_MMC_CLK_MD         (GPIO_MMC_CLK | GPIO_ALT_FN_2_OUT)
#define GPIO_MMC_DATA0_MD       (GPIO_MMC_DATA0 | GPIO_ALT_FN_1_IN | GPIO_ALT_FN_1_OUT)
#define GPIO_MMC_DATA1_MD       (GPIO_MMC_DATA1 | GPIO_ALT_FN_1_IN | GPIO_ALT_FN_1_OUT)
#define GPIO_MMC_DATA2_MD       (GPIO_MMC_DATA2 | GPIO_ALT_FN_1_IN | GPIO_ALT_FN_1_OUT)
#define GPIO_MMC_DATA3_MD       (GPIO_MMC_DATA3 | GPIO_ALT_FN_1_IN | GPIO_ALT_FN_1_OUT)

#define GPIO_MMC_CMD_MD         (GPIO_MMC_CMD | GPIO_ALT_FN_1_IN | GPIO_ALT_FN_1_OUT)

////////////
#define GPIO32_MMCCLK_MD		( 32 | GPIO_ALT_FN_2_OUT)
#define GPIO112_MMCCMD_MD	    (112 | GPIO_ALT_FN_1_OUT)
#define GPIO92_MMCDAT0_MD	(92 | GPIO_ALT_FN_1_OUT)
#define GPIO109_MMCDAT1_MD	(109 | GPIO_ALT_FN_1_OUT)
#define GPIO110_MMCDAT2_MD	(110 | GPIO_ALT_FN_1_OUT)
#define GPIO111_MMCDAT3_MD	(111 | GPIO_ALT_FN_1_OUT)

int
/****************************************************/
mmc_init(int verbose)
{
	int ret;
	unsigned int rate;

#if 1				// GPIO
	/* Setup GPIO for PXA27x MMC/SD controller */
	set_GPIO_mode(GPIO32_MMCCLK_MD);
	set_GPIO_mode(GPIO112_MMCCMD_MD);
	set_GPIO_mode(GPIO92_MMCDAT0_MD);
	set_GPIO_mode(GPIO109_MMCDAT1_MD);
	set_GPIO_mode(GPIO110_MMCDAT2_MD);
	set_GPIO_mode(GPIO111_MMCDAT3_MD);
#endif

	CKEN |= CKEN12_MMC;	/* enable MMC unit clock */

	MMC_CLKRT  = MMC_CLKRT_0_3125MHZ;
	MMC_RESTO = MMC_RES_TO_MAX;
	MMC_SPI = MMC_SPI_DISABLE;

	ret = sd_init_card(&mmci, verbose);
#if 0
	if (ret) {
		mmci.rca = MMC_DEFAULT_RCA;
		ret = mmc_init_card(&mmci, &cid, verbose);
	}
#endif
	if (ret)
		return ret;
	/* Get CSD from the card CMD9 */
	ret = mmc_cmd(MMC_CMD_SEND_CSD, mmci.rca << 16, 0x0007, &mmci.raw_csd);
	if (ret)
		return ret;
	mmc_decode_csd(&mmci);
	mmc_decode_cid(&mmci);
	if (verbose) {
		mmc_dump_csd(&mmci.csd);
		mmc_dump_cid(&mmci.cid);
	}
	if (ret)
		return ret;


	MMC_STRPCL = MMC_STRPCL_STOP_CLK;
	MMC_I_MASK = ~MMC_I_MASK_CLK_IS_OFF;
	while(!(MMC_I_REG & MMC_I_REG_CLK_IS_OFF));
	MMC_CLKRT = (mmci.csd.max_dtr >= 19500000 ? MMC_CLKRT_20MHZ :
			mmci.csd.max_dtr >= 9750000  ? MMC_CLKRT_10MHZ :
			mmci.csd.max_dtr >= 4880000  ? MMC_CLKRT_5MHZ :
			mmci.csd.max_dtr >= 2440000  ? MMC_CLKRT_2_5MHZ :
			mmci.csd.max_dtr >= 1220000  ? MMC_CLKRT_1_25MHZ :
			mmci.csd.max_dtr >= 609000   ? MMC_CLKRT_0_625MHZ :
			MMC_CLKRT_0_3125MHZ);

	/* Initialize the blockdev structure */
	sprintf(mmci.blkdev.vendor,
		"Man %02x%04x Snr %08x",
		mmci.cid.manfid, mmci.cid.oemid, mmci.cid.serial);
	strncpy(mmci.blkdev.product, mmci.cid.prod_name,
		sizeof(mmci.blkdev.product));
	sprintf(mmci.blkdev.revision, "%x %x", mmci.cid.hwrev, mmci.cid.hwrev);
	mmci.blkdev.dev = 0;
	mmci.blkdev.blksz = 1 << mmci.csd.read_blkbits;
	mmci.blkdev.lba = mmci.csd.capacity;

#if 1
	mmci.blkdev.part_type = PART_TYPE_DOS;
	mmci.blkdev.dev = 0;
	mmci.blkdev.lun = 0;
	mmci.blkdev.type = 0;
	if (fat_register_device(&mmci.blkdev, 1))
		printf("Could not register MMC fat device\n");
#else
	init_part(&mmci.blkdev);
	if (verbose)
		print_part_dos(&mmci.blkdev);
#endif
	return 0;
}

int mmc_exit (void)
{
	MMC_STRPCL = MMC_STRPCL_STOP_CLK;
	MMC_I_MASK = ~MMC_I_MASK_CLK_IS_OFF;
	while (!(MMC_I_REG & MMC_I_REG_CLK_IS_OFF));
	MMC_I_MASK = (MMC_I_MASK_TXFIFO_WR_REQ|MMC_I_MASK_RXFIFO_RD_REQ
			|MMC_I_MASK_CLK_IS_OFF|MMC_I_MASK_STOP_CMD
			|MMC_I_MASK_END_CMD_RES|MMC_I_MASK_PRG_DONE
			|MMC_I_MASK_DATA_TRAN_DONE);
	CKEN &= ~(CKEN12_MMC);
	set_GPIO_mode(32 | GPIO_IN);
	set_GPIO_mode(112 | GPIO_IN);
	set_GPIO_mode(92 | GPIO_IN);
	set_GPIO_mode(109 | GPIO_IN);
	set_GPIO_mode(110 | GPIO_IN);
	set_GPIO_mode(111 | GPIO_IN);

}

static unsigned long
mmc_bread(int dev, unsigned long start, lbaint_t blkcnt, unsigned char *buffer)
{
	int ret, i = 0;
	unsigned long resp[4];
	unsigned long card_status, status;
	char data;
	int count;
	struct mmci *mmc = &mmci;

	if (blkcnt == 0)
		return 0;

	debug("mmc_bread: dev %d, \nstart %lx, blkcnt %lx\n", dev, start,
	      blkcnt);

	/* Put the device into Transfer state */
	ret = mmc_cmd(MMC_CMD_SELECT_CARD, mmci.rca << 16, MMC_CMDAT_R1, &resp);
	if (ret)
		goto fail;

	/* Set block length */
	/* 512 regardless of what the card reports, for compatibility */
	mmc->blkdev.blksz = 512;
	ret =
	    mmc_cmd(MMC_CMD_SET_BLOCKLEN, mmc->blkdev.blksz, MMC_CMDAT_R1,
		    &resp);
	if (ret)
		goto fail;

	for (i = 0; i < blkcnt; i++, start++) {
		/* send read command */
//		MMC_STRPCL = MMC_STRPCL_STOP_CLK;
		MMC_RDTO = 0xffff;
		MMC_NOB = 1;
		MMC_BLKLEN = mmc->blkdev.blksz;
		ret = mmc_cmd(MMC_CMD_READ_SINGLE_BLOCK,
			      start * mmc->blkdev.blksz,
			      (MMC_CMDAT_R1 | MMC_CMDAT_DATA_EN | MMC_CMDAT_READ
			       | MMC_CMDAT_BLOCK), &resp);
		if (ret)
			goto fail;

		ret = -EIO;
		count = mmc->blkdev.blksz;

		MMC_I_MASK = ~MMC_I_MASK_RXFIFO_RD_REQ;
		do {
			if (MMC_I_REG & MMC_I_REG_RXFIFO_RD_REQ) {
				int j;
				for (j = min(count, 32); j; j--) {
					data = *((volatile uchar *)&MMC_RXFIFO);
					*buffer++ = data;
//					debug("%03d:%02x\n", count, data);
					count--;
				}
			}
//                      debug(">");
			status = MMC_STAT;
			if (status & MMC_STAT_ERRORS)
				goto fail;
		} while (count);
		debug("mmc: read %u bytes, waiting for BLKE\n",
		      mmc->blkdev.blksz - count);

		MMC_I_MASK = ~MMC_I_MASK_DATA_TRAN_DONE;
		while (!(MMC_I_REG & MMC_I_REG_DATA_TRAN_DONE)) ;

		status = MMC_STAT;
		if (status & MMC_STAT_ERRORS) {
			printf("MMC_STAT error %lx\n", status);
			goto fail;
		}
	}

      out:
	/* Put the device back into Standby state */
	mmc_cmd(MMC_CMD_SELECT_CARD, 0, MMC_CMDAT_R1, &resp);
	return i;

      fail:
	printf("mmc: bread failed, SR = %08lx", status);
	mmc_cmd(MMC_CMD_SEND_STATUS, mmc->rca << 16, &card_status,
		MMC_CMDAT_R1);
	printf(", card status = %08lx\n", card_status);
	i = -1;
	goto out;
}
