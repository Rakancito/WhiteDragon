#include "../../common/service.h"


#include "../../common/length.h"
#include "../../common/item_length.h"
#include "../../common/tables.h"
#include "guild.h"
#include "char_manager.h"
#include "sectree_manager.h"


namespace WhiteDragon
{

	enum eWhiteDragonConfig
	{
		MAP_INDEX = 141,
		SUBMAP_INDEX = 139,
		WHITE_EGG_VNUM = 4030,
		BOSS_EASY_VNUM = 6791,
		BOSS_VNUM = 6790,
		TIME_LIMIT_DUNGEON = 3600,
	};

	extern int iSungmaAffect[4][3];
#ifdef ENABLE_CONQUEROR_LEVEL
	typedef struct SSungmaAffectMapQueen{
		bool bDungeonLevel;
		int iAffecType;
		int iValue;
	} TSungmaAffectQueen;
#endif

	class CWhDrMap
	{
		public:
			CWhDrMap(long lMapIndex, bool bHard);
			~CWhDrMap();

			void Destroy();

#ifdef ENABLE_CONQUEROR_LEVEL
			void	LoadSungmaAttr();
			int	GetSungmaValueAffect(int iAffect);
#endif

			void StartDungeon(LPCHARACTER pkChar);
			void OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller);
			void EndDungeonWarp();
			void Start();

			LPCHARACTER Spawn(DWORD dwVnum, int iX, int iY, int iDir, bool bSpawnMotion = false);

			/*
				Get and Set
			*/

			void	SetDungeonStep(BYTE bStep);
			BYTE	GetDungeonStep()	{return dungeon_step;};


			void	SetDungeonLevel(bool bLevel)	{dungeon_level = bLevel;};
			bool	GetDungeonLevel()	{return dungeon_level;};

			void	SetParty(LPPARTY pParty)	{pPartyReg = pParty;};
			LPPARTY & GetParty()			{return pPartyReg;};

			void 	SetMapSectree(LPSECTREE_MAP pkSectree) {pkSectreeMap = pkSectree;};
			LPSECTREE_MAP & GetMapSectree()		{return pkSectreeMap;};

			void	SetMapIndex(long lMapIndex)	{map_index=lMapIndex;};
			long	GetMapIndex()			{return map_index;};

			void	SetWhiteEggNPC(LPCHARACTER pkWhiteEgg)	{pkWhiteEggNPC = pkWhiteEgg;};
			LPCHARACTER & GetWhiteEggNPC()		{return pkWhiteEggNPC;};

			long map_index;
			LPSECTREE_MAP	pkSectreeMap;
			LPPARTY pPartyReg;
			BYTE	dungeon_step;
			bool	dungeon_level;

			LPCHARACTER 	pkWhiteEggNPC;

		protected:
			LPEVENT	e_SpawnEvent, e_LimitEvent;

#ifdef ENABLE_CONQUEROR_LEVEL
		private:
			std::vector<TSungmaAffectQueen>		m_vec_mapSungmaAffectQueen;
#endif

	};

	class CWhDr : public singleton<CWhDr>
	{
		public:
			void Initialize();
			void Destroy();

			void Remove(long lMapIndex);
			bool Access(LPCHARACTER pChar, bool bHard);

#ifdef ENABLE_CONQUEROR_LEVEL
			int	GetSungmaValueAffectDungeon(LPCHARACTER pkChar, int iAffect);
#endif

			bool IsWhiteMap(long lMapIndex);
			void OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller);
			void StartDungeon(LPCHARACTER pkChar);
			void Start(LPCHARACTER pkChar);

			//Set
			void	SetSubXYZ(long lX, long lY, long lZ)	{lSubMapPos.x = lX, lSubMapPos.y = lY, lSubMapPos.z = lZ;}
			void	SetXYZ(long lX, long lY, long lZ)	{lMapCenterPos.x = lX, lMapCenterPos.y = lY, lMapCenterPos.z = lZ;}
			
			//Get
			const PIXEL_POSITION &	GetSubXYZ() const	{return lSubMapPos;}
			const PIXEL_POSITION &	GetXYZ() const	{return lMapCenterPos;}

		private:
			std::map<long, CWhDrMap*> m_dwRegGroups;
			PIXEL_POSITION lMapCenterPos, lSubMapPos;
	};
};