// Copyright CleverTap All Rights Reserved.
#include "CleverTapPropertiesBlueprintLibrary.h"

#include "CleverTapLog.h"
#include "Misc/CString.h"

FCleverTapProperties UCleverTapPropertiesBlueprintLibrary::MakePropertiesFromObject(UObject* Source)
{
	return FCleverTapProperties::MakeFromObject(Source);
}

void UCleverTapPropertiesBlueprintLibrary::ApplyPropertiesToObject(
	const FCleverTapProperties& Properties, UObject* Target)
{
	Properties.ApplyToObject(Target);
}

int32 UCleverTapPropertiesBlueprintLibrary::GetPropertyCount(const FCleverTapProperties& Properties)
{
	return Properties.Map.Num();
}

TArray<FString> UCleverTapPropertiesBlueprintLibrary::GetKeys(const FCleverTapProperties& Properties)
{
	TArray<FString> Keys;
	Properties.Map.GetKeys(Keys);
	return Keys;
}

FString UCleverTapPropertiesBlueprintLibrary::ConvertPropertiesToDebugString(const FCleverTapProperties& Properties)
{
	return ToDebugString(Properties);
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
	if (Value == nullptr)
	{
		return DefaultValue;
	}
	if (Value->IsType<FCleverTapDate>())
	{
		return Value->Get<FCleverTapDate>();
	}
	else if (Value->IsType<int64>())
	{
		return FCleverTapDate::MakeFromUnixTimestamp(Value->Get<int64>());
	}
	else if (Value->IsType<int32>())
	{
		return FCleverTapDate::MakeFromUnixTimestamp(int64(Value->Get<int32>()));
	}
	else
	{
		return DefaultValue;
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

bool UCleverTapPropertiesBlueprintLibrary::HasDateCompatibleProperty(
	const FCleverTapProperties& Properties, const FString& Key)
{
	auto* Value = Properties.Map.Find(Key);
	return Value && (Value->IsType<FCleverTapDate>() || Value->IsType<int64>() || Value->IsType<int32>());
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

void UCleverTapPropertiesBlueprintLibrary::ClearProperties(UPARAM(ref) FCleverTapProperties& Properties)
{
	Properties.Map.Empty();
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

void UCleverTapPropertiesBlueprintLibrary::SetDoubleProperty(
	FCleverTapProperties& Properties, const FString& Key, FCleverTapDoubleWrapper Value)
{
	Properties.Map.Emplace(Key, Value.Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetIntProperty(
	FCleverTapProperties& Properties, const FString& Key, int32 Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetInt64Property(
	FCleverTapProperties& Properties, const FString& Key, int64 Value)
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

void UCleverTapPropertiesBlueprintLibrary::SetIntArrayProperty(
	FCleverTapProperties& Properties, const FString& Key, const TArray<int32>& Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetInt64ArrayProperty(
	FCleverTapProperties& Properties, const FString& Key, const TArray<int64>& Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetFloatArrayProperty(
	FCleverTapProperties& Properties, const FString& Key, const TArray<float>& Value)
{
	Properties.Map.Emplace(Key, Value);
}

void UCleverTapPropertiesBlueprintLibrary::SetDoubleArrayProperty(
	FCleverTapProperties& Properties, const FString& Key, const TArray<FCleverTapDoubleWrapper>& Value)
{
	TArray<double> Unwrapped;
	Unwrapped.Reserve(Value.Num());

	for (const auto& Wrapper : Value)
	{
		Unwrapped.Add(Wrapper.Value);
	}

	Properties.Map.Emplace(Key, Unwrapped);
}

void UCleverTapPropertiesBlueprintLibrary::SetBoolArrayProperty(
	FCleverTapProperties& Properties, const FString& Key, const TArray<bool>& Value)
{
	Properties.Map.Emplace(Key, Value);
}

int64 UCleverTapPropertiesBlueprintLibrary::Conv_StringToInt64(const FString& InString)
{
	return FCString::Atoi64(*InString);
}

FCleverTapDate UCleverTapPropertiesBlueprintLibrary::Conv_StringToDate(const FString& InString)
{
	// Replace -:. characters with space
	FString ReplacedString = InString.Replace(TEXT("-"), TEXT(" "));
	ReplacedString.ReplaceInline(TEXT(":"), TEXT(" "));
	ReplacedString.ReplaceInline(TEXT("."), TEXT(" "));

	// Split on space
	TArray<FString> Parts{};
	InString.ParseIntoArray(Parts, TEXT(" "));
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

FCleverTapDoubleWrapper UCleverTapPropertiesBlueprintLibrary::Conv_StringToDouble(const FString& InString)
{
	if (!InString.IsNumeric())
	{
		UE_LOG(LogCleverTap, Warning, TEXT("String not a valid double format: %s"), *InString);
		return FCleverTapDoubleWrapper{};
	}

	const double Value = FCString::Atod(*InString);
	return FCleverTapDoubleWrapper{ Value };
}
