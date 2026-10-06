// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MyCharacterStatDataAsset.h"
#include "US_CharacterStats.h"
#include "Engine/DataTable.h"

//void UMyCharacterStatDataAsset::UpdateCharacterStats(int32 CharacterLevel)
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
//}
