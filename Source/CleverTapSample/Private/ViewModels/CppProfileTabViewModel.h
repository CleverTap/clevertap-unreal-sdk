// Copyright CleverTap All Rights Reserved.
#pragma once

#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/ProfileTabViewModelInterface.h"
#include "CppProfileTabViewModel.generated.h"

class UCppUserProfileViewModel;

/**
 * C++ implementation of the IProfileTabViewModelInterface
 */
UCLASS(NotBlueprintable)
class UCppProfileTabViewModel : public UCppViewModelBase, public IProfileTabViewModelInterface
{
	GENERATED_UCLASS_BODY()

public:
	UCppUserProfileViewModel* GetUserProfile() const;

	void Edit(TFunctionRef<void(MutableContext&, UCppProfileTabViewModel&)> EditFn);

private:
	// <IProfileTabViewModelInterface>
	TScriptInterface<IUserProfileViewModelInterface> GetUserProfile_Implementation() const override;
	bool IsPushProfileEnabled_Implementation() const override;
	void PushProfile_Implementation() override;
	FString GetLastPushedProfileData_Implementation() const override;
	void DeleteSaveState_Implementation() override;
	// </IProfileTabViewModelInterface>

private:
	UPROPERTY()
	UCppUserProfileViewModel* UserProfile;

	FString LastPushedProfileData;
};
