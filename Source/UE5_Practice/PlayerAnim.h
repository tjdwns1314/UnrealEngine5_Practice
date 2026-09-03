// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PRACTICE_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public :
	void NativeInitializeAnimation();
	void NativeUpdateAnimation(float DeltaSeconds);
	
public :
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	TObjectPtr<class ACharacter> Character;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float GroundSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float Direction = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool bShouldMove = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool bIsFalling = false;

	// 재생할 공격 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly,Category=PlayerAnim)
	class UAnimMontage* attackAnimMontage;

	// 공격 애니메이션 재생 함수
	void PlayAttackAnim();



	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool BIsRunShooting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	class ATPSPlayer* tpsPlayer;

};
