// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ViewModels/ProfilePropertyViewModel.h"
#include "UObject/Interface.h"
#include "UserProfileViewModelInterface.generated.h"

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UUserProfileViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Model the UI for the user profile
 */
class CLEVERTAPSAMPLE_API IUserProfileViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	FString GetCleverTapId() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	FString GetProfileName() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	void SetProfileName(const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	FString GetEmail() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	void SetEmail(const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	FString GetPhone() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	void SetPhone(const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	TArray<FProfilePropertyViewModel> GetProfileProperties();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	void AddProfileProperty(FProfilePropertyViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	void SetProfilePropertyAt(int32 Index, FProfilePropertyViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|UserProfile")
	void RemoveProfilePropertyAt(int32 Index);
};
