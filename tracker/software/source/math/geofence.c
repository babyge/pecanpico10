/**
  * Geofencing algorithm and geofencing data
  */

#include "ch.h"
#include "hal.h"
#include "geofence.h"
#include "collector.h"
#include "config.h"

static const coord_t america[] = {
	// Latitude  Longitude (in deg*10000000)
	{ 602803500,-1800000000},
	{-250833370,-1800000000},
	{-350000000,-1725193600},
	{-444551600,-1725356300},
	{-510311600,-1800000000},
	{-624631500,-1800000000},
	{-622917200, -805076100},
	{-593631200, -532159100},
	{-613268000, -216598500},
	{-523586500, -216531900},
	{-158237300, -216398800},
	{ -26137000, -256562300},
	{ 116521800, -379076800},
	{ 250055500, -471176200},
	{ 437701700, -474320100},
	{ 529477400, -475106100},
	{ 592874700, -556751500},
	{ 649970500, -579417200},
	{ 677211100, -595630600},
	{ 703992100, -625906400},
	{ 748557600, -742708000},
	{ 900000000, -749253300},
	{ 900000000,-1800000000},
	{ 751492400,-1800000000},
	{ 684708700,-1696228400},
	{ 655163500,-1697060800},
	{ 641285100,-1733928400}
};
static const coord_t china[] = {
	// Latitude  Longitude (in deg*10000000)
	{451738100, 825642300},
	{452352200, 819625200},
	{453544900, 817013800},
	{451756000, 809593900},
	{449717800, 799587900},
	{446579400, 803864200},
	{440643900, 804054400},
	{434967200, 806881200},
	{431645300, 806192500},
	{428305300, 803306500},
	{421086600, 801075700},
	{410720900, 778157100},
	{410197600, 769014100},
	{405847000, 766682700},
	{403815200, 762593500},
	{404379000, 749752300},
	{395074200, 736814000},
	{371897300, 748730300},
	{315530900, 791019900},
	{301780600, 820775400},
	{283590400, 859320000},
	{279363900, 892463900},
	{284084300, 897228300},
	{279579500, 916425000},
	{286829100, 946908200},
	{278888600, 977112800},
	{240215100, 974360200},
	{212269300,1010279300},
	{226236400,1052672100},
	{207971800,1073531600},
	{174983800,1071305300},
	{161966200,1121345000},
	{203309900,1169129400},
	{209227300,1244251400},
	{236120900,1277185900},
	{261685400,1256067700},
	{326694600,1240792500},
	{349869300,1233594400},
	{373804200,1239579800},
	{385913100,1232100700},
	{390679600,1240433600},
	{397587600,1312690000},
	{423981400,1305152200},
	{427504600,1304090500},
	{428757700,1310060100},
	{431897400,1311892000},
	{440344300,1312479400},
	{447908400,1309259800},
	{449651300,1313950300},
	{452936700,1317762000},
	{450233100,1329138800},
	{462123900,1336656800},
	{473018100,1341318300},
	{477423200,1346395600},
	{484421800,1344441700},
	{482578200,1337457700},
	{481039700,1329297000},
	{477062600,1324401400},
	{476285700,1316887800},
	{477875100,1309374300},
	{488034100,1304674300},
	{495930500,1287364200},
	{495253500,1279267800},
	{498558300,1274247600},
	{504821100,1272556800},
	{520086700,1263022800},
	{529189300,1257138500},
	{535240400,1233065100},
	{533144800,1208991700},
	{527595400,1199859800},
	{520423200,1205267200},
	{510068000,1196984600},
	{500606200,1193096500},
	{495287200,1176531300},
	{498308400,1166490900},
	{479328400,1157076200},
	{480844300,1180933300},
	{472134400,1196377400},
	{467122600,1196628800},
	{463993300,1169068900},
	{455875700,1158663400},
	{453936600,1146868800},
	{448262800,1138589800},
	{450878400,1119834600},
	{445395200,1115141900},
	{436638100,1117473200},
	{426459800,1096952900},
	{422650000,1069401300},
	{416851000,1049759900},
	{420205800,1025418600},
	{426135800,1010745200},
	{424921600, 990358900},
	{428878200, 963380800},
	{442724800, 951612100},
	{450616000, 931665300},
	{450499500, 918176200},
	{452861800, 909960600},
	{454215300, 906749700},
	{458940100, 907493900},
	{465557200, 910300600},
	{474407800, 904927700},
	{479146500, 898286200},
	{481795100, 888129100},
	{485449300, 880388900},
	{491957400, 877043300},
	{491705700, 873129800},
	{491552500, 870409600},
	{490895800, 868238800},
	{489435600, 867192900},
	{488602800, 868287300},
	{485646500, 866003400},
	{485116800, 864543400},
	{484311000, 862090400},
	{484087600, 858099400},
	{481666000, 855665300},
	{475765800, 856453300},
	{472641400, 857286700},
	{470697000, 855483500},
	{470438500, 852250700},
	{468830000, 849457300},
	{469921200, 848366900},
	{470261800, 845298900},
	{469969200, 839931900},
	{470275800, 837201600},
	{472396400, 830235700},
	{462430400, 825576900},
	{459932800, 824555100},
	{459642900, 823423500},
	{456150100, 822588400},
	{455358200, 822665200},
	{454102700, 825928100}
};
static const coord_t southkorea[] = {
	// Latitude  Longitude (in deg*10000000)
	{326694600, 1240792500},
	{349869300, 1233594400},
	{373804200, 1239579800},
	{385913100, 1232100700},
	{390679600, 1240433600},
	{397587600, 1312690000},
	{375193480, 1318293030},
	{359878230, 1310822320},
	{344441030, 1293903380},
	{325570380, 1265338930}
};
static const coord_t japan[] = {
	// Latitude  Longitude (in deg*10000000)
	{236120900, 1277185900},
	{261685400, 1256067700},
	{326694600, 1240792500},
	{325570380, 1265338930},
	{344441030, 1293903380},
	{359878230, 1310822320},
	{375193480, 1318293030},
	{397587600, 1312690000},
	{410553650, 1354328180},
	{431098400, 1372785210},
	{482766430, 1410578180},
	{480274380, 1457599670},
	{448956070, 1531153130},
	{314577590, 1453589660},
	{259887040, 1356909970}
};
static const coord_t southeastAsia[] = {
	// Latitude  Longitude (in deg*10000000)
	{ 279579500,  916425000},
	{ 286829100,  946908200},
	{ 278888600,  977112800},
	{ 240215100,  974360200},
	{ 212269300, 1010279300},
	{ 226236400, 1052672100},
	{ 207971800, 1073531600},
	{ 174983800, 1071305300},
	{ 161966200, 1121345000},
	{ 203309900, 1169129400},
	{ 209227300, 1244251400},
	{ 209227300, 1300000000},
	{ 209227300, 1400000000},
	{ 209227300, 1500000000},
	{ 209227300, 1600000000},
	{ 209227300, 1700000000},
	{ 209227300, 1800000000},
	{-250833370, 1800000000},
	{-250833370, 1730000000},
	{-250833370, 1675354920},
	{-141767640, 1558460380},
	{-123697580, 1466175230},
	{-100204890, 1448816830},
	{ -99988510, 1400476990},
	{ -94574350, 1348621520},
	{ -94357600, 1306434020},
	{ -110142370, 1266663510},
	{ -124555940, 1234803160},
	{ -142726100, 1179432060},
	{ -149730480, 1084949640},
	{ -127772250, 1016834410},
	{ -48805060,  911365660},
	{  53013140,  884119560},
	{ 145541020,  886756280},
	{ 217668310,  903455880},
	{ 279579500,  916425000}
};
static const coord_t australia[] = {
	// Latitude  Longitude (in deg*10000000)
	{-250833370, 1675354920},
	{-141767640, 1558460380},
	{-123697580, 1466175230},
	{-100204890, 1448816830},
	{ -99988510, 1400476990},
	{ -94574350, 1348621520},
	{ -94357600, 1306434020},
	{-110142370, 1266663510},
	{-124555940, 1234803160},
	{-142726100, 1179432060},
	{-149730480, 1084949640},
	{-171651090, 1060999440},
	{-208656160, 1061658620},
	{-273587200, 1062537530},
	{-331988240, 1066932060},
	{-377069690, 1085389090},
	{-405694260, 1138123470},
	{-421525130, 1215467220},
	{-436969730, 1297205500},
	{-451406920, 1355213310},
	{-464882790, 1417615660},
	{-472692610, 1478260190},
	{-466695150, 1545057060},
	{-447050730, 1570545340},
	{-414317130, 1594275810},
	{-362324240, 1614490660},
	{-323861110, 1632947690},
	{-285233680, 1654041440}
};
static const coord_t newzealand[] = {
	// Latitude  Longitude (in deg*10000000)
	{-466695150, 1545057060},
	{-447050730, 1570545340},
	{-414317130, 1594275810},
	{-362324240, 1614490660},
	{-323861110, 1632947690},
	{-285233680, 1654041440},
	{-250833370, 1675354920},
	{-250833370, 1675354920},
	{-250833370, 1730000000},
	{-250833370, 1800000000},
	{-510311600, 1800000000},
	{-553577240, 1730000000},
	{-553577240, 1663161210},
	{-545772000, 1615700270},
	{-511849000, 1576149490},
	{-489283240, 1557692460}
};
static const coord_t newzealand2[] = {
	// Latitude  Longitude (in deg*10000000)
	{-250833370,-1800000000},
	{-350000000,-1725193600},
	{-444551600,-1725356300},
	{-510311600,-1800000000}
};
static const coord_t argentina[] = { // Also includes Uruguay and Paraguay
	// Latitude  Longitude (in deg*10000000)
	{-338230310, -532566330},
	{-335121510, -536081950},
	{-327947270, -530808520},
	{-320714680, -537839770},
	{-314549730, -547947190},
	{-308155250, -559812420},
	{-301717910, -568381760},
	{-299054910, -572776290},
	{-286212420, -560911060},
	{-278469150, -551682540},
	{-272234400, -539377850},
	{-263013630, -536741130},
	{-257683040, -538718670},
	{-256098970, -542234300},
	{-255702620, -545969650},
	{-245751710, -543113200},
	{-239541980, -543772380},
	{-238537580, -547727460},
	{-239742760, -551023360},
	{-239140310, -553879810},
	{-233705670, -555198170},
	{-224192240, -557834880},
	{-221956130, -563108320},
	{-222362960, -571018480},
	{-220938540, -579368090},
	{-210312570, -578708910},
	{-202086660, -580686450},
	{-198163940, -582224530},
	{-193609750, -591233320},
	{-193195090, -599802660},
	{-196095450, -617161060},
	{-205176580, -622873950},
	{-210517650, -622434490},
	{-222769680, -626389570},
	{-219920220, -628367110},
	{-220123940, -639353440},
	{-228856080, -643088790},
	{-222769680, -645725510},
	{-220734940, -657151290},
	{-218289380, -661765550},
	{-225004480, -668796800},
	{-230070120, -670554610},
	{-240144240, -673630780},
	{-244152110, -682639570},
	{-249542570, -683957930},
	{-261633950, -683518480},
	{-269887290, -682859300},
	{-272820400, -688352460},
	{-281379450, -693406170},
	{-290639190, -697580980},
	{-294179270, -699228930},
	{-298102100, -699119060},
	{-302857000, -698459880},
	{-303994770, -700657150},
	{-311357880, -703953050},
	{-317170180, -704612230},
	{-324616360, -701316330},
	{-331265750, -698899340},
	{-340600040, -697800700},
	{-346766490, -701536060},
	{-352887370, -703953050},
	{-360029570, -703513590},
	{-364637660, -708127850},
	{-369569760, -711643480},
	{-376212530, -711863200},
	{-381933540, -710105390},
	{-387267210, -708567310},
	{-389321250, -714060470},
	{-394260690, -714060470},
	{-399670750, -716916920},
	{-406706970, -718235270},
	{-414164240, -718015550},
	{-421048000, -717795820},
	{-421536870, -721091720},
	{-425434240, -720432540},
	{-429146530, -720432540},
	{-432196440, -717356370},
	{-434753010, -719114180},
	{-437140020, -715818280},
	{-443144700, -718015550},
	{-444401100, -712302660},
	{-447218130, -712961840},
	{-447530290, -719993090},
	{-449088570, -720432540},
	{-450332160, -714939380},
	{-452811250, -712742110},
	{-454971600, -716916920},
	{-458349760, -717136640},
	{-461403130, -718015550},
	{-466404380, -716477460},
	{-468962900, -719993090},
	{-472405090, -719333910},
	{-475231880, -723728440},
	{-479222830, -724607340},
	{-483913290, -721970630},
	{-484933620, -725486250},
	{-487982300, -725486250},
	{-490436790, -730320240},
	{-496733650, -730539960},
	{-503090380, -732737230},
	{-507837610, -731638590},
	{-506167570, -726584880},
	{-506864140, -723069260},
	{-511159850, -723069260},
	{-515005840, -723288990},
	{-517597280, -721970630},
	{-519361840, -718455000},
	{-519903400, -709665940},
	{-519903400, -700876880},
	{-521524130, -695163990},
	{-522870250, -686374920},
	{-526351090, -686814380},
	{-538058860, -686814380},
	{-549132950, -686155200},
	{-548809170, -681609600},
	{-548967140, -676061510},
	{-550260190, -667382310},
	{-553552720, -660625720},
	{-558486880, -643596910},
	{-554893240, -619646710},
	{-541989660, -615032450},
	{-534205450, -633489490},
	{-517928660, -655901600},
	{-498640640, -651726790},
	{-472171950, -625359600},
	{-431465440, -609099840},
	{-403438810, -562957260},
	{-369119700, -522527570},
	{-352248180, -506267810},
	{-345036770, -520330310}
};
static const coord_t brazil[] = {
	// Latitude  Longitude (in deg*10000000)
	{-345036770, -520330310},
	{-352248180, -506267810},
	{-338230310, -532566330},
	{-335121510, -536081950},
	{-327947270, -530808520},
	{-320714680, -537839770},
	{-314549730, -547947190},
	{-308155250, -559812420},
	{-301717910, -568381760},
	{-299054910, -572776290},
	{-286212420, -560911060},
	{-278469150, -551682540},
	{-272234400, -539377850},
	{-263013630, -536741130},
	{-257683040, -538718670},
	{-256098970, -542234300},
	{-255702620, -545969650},
	{-245751710, -543113200},
	{-239541980, -543772380},
	{-238537580, -547727460},
	{-239742760, -551023360},
	{-239140310, -553879810},
	{-233705670, -555198170},
	{-224192240, -557834880},
	{-221956130, -563108320},
	{-222362960, -571018480},
	{-220938540, -579368090},
	{-210312570, -578708910},
	{-202086660, -580686450},
	{-198163940, -582224530},
	{-182169010, -576017230},
	{-175686980, -578214500},
	{-170022330, -585026020},
	{-163708150, -583707660},
	{-162653770, -601505510},
	{-147834540, -602604150},
	{-139320080, -604361960},
	{-135691790, -610514300},
	{-135264570, -617545550},
	{-131630080, -621720360},
	{-129703740, -628751610},
	{-126489880, -631827780},
	{-124559610, -638859030},
	{-120694780, -649625630},
	{-115102430, -653141250},
	{-106477260, -653360980},
	{ -98260640, -653580710},
	{ -97394510, -658854150},
	{ -99775810, -666324850},
	{-103236400, -672916650},
	{-107340910, -677750630},
	{-110577380, -685221330},
	{-109930370, -692032860},
	{-109714670, -705436180},
	{ -94794790, -705436180},
	{ -99559400, -712906880},
	{ -99775810, -721036760},
	{ -95011510, -724112930},
	{ -94144550, -731583640},
	{ -90457570, -729386370},
	{ -82202640, -736197900},
	{ -75237690, -739054340},
	{ -70224660, -737516250},
	{ -65424520, -731583640},
	{ -60619760, -732462540},
	{ -57122690, -730265280},
	{ -51654270, -728507470},
	{ -48589870, -723673480},
	{ -44866940, -717740860},
	{ -43333410, -709610980},
	{ -42018700, -705655900},
	{ -43552500, -700162740},
	{ -32371210, -697306290},
	{ -21836290, -695548480},
	{  -9975840, -694449850},
	{  -5142290, -697086570},
	{ -02725350, -700602190},
	{   5404440, -700382470},
	{   6942430, -695548480},
	{   6283300, -692032860},
	{  10237960, -691593400},
	{  11116710, -698185200},
	{  17486760, -698624650},
	{  17047510, -689615860},
	{  16827880, -681705710},
	{  19902470, -681485980},
	{  20780820, -676212540},
	{  22317820, -673795550},
	{  17267140, -671818010},
	{  11556080, -670939110},
	{  11995440, -668741840},
	{   7381850, -663688130},
	{   9139480, -657096330},
	{   9578880, -650943990},
	{  14411790, -645011370},
	{  19024070, -640836570},
	{  21439550, -634025040},
	{  24293720, -633805320},
	{  25391320, -640397110},
	{  30439020, -641715470},
	{  35703650, -641935200},
	{  42280160, -648307270},
	{  40746110, -641715470},
	{  38773330, -639078750},
	{  40088570, -633805320},
	{  35922950, -630069970},
	{  36580810, -627652970},
	{  40307750, -627872700},
	{  41184440, -621280900},
	{  43375720, -615348290},
	{  46661450, -608317040},
	{  49288930, -606119770},
	{  51958300, -606892750},
	{  52395930, -600630550},
	{  45828370, -601399590},
	{  43856890, -597554370},
	{  39364380, -595796560},
	{  35746680, -598213550},
	{  27053170, -599751640},
	{  17612210, -596016290},
	{  12889780, -589314630},
	{  13109450, -585139820},
	{  15635510, -580855160},
	{  18490680, -574153500},
	{  19369110, -566572930},
	{  18600490, -559871270},
	{  20686660, -559431820},
	{  22882340, -561519220},
	{  25406950, -559871270},
	{  24089820, -557124690},
	{  24419120, -553938650},
	{  26504460, -549983570},
	{  22882340, -545808770},
	{  21125820, -541743830},
	{  23321440, -537898610},
	{  22113890, -533613940},
	{  22004110, -529329280},
	{  25187440, -525813650},
	{  34951690, -520430350},
	{  43719960, -515486500},
	{  80004680, -481758480},
	{ 111065640, -443635920},
	{  85955290, -393977710},
	{  65918790, -362337090},
	{  37473500, -312239430},
	{    586180, -281038260},
	{ -46384550, -267854670},
	{ -86099650, -266096850},
	{-121109190, -270491390},
	{-157781340, -285432790},
	{-195459640, -303010920},
	{-229853450, -315755060},
	{-261422670, -327620290},
	{-295224630, -345637870},
	{-322003860, -361018730},
	{-349459690, -377717950},
	{-374980860, -390901540},
	{-364801770, -452864430}
};

// http://stackoverflowcom/questions/924171/geo-fencing-point-inside-outside-polygon
/**
  * Determines is location is located in polygon
  * @param poly Polygon
  * @param lat Latitude
  * @param lat Longitude
  */
static bool isPointInPolygon(const coord_t *poly, uint32_t size, int32_t lat, int32_t lon) {
	bool c = false;
	uint32_t j = size-1;

	for(uint32_t i=0; i<size; i++) {
		if((((poly[i].lat <= lat) && (lat < poly[j].lat)) || ((poly[j].lat <= lat) && (lat < poly[i].lat))) && (lon < (poly[j].lon - poly[i].lon) * (lat - poly[i].lat) / (poly[j].lat - poly[i].lat) + poly[i].lon))
			c = !c;
		j = i;
	}

	return c;
}

/**
  * Determines location of GPS point.
  * @param lat Latitude in deg*10000000
  * @param lat Longitude in deg*10000000
  */
static bool isPointInAmerica(int32_t lat, int32_t lon) {
	return isPointInPolygon(america, sizeof(america)/sizeof(america[0]), lat, lon);
}
static bool isPointInChina(int32_t lat, int32_t lon) {
	return isPointInPolygon(china, sizeof(china)/sizeof(china[0]), lat, lon);
}
static bool isPointInJapan(int32_t lat, int32_t lon) {
	return isPointInPolygon(japan, sizeof(japan)/sizeof(japan[0]), lat, lon);
}
static bool isPointInSouthkorea(int32_t lat, int32_t lon) {
	return isPointInPolygon(southkorea, sizeof(southkorea)/sizeof(southkorea[0]), lat, lon);
}
static bool isPointInSoutheastAsia(int32_t lat, int32_t lon) {
	return isPointInPolygon(southeastAsia, sizeof(southeastAsia)/sizeof(southeastAsia[0]), lat, lon);
}
static bool isPointInAustralia(int32_t lat, int32_t lon) {
	return isPointInPolygon(australia, sizeof(australia)/sizeof(australia[0]), lat, lon);
}
static bool isPointInNewZealand(int32_t lat, int32_t lon) {
	return isPointInPolygon(newzealand, sizeof(newzealand)/sizeof(newzealand[0]), lat, lon)
		|| isPointInPolygon(newzealand2, sizeof(newzealand2)/sizeof(newzealand2[0]), lat, lon);
}
static bool isPointInArgentina(int32_t lat, int32_t lon) { // Also includes Uruguay and Paraguay
	return isPointInPolygon(argentina, sizeof(argentina)/sizeof(argentina[0]), lat, lon);
}
static bool isPointInBrazil(int32_t lat, int32_t lon) {
	return isPointInPolygon(brazil, sizeof(brazil)/sizeof(brazil[0]), lat, lon);
}

/**
  * Returns APRS region specific frequency determined by GPS location. It will
  * use the APRS default frequency set in the config file if no GPS fix has
  * been received.
  */
uint32_t getAPRSRegionFrequency() {
	dataPoint_t *point = getLastDataPoint();

	// Position unknown
	if(point == NULL || (point->gps_lat == 0 && point->gps_lon == 0))
		//return conf_sram.freq;
	  // Return code and let pktradio figure out what to do.
	  return FREQ_APRS_DEFAULT;
	
	// America 144.390 MHz
	if(isPointInAmerica(point->gps_lat, point->gps_lon))
		return APRS_FREQ_AMERICA;

	// China 144.640 MHz
	if(isPointInChina(point->gps_lat, point->gps_lon))
		return APRS_FREQ_CHINA;

	// Japan 144.660 MHz
	if(isPointInJapan(point->gps_lat, point->gps_lon))
		return APRS_FREQ_JAPAN;

	// Southkorea 144.620 MHz
	if(isPointInSouthkorea(point->gps_lat, point->gps_lon))
		return APRS_FREQ_SOUTHKOREA;

	// Southkorea 144.620 MHz
	if(isPointInSoutheastAsia(point->gps_lat, point->gps_lon))
		return APRS_FREQ_SOUTHEASTASIA;

	// Australia 145.175 MHz
	if(isPointInAustralia(point->gps_lat, point->gps_lon))
		return APRS_FREQ_AUSTRALIA;

	// Australia 144.575 MHz
	if(isPointInNewZealand(point->gps_lat, point->gps_lon))
		return APRS_FREQ_NEWZEALAND;

	// Argentina/Paraguay/Uruguay 144.930 MHz
	if(isPointInArgentina(point->gps_lat, point->gps_lon))
		return APRS_FREQ_ARGENTINA;

	// Brazil 145.575 MHz
	if(isPointInBrazil(point->gps_lat, point->gps_lon))
		return APRS_FREQ_BRAZIL;

	return FREQ_APRS_DEFAULT;
}

