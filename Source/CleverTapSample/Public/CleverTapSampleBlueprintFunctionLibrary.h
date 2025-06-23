// Copyright CleverTap All Rights Reserved.
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ViewModels/EventPropertyViewModel.h"
#include "ViewModels/ProfilePropertyViewModel.h"
#include "CleverTapSampleBlueprintFunctionLibrary.generated.h"

UCLASS()
class CLEVERTAPSAMPLE_API UCleverTapSampleBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CleverTap|ViewModels|Properties")
	static bool IsEventPropertyValueValid(const FEventPropertyViewModel& Property);

	UFUNCTION(BlueprintCallable, Category = "CleverTap|ViewModels|Properties")
	static bool IsProfilePropertyValueValid(const FProfilePropertyViewModel& Property);
};
