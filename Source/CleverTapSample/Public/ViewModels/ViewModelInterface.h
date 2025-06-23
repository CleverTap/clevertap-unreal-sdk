// Copyright CleverTap All Rights Reserved.
#pragma once

#include "Delegates/Delegate.h"
#include "UObject/Interface.h"
#include "ViewModelInterface.generated.h"

class IViewModelInterface;
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnViewModelChangedBP, TScriptInterface<IViewModelInterface>, ViewModel);

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Base interface for all view models
 */
class CLEVERTAPSAMPLE_API IViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels")
	void AddOnViewModelChangedDelegate(const FOnViewModelChangedBP& InDelegate);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels")
	void RemoveOnViewModelChangedDelegate(const FOnViewModelChangedBP& InDelegate);
};
