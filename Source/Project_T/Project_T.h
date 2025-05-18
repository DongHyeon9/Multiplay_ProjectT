#pragma once

#include "CoreMinimal.h"

#define LOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define REMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define SUBLOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define SUBREMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))
#define NETMODE_INFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), UE::GetPlayInEditorID()) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 

DECLARE_LOG_CATEGORY_EXTERN(PTT, Log, All);
#define PTT_LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
#define PTT_LOG(Verbosity,Format,...) UE_LOG(PTT,Verbosity,TEXT("%s %s"),PTT_LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))
#define PTT_NET_LOG(Verbosity,Format,...) UE_LOG(PTT,Verbosity,TEXT("[%s] [%s/%s] %s %s"),NETMODE_INFO,LOCAL_ROLE_INFO,REMOTE_ROLE_INFO,PTT_LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))
#define PTT_SUBNET_LOG(Verbosity,Format,...) UE_LOG(PTT,Verbosity,TEXT("[%s] [%s/%s] %s %s"),NETMODE_INFO,SUBLOCAL_ROLE_INFO,SUBREMOTE_ROLE_INFO,PTT_LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))
#define PRINTSTR(Key,Time,Color,Format,...) GEngine->AddOnScreenDebugMessage(Key,Time,Color,FString::FromInt(Key) + FString(TEXT(" : ")) + FString::Printf(Format,##__VA_ARGS__))
#define GET_ENUM_DISPLAY_NAME(EnumType, Value) StaticEnum<EnumType>()->GetDisplayNameTextByValue(static_cast<uint8>(Value)).ToString())

extern FName SESSION_NAME;
extern FString PLAYER_NAME_PREFIX;