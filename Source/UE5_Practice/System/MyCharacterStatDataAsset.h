// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyCharacterStatDataAsset.generated.h"

class UDataTable;
struct FUS_CharacterStats;

/**
 * 
 */
UCLASS()
class UE5_PRACTICE_API UMyCharacterStatDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Data")
	class UDataTable* CharacterDataTable;

	//struct FUS_CharacterStats* CharacterStats;

	//FORCEINLINE const FUS_CharacterStats* GetCharacterStats() const { return CharacterStats; }

};
