// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "CleverTapDate.generated.h"

/**
 * Represents a date for CleverTap profile properties.
 */
USTRUCT(BlueprintType)
struct CLEVERTAP_API FCleverTapDate
{
	GENERATED_BODY()

	/**
	 * Numeric year, such as 2025
	 */
	UPROPERTY()
	int32 Year;

	/**
	 * Numeric month between the values of [1, 12]
	 */
	UPROPERTY()
	int32 Month;

	/**
	 * Numeric day for a given month
	 */
	UPROPERTY()
	int32 Day;

	/** Default constructor. */
	FCleverTapDate() : Year(0), Month(0), Day(0) {}

	/** Fully specified constructor. */
	FCleverTapDate(int32 InYear, int32 InMonth, int32 InDay) : Year(InYear), Month(InMonth), Day(InDay) {}

	/** Copy constructor. */
	FCleverTapDate(const FCleverTapDate& Other) = default;

	/** Construct from the date part of an Unreal FDateTime struct. The time part is ignored. */
	FCleverTapDate(const FDateTime& DateTime)
		: Year(DateTime.GetYear()), Month(DateTime.GetMonth()), Day(DateTime.GetDay())
	{
	}

	/** Construct from a unix timestamp (number of seconds since January 1, 1970, 00:00:00 GMT) */
	static FCleverTapDate MakeFromUnixTimestamp(int64_t UnixTimestamp)
	{
		return FCleverTapDate(FDateTime::FromUnixTimestamp(UnixTimestamp));
	}

	/** Returns the current date in UTC. */
	static FCleverTapDate UtcNow() { return FCleverTapDate(FDateTime::UtcNow()); }

	/** Returns the current date in local time. */
	static FCleverTapDate Now() { return FCleverTapDate(FDateTime::Now()); }

	/** Returns an FDateTime set to Midnight on this date. */
	FDateTime ToDateTime() const { return FDateTime(Year, Month, Day); }

	/** Returns a Unix timestamp set to Midnight on this date */
	int64 ToUnixTimestamp() const { return ToDateTime().ToUnixTimestamp(); }

	/** Returns this date formatted as a YYYY-MM-DD string. */
	FString ToString() const { return FString::Printf(TEXT("%04d-%02d-%02d"), Year, Month, Day); }
};

UCLASS()
class UCleverTapDateBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Convert a CleverTapDate to an FDateTime. */
	UFUNCTION(BlueprintPure, Category = "CleverTap|Date")
	static FDateTime CleverTapDateToDateTime(const FCleverTapDate& CleverTapDate) { return CleverTapDate.ToDateTime(); }

	/** Convert an FDateTime to a CleverTapDate. */
	UFUNCTION(BlueprintPure, Category = "CleverTap|Date")
	static FCleverTapDate DateTimeToCleverTapDate(const FDateTime& DateTime) { return FCleverTapDate(DateTime); }

	/** Returns this date formatted as a YYYY-MM-DD string. */
	UFUNCTION(BlueprintPure, Category = "CleverTap|Date")
	static FString CleverTapDateToString(const FCleverTapDate& Date) { return Date.ToString(); }
};
