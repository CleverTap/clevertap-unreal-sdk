// Copyright CleverTap All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CleverTapConfig.generated.h"

/**
 * Configuration for CleverTap instances
 */
UCLASS(BlueprintType, config=Engine)
class CLEVERTAP_API UCleverTapConfig : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * If true then the UCleverTapSubsystem will setup the common CleverTap instance when the
	 *  subsystem initializes. This value is only used on the UCleverTapConfig default object.
	 */
	UPROPERTY(config, EditAnywhere)
	bool bAutoInitializeCommonInstance = true; 

	/**
	 * The project ID taken from the CleverTap dashboard
	 */
	UPROPERTY(config, EditAnywhere)
	FString ProjectId;

	/**
	 * The project token taken from the CleverTap dashboard
	 */
	UPROPERTY(config, EditAnywhere)
	FString ProjectToken;

	/**
	 * The region code for the project taken from https://developer.clevertap.com/docs/idc#ios
	 */
	UPROPERTY(config, EditAnywhere)
	FString RegionCode;
};

