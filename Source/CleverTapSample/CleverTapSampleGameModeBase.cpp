// Copyright Epic Games, Inc. All Rights Reserved.

#include "CleverTapSampleGameModeBase.h"
#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"
#include "Engine.h"

void ACleverTapSampleGameModeBase::BeginPlay()
{
	UCleverTapSubsystem* const CleverTapSubsystem = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	ICleverTapInstance& SharedInst = CleverTapSubsystem->SharedInstance();
	UE_LOG(LogCleverTapSample, Log, TEXT("CleverTap Id: %s"), *SharedInst.GetCleverTapId());
}

