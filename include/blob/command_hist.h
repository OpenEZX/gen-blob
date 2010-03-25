/**********************************************************************
 *  command_hist.h
 *
 *	AUTOR:	SELETZ
 *	
 *	Header file for command_hist.h
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
 *
 */

/* $Id: command_hist.h,v 1.1 2001/10/07 23:32:05 erikm Exp $ */

#ifndef BLOB_COMMAND_HIST_H
#define BLOB_COMMAND_HIST_H

#define CMDHIST_KEY_UP	'\025'		// ^U
#define CMDHIST_KEY_DN	'\004'		// ^D

int cmdhist_push( char *cmd );
int cmdhist_next( char **cmd );
int cmdhist_prev( char **cmd );
int cmdhist_reset( void );

#endif
