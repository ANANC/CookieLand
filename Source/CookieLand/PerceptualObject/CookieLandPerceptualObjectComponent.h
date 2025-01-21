// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CookieLandMapTypes.h"
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

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Init(FCookieLandPieceLocator InitLocator);
		
	void UnInit();

	// 获取Id
	int GetPerceptualObjectId() const;

	// 获取感知信息
	UFUNCTION(BlueprintCallable)
	const FCookieLandPerceptualObjectPerceptionInfo GetPerceptionInfo() const;

	// 设置移动目标位置
	UFUNCTION(BlueprintCallable)
	void SetLocation(FCookieLandLocation InMapCubeLocation);

	// 设置移动目标坐标
	UFUNCTION(BlueprintCallable)
	void SetLocator(FCookieLandPieceLocator InLocator);

	// 获取当前坐标
	FCookieLandPieceLocator GetLocator();

	// 获取当前位置
	FCookieLandLocation GetLocation();
};
