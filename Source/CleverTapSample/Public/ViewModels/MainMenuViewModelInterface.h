// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MainMenuViewModelInterface.generated.h"

class IEventTabViewModelInterface;
class IPrivacyTabViewModelInterface;
class IProfileTabViewModelInterface;
class IPushTabViewModelInterface;
class IUserProfileViewModelInterface;
class IPETabViewModelInterface;
/**
 * UI Tabs for the main menu
 */
UENUM(BlueprintType)
enum class EMainMenuTab : uint8
{
	Profile,
	Push,
	Events,
	Privacy,
	PE,
};

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UMainMenuViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Model the UI for the main menu
 */
class CLEVERTAPSAMPLE_API IMainMenuViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu")
	EMainMenuTab GetActiveTab() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu")
	void SetActiveTab(EMainMenuTab Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu")
	TScriptInterface<IProfileTabViewModelInterface> GetProfileTab() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu")
	TScriptInterface<IPushTabViewModelInterface> GetPushTab() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu")
	TScriptInterface<IEventTabViewModelInterface> GetEventTab() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu")
	TScriptInterface<IPrivacyTabViewModelInterface> GetPrivacyTab() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu")
	TScriptInterface<IPETabViewModelInterface> GetPETab() const;
};
