// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePiece.h"

#include "BasePieceActor.h"
#include "BasePieceLand.h"
#include "CookieLand/Piece/Component/PieceActionStateOComponent.h"
#include "PieceBaseAction.h"
#include "PieceTypes.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"

// Sets default values
UBasePiece::UBasePiece(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CurInfo = NewObject<UPieceInfo>();
	CurInfo->Info = NewObject<UPieceBaseInfo>();
}

void UBasePiece::SetId(int Id)
{
	CurInfo->Id = Id;
}

int UBasePiece::GetId()
{
	return CurInfo->Id;
}

void UBasePiece::SetOwnLand(class UBasePieceLand* land)
{
	OwnLand = land;
}

class UBasePieceLand* UBasePiece::GetOwnLand()
{
	return OwnLand;
}

void UBasePiece::SetConfigData(class UPieceBaseConfigData* configData)
{
	ConfigData = configData;
}

void UBasePiece::Init()
{
	ActionStateOComponent = AddOComponent<UPieceActionStateOComponent>(UPieceActionStateOComponent::StaticClass());
	
	CurInfo->IsOccupy = false;
	CurInfo->IsEnableArrive = false;
	CurInfo->Info->Copy(ConfigData->BaseInfo);

	if(OwnLand->RequestOccupyLocation(CurInfo->Id,GetLocation()))
	{
		CurInfo->IsOccupy = true;
		CurInfo->IsEnableArrive = true;
	}
	else
	{
		CurInfo->Info->Location.IsValid = false;
	}

	if(CurInfo->IsOccupy)
	{
		FTransform transform;
		transform.SetLocation(OwnLand->GetActorLocationByLocation(GetLocation()));
	
		FActorSpawnParameters spawnParameters;
		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		TSubclassOf<class ABasePieceActor> actorType = OwnLand->GetPieceInstanceActorClass(this);
		if(actorType)
		{
			AActor* actor = GetWorld()->SpawnActor(actorType, &transform, spawnParameters);
			if(actor)
			{
				PieceActor = Cast<ABasePieceActor>(actor);
				PieceActor->SetOwnLand(OwnLand);
				PieceActor->SetPiece(this);
				PieceActor->Init();
			}
		}
	}
}

void UBasePiece::UnInit()
{
	if(OwnLand)
	{
		int actionNumber = Actions.Num();
		for(int index = 0;index<actionNumber;++index)
		{
			OwnLand->DeleteActionByPiece(Actions[0]->GetHandle());
		}
	}
	Actions.Empty();

	for(int index = 0;index<OComponents.Num();++index)
	{
		OComponents[index]->UnInit();	
	}
	OComponents.Empty();
	
	if(PieceActor)
	{
		PieceActor->Destroy();
	}

	if(OwnLand && CurInfo->IsOccupy)
	{
		OwnLand->RequestUnOccupyLocation(CurInfo->Id,GetLocation());
	}
}

const UPieceInfo* UBasePiece::GetCurInfo()
{
	return CurInfo;
}

const UPieceBaseConfigData* UBasePiece::GetConfigData()
{
	return ConfigData;
}


FPieceLocation UBasePiece::GetLocation()
{
	if(CurInfo)
	{
		return CurInfo->Info->Location;
	}
	return FPieceLocation(false);
}

bool UBasePiece::GetIsEnableArrive()
{
	if(!CurInfo || !CurInfo->IsEnableArrive)
	{
		return false;
	}

	bool isEnable,isUp;
	GetIsEnableUpOrDownMove(isEnable,isUp);
	if(isEnable)
	{
		//上下不存在地块时，无法到达
		int nextPieceId;
		if(!GetOwnLand()->RequestToNextLocation(GetId(),isUp?EPieceDirection::Up:EPieceDirection::Down,nextPieceId))
		{
			return false;
		}
	}

	//处于地块距离显示时，无法到达
	int curStandOnPieceId = UCommonFunctionLibrary::GetMainCharacterCurrentPieceId(this);
	if(UBasePiece* standPiece = GetOwnLand()->GetPieceById(curStandOnPieceId))
	{
		FPieceDistance standPieceDistance = standPiece->GetCurInfo()->Info->Distance;
		if(!UCommonFunctionLibrary::IsLocationInSideByDistance(standPiece->GetLocation(),GetLocation(),standPieceDistance))
		{
			return false;
		}
	}

	return true;
	
}

bool UBasePiece::GetEnableMove(EPieceDirection direction)
{
	for(int index = 0;index<CurInfo->Info->EnableDirections.Num();++index)
	{
		if(CurInfo->Info->EnableDirections[index] == direction)
		{
			return true;
		}
	}

	return false;
}

void UBasePiece::OverlaySetEnableMove(TArray<EPieceDirection> directions)
{
	CurInfo->Info->EnableDirections.Reset();

	for(int index = 0;index<directions.Num();++index)
	{
		CurInfo->Info->EnableDirections.Add(directions[index]);
	}

	ProtectUpdateMoveDirection();
}

void UBasePiece::AddEnableMove(EPieceDirection direction)
{
	if(!GetEnableMove(direction))
	{
		AddEnableMove(direction);
	}

	ProtectUpdateMoveDirection();
}
	
void UBasePiece::RemoveEnableMove(EPieceDirection direction)
{
	for(int index = 0;index<CurInfo->Info->EnableDirections.Num();++index)
	{
		if(CurInfo->Info->EnableDirections[index] == direction)
		{
			CurInfo->Info->EnableDirections.RemoveAt(index);
			break;
		}
	}

	ProtectUpdateMoveDirection();
}

void UBasePiece::GetIsEnableUpOrDownMove(bool& isEnable,bool& isUp)
{
	if(GetEnableMove(EPieceDirection::Up))
	{
		isEnable = true;
		isUp = true;
	}else if(GetEnableMove(EPieceDirection::Down))
	{
		isEnable = true;
		isUp = false;
	}
	else
	{
		isEnable = false;
		isUp = true;
	}
}

void UBasePiece::ProtectUpdateMoveDirection()
{
	if(OwnLand->GetInitialPieceId() == GetId())
	{
		CurInfo->Info->EnableDirections.Remove(EPieceDirection::Up);
		CurInfo->Info->EnableDirections.Remove(EPieceDirection::Down);
	}
	else if(OwnLand->IsFinishPieceId(GetId()))
	{
		CurInfo->Info->EnableDirections.Empty();
	}
	else if(GetEnableMove(EPieceDirection::Up))
	{
		CurInfo->Info->EnableDirections.Empty();
		CurInfo->Info->EnableDirections.Add(EPieceDirection::Up);
	}
	else if(GetEnableMove(EPieceDirection::Down))
	{
		CurInfo->Info->EnableDirections.Empty();
		CurInfo->Info->EnableDirections.Add(EPieceDirection::Down);
	}
}

void UBasePiece::AddAction(class UPieceBaseAction* action)
{
	Actions.Add(action);
}

void UBasePiece::RemoveAction(FPieceActionHandle handle)
{
	for(int index = 0;index<Actions.Num();++index)
	{
		if(Actions[index]->GetHandle() == handle)
		{
			Actions.RemoveAt(index);
			break;
		}
	}
}

ABasePieceActor* UBasePiece::GetPieceActor()
{
	return PieceActor;
}

UPieceActionStateOComponent* UBasePiece::GetActionStateOComponent()
{
	return ActionStateOComponent;
}