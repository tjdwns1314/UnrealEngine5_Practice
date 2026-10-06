// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MyDataSubsystem.h"
#include "UE5_Practice.h"
#include "MyInputDataAsset.h"
#include "MyDataConfigAsset.h"
#include "System/MyGameInstance.h"
#include "MyCharacterStatDataAsset.h"

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

FUS_CharacterStats* UMyDataSubsystem::ReturnUpdatedCharacterStats(int32 CharacterLevel)
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI == nullptr || GI->DataConfig == nullptr || GI->DataConfig->DA_Stats == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DataConfig 또는 DA_Stats가 설정되지 않았습니다!"));
		return nullptr;
	}

	UMyCharacterStatDataAsset* DA_Stats = Cast<UMyCharacterStatDataAsset>(GI->DataConfig->DA_Stats);
	if (DA_Stats == nullptr || DA_Stats->CharacterDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterDataTable이 비어있습니다!"));
		return nullptr;
	}

	// 데이터 테이블 행 이름 형식(level_01, level_02)에 맞춰 문자열 생성
	FString RowNameStr = FString::Printf(TEXT("level_%02d"), CharacterLevel);

	FUS_CharacterStats* FoundRow = DA_Stats->CharacterDataTable->FindRow<FUS_CharacterStats>(
		FName(*RowNameStr),
		TEXT("CharacterStats"));

	if (FoundRow == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("데이터 테이블에서 행 [%s]을 찾지 못했습니다!"), *RowNameStr);
	}

	return FoundRow;
}

//void UMyDataSubsystem::UpdateCharacterStats(int32 CharacterLevel)
//{
//	CharacterStats = nullptr;
//	if (CharacterDataTable)
//	{
//		TArray<FUS_CharacterStats*> CharacterStatsRows;
//		CharacterDataTable->GetAllRows<FUS_CharacterStats>(TEXT("US_Character"),
//			CharacterStatsRows);
//
//		if (CharacterStatsRows.Num() > 0)
//		{
//			const int32 NewCharacterLevel = FMath::Clamp(CharacterLevel, 1,
//				CharacterStatsRows.Num());
//			CharacterStats = CharacterStatsRows[NewCharacterLevel - 1];
//		}
//	}

