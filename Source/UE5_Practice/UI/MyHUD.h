// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PRACTICE_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

	void SetCrosshairSpread(float Spread) { CrosshairSpread = Spread; }
	void SetCrosshairColor(FColor Color) { CrosshairColor = Color; }

private:
	// 크로스헤어가 최종적으로 얼만큼 벌어져서 그려져야하는지
	float CrosshairSpread = 0;
	// 크로스헤어 색상값
	FColor CrosshairColor = FColor::White;

private:
	void DrawCrossHair(UTexture2D* Texture, FVector2D Position, FVector2D Spread);
	float ComputeMaxScreenspaceSpreadRadius();

public:
	// 가운데 중심점
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> CrosshairsCenter;

	// 네장의 상하좌우
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> CrosshairsLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> CrosshairsRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> CrosshairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> CrosshairsBottom;
};
