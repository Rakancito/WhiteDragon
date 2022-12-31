//Search

#ifdef ENABLE_QUEEN_NETHIS
#include "SnakeLair.h"
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
#include "WhiteDragon.h"
#endif

//Search

#ifdef ENABLE_QUEEN_NETHIS
	SnakeLair::CSnk SnakeLair_manager;
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
	WhiteDragon::CWhDr WhiteDragon_manager;
#endif

//Search

#ifdef ENABLE_QUEEN_NETHIS
		SnakeLair_manager.Initialize();
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
		WhiteDragon_manager.Initialize();
#endif

//Search

#ifdef ENABLE_QUEEN_NETHIS
		sys_log(0, "<shutdown> Destroying SnakeLair_manager.");
		SnakeLair_manager.Destroy();
#endif

//Add after

#ifdef ENABLE_WHITE_DRAGON
		sys_log(0, "<shutdown> Destroying WhiteDragon_manager.");
		WhiteDragon_manager.Destroy();
#endif