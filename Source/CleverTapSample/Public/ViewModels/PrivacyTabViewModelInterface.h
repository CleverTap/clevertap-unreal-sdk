// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PrivacyTabViewModelInterface.generated.h"

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UPrivacyTabViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Model the UI for the main menu's privacy tab
 */
class CLEVERTAPSAMPLE_API IPrivacyTabViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Privacy")
	bool GetOptIn() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Privacy")
	void SetOptIn(bool bIsOptIn);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Privacy")
	bool GetOffline() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Privacy")
	void SetOffline(bool bIsOffline);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Privacy")
	bool GetNetworkRecording() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Privacy")
	void SetNetworkRecording(bool bIsRecording);
};
