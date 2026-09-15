// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInputBindingDelegate, class UEnhancedInputComponent*);

class UPlayerMove;
class UPlayerFire;

UCLASS()
class UE5_PRACTICE_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:

	// 입력 바인딩 델리게이트
	FInputBindingDelegate onInputBindingDelegate;

	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	// 현재 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 hp;
	// 초기 hp 값
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 initialHp = 1;

	// 피격 당했을 때 처리
	UFUNCTION(BlueprintCallable, Category = Health)
	void OnHitEvent();

	// 게임 오버될 때 호출될 함수
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Health)
	void OnGameOver();

	// 총 바꿀 때 호출되는 이벤트 함수
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Health)
	void OnUsingGrenade(bool isGrenade);




	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera)
	class UCameraComponent* tpsCamComp;

	UPROPERTY(EditDefaultsOnly, Category ="Input")
	class UInputMappingContext* imc_TPS;

	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	// 스나이퍼건 스태틱 메시 추가
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* sniperGunComp;



	public:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
		UPlayerMove* playerMove;

		// 블프에서 만들었음
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
		//UPlayerFire* playerFire;

};