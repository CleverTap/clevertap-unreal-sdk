// Copyright CleverTap All Rights Reserved.
#pragma once

#include "GameFramework/HUD.h"
#include "CppDemonstrationHUD.generated.h"

class IViewModelInterface;
class UCppDemonstrationSaveGame;
class UCppMainMenuViewModel;
class UCppLoginPageViewModel;
class UUserWidgetView;

/**
 * States in the simple UI state machine
 */
UENUM()
enum class ECppDemonstrationUIState : uint8
{
	Unknown,
	Login,
	MainMenu,
};

/**
 * C++ demonstration HUD that spawns the sample menu and a controller for it.
 */
UCLASS(config = Engine)
class CLEVERTAPSAMPLE_API ACppDemonstrationHUD : public AHUD
{
	GENERATED_BODY()

public:
	void DeleteSaveState();
	void Login(const FString& Name, const FString& Email, const FString& Phone, const FString& Identity,
		const FString& CustomCleverTapId);

	// <AActor>
	void BeginPlay() override;
	// </AActor>

protected:
	UPROPERTY(config, EditAnywhere, meta = (MetaClass = "UserWidgetView", DisplayName = "CleverTap Sample Menu Class"))
	FSoftClassPath CleverTapMainMenuViewClassName;

	UPROPERTY(
		config, EditAnywhere, meta = (MetaClass = "UserWidgetView", DisplayName = "CleverTap Sample Login Page Class"))
	FSoftClassPath CleverTapLoginPageViewClassName;

private:
	UCppDemonstrationSaveGame& GetOrCreateSaveState();
	void ApplyPrivacySettingsFromSaveState();

	UFUNCTION() // For dynamic binding
	void SyncSaveStateToViewModels(TScriptInterface<IViewModelInterface> VM);

	void SetUIState(ECppDemonstrationUIState Value);

	void BeginDisplay_LoginPage();
	void EndDisplay_LoginPage();

	void BeginDisplay_MainMenu();

private:
	UPROPERTY(Transient)
	UCppDemonstrationSaveGame* SaveState;

	UPROPERTY(Transient)
	UCppMainMenuViewModel* MainMenuVM;

	UPROPERTY(Transient)
	UCppLoginPageViewModel* LoginPageVM;

	UPROPERTY(Transient)
	UUserWidgetView* ActiveView;

	UPROPERTY(Transient)
	ECppDemonstrationUIState State;
};
