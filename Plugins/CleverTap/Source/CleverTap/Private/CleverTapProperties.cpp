// Copyright CleverTap All Rights Reserved.
#include "CleverTapProperties.h"

#include "CleverTapLog.h"

#include "UObject/UnrealType.h"

namespace internal {

template <typename T>
const TCHAR* GetTypeName();

template <>
const TCHAR* GetTypeName<int32>()
{
	return TEXT("int32");
}

template <>
const TCHAR* GetTypeName<int64>()
{
	return TEXT("int64");
}

template <>
const TCHAR* GetTypeName<float>()
{
	return TEXT("float");
}

template <>
const TCHAR* GetTypeName<double>()
{
	return TEXT("double");
}

template <>
const TCHAR* GetTypeName<bool>()
{
	return TEXT("bool");
}

template <>
const TCHAR* GetTypeName<FString>()
{
	return TEXT("string");
}

template <>
const TCHAR* GetTypeName<FCleverTapDate>()
{
	return TEXT("date");
}

template <>
const TCHAR* GetTypeName<TArray<FString>>()
{
	return TEXT("array<string>");
}

template <>
const TCHAR* GetTypeName<TArray<bool>>()
{
	return TEXT("array<bool>");
}

template <>
const TCHAR* GetTypeName<TArray<int32>>()
{
	return TEXT("array<int32>");
}

template <>
const TCHAR* GetTypeName<TArray<int64>>()
{
	return TEXT("array<int64>");
}

template <>
const TCHAR* GetTypeName<TArray<float>>()
{
	return TEXT("array<float>");
}

template <>
const TCHAR* GetTypeName<TArray<double>>()
{
	return TEXT("array<double>");
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
	return Value;
}

FString ToString(const FCleverTapDate& Value)
{
	return Value.ToString();
}

template <typename T>
FString ToString(const TArray<T>& Array)
{
	FString Result;
	for (int Index = 0; Index < Array.Num(); Index++)
	{
		if (Index != 0)
		{
			Result += TEXT(", ");
		}
		Result += ToString(Array[Index]);
	}
	return Result;
}

template <>
FString ToString(const TArray<FString>& Array)
{
	FString Result;
	for (int Index = 0; Index < Array.Num(); Index++)
	{
		if (Index != 0)
		{
			Result += TEXT(", ");
		}
		Result += TEXT("\"");
		Result += ToString(Array[Index]);
		Result += TEXT("\"");
	}
	return Result;
}

template <typename T>
FString ToDebugString(const T& Value)
{
	return FString::Printf(TEXT("%s(%s)"), GetTypeName<T>(), *ToString(Value));
}

template <typename T>
FString ToDebugString(const TArray<T>& Array)
{
	return FString::Printf(TEXT("%s[ %s ]"), GetTypeName<TArray<T>>(), *ToString(Array));
}

template <>
FString ToDebugString(const FString& Value)
{
	return FString::Printf(TEXT("%s(\"%s\")"), GetTypeName<FString>(), *Value);
}

template <>
FString ToDebugString(const FCleverTapDate& Value)
{
	return FString::Printf(TEXT("date(y=%d, m=%d, d=%d)"), Value.Year, Value.Month, Value.Day);
}

static FString GetPropertyTypeName(FProperty* Property)
{
	check(Property);

	FString ExtendedTypeInfo;
	FString TypeInfo = Property->GetCPPType(&ExtendedTypeInfo, 0);

	if (ExtendedTypeInfo.IsEmpty())
	{
		return TypeInfo;
	}
	else
	{
		return FString::Printf(TEXT("%s %s"), *TypeInfo, *ExtendedTypeInfo);
	}
}

} // namespace internal

FString FCleverTapPropertyValue::ToString() const
{
	switch (Value.GetIndex())
	{
		case IndexOfType<int32>():
			return internal::ToString(Value.Get<int32>());

		case IndexOfType<int64>():
			return internal::ToString(Value.Get<int64>());

		case IndexOfType<float>():
			return internal::ToString(Value.Get<float>());

		case IndexOfType<double>():
			return internal::ToString(Value.Get<double>());

		case IndexOfType<bool>():
			return internal::ToString(Value.Get<bool>());

		case IndexOfType<FString>():
			return internal::ToString(Value.Get<FString>());

		case IndexOfType<FCleverTapDate>():
			return internal::ToString(Value.Get<FCleverTapDate>());

		case IndexOfType<TArray<int32>>():
			return internal::ToString(Value.Get<TArray<int32>>());

		case IndexOfType<TArray<int64>>():
			return internal::ToString(Value.Get<TArray<int64>>());

		case IndexOfType<TArray<float>>():
			return internal::ToString(Value.Get<TArray<float>>());

		case IndexOfType<TArray<double>>():
			return internal::ToString(Value.Get<TArray<double>>());

		case IndexOfType<TArray<bool>>():
			return internal::ToString(Value.Get<TArray<bool>>());

		case IndexOfType<TArray<FString>>():
			return internal::ToString(Value.Get<TArray<FString>>());

		default:
			return TEXT("");
	}
}

FString FCleverTapPropertyValue::ToDebugString() const
{
	switch (Value.GetIndex())
	{
		case IndexOfType<int32>():
			return internal::ToDebugString(Value.Get<int32>());

		case IndexOfType<int64>():
			return internal::ToDebugString(Value.Get<int64>());

		case IndexOfType<float>():
			return internal::ToDebugString(Value.Get<float>());

		case IndexOfType<double>():
			return internal::ToDebugString(Value.Get<double>());

		case IndexOfType<bool>():
			return internal::ToDebugString(Value.Get<bool>());

		case IndexOfType<FString>():
			return internal::ToDebugString(Value.Get<FString>());

		case IndexOfType<FCleverTapDate>():
			return internal::ToDebugString(Value.Get<FCleverTapDate>());

		case IndexOfType<TArray<int32>>():
			return internal::ToDebugString(Value.Get<TArray<int32>>());

		case IndexOfType<TArray<int64>>():
			return internal::ToDebugString(Value.Get<TArray<int64>>());

		case IndexOfType<TArray<float>>():
			return internal::ToDebugString(Value.Get<TArray<float>>());

		case IndexOfType<TArray<double>>():
			return internal::ToDebugString(Value.Get<TArray<double>>());

		case IndexOfType<TArray<bool>>():
			return internal::ToDebugString(Value.Get<TArray<bool>>());

		case IndexOfType<TArray<FString>>():
			return internal::ToDebugString(Value.Get<TArray<FString>>());

		default:
			return TEXT("invalid");
	}
}

FString FCleverTapPropertyValue::GetTypeName() const
{
	switch (Value.GetIndex())
	{
		case IndexOfType<int32>():
			return internal::GetTypeName<int32>();

		case IndexOfType<int64>():
			return internal::GetTypeName<int64>();

		case IndexOfType<float>():
			return internal::GetTypeName<float>();

		case IndexOfType<double>():
			return internal::GetTypeName<double>();

		case IndexOfType<bool>():
			return internal::GetTypeName<bool>();

		case IndexOfType<FString>():
			return internal::GetTypeName<FString>();

		case IndexOfType<FCleverTapDate>():
			return internal::GetTypeName<FCleverTapDate>();

		case IndexOfType<TArray<int32>>():
			return internal::GetTypeName<TArray<int32>>();

		case IndexOfType<TArray<int64>>():
			return internal::GetTypeName<TArray<int64>>();

		case IndexOfType<TArray<float>>():
			return internal::GetTypeName<TArray<float>>();

		case IndexOfType<TArray<double>>():
			return internal::GetTypeName<TArray<double>>();

		case IndexOfType<TArray<bool>>():
			return internal::GetTypeName<TArray<bool>>();

		case IndexOfType<TArray<FString>>():
			return internal::GetTypeName<TArray<FString>>();

		default:
			return TEXT("unknown");
	}
}

FString ToDebugString(const FCleverTapPropertyValue& Value)
{
	return Value.ToDebugString();
}

FString ToDebugString(const TOptional<FCleverTapPropertyValue>& OptionalValue)
{
	if (OptionalValue.IsSet())
	{
		return OptionalValue.GetValue().ToDebugString();
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
		Result += FString::Printf(TEXT("\"%s\"=%s"), *KVP.Key, *KVP.Value.ToDebugString());
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

		const FString& Name = Property->GetName();
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
				UE_LOG(LogCleverTap, Warning,
					TEXT(
						"FCleverTapProperties::LoadFromStruct: Source StructProperty \"%s\" has unsupported struct type %s"),
					*Name, *internal::GetPropertyTypeName(StructProp));
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
				UE_LOG(LogCleverTap, Warning,
					TEXT(
						"FCleverTapProperties::LoadFromStruct: Source ArrayProperty \"%s\" has unsupported element type %s"),
					*Name, *internal::GetPropertyTypeName(ArrayProp->Inner));
			}
		}
		else
		{
			UE_LOG(LogCleverTap, Warning,
				TEXT("FCleverTapProperties::LoadFromStruct: Source Property \"%s\" has unsupported type %s"), *Name,
				*internal::GetPropertyTypeName(Property));
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

	ApplyCleverTapPropertyValueToStruct(*SourceValue, StructDef, Property, TargetStructInstance);
}

void ApplyCleverTapPropertyValueToStruct(const FCleverTapPropertyValue& SourceValue, const UStruct* StructDef,
	FProperty* Property, void* TargetStructInstance)
{
	check(StructDef);
	check(Property);
	check(TargetStructInstance);

	if (FCleverTapProperties::ShouldSkipProperty(Property))
	{
		// this property has been excluded from load/apply
		return;
	}

	// The target Value
	void* TargetValuePtr = Property->ContainerPtrToValuePtr<void>(TargetStructInstance);

	// Each FProperty type needs custom handling
	if (auto* BoolProp = CastField<FBoolProperty>(Property))
	{
		if (SourceValue.IsType<bool>())
		{
			BoolProp->SetPropertyValue(TargetValuePtr, SourceValue.Get<bool>());
			return;
		}
	}
	else if (auto* StringProp = CastField<FStrProperty>(Property))
	{
		if (SourceValue.IsType<FString>())
		{
			StringProp->SetPropertyValue(TargetValuePtr, SourceValue.Get<FString>());
			return;
		}
	}
	else if (auto* IntProp = CastField<FIntProperty>(Property))
	{
		if (SourceValue.IsType<int32>())
		{
			IntProp->SetPropertyValue(TargetValuePtr, SourceValue.Get<int32>());
			return;
		}
	}
	else if (auto* Int64Prop = CastField<FInt64Property>(Property))
	{
		if (SourceValue.IsType<int64>())
		{
			Int64Prop->SetPropertyValue(TargetValuePtr, SourceValue.Get<int64>());
			return;
		}
	}
	else if (auto* FloatProp = CastField<FFloatProperty>(Property))
	{
		if (SourceValue.IsType<float>())
		{
			FloatProp->SetPropertyValue(TargetValuePtr, SourceValue.Get<float>());
			return;
		}
	}
	else if (auto* DoubleProp = CastField<FFloatProperty>(Property))
	{
		if (SourceValue.IsType<double>())
		{
			DoubleProp->SetPropertyValue(TargetValuePtr, SourceValue.Get<double>());
			return;
		}
	}
	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		if (StructProp->Struct == FCleverTapDate::StaticStruct())
		{
			FCleverTapDate* TargetDatePtr = static_cast<FCleverTapDate*>(TargetValuePtr);
			if (SourceValue.IsType<FCleverTapDate>())
			{
				*TargetDatePtr = SourceValue.Get<FCleverTapDate>();
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
			if (SourceValue.IsType<TArray<FString>>())
			{
				const TArray<FString>& Values = SourceValue.Get<TArray<FString>>();
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
