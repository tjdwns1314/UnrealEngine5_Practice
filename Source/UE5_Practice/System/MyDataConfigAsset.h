// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyInputDataAsset.h"
#include "MyDataConfigAsset.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PRACTICE_API UMyDataConfigAsset : public UDataAsset
{
	GENERATED_BODY()

public :
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMyInputDataAsset> DA_Input;
	
};
