// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ViewModels/EventPropertyViewModel.h"
#include "ChargedEventProductViewModel.generated.h"

/**
 * Model the UI for charged event products
 */
USTRUCT(BlueprintType)
struct CLEVERTAPSAMPLE_API FChargedEventProductViewModel
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ProductIndex{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEventPropertyViewModel> Properties;
};
