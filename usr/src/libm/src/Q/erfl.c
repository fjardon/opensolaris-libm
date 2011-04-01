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

#pragma ident	"@(#)erfl.c	1.8	06/01/31 SMI"

/*
 * long double function erf,erfc (long double x)
 * K.C. Ng, September, 1989.
 *			     x
 *		      2      |\
 *     erf(x)  =  ---------  | exp(-t*t)dt
 *	 	   sqrt(pi) \|
 *			     0
 *
 *     erfc(x) =  1-erf(x)
 *
 * method:
 * 	Since erf(-x) = -erf(x), we assume x>=0.
 *	For x near 0, we have the expansion
 *
 *     	    erf(x) = (2/sqrt(pi))*(x - x^3/3 + x^5/10 - x^7/42 + ....).
 *
 * 	Since 2/sqrt(pi) = 1.128379167095512573896158903121545171688,
 *	we use x + x*P(x^2) to approximate erf(x). This formula will
 *	guarantee the error less than one ulp where x is not too far
 *	away from 0. We note that erf(x)=x at x = 0.6174...... After
 *	some experiment, we choose the following approximation on
 *	interval [0,0.84375].
 *
 *	For x in [0,0.84375]
 *		   2		    2        4		     40
 *	   P = 	P(x ) = (p0 + p1 * x + p2 * x + ... + p20 * x  )
 *
 *	   erf(x)  = x + x*P
 *	   erfc(x) = 1 - erf(x) 	  if x<=0.25
 *		   = 0.5 + ((0.5-x)-x*P)  if x in [0.25,0.84375]
 *	precision: |P(x^2)-(erf(x)-x)/x| <= 2**-122.50
 *
 *	For x in [0.84375,1.25], let s = x - 1, and
 *	c = 0.84506291151 rounded to single (24 bits)
 *	   erf(x)  = c  + P1(s)/Q1(s)
 *	   erfc(x) = (1-c)  - P1(s)/Q1(s)
 *	precision: |P1/Q1 - (erf(x)-c)| <= 2**-118.41
 *
 *
 *	For x in [1.25,1.75], let s = x - 1.5, and
 *	c = 0.95478588343 rounded to single (24 bits)
 *	   erf(x)  = c  + P2(s)/Q2(s)
 *	   erfc(x) = (1-c)  - P2(s)/Q2(s)
 *	precision: |P1/Q1 - (erf(x)-c)| <= 2**-123.83
 *
 *
 *	For x in [1.75,16/3]
 *	   erfc(x) = exp(-x*x)*(1/x)*R1(1/x)/S1(1/x)
 *	   erf(x)  = 1 - erfc(x)
 *	precision: absolute error of R1/S1 is bounded by 2**-124.03
 *
 *	For x in [16/3,107]
 *	   erfc(x) = exp(-x*x)*(1/x)*R2(1/x)/S2(1/x)
 *	   erf(x)  = 1 - erfc(x) (if x>=9 simple return erf(x)=1 with inexact)
 *	precision: absolute error of R2/S2 is bounded by 2**-120.07
 *
 *	Else if inf > x >= 107
 *	   erf(x)  = 1 with inexact
 *	   erfc(x) = 0 with underflow
 *
 *	Special case:
 *	   erf(inf)  = 1
 *	   erfc(inf) = 0
 */

#pragma weak erfl = __erfl
#pragma weak erfcl = __erfcl

#include "libm.h"
#include "longdouble.h"

static const long double
	tiny	    = 1e-40L,
	nearunfl    = 1e-4000L,
	half	    = 0.5L,
	one	    = 1.0L,
	onehalf	    = 1.5L,
	L16_3	    = 16.0L/3.0L;
/*
 * Coefficients for even polynomial P for erf(x)=x+x*P(x^2) on [0,0.84375]
 */
static const long double P[] = { 	/* 21 coeffs */
   1.283791670955125738961589031215451715556e-0001L,
  -3.761263890318375246320529677071815594603e-0001L,
   1.128379167095512573896158903121205899135e-0001L,
  -2.686617064513125175943235483344625046092e-0002L,
   5.223977625442187842111846652980454568389e-0003L,
  -8.548327023450852832546626271083862724358e-0004L,
   1.205533298178966425102164715902231976672e-0004L,
  -1.492565035840625097674944905027897838996e-0005L,
   1.646211436588924733604648849172936692024e-0006L,
  -1.636584469123491976815834704799733514987e-0007L,
   1.480719281587897445302529007144770739305e-0008L,
  -1.229055530170782843046467986464722047175e-0009L,
   9.422759064320307357553954945760654341633e-0011L,
  -6.711366846653439036162105104991433380926e-0012L,
   4.463224090341893165100275380693843116240e-0013L,
  -2.783513452582658245422635662559779162312e-0014L,
   1.634227412586960195251346878863754661546e-0015L,
  -9.060782672889577722765711455623117802795e-0017L,
   4.741341801266246873412159213893613602354e-0018L,
  -2.272417596497826188374846636534317381203e-0019L,
   8.069088733716068462496835658928566920933e-0021L,
};

/*
 * Rational erf(x) = ((float)0.84506291151) + P1(x-1)/Q1(x-1) on [0.84375,1.25]
 */
static const long double C1   = (long double)((float)0.84506291151);
static const long double P1[] = { 	/*  12 top coeffs */
  -2.362118560752659955654364917390741930316e-0003L,
   4.129623379624420034078926610650759979146e-0001L,
  -3.973857505403547283109417923182669976904e-0002L,
   4.357503184084022439763567513078036755183e-0002L,
   8.015593623388421371247676683754171456950e-0002L,
  -1.034459310403352486685467221776778474602e-0002L,
   5.671850295381046679675355719017720821383e-0003L,
   1.219262563232763998351452194968781174318e-0003L,
   5.390833481581033423020320734201065475098e-0004L,
  -1.978853912815115495053119023517805528300e-0004L,
   6.184234513953600118335017885706420552487e-0005L,
  -5.331802711697810861017518515816271808286e-0006L,
};
static const long double Q1[] = { 	/*  12 bottom coeffs with leading 1.0 hidden */
   9.081506296064882195280178373107623196655e-0001L,
   6.821049531968204097604392183650687642520e-0001L,
   4.067869178233539502315055970743271822838e-0001L,
   1.702332233546316765818144723063881095577e-0001L,
   7.498098377690553934266423088708614219356e-0002L,
   2.050154396918178697056927234366372760310e-0002L,
   7.012988534031999899054782333851905939379e-0003L,
   1.149904787014400354649843451234570731076e-0003L,
   3.185620255011299476196039491205159718620e-0004L,
   1.273405072153008775426376193374105840517e-0005L,
   4.753866999959432971956781228148402971454e-0006L,
  -1.002287602111660026053981728549540200683e-0006L,
};
/*
 * Rational erf(x) = ((float)0.95478588343) + P2(x-1.5)/Q2(x-1.5)
 * on [1.25,1.75]
 */
static const long double C2   = (long double)((float)0.95478588343);
static const long double P2[] = { 	/*  12 top coeffs */
   1.131926304864446730135126164594785863512e-0002L,
   1.273617996967754151544330055186210322832e-0001L,
  -8.169980734667512519897816907190281143423e-0002L,
   9.512267486090321197833634271787944271746e-0002L,
  -2.394251569804872160005274999735914368170e-0002L,
   1.108768660227528667525252333184520222905e-0002L,
   3.527435492933902414662043314373277494221e-0004L,
   4.946116273341953463584319006669474625971e-0004L,
  -4.289851942513144714600285769022420962418e-0005L,
   8.304719841341952705874781636002085119978e-0005L,
  -1.040460226177309338781902252282849903189e-0005L,
   2.122913331584921470381327583672044434087e-0006L,
};
static const long double Q2[] = { 	/*  13 bottom coeffs with leading 1.0 hidden */
   7.448815737306992749168727691042003832150e-0001L,
   7.161813850236008294484744312430122188043e-0001L,
   3.603134756584225766144922727405641236121e-0001L,
   1.955811609133766478080550795194535852653e-0001L,
   7.253059963716225972479693813787810711233e-0002L,
   2.752391253757421424212770221541238324978e-0002L,
   7.677654852085240257439050673446546828005e-0003L,
   2.141102244555509687346497060326630061069e-0003L,
   4.342123013830957093949563339130674364271e-0004L,
   8.664587895570043348530991997272212150316e-0005L,
   1.109201582511752087060167429397033701988e-0005L,
   1.357834375781831062713347000030984364311e-0006L,
   4.957746280594384997273090385060680016451e-0008L,
};
/*
 * erfc(x) = exp(-x*x)/x * R1(1/x)/S1(1/x) on [1.75, 16/3]
 */
static const long double R1[] = { 	/*  14 top coeffs */
   4.630195122654315016370705767621550602948e+0006L,
   1.257949521746494830700654204488675713628e+0007L,
   1.704153822720260272814743497376181625707e+0007L,
   1.502600568706061872381577539537315739943e+0007L,
   9.543710793431995284827024445387333922861e+0006L,
   4.589344808584091011652238164935949522427e+0006L,
   1.714660662941745791190907071920671844289e+0006L,
   5.034802147768798894307672256192466283867e+0005L,
   1.162286400443554670553152110447126850725e+0005L,
   2.086643834548901681362757308058660399137e+0004L,
   2.839793161868140305907004392890348777338e+0003L,
   2.786687241658423601778258694498655680778e+0002L,
   1.779177837102695602425897452623985786464e+0001L,
   5.641895835477470769043614623819144434731e-0001L,
};
static const long double S1[] = { 	/* 15 bottom coeffs with leading 1.0 hidden */
   4.630195122654331529595606896287596843110e+0006L,
   1.780411093345512024324781084220509055058e+0007L,
   3.250113097051800703707108623715776848283e+0007L,
   3.737857099176755050912193712123489115755e+0007L,
   3.029787497516578821459174055870781168593e+0007L,
   1.833850619965384765005769632103205777227e+0007L,
   8.562719999736915722210391222639186586498e+0006L,
   3.139684562074658971315545539760008136973e+0006L,
   9.106421313731384880027703627454366930945e+0005L,
   2.085108342384266508613267136003194920001e+0005L,
   3.723126272693120340730491416449539290600e+0004L,
   5.049169878567344046145695360784436929802e+0003L,
   4.944274532748010767670150730035392093899e+0002L,
   3.153510608818213929982940249162268971412e+0001L,
   1.0e00L,
};

/*
 * erfc(x) = exp(-x*x)/x * R2(1/x)/S2(1/x) on [16/3, 107]
 */
static const long double R2[] = { 	/*  15 top coeffs in reverse order!!*/
   2.447288012254302966796326587537136931669e+0005L,
   8.768592567189861896653369912716538739016e+0005L,
   1.552293152581780065761497908005779524953e+0006L,
   1.792075924835942935864231657504259926729e+0006L,
   1.504001463155897344947500222052694835875e+0006L,
   9.699485556326891411801230186016013019935e+0005L,
   4.961449933661807969863435013364796037700e+0005L,
   2.048726544693474028061176764716228273791e+0005L,
   6.891532964330949722479061090551896886635e+0004L,
   1.888014709010307507771964047905823237985e+0004L,
   4.189692064988957745054734809642495644502e+0003L,
   7.362346487427048068212968889642741734621e+0002L,
   9.980359714211411423007641056580813116207e+0001L,
   9.426910895135379181107191962193485174159e+0000L,
   5.641895835477562869480794515623601280429e-0001L,
};
static const long double S2[] = { 	/* 16 coefficients */
   2.447282203601902971246004716790604686880e+0005L,
   1.153009852759385309367759460934808489833e+0006L,
   2.608580649612639131548966265078663384849e+0006L,
   3.766673917346623308850202792390569025740e+0006L,
   3.890566255138383910789924920541335370691e+0006L,
   3.052882073900746207613166259994150527732e+0006L,
   1.885574519970380988460241047248519418407e+0006L,
   9.369722034759943185851450846811445012922e+0005L,
   3.792278350536686111444869752624492443659e+0005L,
   1.257750606950115799965366001773094058720e+0005L,
   3.410830600242369370645608634643620355058e+0004L,
   7.513984469742343134851326863175067271240e+0003L,
   1.313296320593190002554779998138695507840e+0003L,
   1.773972700887629157006326333696896516769e+0002L,
   1.670876451822586800422009013880457094162e+0001L,
   1.000L,
};

long double erfl(x)
long double x;
{
	long double s,y,t;

	if(!finitel(x)) {
	    if(x!=x) return x+x; 	/* NaN */
	    return copysignl(one,x);	/* return +-1.0 is x=Inf */
	}

	y = fabsl(x);
	if(y <= 0.84375L) {
	    if(y<=tiny) return x+P[0]*x;
	    s = y*y;
	    t = __poly_libmq(s,21,P);
	    return  x+x*t;
	}
	if(y<=1.25L) {
	    s = y-one;
	    t = C1+__poly_libmq(s,12,P1)/(one+s*__poly_libmq(s,12,Q1));
	    return (signbitl(x))? -t: t;
	} else if(y<=1.75L) {
	    s = y-onehalf;
	    t = C2+__poly_libmq(s,12,P2)/(one+s*__poly_libmq(s,13,Q2));
	    return (signbitl(x))? -t: t;
	}
	if(y<=9.0L) t = erfcl(y); else t = tiny;
	return (signbitl(x))? t-one: one-t;
}

long double erfcl(x)
long double x;
{
	long double s,y,t;

	if(!finitel(x)) {
	    if(x!=x) return x+x; 	/* NaN */
	    /* return 2.0 if x= -inf; 0.0 if x= +inf */
	    if (x < 0.0L) return 2.0L; else return 0.0L;
	}

	if(x <= 0.84375L) {
	    if(x<=0.25) return one-erfl(x);
	    s = x*x;
	    t = half-x;
	    t = t - x*__poly_libmq(s,21,P);
	    return  half+t;
	}
	if(x<=1.25L) {
	    s = x-one;
	    t = one-C1;
	    return t - __poly_libmq(s,12,P1)/(one+s*__poly_libmq(s,12,Q1));
	} else if(x<=1.75L) {
	    s = x-onehalf;
	    t = one-C2;
	    return t - __poly_libmq(s,12,P2)/(one+s*__poly_libmq(s,13,Q2));
	}
	if(x>=107.0L) return nearunfl*nearunfl;		/* underflow */
	else if(x >= L16_3) {
	    y = __poly_libmq(x,15,R2);
	    t = y/__poly_libmq(x,16,S2);
	} else {
	    y = __poly_libmq(x,14,R1);
	    t = y/__poly_libmq(x,15,S1);
	}
	/*
	 * Note that exp(-x*x+d) = exp(-x*x)*exp(d), so to compute
	 * exp(-x*x) with a small relative error, we need to compute
	 * -x*x with a small absolute error.  To this end, we set y
	 * equal to the leading part of x but with enough trailing
	 * zeros that y*y can be computed exactly and we rewrite x*x
	 * as y*y + (x-y)*(x+y), distributing the latter expression
	 * across the exponential.
	 *
	 * We could construct y in a portable way by setting
	 *
	 *   int i = (int)(x * ptwo);
	 *   y = (long double)i * 1/ptwo;
	 *
	 * where ptwo is some power of two large enough to make x-y
	 * small but not so large that the conversion to int overflows.
	 * When long double arithmetic is slow, however, the following
	 * non-portable code is preferable.
	 */
	y = x;
	*(2+(int*)&y) = *(3+(int*)&y) = 0;
	t *= expl(-y*y)*expl(-(x-y)*(x+y));
	return  t;
}
