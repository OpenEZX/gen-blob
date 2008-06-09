/*
 * include/blob/log.h
 *
 * header file for output log in blob
 *
 * Copyright (C) 2005 Motorola
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
 * 2005-May-14 - (Motorola) init draft
 *
 */

#ifndef BLOB_LOG_H
#define BLOB_LOG_H
/* we us ST serial port now... */
extern void STSerialOutputString(const char *s);
extern void STSerialOutputHex(const u32 h);
extern void STSerialOutputDec(const u32 d);

/* chang below macros definatio to select output functions... */
#define OUTPUT_STR(x) STSerialOutputString(x) 
#define OUTPUT_HEX(x) STSerialOutputHex(x)
#define OUTPUT_DEC(x) STSerialOutputDec(x)


/* keep below general macros unchanged... */
#define logstr(x) OUTPUT_STR(x)
#define logvarhex(name,value) {OUTPUT_STR(name);\
                               OUTPUT_HEX(value);\
                               OUTPUT_STR("\n");}

#define logvardec(name,value) {OUTPUT_STR(name);\
                               OUTPUT_DEC(value);\
                               OUTPUT_STR("\n");}




#endif
