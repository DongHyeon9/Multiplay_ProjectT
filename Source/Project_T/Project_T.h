#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(PTT, Log, All);
#define PTT_LOG_CALLINFO (FString(__FUNCTION__)+TEXT("(")+FString::FromInt(__LINE__)+TEXT(")"))
#define PTT_LOG(Verbosity,Format,...) UE_LOG(PTT,Verbosity,TEXT("%s %s"),*PTT_LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))
#define PRINTSTR(Key,Time,Color,Format,...) GEngine->AddOnScreenDebugMessage(Key,Time,Color,FString::FromInt(Key) + FString(TEXT(" : ")) + FString::Printf(Format,##__VA_ARGS__))
#define GET_ENUM_DISPLAY_NAME(EnumType, Value) StaticEnum<EnumType>()->GetDisplayNameTextByValue(static_cast<uint8>(Value)).ToString())