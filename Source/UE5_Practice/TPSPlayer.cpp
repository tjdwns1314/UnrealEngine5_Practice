// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "EnemyFSM.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "PlayerAnim.h"


// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1.���̷�Ż �޽� �����͸� �ҷ����� �ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("Skeleton'/Game/Assets/ThirdPersonTemplate/Characters/Mannequins/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// 2.Mesh ������Ʈ�� ��ġ�� ȸ�� ���� �����ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	// 3.TPS ī�޶� ���̰� �ʹ�.
	// 3-1. SpringArm ������Ʈ ���̱�
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	//springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->SocketOffset = FVector(0, 70, 90);
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;
	//3-2. Camera ������Ʈ ���̱� 
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;

	JumpMaxCount = 2;

	// 4. 총 스켈레탈 메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1 부모 컴포넌트를 Mesh 컴포넌트로 설정
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// 4-2. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));
	// 4-3. 데이터 로드가 성공했다면
	if (TempGunMesh.Succeeded())
	{
		// 4-4. 스켈레탈 메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// 4-5 위치 조정하기
		//gunMeshComp->SetRelativeLocation(FVector(-14, 11, 138));	
	}

	// 5.스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SniperGunComp"));
	// 5-1 부모 컴포넌트를 Mesh 컴포넌트로 설정
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// 5-2 스태틱 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSniperMesh(TEXT("SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Sniper_Rifle_A.Sniper_Rifle_A'"));

	// 5-3 데이터 로드가 성공했다면
	if (TempSniperMesh.Succeeded())
	{
		// 5-4. 스태틱 메시 데이터 할당
		sniperGunComp->SetSkeletalMesh(TempSniperMesh.Object);
		// 5-5 위치 조정하기
		//sniperGunComp->SetRelativeLocation(FVector(-22, 31, 128));
	}
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundCue'/Game/Assets/MilitaryWeapSilver/Sound/Rifle/Cues/RifleA_Fire_Cue.RifleA_Fire_Cue'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 초기 속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlayerMove();
	//BIsRunShooting = BRunning && BShooting;
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
		PlayerInput->BindAction(ia_ChangeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeGun);
		PlayerInput->BindAction(ia_SniperAim, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_SniperAim, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);
		
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);
		
	}

}

void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	direction.X = value.X;
	direction.Y = value.Y;
}

void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);

}

void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::PlayerMove()
{
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	//FVector P0 = GetActorLocation();
	//FVector vt = direction * walkSpeed * DeltaTime;
	//FVector P = P0 + vt;
	//SetActorLocation(P);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{

	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);
	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

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
		params.AddIgnoredActor(this);
		// Channel 필터를 이용한 LineTrace 충돌 검출(충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_GameTraceChannel1, params);
		// LineTrace가 부딪혔을 때
		if (bHit)
		{
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

	// 발사 "시점"에 뛰고 있었는지를 여기서만 판정 (Tick에서는 더 이상 안 건드림)
	if (BRunning)
	{
		BIsRunShooting = true;
	}

	ThisDelegate.BindLambda([this]() {
		BShooting = false;
		BIsRunShooting = false;
		});

	GetWorld()->GetTimerManager().ClearTimer(ThisHandle);

	GetWorld()->GetTimerManager().SetTimer(
		ThisHandle,
		ThisDelegate,
		5.0f, // 실행 주기
		false // 반복 여부
	);

}

void ATPSPlayer::ChangeGun()
{
	bUsingSniperGun = !bUsingSniperGun;
	gunMeshComp->SetVisibility(!bUsingSniperGun);
	sniperGunComp->SetVisibility(bUsingSniperGun);
}

void ATPSPlayer::SniperAim(const FInputActionValue& inputValue)
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

void ATPSPlayer::InputRun()
{
	auto movement = GetCharacterMovement();

	BRunning = !BRunning;

	if (BRunning)
	{
		movement->MaxWalkSpeed = runSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = walkSpeed;
		BIsRunShooting = false;
	}
}



