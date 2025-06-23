// Copyright CleverTap All Rights Reserved.
#pragma once

#include "Tickable.h"
#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/UserProfileViewModelInterface.h"
#include "CppUserProfileViewModel.generated.h"

/**
 * C++ implementation of the IUserProfileViewModelInterface
 */
UCLASS(NotBlueprintable)
class UCppUserProfileViewModel :
	public UCppViewModelBase,
	public FTickableGameObject,
	public IUserProfileViewModelInterface
{
	GENERATED_BODY()

public:
	const FString& GetCleverTapId() const;
	const FString& GetProfileName() const;
	const FString& GetEmail() const;
	const FString& GetPhone() const;
	TArrayView<FProfilePropertyViewModel const> GetProfileProperties() const;

	void Edit(TFunctionRef<void(MutableContext&, UCppUserProfileViewModel&)> EditFn);
	UCppUserProfileViewModel& SetCleverTapId(MutableContext&, const FString& Value);
	UCppUserProfileViewModel& SetProfileName(MutableContext&, const FString& Value);
	UCppUserProfileViewModel& SetEmail(MutableContext&, const FString& Value);
	UCppUserProfileViewModel& SetPhone(MutableContext&, const FString& Value);
	UCppUserProfileViewModel& SetProfileProperties(MutableContext&, const TArray<FProfilePropertyViewModel>& Value);

	// <FTickableGameObject>
	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
	ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	// </FTickableGameObject>

private:
	// <IUserProfileViewModelInterface>
	FString GetCleverTapId_Implementation() const override;
	FString GetProfileName_Implementation() const override;
	void SetProfileName_Implementation(const FString& Value) override;
	FString GetEmail_Implementation() const override;
	void SetEmail_Implementation(const FString& Value) override;
	FString GetPhone_Implementation() const override;
	void SetPhone_Implementation(const FString& Value) override;
	TArray<FProfilePropertyViewModel> GetProfileProperties_Implementation() override;
	void AddProfileProperty_Implementation(FProfilePropertyViewModel Value) override;
	void SetProfilePropertyAt_Implementation(int32 Index, FProfilePropertyViewModel Value) override;
	void RemoveProfilePropertyAt_Implementation(int32 Index) override;
	// </IUserProfileViewModelInterface>

private:
	FString CleverTapId;
	FString Name;
	FString Email;
	FString Phone;

	TArray<FProfilePropertyViewModel> Properties;
};
