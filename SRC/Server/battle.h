You can skip this if you do not have Conqueror of Yohara.


//Search

#ifdef ENABLE_QUEEN_NETHIS
					if (pkVictim->IsSnakeMap())
						aValue = pkVictim->GetSungmaValueAffectDungeon(AFFECT_SUNGMA_INMUNE);

//Add after

#ifdef ENABLE_WHITE_DRAGON
					else if (pkVictim->IsWhiteMap())
						aValue = pkVictim->GetSungmaValueAffectWhiteDungeon(AFFECT_SUNGMA_INMUNE);
#endif