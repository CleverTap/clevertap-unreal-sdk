// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ProfilePropertyViewModel.generated.h"

/**
 * The profile property's underlying value type
 */
UENUM(BlueprintType)
enum class EProfilePropertyType : uint8
{
	String UMETA(DisplayName = "String"),
	Int32 UMETA(DisplayName = "Int32"),
	Int64 UMETA(DisplayName = "Int64"),
	Float UMETA(DisplayName = "Float"),
	Double UMETA(DisplayName = "Double"),
	Bool UMETA(DisplayName = "Bool"),
	Date UMETA(DisplayName = "Date"),
	Array_Int32 UMETA(DisplayName = "Array of Int32s"),
	Array_Int64 UMETA(DisplayName = "Array of Int64s"),
	Array_Float UMETA(DisplayName = "Array of Floats"),
	Array_Double UMETA(DisplayName = "Array of Doubles"),
	Array_Bool UMETA(DisplayName = "Array of Bools"),
	Array_String UMETA(DisplayName = "Array of Strings"),
};

/**
 * Model the UI for profile properties
 */
USTRUCT(BlueprintType)
struct CLEVERTAPSAMPLE_API FProfilePropertyViewModel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EProfilePropertyType ValueType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Value"))
	FString ValueString;
};
