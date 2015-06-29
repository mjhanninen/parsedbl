/* parsedbl.cpp -- parses strings to IEEE doubles
   Copyright (c) 2015 Matti Hänninen */

#include <iostream>
#include <iomanip>
#include <cinttypes>

/*

MSVC doesn't support the POSIX functions for bit scanning. I'll probably need
to use something like this (ripped from Redis sources) on Windog. We'll see.

#ifndef strings_h
#define strings_h

// MSVC doesn't define ffs/ffsl. This dummy strings.h header is provided
// for both
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
static __forceinline int ffsl(long x)
{
	unsigned long i;

	if (_BitScanForward(&i, x))
		return (i + 1);
	return (0);
}

static __forceinline int ffs(int x)
{

	return (ffsl(x));
}

#endif

*/

#include <strings.h>

#define DEBUG

#ifdef DEBUG
#define LOOKING_AT(X)    (looking_at(X))
#define TRANSITION(X,Y)  (announce_transition(#X,#Y))
#else
#define LOOKING_AT(X)
#define TRANSITION(X,Y)
#endif

void
looking_at(const char *ch)
{
    std::cout << "Looking at '" << reinterpret_cast<const char *>(ch)
              << "'" << std::endl;
}

void
announce_transition(const std::string from, const std::string to)
{
    std::cout << "State transition: " << from << " -> " << to << std::endl;
}

const int min_power_of_ten = -323;
const int max_power_of_ten = 308;
const size_t powers_of_ten_count = max_power_of_ten - min_power_of_ten + 1;
const size_t index_of_one = -min_power_of_ten;

const uint64_t u_powers_of_ten[powers_of_ten_count] = {
    0x0000000000000002, 0x0000000000000014, 0x00000000000000CA, 0x00000000000007E8,
    0x0000000000004F10, 0x00000000000316A2, 0x00000000001EE257, 0x000000000134D761,
    0x000000000C1069CD, 0x0000000078A42205, 0x00000004B6695433, 0x0000002F201D49FB,
    0x000001D74124E3D1, 0x000012688B70E62B, 0x0000B8157268FDAF, 0x000730D67819E8D2,
    0x0031FA182C40C60D, 0x0066789E3750F791, 0x009C16C5C5253575, 0x00D18E3B9B374169,
    0x0105F1CA820511C3, 0x013B6E3D22865634, 0x017124E63593F5E1, 0x01A56E1FC2F8F359,
    0x01DAC9A7B3B7302F, 0x0210BE08D0527E1D, 0x0244ED8B04671DA5, 0x027A28EDC580E50E,
    0x02B059949B708F29, 0x02E46FF9C24CB2F3, 0x03198BF832DFDFB0, 0x034FEEF63F97D79C,
    0x0383F559E7BEE6C1, 0x03B8F2B061AEA072, 0x03EF2F5C7A1A488E, 0x04237D99CC506D59,
    0x04585D003F6488AF, 0x048E74404F3DAADB, 0x04C308A831868AC9, 0x04F7CAD23DE82D7B,
    0x052DBD86CD6238D9, 0x05629674405D6388, 0x05973C115074BC6A, 0x05CD0B15A491EB84,
    0x060226ED86DB3333, 0x0636B0A8E891FFFF, 0x066C5CD322B67FFF, 0x06A1BA03F5B21000,
    0x06D62884F31E93FF, 0x070BB2A62FE638FF, 0x07414FA7DDEFE3A0, 0x0775A391D56BDC87,
    0x07AB0C764AC6D3A9, 0x07E0E7C9EEBC444A, 0x081521BC6A6B555C, 0x084A6A2B85062AB3,
    0x0880825B3323DAB0, 0x08B4A2F1FFECD15C, 0x08E9CBAE7FE805B3, 0x09201F4D0FF10390,
    0x0954272053ED4474, 0x098930E868E89591, 0x09BF7D228322BAF5, 0x09F3AE3591F5B4D9,
    0x0A2899C2F6732210, 0x0A5EC033B40FEA93, 0x0A9338205089F29C, 0x0AC8062864AC6F43,
    0x0AFE07B27DD78B14, 0x0B32C4CF8EA6B6EC, 0x0B677603725064A8, 0x0B9D53844EE47DD1,
    0x0BD25432B14ECEA3, 0x0C06E93F5DA2824C, 0x0C3CA38F350B22DF, 0x0C71E6398126F5CB,
    0x0CA65FC7E170B33E, 0x0CDBF7B9D9CCE00D, 0x0D117AD428200C08, 0x0D45D98932280F0A,
    0x0D7B4FEB7EB212CD, 0x0DB111F32F2F4BC0, 0x0DE5566FFAFB1EB0, 0x0E1AAC0BF9B9E65C,
    0x0E50AB877C142FFA, 0x0E84D6695B193BF8, 0x0EBA0C03B1DF8AF6, 0x0EF047824F2BB6DA,
    0x0F245962E2F6A490, 0x0F596FBB9BB44DB4, 0x0F8FCBAA82A16121, 0x0FC3DF4A91A4DCB5,
    0x0FF8D71D360E13E2, 0x102F0CE4839198DB, 0x1063680ED23AFF89, 0x1098421286C9BF6B,
    0x10CE5297287C2F45, 0x1102F39E794D9D8B, 0x1137B08617A104EE, 0x116D9CA79D89462A,
    0x11A281E8C275CBDA, 0x11D72262F3133ED1, 0x120CEAFBAFD80E85, 0x124212DD4DE70913,
    0x12769794A160CB58, 0x12AC3D79C9B8FE2E, 0x12E1A66C1E139EDD, 0x1316100725988694,
    0x134B9408EEFEA839, 0x13813C85955F2923, 0x13B58BA6FAB6F36C, 0x13EAEE90B964B047,
    0x1420D51A73DEEE2D, 0x14550A6110D6A9B8, 0x148A4CF9550C5426, 0x14C0701BD527B498,
    0x14F48C22CA71A1BD, 0x1529AF2B7D0E0A2D, 0x15600D7B2E28C65C, 0x159410D9F9B2F7F3,
    0x15C91510781FB5F0, 0x15FF5A549627A36C, 0x16339874DDD8C623, 0x16687E92154EF7AC,
    0x169E9E369AA2B597, 0x16D322E220A5B17E, 0x1707EB9AA8CF1DDE, 0x173DE6815302E556,
    0x1772B010D3E1CF56, 0x17A75C1508DA432B, 0x17DD331A4B10D3F6, 0x18123FF06EEA847A,
    0x1846CFEC8AA52598, 0x187C83E7AD4E6EFE, 0x18B1D270CC51055F, 0x18E6470CFF6546B6,
    0x191BD8D03F3E9864, 0x1951678227871F3E, 0x1985C162B168E70E, 0x19BB31BB5DC320D2,
    0x19F0FF151A99F483, 0x1A253EDA614071A4, 0x1A5A8E90F9908E0D, 0x1A90991A9BFA58C8,
    0x1AC4BF6142F8EEFA, 0x1AF9EF3993B72AB8, 0x1B303583FC527AB3, 0x1B6442E4FB671960,
    0x1B99539E3A40DFB8, 0x1BCFA885C8D117A6, 0x1C03C9539D82AEC8, 0x1C38BBA884E35A7A,
    0x1C6EEA92A61C3118, 0x1CA3529BA7D19EAF, 0x1CD8274291C6065B, 0x1D0E3113363787F2,
    0x1D42DEAC01E2B4F7, 0x1D779657025B6235, 0x1DAD7BECC2F23AC2, 0x1DE26D73F9D764B9,
    0x1E1708D0F84D3DE7, 0x1E4CCB0536608D61, 0x1E81FEE341FC585D, 0x1EB67E9C127B6E74,
    0x1EEC1E43171A4A11, 0x1F2192E9EE706E4B, 0x1F55F7A46A0C89DD, 0x1F8B758D848FAC55,
    0x1FC1297872D9CBB5, 0x1FF573D68F903EA2, 0x202AD0CC33744E4B, 0x2060C27FA028B0EF,
    0x2094F31F8832DD2A, 0x20CA2FE76A3F9475, 0x21005DF0A267BCC9, 0x2134756CCB01ABFB,
    0x216992C7FDC216FA, 0x219FF779FD329CB9, 0x21D3FAAC3E3FA1F3, 0x2208F9574DCF8A70,
    0x223F37AD21436D0C, 0x227382CC34CA2428, 0x22A8637F41FCAD32, 0x22DE7C5F127BD87E,
    0x23130DBB6B8D674F, 0x2347D12A4670C123, 0x237DC574D80CF16B, 0x23B29B69070816E3,
    0x23E7424348CA1C9C, 0x241D12D41AFCA3C3, 0x24522BC490DDE65A, 0x2486B6B5B5155FF0,
    0x24BC6463225AB7EC, 0x24F1BEBDF578B2F4, 0x25262E6D72D6DFB0, 0x255BBA08CF8C979D,
    0x2591544581B7DEC2, 0x25C5A956E225D672, 0x25FB13AC9AAF4C0F, 0x2630EC4BE0AD8F89,
    0x2665275ED8D8F36C, 0x269A71368F0F3047, 0x26D086C219697E2C, 0x2704A8729FC3DDB7,
    0x2739D28F47B4D525, 0x277023998CD10537, 0x27A42C7FF0054685, 0x27D9379FEC069826,
    0x280F8587E7083E30, 0x2843B374F06526DE, 0x2878A0522C7E7095, 0x28AEC866B79E0CBA,
    0x28E33D4032C2C7F5, 0x29180C903F7379F2, 0x294E0FB44F50586E, 0x2982C9D0B1923745,
    0x29B77C44DDF6C516, 0x29ED5B561574765B, 0x2A225915CD68C9F9, 0x2A56EF5B40C2FC77,
    0x2A8CAB3210F3BB95, 0x2AC1EAFF4A98553D, 0x2AF665BF1D3E6A8D, 0x2B2BFF2EE48E0530,
    0x2B617F7D4ED8C33E, 0x2B95DF5CA28EF40D, 0x2BCB5733CB32B111, 0x2C0116805EFFAEAA,
    0x2C355C2076BF9A55, 0x2C6AB328946F80EA, 0x2CA0AFF95CC5B092, 0x2CD4DBF7B3F71CB7,
    0x2D0A12F5A0F4E3E5, 0x2D404BD984990E6F, 0x2D745ECFE5BF520B, 0x2DA97683DF2F268D,
    0x2DDFD424D6FAF031, 0x2E13E497065CD61F, 0x2E48DDBCC7F40BA6, 0x2E7F152BF9F10E90,
    0x2EB36D3B7C36A91A, 0x2EE8488A5B445360, 0x2F1E5AACF2156838, 0x2F52F8AC174D6123,
    0x2F87B6D71D20B96C, 0x2FBDA48CE468E7C7, 0x2FF286D80EC190DC, 0x3027288E1271F513,
    0x305CF2B1970E7258, 0x309217AEFE690777, 0x30C69D9ABE034955, 0x30FC45016D841BAA,
    0x3131AB20E472914A, 0x316615E91D8F359D, 0x319B9B6364F30304, 0x31D1411E1F17E1E3,
    0x32059165A6DDDA5B, 0x323AF5BF109550F2, 0x3270D9976A5D5297, 0x32A50FFD44F4A73D,
    0x32DA53FC9631D10D, 0x3310747DDDDF22A8, 0x3344919D5556EB52, 0x3379B604AAACA626,
    0x33B011C2EAABE7D8, 0x33E41633A556E1CE, 0x34191BC08EAC9A41, 0x344F62B0B257C0D2,
    0x34839DAE6F76D883, 0x34B8851A0B548EA4, 0x34EEA6608E29B24D, 0x352327FC58DA0F70,
    0x3557F1FB6F10934C, 0x358DEE7A4AD4B81F, 0x35C2B50C6EC4F313, 0x35F7624F8A762FD8,
    0x362D3AE36D13BBCE, 0x366244CE242C5561, 0x3696D601AD376AB9, 0x36CC8B8218854567,
    0x3701D7314F534B61, 0x37364CFDA3281E39, 0x376BE03D0BF225C7, 0x37A16C262777579C,
    0x37D5C72FB1552D83, 0x380B38FB9DAA78E4, 0x3841039D428A8B8F, 0x38754484932D2E72,
    0x38AA95A5B7F87A0F, 0x38E09D8792FB4C49, 0x3914C4E977BA1F5C, 0x3949F623D5A8A733,
    0x398039D665896880, 0x39B4484BFEEBC2A0, 0x39E95A5EFEA6B347, 0x3A1FB0F6BE506019,
    0x3A53CE9A36F23C10, 0x3A88C240C4AECB14, 0x3ABEF2D0F5DA7DD9, 0x3AF357C299A88EA7,
    0x3B282DB34012B251, 0x3B5E392010175EE6, 0x3B92E3B40A0E9B4F, 0x3BC79CA10C924223,
    0x3BFD83C94FB6D2AC, 0x3C32725DD1D243AC, 0x3C670EF54646D497, 0x3C9CD2B297D889BC,
    0x3CD203AF9EE75616, 0x3D06849B86A12B9B, 0x3D3C25C268497682, 0x3D719799812DEA11,
    0x3DA5FD7FE1796495, 0x3DDB7CDFD9D7BDBB, 0x3E112E0BE826D695, 0x3E45798EE2308C3A,
    0x3E7AD7F29ABCAF48, 0x3EB0C6F7A0B5ED8D, 0x3EE4F8B588E368F1, 0x3F1A36E2EB1C432D,
    0x3F50624DD2F1A9FC, 0x3F847AE147AE147B, 0x3FB999999999999A, 0x3FF0000000000000,
    0x4024000000000000, 0x4059000000000000, 0x408F400000000000, 0x40C3880000000000,
    0x40F86A0000000000, 0x412E848000000000, 0x416312D000000000, 0x4197D78400000000,
    0x41CDCD6500000000, 0x4202A05F20000000, 0x42374876E8000000, 0x426D1A94A2000000,
    0x42A2309CE5400000, 0x42D6BCC41E900000, 0x430C6BF526340000, 0x4341C37937E08000,
    0x4376345785D8A000, 0x43ABC16D674EC800, 0x43E158E460913D00, 0x4415AF1D78B58C40,
    0x444B1AE4D6E2EF50, 0x4480F0CF064DD592, 0x44B52D02C7E14AF6, 0x44EA784379D99DB4,
    0x45208B2A2C280291, 0x4554ADF4B7320335, 0x4589D971E4FE8402, 0x45C027E72F1F1281,
    0x45F431E0FAE6D721, 0x46293E5939A08CEA, 0x465F8DEF8808B024, 0x4693B8B5B5056E17,
    0x46C8A6E32246C99C, 0x46FED09BEAD87C03, 0x4733426172C74D82, 0x476812F9CF7920E3,
    0x479E17B84357691B, 0x47D2CED32A16A1B1, 0x48078287F49C4A1D, 0x483D6329F1C35CA5,
    0x48725DFA371A19E7, 0x48A6F578C4E0A061, 0x48DCB2D6F618C879, 0x4911EFC659CF7D4C,
    0x49466BB7F0435C9E, 0x497C06A5EC5433C6, 0x49B18427B3B4A05C, 0x49E5E531A0A1C873,
    0x4A1B5E7E08CA3A8F, 0x4A511B0EC57E649A, 0x4A8561D276DDFDC0, 0x4ABABA4714957D30,
    0x4AF0B46C6CDD6E3E, 0x4B24E1878814C9CE, 0x4B5A19E96A19FC41, 0x4B905031E2503DA9,
    0x4BC4643E5AE44D13, 0x4BF97D4DF19D6057, 0x4C2FDCA16E04B86D, 0x4C63E9E4E4C2F344,
    0x4C98E45E1DF3B015, 0x4CCF1D75A5709C1B, 0x4D03726987666191, 0x4D384F03E93FF9F5,
    0x4D6E62C4E38FF872, 0x4DA2FDBB0E39FB47, 0x4DD7BD29D1C87A19, 0x4E0DAC74463A989F,
    0x4E428BC8ABE49F64, 0x4E772EBAD6DDC73D, 0x4EACFA698C95390C, 0x4EE21C81F7DD43A7,
    0x4F16A3A275D49491, 0x4F4C4C8B1349B9B5, 0x4F81AFD6EC0E1411, 0x4FB61BCCA7119916,
    0x4FEBA2BFD0D5FF5B, 0x502145B7E285BF99, 0x50559725DB272F7F, 0x508AFCEF51F0FB5F,
    0x50C0DE1593369D1B, 0x50F5159AF8044462, 0x512A5B01B605557B, 0x516078E111C3556D,
    0x5194971956342AC8, 0x51C9BCDFABC1357A, 0x5200160BCB58C16C, 0x52341B8EBE2EF1C7,
    0x526922726DBAAE39, 0x529F6B0F092959C7, 0x52D3A2E965B9D81D, 0x53088BA3BF284E24,
    0x533EAE8CAEF261AD, 0x53732D17ED577D0C, 0x53A7F85DE8AD5C4F, 0x53DDF67562D8B363,
    0x5412BA095DC7701E, 0x5447688BB5394C25, 0x547D42AEA2879F2E, 0x54B249AD2594C37D,
    0x54E6DC186EF9F45C, 0x551C931E8AB87173, 0x5551DBF316B346E8, 0x558652EFDC6018A2,
    0x55BBE7ABD3781ECA, 0x55F170CB642B133F, 0x5625CCFE3D35D80E, 0x565B403DCC834E12,
    0x569108269FD210CB, 0x56C54A3047C694FE, 0x56FA9CBC59B83A3D, 0x5730A1F5B8132466,
    0x5764CA732617ED80, 0x5799FD0FEF9DE8E0, 0x57D03E29F5C2B18C, 0x58044DB473335DEF,
    0x583961219000356B, 0x586FB969F40042C5, 0x58A3D3E2388029BB, 0x58D8C8DAC6A0342A,
    0x590EFB1178484135, 0x59435CEAEB2D28C1, 0x59783425A5F872F1, 0x59AE412F0F768FAD,
    0x59E2E8BD69AA19CC, 0x5A17A2ECC414A03F, 0x5A4D8BA7F519C84F, 0x5A827748F9301D32,
    0x5AB7151B377C247E, 0x5AECDA62055B2D9E, 0x5B22087D4358FC82, 0x5B568A9C942F3BA3,
    0x5B8C2D43B93B0A8C, 0x5BC19C4A53C4E697, 0x5BF6035CE8B6203D, 0x5C2B843422E3A84D,
    0x5C6132A095CE4930, 0x5C957F48BB41DB7C, 0x5CCADF1AEA12525B, 0x5D00CB70D24B7379,
    0x5D34FE4D06DE5057, 0x5D6A3DE04895E46D, 0x5DA066AC2D5DAEC4, 0x5DD4805738B51A75,
    0x5E09A06D06E26112, 0x5E400444244D7CAB, 0x5E7405552D60DBD6, 0x5EA906AA78B912CC,
    0x5EDF485516E7577F, 0x5F138D352E5096AF, 0x5F48708279E4BC5B, 0x5F7E8CA3185DEB72,
    0x5FB317E5EF3AB327, 0x5FE7DDDF6B095FF1, 0x601DD55745CBB7ED, 0x6052A5568B9F52F4,
    0x60874EAC2E8727B1, 0x60BD22573A28F19D, 0x60F2357684599702, 0x6126C2D4256FFCC3,
    0x615C73892ECBFBF4, 0x6191C835BD3F7D78, 0x61C63A432C8F5CD6, 0x61FBC8D3F7B3340C,
    0x62315D847AD00087, 0x6265B4E5998400A9, 0x629B221EFFE500D4, 0x62D0F5535FEF2084,
    0x630532A837EAE8A5, 0x633A7F5245E5A2CF, 0x63708F936BAF85C1, 0x63A4B378469B6732,
    0x63D9E056584240FE, 0x64102C35F729689F, 0x6444374374F3C2C6, 0x647945145230B378,
    0x64AF965966BCE056, 0x64E3BDF7E0360C36, 0x6518AD75D8438F43, 0x654ED8D34E547314,
    0x6583478410F4C7EC, 0x65B819651531F9E8, 0x65EE1FBE5A7E7861, 0x6622D3D6F88F0B3D,
    0x665788CCB6B2CE0C, 0x668D6AFFE45F818F, 0x66C262DFEEBBB0F9, 0x66F6FB97EA6A9D38,
    0x672CBA7DE5054486, 0x6761F48EAF234AD4, 0x679671B25AEC1D89, 0x67CC0E1EF1A724EB,
    0x680188D357087713, 0x6835EB082CCA94D7, 0x686B65CA37FD3A0D, 0x68A11F9E62FE4448,
    0x68D56785FBBDD55A, 0x690AC1677AAD4AB1, 0x6940B8E0ACAC4EAF, 0x6974E718D7D7625A,
    0x69AA20DF0DCD3AF1, 0x69E0548B68A044D6, 0x6A1469AE42C8560C, 0x6A498419D37A6B8F,
    0x6A7FE52048590673, 0x6AB3EF342D37A408, 0x6AE8EB0138858D0A, 0x6B1F25C186A6F04C,
    0x6B537798F4285630, 0x6B88557F31326BBB, 0x6BBE6ADEFD7F06AA, 0x6BF302CB5E6F642A,
    0x6C27C37E360B3D35, 0x6C5DB45DC38E0C82, 0x6C9290BA9A38C7D1, 0x6CC734E940C6F9C6,
    0x6CFD022390F8B837, 0x6D3221563A9B7323, 0x6D66A9ABC9424FEB, 0x6D9C5416BB92E3E6,
    0x6DD1B48E353BCE70, 0x6E0621B1C28AC20C, 0x6E3BAA1E332D728F, 0x6E714A52DFFC6799,
    0x6EA59CE797FB817F, 0x6EDB04217DFA61DF, 0x6F10E294EEBC7D2C, 0x6F451B3A2A6B9C76,
    0x6F7A6208B5068394, 0x6FB07D457124123D, 0x6FE49C96CD6D16CC, 0x7019C3BC80C85C7F,
    0x70501A55D07D39CF, 0x708420EB449C8843, 0x70B9292615C3AA54, 0x70EF736F9B3494E9,
    0x7123A825C100DD11, 0x7158922F31411456, 0x718EB6BAFD91596B, 0x71C33234DE7AD7E3,
    0x71F7FEC216198DDC, 0x722DFE729B9FF153, 0x7262BF07A143F6D4, 0x72976EC98994F489,
    0x72CD4A7BEBFA31AB, 0x73024E8D737C5F0B, 0x7336E230D05B76CD, 0x736C9ABD04725481,
    0x73A1E0B622C774D0, 0x73D658E3AB795204, 0x740BEF1C9657A686, 0x74417571DDF6C814,
    0x7475D2CE55747A18, 0x74AB4781EAD1989E, 0x74E10CB132C2FF63, 0x75154FDD7F73BF3C,
    0x754AA3D4DF50AF0B, 0x7580A6650B926D67, 0x75B4CFFE4E7708C0, 0x75EA03FDE214CAF1,
    0x7620427EAD4CFED6, 0x7654531E58A03E8C, 0x768967E5EEC84E2F, 0x76BFC1DF6A7A61BB,
    0x76F3D92BA28C7D15, 0x7728CF768B2F9C5A, 0x775F03542DFB8370, 0x779362149CBD3226,
    0x77C83A99C3EC7EB0, 0x77FE494034E79E5C, 0x7832EDC82110C2F9, 0x7867A93A2954F3B8,
    0x789D9388B3AA30A5, 0x78D27C35704A5E67, 0x79071B42CC5CF601, 0x793CE2137F743382,
    0x79720D4C2FA8A031, 0x79A6909F3B92C83D, 0x79DC34C70A777A4D, 0x7A11A0FC668AAC70,
    0x7A46093B802D578C, 0x7A7B8B8A6038AD6F, 0x7AB137367C236C65, 0x7AE585041B2C477F,
    0x7B1AE64521F7595E, 0x7B50CFEB353A97DB, 0x7B8503E602893DD2, 0x7BBA44DF832B8D46,
    0x7BF06B0BB1FB384C, 0x7C2485CE9E7A065F, 0x7C59A742461887F6, 0x7C9008896BCF54FA,
    0x7CC40AABC6C32A38, 0x7CF90D56B873F4C7, 0x7D2F50AC6690F1F8, 0x7D63926BC01A973B,
    0x7D987706B0213D0A, 0x7DCE94C85C298C4C, 0x7E031CFD3999F7B0, 0x7E37E43C8800759C,
    0x7E6DDD4BAA009303, 0x7EA2AA4F4A405BE2, 0x7ED754E31CD072DA, 0x7F0D2A1BE4048F90,
    0x7F423A516E82D9BA, 0x7F76C8E5CA239029, 0x7FAC7B1F3CAC7433, 0x7FE1CCF385EBC8A0
};

const double *powers_of_ten = reinterpret_cast<const double *>(u_powers_of_ten);

bool
parse_double(const char *ch, double &v)
{
    union {
        double d;                       // as double
        struct {
            uint64_t m : 52;            // the mantissa part
            uint64_t e : 11;            // the exponent past (base-2)
            uint64_t s : 1;             // the sign bit
        } b;                            // as IEEE double fields
        uint64_t u;                     // as 64-bit uint
    } result = { 0 };

    uint64_t is_mantissa_neg;           // Is the mantissa negative?  No need to
                                        // initialize, this is set always.

    uint64_t mantissa = 0;              // The mantissa gradually built up to
                                        // rougly 53 bits of significance (if
                                        // enough digits available).

    bool is_exp10_neg;                  // Is the exponent negative? No need to
                                        // initialize, this is set always.

    int32_t exp10 = 0;                  // The exponent part; first taken from
                                        // the explicit exponent and then
                                        // adjusted in the exponent part of the
                                        // literal.

    uint64_t digit;                     // Holds the digit that is currently
                                        // being processed.

    uint32_t decimal_count = 0;         // Tracks the number of digits
                                        // encountered since the decimal mark
                                        // regardless of their singificance.

    uint32_t digits_left = 16;          // Tracks how many significant digits we
                                        // still want to collect to the mantissa
                                        // (if available).
                                        //
                                        // Collect only the 1 + 16 most
                                        // significant digits. This fills
                                        // between 54 and 56 bits of
                                        // significance that is slightly more
                                        // then 53 bits present in a double
                                        // (noting that lg2(10^16) =
                                        // 53.15). After this the digits don't
                                        // contribute to the accuracy (ignoring
                                        // rounding effect), only to the
                                        // magnitude, possible.

    uint32_t zero_count = 0;            // Tracks how many trailing zeros (or
                                        // other non-significant digits) there
                                        // are since the last significant
                                        // non-zero digit.

    int32_t leading_bit;                // The index of the most significant bit
                                        // of the mantissa. Used to finally
                                        // shift the mantissa so that the most
                                        // significant bit ends up being the
                                        // 53rd bit.

    // Skip leading whitespace.

    while (*ch == ' ' || *ch == '\t') {
        ch++;
    }

    // Process the optional sign; only the minus sign has any real effect. We
    // skip also the whitespace trailing the sign to be compatible with the
    // reference implementation. It is however very uncustomary to allow
    // whitespace here.

    if ((is_mantissa_neg = *ch == '-') || *ch == '+') {
        ch++;
        while (*ch == ' ' || *ch == '\t') {
            ch++;
        }
    }

    TRANSITION(START, leading_integral_zeros);

  leading_integral_zeros:

    while (*ch == '0') {
        ch++;
    }
    if (*ch == '.' || *ch == ',') {
        ch++;
        TRANSITION(leading_integral_zeros, leading_decimal_zeros);
    }
    else {
        LOOKING_AT(ch);
        TRANSITION(leading_integral_zeros, significant_integral);
        goto significant_integral;
    }

  leading_decimal_zeros:

    while (*ch == '0') {
        decimal_count++;
        ch++;
    }
    TRANSITION(leading_decimal_zeros, significant_decimals);
    goto significant_decimals;

  significant_integral:

    digit = *ch - '0';
    while (digit < 10) {
        while (zero_count) {
            zero_count--;
            mantissa *= 10;
        }
        while ((digit - 1) < 9) {
            ch++;
            mantissa = digit + mantissa * 10;
            if (--digits_left) {
                digit = *ch - '0';
            }
            else {
                TRANSITION(significant_integral, insignificant_integral);
                goto insignificant_integral;
            }
        }
        while (digit == 0) {
            ch++;
            zero_count++;
            if (--digits_left) {
                digit = *ch - '0';
            }
            else {
                TRANSITION(significant_integral, insignificant_integral);
                goto insignificant_integral;
            }
            digit = *ch - '0';
        }
    }
    if (*ch == '.' || *ch == ',') {
        ch++;
        TRANSITION(significant_integral, significant_decimals);
    }
    else {
        TRANSITION(significant_integral, exponent);
        goto exponent;
    }

  significant_decimals:

    digit = *ch - '0';
    while (digit < 10) {
        if (digit != 0) {
            while (zero_count) {
                zero_count--;
                mantissa *= 10;
            }
        }
        while ((digit - 1) < 9) {
            ch++;
            decimal_count++;
            mantissa = digit + mantissa * 10;
            if (--digits_left) {
                digit = *ch - '0';
            }
            else {
                TRANSITION(significant_decimals, insignificant_decimals);
                goto insignificant_decimals;
            }
        }
        while (digit == 0) {
            ch++;
            decimal_count++;
            zero_count++;
            if (--digits_left) {
                digit = *ch - '0';
            }
            else {
                TRANSITION(significant_decimals, insignificant_decimals);
                goto insignificant_decimals;
            }
        }
    }
    TRANSITION(significant_decimals, exponent);
    goto exponent;

  insignificant_integral:

    // NB: Assignment to digit to prevent C/C++ integer promotion. Check if a
    // static cast would produce better code.
    while ((digit = *ch - '0') < 10) {
        ch++;
        zero_count++;
    }
    if (*ch == '.' || *ch == ',') {
        ch++;
    }
    else {
        TRANSITION(insignificant_integral, exponent);
        goto exponent;
    }
    TRANSITION(insignificant_integral, insignificant_decimals);

  insignificant_decimals:

    while ((digit = *ch - '0') < 10) {
        ch++;
        zero_count++;
        decimal_count++;
    }
    TRANSITION(insignificant_decimals, exponent);

  exponent:

    if (*ch == 'e' || *ch == 'E') {
        ch++;
        if ((is_exp10_neg = *ch == '-') || *ch == '+') {
            ch++;
        }
        while ((digit = *ch - '0') < 10) {
            exp10 = digit + exp10 * 10;
            ch++;
        }
        if (is_exp10_neg) {
            exp10 *= -1;
        }
    }
    TRANSITION(exponent, FINISH);

  finish:

    if (*ch != '\0' && *ch != ' ' && *ch != '\t') {
        LOOKING_AT(ch);
        return false;
    }

    // Is it a zero?

    if (!mantissa) {
        v = 0.0;
        return true;
    }

    exp10 += zero_count - decimal_count;
    if (exp10 < min_power_of_ten) {
        return false;
    }
    if (exp10 > max_power_of_ten) {
        return false;
    }
    leading_bit = flsl(mantissa);
    if (leading_bit > 53) {
        result.u = mantissa >> (leading_bit - 53);
    }
    else if (leading_bit < 53) {
        result.u = mantissa << (53 - leading_bit);
    }
    result.b.e = 1022 + leading_bit;
    result.b.s = is_mantissa_neg ? 1 : 0;
    result.d *= powers_of_ten[exp10 + index_of_one];
    v = result.d;
    return true;
}

int
main(int argc, char *argv[])
{
    double x;
    bool r;
    for (size_t i = 1; i < argc; i++) {
        r = parse_double(argv[i], x);
        std::cout << "ARG(" << i << ") = '" << argv[i]
                  << "' -> " << std::setprecision(15) << x
                  << " (" << (r ? "SUCCESS" : "FAILURE") << ")"
                  << std::endl;
    }
}
