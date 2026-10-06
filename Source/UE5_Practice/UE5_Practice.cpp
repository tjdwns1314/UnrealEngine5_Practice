// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_Practice.h"
#include "Modules/ModuleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, UE5_Practice, "UE5_Practice" );

DEFINE_LOG_CATEGORY(TPS);

void PrintLogWithRole(const AActor* WorldContextObject, FString Text, FLinearColor TextColor, float Duration)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			UKismetSystemLibrary::PrintString(WorldContextObject, Text, true, true, TextColor, Duration);
		}
		else
		{
			FString ModeMsg;
			switch (World->GetNetMode())
			{
			case NM_Standalone:
			{
				ModeMsg = TEXT("Server");
			}break;
			case NM_DedicatedServer:
			case NM_ListenServer:
			{
				ModeMsg = TEXT("Server");
			}break;
			case NM_Client:
			{
				ModeMsg = TEXT("Client");
			}break;
			default:
			{
				ModeMsg = TEXT("Unknown");
			}break;
			}
			Text.InsertAt(0, FString::Printf(TEXT("%s : "), *ModeMsg));

			GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor.ToFColor(true), Text);
		}
	}
}
