// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePieceActor.h"

#include "BasePiece.h"
#include "BasePieceLand.h"
#include "CookieLand/Piece/Component/PieceLandComponent.h"
#include "CookieLand/Character/BaseCharacter.h"
#include "CookieLand/Gameplay/CommonFunctionLibrary.h"


// Sets default values
ABasePieceActor::ABasePieceActor()
{
}

// Called when the game starts or when spawned
void ABasePieceActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePieceActor::BeginDestroy()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		pieceLandComponent->MoveToNextPieceEvent.RemoveAll(this);
		pieceLandComponent->StandToInitialPieceEvent.RemoveAll(this);
	}

	Super::BeginDestroy();
}

void ABasePieceActor::SetOwnLand(class UBasePieceLand* land)
{
	OwnLand = land;
}

void ABasePieceActor::SetPiece(class UBasePiece* piece)
{
	Piece = piece;	
}

void ABasePieceActor::Init()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		pieceLandComponent->MoveToNextPieceEvent.AddDynamic(this,&ABasePieceActor::MoveToNextPieceEventCallback);
		pieceLandComponent->StandToInitialPieceEvent.AddDynamic(this,&ABasePieceActor::StandToInitialPieceEventCallback);
	}
	
	InitArt();
}

FPieceLocation ABasePieceActor::GetMyLocation()
{
	return Piece->GetCurInfo()->Info->Location;
}

bool ABasePieceActor::GetIsPlayerStandOnMyself()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		bool isStandOn = pieceLandComponent->GetCurLocation() == GetMyLocation();
		return isStandOn;
	}

	return false;
}
	
bool ABasePieceActor::GetIsPlayerStandOnThisFloor()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		bool isStandOn = pieceLandComponent->GetCurLocation().Floor == GetMyLocation().Floor;
		return isStandOn;
	}

	return false;
}
	
bool ABasePieceActor::GetIsFinishPieceByMyself()
{
	return Piece->GetOwnLand()->IsFinishPieceId(Piece->GetId());
}


FPieceObserveStateData ABasePieceActor::GetPieceObserveState()
{
	ABaseCharacter* mainCharacter = UCommonFunctionLibrary::GetMainCharacter(this);
	if(mainCharacter)
	{
		UPieceLandComponent* pieceLandComponent = mainCharacter->GetPieceLandComponent();
		return Piece->GetOwnLand()->GetObserveStateData(GetMyLocation(),pieceLandComponent);
	}
	return FPieceObserveStateData(false);
}

bool ABasePieceActor::GetIsInAnimation()
{
	return IsInAnimation;
}

void ABasePieceActor::SetIsInAnimation(bool isAnimation)
{
	IsInAnimation = isAnimation;

	if(IsInAnimation)
	{
		PieceActorStartInAnimationEvent.Broadcast();
	}
	else
	{
		PieceActorFinishInAnimationEvent.Broadcast();
	}
}