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


UPlayerFire::UPlayerFire()
{
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundCue'/Game/Assets/MilitaryWeapSilver/Sound/Rifle/Cues/RifleA_Fire_Cue.RifleA_Fire_Cue'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
}

void UPlayerFire::BeginPlay()
{
	Super::BeginPlay();
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	tpsCamComp = me->tpsCamComp;
	gunMeshComp = me->gunMeshComp;
	sniperGunComp = me->sniperGunComp;

}

void UPlayerFire::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &UPlayerFire::InputFire);

	PlayerInput->BindAction(ia_ChangeGun, ETriggerEvent::Started, this, &UPlayerFire::ChangeGun);

	PlayerInput->BindAction(ia_SniperAim, ETriggerEvent::Started, this, &UPlayerFire::SniperAim);
	PlayerInput->BindAction(ia_SniperAim, ETriggerEvent::Completed, this, &UPlayerFire::SniperAim);
}

void UPlayerFire::ChangeGun()
{
	bUsingSniperGun = !bUsingSniperGun;

	if (bUsingSniperGun)me->OnUsingGrenade(false);
	else me->OnUsingGrenade(true);

	gunMeshComp->SetVisibility(!bUsingSniperGun);
	sniperGunComp->SetVisibility(bUsingSniperGun);
}
void UPlayerFire::SniperAim(const FInputActionValue& inputValue)
{
	if (!bUsingSniperGun)
	{
		return;
	}
	if (bSniperAim == false)
	{
		//1.스나이퍼 조준 모드 활성화
		bSniperAim = true;
		//2.스나이퍼조준 UI 등록
		_sniperUI->AddToViewport();
		// 3. 카메라의 시야각 Field Of View 설정
		tpsCamComp->SetFieldOfView(45.0f);
	}
	else
	{
		//1.스나이퍼 조준 모드 비활성화
		bSniperAim = false;
		//2.스나이퍼 조준 UI 화면에서 제거
		_sniperUI->RemoveFromParent();
		// 3. 카메라의 시야각 원래대로 복원
		tpsCamComp->SetFieldOfView(90.0f);
	}
}

FVector UPlayerFire::GetMuzzleFlashLocation() const
{
	USkeletalMeshComponent* CurrentGun = bUsingSniperGun
		? sniperGunComp
		: gunMeshComp;

	if (CurrentGun)
	{
		return CurrentGun->GetSocketLocation(TEXT("MuzzleFlash"));
	}

	return FVector::ZeroVector;
}



void UPlayerFire::InputFire(const FInputActionValue& inputValue)
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	if (!bUsingSniperGun)
	{
		// 총알 발사 처리
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("MuzzleFlash"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else
	{
		// LineTrace 의 시작 위치
		FVector startPos = tpsCamComp->GetComponentLocation();
		// LineTrace 의 종료 위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// LineTrace 의 충돌 정보를 담을 변수
		FHitResult hitInfo;
		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;
		// 자기 자신(플레이어)는 충돌에서 제외
		params.AddIgnoredActor(me);
		// Channel 필터를 이용한 LineTrace 충돌 검출(충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_GameTraceChannel1, params);
		// LineTrace가 부딪혔을 때
		// 탄흔 흔적 데칼 
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
			BulletDecalMaterial,	// 데칼 머티리얼 자체를 변수로
			DecalSize,	// 사이즈는 원하는 데칼 크기
			hitInfo.ImpactPoint,
			hitInfo.ImpactNormal.Rotation(),
			DecalLifetime);	// 탄흔이 몇초동안 유지되어야 하는지

		Decal->SetFadeScreenSize(0); // 화면 크기에 따른 페이드 설정

		if (bHit)
		{
			if (BeamParticles)
			{
				UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					BeamParticles,  // UNiagaraSystem* 타입
					GetMuzzleFlashLocation(),
					FRotator::ZeroRotator,
					FVector(1.0f, 1.0f, 1.0f),  // Scale
					true,  // AutoDestroy
					true,  // AutoActivate
					ENCPoolMethod::AutoRelease  // Pooling 방식
				);

				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
					NiagaraComp,
					FName("ImpactPositions"),  // Niagara 변수 이름
					TArray<FVector>({ hitInfo.ImpactPoint })  // ImpactPoint를 포함하는 배열
				);

				NiagaraComp->SetVariableBool(FName(TEXT("Trigger")), true);
			}
			// 충돌 처리 -> 총알 파편 효과 재생
			FTransform bulletTrans;
			// 부딪힌 위치 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// 총알 파편 효과 인스턴스 생성
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				bulletEffectFactory,
				hitInfo.ImpactPoint
			);
			auto hitComp = hitInfo.GetComponent();
			// 1.  만약 컴포넌트에 물리가 적용되어 있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				//2. 조준한 방향이 필요
				FVector dir = (endPos - startPos).GetSafeNormal();
				// 날려 버릴 힘(F=ma)
				FVector force = dir * hitComp->GetMass() * 500000;
				// 3.그 방향으로 날려 버리고 싶다.
				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}
			// 부딪힌 대상이 적인지 판단하기
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
	BShooting = true;

	ThisDelegate.BindLambda([this]() {
		BShooting = false;
		});

	GetWorld()->GetTimerManager().ClearTimer(ThisHandle);

	GetWorld()->GetTimerManager().SetTimer(
		ThisHandle,
		ThisDelegate,
		TransitionTime, // 실행 주기
		false // 반복 여부
	);

}
