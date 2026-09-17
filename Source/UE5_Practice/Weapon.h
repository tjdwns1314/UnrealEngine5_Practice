// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class UE5_PRACTICE_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();


	// 총알 발사 처리함수
	void InputFire(const struct FInputActionValue& inputValue);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	// 플레이어
	UPROPERTY()
	class ATPSPlayer* tpsPlayer;

	// --- 카메라 컴포넌트 ---
	//UPROPERTY(VisibleAnywhere, Category = Camera)
	//class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* tpsCamComp;

	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	//총알 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// --- 무기 및 조준 상태 ---
	bool bUsingSniperGun = true;

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

	// 총흔
	UPROPERTY(EditDefaultsOnly, Category = "MyGame")
	TObjectPtr<class UMaterialInterface> BulletDecalMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	FVector DecalSize = FVector(5, 5, 5);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	float DecalLifetime = 2;

	// 총 트레이
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> BeamParticles;

	UFUNCTION()
	FVector GetMuzzleFlashLocation();

	// 주무기인지
	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	bool isPrimary;

	// 발사체가 나가는지 라인트레이스를 쏘는지 
	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	bool isBullet = false;

	// 총쏠때마다 크로스헤어
	//UPROPERTY(EditDefaultsOnly, Category = Shooting)
	
	// 


	// 총을 쐈는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	bool BShooting = false;

	// 타이머 핸들 선언
	FTimerHandle ThisHandle;

	// 타이머 델리게이트 선언
	FTimerDelegate ThisDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	float TransitionTime = 2.0f;

	// 총마다 다른 애님몽타주 선언
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
	UAnimMontage* shootingAnimMontage;
};
