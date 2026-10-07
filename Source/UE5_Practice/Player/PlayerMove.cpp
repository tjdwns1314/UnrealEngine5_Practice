// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "System/MyDataSubsystem.h"
#include "System/MyGameInstance.h"
#include "System/MyDataConfigAsset.h"
#include "System/MyCharacterStatDataAsset.h"
#include "Game/US_CharacterStats.h"
#include "GameplayTagContainer.h"
#include "System/MyGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/InteractInterface.h"
#include "UE5_Practice.h"

void UPlayerMove::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	UMyDataSubsystem* DataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem< UMyDataSubsystem>();
	if (DataSubsystem == nullptr)
		return;
	//DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Turn)
	PlayerInput->BindAction(DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Turn), ETriggerEvent::Triggered, this, &UPlayerMove::Turn);
	PlayerInput->BindAction(DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Look), ETriggerEvent::Triggered, this, &UPlayerMove::LookUp);
	PlayerInput->BindAction(DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Move), ETriggerEvent::Triggered, this, &UPlayerMove::Move);
	PlayerInput->BindAction(DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Run), ETriggerEvent::Started, this, &UPlayerMove::RunStarted);
	PlayerInput->BindAction(DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Run), ETriggerEvent::Completed, this, &UPlayerMove::RunCompleted);
	PlayerInput->BindAction(DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Jump), ETriggerEvent::Started, this, &UPlayerMove::InputJump);
	PlayerInput->BindAction(DataSubsystem->FindInputActionByTag(MyGameplayTags::Input_Action_Interact), ETriggerEvent::Started, this, &UPlayerMove::Interact);
}

void UPlayerMove::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	direction.X = value.X;
	direction.Y = value.Y;
}

//void UPlayerMove::PlayerMove()
//{
//	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
//	me->AddMovementInput(direction);
//	direction = FVector::ZeroVector;
//}


void UPlayerMove::PlayerMove()
{
	const FRotator YawRotation(
		0.0f,
		me->GetControlRotation().Yaw,
		0.0f
	);

	const FVector MoveDirection = YawRotation.RotateVector(direction);
	me->AddMovementInput(MoveDirection);

	direction = FVector::ZeroVector;

}


UPlayerMove::UPlayerMove()
{
	//Tick 함수 호출되도록 처리
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();
	BRunning = false;
	if (moveComp)
		moveComp->MaxWalkSpeed = CurrentStats.WalkSpeed;
	
	//UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	//StatDataAsset = (GI && GI->DataConfig)
	//	? GI->DataConfig->DA_Stat.Get()
	//	: nullptr;

	// 초기 속도를 걷기로 설정
	//moveComp->MaxWalkSpeed = walkSpeed;
	//FUS_CharacterStats* Stats =
	//	StatDataAsset ? StatDataAsset->GetCharacterStats() : nullptr;

}

void UPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PlayerMove();
}

void UPlayerMove::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerYawInput(value);

}

void UPlayerMove::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerPitchInput(value);
}

//void UPlayerMove::InputRun()
//{
//	auto movement = me->GetCharacterMovement();
//
//	BRunning = !BRunning;
//
//	if (BRunning)
//	{
//		if (Stats)
//		{
//			moveComp->MaxWalkSpeed = Stats->SprintSpeed; // 달리기일 때
//		}
//		//movement->MaxWalkSpeed = runSpeed;
//	}
//	else
//	{
//		if (Stats)
//		{
//			moveComp->MaxWalkSpeed = Stats->WalkSpeed; // 달리기일 때
//		}
//		//movement->MaxWalkSpeed = walkSpeed;
//		BIsRunShooting = false;
//	}
//}
//void UPlayerMove::Interact()
//{
//	AActor* TargetActor = UGameplayStatics::GetActorOfClass(GetWorld(), InteractTargetClass);
//
//	if (UFunction* Function = TargetActor->FindFunction(TEXT("Interact")))
//	{
//		TargetActor->ProcessEvent(Function, nullptr);
//	}
//}

void UPlayerMove::Interact()
{
	const FVector Start = me->GetPawnViewLocation();
	const FVector End =
		Start + me->GetControlRotation().Vector() * 300.0f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(me);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
	{
		return;
	}

	AActor* TargetActor = HitResult.GetActor();

	if (IsValid(TargetActor) &&
		TargetActor->Implements<UInteractInterface>())
	{
		IInteractInterface::Execute_Interact(TargetActor, me);
	}
	PrintLogWithRole(me, TEXT("F Interact"), FColor::Cyan, 1);
}



void UPlayerMove::RunStarted()
{
	PrintLogWithRole(me, TEXT("RunStarted"), FColor::Red, 10);
	//BRunning = true;
	//if (moveComp)
	//{
	//	moveComp->MaxWalkSpeed = CurrentStats.SprintSpeed;
	//}
	SprintStart_Server();
}

void UPlayerMove::RunCompleted()
{
	PrintLogWithRole(me, TEXT("RunComplete"), FColor::Red, 10);
	//BRunning = false;
	//if (moveComp)
	//{
	//	moveComp->MaxWalkSpeed = CurrentStats.WalkSpeed;
	//}
	SprintEnd_Server();

}

void UPlayerMove::InputJump(const FInputActionValue& inputValue)
{
	me->Jump();
}

// 서버가 호출하는 스탯 갱신
void UPlayerMove::SetCharacterStats(const FUS_CharacterStats& NewStats)
{
	CurrentStats = NewStats;
	UE_LOG(LogTemp, Warning, TEXT("전달받은 스탯: Walk=%.1f Sprint=%.1f"),
		CurrentStats.WalkSpeed, CurrentStats.SprintSpeed);
	if (moveComp)
	{
		// 서버는 레벨업한 스피드를 알고있어.
		moveComp->MaxWalkSpeed = BRunning ? CurrentStats.SprintSpeed : CurrentStats.WalkSpeed;
		
		UpdateWalkSpeed_Multicast(moveComp->MaxWalkSpeed);
	}
}

void UPlayerMove::SprintStart_Server_Implementation()
{
	BRunning = true;
	if (moveComp)
	{
		moveComp->MaxWalkSpeed = CurrentStats.SprintSpeed;
		UpdateWalkSpeed_Multicast(moveComp->MaxWalkSpeed);
	}
}
void UPlayerMove::SprintEnd_Server_Implementation()
{
	BRunning = false;
	if (moveComp)
	{
		moveComp->MaxWalkSpeed = CurrentStats.WalkSpeed;
		UpdateWalkSpeed_Multicast(moveComp->MaxWalkSpeed);
	}
}

void UPlayerMove::UpdateWalkSpeed_Multicast_Implementation(float Speed)
{
	PrintLogWithRole(me, *FString::Printf(TEXT("Multicast %f"), Speed), FColor::Red, 10);

	if (moveComp)
	{
		moveComp->MaxWalkSpeed = Speed;
	}
}


