// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

void UPlayerMove::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &UPlayerMove::Turn);
	PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &UPlayerMove::LookUp);
	PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &UPlayerMove::Move);
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &UPlayerMove::RunStarted);
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &UPlayerMove::RunCompleted);
	PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &UPlayerMove::InputJump);
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

