// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IDetailCustomization.h"
//#include "CookieLandMapBuildActorDetail.generated.h"

class ACookieLandMapBuildActor;

class FCookieLandMapBuildActorDetail : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FCookieLandMapBuildActorDetail());
	}

	virtual ~FCookieLandMapBuildActorDetail() {}

	virtual void CustomizeDetails(IDetailLayoutBuilder& InDetailBuilder) override;

protected:
	UPROPERTY()
	ACookieLandMapBuildActor* ModifierInstance;

protected:

	FReply ReloadMapBuildInfoButtonClick();

	FReply CreateEnvironmentButtonClick();

	FReply OpenMapBuilderEditorViewButtonClick();
};
