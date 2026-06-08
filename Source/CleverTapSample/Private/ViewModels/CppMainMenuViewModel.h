// Copyright CleverTap All Rights Reserved.
#pragma once

#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/MainMenuViewModelInterface.h"
#include "ViewModels/PETabViewModelInterface.h"
#include "CppMainMenuViewModel.generated.h"

enum class ECleverTapPushPermissionStatus : uint8;
class UCppEventTabViewModel;
class UCppPrivacyTabViewModel;
class UCppProfileTabViewModel;
class UCppPushTabViewModel;
class UCppUserProfileViewModel;
class UCppPETabViewModel;

/**
 * C++ implementation of the IMainMenuViewModelInterface
 */
UCLASS(NotBlueprintable)
class UCppMainMenuViewModel : public UCppViewModelBase, public IMainMenuViewModelInterface
{
	GENERATED_UCLASS_BODY()

public:
	UCppProfileTabViewModel* GetProfileTab() const;
	UCppPushTabViewModel* GetPushTab() const;
	UCppEventTabViewModel* GetEventTab() const;
	UCppPrivacyTabViewModel* GetPrivacyTab() const;
	
	UCppPETabViewModel* GetPETab() const;
	
	void Edit(TFunctionRef<void(MutableContext&, UCppMainMenuViewModel&)> EditFn);
	UCppMainMenuViewModel& SetActiveTab(MutableContext&, EMainMenuTab Value);

private:
	// <IMainMenuViewModelInterface>
	EMainMenuTab GetActiveTab_Implementation() const override;
	void SetActiveTab_Implementation(EMainMenuTab Value) override;
	TScriptInterface<IProfileTabViewModelInterface> GetProfileTab_Implementation() const override;
	TScriptInterface<IPushTabViewModelInterface> GetPushTab_Implementation() const override;
	TScriptInterface<IEventTabViewModelInterface> GetEventTab_Implementation() const override;
	TScriptInterface<IPrivacyTabViewModelInterface> GetPrivacyTab_Implementation() const override;
	TScriptInterface<IPETabViewModelInterface> GetPETab_Implementation() const override;
	// </IMainMenuViewModelInterface>

private:
	UPROPERTY()
	UCppProfileTabViewModel* ProfileTab;

	UPROPERTY()
	UCppPushTabViewModel* PushTab;

	UPROPERTY()
	UCppEventTabViewModel* EventTab;

	UPROPERTY()
	UCppPrivacyTabViewModel* PrivacyTab;
	
	UPROPERTY()
	UCppPETabViewModel* PETab;
	
	EMainMenuTab ActiveTab{};
};
