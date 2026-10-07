// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"
//#include "Perception/PawnSensingComponent.h" -> 구버전. 필요없음.

//AI 필요한 헤더파일들
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Assets/Book_Life/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));
	// 1-1. 데이터 로드가 성공하면
	if (tempMesh.Succeeded())
	{
		//1-2 데이터 할당
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		//1-3 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}

	//EnemyFSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	// 애니메이션 블루프린트 할당하기
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("AnimBlueprint'/Game/Blueprints/ABP_Enemy.ABP_Enemy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

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




}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//void AEnemy::PostInitializeComponents()
//{
//	//..기존 코드 ..
//		// 구버전
//		  //GetPawnSense()->OnSeePawn.AddDynamic(this, &AIHMinion::OnPawnDetected);
//		  //GetPawnSense()->OnHearNoise.AddDynamic(this, &AIHMinion::OnHearNoise);
//
//		  // 새로운 버전
//		if (AISense)
//		{
//			AISense->OnTargetPerceptionUpdated.AddDynamic(this, &AMyMinion::OnTargetPerceptionUpdated);
//		}
//}

//// AI가 특정 감각으로 Actor를 인지했다.
//void AEnemy::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
//{
//	if (Stimulus.WasSuccessfullySensed() == false)
//		return;
//
//	FAISenseID SightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
//	FAISenseID HearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());
//
//	// 감지된 타입에 따라서 네트워크책의 함수를 호출해준다.
//	if (Stimulus.Type == SightID)
//	{
//		// 시각 감지
//		if (APawn* Pawn = Cast<APawn>(Actor))
//		{
//			OnPawnDetected(Pawn); // 책 코드 호출
//		}
//	}
//}
