// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoginPageViewModelInterface.generated.h"

class IUserProfileViewModelInterface;

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API ULoginPageViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Model the UI for the user profile login page
 */
class CLEVERTAPSAMPLE_API ILoginPageViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	TScriptInterface<IUserProfileViewModelInterface> GetUserProfile() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	FString GetIdentity() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	void SetIdentity(const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	FString GetCustomCleverTapId() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	void SetCustomCleverTapId(const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	bool IsCustomCleverTapIdVisible() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	bool IsLoginEnabled() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|Login")
	void Login();
};
