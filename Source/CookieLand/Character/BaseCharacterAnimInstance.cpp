// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"

#include "BaseCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Character = Cast<ABaseCharacter>(TryGetPawnOwner());
	if (!Character.IsValid())
	{
		return;
	}

	LocomotionComponent = Character->GetLocomotionComponent();
	
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!Character.IsValid())
	{
		return;
	}

	FVector velocity = Character->GetMovementComponent()->Velocity;
	CurGaitSpeed = FVector2d(velocity.X,velocity.Y).Length();

	if(abs(velocity.Z)<0.1)
	{
		JumpSequenceStateType = EJumpSequenceStateType::None;
	}else
	{
		JumpSequenceStateType = velocity.Z>0?EJumpSequenceStateType::JumpUp:EJumpSequenceStateType::JumpDown;
	}
}

void UBaseCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	
}