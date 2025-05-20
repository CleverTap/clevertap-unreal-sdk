// Copyright CleverTap All Rights Reserved.
#include "CleverTapPropertiesBlueprintLibrary.h"

FCleverTapProperties UCleverTapPropertiesBlueprintLibrary::MakePropertiesFromObject(UObject* Source)
{

	return FCleverTapProperties::MakeFromObject(Source);
}

void UCleverTapPropertiesBlueprintLibrary::ApplyPropertiesToObject(
	const FCleverTapProperties& Properties, UObject* Target)
{
	Properties.ApplyToObject(Target);
}

bool UCleverTapPropertiesBlueprintLibrary::HasProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	return Properties.Map.Contains(Key);
}

bool UCleverTapPropertiesBlueprintLibrary::HasStringProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<FString>();
}

FString UCleverTapPropertiesBlueprintLibrary::GetPropertyAsString(
	const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr)
	{
		return {};
	}
	else
	{
		return Value->GetDebugString(); // todo real ToString() cleanup support
	}
}

FString UCleverTapPropertiesBlueprintLibrary::GetStringProperty(
	const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr || Value->IsType<FString>() == false)
	{
		return {};
	}
	else
	{
		return Value->Get<FString>();
	}
}

void UCleverTapPropertiesBlueprintLibrary::SetStringProperty(
	FCleverTapProperties& Properties, const FString& Key, const FString& Value)
{
	Properties.Map.Emplace(Key, Value);
}
