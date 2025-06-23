// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PushTabViewModelInterface.generated.h"

UENUM(BlueprintType)
enum class EPushPermissionPrimerType : uint8
{
	None,
	Alert,
	HalfInterstitial,
};

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UPushTabViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Model the UI for the main menu's push notification tab
 */
class CLEVERTAPSAMPLE_API IPushTabViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	FString GetPushNotificationData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	void ClearPushNotificationData();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	FString GetPushNotificationPermissionStatus() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	void PromptForPushPermission(EPushPermissionPrimerType PrimerType);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	FString GetDeepLinkData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	void ClearDeepLinkData();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	FString GetInAppShownData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	FString GetInAppDismissedData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	FString GetInAppDismissedActionData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	FString GetInAppButtonData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Push")
	void ClearInAppData();
};
