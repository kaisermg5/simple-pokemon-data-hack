
#include "headers/types.h"
#include "headers/pokemon.h"

extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u16 gSpecialVar_0x8006;
extern u16 gScriptResult; // var 800D
extern struct Pokemon gPlayerParty[6];

extern u32 GetMonData(struct Pokemon *mon, s32 field, u8* data);
extern void SetMonData(struct Pokemon *mon, s32 field, const void *data);
extern void CalculateMonStats(struct Pokemon *mon);
extern u16 CalculateBoxMonChecksum(struct BoxPokemon *boxMon);
extern void EncryptBoxMon(struct BoxPokemon *boxMon);
extern void DecryptBoxMon(struct BoxPokemon *boxMon);
extern u16 Random();
extern u32 __umodsi3(u32 x, u32 y);


void GetData()
{
	u32 ret;
	ret = GetMonData(&gPlayerParty[gSpecialVar_0x8004], gScriptResult, 0);
	if (!gSpecialVar_0x8005) {
		gScriptResult = (u16) ret;
	} else {
		gScriptResult = (u16) (ret >> 16);
	}
}

void SetData()
{
	u32 data;
	data = gSpecialVar_0x8005 + (gSpecialVar_0x8006 << 16); 
	SetMonData(&gPlayerParty[gSpecialVar_0x8004], gScriptResult, &data);
	CalculateMonStats(&gPlayerParty[gSpecialVar_0x8004]);
}

#define MOD_25_EQUALS_24 0x4f00
#define MOD_25_AND_24_EQUALS_0 0x4b00
#define MOD_24_EQUALS_16 0x1900

void SetNature()
{
	u32 personality, oldMod24, newMod24;
	u16 nature, originalNature, partyPos, firstByte;
	partyPos = gSpecialVar_0x8004;
	nature = gScriptResult;
	
	personality = GetMonData(&gPlayerParty[partyPos], MON_DATA_PERSONALITY, 0);
	
	firstByte = personality & 0xff;
	// The first byte contains data from the gender and ability
	originalNature = __umodsi3(firstByte, 25);

	// Lot's of numeric "hacks" to prevent the change in the order of the pokemon's subtructures
	// whilst modifiying the nature
	oldMod24 = __umodsi3(personality, 24);
	
	personality = firstByte + MOD_25_EQUALS_24 * (50 - nature + originalNature) + Random() * MOD_25_AND_24_EQUALS_0;
	newMod24 = __umodsi3(personality, 24);
	while (newMod24 != oldMod24) {
		personality += MOD_24_EQUALS_16;
		newMod24 = __umodsi3(personality, 24);
	}

	
	// Setting the new personality correctly...
	DecryptBoxMon(&gPlayerParty[partyPos].box);
	SetMonData(&gPlayerParty[partyPos], MON_DATA_PERSONALITY, &personality);

	gPlayerParty[partyPos].box.checksum = CalculateBoxMonChecksum(&gPlayerParty[partyPos].box);
	EncryptBoxMon(&gPlayerParty[partyPos].box);

	CalculateMonStats(&gPlayerParty[partyPos]);	
}

