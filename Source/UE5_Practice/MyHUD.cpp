// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();

	// 크로스헤어 그리기
	// 항상 화면의 가운데 기준으로 그려져야한다.
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController == nullptr)	// nullptr 체크
		return;

	// 현재 화면 사이즈 구해오기
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// 센터 위치 구하기
	FVector2D ViewportCenter(ViewportSizeX / 2.f, ViewportSizeY / 2.f);

	// 가운데 점 텍스처
	if (CrosshairsCenter)
	{
		FVector2D Spread(0, 0);
		DrawCrossHair(CrosshairsCenter, ViewportCenter, Spread);
	}

	// 크로스헤어 벌어짐 정도를 좌표계 맞춰서 계산
	float SpreadScaled = ComputeMaxScreenspaceSpreadRadius();

	// 네장의 텍스처
	if (CrosshairsLeft)
	{
		FVector2D Spread(-SpreadScaled, 0);
		DrawCrossHair(CrosshairsLeft, ViewportCenter, Spread);
	}
	if (CrosshairsRight)
	{
		FVector2D Spread(SpreadScaled, 0);
		DrawCrossHair(CrosshairsRight, ViewportCenter, Spread);
	}
	if (CrosshairsTop)
	{
		FVector2D Spread(0, -SpreadScaled);
		DrawCrossHair(CrosshairsTop, ViewportCenter, Spread);
	}
	if (CrosshairsBottom)
	{
		FVector2D Spread(0, SpreadScaled);
		DrawCrossHair(CrosshairsBottom, ViewportCenter, Spread);
	}
}

void AMyHUD::DrawCrossHair(UTexture2D* Texture, FVector2D Position, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	//@TODO 추후, 에임이 벌어지는 효과 만들때 수정할 예정
	const FVector2D TextureDrawPoint = Position + Spread;

	DrawTexture(
		Texture,
		TextureDrawPoint.X - TextureWidth / 2,	// 텍스처의 센터 기준으로 보정
		TextureDrawPoint.Y - TextureHeight / 2,	// 텍스처의 센터 기준으로 보정
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor	// 크로스헤어 색상값
	);
}

float AMyHUD::ComputeMaxScreenspaceSpreadRadius()
{
	APlayerController* PC = GetOwningPlayerController();
	if (PC && PC->PlayerCameraManager)
	{
		float FOV = PC->PlayerCameraManager->GetFOVAngle();

		int32 ViewportSizeX, ViewportSizeY;
		PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

		// 탄젠트 공식으로 각도를 스크린 좌표계로 변환
		//         object
		//        *
		//	     /|
		//	    / |  h(창문 위 높이)
		//	   /  |
		//eye *---+
		//	         d(눈~창문 거리)
		// tan(FOV) = (screenSize / 2) / Distance_to_Screen : 원근 투영
		// 언리얼은 가로축 기준으로 FOV 계산.
		float ScreenDistance = (ViewportSizeX * 0.5f) / FMath::Tan(FMath::DegreesToRadians(FOV * 0.5f));

		// 크로스헤어 벌어지는 정도를 스크린 크기로 변환
		// "이제 같은 창문(같은 d)에서, 탄퍼짐 각도(CrosshairSpread)만큼 벌어진 물체는 창문 위 어디에 비칠까요?"
		// radius = d * tan(spread/2)
		float ScreenSpreadRadius = ScreenDistance * FMath::Tan(FMath::DegreesToRadians(CrosshairSpread * 0.5f));
		return ScreenSpreadRadius;
	}

	return 0.f;
}
