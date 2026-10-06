// Fill out your copyright notice in the Description page of Project Settings.


#include "US_PlayerState.h"
#include "Player/TPSPlayer.h"
#include "US_CharacterStats.h"
#include "Net/UnrealNetwork.h"
#include "System/MyDataSubsystem.h"
#include "Player/PlayerMove.h" // UPlayerMove 사용을 위해 추가

void AUS_PlayerState::UpdateCharacterStatsFromSubsystem()
{
	if (const auto Character = Cast<ATPSPlayer>(GetPawn()))
	{
		if (UMyDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UMyDataSubsystem>())
		{
			if (FUS_CharacterStats* NewStats = DataSubsystem->ReturnUpdatedCharacterStats(CharacterLevel))
			{
				Character->CharacterStats = NewStats;
				if (Character->playerMove)
				{
					Character->playerMove->SetCharacterStats(*NewStats);
				}
			}
		}
	}
}


void AUS_PlayerState::AddXp(int32 Value)
{
	Xp += Value;
	OnXpChanged.Broadcast(Xp);

	GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow,
		FString::Printf(TEXT("Total Xp: %d"), Value));

	if (const auto Character = Cast<ATPSPlayer>(GetPawn()))
	{
		if (Character->GetStats()->NextLevelXp < Xp)
		{
			GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Red, TEXT("Level Up!"));

			CharacterLevel++;
			if (UMyDataSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UMyDataSubsystem>())
			{
				Character->CharacterStats = DataSubsystem->ReturnUpdatedCharacterStats(CharacterLevel);
				UpdateCharacterStatsFromSubsystem();
			}
			OnCharacterLevelUp.Broadcast(CharacterLevel);
		}
	}
}

void AUS_PlayerState::OnRep_Xp(int32 OldValue) 
{
	OnXpChanged.Broadcast(Xp);
}

void AUS_PlayerState::OnRep_CharacterLevelUp(int32 OldValue) 
{
	UpdateCharacterStatsFromSubsystem();
	OnCharacterLevelUp.Broadcast(CharacterLevel);
}


void AUS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AUS_PlayerState, Xp, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AUS_PlayerState, CharacterLevel, COND_OwnerOnly);
}
