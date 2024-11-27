// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CookieLand/Map/Public/CookieLandMapTypes.h"
#include "CookieLandPerceptualObjectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOKIELAND_API UCookieLandPerceptualObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCookieLandPerceptualObjectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Perceptual, meta = (DisplayName = "感知类型"))
	FName PerceptualObjectType = "Default";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Perceptual, meta = (DisplayName = "是否主感知对象"))
	bool bMainPerceptualObject = false;

protected:

	UPROPERTY(VisibleAnywhere, Category = Perceptual, meta = (DisplayName = "Id"))
	int PerceptualObjectId = -1;

	UPROPERTY(VisibleAnywhere, Category = Perceptual, meta = (DisplayName = "坐标"))
	FCookieLandLocation MapCubeLocation;

	UPROPERTY(VisibleAnywhere, Category = Perceptual, meta = (DisplayName = "方向"))
	ECookieLandPieceOrientation PieceOrientation;

	UPROPERTY(VisibleAnywhere, Category = Perceptual, meta = (DisplayName = "感知信息"))
	FCookieLandPerceptualObjectPerceptionInfo PerceptionInfo;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Init(FCookieLandPieceLocator InitLocator);
		
	void UnInit();

protected:
	void SetLocation(FCookieLandLocation InMapCubeLocation);

	void SetLocator(FCookieLandPieceLocator InLocator);
};
