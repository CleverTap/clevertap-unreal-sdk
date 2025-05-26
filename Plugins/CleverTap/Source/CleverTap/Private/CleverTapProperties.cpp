// Copyright CleverTap All Rights Reserved.
#include "CleverTapProperties.h"

#include "CleverTapLog.h"

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

FCleverTapProperties FCleverTapProperties::MakeFromObject(const UObject* Source)
{
	if (IsValid(Source) == false)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("FCleverTapProperties::MakeFromObject: Invalid Source UObject instance!"));
		return {};
	}

	FCleverTapProperties Out;
	Out.LoadFromStruct(Source->GetClass(), Source);
	return Out;
}

void FCleverTapProperties::LoadFromObject(const UObject* Source)
{
	if (IsValid(Source) == false)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("FCleverTapProperties::LoadFromObject: Invalid Source UObject instance!"));
		return;
	}

	LoadFromStruct(Source->GetClass(), Source);
}

void FCleverTapProperties::ApplyToObject(UObject* Target) const
{
	if (IsValid(Target) == false)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("FCleverTapProperties::ApplyToObject: Invalid Target UObject instance!"));
		return;
	}

	ApplyToStruct(Target->GetClass(), Target);
}

void FCleverTapProperties::MergeFrom(const FCleverTapProperties& Other)
{
	for (const auto& KVP : Other.Map)
	{
		Map.Emplace(KVP.Key, KVP.Value);
	}
}

bool FCleverTapProperties::ShouldSkipProperty(const FProperty* Property)
{
	check(Property);
	constexpr EPropertyFlags SkipFlags = CPF_Transient | CPF_Deprecated | CPF_DisableEditOnInstance | CPF_EditorOnly;
	return Property->HasAnyPropertyFlags(SkipFlags);
}

void FCleverTapProperties::LoadFromStruct(const UStruct* StructDef, const void* SourceStructInstance)
{
	check(StructDef);
	check(SourceStructInstance);
	for (TFieldIterator<FProperty> It(StructDef); It; ++It)
	{
		FProperty* Property = *It;
		if (ShouldSkipProperty(Property))
		{
			continue;
		}

		const FString Name = Property->GetName();
		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(SourceStructInstance);

		if (auto* BoolProp = CastField<FBoolProperty>(Property))
		{
			Map.Emplace(Name, BoolProp->GetPropertyValue(ValuePtr));
		}
		else if (auto* StringProp = CastField<FStrProperty>(Property))
		{
			Map.Emplace(Name, StringProp->GetPropertyValue(ValuePtr));
		}
		else if (auto* IntProp = CastField<FIntProperty>(Property))
		{
			Map.Emplace(Name, IntProp->GetPropertyValue(ValuePtr));
		}
		else if (auto* Int64Prop = CastField<FInt64Property>(Property))
		{
			Map.Emplace(Name, Int64Prop->GetPropertyValue(ValuePtr));
		}
		else if (auto* FloatProp = CastField<FFloatProperty>(Property))
		{
			Map.Emplace(Name, FloatProp->GetPropertyValue(ValuePtr));
		}
		else if (auto* DoubleProp = CastField<FDoubleProperty>(Property))
		{
			Map.Emplace(Name, DoubleProp->GetPropertyValue(ValuePtr));
		}
		else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			if (StructProp->Struct == FCleverTapDate::StaticStruct())
			{
				const FCleverTapDate* DatePtr = static_cast<const FCleverTapDate*>(ValuePtr);
				Map.Emplace(Name, *DatePtr);
			}
			else
			{
				// todo log unsupported struct type in property
			}
		}
		else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
		{
			if (FStrProperty* InnerStrProp = CastField<FStrProperty>(ArrayProp->Inner))
			{
				FScriptArrayHelper Helper(ArrayProp, ValuePtr);

				TArray<FString> Values;
				Values.Empty(Helper.Num());
				for (int32 i = 0; i < Helper.Num(); ++i)
				{
					FString Element = InnerStrProp->GetPropertyValue(Helper.GetRawPtr(i));
					Values.Add(Element);
				}
				Map.Emplace(Name, Values);
			}
			else
			{
				// todo: log unsupported array type
			}
		}
		else
		{
			// todo: unsupported property type; log / maybe fallback to string if possible
		}
	}
}

void FCleverTapProperties::ApplyToStruct(const UStruct* StructDef, void* TargetStructInstance) const
{
	check(StructDef);
	check(TargetStructInstance);
	for (TFieldIterator<FProperty> It(StructDef); It; ++It)
	{
		FProperty* Property = *It;
		ApplyPropertyToStruct(StructDef, Property, TargetStructInstance);
	}
}

void FCleverTapProperties::ApplyPropertyToStruct(
	const UStruct* StructDef, FProperty* Property, void* TargetStructInstance) const
{
	check(StructDef);
	check(Property);
	check(TargetStructInstance);

	if (ShouldSkipProperty(Property))
	{
		// this property has been excluded from load/apply
		return;
	}

	const FCleverTapPropertyValue* SourceValue = Map.Find(Property->GetName());
	if (!SourceValue)
	{
		// we dont have an entry in the map for this property
		return;
	}

	// The target Value
	void* TargetValuePtr = Property->ContainerPtrToValuePtr<void>(TargetStructInstance);

	// Each FProperty type needs custom handling
	if (auto* BoolProp = CastField<FBoolProperty>(Property))
	{
		if (SourceValue->IsType<bool>())
		{
			BoolProp->SetPropertyValue(TargetValuePtr, SourceValue->Get<bool>());
			return;
		}
	}
	else if (auto* StringProp = CastField<FStrProperty>(Property))
	{
		if (SourceValue->IsType<FString>())
		{
			StringProp->SetPropertyValue(TargetValuePtr, SourceValue->Get<FString>());
			return;
		}
	}
	else if (auto* IntProp = CastField<FIntProperty>(Property))
	{
		if (SourceValue->IsType<int32>())
		{
			IntProp->SetPropertyValue(TargetValuePtr, SourceValue->Get<int32>());
			return;
		}
	}
	else if (auto* Int64Prop = CastField<FInt64Property>(Property))
	{
		if (SourceValue->IsType<int64>())
		{
			Int64Prop->SetPropertyValue(TargetValuePtr, SourceValue->Get<int64>());
			return;
		}
	}
	else if (auto* FloatProp = CastField<FFloatProperty>(Property))
	{
		if (SourceValue->IsType<float>())
		{
			Int64Prop->SetPropertyValue(TargetValuePtr, SourceValue->Get<float>());
			return;
		}
	}
	else if (auto* DoubleProp = CastField<FFloatProperty>(Property))
	{
		if (SourceValue->IsType<double>())
		{
			Int64Prop->SetPropertyValue(TargetValuePtr, SourceValue->Get<double>());
			return;
		}
	}
	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		if (StructProp->Struct == FCleverTapDate::StaticStruct())
		{
			FCleverTapDate* TargetDatePtr = static_cast<FCleverTapDate*>(TargetValuePtr);
			if (SourceValue->IsType<FCleverTapDate>())
			{
				*TargetDatePtr = SourceValue->Get<FCleverTapDate>();
				return;
			}
			else
			{
				// todo support setting a date from an int here
			}
		}
		else
		{
			// todo: log unsupported struct property type
		}
	}
	else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
	{
		if (FStrProperty* InnerStrProp = CastField<FStrProperty>(ArrayProp->Inner))
		{
			if (SourceValue->IsType<TArray<FString>>())
			{
				const TArray<FString>& Values = SourceValue->Get<TArray<FString>>();
				FScriptArrayHelper Helper(ArrayProp, TargetValuePtr);
				Helper.Resize(Values.Num());
				for (int32 i = 0; i < Values.Num(); ++i)
				{
					InnerStrProp->SetPropertyValue(Helper.GetRawPtr(i), Values[i]);
				}
			}
			else
			{
				// todo not a string; should we convert to string here?
			}
		}
		else
		{
			// todo: log target property has unsupported element type
		}
	}
	else
	{
		// todo: log unsupported property type
		return;
	}

	// todo: log unsupported type mismatch
}
