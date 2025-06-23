// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProfileTabViewModelInterface.generated.h"

class IUserProfileViewModelInterface;

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UProfileTabViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Model the UI for the main menu's profile tab
 */
class CLEVERTAPSAMPLE_API IProfileTabViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Profile")
	TScriptInterface<IUserProfileViewModelInterface> GetUserProfile() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Profile")
	bool IsPushProfileEnabled() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Profile")
	void PushProfile();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Profile")
	FString GetLastPushedProfileData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Profile")
	void DeleteSaveState();
};
