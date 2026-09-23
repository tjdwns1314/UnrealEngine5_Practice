// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MyInputDataAsset.generated.h"

USTRUCT()
struct FMyInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* InputAction;
};

/**
 * 
 */
UCLASS()
class UE5_PRACTICE_API UMyInputDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* IMC_TPS;

	UPROPERTY(EditDefaultsOnly)
	TArray<FMyInputAction> InputActions;
};
