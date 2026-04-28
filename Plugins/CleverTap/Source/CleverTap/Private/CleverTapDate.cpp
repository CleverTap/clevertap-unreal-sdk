// Copyright CleverTap All Rights Reserved.
#include "CleverTapDate.h"

#include "CleverTapLog.h"

FCleverTapDate UCleverTapDateBlueprintLibrary::Conv_StringToDate(const FString& InString)
{
	// Replace -:. characters with space
	FString ReplacedString = InString.Replace(TEXT("-"), TEXT(" "));
	ReplacedString.ReplaceInline(TEXT(":"), TEXT(" "));
	ReplacedString.ReplaceInline(TEXT("."), TEXT(" "));

	// Split on space
	TArray<FString> Parts{};
	ReplacedString.ParseIntoArray(Parts, TEXT(" "));
	if (Parts.Num() < 3)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("Invalid date format: %s"), *InString);
		return FCleverTapDate{};
	}

	int32 Year{};
	LexFromString(Year, *Parts[0]);

	int32 Month{};
	LexFromString(Month, *Parts[1]);

	int32 Day{};
	LexFromString(Day, *Parts[2]);

	return FCleverTapDate{ Year, Month, Day };
}
