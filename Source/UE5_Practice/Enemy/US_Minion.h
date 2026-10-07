// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionTypes.h"
#include "US_Minion.generated.h"

UCLASS()
class UE5_PRACTICE_API AUS_Minion : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUS_Minion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IHGame", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAIPerceptionComponent> AISense;

	UPROPERTY(VISIBLEAnyWhere, BlueprintReadOnly, Category = "Minion Perception",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> Collision;

	UPROPERTY()
	FVector PatrolLocation;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Minion AI")
	float PatrolSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float ChaseSpeed = 350.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float PatrolRadius = 50000.0f;

	UFUNCTION(BLueprintCallable,Category = "Minion AI")
	void SetNextPatrolLocation();

	UFUNCTION(BLueprintCallable, Category = "Minion AI")
	void Chase(APawn* Pawn);

	virtual void PostInitializeComponents() override;

	FORCEINLINE USphereComponent* GetCollision() const { return Collision; }

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnPawnDetected(APawn* Pawn);

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

};
