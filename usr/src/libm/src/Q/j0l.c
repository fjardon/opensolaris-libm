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

#pragma ident	"@(#)j0l.c	1.8	06/01/31 SMI"

/*
 * Floating point Bessel's function of the first and second kinds
 * of order zero: j0(x),y0(x);
 *
 * Special cases:
 *	y0(0)=y1(0)=yn(n,0) = -inf with division by zero signal;
 *	y0(-ve)=y1(-ve)=yn(n,-ve) are NaN with invalid signal.
 */

#pragma weak j0l = __j0l
#pragma weak y0l = __y0l

#include "libm.h"

#define	GENERIC long double
static const GENERIC
zero    = 0.0L,
small	= 1.0e-9L,
tiny	= 1.0e-38L,
one	= 1.0L,
five	= 5.0L,
eight   = 8.0L,
invsqrtpi= 5.641895835477562869480794515607725858441e-0001L,
tpi	= 0.636619772367581343075535053490057448L;

static GENERIC pzero(GENERIC);
static GENERIC qzero(GENERIC);

static GENERIC r0[7] = {
  -2.499999999999999999999999999999998934492e-0001L,
   1.272657927360049786327618451133763714880e-0002L,
  -2.694499763712963276900636693400659600898e-0004L,
   2.724877475058977576903234070919616447883e-0006L,
  -1.432617103214330236967477495393076320281e-0008L,
   3.823248804080079168706683540513792224471e-0011L,
  -4.183174277567983647337568504286313665065e-0014L,
};
static GENERIC s0[7] = {
   1.0e0L,
   1.159368290559800854689526195462884666395e-0002L,
   6.629397597394973383009743876169946772559e-0005L,
   2.426779981394054406305431142501735094340e-0007L,
   6.097663491248511069094400469635449749883e-0010L,
   1.017019133340929220238747413216052224036e-0012L,
   9.012593179306197579518374581969371278481e-0016L,
};

GENERIC
j0l(x) GENERIC x;{
	GENERIC z, s,c,ss,cc,r,u,v;
	int i;

	if(isnanl(x)) return x+x;
	x = fabsl(x);
	if(x > 1.28L){
		if(!finitel(x)) return zero;
		s = sinl(x);
		c = cosl(x);
	/*
	 * j0(x) = sqrt(2/(pi*x))*(p0(x)*cos(x0)-q0(x)*sin(x0))
	 * where x0 = x-pi/4
	 * 	Better formula:
	 *		cos(x0) = cos(x)cos(pi/4)+sin(x)sin(pi/4)
	 *			= 1/sqrt(2) * (cos(x) + sin(x))
	 *		sin(x0) = sin(x)cos(pi/4)-cos(x)sin(pi/4)
	 *			= 1/sqrt(2) * (sin(x) - cos(x))
	 * To avoid cancellation, use
	 *		sin(x) +- cos(x) = -cos(2x)/(sin(x) -+ cos(x))
	 * to compute the worse one.
	 */
		if(x>1.0e2450L) {	/* x+x may overflow */
			ss = s-c;
			cc = s+c;
		} else if(signbitl(s)!=signbitl(c)) {
			ss = s - c;
			cc = -cosl(x+x)/ss;
		} else {
			cc = s + c;
			ss = -cosl(x+x)/cc;
		}
	/*
	 * j0(x) = 1/sqrt(pi) * (P(0,x)*cc - Q(0,x)*ss) / sqrt(x)
	 * y0(x) = 1/sqrt(pi) * (P(0,x)*ss + Q(0,x)*cc) / sqrt(x)
	 */
		if(x>1.0e120L) return (invsqrtpi*cc)/sqrtl(x);
		u = pzero(x); v = qzero(x);
		return invsqrtpi*(u*cc-v*ss)/sqrtl(x);
	}
	if(x<=small) {
	    if(x<=tiny) return one-x;
	    else return one-x*x*0.25L;
	}
	z = x*x;
	r = r0[6]; s = s0[6];
	for(i=5;i>=0;i--) {
	    r = r*z + r0[i];
	    s = s*z + s0[i];
	}
	return(one+z*(r/s));
}

static const GENERIC u0[8] = {
  -7.380429510868722527434392794848301631220e-0002L,
   1.766855559625940791857536949301981816513e-0001L,
  -1.386470722701047923235553251240162839408e-0002L,
   3.520149242724811578636970811631224862615e-0004L,
  -3.978599663243790049853642275624951870025e-0006L,
   2.228801153263957224547222556806915479763e-0008L,
  -6.121246764298785018658597179498837316177e-0011L,
   6.677103629722678833475965810525587396596e-0014L,
};
static const GENERIC v0[8] = {
   1.0e0L,
   1.247164416539111311571676766127767127970e-0002L,
   7.829144749639791500052900281489367443576e-0005L,
   3.247126540422245330511218321013360336606e-0007L,
   9.750516724789499678567062572549568447869e-0010L,
   2.156713223173591212250543390258458098776e-0012L,
   3.322169561597890004231482431236452752624e-0015L,
   2.821213295314000924252226486305726805093e-0018L,
};

GENERIC
y0l(x) GENERIC x;{
	GENERIC z, s,c,ss,cc,u,v;
	int i;
	volatile GENERIC d;

	if(isnanl(x)) return x+x;
	if(x <= zero){
		if(x==zero)
		    d= -one/(x-x);
		else
		    d = zero/(x-x);
	}
#ifdef lint
	d = d;
#endif
	if(x > 1.28L){
		if(!finitel(x)) return zero;
		s = sinl(x);
		c = cosl(x);
	/*
	 * j0(x) = sqrt(2/(pi*x))*(p0(x)*cos(x0)-q0(x)*sin(x0))
	 * where x0 = x-pi/4
	 * 	Better formula:
	 *		cos(x0) = cos(x)cos(pi/4)+sin(x)sin(pi/4)
	 *			= 1/sqrt(2) * (cos(x) + sin(x))
	 *		sin(x0) = sin(x)cos(pi/4)-cos(x)sin(pi/4)
	 *			= 1/sqrt(2) * (sin(x) - cos(x))
	 * To avoid cancellation, use
	 *		sin(x) +- cos(x) = -cos(2x)/(sin(x) -+ cos(x))
	 * to compute the worse one.
	 */
		if(x>1.0e2450L) {	/* x+x may overflow */
			ss = s-c;
			cc = s+c;
		} else if(signbitl(s)!=signbitl(c)) {
			ss = s - c;
			cc = -cosl(x+x)/ss;
		} else {
			cc = s + c;
			ss = -cosl(x+x)/cc;
		}
	/*
	 * j0(x) = 1/sqrt(pi*x) * (P(0,x)*cc - Q(0,x)*ss)
	 * y0(x) = 1/sqrt(pi*x) * (P(0,x)*ss + Q(0,x)*cc)
	 */
		if(x>1.0e120L) return (invsqrtpi*ss)/sqrtl(x);
		return invsqrtpi*(pzero(x)*ss+qzero(x)*cc)/sqrtl(x);

	}
	if(x<=tiny) {
	    return(u0[0] + tpi*logl(x));
	}
	z = x*x;
	u = u0[7]; v = v0[7];
	for(i=6;i>=0;i--){
	    u = u*z + u0[i];
	    v = v*z + v0[i];
	}
	return(u/v + tpi*(j0l(x)*logl(x)));
}

static const GENERIC pr0[12] = {	/* [16 -- inf] */
   9.999999999999999999999999999999999997515e-0001L,
   1.065981615377273376425365823967550598358e+0003L,
   4.390991200927588978306374718984240719130e+0005L,
   9.072086218607986711847069407339321363103e+0007L,
   1.022552886177375367408408501046461671528e+0010L,
   6.420766912243658241570635854089597269031e+0011L,
   2.206451725126933913591080211081242266908e+0013L,
   3.928369596816895077363705478743346298368e+0014L,
   3.258159928874124597286701119721482876596e+0015L,
   1.025715808134188978860679130140685101348e+0016L,
   7.537170874795721255796001687024031280685e+0015L,
  -1.579413901450157332307745586004207687796e+0014L,
};
static const GENERIC ps0[11] = {
   1.0e0L,
   1.066051927877273376425365823967550512687e+0003L,
   4.391739647168381592399173804329266353038e+0005L,
   9.075162261801343671805658294123888867884e+0007L,
   1.023186118519904751819581912075985995058e+0010L,
   6.427861860414223746340515376512730275061e+0011L,
   2.210861503237823589735481303627993406235e+0013L,
   3.943247335784292905915956840901818177989e+0014L,
   3.283720976777545142150200110647270004481e+0015L,
   1.045346918812754048903645641538728986759e+0016L,
   8.043455468065618900750599584291193680463e+0015L,
};
static const GENERIC pr1[12] = {	/* [8 -- 16] */
   9.999999999999999999999784422701108683618e-0001L,
   6.796098532948334207755488692777907062894e+0002L,
   1.840036112605722168824530758797169836042e+0005L,
   2.598490483191916637264894340635847598122e+0007L,
   2.105774863242707025525730249472054578523e+0009L,
   1.015822044230542426666314997796944979959e+0011L,
   2.931557457008110436764077699944189071875e+0012L,
   4.962885121125457633655259224179322808824e+0013L,
   4.705424055148223269155430598563351566279e+0014L,
   2.294439854910747229152056080910427001110e+0015L,
   4.905531843137486691500950019322475458629e+0015L,
   3.187543169710339218793442542845735994565e+0015L,
};
static const GENERIC ps1[14] = {
   1.0e0L,
   6.796801657948334207754571576066758180288e+0002L,
   1.840512891201300567325421059826676366447e+0005L,
   2.599777028312918975306252167127695075221e+0007L,
   2.107582572771047636846811284634244892537e+0009L,
   1.017275794694156108975782763889979940348e+0011L,
   2.938487645192463845428059755454762316011e+0012L,
   4.982512164735557054521042916182317924466e+0013L,
   4.737639900153703274792677468264564361437e+0014L,
   2.323398719123742743524249528275097100646e+0015L,
   5.033419107069210577868909797896984419391e+0015L,
   3.409036105931068609601317076759804716059e+0015L,
   7.505655364352679737585745147753521662166e+0013L,
  -9.976837153983688250780198248297109118313e+0012L,
};
static const GENERIC pr2[12] = {	/* [5 -- 8 ] */
   9.999999999999999937857236789277366320220e-0001L,
   3.692848765268649571651602420376358849214e+0002L,
   5.373022067535476576926715900057760985410e+0004L,
   4.038738891191314969971504035057219430725e+0006L,
   1.728285706306940523397385566659762646999e+0008L,
   4.375400819645889911158688737206054788534e+0009L,
   6.598950418204912408375591217782088567076e+0010L,
   5.827182039183238492480275401520072793783e+0011L,
   2.884222642913492390887572414999490975844e+0012L,
   7.373278873797767721932837830628688632775e+0012L,
   8.338295457568973761205077964397969230489e+0012L,
   2.911383183467288345772308817209806922143e+0012L,
};
static const GENERIC ps2[14] = {
   1.0e0L,
   3.693551890268649477288896267171993213102e+0002L,
   5.375607880998361502474715133828068514297e+0004L,
   4.042477764024108249744998862572786367328e+0006L,
   1.731069838737016956685839588670132939513e+0008L,
   4.387147674049898778738226585935491417728e+0009L,
   6.628058659620653765349556940567715258165e+0010L,
   5.869659904164177740471685856367322160664e+0011L,
   2.919839445622817017058977559638969436383e+0012L,
   7.535314897696671402628203718612309253907e+0012L,
   8.696355561452933775773309859748610658935e+0012L,
   3.216155103141537221173601557697083216257e+0012L,
   4.756857081068942248246880159213789086363e+0010L,
  -3.496356619666608032231074866481472824067e+0009L,
};
static const GENERIC pr3[13] = {	/* [3.5 -- 5 ] */
   9.999999999999916693107285612398196588247e-0001L,
   2.263975921282917721194425320484974336945e+0002L,
   1.994358386744245848889492762781484199966e+0004L,
   8.980067458430542243559962493831661323168e+0005L,
   2.282213787521372663705567756420087553508e+0007L,
   3.409784374889063618250288699908375135923e+0008L,
   3.024380857401448589254343517589811711108e+0009L,
   1.571110368046740246895071721443082286379e+0010L,
   4.603187020243604632153685300463160593768e+0010L,
   7.087196453409712719449549280664058793403e+0010L,
   5.046196021776346356803687409644239065041e+0010L,
   1.287758439080165765709154276618854799932e+0010L,
   5.900679773415023433787846658096813590784e+0008L,
};
static const GENERIC ps3[13] = {
   1.0e0L,
   2.264679046282855061328604619231774747116e+0002L,
   1.995939523988944553755653255389812103448e+0004L,
   8.993853144706348727038389967490183236820e+0005L,
   2.288326099634588843906989983704795468773e+0007L,
   3.424967100255240885169240956804790118282e+0008L,
   3.046311797972463991368023759640028910016e+0009L,
   1.589614961932826812790222479700797224003e+0010L,
   4.692406624527744816497089139325073939927e+0010L,
   7.320486495902008912866462849073108323948e+0010L,
   5.345945972828978289935309597742981360994e+0010L,
   1.444033091910423754121309915092247171008e+0010L,
   7.987714685115314668378957273824383610525e+0008L,
};
static const GENERIC pr4[13] = {	/* [2.5 , 3.5] */
   9.999999999986736677961118722747757712260e-0001L,
   1.453824980703800559037873123568378845663e+0002L,
   8.097327216430682288267610447006508661032e+0003L,
   2.273847252038264370231169686380192662135e+0005L,
   3.561056728046211111354759998976985449622e+0006L,
   3.244933588800096378434627029369680378599e+0007L,
   1.740112392860717950376210038908476792588e+0008L,
   5.426170187455893285197878563881579269524e+0008L,
   9.490107486454362321004377336020526281371e+0008L,
   8.688872439428470049801714121070005313806e+0008L,
   3.673315853166437222811910656900123215515e+0008L,
   5.577770470359303305164877446339693270239e+0007L,
   1.540438642031689641308197880181291865714e+0006L,
};
static const GENERIC ps4[13] = {	/* [2.5 , 3.5] */
   1.0e0L,
   1.454528105698159439773035951959131799816e+0002L,
   8.107442215200392397172179900434987859618e+0003L,
   2.279390393778242887574177096606328994140e+0005L,
   3.576251625592252008424781111770934135844e+0006L,
   3.267909499056932631405942058670933813863e+0007L,
   1.760021515330805537499778238099704648805e+0008L,
   5.525553787667353981242060222587465726729e+0008L,
   9.769870295912820457889384082671269328511e+0008L,
   9.110582071004774279226905629624018008454e+0008L,
   3.981857678621955599371967680343918454345e+0008L,
   6.482404686230769399073192961667697036706e+0007L,
   2.210046943095878402443535460329391782298e+0006L,
};
static const GENERIC pr5[13] = {	/* [1.777..., 2.5] */
   9.999999999114986107951817871144655880699e-0001L,
   9.252583736048588342568344570315435947614e+0001L,
   3.218726757856078715214631502407386264637e+0003L,
   5.554009964621111656479588505862577040831e+0004L,
   5.269993115643664338253196944523510290175e+0005L,
   2.874613773778430691192912190618220544575e+0006L,
   9.133538151103658353874146919613442436035e+0006L,
   1.673067041410338922825193013077354249193e+0007L,
   1.706913873848398011744790289200151840498e+0007L,
   9.067766583853288534551600235576747618679e+0006L,
   2.216746733457884568532695355036338655872e+0006L,
   1.945753880802872541235703812722344514405e+0005L,
   3.132374412921948071539195638885330951749e+0003L,
};
static const GENERIC ps5[13] = {	/* [1.777..., 2.5] */
   1.0e0L,
   9.259614983862181118883831670990340052982e+0001L,
   3.225125275462903384842124075132609290304e+0003L,
   5.575705362829101545292760055941855246492e+0004L,
   5.306049863037087855496170121958448492522e+0005L,
   2.907060758873509564309729903109018597215e+0006L,
   9.298059206584995898298257827131208539289e+0006L,
   1.720391071006963176836108026556547062980e+0007L,
   1.782614812922865190479394509487941920612e+0007L,
   9.708016389605273153536452032839879950155e+0006L,
   2.476495084688170096480215640962175140027e+0006L,
   2.363200660365585759668077790194604917187e+0005L,
   4.803239569848196077121203575704356936731e+0003L,
};
static const GENERIC pr6[13] = {	/* [1.28, 1.777...] */
   9.999999969777095495998606925524322559556e-0001L,
   5.825486719466194430503283824096872219216e+0001L,
   1.248155491637757281915184824965379905380e+0003L,
   1.302093199842358609321338417071710477615e+0004L,
   7.353835804186292782840961999810543016039e+0004L,
   2.356471661113686180549195092555751341757e+0005L,
   4.350553267429009581632987060942780847101e+0005L,
   4.588762661876600638719159826652389418235e+0005L,
   2.675796398548523436544221045225290128611e+0005L,
   8.077649557108971388298292919988449940464e+0004L,
   1.117640459221306873519068741664054573776e+0004L,
   5.544400072396814695175787511557757885585e+0002L,
   5.072550541191480498431289089905822910718e+0000L,
};
static const GENERIC ps6[13] = {	/* [1.28, 1.777...] */
   1.0e0L,
   5.832517925357165050639075848183613063291e+0001L,
   1.252144364743592128171256104364976466898e+0003L,
   1.310300234342216813579118022415585740772e+0004L,
   7.434667697093812197817292154032863632923e+0004L,
   2.398706595587719165726469002404004614711e+0005L,
   4.472737517625103157004869372427480602511e+0005L,
   4.786313523337761975294171429067037723611e+0005L,
   2.851161872872731228472536061865365370192e+0005L,
   8.891648269899148412331918021801385815586e+0004L,
   1.297097489535351517572978123584751042287e+0004L,
   7.096761640545975756202184143400469812618e+0002L,
   8.378049338590233325977702401733340820351e+0000L,
};
static const GENERIC sixteen = 16.0L;
static const GENERIC huge    = 1.0e30L;

static GENERIC pzero(x)
GENERIC x;
{
	GENERIC s,r,t,z;
	int i;
	if(x>huge) return one;
	t = one/x; z = t*t;
	if(x>sixteen) {
	    r = z*pr0[11]+pr0[10]; s = ps0[10];
	    for(i=9;i>=0;i--) {
		r = z*r + pr0[i];
		s = z*s + ps0[i];
	    }
	} else if (x > eight){
	    r = pr1[11]; s = ps1[11]+z*(ps1[12]+z*ps1[13]);
	    for(i=10;i>=0;i--) {
		r = z*r + pr1[i];
		s = z*s + ps1[i];
	    }
	} else if (x > five){ 	/* x > 5.0 */
	    r = pr2[11]; s = ps2[11]+z*(ps2[12]+z*ps2[13]);
	    for(i=10;i>=0;i--) {
		r = z*r + pr2[i];
		s = z*s + ps2[i];
	    }
	} else if( x>3.5L) {
	    r = pr3[12]; s = ps3[12];
	    for(i=11;i>=0;i--) {
		r = z*r + pr3[i];
		s = z*s + ps3[i];
	    }
	} else if( x>2.5L) {
	    r = pr4[12]; s = ps4[12];
	    for(i=11;i>=0;i--) {
		r = z*r + pr4[i];
		s = z*s + ps4[i];
	    }
	} else if( x> (1.0L/0.5625L)){
	    r = pr5[12]; s = ps5[12];
	    for(i=11;i>=0;i--) {
		r = z*r + pr5[i];
		s = z*s + ps5[i];
	    }
	} else {	/* assume x > 1.28 */
	    r = pr6[12]; s = ps6[12];
	    for(i=11;i>=0;i--) {
		r = z*r + pr6[i];
		s = z*s + ps6[i];
	    }
	}
	return r/s;
}


static const GENERIC qr0[12] = {	/* [16, inf] */
  -1.249999999999999999999999999999999972972e-0001L,
  -1.425179595545670577414395762503991596897e+0002L,
  -6.312499645625970845534460257936222407219e+0004L,
  -1.411374326457208384315121243698814446848e+0007L,
  -1.735034212758873581410984757860787252842e+0009L,
  -1.199777647512789489421826342485055280680e+0011L,
  -4.596025334081655714499860409699100373644e+0012L,
  -9.262525628201284107792924477031653399187e+0013L,
  -8.858394728685039245344398842180662867639e+0014L,
  -3.267527953687534887623740622709505972113e+0015L,
  -2.664222971186311967587129347029450062019e+0015L,
   3.442464060723987869585180095344504100204e+0014L,
};
static const GENERIC qs0[11] = {
   1.0e0L,
   1.140729613936536461931516610003185687881e+0003L,
   5.056665510442299351009198186490085803580e+0005L,
   1.132041763825642787943941650522718199115e+0008L,
   1.394570111872581606392620678214246479767e+0010L,
   9.677945218152264789534431079563744378421e+0011L,
   3.731140327851536828225143058896348502096e+0013L,
   7.612785951064869291722846681020881676410e+0014L,
   7.476077016406764891730191004811863975940e+0015L,
   2.951246482613592035421503427100393831709e+0016L,
   3.108361803691811711136854587074302034901e+0016L,
};
static const GENERIC qr1[12] = {	/* [8, 16 ] */
  -1.249999999999999999997949010383433818157e-0001L,
  -9.051215166393822640636752244895124126934e+0001L,
  -2.620782703428148837671179031904208303947e+0004L,
  -3.975571261553504457766177974508785790884e+0006L,
  -3.479029330759311306270072218074074994090e+0008L,
  -1.823955008124268573036216746186239829089e+0010L,
  -5.765932697111801375765156029221568664435e+0011L,
  -1.079843680798742592954002192417934779114e+0013L,
  -1.146893630504592739082205764611581332897e+0014L,
  -6.367016059683898464936104447282880704182e+0014L,
  -1.583109041961213490464459111903484209098e+0015L,
  -1.230149555764242473103128650135795639412e+0015L,
};
static const GENERIC qs1[14] = {
   1.0e0L,
   7.246831508115058112438579847778014458432e+0002L,
   2.100854184439168518399383786306927037611e+0005L,
   3.192636418837951507430188285940994235122e+0007L,
   2.801558443383354674538443461124434216152e+0009L,
   1.475026997664373739293483927250653467487e+0011L,
   4.694486824913954608552363821799927145318e+0012L,
   8.890350100919200250838438709601547334021e+0013L,
   9.626844429082905144874701068760469752067e+0014L,
   5.541110744600460773528263862687521642140e+0015L,
   1.486500494789452556727470329232123096563e+0016L,
   1.415840104845959400365430773732093899210e+0016L,
   1.780866095241517418081312567239682336483e+0015L,
  -2.359230917384889357887631544079990129494e+0014L,
};
static const GENERIC qr2[12] = {	/* [5, 8] */
  -1.249999999999999531937744362527772181614e-0001L,
  -4.944373897356969774839375977239241573966e+0001L,
  -7.728449175433465285314261650078450473909e+0003L,
  -6.262574329612752346336901434651220705903e+0005L,
  -2.900948220220943306027235217424380672732e+0007L,
  -7.988719647634192770463917157562874119535e+0008L,
  -1.318228171927181389547760026626357012375e+0010L,
  -1.282439773983029245309263271945424928196e+0011L,
  -7.050925570827818040186149940257918845138e+0011L,
  -2.021751882573871990004205616874202684429e+0012L,
  -2.592939962400668552384333900573812635658e+0012L,
  -1.038267109518891262840601514932972850326e+0012L,
};
static const GENERIC qs2[14] = {
   1.0e0L,
   3.961358492885570003202784022894248952116e+0002L,
   6.205788738864701882828752634586510926968e+0004L,
   5.045715603932670286550673813011764406749e+0006L,
   2.349248611362658323353343389430968751429e+0008L,
   6.520244524415828635917683553721880063911e+0009L,
   1.089111211223507719337067159886281887722e+0011L,
   1.080406000905359867958779409414903018610e+0012L,
   6.135645280895514703514154680623769562148e+0012L,
   1.862433040246625874245867151368643668215e+0013L,
   2.667780805786648888840777888702193708994e+0013L,
   1.394401107289087774765300711809313112824e+0013L,
   1.093247500616320375562898297156722445484e+0012L,
  -7.228875530378928722826604216491493780775e+0010L,
};
static const GENERIC qr3[13] = {	/* [3.5 5] */
  -1.249999999999473067748420379578481661075e-0001L,
  -3.044549048635289351913574324803250977998e+0001L,
  -2.890081140649769078496693003524681440869e+0003L,
  -1.404922456817202235879343275330529107684e+0005L,
  -3.862746614385573443518177403617349281869e+0006L,
  -6.257517309110249049201133708911155047689e+0007L,
  -6.031451330920839916987079782727323477520e+0008L,
  -3.411542405173830611454025765755854382346e+0009L,
  -1.089392478149726672133014498723021526099e+0010L,
  -1.824934078420210941290140903415956782726e+0010L,
  -1.400780278304358710423481070486939531139e+0010L,
  -3.716484136064917363926635716743771092093e+0009L,
  -1.397591075296425529970434890954904331580e+0008L,
};
static const GENERIC qs3[13] = {
   1.0e0L,
   2.441498613904962049391000187014945858042e+0002L,
   2.326188882072370711500164222341514337043e+0004L,
   1.137138213121231338494977104659239578165e+0006L,
   3.152918070735662728722998452605364253517e+0007L,
   5.172877993426507259314270488444013595108e+0008L,
   5.083086439731669807455961078856470774115e+0009L,
   2.961842732066434123119325521139476909941e+0010L,
   9.912185866862440735829781856081353151390e+0010L,
   1.793560561251622234430564181567297983598e+0011L,
   1.577090119341228122525265108497940403073e+0011L,
   5.509910306780166194333889999985463681636e+0010L,
   4.761691134078874491202320181517936758141e+0009L,
};
static const GENERIC qr4[13] = {	/* [2.5 3.5] */
  -1.249999999928567734339745043490705340835e-0001L,
  -1.967201748731419063051601624435565528481e+0001L,
  -1.186329146714562236407099740615528170707e+0003L,
  -3.607736959222941810356301491152457934060e+0004L,
  -6.119200717978104904932828468575194267125e+0005L,
  -6.037847781158358226670305078652205586384e+0006L,
  -3.503558153336140359700536720393565984740e+0007L,
  -1.180196478268225718757218523746787309773e+0008L,
  -2.221860232085134915841426363505169680528e+0008L,
  -2.173372505452747585296176761701746236760e+0008L,
  -9.649364865061237558517730539506568013963e+0007L,
  -1.465429227847933034546039640094862650385e+0007L,
  -3.083003197920262085170581866246663380607e+0005L,
};
static const GENERIC qs4[13] = {	/* [2.5 3.5] */
   1.0e0L,
   1.579620773732259142752614142139986854055e+0002L,
   9.581372220329138733203879503753685054968e+0003L,
   2.939598672379108095776114131010825885308e+0005L,
   5.052183049314542218630341818692588448168e+0006L,
   5.083497695595206639433839326338971980149e+0007L,
   3.036385361800553388049719014005099206516e+0008L,
   1.067826481452753409910563785161661492137e+0009L,
   2.145644125557118044720741775125319669272e+0009L,
   2.324115615959719949363946673491552216799e+0009L,
   1.223262962112070757966959855619847011146e+0009L,
   2.569765553318495423738478585947110270709e+0008L,
   1.354744744299227127897905787732636565504e+0007L,
};
static const GENERIC qr5[13] = {	/* [1.777.., 2.5] */
  -1.249999995936639697637680428174576069971e-0001L,
  -1.260846055371311453485891923426489068315e+0001L,
  -4.772398467544467480801174330290141578895e+0002L,
  -8.939852599990298486613760833996490599724e+0003L,
  -9.184070787149542050979542226446134243197e+0004L,
  -5.406038945018274458362637897739280435171e+0005L,
  -1.845896544705190261018653728678171084418e+0006L,
  -3.613616990680809501878667570653308071547e+0006L,
  -3.908782978135693252252557720414348623779e+0006L,
  -2.173711022517323927109138170588442768176e+0006L,
  -5.431253130679918485836408549007856244495e+0005L,
  -4.591098546452684510082591587275940765959e+0004L,
  -5.244711364168207806835520057792229646578e+0002L,
};
static const GENERIC qs5[13] = {	/* [1.777.., 2.5] */
   1.0e0L,
   1.014536210851290878350892750972474861447e+0002L,
   3.875547510687135314064434160096139681076e+0003L,
   7.361913122670079814955259281995617732580e+0004L,
   7.720288944218771126581086539585529314636e+0005L,
   4.681529554446752496404431433608306558038e+0006L,
   1.667882621940503925455031252308367745820e+0007L,
   3.469403153761399881888272620855305156241e+0007L,
   4.096992047964210711867089384719947863019e+0007L,
   2.596804755829217449311530735959560630554e+0007L,
   7.983933774697889238154465064019410763845e+0006L,
   9.818133816979900819087242425280757938152e+0005L,
   3.061083930868694396013541535670745443560e+0004L,
};

static const GENERIC qr6[13] = {	/* [1.28, 1.777..] */
  -1.249999881577289001807137282824929082771e-0001L,
  -7.998273510053110759610810594119533619282e+0000L,
  -1.872481955335172543369089617771565632719e+0002L,
  -2.122116786726300805079874003303799646812e+0003L,
  -1.293850285839529282503178263484773478457e+0004L,
  -4.445024742266316181033354192262529356093e+0004L,
  -8.730161378334357767668344467356505347070e+0004L,
  -9.706222895172078442801444972505315054736e+0004L,
  -5.896325518259858270165531513618195321041e+0004L,
  -1.823172034368108822276420827074668832233e+0004L,
  -2.509304178635055926638833040337472387175e+0003L,
  -1.156608965715779237316769828941729964099e+0002L,
  -7.028005789650731396887346826397785210442e-0001L,
};
static const GENERIC qs6[13] = {	/* [1.28, 1.777..] */
   1.0e0L,
   6.457211085058064845601261321277721075900e+0001L,
   1.534005216588011210342824555136008682950e+0003L,
   1.777217999176441782593357660462379097171e+0004L,
   1.118372652642469468091084810263231199696e+0005L,
   4.015242433858461813142365748386473605294e+0005L,
   8.377081045517098645448616514388280497673e+0005L,
   1.011495020008010352575398009604164287337e+0006L,
   6.886722075290430568652227875200208955970e+0005L,
   2.504735189948021472047157148613171956537e+0005L,
   4.408138920171044846941001844352009817062e+0004L,
   3.105572178072115145673058722853640854884e+0003L,
   5.588294821118916113437396504573817033678e+0001L,
};
static GENERIC qzero(x)
GENERIC x;
{
	GENERIC s,r,t,z;
	int i;
	if(x>huge) return -0.125L/x;
	t = one/x; z = t*t;
	if(x>sixteen) {
	    r = z*qr0[11]+qr0[10]; s = qs0[10];
	    for(i=9;i>=0;i--) {
		r = z*r + qr0[i];
		s = z*s + qs0[i];
	    }
	} else if(x>eight) {
	    r = qr1[11]; s = qs1[11]+z*(qs1[12]+z*qs1[13]);
	    for(i=10;i>=0;i--) {
		r = z*r + qr1[i];
		s = z*s + qs1[i];
	    }
	} else if(x>five){  /* assume x > 5.0 */
	    r = qr2[11]; s = qs2[11]+z*(qs2[12]+z*qs2[13]);
	    for(i=10;i>=0;i--) {
		r = z*r + qr2[i];
		s = z*s + qs2[i];
	    }
	} else if(x>3.5L) {
	    r = qr3[12]; s = qs3[12];
	    for(i=11;i>=0;i--) {
		r = z*r + qr3[i];
		s = z*s + qs3[i];
	    }
	} else if(x>2.5L) {
	    r = qr4[12]; s = qs4[12];
	    for(i=11;i>=0;i--) {
		r = z*r + qr4[i];
		s = z*s + qs4[i];
	    }
	} else if(x> (1.0L/0.5625L)) {
	    r = qr5[12]; s = qs5[12];
	    for(i=11;i>=0;i--) {
		r = z*r + qr5[i];
		s = z*s + qs5[i];
	    }
	} else {	/* assume x > 1.28 */
	    r = qr6[12]; s = qs6[12];
	    for(i=11;i>=0;i--) {
		r = z*r + qr6[i];
		s = z*s + qs6[i];
	    }
	}
	return t*(r/s);
}
