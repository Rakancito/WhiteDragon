You can skip this if you do not have Conqueror of Yohara.


//Search

#include "XTrapManager.h"

//Add after

#ifdef ENABLE_WHITE_DRAGON
#include "WhiteDragon.h"
#endif

//Search
#ifdef ENABLE_QUEEN_NETHIS
	if (SnakeLair::CSnk::instance().IsSnakeMap(ch->GetMapIndex()) && ch->IsPC())
	{
		ch->PointChange(POINT_MAX_HP, 0);
		ch->PointChange(POINT_MOV_SPEED, 0);

		const BYTE bSTR = SnakeLair::CSnk::instance().GetSungmaValueAffectDungeon(ch, AFFECT_SUNGMA_STR);
		const BYTE bHP = SnakeLair::CSnk::instance().GetSungmaValueAffectDungeon(ch, AFFECT_SUNGMA_HP);
		const BYTE bMOVE = SnakeLair::CSnk::instance().GetSungmaValueAffectDungeon(ch, AFFECT_SUNGMA_MOVE);
		const BYTE bINMUNE = SnakeLair::CSnk::instance().GetSungmaValueAffectDungeon(ch, AFFECT_SUNGMA_INMUNE);
		TPacketGCSungmaAttrUpdate p;

		p.bHeader = HEADER_GC_SUNGMA_ATTR_UPDATE;
		p.bSTR = bSTR;
		p.bHP = bHP;
		p.bMOVE = bMOVE;
		p.bINMUNE = bINMUNE;
		ch->GetDesc()->Packet(&p, sizeof(p));
	}
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
	if (ch->IsWhiteMap() && ch->IsPC())
	{
		ch->PointChange(POINT_MAX_HP, 0);
		ch->PointChange(POINT_MOV_SPEED, 0);

		const BYTE bSTR = ch->GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_STR);
		const BYTE bHP = ch->GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_HP);
		const BYTE bMOVE = ch->GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_MOVE);
		const BYTE bINMUNE = ch->GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_INMUNE);
		TPacketGCSungmaAttrUpdate p;

		p.bHeader = HEADER_GC_SUNGMA_ATTR_UPDATE;
		p.bSTR = bSTR;
		p.bHP = bHP;
		p.bMOVE = bMOVE;
		p.bINMUNE = bINMUNE;
		ch->GetDesc()->Packet(&p, sizeof(p));
	}

#endif