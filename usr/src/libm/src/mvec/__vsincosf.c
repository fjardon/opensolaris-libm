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
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"@(#)__vsincosf.c	1.4	06/01/23 SMI"

/*
 * __vsincosf: single precision vector sincos
 *
 * Algorithm:
 *
 * For |x| < pi/4, approximate sin(x) by a polynomial x+x*z*(S0+
 * z*(S1+z*S2)) and cos(x) by a polynomial 1+z*(-1/2+z*(C0+z*(C1+
 * z*C2))), where z = x*x, all evaluated in double precision.
 *
 * Accuracy:
 *
 * The largest error is less than 0.6 ulps.
 */

#include <sys/isa_defs.h>

#ifdef _LITTLE_ENDIAN
#define	HI(x)	*(1+(int *)&x)
#define	LO(x)	*(unsigned *)&x
#else
#define	HI(x)	*(int *)&x
#define	LO(x)	*(1+(unsigned *)&x)
#endif

#ifdef __RESTRICT
#define	restrict _Restrict
#else
#define	restrict
#endif

extern int __vlibm_rem_pio2m(double *, double *, int, int, int);

static const double C[] = {
	-1.66666552424430847168e-01,	/* 2^ -3 * -1.5555460000000 */
	8.33219196647405624390e-03,	/* 2^ -7 *  1.11077E0000000 */
	-1.95187909412197768688e-04,	/* 2^-13 * -1.9956B60000000 */
	1.0,
	-0.5,
	4.16666455566883087158e-02,	/* 2^ -5 *  1.55554A0000000 */
	-1.38873036485165357590e-03,	/* 2^-10 * -1.6C0C1E0000000 */
	2.44309903791872784495e-05,	/* 2^-16 *  1.99E24E0000000 */
	0.636619772367581343075535,	/* 2^ -1  * 1.45F306DC9C883 */
	6755399441055744.0,		/* 2^ 52  * 1.8000000000000 */
	1.570796326734125614166,	/* 2^  0  * 1.921FB54400000 */
	6.077100506506192601475e-11,	/* 2^-34  * 1.0B4611A626331 */
};

#define	S0	C[0]
#define	S1	C[1]
#define	S2	C[2]
#define	one	C[3]
#define	mhalf	C[4]
#define	C0	C[5]
#define	C1	C[6]
#define	C2	C[7]
#define	invpio2	C[8]
#define	c3two51	C[9]
#define	pio2_1  C[10]
#define	pio2_t	C[11]

#define	PREPROCESS(N, sindex, cindex, label)				\
	hx = *(int *)x;							\
	ix = hx & 0x7fffffff;						\
	t = *x;								\
	x += stridex;							\
	if (ix <= 0x3f490fdb) { /* |x| < pi/4 */			\
		if (ix == 0) {						\
			s[sindex] = t;					\
			c[cindex] = one;				\
			goto label;					\
		}							\
		y##N = (double)t;					\
		n##N = 0;						\
	} else if (ix <= 0x49c90fdb) { /* |x| < 2^19*pi */		\
		y##N = (double)t;					\
		medium = 1;						\
	} else {							\
		if (ix >= 0x7f800000) { /* inf or nan */		\
			s[sindex] = c[cindex] = t / t;			\
			goto label;					\
		}							\
		z##N = y##N = (double)t;				\
		hx = HI(y##N);						\
		n##N = ((hx >> 20) & 0x7ff) - 1046;			\
		HI(z##N) = (hx & 0xfffff) | 0x41600000;			\
		n##N = __vlibm_rem_pio2m(&z##N, &y##N, n##N, 1, 0);	\
		if (hx < 0) {						\
			y##N = -y##N;					\
			n##N = -n##N;					\
		}							\
		z##N = y##N * y##N;					\
		f##N = (float)(y##N + y##N * z##N * (S0 + z##N *	\
		    (S1 + z##N * S2)));					\
		g##N = (float)(one + z##N * (mhalf + z##N * (C0 +	\
		    z##N * (C1 + z##N * C2))));				\
		if (n##N & 2) {						\
			f##N = -f##N;					\
			g##N = -g##N;					\
		}							\
		if (n##N & 1) {						\
			s[sindex] = g##N;				\
			c[cindex] = -f##N;				\
		} else {						\
			s[sindex] = f##N;				\
			c[cindex] = g##N;				\
		}							\
		goto label;						\
	}

#define	PROCESS(N)							\
	if (medium) {							\
		z##N = y##N * invpio2 + c3two51;			\
		n##N = LO(z##N);					\
		z##N -= c3two51;					\
		y##N = (y##N - z##N * pio2_1) - z##N * pio2_t;		\
	}								\
	z##N = y##N * y##N;						\
	f##N = (float)(y##N + y##N * z##N * (S0 + z##N * (S1 + z##N * S2)));\
	g##N = (float)(one + z##N * (mhalf + z##N * (C0 + z##N *	\
	    (C1 + z##N * C2))));					\
	if (n##N & 2) {							\
		f##N = -f##N;						\
		g##N = -g##N;						\
	}								\
	if (n##N & 1) {							\
		*s = g##N;						\
		*c = -f##N;						\
	} else {							\
		*s = f##N;						\
		*c = g##N;						\
	}								\
	s += strides;							\
	c += stridec

void
__vsincosf(int n, float *restrict x, int stridex,
    float *restrict s, int strides, float *restrict c, int stridec)
{
	double		y0, y1, y2, y3;
	double		z0, z1, z2, z3;
	float		f0, f1, f2, f3, t;
	float		g0, g1, g2, g3;
	int		n0, n1, n2, n3, hx, ix, medium;

	s -= strides;
	c -= stridec;

	for (;;) {
begin:
		s += strides;
		c += stridec;

		if (--n < 0)
			break;

		medium = 0;
		PREPROCESS(0, 0, 0, begin);

		if (--n < 0)
			goto process1;

		PREPROCESS(1, strides, stridec, process1);

		if (--n < 0)
			goto process2;

		PREPROCESS(2, (strides << 1), (stridec << 1), process2);

		if (--n < 0)
			goto process3;

		PREPROCESS(3, (strides << 1) + strides,
		    (stridec << 1) + stridec, process3);

		if (medium) {
			z0 = y0 * invpio2 + c3two51;
			z1 = y1 * invpio2 + c3two51;
			z2 = y2 * invpio2 + c3two51;
			z3 = y3 * invpio2 + c3two51;

			n0 = LO(z0);
			n1 = LO(z1);
			n2 = LO(z2);
			n3 = LO(z3);

			z0 -= c3two51;
			z1 -= c3two51;
			z2 -= c3two51;
			z3 -= c3two51;

			y0 = (y0 - z0 * pio2_1) - z0 * pio2_t;
			y1 = (y1 - z1 * pio2_1) - z1 * pio2_t;
			y2 = (y2 - z2 * pio2_1) - z2 * pio2_t;
			y3 = (y3 - z3 * pio2_1) - z3 * pio2_t;
		}

		z0 = y0 * y0;
		z1 = y1 * y1;
		z2 = y2 * y2;
		z3 = y3 * y3;

		f0 = (float)(y0 + y0 * z0 * (S0 + z0 * (S1 + z0 * S2)));
		f1 = (float)(y1 + y1 * z1 * (S0 + z1 * (S1 + z1 * S2)));
		f2 = (float)(y2 + y2 * z2 * (S0 + z2 * (S1 + z2 * S2)));
		f3 = (float)(y3 + y3 * z3 * (S0 + z3 * (S1 + z3 * S2)));

		g0 = (float)(one + z0 * (mhalf + z0 * (C0 + z0 *
		    (C1 + z0 * C2))));
		g1 = (float)(one + z1 * (mhalf + z1 * (C0 + z1 *
		    (C1 + z1 * C2))));
		g2 = (float)(one + z2 * (mhalf + z2 * (C0 + z2 *
		    (C1 + z2 * C2))));
		g3 = (float)(one + z3 * (mhalf + z3 * (C0 + z3 *
		    (C1 + z3 * C2))));

		if (n0 & 2) {
			f0 = -f0;
			g0 = -g0;
		}
		if (n1 & 2) {
			f1 = -f1;
			g1 = -g1;
		}
		if (n2 & 2) {
			f2 = -f2;
			g2 = -g2;
		}
		if (n3 & 2) {
			f3 = -f3;
			g3 = -g3;
		}

		if (n0 & 1) {
			*s = g0;
			*c = -f0;
		} else {
			*s = f0;
			*c = g0;
		}
		s += strides;
		c += stridec;

		if (n1 & 1) {
			*s = g1;
			*c = -f1;
		} else {
			*s = f1;
			*c = g1;
		}
		s += strides;
		c += stridec;

		if (n2 & 1) {
			*s = g2;
			*c = -f2;
		} else {
			*s = f2;
			*c = g2;
		}
		s += strides;
		c += stridec;

		if (n3 & 1) {
			*s = g3;
			*c = -f3;
		} else {
			*s = f3;
			*c = g3;
		}
		continue;

process1:
		PROCESS(0);
		continue;

process2:
		PROCESS(0);
		PROCESS(1);
		continue;

process3:
		PROCESS(0);
		PROCESS(1);
		PROCESS(2);
	}
}
