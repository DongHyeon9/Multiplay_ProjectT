#include "Project_T.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Project_T, "Project_T" );
DEFINE_LOG_CATEGORY(PTT);

const FName SESSION_NAME{ TEXT("ProjectTSession") };
const FString PLAYER_NAME_PREFIX{ TEXT("PNPF.") };
const int32 MAX_PLAYER_COUNT{ 2 };
