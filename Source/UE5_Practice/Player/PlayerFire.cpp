// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFire.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "Blueprint/UserWidget.h"
#include"Kismet/GameplayStatics.h"
#include"Enemy/EnemyFSM.h"
#include"Camera/CameraComponent.h"
#include"PlayerAnim.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Weapon.h"
#include "Engine/SkeletalMeshSocket.h"


UPlayerFire::UPlayerFire()
{
	// 총알 사운드 가져오기
	//ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundCue'/Game/Assets/MilitaryWeapSilver/Sound/Rifle/Cues/RifleA_Fire_Cue.RifleA_Fire_Cue'"));
	//if (tempSound.Succeeded())
	//{
	//	bulletSound = tempSound.Object;
	//}
}

void UPlayerFire::BeginPlay()
{
	Super::BeginPlay();
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	//tpsCamComp = me->tpsCamComp;
	//gunMeshComp = me->gunMeshComp;
	//sniperGunComp = me->sniperGunComp;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = me; // 스폰시킨 캐릭터를 Owner로 설정하고 Spawn시킨다.
	currentWeapon = GetWorld()->SpawnActor<AWeapon>(startingWeapon, me->GetActorTransform(), SpawnParams);
	anotherWeapon = GetWorld()->SpawnActor<AWeapon>(startingPistolWeapon, me->GetActorTransform(), SpawnParams);

	const USkeletalMeshSocket* HandSocket = me->GetMesh()->GetSocketByName(FName("hand_rSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(currentWeapon, me->GetMesh());
		HandSocket->AttachActor(anotherWeapon, me->GetMesh());
	}
	anotherWeapon->SetActorHiddenInGame(true);
}

void UPlayerFire::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &UPlayerFire::InputFire);
	PlayerInput->BindAction(ia_ChangeGun, ETriggerEvent::Started, this, &UPlayerFire::ChangeGun);
	PlayerInput->BindAction(ia_SniperAim, ETriggerEvent::Started, this, &UPlayerFire::SniperAim);
	PlayerInput->BindAction(ia_SniperAim, ETriggerEvent::Completed, this, &UPlayerFire::SniperAim);
}

//void UPlayerFire::ChangeGun()
//{
//	bUsingSniperGun = !bUsingSniperGun;
//
//	if (bUsingSniperGun)me->OnUsingGrenade(false);
//	else me->OnUsingGrenade(true);
//
//	gunMeshComp->SetVisibility(!bUsingSniperGun);
//	sniperGunComp->SetVisibility(bUsingSniperGun);
//}
//void UPlayerFire::SniperAim(const FInputActionValue& inputValue)
//{
//	if (!bUsingSniperGun)
//	{
//		return;
//	}
//	if (bSniperAim == false)
//	{
//		//1.스나이퍼 조준 모드 활성화
//		bSniperAim = true;
//		//2.스나이퍼조준 UI 등록
//		_sniperUI->AddToViewport();
//		// 3. 카메라의 시야각 Field Of View 설정
//		tpsCamComp->SetFieldOfView(45.0f);
//	}
//	else
//	{
//		//1.스나이퍼 조준 모드 비활성화
//		bSniperAim = false;
//		//2.스나이퍼 조준 UI 화면에서 제거
//		_sniperUI->RemoveFromParent();
//		// 3. 카메라의 시야각 원래대로 복원
//		tpsCamComp->SetFieldOfView(90.0f);
//	}
//}

//FVector UPlayerFire::GetMuzzleFlashLocation() const
//{
//	USkeletalMeshComponent* CurrentGun = bUsingSniperGun
//		? sniperGunComp
//		: gunMeshComp;
//
//	if (CurrentGun)
//	{
//		return CurrentGun->GetSocketLocation(TEXT("MuzzleFlash"));
//	}
//
//	return FVector::ZeroVector;
//}



void UPlayerFire::InputFire(const FInputActionValue& inputValue)
{
	if (currentWeapon) {
		currentWeapon->InputFire(inputValue);
	}
}
void UPlayerFire::ChangeGun() // 주무기 보조무기 바꾸기
{
	if (!currentWeapon || !anotherWeapon)
	{
		return;
	}

	// 기존 무기를 숨기고, 교체된 현재 무기를 보이게 한다.
	currentWeapon->SetActorHiddenInGame(true);
	tempWeapon = currentWeapon;
	currentWeapon = anotherWeapon;
	anotherWeapon = tempWeapon;
	currentWeapon->SetActorHiddenInGame(false);

	if (currentWeapon->isPrimary)
	{
		me->OnUsingGrenade(true);
	}
	else
	{
		me->OnUsingGrenade(false);
	}

}

void UPlayerFire::SniperAim(const FInputActionValue & inputValue)
{
	if (currentWeapon)
	{
		if (!currentWeapon->bUsingSniperGun)
			return;
		if (!currentWeapon->bSniperAim) //조준을 안했을 때 조준을 켜기
		{
			// 스나이퍼 조준 띄우기
			//1.스나이퍼 조준 모드 활성화
			currentWeapon->bSniperAim = true;
			//2.스나이퍼조준 UI 등록
			_sniperUI->AddToViewport();
			// 3. 카메라의 시야각 Field Of View 설정
			me->tpsCamComp->SetFieldOfView(45.0f);
		}
		else
		{
			//1.스나이퍼 조준 모드 비활성화
			currentWeapon->bSniperAim = false;
			//2.스나이퍼 조준 UI 화면에서 제거
			_sniperUI->RemoveFromParent();
			// 3. 카메라의 시야각 원래대로 복원
			me->tpsCamComp->SetFieldOfView(90.0f);
		}
	}
}
