// Copyright CleverTap All Rights Reserved.
//  CleverTapGameEvents.h
//  CleverTapSample

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CleverTapGameEvents.generated.h"

UENUM(BlueprintType)
enum class ECleverTapResourceFlow : uint8
{
	Earned UMETA(DisplayName = "Earned"),
	Spent  UMETA(DisplayName = "Spent"),
};

UENUM(BlueprintType)
enum class ECleverTapLevelStatus : uint8
{
	Started   UMETA(DisplayName = "Started"),
	Completed UMETA(DisplayName = "Completed"),
	Failed    UMETA(DisplayName = "Failed"),
};

UENUM(BlueprintType)
enum class ECleverTapAdEvent : uint8
{
	Shown         UMETA(DisplayName = "Shown"),
	Clicked       UMETA(DisplayName = "Clicked"),
	Skipped       UMETA(DisplayName = "Skipped"),
	RewardGranted UMETA(DisplayName = "RewardGranted"),
};

UENUM(BlueprintType)
enum class ECleverTapAdFormat : uint8
{
	None          UMETA(DisplayName = "None (omit)"),
	Banner        UMETA(DisplayName = "Banner"),
	Interstitial  UMETA(DisplayName = "Interstitial"),
	RewardedVideo UMETA(DisplayName = "RewardedVideo"),
	OfferWall     UMETA(DisplayName = "OfferWall"),
	Playable      UMETA(DisplayName = "Playable"),
};

UENUM(BlueprintType)
enum class ECleverTapErrorSeverity : uint8
{
	Low      UMETA(DisplayName = "Low"),
	Medium   UMETA(DisplayName = "Medium"),
	High     UMETA(DisplayName = "High"),
	Critical UMETA(DisplayName = "Critical"),
};

/**
 * Typed game-analytics helpers
 * All functions are available from both C++ and Blueprint (CleverTap|Gaming category).
 *
 * Optional parameters:
 *   Level/Stage/Placement — pass empty string TEXT("") to omit from the event
 *   Score                 — pass -1 to omit
 *   Value                 — pass -1.0f to omit
 *   Format                — pass ECleverTapAdFormat::None to omit
 */
UCLASS()
class CLEVERTAP_API UCleverTapGameEvents : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Gaming")
	static void RecordResourceEvent(
		ECleverTapResourceFlow Flow,
		const FString& Currency,
		float Amount,
		const FString& Category,
		const FString& ItemName);

	UFUNCTION(BlueprintCallable, Category = "CleverTap|Gaming")
	static void RecordLevelEvent(
		ECleverTapLevelStatus Status,
		const FString& World,
		const FString& Level,
		const FString& Stage,
		int32 Score);

	UFUNCTION(BlueprintCallable, Category = "CleverTap|Gaming")
	static void RecordGameEvent(
		const FString& EventName,
		float Value);

	UFUNCTION(BlueprintCallable, Category = "CleverTap|Gaming")
	static void RecordAdEvent(
		ECleverTapAdEvent Action,
		ECleverTapAdFormat Format,
		const FString& Placement);

	UFUNCTION(BlueprintCallable, Category = "CleverTap|Gaming")
	static void RecordErrorEvent(
		ECleverTapErrorSeverity Severity,
		const FString& Description);
};


//  Created by Nilesh Sharma on 28/09/26.
//  Copyright © 2026 Epic Games, Inc. All rights reserved.
//

