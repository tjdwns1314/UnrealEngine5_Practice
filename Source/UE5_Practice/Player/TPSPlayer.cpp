// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerMove.h"
#include "PlayerFire.h"
#include "UE5_Practice.h"
#include <Kismet/GameplayStatics.h>
#include "MyHUD.h"



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

	playerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("PlayerMove"));
	//playerFire = CreateDefaultSubobject<UPlayerFire>(TEXT("PlayerFire"));
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}

	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);

	hp = initialHp;
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCrossHair();
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput)
	{
		// 컴포넌트에서 입력 바인딩 처리하도록 호출
		onInputBindingDelegate.Broadcast(PlayerInput);



		// 컴포넌트에서 입력 바인딩 처리하도록 호출 
		/*playerMove->SetupInputBinding(PlayerInput);
		playerFire->SetupInputBinding(PlayerInput);*/
	}
}

void ATPSPlayer::OnHitEvent()
{
	PRINT_LOG(TEXT("Damaged !!!!!"));
	hp--;
	if (hp <= 0)
	{
		PRINT_LOG(TEXT("Player is dead!"));
		OnGameOver();
	}

}

void ATPSPlayer::UpdateCrossHair()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
		return;

	// 크로스헤어가 그려지고 있는 HUD 가져오기
	AMyHUD* HUD = Cast<AMyHUD>(PlayerController->GetHUD());
	if (HUD == nullptr)
		return;


	// 1. 이동이 있으면 크로스헤어 벌어진다.
	// Velocity 값 기준으로 얼만큼 벌어지게 할지 결정.
	// 최대 이속 대비, 현재 이속값을 계산해서 얼마나 크로스헤어가 벌어지는지 계산
	FVector2D WalkSpeedRange(0.f, GetCharacterMovement()->MaxWalkSpeed);
	FVector2D VelocityMulRange(0.f, 1.f);
	FVector Velocity = GetVelocity();
	Velocity.Z = 0; // 점프속도는 무시

	// 최대속도 600, 현재 이속 : 300 -> 0.5f 라는 수치를 얻고싶다.
	// 최대속도 600, 현재 이속 : 600 -> 1.0f
	float CrosshairVelocityAlpha = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMulRange, Velocity.Size());

	// 환산된값 0~1 사이의 기준으로, Weapon Min,Max 값에 대응하는 값을 뽑는다.
	// min :2, max:6, 2~6 사이의 적절한값으로 환산해준다.
	float CrosshairVelocityFactor = FMath::Lerp(CrosshairSpreadMin, CrosshairSpreadMax, CrosshairVelocityAlpha);

	// 최종값 : 이동 Factor + Fire Factor
	float CrosshairFireFactor = 0;	//@TODO : 총기마다 벌어지는 크로스헤어 값
	float TotalCrosshairFactor = CrosshairVelocityFactor + CrosshairFireFactor;

	// 최종적으로 HUD 객체에 Spread 정보를 전달한다.
	HUD->SetCrosshairSpread(TotalCrosshairFactor);
}


void ATPSPlayer::OnGameOver_Implementation()
{
	// 게임 오버 시 일시 정지
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}



