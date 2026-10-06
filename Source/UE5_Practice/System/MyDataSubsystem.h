// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "US_CharacterStats.h"
#include "MyDataSubsystem.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class UE5_PRACTICE_API UMyDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	

public :
	const UInputAction* FindInputActionByTag(const FGameplayTag& InputTag) const;
	FUS_CharacterStats* ReturnUpdatedCharacterStats(int32 CharacterLevel);
};
