// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include"Kismet/GameplayStatics.h"
#include "Player/PlayerAnim.h"
#include "NiagaraFunctionLibrary.h"
#include"Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include"Enemy/EnemyFSM.h"
#include "Player/TPSPlayer.h"
#include "Bullet.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundCue'/Game/Assets/MilitaryWeapSilver/Sound/Rifle/Cues/RifleA_Fire_Cue.RifleA_Fire_Cue'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}

	// 총 스켈레탈 메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	//// 부모 컴포넌트를 Mesh 컴포넌트로 설정
	//gunMeshComp->SetupAttachment(me->GetMesh(), TEXT("hand_rSocket"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	tpsPlayer = Cast<ATPSPlayer>(GetOwner());
	if (tpsPlayer)
	{
		tpsCamComp = tpsPlayer->tpsCamComp;
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




void AWeapon::InputFire(const FInputActionValue& inputValue)
{
	if (IsHidden())
		return;

	if(bulletSound)
		UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);
	
	auto controller = GetWorld()->GetFirstPlayerController();
	if(controller)
		controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(tpsPlayer->GetMesh()->GetAnimInstance());
	if(anim)
		anim->PlayAttackAnim(shootingAnimMontage);

	if (isBullet)
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
		params.AddIgnoredActor(tpsPlayer);
		// Channel 필터를 이용한 LineTrace 충돌 검출(충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_GameTraceChannel1, params);
		// LineTrace가 부딪혔을 때
		if (bHit)
		{
			if (BulletDecalMaterial)
			{

				// 탄흔 흔적 데칼 
				UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
					BulletDecalMaterial,	// 데칼 머티리얼 자체를 변수로
					DecalSize,	// 사이즈는 원하는 데칼 크기
					hitInfo.ImpactPoint,
					hitInfo.ImpactNormal.Rotation(),
					DecalLifetime);	// 탄흔이 몇초동안 유지되어야 하는지
				if (Decal)
					Decal->SetFadeScreenSize(0); // 화면 크기에 따른 페이드 설정
			}
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
				if (NiagaraComp)
				{
					UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
						NiagaraComp,
						FName("ImpactPositions"),  // Niagara 변수 이름
						TArray<FVector>({ hitInfo.ImpactPoint })  // ImpactPoint를 포함하는 배열
					);
					NiagaraComp->SetVariableBool(FName(TEXT("Trigger")), true);
				}
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


FVector AWeapon::GetMuzzleFlashLocation()
{
	if (gunMeshComp)
		return gunMeshComp->GetSocketLocation(TEXT("MuzzleFlash"));
	else
		return FVector::ZeroVector;
}

