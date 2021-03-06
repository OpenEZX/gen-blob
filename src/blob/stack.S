/*
 * stack.S: blob stack
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: stack.S,v 1.3 2001/11/04 23:16:58 erikm Exp $
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

.ident "$Id: stack.S,v 1.3 2001/11/04 23:16:58 erikm Exp $"

#define __ASSEBMLY__

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif


.section ".stack"
	/* 16kB stack should be enough */
	.space	(16 * 1024)
