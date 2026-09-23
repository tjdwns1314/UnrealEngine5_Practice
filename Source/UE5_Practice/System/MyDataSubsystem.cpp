// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MyDataSubsystem.h"
#include "UE5_Practice.h"
#include "MyInputDataAsset.h"
#include "MyDataConfigAsset.h"
#include "System/MyGameInstance.h"

const UInputAction* UMyDataSubsystem::FindInputActionByTag(const FGameplayTag& InputTag) const
{
	UDataAsset* InputData = Cast<UMyGameInstance>(GetGameInstance())->DataConfig->DA_Input;
	if (InputData == nullptr)
		return nullptr;

	UMyInputDataAsset* DA_Input = Cast<UMyInputDataAsset>(InputData);

	for (const FMyInputAction& Action : DA_Input->InputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	PRINT_LOG(TEXT("Can't find InputAction for InputTag [%s]"), *InputTag.ToString());

	return nullptr;
}
