// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimTypes.h"
#include "BaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COOKIELAND_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class ABaseCharacter> Character;

	UPROPERTY(BlueprintReadOnly)
	class UCharacterLocomotionComponent* LocomotionComponent;
	
	UPROPERTY(BlueprintReadOnly)
	float CurGaitSpeed;
	
	UPROPERTY(BlueprintReadOnly)
	EJumpSequenceStateType JumpSequenceStateType;
	
public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
public:

};
