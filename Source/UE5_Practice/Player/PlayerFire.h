// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include <Components/InputComponent.h>
#include "PlayerFire.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(PlayerComponent),meta = (BlueprintSpawnableComponent))
class UE5_PRACTICE_API UPlayerFire : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	// ==================== 함수 ====================

	// --- 생성 및 초기화 ---
	UPlayerFire();

	virtual void BeginPlay() override;


	// --- 입력 바인딩 ---
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;


	// --- 발사 ---
	// 총알 발사 처리함수
	void InputFire(const struct FInputActionValue& inputValue);


	// --- 무기 전환 및 조준 ---
	void ChangeGun();

	void SniperAim(const struct FInputActionValue& inputValue);


public:
	// ==================== 변수 ====================

	// --- 카메라 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* tpsCamComp;


	// --- 무기 메시 ---
	// 스나이퍼건 스태틱 메시 추가
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* sniperGunComp;

	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;


	// --- 입력 액션 ---
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Fire;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_ChangeGun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_SniperAim;


	// --- 총알 생성 ---
	//총알 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;


	// --- 무기 및 조준 상태 ---
	bool bUsingSniperGun = false;

	// 스나이퍼 조준 중인지 여부
	bool bSniperAim = false;


	// --- 스나이퍼 UI ---
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	UPROPERTY()
	class UUserWidget* _sniperUI;


	// --- 발사 효과 및 사운드 ---
	// 총알 파편 효과 공장
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UNiagaraSystem* bulletEffectFactory;

	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	//총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;


	// --- 발사 및 달리기 상태 ---
	// 개선해볼것 - 총 발사 관련 
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

	UPROPERTY(EditDefaultsOnly, Category = "MyGame")
	TObjectPtr<class UMaterialInterface> BulletDecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	FVector DecalSize = FVector(5,5,5);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	float DecalLifetime = 2;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> BeamParticles;

	FVector GetMuzzleFlashLocation() const;
};
