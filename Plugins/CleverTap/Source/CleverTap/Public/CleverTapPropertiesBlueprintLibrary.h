// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CleverTapPropertiesBlueprintLibrary.generated.h"

/**
 * Unreal doesn't natively support double types in blueprint so this wrapper can be used to get around doubles being
 *  exposed to Blueprint.
 */
USTRUCT(BlueprintType, meta = (HasNativeMake = "CleverTap.CleverTapPropertiesBlueprintLibrary.Conv_StringToDouble"))
struct CLEVERTAP_API FCleverTapDoubleWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	double Value{};
};

/**
 * Blueprint Function Library for various utility methods when interacting with CleverTap types
 */
UCLASS()
class CLEVERTAP_API UCleverTapPropertiesBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Create a FCleverTapProperties populated from the properties of the Source UObject  */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static FCleverTapProperties MakePropertiesFromObject(UObject* Source);

	/** Copies all matching properties in Properties to the Target UObject.  */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void ApplyPropertiesToObject(const FCleverTapProperties& Properties, UObject* Target);

	/** Get the number of property entries. */
	UFUNCTION(BlueprintPure, Category = "CleverTap|Properties")
	static int32 GetPropertyCount(const FCleverTapProperties& Properties);

	/** Get the keys for each property entry. */
	UFUNCTION(BlueprintPure, Category = "CleverTap|Properties")
	static TArray<FString> GetKeys(const FCleverTapProperties& Properties);

	/** Convert the properties to a debug string. */
	UFUNCTION(BlueprintPure, Category = "CleverTap|Properties")
	static FString ConvertPropertiesToDebugString(const FCleverTapProperties& Properties);

	/** Returns the value of a property formatted as a String, or DefaultValue if the property does not exist. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static FString GetPropertyAsString(
		const FCleverTapProperties& Properties, const FString& Key, const FString& DefaultValue);

	/** Returns the value of a Boolean property, or DefaultValue if the property is missing or not a Boolean. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool GetBoolProperty(const FCleverTapProperties& Properties, const FString& Key, bool DefaultValue);

	/** Returns the value of a Date property, or DefaultValue if the property is missing or not convertible to a Date.
	 *
	 * Note: If the property holds an int32 or an int64 it will be treated as a unix timestamp and converted to
	 *       an FCleverTapDate. See HasDateCompatibleProperty().
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static FCleverTapDate GetDateProperty(
		const FCleverTapProperties& Properties, const FString& Key, const FCleverTapDate& DefaultValue);

	/** Returns the value of a Float property, or DefaultValue if the property is missing or not a Float.
	 *
	 *  Note: FCleverTapPropertyValue supports 64-bit doubles, but Blueprint only supports 32-bit floats.
	 *        64-bit values must be accessed from C++.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static float GetFloatProperty(const FCleverTapProperties& Properties, const FString& Key, float DefaultValue);

	/** Returns the value of a 32-bit Integer property, or DefaultValue if the property is missing or not a 32-bit
	 *  Integer.
	 *
	 *  Note: FCleverTapPropertyValue supports 64-bit integers, but Blueprint does not.
	 *        64-bit values must be accessed from C++.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static int32 GetIntProperty(const FCleverTapProperties& Properties, const FString& Key, int32 DefaultValue);

	/** Returns the value of a String property, or DefaultValue if the property is missing or not a String. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static FString GetStringProperty(
		const FCleverTapProperties& Properties, const FString& Key, const FString& DefaultValue);

	/** Returns the value of a String array property, or DefaultValue if the property is missing or not an array of
	 *  Strings. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static TArray<FString> GetStringArrayProperty(
		const FCleverTapProperties& Properties, const FString& Key, const TArray<FString>& DefaultValue);

	/** Returns true if Properties contains a property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a Boolean property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasBoolProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a Date property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasDateProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains this key with a date or a value that can be converted to a date.
	 *
	 *  int32 and int64 values are treated as Unix timestamps & considered date compatible.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasDateCompatibleProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a 64-bit Double property with the given Key.
	 *
	 *  Note: Double values cannot be accessed from Blueprint — C++ only.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasDoubleProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a Float property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasFloatProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a 32-bit Integer property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasIntProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a 64-bit Integer property with the given Key.
	 *
	 *  Note: 64-bit Integer values cannot be accessed from Blueprint — C++ only.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasInt64Property(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a String property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasStringProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a String Array property with the given Key. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasStringArrayProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Removes all properties. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void ClearProperties(UPARAM(ref) FCleverTapProperties& Properties);

	/** Stores the given Value under the specified Key as a Boolean property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetBoolProperty(UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, bool Value);

	/** Stores the given Value under the specified Key as a Date property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetDateProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const FCleverTapDate& Value);

	/** Stores the given Value under the specified Key as a Float property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetFloatProperty(UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, float Value);

	/** Stores the given Value under the specified Key as a Float property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetDoubleProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, FCleverTapDoubleWrapper Value);

	/** Stores the given Value under the specified Key as an Integer property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetIntProperty(UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, int32 Value);

	/** Stores the given Value under the specified Key as a 64-bit Integer property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetInt64Property(UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, int64 Value);

	/** Stores the given Value under the specified Key as a String property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetStringProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const FString& Value);

	/** Stores the given Value under the specified Key as a String Array property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetStringArrayProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const TArray<FString>& Value);

	/** Stores the given Value under the specified Key as an Integer Array property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetIntArrayProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const TArray<int32>& Value);

	/** Stores the given Value under the specified Key as a 64-bit Integer Array property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetInt64ArrayProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const TArray<int64>& Value);

	/** Stores the given Value under the specified Key as an Float Array property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetFloatArrayProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const TArray<float>& Value);

	/** Stores the given Value under the specified Key as an Double Array property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetDoubleArrayProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const TArray<FCleverTapDoubleWrapper>& Value);

	/** Stores the given Value under the specified Key as an Boolean Array property. */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetBoolArrayProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const TArray<bool>& Value);

	/** Convert a FString -> Int64 (missing from Unreal) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Int64", CompactNodeTitle = "->", BlueprintAutocast),
		Category = "CleverTap|Utilities")
	static int64 Conv_StringToInt64(const FString& InString);

	/** Convert a FString -> FCleverTapDoubleWrapper */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Double", CompactNodeTitle = "->", BlueprintAutocast),
		Category = "CleverTap|Utilities")
	static FCleverTapDoubleWrapper Conv_StringToDouble(const FString& InString);
};
