// Copyright CleverTap All Rights Reserved.
#pragma once

#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/LoginPageViewModelInterface.h"
#include "CppLoginPageViewModel.generated.h"

class UCppUserProfileViewModel;

/**
 * C++ implementation of the ILoginPageViewModelInterface
 */
UCLASS(NotBlueprintable)
class UCppLoginPageViewModel : public UCppViewModelBase, public ILoginPageViewModelInterface
{
	GENERATED_UCLASS_BODY()

public:
	UCppUserProfileViewModel* GetUserProfile() const;

	void Edit(TFunctionRef<void(MutableContext&, UCppLoginPageViewModel&)> EditFn);
	UCppLoginPageViewModel& SetIdentity(MutableContext&, const FString& Value);
	UCppLoginPageViewModel& SetCustomCleverTapId(MutableContext&, const FString& Value);

private:
	// <ILoginPageViewModelInterface>
	TScriptInterface<IUserProfileViewModelInterface> GetUserProfile_Implementation() const override;
	FString GetIdentity_Implementation() const override;
	void SetIdentity_Implementation(const FString& Value) override;
	FString GetCustomCleverTapId_Implementation() const override;
	void SetCustomCleverTapId_Implementation(const FString& Value) override;
	bool IsCustomCleverTapIdVisible_Implementation() const override;
	bool IsLoginEnabled_Implementation() const override;
	void Login_Implementation() override;
	// </ILoginPageViewModelInterface>

private:
	UPROPERTY()
	UCppUserProfileViewModel* UserProfile;

	FString Identity;
	FString CustomCleverTapId;
};
