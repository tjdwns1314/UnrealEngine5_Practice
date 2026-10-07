// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TPSProjectGameMode.h"
#include "UE5_Practice.h"
#include"Game/US_GameState.h"
#include"Game/US_PlayerState.h"
#include "UObject/ConstructorHelpers.h"

ATPSProjectGameMode::ATPSProjectGameMode()
{
	PRINT_LOG(TEXT("My Log : %s"), TEXT("TPS project!!"));
	GameStateClass = AUS_GameState::StaticClass();

	PlayerStateClass = AUS_PlayerState::StaticClass();

}
