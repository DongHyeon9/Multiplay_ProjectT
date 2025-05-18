#include "Project_T.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Project_T, "Project_T" );
DEFINE_LOG_CATEGORY(PTT);

FName SESSION_NAME{ TEXT("ProjectTSession") };
FString PLAYER_NAME_PREFIX{ TEXT("PNPF.") };