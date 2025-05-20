// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/TVariant.h"
#include "CleverTapProperties.generated.h"

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
USTRUCT(BlueprintType)
struct CLEVERTAP_API FCleverTapPropertyValue
{
	GENERATED_BODY()

	using VariantType = TVariant<int32, int64, float, double, bool, FString, FCleverTapDate, TArray<int32>,
		TArray<int64>, TArray<float>, TArray<double>, TArray<bool>, TArray<FString>>;

	// Default constructors
	FCleverTapPropertyValue() = default;

	// Type-specific constructors
	FCleverTapPropertyValue(int32 InValue) : Value(TInPlaceType<int32>(), InValue) {}
	FCleverTapPropertyValue(int64 InValue) : Value(TInPlaceType<int64>(), InValue) {}
	FCleverTapPropertyValue(double InValue) : Value(TInPlaceType<double>(), InValue) {}
	FCleverTapPropertyValue(float InValue) : Value(TInPlaceType<float>(), InValue) {}
	FCleverTapPropertyValue(bool InValue) : Value(TInPlaceType<bool>(), InValue) {}
	FCleverTapPropertyValue(const ANSICHAR* InValue) : Value(TInPlaceType<FString>(), FString(InValue)) {}
	FCleverTapPropertyValue(const FString& InValue) : Value(TInPlaceType<FString>(), InValue) {}
	FCleverTapPropertyValue(FString&& InValue) : Value(TInPlaceType<FString>(), MoveTemp(InValue)) {}
	FCleverTapPropertyValue(const FCleverTapDate& InValue) : Value(TInPlaceType<FCleverTapDate>(), InValue) {}
	FCleverTapPropertyValue(const TArray<int32>& InValue) : Value(TInPlaceType<TArray<int32>>(), InValue) {}
	FCleverTapPropertyValue(TArray<int32>&& InValue) : Value(TInPlaceType<TArray<int32>>(), MoveTemp(InValue)) {}
	FCleverTapPropertyValue(const TArray<int64>& InValue) : Value(TInPlaceType<TArray<int64>>(), InValue) {}
	FCleverTapPropertyValue(TArray<int64>&& InValue) : Value(TInPlaceType<TArray<int64>>(), MoveTemp(InValue)) {}
	FCleverTapPropertyValue(const TArray<float>& InValue) : Value(TInPlaceType<TArray<float>>(), InValue) {}
	FCleverTapPropertyValue(TArray<float>&& InValue) : Value(TInPlaceType<TArray<float>>(), MoveTemp(InValue)) {}
	FCleverTapPropertyValue(const TArray<double>& InValue) : Value(TInPlaceType<TArray<double>>(), InValue) {}
	FCleverTapPropertyValue(TArray<double>&& InValue) : Value(TInPlaceType<TArray<double>>(), MoveTemp(InValue)) {}
	FCleverTapPropertyValue(const TArray<bool>& InValue) : Value(TInPlaceType<TArray<bool>>(), InValue) {}
	FCleverTapPropertyValue(TArray<bool>&& InValue) : Value(TInPlaceType<TArray<bool>>(), MoveTemp(InValue)) {}
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
	// clang-format off
	template <typename U> bool IsType() const { return Value.IsType<U>(); }
	template <typename U> U& Get() { return Value.Get<U>(); }
	template <typename U> const U& Get() const { return Value.Get<U>(); }
	template <typename U> U* TryGet() { return Value.TryGet<U>(); }
	template <typename U> const U* TryGet() const { return Value.TryGet<U>(); }
	template <typename U> void Set(typename TIdentity<U>::Type&& InValue) { Value.Set<U>(MoveTemp(InValue)); }
	template <typename U> void Set(const typename TIdentity<U>::Type& InValue) { Value.Set<U>(InValue); }
	template <typename U, typename... TArgs> void Emplace(TArgs&&... Args) { Value.Emplace<U>(Forward<TArgs>(Args)...);	}
	template <typename U> static constexpr SIZE_T IndexOfType() { return VariantType::IndexOfType<U>(); }
	// clang-format on

	/** Returns the Index of the currently loaded type; as per IndexOfType<T>() */
	SIZE_T GetIndex() const { return Value.GetIndex(); }

	/** Returns a debug string describing the property's value and type. */
	FString GetDebugString() const;

private:
	VariantType Value;
};

/**
 * A generic map of property keys & values.
 *
 * todo documentation needed here
 */
USTRUCT(BlueprintType)
struct CLEVERTAP_API FCleverTapProperties
{
	GENERATED_BODY()

	/** Map of keys to values. Safe to access directly from C++ */
	UPROPERTY()
	TMap<FString, FCleverTapPropertyValue> Map;

	/** Create a properties map populated with the properties from a UObject */
	static FCleverTapProperties MakeFromObject(const UObject* Source);

	/** Create a properties map populated with the properties from UStruct-type T */
	template <typename T>
	static FCleverTapProperties MakeFromStruct(const T& Source);

	/** Load all properties from a UObject (overwriting existing values) */
	void LoadFromObject(const UObject* Source);

	/** Load all properties from a UStruct (overwriting existing values) */
	template <typename T>
	void LoadFromStruct(const T& Source);

	/** Apply all matching properties to a UObject */
	void ApplyToObject(UObject* Target) const;

	/** Apply all matching properties to a UStruct-based type T */
	template <typename T>
	void ApplyToStruct(T* Source) const;

	/** Merge in values from another properties object (overwriting existing values) */
	void MergeFrom(const FCleverTapProperties& Other);

private:
	/** Returns true if this property should be skipped when loading/applying
	 *
	  (because it's marked Transent, Deprecated, DisableEditOnInstance or EditorOnly etc)
	 */
	static bool ShouldSkipProperty(const FProperty* Property);

	/** Load all properties from a generic UStruct (overwrites existing values) */
	void LoadFromStruct(const UStruct* StructDef, const void* SourceStructInstance);

	/** Apply properties to a generic UStruct */
	void ApplyToStruct(const UStruct* StructDef, void* TargetStructInstance) const;

	/** Set the given Property on TargetStructInstance if this map has a value for that key. */
	void ApplyPropertyToStruct(const UStruct* StructDef, FProperty* Prop, void* TargetStructInstance) const;
};

/** Returns a debug string describing the property's value and type. */
CLEVERTAP_API FString ToDebugString(const FCleverTapPropertyValue& Value);

/** Returns a debug string describing the property's value and type, or "<not set>" if not set. */
CLEVERTAP_API FString ToDebugString(const TOptional<FCleverTapPropertyValue>& OptionalValue);

/** Returns a debug string listing all properties with their keys, types, and values. */
CLEVERTAP_API FString ToDebugString(const FCleverTapProperties& Properties);

/** Returns a debug string listing all properties with their keys, types, and values. */
CLEVERTAP_API FString ToDebugString(const TArray<FCleverTapProperties>& PropertiesArray);

// ================= inlines =================

template <typename T>
inline FCleverTapProperties FCleverTapProperties::MakeFromStruct(const T& Source)
{
	FCleverTapProperties Out;
	Out.LoadFromStruct(TBaseStructure<T>::Get(), &Source);
	return Out;
}

template <typename T>
inline void FCleverTapProperties::LoadFromStruct(const T& Source)
{
	LoadFromStruct(TBaseStructure<T>::Get(), &Source);
}

template <typename T>
inline void FCleverTapProperties::ApplyToStruct(T* Target) const
{
	ApplyToStruct(TBaseStructure<T>::Get(), Target);
}
