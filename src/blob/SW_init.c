/*
 * src/blob/SW_init.c
 * 
 * special software init code for ezx platform
 *
 * Copyright (C) 2005 Motorola Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
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
 * 2005-Jun-06 - (Motorola) init draft
 *
 */
#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif


#include <blob/main.h>
#include <blob/log.h>
extern blob_status_t blob_status;

#ifdef BARBADOS
u32 barbados_sw_init(u32 x){
  /* initialise status */
  blob_status.paramType = fromFlash;
  blob_status.kernelType = fromFlash;
  blob_status.ramdiskType = fromFlash;
  blob_status.downloadSpeed = baud_115200;
  blob_status.terminalSpeed = baud_115200;
  blob_status.load_ramdisk = 1;
  blob_status.boot_delay = 10;
  logstr("\nbarbados_sw_init()");
  return 0;
}

#endif

#ifdef MARTINIQUE
u32 martinique_sw_init(u32 x){
  /* initialise status */
  blob_status.paramType = fromFlash;
  blob_status.kernelType = fromFlash;
  blob_status.ramdiskType = fromFlash;
  blob_status.downloadSpeed = baud_115200;
  blob_status.terminalSpeed = baud_115200;
  blob_status.load_ramdisk = 1;
  blob_status.boot_delay = 10;
  logstr("\nmartinique_sw_init()");
  return 0;
}

#endif

#ifdef HAINAN
u32 hainan_sw_init(u32 x){
  /* initialise status */
  blob_status.paramType = fromFlash;
  blob_status.kernelType = fromFlash;
  blob_status.ramdiskType = fromFlash;
  blob_status.downloadSpeed = baud_115200;
  blob_status.terminalSpeed = baud_115200;
  blob_status.load_ramdisk = 1;
  blob_status.boot_delay = 10;
  logstr("\nhainan_sw_init()");
  return 0;
}

#endif

#ifdef SUMATRA
u32 sumatra_sw_init(u32 x){
  /* initialise status */
  blob_status.paramType = fromFlash;
  blob_status.kernelType = fromFlash;
  blob_status.ramdiskType = fromFlash;
  blob_status.downloadSpeed = baud_115200;
  blob_status.terminalSpeed = baud_115200;
  blob_status.load_ramdisk = 1;
  blob_status.boot_delay = 10;
  logstr("\nsumatra_sw_init()");
  return 0;
}

#endif
