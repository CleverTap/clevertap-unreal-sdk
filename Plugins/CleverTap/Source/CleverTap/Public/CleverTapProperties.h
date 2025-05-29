// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapDate.h"

#include "CoreMinimal.h"
#include "Misc/TVariant.h"
#include "CleverTapProperties.generated.h"

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

	/** Returns the Name of the currently loaded type as a string (intended for logging). */
	FString GetTypeName() const;

	/** Returns the value converted to a string. */
	FString ToString() const;

	/** Returns a debug string describing the property's value and type. */
	FString ToDebugString() const;

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

	/** Copy all matching properties to a UObject */
	void ApplyToObject(UObject* Target) const;

	/** Copy all matching properties to a UStruct-based type T */
	template <typename T>
	void ApplyToStruct(T* Target) const;

	/** Merge in values from another properties object (overwriting existing values) */
	void MergeFrom(const FCleverTapProperties& Other);

	/** Returns true if this property should be skipped when applying or loading values.
	 *
	 *  Skips properties that are:
	 *  - Transient, Deprecated, EditorOnly, or DisableEditOnInstance
	 */
	static bool ShouldSkipProperty(const FProperty* Property);

private:
	/** Load all properties from a generic UStruct (overwrites existing values) */
	void LoadFromStruct(const UStruct* StructDef, const void* SourceStructInstance);

	/** Apply properties to a generic UStruct */
	void ApplyToStruct(const UStruct* StructDef, void* TargetStructInstance) const;

	/** Set the given Property on TargetStructInstance if this map has a value for that key. */
	void ApplyPropertyToStruct(const UStruct* StructDef, FProperty* Prop, void* TargetStructInstance) const;
};

/** Sets the given Value into the Property on TargetStructInstance described by StructDef */
void ApplyCleverTapPropertyValueToStruct(
	const FCleverTapPropertyValue& Value, const UStruct* StructDef, FProperty* Property, void* TargetStructInstance);

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
