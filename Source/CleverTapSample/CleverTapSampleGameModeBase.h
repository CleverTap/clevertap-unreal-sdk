// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CleverTapSampleGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CLEVERTAPSAMPLE_API ACleverTapSampleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
};
