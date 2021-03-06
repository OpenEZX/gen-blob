/*
 *  linux/drivers/mmc/mmc_pxa.h
 *
 *  Author: Vladimir Shebordaev, Igor Oblakov
 *  Copyright:  MontaVista Software Inc.
 *
 *  $Id: mmc_pxa.h,v 0.3.1.6 2002/09/25 19:25:48 ted Exp ted $
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#ifndef __MMC_PXA_P_H__
#define __MMC_PXA_P_H__

/* PXA-250 MMC controller registers */

/* MMC_STRPCL */
#define MMC_STRPCL_STOP_CLK     	(0x0001UL)
#define MMC_STRPCL_START_CLK		(0x0002UL)

/* MMC_STAT */
#define MMC_STAT_END_CMD_RES		(0x0001UL << 13)
#define MMC_STAT_PRG_DONE       	(0x0001UL << 12)
#define MMC_STAT_DATA_TRAN_DONE     	(0x0001UL << 11)
#define MMC_STAT_CLK_EN	 		(0x0001UL << 8)
#define MMC_STAT_RECV_FIFO_FULL     	(0x0001UL << 7)
#define MMC_STAT_XMIT_FIFO_EMPTY    	(0x0001UL << 6)
#define MMC_STAT_RES_CRC_ERROR      	(0x0001UL << 5)
#define MMC_STAT_SPI_READ_ERROR_TOKEN   (0x0001UL << 4)
#define MMC_STAT_CRC_READ_ERROR     	(0x0001UL << 3)
#define MMC_STAT_CRC_WRITE_ERROR    	(0x0001UL << 2)
#define MMC_STAT_TIME_OUT_RESPONSE  	(0x0001UL << 1)
#define MMC_STAT_READ_TIME_OUT      	(0x0001UL)

#define MMC_STAT_ERRORS (MMC_STAT_RES_CRC_ERROR|MMC_STAT_SPI_READ_ERROR_TOKEN\
	|MMC_STAT_CRC_READ_ERROR|MMC_STAT_TIME_OUT_RESPONSE\
	|MMC_STAT_READ_TIME_OUT|MMC_STAT_CRC_WRITE_ERROR)

/* MMC_CLKRT */
#define MMC_CLKRT_20MHZ	 		(0x0000UL)
#define MMC_CLKRT_10MHZ	 		(0x0001UL)
#define MMC_CLKRT_5MHZ	  		(0x0002UL)
#define MMC_CLKRT_2_5MHZ		(0x0003UL)
#define MMC_CLKRT_1_25MHZ       	(0x0004UL)
#define MMC_CLKRT_0_625MHZ      	(0x0005UL)
#define MMC_CLKRT_0_3125MHZ     	(0x0006UL)

/* MMC_SPI */
#define MMC_SPI_DISABLE	 		(0x00UL)
#define MMC_SPI_EN	  		(0x01UL)
#define MMC_SPI_CS_EN	   		(0x01UL << 2)
#define MMC_SPI_CS_ADDRESS      	(0x01UL << 3)
#define MMC_SPI_CRC_ON	  		(0x01UL << 1)

#if 1
/* MMC_CMDAT */
#define	MMC_CMDAT_STOP_TRAN		(0x0001UL << 10)
#define MMC_CMDAT_MMC_DMA_EN		(0x0001UL << 7)
#define MMC_CMDAT_INIT	  		(0x0001UL << 6)
#define MMC_CMDAT_BUSY	  		(0x0001UL << 5)
#define MMC_CMDAT_STREAM		(0x0001UL << 4)
#define MMC_CMDAT_BLOCK	 		(0x0000UL << 4)
#define MMC_CMDAT_WRITE	 		(0x0001UL << 3)
#define MMC_CMDAT_READ	  		(0x0000UL << 3)
#define MMC_CMDAT_DATA_EN       	(0x0001UL << 2)
#define MMC_CMDAT_NORESP		0
#define MMC_CMDAT_R1	    		(0x0001UL)
#define MMC_CMDAT_R2	    		(0x0002UL)
#define MMC_CMDAT_R3	    		(0x0003UL)
#endif

/* MMC_RESTO */
#define MMC_RES_TO_MAX	  		(0x007fUL) /* [6:0] */

/* MMC_RDTO */
#define MMC_READ_TO_MAX	 		(0x0ffffUL) /* [15:0] */

/* MMC_BLKLEN */
#define MMC_BLK_LEN_MAX	 		(0x03ffUL) /* [9:0] */

/* MMC_PRTBUF */
#define MMC_PRTBUF_BUF_PART_FULL       	(0x01UL)
#define MMC_PRTBUF_BUF_FULL		(0x00UL    )

/* MMC_I_MASK */
#define MMC_I_MASK_TXFIFO_WR_REQ	(0x01UL << 6)
#define MMC_I_MASK_RXFIFO_RD_REQ	(0x01UL << 5)
#define MMC_I_MASK_CLK_IS_OFF	   	(0x01UL << 4)
#define MMC_I_MASK_STOP_CMD	 	(0x01UL << 3)
#define MMC_I_MASK_END_CMD_RES	  	(0x01UL << 2)
#define MMC_I_MASK_PRG_DONE	 	(0x01UL << 1)
#define MMC_I_MASK_DATA_TRAN_DONE       (0x01UL)
#define MMC_I_MASK_ALL	      		(0x07fUL)


/* MMC_I_REG */
#define MMC_I_REG_TXFIFO_WR_REQ     	(0x01UL << 6)
#define MMC_I_REG_RXFIFO_RD_REQ     	(0x01UL << 5)
#define MMC_I_REG_CLK_IS_OFF		(0x01UL << 4)
#define MMC_I_REG_STOP_CMD      	(0x01UL << 3)
#define MMC_I_REG_END_CMD_RES       	(0x01UL << 2)
#define MMC_I_REG_PRG_DONE      	(0x01UL << 1)
#define MMC_I_REG_DATA_TRAN_DONE    	(0x01UL)
#define MMC_I_REG_ALL	   		(0x007fUL)


/* MMC_CMD */
#define MMC_CMD_INDEX_MAX       	(0x006fUL)  /* [5:0] */
#define CMD(x)  (x)

#define MMC_DEFAULT_RCA			1

#define MMC_BLOCK_SIZE			512
#define MMC_CMD_CIM_RESET		-1
#define	MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID 		2
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SET_DSR			4
#define	MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_CSD 		9
#define MMC_CMD_SEND_CID 		10
#define	MMC_CMD_READ_DAT_UNTIL_STOP	11
#define	MMC_CMD_STOP_TRANSMISSION	12
#define MMC_CMD_SEND_STATUS		13
#define	MMC_CMD_GO_INACTIVE_STATE	15
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define	MMC_CMD_WRITE_DAT_UNTIL_STOP	20
#define	MMC_CMD_SET_BLOCK_COUNT		23
#define MMC_CMD_WRITE_BLOCK		24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	25
#define MMC_CMD_SD_SEND_OP_COND		41
#define MMC_MAX_BLOCK_SIZE		512

#define MMC_R1_IDLE_STATE		0x01
#define MMC_R1_ERASE_STATE		0x02
#define MMC_R1_ILLEGAL_CMD		0x04
#define MMC_R1_COM_CRC_ERR		0x08
#define MMC_R1_ERASE_SEQ_ERR		0x01
#define MMC_R1_ADDR_ERR			0x02
#define MMC_R1_PARAM_ERR		0x04

#define MMC_R1B_WP_ERASE_SKIP		0x0002
#define MMC_R1B_ERR			0x0004
#define MMC_R1B_CC_ERR			0x0008
#define MMC_R1B_CARD_ECC_ERR		0x0010
#define MMC_R1B_WP_VIOLATION		0x0020
#define MMC_R1B_ERASE_PARAM		0x0040
#define MMC_R1B_OOR			0x0080
#define MMC_R1B_IDLE_STATE		0x0100
#define MMC_R1B_ERASE_RESET		0x0200
#define MMC_R1B_ILLEGAL_CMD		0x0400
#define MMC_R1B_COM_CRC_ERR		0x0800
#define MMC_R1B_ERASE_SEQ_ERR		0x1000
#define MMC_R1B_ADDR_ERR		0x2000
#define MMC_R1B_PARAM_ERR		0x4000

/* MMC Command numbers */
#define MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID 		2
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SD_SEND_RELATIVE_ADDR	3
#define MMC_CMD_SET_DSR			4
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_CSD 		9
#define MMC_CMD_SEND_CID 		10
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define MMC_CMD_WRITE_BLOCK		24
#define MMC_CMD_APP_CMD			55

#define MMC_ACMD_SD_SEND_OP_COND	41

#define R1_ILLEGAL_COMMAND		(1 << 22)
#define R1_APP_CMD			(1 << 5)

#define RESPONSE_NONE	0
#define RESPONSE_R1	1
#define RESPONSE_R1B	2
#define RESPONSE_R2_CID	3
#define RESPONSE_R2_CSD	4
#define RESPONSE_R3	5
#define RESPONSE_R4	6
#define RESPONSE_R5	7
#define RESPONSE_R6	8


struct mmc_cid {
	unsigned int		manfid;
	char			prod_name[8];
	unsigned int		serial;
	unsigned short		oemid;
	unsigned short		year;
	unsigned char		hwrev;
	unsigned char		fwrev;
	unsigned char		month;
};

struct mmc_csd {
	unsigned char		mmca_vsn;
	unsigned short		cmdclass;
	unsigned short		tacc_clks;
	unsigned int		tacc_ns;
	unsigned int		r2w_factor;
	unsigned int		max_dtr;
	unsigned int		read_blkbits;
	unsigned int		write_blkbits;
	unsigned int		capacity;
	unsigned int		read_partial:1,
				read_misalign:1,
				write_partial:1,
				write_misalign:1;
};

struct mmci {
	unsigned int rca;
	struct mmc_cid cid;
	struct mmc_csd csd;
	ulong raw_cid[4];
	ulong raw_csd[4];
	block_dev_desc_t blkdev;
	const struct device *dev;
};

#endif /* __MMC_PXA_P_H__ */
