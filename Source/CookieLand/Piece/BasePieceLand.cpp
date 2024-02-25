// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePieceLand.h"

#include "BasePiece.h"
#include "BasePieceActor.h"
#include "PieceBaseAction.h"
#include "PieceLandComponent.h"
#include "PieceLandSystem.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"

void UBasePieceLand::CreateLand(FName levelName,class ULandDataAsset* landDA)
{
	LevelName = levelName;
	LandDataAsset = landDA;

	BoundInfo = NewObject<UPieceLandBoundInfo>();
	
	for(int index = 0;index<LandDataAsset->Pieces.Num();++index)
	{
		TObjectPtr<UPieceBaseConfigData> pieceData = LandDataAsset->Pieces[index];

		CreatePiece(pieceData.Get());
	}
}

void UBasePieceLand::DestroyLand()
{
	LandDestroyBeforeEvent.Broadcast();
	
	for(int index = 0;index<Pieces.Num();++index)
	{
		Pieces[index]->UnInit();
	}

	Pieces.Reset();
	PieceMap.Reset();

	UCommonFunctionLibrary::GetPieceLandSystem()->PieceLandHideAllTipEvent.Broadcast(false,false);
	
	LandDestroyEvent.Broadcast();
}


void UBasePieceLand::CreateDynamicPieceByLocation(FPieceLocation location,bool isDefaultConfig,UPieceBaseConfigData* pieceData)
{
	UPieceBaseConfigData* createConfigData = NewObject<UPieceBaseConfigData>();
	createConfigData->Id = pieceAutoId;

	UPieceBaseInfo* baseInfo = NewObject<UPieceBaseInfo>();
	createConfigData->BaseInfo = baseInfo;
	
	if(isDefaultConfig && LandDataAsset &&  LandDataAsset->DefaultPieceConfig)
	{
		createConfigData->ActorClass = LandDataAsset->DefaultPieceConfig->ActorClass;
		baseInfo->Copy(LandDataAsset->DefaultPieceConfig->BaseInfo);

		for(int index = 0;index<LandDataAsset->DefaultPieceConfig->Actions.Num();++index)
		{
			createConfigData->Actions.Add(LandDataAsset->DefaultPieceConfig->Actions[index]);
		}
	}
	else if(pieceData)
	{
		createConfigData->ActorClass = pieceData->ActorClass;

		baseInfo->Copy(pieceData->BaseInfo);

		for(int index = 0;index<pieceData->Actions.Num();++index)
		{
			createConfigData->Actions.Add(pieceData->Actions[index]);
		}
	}
	else
	{
		return;
	}

	baseInfo->Location = location;
	
	pieceAutoId+=1;

	CreatePiece(createConfigData);
}

UBasePiece* UBasePieceLand::CreatePiece(UPieceBaseConfigData* pieceData)
{
	UBasePiece* piece = NewObject<UBasePiece>(this);
	Pieces.Add(piece);
	PieceMap.Add(pieceData->Id,piece);
	
	piece->SetOwnLand(this);
	piece->SetId(pieceData->Id);
	piece->SetConfigData(pieceData);

	piece->Init();

	BoundInfo->AddPiece(piece);
	
	for(int index = 0;index<pieceData->Actions.Num();++index)
	{
		TObjectPtr<UPieceBaseActionConfigData> actionConfigData = pieceData->Actions[index];
		if(!actionConfigData.Get() || !actionConfigData->ActionClass)
		{
			continue;
		}

		FPieceActionHandle handle;
		CreateActionToPiece(handle,piece->GetId(),actionConfigData);
	}
	
	return piece;
}

void UBasePieceLand::DeletePieceById(int pieceId)
{
	UBasePiece* piece = GetPieceById(pieceId);
	if(!piece)
	{
		return;
	}

	Pieces.Remove(piece);
	PieceMap.Remove(piece->GetId());
	BoundInfo->RemovePiece(piece);
	
	piece->UnInit();
}

bool UBasePieceLand::GetEnableOccupyLocation(FPieceLocation location,int& OccupyId)
{
	UPieceLocationInfo* locationInfo = GetLocationInfo(location);

	OccupyId = locationInfo->PieceId;

	if(locationInfo->IsOccupy || !locationInfo->EnableOccupy)
	{
		return false;
	}

	return true;
}

bool UBasePieceLand::RequestOccupyLocation(int pieceId,FPieceLocation location)
{
	int OccupyId;
	if(!GetEnableOccupyLocation(location,OccupyId))
	{
		return false;
	}
	
	UPieceLocationInfo* locationInfo = GetLocationInfo(location);
	if(locationInfo->EnableOccupy)
	{
		locationInfo->IsOccupy = true;
		locationInfo->PieceId = pieceId;

		LandLocationOccupyStateChangeEvent.Broadcast(pieceId,location);
		return true;
	}

	return false;
}

bool UBasePieceLand::RequestUnOccupyLocation(int pieceId,FPieceLocation location)
{
	UPieceLocationInfo* locationInfo = GetLocationInfo(location);
	if(locationInfo->IsOccupy && locationInfo->PieceId == pieceId)
	{
		locationInfo->IsOccupy = false;
		locationInfo->PieceId = -1;

		LandLocationUnOccupyStateChangeEvent.Broadcast(pieceId,location);
		return true;
	}

	return false;
}


bool UBasePieceLand::CreateActionToPiece(FPieceActionHandle& handle,int pieceId,class UPieceBaseActionConfigData* actionData)
{
	if(!actionData || !actionData->ActionClass)
	{
		return false;
	}
	
	UBasePiece* piece = GetPieceById(pieceId);
	if(!piece)
	{
		return false;
	}

	if(CreateAction(handle,piece->GetCurInfo()->Info->Location,actionData,pieceId))
	{
		return true;
	}
	
	return false;
}

class UPieceBaseAction* UBasePieceLand::CreateAction(FPieceActionHandle& handle,FPieceLocation triggerLocation,class UPieceBaseActionConfigData* actionData,int pieceId)
{
	UBasePiece* piece = nullptr;
	
	if(pieceId!=-1)
	{
		piece = GetPieceById(pieceId);
		if(!piece)
		{
			return nullptr;
		}
	}
	
	UPieceBaseAction* action = NewObject<UPieceBaseAction>(this,actionData->ActionClass);
	if(!action)
	{
		return false;
	}

	handle = FPieceActionHandle(actionAutoId,pieceId);
	
	actionAutoId+=1;
	
	action->SetHandle(handle);
	action->SetData(actionData);
	action->SetTriggerLocation(triggerLocation);
	action->SetPiece(piece);
	action->Init();

	if(action->IsFinish())
	{
		action->UnInit();
		return nullptr;
	}
	
	if(piece)
	{
		piece->AddAction(action);
	}
	
	if(actionData->IsAutoExecute)
	{
		action->Execute();
	}

	return action;
}

void UBasePieceLand::DeleteActionByPiece(FPieceActionHandle handle)
{
	if(!handle.GetIsValid())
	{
		return;
	}

	do
	{
		UBasePiece* piece = GetPieceById(handle.GetPieceId());
		if(!piece)
		{
			break;
		}

		piece->RemoveAction(handle);
	}
	while (false);
	
	handle.Clear();
}

FName UBasePieceLand::GetLevelName()
{
	return LevelName;
}

int UBasePieceLand::GetInitialPieceId()
{
	if(LandDataAsset)
	{
		return LandDataAsset->InitialPieceId;
	}
	return -1;
}

UBasePiece* UBasePieceLand::GetPieceById(int pieceId)
{
	UBasePiece** piecePtr = PieceMap.Find(pieceId);
	if(piecePtr)
	{
		return *piecePtr;
	}
	return nullptr;
}

FPieceLocation UBasePieceLand::GetLocationById(int pieceId)
{
	UBasePiece** piecePtr = PieceMap.Find(pieceId);
	if(piecePtr)
	{
		UBasePiece* piece = *piecePtr;
		return piece->GetCurInfo()->Info->Location;
	}
	return FPieceLocation(false);
}

FVector UBasePieceLand::GetActorLocationById(int pieceId)
{
	FVector actorLocation = FVector::ZeroVector;
	
	UBasePiece* piece = GetPieceById(pieceId);
	if(piece)
	{
		const UPieceInfo* curInfo = piece->GetCurInfo();
		if(curInfo->IsOccupy)
		{
			actorLocation = GetActorLocationByLocation(curInfo->Info->Location);
		}
	}

	return actorLocation;
}

FVector UBasePieceLand::GetActorLocationByLocation(FPieceLocation location)
{
	FVector actorLocation = LandDataAsset->CreateInitialLocation;
	actorLocation.X += location.X * LandDataAsset->ActorInterval.X;
	actorLocation.Y += location.Y * LandDataAsset->ActorInterval.Y;
	actorLocation.Z += location.Floor * LandDataAsset->ActorInterval.Z;

	return actorLocation;
}

UPieceLocationInfo* UBasePieceLand::GetLocationInfo(FPieceLocation location)
{
	for(int index = 0;index<OccupyStates.Num();++index)
	{
		if(OccupyStates[index]->Location == location)
		{
			return OccupyStates[index];
		}
	}

	UPieceLocationInfo* locationInfo = NewObject<UPieceLocationInfo>();
	locationInfo->Location = location;
	OccupyStates.Add(locationInfo);

	return locationInfo;
}

bool UBasePieceLand::GetPieceIdByLocation(FPieceLocation location,int& pieceId)
{
	pieceId = -1;
	
	for(int index = 0;index<OccupyStates.Num();++index)
	{
		if(OccupyStates[index]->Location == location)
		{
			if(OccupyStates[index]->IsOccupy)
			{
				pieceId = OccupyStates[index]->PieceId;
				return true;
			}
		}
	}

	return false;
}
	
bool UBasePieceLand::IsInFinishLocation(int pieceId)
{
	bool isFinishLocation = LandDataAsset?LandDataAsset->FinishPieceId == pieceId:false;
	return isFinishLocation;
}

bool UBasePieceLand::IsFinishPieceId(int pieceId)
{
	return LandDataAsset?LandDataAsset->FinishPieceId == pieceId:false;
}

bool UBasePieceLand::RequestToNextLocation(int pieceId,EPieceDirection direction,int& nextPieceId)
{
	UBasePiece* piece = GetPieceById(pieceId);
	if(!piece)
	{
		return false;	
	}

	if(!piece->GetEnableMove(direction))
	{
		return false;
	}

	FPieceDistance distance = piece->GetCurInfo()->Info->Distance;
	
	UBasePiece* nextPiece = nullptr;
	if(direction != EPieceDirection::Up && direction != EPieceDirection::Down)
	{
		nextPiece = GetNearPieceByDirectionPieces(pieceId,distance,direction);
	}
	else
	{
		nextPiece = GetNearPieceByUpOrDown(pieceId,distance,direction);
	}
	
	if(!nextPiece)
	{
		return false;
	}

	nextPieceId = nextPiece->GetId();
	return true;
}

TArray<UBasePiece*> UBasePieceLand::GetFloorPieces(int floor)
{
	UPieceLandFloorBoundInfo** floorBoundInfoPtr = BoundInfo->Floor2BoundInfos.Find(floor);
	if(floorBoundInfoPtr)
	{
		UPieceLandFloorBoundInfo* floorBoundInfo = *floorBoundInfoPtr;
		return floorBoundInfo->Pieces;
	}

	TArray<UBasePiece*> floorPieces;
	return  floorPieces;
}

TArray<UBasePiece*> UBasePieceLand::GetDirectionPiecesByFloorPieces(TArray<UBasePiece*> floorPieces,FPieceLocation curLocation,EPieceDirection direction)
{
	TArray<UBasePiece*> directionPieces;

	for(int index = 0;index<floorPieces.Num();++index)
	{
		UBasePiece* piece = floorPieces[index];
		FPieceLocation pieceLocation = piece->GetCurInfo()->Info->Location;

		if(direction == EPieceDirection::Left && (pieceLocation.Y == curLocation.Y && pieceLocation.X < curLocation.X))
		{
			directionPieces.Add(piece);
		}
		if(direction == EPieceDirection::Right && (pieceLocation.Y == curLocation.Y && pieceLocation.X > curLocation.X))
		{
			directionPieces.Add(piece);
		}
		if(direction == EPieceDirection::Forward && (pieceLocation.X == curLocation.X && pieceLocation.Y < curLocation.Y))
		{
			directionPieces.Add(piece);
		}
		if(direction == EPieceDirection::Backward && (pieceLocation.X == curLocation.X && pieceLocation.Y > curLocation.Y))
		{
			directionPieces.Add(piece);
		}
	}

	return directionPieces;
}

UBasePiece* UBasePieceLand::GetNearPieceByDirectionPieces(int pieceId,FPieceDistance distance,EPieceDirection direction)
{
	UBasePiece* piece = GetPieceById(pieceId);
	FPieceLocation curLocation = piece->GetCurInfo()->Info->Location;
	
	TArray<UBasePiece*> floorPieces = GetFloorPieces(curLocation.Floor);
	TArray<UBasePiece*> directionPieces = GetDirectionPiecesByFloorPieces(floorPieces,curLocation,direction);
	
	UBasePiece* minPiece = nullptr;
	FPieceLocation minLocation(0,0,0);
	
	for(int index = 0;index<directionPieces.Num();++index)
	{
		bool update = false;
		UBasePiece* directionPiece = directionPieces[index];
		FPieceLocation pieceLocation = directionPiece->GetCurInfo()->Info->Location;
		if(direction == EPieceDirection::Left)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.X-curLocation.X)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.X < pieceLocation.X);
		}
		if(direction == EPieceDirection::Right)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.X-curLocation.X)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.X > pieceLocation.X);
		}
		if(direction == EPieceDirection::Forward)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.Y-curLocation.Y)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.Y < pieceLocation.Y);
		}
		if(direction == EPieceDirection::Backward)
		{
			if(!distance.IsUnLimit && (abs(pieceLocation.Y-curLocation.Y)>distance.LimitDistance))
			{
				continue;
			}
			update = (minPiece == nullptr) || (minLocation.Y > pieceLocation.Y);
		}

		if(update)
		{
			minPiece = directionPiece;
			minLocation.Copy(pieceLocation);
		}
	}

	if(minPiece)
	{
		EPieceDirection oppositeDir = UCommonFunctionLibrary::ChangeGameDirectionByAngle(180,direction);
		if(!minPiece->GetEnableMove(EPieceDirection::Up) && !minPiece->GetEnableMove(EPieceDirection::Down)
			&& !IsFinishPieceId(minPiece->GetId())
			&& !minPiece->GetEnableMove(oppositeDir))
		{
			minPiece = nullptr;
		}
	}
	return minPiece;
}

UBasePiece* UBasePieceLand::GetNearPieceByUpOrDown(int pieceId,FPieceDistance distance,EPieceDirection direction)
{
	UBasePiece* piece = GetPieceById(pieceId);
	FPieceLocation curLocation = piece->GetCurInfo()->Info->Location;
	int intDirection = direction == EPieceDirection::Up?1:direction==EPieceDirection::Down?-1:0;

	int range;
	if(distance.IsUnLimit)
	{
		if(BoundInfo->HasValidFloors())
		{
			range = direction == EPieceDirection::Up?BoundInfo->MaxFloor:BoundInfo->MinFloor;
		}
		else
		{
			range = curLocation.Floor;
		}
	}
	else
	{
		range = curLocation.Floor + distance.LimitDistance * intDirection;
	}
	
	for(int index = 1, floor = curLocation.Floor;index<=abs(range-curLocation.Floor);++index)
	{
		floor += intDirection;
		FPieceLocation nextLocation(curLocation,floor);

		int nextPieceId;
		if(GetPieceIdByLocation(nextLocation,nextPieceId))
		{
			UBasePiece* nextPiece = GetPieceById(nextPieceId);
			return nextPiece;
		}
	}

	return nullptr;
}


TArray<UBasePiece*> UBasePieceLand::GetOutDistancePieces(int pieceId,FPieceDistance distance,EPieceDirection direction)
{
	TArray<UBasePiece*> outDistancePieces;
	if(distance.IsUnLimit)
	{
		return outDistancePieces;
	}
	
	UBasePiece* piece = GetPieceById(pieceId);
	FPieceLocation pieceLocation = piece->GetCurInfo()->Info->Location;

	int initialIndex = 0;
	int intDirection = (direction == EPieceDirection::Left || direction == EPieceDirection::Backward || direction == EPieceDirection::Down)?-1:1;
	int range = 0;
	
	UPieceLandFloorBoundInfo* floorBoundInfo = BoundInfo->GetFloorBoundInfo(pieceLocation.Floor);
	if(!floorBoundInfo)
	{
		return outDistancePieces;
	}
	
	switch (direction)
	{
	case EPieceDirection::Left:
	case EPieceDirection::Right:
		initialIndex = pieceLocation.X + distance.LimitDistance * intDirection;
		range = intDirection == 1? floorBoundInfo->MaxX:floorBoundInfo->MinX;
		break;
	case EPieceDirection::Forward:
	case EPieceDirection::Backward:
		initialIndex = pieceLocation.Y + distance.LimitDistance * intDirection;
		range = intDirection == 1? floorBoundInfo->MaxY:floorBoundInfo->MinY;
		break;
	case EPieceDirection::Up:
	case EPieceDirection::Down:
		initialIndex = piece->GetCurInfo()->Info->Location.Floor + distance.LimitDistance * intDirection;
		range = intDirection == 1? BoundInfo->MaxFloor:BoundInfo->MinFloor;
		break;
	}
	
	for(int index = 0,value = initialIndex;index<abs(range-initialIndex);++index)
	{
		FPieceLocation nextLocation(pieceLocation);
		value+=intDirection;
		
		switch (direction)
		{
		case EPieceDirection::Left:
		case EPieceDirection::Right:
			nextLocation.X = value;
			break;
		case EPieceDirection::Forward:
		case EPieceDirection::Backward:
			nextLocation.Y = value;
			break;
		case EPieceDirection::Up:
		case EPieceDirection::Down:
			nextLocation.Floor = value;
			break;
		}
		int nextPieceId;
		if(GetPieceIdByLocation(nextLocation,nextPieceId))
		{
			UBasePiece* nextPiece = GetPieceById(nextPieceId);
			outDistancePieces.Add(nextPiece);
		}
	}

	return outDistancePieces;
}

TSubclassOf<class ABasePieceActor> UBasePieceLand::GetPieceInstanceActorClass(UBasePiece* piece)
{
	TSubclassOf<class ABasePieceActor> actorClassType = nullptr;
	if(LandDataAsset && LandDataAsset->DefaultPieceConfig)
	{
		actorClassType = LandDataAsset->DefaultPieceConfig->ActorClass;
	}
	
	if(!actorClassType)
	{
		actorClassType = piece->GetConfigData()->ActorClass;
	}

	if(!actorClassType)
	{
		actorClassType = ABasePieceActor::StaticClass();
	}

	return actorClassType;
}

FPieceLocation UBasePieceLand::GetNearLogicLocationByActorLocation(FVector location)
{
	FVector actorInterval = LandDataAsset->ActorInterval;
	int floor = (int)(location.Z / actorInterval.Z);

	int x = FMath::RoundToInt(location.X / actorInterval.X);
	int y = FMath::RoundToInt(location.Y / actorInterval.Y);

	FPieceLocation pieceLocation(x,y,floor);
	return pieceLocation;
}

bool UBasePieceLand::GetPieceCardEnableAttach(FPieceLocation pieceLocation,FPieceLocation characterLocation,FName pieceCardName)
{
	UPieceLandSystem* pieceLandSystem = UCommonFunctionLibrary::GetPieceLandSystem();

	FPieceCardConfigData pieceCardConfigData;
	if(!pieceLandSystem->GetPieceCardConfigData(pieceCardName,pieceCardConfigData))
	{
		return false;
	}
	if(!pieceCardConfigData.ActionConfigData || !pieceCardConfigData.ActionConfigData->ActionClass)
	{
		return false;
	}

	bool isEnable = false;

	do
	{
		int pieceId;
		bool hasPiece = GetPieceIdByLocation(pieceLocation,pieceId);
		
		if(pieceCardConfigData.ActionConfigData->IsMustAttachToValidPiece && !hasPiece)
		{
			break;
		}

		if(pieceCardConfigData.IsOnlyCross)
		{
			if(!(characterLocation.X == pieceLocation.X || (characterLocation.Y == pieceLocation.Y)))
			{
				break;
			}
		}

		int distance = pieceCardConfigData.ValidDistance;
		if(abs(characterLocation.X - pieceLocation.X)>distance || abs(characterLocation.Y - pieceLocation.Y)>distance)
		{
			break;
		}

		if(!pieceCardConfigData.IsHorizontal)
		{
			if(pieceLocation!=characterLocation)
			{
				break;
			}
		}

		if(!pieceCardConfigData.IsVertical)
		{
			if(pieceLocation.Floor != characterLocation.Floor)
			{
				break;
			}
		}

		isEnable = true;
	}while(false);

	return isEnable;
}

void UBasePieceLand::UsePieceCardToLocation(FPieceLocation pieceLocation,FName pieceCardName)
{
	UPieceLandSystem* pieceLandSystem = UCommonFunctionLibrary::GetPieceLandSystem();

	FPieceCardConfigData pieceCardConfigData;
	if(!pieceLandSystem->GetPieceCardConfigData(pieceCardName,pieceCardConfigData))
	{
		return;
	}

	if(!pieceCardConfigData.ActionConfigData || !pieceCardConfigData.ActionConfigData->ActionClass)
	{
		return;
	}

	bool attachActionToPiece = false;
	
	int pieceId;
	if( GetPieceIdByLocation(pieceLocation,pieceId))
	{
		if(UBasePiece* piece = GetPieceById(pieceId))
		{
			attachActionToPiece = true;

			FPieceActionHandle handle;
			CreateActionToPiece(handle,pieceId,pieceCardConfigData.ActionConfigData);
		}
	}

	if(!attachActionToPiece && !pieceCardConfigData.ActionConfigData->IsMustAttachToValidPiece)
	{
		FPieceActionHandle handle;
		CreateAction(handle,pieceLocation,pieceCardConfigData.ActionConfigData);
	}
	
}

FPieceObserveStateData UBasePieceLand::GetObserveStateData(FPieceLocation pieceLocation,class UPieceLandComponent* pieceLandComponent)
{
	FPieceObserveStateData pieceObserveStateData(false);

	FPieceLocation enableObserveDistance = pieceLandComponent->GetEnableObserveDistance();
	FPieceLocation curLocation = pieceLandComponent->GetCurLocation();
	if(!curLocation.IsValid)
	{
		curLocation = GetLocationById(GetInitialPieceId());
	}
	
	if(UCommonFunctionLibrary::IsLocationInSideWithInRange(curLocation,pieceLocation,enableObserveDistance))
	{
		pieceObserveStateData.IsVisible = true;

		pieceObserveStateData.ObserveFormType = EPieceObserveFormType::Full;
		if(curLocation.Floor != pieceLocation.Floor)
		{
			pieceObserveStateData.ObserveFormType = EPieceObserveFormType::OnlySimpleLine;
		}
	}
	else
	{
		pieceObserveStateData.IsVisible = false;
	}
	
	return pieceObserveStateData;
}