// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMove.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PRACTICE_API UPlayerMove : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	// ==================== 함수 ====================

	// --- 생성 및 생명주기 ---
	UPlayerMove();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;


	// --- 입력 바인딩 ---
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;


	// --- 시점 회전 ---
	// 좌우 회전 입력 처리
	void Turn(const struct FInputActionValue& inputValue);

	// 상하 회전 입력 처리
	void LookUp(const struct FInputActionValue& inputValue);


	// --- 이동 및 달리기 ---
	void Move(const struct FInputActionValue& inputValue);

	void PlayerMove();

	// 달리기 이벤트 처리함수
	void InputRun();


	// --- 점프 ---
	void InputJump(const struct FInputActionValue& inputValue);


public:
	// ==================== 변수 ====================

	// --- 시점 입력 액션 ---
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;


	// --- 이동 입력 액션 ---
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Run;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;


	// --- 이동 방향 및 속도 ---
	FVector direction;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 300;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;


	// --- 발사 및 달리기 상태 ---
	// 총을 쐈는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	bool BShooting = false;

	// 달리고 있는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	bool BRunning = false;

	// 앞으로 총을 두는 상태가 필요한지 체크 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	bool BIsRunShooting = false;


	// --- 상태 전환 타이머 ---
	// 총 쏘고 전환시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	float TransitionTime = 5.0f;

	// 타이머 핸들 선언
	FTimerHandle ThisHandle;

	// 타이머 델리게이트 선언
	FTimerDelegate ThisDelegate;
};
