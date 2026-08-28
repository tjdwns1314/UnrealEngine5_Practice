// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class UE5_PRACTICE_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
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
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera)
	class UCameraComponent* tpsCamComp;

	UPROPERTY(EditDefaultsOnly, Category ="Input")
	class UInputMappingContext* imc_TPS;
	UPROPERTY(EditDefaultsOnly, Category ="Input")
	class UInputAction* ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category ="Input")
	class UInputAction* ia_Turn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;



	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float walkSpeed = 600;
	FVector direction;

	void Move(const struct FInputActionValue& inputValue);
	void Turn(const struct FInputActionValue& inputValue);
	void LookUp(const struct FInputActionValue& inputValue);
	void InputJump(const struct FInputActionValue& inputValue);

	void PlayerMove();


	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	//총알 공장
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Fire;
	// 총알 발사 처리함수
	void InputFire(const struct FInputActionValue& inputValue);

	// 스나이퍼건 스태틱 메시 추가
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* sniperGunComp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_ChangeGun;

	bool bUsingSniperGun = false;

	void ChangeGun();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_SniperAim;

	void SniperAim(const struct FInputActionValue& inputValue);

	// 스나이퍼 조준 중인지 여부
	bool bSniperAim = false;

	// 스나이퍼 UI 위젯 공장
	UPROPERTY(EditDefaultsOnly,Category=SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	// 스나이퍼 UI 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _sniperUI;

	// 총알 파편 효과 공장
	UPROPERTY(EditAnywhere, Category=BulletEffect)
	class UNiagaraSystem* bulletEffectFactory;
};
