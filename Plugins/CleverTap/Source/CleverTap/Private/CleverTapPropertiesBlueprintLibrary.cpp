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

FString UCleverTapPropertiesBlueprintLibrary::GetPropertyAsString(
	const FCleverTapProperties& Properties, const FString& Key, const FString& DefaultValue)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr)
	{
		return DefaultValue;
	}
	else
	{
		return Value->ToString();
	}
}

bool UCleverTapPropertiesBlueprintLibrary::GetBoolProperty(
	const FCleverTapProperties& Properties, const FString& Key, bool DefaultValue)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr || Value->IsType<bool>() == false)
	{
		return DefaultValue;
	}
	else
	{
		return Value->Get<bool>();
	}
}

FCleverTapDate UCleverTapPropertiesBlueprintLibrary::GetDateProperty(
	const FCleverTapProperties& Properties, const FString& Key, const FCleverTapDate& DefaultValue)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr || Value->IsType<FCleverTapDate>() == false)
	{
		return DefaultValue;
	}
	else
	{
		return Value->Get<FCleverTapDate>();
	}
}

float UCleverTapPropertiesBlueprintLibrary::GetFloatProperty(
	const FCleverTapProperties& Properties, const FString& Key, float DefaultValue)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr || Value->IsType<float>() == false)
	{
		return DefaultValue;
	}
	else
	{
		return Value->Get<float>();
	}
}

int32 UCleverTapPropertiesBlueprintLibrary::GetIntProperty(
	const FCleverTapProperties& Properties, const FString& Key, int32 DefaultValue)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr || Value->IsType<int32>() == false)
	{
		return DefaultValue;
	}
	else
	{
		return Value->Get<int32>();
	}
}

FString UCleverTapPropertiesBlueprintLibrary::GetStringProperty(
	const FCleverTapProperties& Properties, const FString& Key, const FString& DefaultValue)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr || Value->IsType<FString>() == false)
	{
		return DefaultValue;
	}
	else
	{
		return Value->Get<FString>();
	}
}

TArray<FString> UCleverTapPropertiesBlueprintLibrary::GetStringArrayProperty(
	const FCleverTapProperties& Properties, const FString& Key, const TArray<FString>& DefaultValue)
{
	auto* Value = Properties.Map.Find(Key);
	if (Value == nullptr || Value->IsType<TArray<FString>>() == false)
	{
		return DefaultValue;
	}
	else
	{
		return Value->Get<TArray<FString>>();
	}
}

bool UCleverTapPropertiesBlueprintLibrary::HasProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	return Properties.Map.Contains(Key);
}

bool UCleverTapPropertiesBlueprintLibrary::HasBoolProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<bool>();
}

bool UCleverTapPropertiesBlueprintLibrary::HasDateProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<FCleverTapDate>();
}

bool UCleverTapPropertiesBlueprintLibrary::HasDoubleProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<double>();
}

bool UCleverTapPropertiesBlueprintLibrary::HasFloatProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<float>();
}

bool UCleverTapPropertiesBlueprintLibrary::HasIntProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<int32>();
}

bool UCleverTapPropertiesBlueprintLibrary::HasInt64Property(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<int64>();
}

bool UCleverTapPropertiesBlueprintLibrary::HasStringProperty(const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<FString>();
}

bool UCleverTapPropertiesBlueprintLibrary::HasStringArrayProperty(
	const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && Value->IsType<TArray<FString>>();
}

void UCleverTapPropertiesBlueprintLibrary::SetBoolProperty(
	FCleverTapProperties& Properties, const FString& Key, bool Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetDateProperty(
	FCleverTapProperties& Properties, const FString& Key, const FCleverTapDate& Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetFloatProperty(
	FCleverTapProperties& Properties, const FString& Key, float Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetIntProperty(
	FCleverTapProperties& Properties, const FString& Key, int Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetStringProperty(
	FCleverTapProperties& Properties, const FString& Key, const FString& Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetStringArrayProperty(
	FCleverTapProperties& Properties, const FString& Key, const TArray<FString>& Value)
{
	Properties.Map.Emplace(Key, Value);
}
