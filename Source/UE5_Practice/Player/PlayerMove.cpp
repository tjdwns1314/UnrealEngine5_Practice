// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "System/MyDataSubsystem.h"
#include "GameplayTagContainer.h"
#include "System/MyGameplayTags.h"

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
}

void UPlayerMove::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	direction.X = value.X;
	direction.Y = value.Y;
}

void UPlayerMove::PlayerMove()
{
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector;
}


UPlayerMove::UPlayerMove()
{
	//Tick 함수 호출되도록 처리
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();
	
	// 초기 속도를 걷기로 설정
	moveComp->MaxWalkSpeed = walkSpeed;
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

void UPlayerMove::InputRun()
{
	auto movement = me->GetCharacterMovement();

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
void UPlayerMove::RunStarted()
{
	BRunning = true;
	me->GetCharacterMovement()->MaxWalkSpeed = runSpeed;
}

void UPlayerMove::RunCompleted()
{
	BRunning = false;
	me->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}

void UPlayerMove::InputJump(const FInputActionValue& inputValue)
{
	me->Jump();
}

