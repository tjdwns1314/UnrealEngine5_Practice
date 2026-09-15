// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBaseComponent.h"


// Sets default values for this component's properties
UPlayerBaseComponent::UPlayerBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	// ...
}
void UPlayerBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();

	me = Cast<ATPSPlayer>(GetOwner());
	moveComp = me->GetCharacterMovement();

	// 델리게이트에 처리 함수 등록
	me->onInputBindingDelegate.AddUObject(this, &UPlayerBaseComponent::SetupInputBinding);
}

// Called when the game starts
void UPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}



