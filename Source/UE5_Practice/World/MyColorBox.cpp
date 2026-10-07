// Fill out your copyright notice in the Description page of Project Settings.


#include "World/MyColorBox.h"

// Sets default values
AMyColorBox::AMyColorBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyColorBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyColorBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

