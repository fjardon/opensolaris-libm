/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"@(#)csinf.c	1.3	06/01/31 SMI"

#pragma weak csinf = __csinf

#include "libm.h"
#include "complex_wrapper.h"

fcomplex
csinf(fcomplex z) {
	float x, y;
	fcomplex ans, ct;

	x = F_RE(z);
	y = F_IM(z);
	F_RE(z) = y;
	F_IM(z) = -x;
	ct = csinhf(z);
	F_RE(ans) = -F_IM(ct);
	F_IM(ans) = F_RE(ct);
	return (ans);
}
