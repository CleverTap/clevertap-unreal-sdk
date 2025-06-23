// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "EventPropertyViewModel.generated.h"

/**
 * The event property's underlying value type
 */
UENUM(BlueprintType)
enum class EEventPropertyType : uint8
{
	String UMETA(DisplayName = "String"),
	Int32 UMETA(DisplayName = "Int32"),
	Int64 UMETA(DisplayName = "Int64"),
	Float UMETA(DisplayName = "Float"),
	Double UMETA(DisplayName = "Double"),
	Bool UMETA(DisplayName = "Bool"),
	Date UMETA(DisplayName = "Date"),
};

/**
 * Model the UI for event properties
 */
USTRUCT(BlueprintType)
struct CLEVERTAPSAMPLE_API FEventPropertyViewModel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEventPropertyType ValueType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Value"))
	FString ValueString;
};
