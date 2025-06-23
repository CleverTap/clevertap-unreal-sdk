// Copyright CleverTap All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "UserWidgetView.generated.h"

class IViewModelInterface;

/**
 * Base class for UUserWidget derived views
 */
UCLASS(Blueprintable, Abstract)
class CLEVERTAPSAMPLE_API UUserWidgetView : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetViewModel(const TScriptInterface<IViewModelInterface>& ViewModel);
};
