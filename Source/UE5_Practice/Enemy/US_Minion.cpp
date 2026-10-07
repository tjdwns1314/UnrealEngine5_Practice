// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/US_Minion.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Player/TPSPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/SphereComponent.h"
#include "NavigationInvokerComponent.h"

//#include "Perception/PawnSensingComponent.h" -> 구버전. 필요없음.

//AI 필요한 헤더파일들
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
AUS_Minion::AUS_Minion()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetSphereRadius(100);
	Collision->SetupAttachment(RootComponent);

	GetCapsuleComponent()->InitCapsuleSize(60.f, 96.0f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	// Set the skeletal mesh for the character
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -91.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/KayKit/Skeletons/skeleton_minion"));
	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	// Set the character movement properties
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;



	// 책은 구버전
	//PawnSense = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSense"));
	//PawnSense->SensingInterval = .8f;
	//PawnSense->SetPeripheralVisionAngle(45.f);
	//PawnSense->SightRadius = 1500.f;
	//PawnSense->HearingThreshold = 400.f;
	//PawnSense->LOSHearingThreshold = 800.f;

	// AI Perception
	AISense = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AISense"));

	// Sight Config (시각)
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1600.f;
	SightConfig->PeripheralVisionAngleDegrees = 45.f;
	SightConfig->SetMaxAge(5.0f); // 5초정도 기억하기
	AISense->ConfigureSense(*SightConfig);	// 시각 등록

	// Hearing Config (청각)
	UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->HearingRange = 800.f;
	AISense->ConfigureSense(*HearingConfig);	// 청각 등록

	// 메인 감각 설정
	AISense->SetDominantSense(SightConfig->GetSenseImplementation());

	//.기존 코드 ..
		// 네비게이션 인보커를 디폴트로 생성한다.
		//NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavMeshInvoker"));
		//NavInvoker->SetGenerationRadii(500, 800);
}

// Called when the game starts or when spawned
void AUS_Minion::BeginPlay()
{
	Super::BeginPlay();
	SetNextPatrolLocation();
	
}

// Called every frame
void AUS_Minion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() != ROLE_Authority) return;

	if (GetMovementComponent()->GetMaxSpeed() == ChaseSpeed) return;

	if ((GetActorLocation() - PatrolLocation).Size() < 500.f)
	{
		SetNextPatrolLocation();
	}
}

// Called to bind functionality to input
void AUS_Minion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AUS_Minion::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() != ROLE_Authority) return;

	OnActorBeginOverlap.AddDynamic(this, &AUS_Minion::OnBeginOverlap);
	if (AISense)
	{
		AISense->OnTargetPerceptionUpdated.AddDynamic(this, &AUS_Minion::OnTargetPerceptionUpdated);
	}
}

void AUS_Minion::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Stimulus.WasSuccessfullySensed() ||
		Stimulus.Type != UAISense::GetSenseID(UAISense_Sight::StaticClass()))
	{
		return;
	}

	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		OnPawnDetected(Pawn);
	}
}

void AUS_Minion::OnPawnDetected(APawn* Pawn)
{
	// Checks if the pawn is a US_Character
	if (!Pawn->IsA<ATPSPlayer>()) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Character detected!"));

	// If the AI character is not already in the chase state
	if (GetCharacterMovement()->MaxWalkSpeed != ChaseSpeed)
	{
		Chase(Pawn);
	}
}

void AUS_Minion::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Checks if the pawn is a US_Character
	if (!OtherActor->IsA<ATPSPlayer>()) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Character captured!"));
}

void AUS_Minion::SetNextPatrolLocation()
{
	if (GetLocalRole() != ROLE_Authority) return;

	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	const auto LocationFound = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
		this, GetActorLocation(), PatrolLocation, PatrolRadius);
	if (LocationFound)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
	}
}

void AUS_Minion::Chase(APawn* Pawn)
{
	if (GetLocalRole() != ROLE_Authority) return;

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	// Set the AI character's destination to the player's location
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Pawn);

	// Displays the AI character's destination
	DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(), 25.f, 12, FColor::Red, true, 10.f, 0, 2.f);
}
