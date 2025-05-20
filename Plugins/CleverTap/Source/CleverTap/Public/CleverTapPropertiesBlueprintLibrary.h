// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"
#include "CleverTapPropertiesBlueprintLibrary.generated.h"

UCLASS()
class UCleverTapPropertiesBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Create a FCleverTapProperties populated from the properties of the Source UObject  */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static FCleverTapProperties MakePropertiesFromObject(UObject* Source);

	/** Copies all matching properties in Properties to the Target UObject.  */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void ApplyPropertiesToObject(const FCleverTapProperties& Properties, UObject* Target);

	/** Returns true if Properties contains a property with the given Key.  */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns true if Properties contains a string property with the given Key.  */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static bool HasStringProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns the value of the keyed property formatted as a String.
	 * Returns an empty string if no such Key.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static FString GetPropertyAsString(const FCleverTapProperties& Properties, const FString& Key);

	/** Returns the value of a String property, or an empty string if no such property or not a string property.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static FString GetStringProperty(const FCleverTapProperties& Properties, const FString& Key);

	/** Sets a property with the new string value.
	 */
	UFUNCTION(BlueprintCallable, Category = "CleverTap|Properties")
	static void SetStringProperty(
		UPARAM(ref) FCleverTapProperties& Properties, const FString& Key, const FString& Value);
};
