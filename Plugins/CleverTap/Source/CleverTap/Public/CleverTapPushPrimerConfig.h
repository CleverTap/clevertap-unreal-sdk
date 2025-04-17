// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "CleverTapPushPrimerConfig.generated.h"

/**
 *  The configuration settings for a Push Primer Alert.
 *
 *	A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
 *	It is an InApp notification that provides the details of message types, your users can expect, before requesting
 *	notification permission.
 *
 *  See ICleverTapInstance::PromptForPushPermission()
 */
USTRUCT(BlueprintType)
struct FCleverTapPushPrimerAlertConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MessageText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PositiveButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NegativeButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFollowDeviceOrientation = true;

	/**
	 * When true and permissions were previously denied, then show an alert dialog which routes to app's notification
	 * settings page.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFallbackToSettings = false;
};

/**
 *  The configuration settings for a Half-Interstitial Push Primer.
 *
 *	A Push Primer explains the need for push notifications to your users and helps to improve your engagement rates.
 *	It is an InApp notification that provides the details of message types, your users can expect, before requesting
 *	notification permission.
 *
 *  See ICleverTapInstance::PromptForPushPermission()
 */
USTRUCT(BlueprintType)
struct FCleverTapPushPrimerHalfInterstitialConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MessageText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PositiveButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NegativeButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ImageURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor BackgroundColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor ButtonBorderColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor TitleTextColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor MessageTextColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor ButtonTextColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor ButtonBackgroundColor = FColor::Silver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ButtonBorderRadius; // todo is a string really what we want here

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFollowDeviceOrientation = true;

	/**
	 * When true and permissions were previously denied, then show an alert dialog which routes to app's notification
	 * settings page.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFallbackToSettings = false;
};
