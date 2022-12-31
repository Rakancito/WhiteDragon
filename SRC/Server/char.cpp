//Search
#include "buff_on_attributes.h"

//Add after

#ifdef ENABLE_WHITE_DRAGON
#include "WhiteDragon.h"
#endif


//Add in the end file

#ifdef ENABLE_WHITE_DRAGON
bool CHARACTER::IsWhiteMap()
{
	return WhiteDragon::CWhDr::instance().IsWhiteMap(GetMapIndex());
}
#endif


/*

The next part it's if you have Conqueror system, if you don't have, you can skip this.

*/

//Search

#ifdef ENABLE_QUEEN_NETHIS
				if (SnakeLair::CSnk::instance().IsSnakeMap(GetMapIndex()) && IsPC())
				{
					int value = 0; 
					int aValue = 0;

					value = GetPoint(POINT_SUNGMA_HP);
					aValue = SnakeLair::CSnk::instance().GetSungmaValueAffectDungeon(this, AFFECT_SUNGMA_HP); 
					if(value < aValue)
						SetMaxHP(hp + add_hp/2);
				}
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
				if (IsWhiteMap() && IsPC())
				{
					int value = 0; 
					int aValue = 0;

					value = GetPoint(POINT_SUNGMA_HP);
					aValue = GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_HP); 
					if(value < aValue)
						SetMaxHP(hp + add_hp/2);
				}

#endif

//Search


#ifdef ENABLE_QUEEN_NETHIS
			if (SnakeLair::CSnk::instance().IsSnakeMap(GetMapIndex()) && IsPC())
			{
				int value = 0; 
				int aValue = 0;
				value = GetPoint(POINT_SUNGMA_MOVE);
				aValue = SnakeLair::CSnk::instance().GetSungmaValueAffectDungeon(this, AFFECT_SUNGMA_MOVE); 
				if(value < aValue)
					SetPoint(type, 100);
				else
					SetPoint(type, 209);
			}
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
			if (IsWhiteMap() && IsPC())
			{
				int value = 0; 
				int aValue = 0;
				value = GetPoint(POINT_SUNGMA_MOVE);
				aValue = GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_MOVE); 
				if(value < aValue)
					SetPoint(type, 100);
				else
					SetPoint(type, 209);
			}
#endif

//Add in the end file

#ifdef ENABLE_CONQUEROR_LEVEL
int CHARACTER::GetSungmaValueAffectWhiteDungeon(int iAffect)
{
	return WhiteDragon::CWhDr::instance().GetSungmaValueAffectDungeon(this, iAffect);
}
#endif

