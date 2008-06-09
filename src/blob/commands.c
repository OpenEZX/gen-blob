/*
 * commands.c: commands for blob
 *
 * Copyright (C) 2001  Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 *
 * $Id: commands.c,v 1.2 2001/10/07 22:36:11 erikm Exp $
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

#ident "$Id: commands.c,v 1.2 2001/10/07 22:36:11 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif


#include <blob/command.h>
#include <blob/reboot.h>
#include <blob/terminal.h>




__commandlist(reset_terminal, "reset", resethelp);
__commandlist(reboot, "reboot", reboothelp);
