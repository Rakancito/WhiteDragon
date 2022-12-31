//Search

#include "DragonLair.h"

//Add after

#ifdef ENABLE_WHITE_DRAGON
#include "WhiteDragon.h"
#endif

//Search

#ifdef ENABLE_QUEEN_NETHIS
	if ((IsStone()) || (IsMonster()))
	{
		if (pkKiller && pkKiller->IsPC())
		{
			if (SnakeLair::CSnk::instance().IsSnakeMap(pkKiller->GetMapIndex()))
				SnakeLair::CSnk::instance().OnKill(this, pkKiller);
		}
	}
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
	if (IsMonster())
	{
		if (pkKiller && pkKiller->IsPC())
		{
			if (WhiteDragon::CWhDr::instance().IsWhiteMap(pkKiller->GetMapIndex()))
				WhiteDragon::CWhDr::instance().OnKill(this, pkKiller);
		}
	}
#endif


/*

The next part it's only if you have Conqueror System

*/

//Search

#ifdef ENABLE_QUEEN_NETHIS
		if (SnakeLair::CSnk::instance().IsSnakeMap(pAttacker->GetMapIndex()) && IsPC())
		{
			if(!IsPC())
			{
				int value = pAttacker->GetPoint(POINT_SUNGMA_STR); 
				int aValue =  SnakeLair::CSnk::instance().GetSungmaValueAffectDungeon(pAttacker, AFFECT_SUNGMA_STR);
				if(value < aValue)
				{
					dam /= 2;
				}

				if (pAttacker->GetConquerorLevel() == 0)
					dam = 0;
			}
		}
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
		if (pAttacker->IsWhiteMap() && IsPC())
		{
			if(!IsPC())
			{
				int value = pAttacker->GetPoint(POINT_SUNGMA_STR); 
				int aValue =  GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_STR);
				if(value < aValue)
				{
					dam /= 2;
				}

				if (pAttacker->GetConquerorLevel() == 0)
					dam = 0;
			}
		}
#endif