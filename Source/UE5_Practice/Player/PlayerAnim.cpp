// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include "PlayerFIre.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMove.h"


void UPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 초기화 될때 캐싱해두기
	Character = Cast<ACharacter>(TryGetPawnOwner());

	MovementComponent = nullptr;
	playerFire = nullptr;
	playerMove = nullptr;

	if (Character != nullptr)
	{
		MovementComponent = Character->GetCharacterMovement();
		playerFire = Character->FindComponentByClass<UPlayerFire>();
		playerMove = Character->FindComponentByClass<UPlayerMove>();
	}
	tpsPlayer = Cast<ATPSPlayer>(TryGetPawnOwner());
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	

	if (!playerFire && Character)
	{
		playerFire = Character->FindComponentByClass<UPlayerFire>();
	}

	if (!playerMove && Character)
	{
		playerMove = Character->FindComponentByClass<UPlayerMove>();
	}

	if (Character && MovementComponent)
	{
		// 속도 계산
		Velocity = Character->GetVelocity();
		GroundSpeed = Velocity.Length();
		bShouldMove = (MovementComponent->GetCurrentAcceleration().Size() > 0) && (GroundSpeed >= 0.01f);
		bIsFalling = MovementComponent->IsFalling();

		// 방향 계산
		Direction = CalculateDirection(Velocity, Character->GetActorRotation());
	}
	if (playerFire && playerMove)   // null 체크도 추가 권장
	{
		BIsRunShooting = playerFire->BShooting && playerMove->BRunning;
	}
	else
	{
		BIsRunShooting = false;
	}
	// 왼손이 있어야할 위치를 소켓정보를 통해 갱신한다.
	if (tpsPlayer && tpsPlayer->gunMeshComp)
	{
		// 무기의 LeftHandSocket 위치를 월드 좌표로 가져온다.
		FTransform LeftHandWorldTransform = tpsPlayer->gunMeshComp->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);

		FVector OutPosition;
		FRotator OutRotator;
		Character->GetMesh()->TransformToBoneSpace(FName("hand_r"),
			LeftHandWorldTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotator);

		// hand_r 공간으로 변환한 좌표를 animation 좌표로 설정한다.
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotator));
	}
}

void UPlayerAnim::PlayAttackAnim()
{
	Montage_Play(attackAnimMontage);
}
