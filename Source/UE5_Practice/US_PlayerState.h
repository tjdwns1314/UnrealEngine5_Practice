// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "US_PlayerState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnXpChanged, int32, NexXp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterLevelUp,int32, NewLevelXp);
UCLASS()
class UE5_PRACTICE_API AUS_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Xp, Category = "Experience")
	int32 Xp = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_CharacterLevelUp, Category = "Experience")
	int32 CharacterLevel = 1;

	UFUNCTION()
	void OnRep_Xp(int32 OldValue);

	UFUNCTION()
	void OnRep_CharacterLevelUp(int32 OldValue);

public:
	UFUNCTION(blueprintCallable,Category = "Experience")
	void AddXp(int32 Value);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnXpChanged OnXpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterLevelUp OnCharacterLevelUp;

	void UpdateCharacterStatsFromSubsystem();

	// AddXp Broadcast → 서버 컴퓨터한테 알림
	// OnRep Broadcast →** 내 PC(클라이언트)** 한테 알림
};
