// Copyright CleverTap All Rights Reserved.
#include "CleverTapProperties.h"

namespace {

template <typename T>
const TCHAR* DebugNameForType();

template <>
const TCHAR* DebugNameForType<int32>()
{
	return TEXT("int32");
}

template <>
const TCHAR* DebugNameForType<int64>()
{
	return TEXT("int64");
}

template <>
const TCHAR* DebugNameForType<float>()
{
	return TEXT("float");
}

template <>
const TCHAR* DebugNameForType<double>()
{
	return TEXT("double");
}

template <>
const TCHAR* DebugNameForType<bool>()
{
	return TEXT("bool");
}

template <>
const TCHAR* DebugNameForType<FString>()
{
	return TEXT("string");
}

FString ToString(int32 Value)
{
	return FString::Printf(TEXT("%d"), Value);
}

FString ToString(int64 Value)
{
	return FString::Printf(TEXT("%lld"), Value);
}

FString ToString(float Value)
{
	return FString::Printf(TEXT("%f"), Value);
}

FString ToString(double Value)
{
	return FString::Printf(TEXT("%f"), Value);
}

FString ToString(bool Value)
{
	return FString::Printf(TEXT("%s"), Value ? TEXT("true") : TEXT("false"));
}

FString ToString(const FString& Value)
{
	return FString::Printf(TEXT("\"%s\""), *Value);
}

template <typename T>
FString ToDebugString(const T& Value)
{
	return FString::Printf(TEXT("%s(%s)"), DebugNameForType<T>(), *ToString(Value));
}

template <typename T>
FString ToDebugString(const TArray<T>& Array)
{
	FString Result = FString::Printf(TEXT("Array<%s>[ "), DebugNameForType<T>());
	for (int Index = 0; Index < Array.Num(); Index++)
	{
		if (Index != 0)
		{
			Result += TEXT(", ");
		}
		Result += ToString(Array[Index]);
	}
	Result += TEXT(" ]");
	return Result;
}

template <>
FString ToDebugString(const FCleverTapDate& Value)
{
	return FString::Printf(TEXT("date(y=%d, m=%d, d=%d)"), Value.Year, Value.Month, Value.Day);
}

} // namespace

FString FCleverTapPropertyValue::GetDebugString() const
{
	switch (Value.GetIndex())
	{
		case IndexOfType<int32>():
			return FString::Printf(TEXT("int32(%d)"), Value.Get<int32>());

		case IndexOfType<int64>():
			return FString::Printf(TEXT("int64(%lld)"), Value.Get<int64>());

		case IndexOfType<float>():
			return FString::Printf(TEXT("float(%f)"), Value.Get<float>());

		case IndexOfType<double>():
			return FString::Printf(TEXT("double(%f)"), Value.Get<double>());

		case IndexOfType<bool>():
			return FString::Printf(TEXT("bool(%s)"), *ToString(Value.Get<bool>()));

		case IndexOfType<FString>():
			return FString::Printf(TEXT("string(\"%s\")"), *Value.Get<FString>());

		case IndexOfType<FCleverTapDate>():
			return ToDebugString(Value.Get<FCleverTapDate>());

		case IndexOfType<TArray<int32>>():
			return ToDebugString(Value.Get<TArray<int32>>());

		case IndexOfType<TArray<int64>>():
			return ToDebugString(Value.Get<TArray<int64>>());

		case IndexOfType<TArray<float>>():
			return ToDebugString(Value.Get<TArray<float>>());

		case IndexOfType<TArray<double>>():
			return ToDebugString(Value.Get<TArray<double>>());

		case IndexOfType<TArray<bool>>():
			return ToDebugString(Value.Get<TArray<bool>>());

		case IndexOfType<TArray<FString>>():
			return ToDebugString(Value.Get<TArray<FString>>());

		default:
			return TEXT("unknown");
	}
}

FString ToDebugString(const FCleverTapPropertyValue& Value)
{
	return Value.GetDebugString();
}

FString ToDebugString(const TOptional<FCleverTapPropertyValue>& OptionalValue)
{
	if (OptionalValue.IsSet())
	{
		return ToDebugString(OptionalValue.GetValue());
	}
	else
	{
		return TEXT("<not set>");
	}
}

FString ToDebugString(const FCleverTapProperties& Properties)
{
	FString Result = TEXT("{ ");
	int Count = 0;
	for (const auto& KVP : Properties.Map)
	{
		if (Count++)
		{
			Result += TEXT(", ");
		}
		Result += FString::Printf(TEXT("\"%s\"=%s"), *KVP.Key, *KVP.Value.GetDebugString());
	}
	Result += TEXT(" }");
	return Result;
}

FString ToDebugString(const TArray<FCleverTapProperties>& PropertiesArray)
{
	FString Result = TEXT("[ ");
	int Count = 0;
	for (const auto& Properties : PropertiesArray)
	{
		if (Count++)
		{
			Result += TEXT(", ");
		}
		Result += ToDebugString(Properties);
	}
	Result += TEXT(" ]");
	return Result;
}
