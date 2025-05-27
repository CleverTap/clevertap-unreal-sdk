// Copyright CleverTap All Rights Reserved.
#include "CleverTapInstance.h"

#include "CleverTapLog.h"
#include "CleverTapUtilities.h"

FString UCleverTapInstance::GetPropertyAsString(const FString& Key, const FString& DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet())
	{
		return MaybeValue.GetValue().GetDebugString(); // TODO fix this
	}
	else
	{
		return DefaultValue;
	}
}

bool UCleverTapInstance::GetBoolProperty(const FString& Key, bool DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<bool>())
	{
		return MaybeValue.GetValue().Get<bool>();
	}
	else
	{
		return DefaultValue;
	}
}

FCleverTapDate UCleverTapInstance::GetDateProperty(const FString& Key, const FCleverTapDate& DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<FCleverTapDate>())
	{
		return MaybeValue.GetValue().Get<FCleverTapDate>();
	}
	else
	{
		return DefaultValue;
	}
}

double UCleverTapInstance::GetDoubleProperty(const FString& Key, double DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<double>())
	{
		return MaybeValue.GetValue().Get<double>();
	}
	else
	{
		return DefaultValue;
	}
}

float UCleverTapInstance::GetFloatProperty(const FString& Key, float DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<float>())
	{
		return MaybeValue.GetValue().Get<float>();
	}
	else
	{
		return DefaultValue;
	}
}

int32 UCleverTapInstance::GetIntProperty(const FString& Key, int32 DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<int32>())
	{
		return MaybeValue.GetValue().Get<int32>();
	}
	else
	{
		return DefaultValue;
	}
}

int64 UCleverTapInstance::GetInt64Property(const FString& Key, int64 DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<int64>())
	{
		return MaybeValue.GetValue().Get<int64>();
	}
	else
	{
		return DefaultValue;
	}
}

FString UCleverTapInstance::GetStringProperty(const FString& Key, const FString& DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<FString>())
	{
		return MaybeValue.GetValue().Get<FString>();
	}
	else
	{
		return DefaultValue;
	}
}

TArray<FString> UCleverTapInstance::GetStringArrayProperty(const FString& Key, const TArray<FString>& DefaultValue)
{
	auto MaybeValue = GetProperty(Key);
	if (MaybeValue.IsSet() && MaybeValue.GetValue().IsType<TArray<FString>>())
	{
		return MaybeValue.GetValue().Get<TArray<FString>>();
	}
	else
	{
		return DefaultValue;
	}
}

bool UCleverTapInstance::HasProperty(const FString& Key)
{
	return GetProperty(Key).IsSet();
}

bool UCleverTapInstance::HasBoolProperty(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<bool>();
}

bool UCleverTapInstance::HasDateProperty(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<FCleverTapDate>();
}

bool UCleverTapInstance::HasDoubleProperty(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<double>();
}

bool UCleverTapInstance::HasFloatProperty(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<float>();
}

bool UCleverTapInstance::HasIntProperty(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<int32>();
}

bool UCleverTapInstance::HasInt64Property(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<int64>();
}

bool UCleverTapInstance::HasStringProperty(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<FString>();
}

bool UCleverTapInstance::HasStringArrayProperty(const FString& Key)
{
	auto MaybeValue = GetProperty(Key);
	return MaybeValue.IsSet() && MaybeValue.GetValue().IsType<TArray<FString>>();
}

void UCleverTapInstance::ApplyProfileToObject(UObject* Target)
{
	if (IsValid(Target) == false)
	{
		UE_LOG(
			LogCleverTap, Warning, TEXT("UCleverTapInstance::ApplyProfileToObject: Invalid Target UObject instance!"));
		return;
	}

	ApplyProfileToStruct(Target->GetClass(), Target);
}

void UCleverTapInstance::ApplyProfileToStruct(const UStruct* StructDef, void* TargetStructInstance)
{
	check(StructDef);
	check(TargetStructInstance);
	for (TFieldIterator<FProperty> It(StructDef); It; ++It)
	{
		FProperty* Property = *It;
		if (FCleverTapProperties::ShouldSkipProperty(Property))
		{
			// this property has been excluded from load/apply
			continue;
		}

		auto MaybeValue = GetProperty(Property->GetName());
		if (MaybeValue.IsSet() == false)
		{
			// the user profile doesn't have this property
			continue;
		}

		ApplyCleverTapPropertyValueToStruct(MaybeValue.GetValue(), StructDef, Property, TargetStructInstance);
	}
}
