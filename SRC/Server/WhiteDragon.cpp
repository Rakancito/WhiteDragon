/*
	Author: R4kan
	Date: 31-01-2022
	Version: 0.5
	Github: https://github.com/Rakancito/
*/

#include "stdafx.h"
#include "WhiteDragon.h"

#include "item.h"
#include "char.h"
#include "utils.h"
#include "party.h"
#include "regen.h"
#include "config.h"
#include "packet.h"
#include "motion.h"
#include "item_manager.h"
#include "guild_manager.h"
#include "guild.h"
#include "start_position.h"
#include "locale_service.h"
#include "char_manager.h"
#include <boost/lexical_cast.hpp> 

namespace WhiteDragon
{
	//Estructura recolectora de miembros de grupo.
	struct FPartyCHCollector
	{
		std::vector <DWORD> pPlayerID;
		FPartyCHCollector()
		{
		}
		void operator () (LPCHARACTER ch)
		{
			pPlayerID.push_back(ch->GetPlayerID());
		}
	};

	struct FExitAndGoTo
	{
		FExitAndGoTo() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = (LPCHARACTER) ent;
				if (pkChar && pkChar->IsPC())
				{
					PIXEL_POSITION posSub = CWhDr::instance().GetSubXYZ();
					if (!posSub.x)
						pkChar->WarpSet(EMPIRE_START_X(pkChar->GetEmpire()), EMPIRE_START_Y(pkChar->GetEmpire()));
					else
						pkChar->WarpSet(posSub.x, posSub.y);
				}
			}
		}
	};

	// DUNGEON_KILL_ALL_BUG_FIX
	struct FPurgeSectree
	{
		void operator () (LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;

#ifdef NEW_PET_SYSTEM
				if (ch && !ch->IsPC() && !ch->IsPet() && !ch->IsNewPet())
#else
				if (ch && !ch->IsPC() && !ch->IsPet())
#endif
				{
					if (ch->IsNPC())
						M2_DESTROY_CHARACTER(ch);
					else
						ch->Dead(); //fix Purge Area
				}
			}
		}
	};
	// END_OF_DUNGEON_KILL_ALL_BUG_FIX

#ifdef ENABLE_CONQUEROR_LEVEL
	int iSungmaAffect[4][3] =
	{
		{AFFECT_SUNGMA_STR, 60, 70},
		{AFFECT_SUNGMA_HP, 50, 60},
		{AFFECT_SUNGMA_MOVE, 60, 70},
		{AFFECT_SUNGMA_INMUNE, 65, 75},
	};
#endif

	EVENTINFO(r_whitespawn_info)
	{
		CWhDrMap*	pMap;
		BYTE	bStep;
		BYTE	bSubStep;
	};
	
	EVENTFUNC(r_whitespawn_event)
	{
		r_whitespawn_info* pEventInfo = dynamic_cast<r_whitespawn_info*>(event->info);
		if (!pEventInfo)
			return 0;
		
		CWhDrMap* pMap = pEventInfo->pMap;
		if (!pMap)
			return 0;
		
		if (!pMap->GetMapSectree())
			return 0;
		
		if (pEventInfo->bStep == 2)
		{
			if (pMap->GetDungeonLevel())
				pMap->Spawn((DWORD)BOSS_VNUM, 138, 94, 0);
			else
				pMap->Spawn((DWORD)BOSS_EASY_VNUM, 138, 94, 0);
			return 0;
		}

		if (pEventInfo->bStep == 3)
		{
			pMap->EndDungeonWarp();
			return 0;
		}

		return 0;
	}

	EVENTINFO(r_whitelimit_info)
	{
		CWhDrMap*	pMap;
	};
	
	EVENTFUNC(r_whitelimit_event)
	{
		r_whitelimit_info* pEventInfo = dynamic_cast<r_whitelimit_info*>(event->info);
		if (pEventInfo)
		{
			CWhDrMap* pMap = pEventInfo->pMap;
			if (pMap)
			{
				pMap->EndDungeonWarp();
			}
		}
		
		return 0;
	}

	/*
		Object Part
	*/
	CWhDrMap::CWhDrMap(long lMapIndex, bool bHard)
	{
		SetDungeonStep(1);
		SetMapIndex(lMapIndex);

		SetMapSectree(SECTREE_MANAGER::instance().GetMap(map_index));

		SetDungeonLevel(bHard);
#ifdef ENABLE_CONQUEROR_LEVEL
		m_vec_mapSungmaAffectQueen.clear();
		LoadSungmaAttr();
#endif

		Start();
	}

	CWhDrMap::~CWhDrMap()
	{
		if (e_SpawnEvent != NULL)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = NULL;

		if (e_LimitEvent != NULL)
			event_cancel(&e_LimitEvent);
		e_LimitEvent = NULL;
	}

	void CWhDrMap::Destroy()
	{
		if (e_SpawnEvent != NULL)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = NULL;

		if (e_LimitEvent != NULL)
			event_cancel(&e_LimitEvent);
		e_LimitEvent = NULL;

		SetDungeonStep(1);
		SetMapIndex(NULL);
		SetMapSectree(NULL);
		SetParty(NULL);

#ifdef ENABLE_CONQUEROR_LEVEL
		m_vec_mapSungmaAffectQueen.clear();
#endif

	}

	void CWhDrMap::StartDungeon(LPCHARACTER pkChar)
	{
		char szNotice[512];
		snprintf(szNotice, sizeof(szNotice), LC_TEXT("The dungeon will be available for %d minutes."), DWORD(TIME_LIMIT_DUNGEON)/60);
		SendNoticeMap(szNotice, pkChar->GetMapIndex(), true);
										
		pkChar->SetQuestNPCID(0);

		GetWhiteEggNPC()->Dead();
		SetWhiteEggNPC(NULL);
		
		if (e_LimitEvent != NULL)
			event_cancel(&e_LimitEvent);
		e_LimitEvent = NULL;

		r_whitelimit_info* pEventInfo = AllocEventInfo<r_whitelimit_info>();
		pEventInfo->pMap = this;
		e_LimitEvent = event_create(r_whitelimit_event, pEventInfo, PASSES_PER_SEC(3600));

		SetDungeonStep(2);
	}

	void CWhDrMap::SetDungeonStep(BYTE bStep)
	{		
		dungeon_step = bStep;

		if (e_SpawnEvent != NULL)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = NULL;
				
		if (bStep == 2)
		{
			r_whitespawn_info* pEventInfo = AllocEventInfo<r_whitespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_whitespawn_event, pEventInfo, PASSES_PER_SEC(2));
		}
	}

	void CWhDrMap::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{		
		BYTE bStep = dungeon_step;

		if (!bStep)
			return;
		
		if (!GetMapSectree())
			return;

		if (((bStep == 2) && (pkMonster->GetMobTable().dwVnum == BOSS_VNUM || pkMonster->GetMobTable().dwVnum == BOSS_EASY_VNUM)))
		{
			FPurgeSectree f;
			GetMapSectree()->for_each(f);

			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_TEXT("All partecipants will be teleport in 60 seconds."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			if (e_SpawnEvent != NULL)
				event_cancel(&e_SpawnEvent);
			e_SpawnEvent = NULL;

			r_whitespawn_info* pEventInfo = AllocEventInfo<r_whitespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = 3;
			e_SpawnEvent = event_create(r_whitespawn_event, pEventInfo, PASSES_PER_SEC(60));

		}
	}

	void CWhDrMap::EndDungeonWarp()
	{
		if (GetMapSectree())
		{
			FExitAndGoTo f;
			GetMapSectree()->for_each(f);
		}

		long lMapIndex = GetMapIndex();
		SECTREE_MANAGER::instance().DestroyPrivateMap(GetMapIndex());
		Destroy();
		CWhDr::instance().Remove(lMapIndex);
		M2_DELETE(this);

	}

	void CWhDrMap::Start()
	{
		if (!GetMapSectree())
			EndDungeonWarp();

		else
		{
			SetWhiteEggNPC(Spawn((DWORD)WHITE_EGG_VNUM, 133, 134, 0));
		}
	}

	LPCHARACTER CWhDrMap::Spawn(DWORD dwVnum, int iX, int iY, int iDir, bool bSpawnMotion)
	{
		if (dwVnum == 0)
			return NULL;

		if (!GetMapSectree())
			return NULL;
	
		LPCHARACTER pkMob = CHARACTER_MANAGER::instance().SpawnMob(dwVnum, GetMapIndex(), GetMapSectree()->m_setting.iBaseX + iX * 100, GetMapSectree()->m_setting.iBaseY + iY * 100, 0, bSpawnMotion, iDir == 0 ? -1 : (iDir - 1) * 45);
		if (pkMob)
			sys_log(0, "<SnakeLair> Spawn: %s (map index: %d). x: %d y: %d", pkMob->GetName(), GetMapIndex(), (GetMapSectree()->m_setting.iBaseX + iX * 100), (GetMapSectree()->m_setting.iBaseY + iY * 100));
		
		return pkMob;
	}

#ifdef ENABLE_CONQUEROR_LEVEL
	void CWhDrMap::LoadSungmaAttr()
	{
		TSungmaAffectQueen aMap;

		for (BYTE by = 0; by < 4; by++)
		{
			if (GetDungeonLevel())
			{
				aMap.bDungeonLevel = true;
				aMap.iAffecType = iSungmaAffect[by][0];
				aMap.iValue = iSungmaAffect[by][2];
				m_vec_mapSungmaAffectQueen.push_back(aMap);
			}
			else
			{
				aMap.bDungeonLevel = false;
				aMap.iAffecType = iSungmaAffect[by][0];
				aMap.iValue = iSungmaAffect[by][1];
				m_vec_mapSungmaAffectQueen.push_back(aMap);
			}
		}
	}

	int CWhDrMap::GetSungmaValueAffect(int iAffect)
	{
		if (!m_vec_mapSungmaAffectQueen.empty())
		{
			std::vector<TSungmaAffectQueen>::iterator it = m_vec_mapSungmaAffectQueen.begin();
			while (it != m_vec_mapSungmaAffectQueen.end())
			{
				TSungmaAffectQueen & rRegion = *(it++);
				if (rRegion.bDungeonLevel == GetDungeonLevel())
					if(rRegion.iAffecType == iAffect)
						return rRegion.iValue;
			}
		}
	}

#endif

	/*
		Global Part
	*/

	void CWhDr::Initialize()
	{
		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}

	void CWhDr::Destroy()
	{
		itertype(m_dwRegGroups) iter = m_dwRegGroups.begin();
		for (; iter != m_dwRegGroups.end(); ++iter)
		{
			CWhDrMap* pMap = iter->second;
			SECTREE_MANAGER::instance().DestroyPrivateMap(pMap->GetMapIndex());
			pMap->Destroy();
			M2_DELETE(pMap);
		}
		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}


	void CWhDr::Remove(long lMapIndex)
	{
		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex);

		if (iter != m_dwRegGroups.end())
		{
			m_dwRegGroups.erase(iter);
		}

		return;
	}

	bool CWhDr::Access(LPCHARACTER pChar, bool bHard)
	{
		if (!pChar)
			return false;

		long lNormalMapIndex = (long)MAP_INDEX;
		PIXEL_POSITION pos = GetXYZ(), posSub = GetSubXYZ();

		if (!pos.x)
		{
			LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap((long)(WhiteDragon::MAP_INDEX));
			if (pkSectreeMap)
				SetXYZ(pkSectreeMap->m_setting.iBaseX, pkSectreeMap->m_setting.iBaseY, 0);
		}
			
		if (!posSub.x)
		{
			LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap((long)(WhiteDragon::SUBMAP_INDEX));
			if (pkSectreeMap)
				SetSubXYZ(pkSectreeMap->m_setting.iBaseX + 82*100, pkSectreeMap->m_setting.iBaseY + 639*100, 0);
		}

		long lMapIndex = SECTREE_MANAGER::instance().CreatePrivateMap(lNormalMapIndex);

		if (!lMapIndex)
		{
			pChar->ChatPacket(CHAT_TYPE_INFO, "An error ocurred during map creation.");
			return false;
		}

		CWhDrMap *pMap;

		if (bHard)
			pMap = M2_NEW CWhDrMap(lMapIndex, true);
		else
			pMap = M2_NEW CWhDrMap(lMapIndex, false);

		if (pMap)
		{
			m_dwRegGroups.insert(std::make_pair(lMapIndex, pMap));

			PIXEL_POSITION mPos;
			if (!SECTREE_MANAGER::instance().GetRecallPositionByEmpire((int)(MAP_INDEX), 0, mPos))
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, "Sectree Error get recall position.");
				return true;
			}

			pMap->SetDungeonStep(1);

			LPPARTY pParty = pChar->GetParty();

			if (pParty)
			{
				pMap->SetParty(pParty);

				FPartyCHCollector f;
				pChar->GetParty()->ForEachOnMapMember(f, pChar->GetMapIndex());
				std::vector <DWORD>::iterator it;
				for (it = f.pPlayerID.begin(); it != f.pPlayerID.end(); it++)
				{
					LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(*it);
					if(pkChr)
					{
						pkChr->SaveExitLocation();
						pkChr->WarpSet(mPos.x, mPos.y, pMap->GetMapIndex());
					}
				}
			}
			else
			{
				if(pChar)
				{
					pChar->SaveExitLocation();
					pChar->WarpSet(mPos.x, mPos.y, pMap->GetMapIndex());
				}
			}
			return true;
		}
	}

	bool CWhDr::IsWhiteMap(long lMapIndex)
	{
		long lMinIndex = (long)(MAP_INDEX) * 10000, lMaxIndex = (long)(MAP_INDEX) * 10000 + 10000;
		if (((lMapIndex >= lMinIndex) && (lMapIndex <= lMaxIndex)) || (lMapIndex == (long)(MAP_INDEX)))
			return true;
		
		return false;
	}

#ifdef ENABLE_CONQUEROR_LEVEL
	int CWhDr::GetSungmaValueAffectDungeon(LPCHARACTER pkChar, int iAffect)
	{
		if (!pkChar)
			return 0;

		if (!pkChar->IsPC())
			return 0;

		long lMapIndex = pkChar->GetMapIndex();
		
		if (lMapIndex < 1000)
			return 0;

		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return 0;

		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (pMap)
			return pMap->GetSungmaValueAffect(iAffect);

		return 0;
	}
#endif


	void CWhDr::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{
		if ((!pkMonster) || (!pKiller))
			return;

		long lMapIndex = pKiller->GetMapIndex();
		
		if (lMapIndex < 1000)
			return;
	
		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (!pMap)
			return;
		
		pMap->OnKill(pkMonster, pKiller);

		return;
	}

	void CWhDr::StartDungeon(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;
		
		long lMapIndex = pkChar->GetMapIndex();
		
		if (lMapIndex < 1000)
			return;
	
		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return;
		
		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
		{
			pMap->StartDungeon(pkChar);
		}
	}

	void CWhDr::Start(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;
		
		long lMapIndex = pkChar->GetMapIndex();
		
		if (lMapIndex < 1000)
			return;
		
		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
			pMap->StartDungeon(pkChar);
	}

};
