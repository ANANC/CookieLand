// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CookieLandActionTypes.h"
#include "CookieLandBaseCueActor.generated.h"

class UCookieLandPiece;

UCLASS()
class COOKIELAND_API ACookieLandBaseCueActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACookieLandBaseCueActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

protected:
	UPROPERTY()
	TObjectPtr< UCookieLandBasePieceAction> Action;

	UPROPERTY()
	TObjectPtr< UCookieLandBaseCueActorData> BaseData;

	bool bInit = false;
	bool bUnInit = false;

public:
	virtual void Init(UCookieLandBasePieceAction* InAction, UCookieLandBaseCueActorData* InBaseData);
	virtual void UnInit();

public:

	UCookieLandBasePieceAction* GetAction();

	UCookieLandBaseCueActorData* GetBaseData();

	UCookieLandPiece* GetPiece();

	UActorComponent* FindComponentByName(TSubclassOf<UActorComponent> InComponentType, FString InName);

	template <class T>
	T* GetComponentByName(FString InName)
	{
		if (UActorComponent* Component = FindComponentByName(T::StaticClass(),InName))
		{
			return dynamic_cast<T*>(Component);
		}

		return nullptr;
	}
};
