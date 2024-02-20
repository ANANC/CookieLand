// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPieceBaseOComponent.h"
#include "GameFramework/Actor.h"
#include "PieceTypes.h"
#include "BasePiece.generated.h"



UCLASS()
class COOKIELAND_API UBasePiece : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UBasePiece(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY(BlueprintReadOnly)
	class UBasePieceLand* OwnLand;
	
	UPROPERTY(BlueprintReadOnly)
	class UPieceBaseConfigData* ConfigData;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UPieceInfo* CurInfo;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class ABasePieceActor* PieceActor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<class UPieceBaseAction*> Actions;

	UPROPERTY()
	TArray<IPieceBaseOComponent*> OComponents;
	
	UPROPERTY(BlueprintReadOnly)
	class UPieceActionStateOComponent* ActionStateOComponent;
	
public:
	void SetId(int Id);

	UFUNCTION(BlueprintPure)
	int GetId();

	void SetOwnLand(class UBasePieceLand* land);

	class UBasePieceLand* GetOwnLand();
	
	void SetConfigData(class UPieceBaseConfigData* configData);

	void Init();

	void UnInit();

public:
	
	UFUNCTION(BlueprintPure)
	const UPieceInfo* GetCurInfo();

	UFUNCTION(BlueprintPure)
	const UPieceBaseConfigData* GetConfigData();
	
	UFUNCTION(BlueprintPure)
	bool GetEnableMove(EPieceDirection direction);

	void OverlaySetEnableMove(TArray<EPieceDirection> directions);

	void AddEnableMove(EPieceDirection direction);
	
	void RemoveEnableMove(EPieceDirection direction);
	
	void AddAction(class UPieceBaseAction* action);

	void RemoveAction(FPieceActionHandle handle);
	
	class ABasePieceActor* GetPieceActor();

public:
	
	template <class T>
	T* AddOComponent(TSubclassOf<UObject> ocomponentClass)
	{
		UObject* OComponentObject = NewObject<UObject>(this,ocomponentClass);
		if(OComponentObject)
		{
			IPieceBaseOComponent* iOComponent = Cast<IPieceBaseOComponent>(OComponentObject);
			if(iOComponent)
			{
				OComponents.Add(iOComponent);

				iOComponent->SetPiece(this);
				iOComponent->Init();
				return dynamic_cast<T*>(OComponentObject);
			}
		}
		return nullptr;
	}

	class UPieceActionStateOComponent* GetActionStateOComponent();

protected:
	void ProtectUpdateMoveDirection();
};
