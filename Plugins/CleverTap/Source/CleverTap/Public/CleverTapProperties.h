// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/TVariant.h"

/**
 * Represents a date for CleverTap profile properties.
 */
struct FCleverTapDate
{
	/**
	 * Numeric year, such as 2025
	 */
	int32 Year;

	/**
	 * Numeric month between the values of [1, 12]
	 */
	int32 Month;

	/**
	 * Numeric day for a given month
	 */
	int32 Day;

	FCleverTapDate() : Year(0), Month(0), Day(0) {}
	FCleverTapDate(int32 InYear, int32 InMonth, int32 InDay) : Year(InYear), Month(InMonth), Day(InDay) {}
	FCleverTapDate(const FCleverTapDate& Other) = default;

	/**
	 * Construct from the date part of an Unreal FDateTime struct. The time part is ignored.
	 */
	FCleverTapDate(const FDateTime& DateTime)
		: Year(DateTime.GetYear()), Month(DateTime.GetMonth()), Day(DateTime.GetDay())
	{
	}

	FString ToString() const { return FString::Printf(TEXT("%04d-%02d-%02d"), Year, Month, Day); }
};

/**
 * Variant type for allowed property value types.
 */
class FCleverTapPropertyValue
{
public:
	using VariantType = TVariant<int64, float, double, bool, FString, FCleverTapDate, TArray<FString>>;

	// Default constructors
	FCleverTapPropertyValue() = default;

	// Type-specific constructors
	FCleverTapPropertyValue(int InValue) : Value(TInPlaceType<int64>(), int64(InValue)) {}
	FCleverTapPropertyValue(int64 InValue) : Value(TInPlaceType<int64>(), InValue) {}
	FCleverTapPropertyValue(double InValue) : Value(TInPlaceType<double>(), InValue) {}
	FCleverTapPropertyValue(float InValue) : Value(TInPlaceType<float>(), InValue) {}
	FCleverTapPropertyValue(bool InValue) : Value(TInPlaceType<bool>(), InValue) {}
	FCleverTapPropertyValue(const char* InValue) : Value(TInPlaceType<FString>(), FString(InValue)) {}
	FCleverTapPropertyValue(const FString& InValue) : Value(TInPlaceType<FString>(), InValue) {}
	FCleverTapPropertyValue(FString&& InValue) : Value(TInPlaceType<FString>(), MoveTemp(InValue)) {}
	FCleverTapPropertyValue(const FCleverTapDate& InValue) : Value(TInPlaceType<FCleverTapDate>(), InValue) {}
	FCleverTapPropertyValue(const TArray<FString>& InValue) : Value(TInPlaceType<TArray<FString>>(), InValue) {}
	FCleverTapPropertyValue(TArray<FString>&& InValue) : Value(TInPlaceType<TArray<FString>>(), MoveTemp(InValue)) {}

	// Copy constructor
	FCleverTapPropertyValue(const FCleverTapPropertyValue& Other) = default;

	// Move constructor
	FCleverTapPropertyValue(FCleverTapPropertyValue&& Other) noexcept = default;

	// Copy assignment operator
	FCleverTapPropertyValue& operator=(const FCleverTapPropertyValue& Other) = default;

	// Move assignment operator
	FCleverTapPropertyValue& operator=(FCleverTapPropertyValue&& Other) noexcept = default;

	// Destructor
	~FCleverTapPropertyValue() = default;

	// Variant Methods
	template <typename U> bool IsType() const { return Value.IsType<U>(); }
	template <typename U> U& Get() { return Value.Get<U>(); }
	template <typename U> const U& Get() const { return Value.Get<U>(); }
	template <typename U> U* TryGet() { return Value.TryGet<U>(); }
	template <typename U> const U* TryGet() const { return Value.TryGet<U>(); }
	template <typename U> void Set(typename TIdentity<U>::Type&& InValue) { Value.Set<U>(MoveTemp(InValue)); }
	template <typename U> void Set(const typename TIdentity<U>::Type& InValue) { Value.Set<U>(InValue); }
	template <typename U, typename... TArgs> void Emplace(TArgs&&... Args)
	{
		Value.Emplace<U>(Forward<TArgs>(Args)...);
	}
	template <typename U> static constexpr SIZE_T IndexOfType() { return VariantType::IndexOfType<U>(); }
	SIZE_T GetIndex() const { return Value.GetIndex(); }

private:
	VariantType Value;
};

using FCleverTapProperties = TMap<FString, FCleverTapPropertyValue>;
