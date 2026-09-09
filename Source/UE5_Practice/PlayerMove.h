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
	UPlayerMove();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;

	// 좌우 회전 입력 처리
	void Turn(const struct FInputActionValue& inputValue);
	// 상하 회전 입력 처리
	void LookUp(const struct FInputActionValue& inputValue);

	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;

	FVector direction;

	void Move(const struct FInputActionValue& inputValue);

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 300;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;

	void PlayerMove();


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Run;


	// 달리기 이벤트 처리함수
	void InputRun();



	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;

	void InputJump(const struct FInputActionValue& inputValue);

















	// 총을 쐈는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	bool BShooting = false;

	// 달리고 있는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	bool BRunning = false;

	// 앞으로 총을 두는 상태가 필요한지 체크 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	bool BIsRunShooting = false;

	// 총 쏘고 전환시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	float TransitionTime = 5.0f;

	// 타이머 핸들 선언
	FTimerHandle ThisHandle;

	// 타이머 델리게이트 선언
	FTimerDelegate ThisDelegate;
};
