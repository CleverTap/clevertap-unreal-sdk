// Copyright CleverTap All Rights Reserved.
#include "CleverTapSampleBlueprintFunctionLibrary.h"

#include "Algo/AllOf.h"
#include "CleverTapSample.h"
#include "Math/NumericLimits.h"
#include "Misc/Optional.h"

namespace {
TOptional<int64> CleverTapProperty_TryParseInt64(FStringView Str)
{
	if (Str.IsEmpty())
	{
		return TOptional<int64>{};
	}

	const bool bIsExplicitlyNegative = Str.StartsWith(TCHAR{ '-' });
	if (bIsExplicitlyNegative || Str.StartsWith(TCHAR{ '+' }))
	{
		Str.RemovePrefix(1);
	}

	if (Str.IsEmpty())
	{
		return TOptional<int64>{};
	}

	if (!Algo::AllOf(Str, FChar::IsDigit))
	{
		return TOptional<int64>{};
	}

	int64 const ParsedValue = FCString::Atoi64(Str.GetData());
	int64 const AsInt64 = bIsExplicitlyNegative ? -ParsedValue : ParsedValue;
	return TOptional<int64>{ AsInt64 };
}

bool CleverTapProperty_IsInt32(FStringView Str)
{
	const TOptional<int64> MaybeValue = CleverTapProperty_TryParseInt64(Str);
	if (!MaybeValue)
	{
		return false;
	}

	const int64 AsInt64 = MaybeValue.GetValue();
	return AsInt64 >= TNumericLimits<int32>::Min() && AsInt64 <= TNumericLimits<int32>::Max();
}

bool CleverTapProperty_IsInt64(FStringView Str)
{
	const TOptional<int64> MaybeValue = CleverTapProperty_TryParseInt64(Str);
	if (!MaybeValue)
	{
		return false;
	}

	if (Str.StartsWith('+'))
	{
		Str.RemovePrefix(1);
	}

	const int64 AsInt64 = MaybeValue.GetValue();
	return LexToString(AsInt64) == Str;
}

bool CleverTapProperty_IsBool(FStringView Str)
{
	return Str.Equals(TEXT("true"), ESearchCase::IgnoreCase) || Str.Equals(TEXT("false"), ESearchCase::IgnoreCase);
}

bool CleverTapProperty_IsDate(FStringView Str)
{
	// Expect date formatted as a YYYY-MM-DD string
	int32 SepIdx{};
	if (!Str.FindChar(TCHAR{ '-' }, SepIdx))
	{
		return false;
	}
	const FStringView YearPart = Str.Left(SepIdx);
	Str.RemovePrefix(SepIdx + 1);

	if (!Str.FindChar(TCHAR{ '-' }, SepIdx))
	{
		return false;
	}
	const FStringView MonthPart = Str.Left(SepIdx);
	const FStringView DayPart = Str.RightChop(SepIdx + 1);

	// Validate year format of 4 digits
	if (YearPart.IsEmpty() || YearPart.Len() > 4 || !Algo::AllOf(YearPart, FChar::IsDigit))
	{
		return false;
	}

	// Validate month format of 2 digits
	if (MonthPart.IsEmpty() || MonthPart.Len() > 2 || !Algo::AllOf(MonthPart, FChar::IsDigit))
	{
		return false;
	}

	// Validate day format of 2 digits
	if (DayPart.IsEmpty() || DayPart.Len() > 2 || !Algo::AllOf(DayPart, FChar::IsDigit))
	{
		return false;
	}

	// Atoi is fine here as we know there's a '-' character between month and day bits
	const int32 MonthValue = FCString::Atoi(MonthPart.GetData());
	if (MonthValue <= 0 || MonthValue > 12)
	{
		return false;
	}

	// Atoi is fine here as we know there's a null terminator due to it being from a FString
	const int32 DayValue = FCString::Atoi(DayPart.GetData());
	constexpr int64 DAYS_PER_MONTH[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (DayValue <= 0 || DayValue > DAYS_PER_MONTH[MonthValue])
	{
		return false;
	}

	// Non-Leap years have 28 days, so check if we matched 29 for the month of Februrary 29 above
	if (MonthValue == 2 && DayValue == DAYS_PER_MONTH[2])
	{
		// Atoi is fine here as we know there's a '-' character between year and month bits
		const int32 YearValue = FCString::Atoi(YearPart.GetData());
		if (!FDateTime::IsLeapYear(YearValue))
		{
			return false;
		}
	}

	return true;
}

} // namespace

bool UCleverTapSampleBlueprintFunctionLibrary::IsEventPropertyValueValid(const FEventPropertyViewModel& Property)
{
	switch (Property.ValueType)
	{
		case EEventPropertyType::String:
		{
			return true;
		}

		case EEventPropertyType::Int32:
		{
			return CleverTapProperty_IsInt32(Property.ValueString);
		}

		case EEventPropertyType::Int64:
		{
			return CleverTapProperty_IsInt64(Property.ValueString);
		}

		case EEventPropertyType::Float:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EEventPropertyType::Double:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EEventPropertyType::Bool:
		{
			return CleverTapProperty_IsBool(Property.ValueString);
		}

		case EEventPropertyType::Date:
		{
			return CleverTapProperty_IsDate(Property.ValueString);
		}
	}

	UE_LOG(LogCleverTapSample, Error, TEXT("Unhandled EEventPropertyType: %d"), static_cast<uint8>(Property.ValueType));
	return false;
}

bool UCleverTapSampleBlueprintFunctionLibrary::IsProfilePropertyValueValid(const FProfilePropertyViewModel& Property)
{
	switch (Property.ValueType)
	{
		case EProfilePropertyType::String:
		{
			return true;
		}

		case EProfilePropertyType::Int32:
		{
			return CleverTapProperty_IsInt32(Property.ValueString);
		}

		case EProfilePropertyType::Int64:
		{
			return CleverTapProperty_IsInt64(Property.ValueString);
		}

		case EProfilePropertyType::Float:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EProfilePropertyType::Double:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EProfilePropertyType::Bool:
		{
			return CleverTapProperty_IsBool(Property.ValueString);
		}

		case EProfilePropertyType::Date:
		{
			return CleverTapProperty_IsDate(Property.ValueString);
		}

		case EProfilePropertyType::Array_Int32:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));
			return Algo::AllOf(ElementStrings,
				[](const FString& Str) { return CleverTapProperty_IsInt32(FStringView{ Str }.TrimStartAndEnd()); });
		}

		case EProfilePropertyType::Array_Int64:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));
			return Algo::AllOf(ElementStrings,
				[](const FString& Str) { return CleverTapProperty_IsInt64(FStringView{ Str }.TrimStartAndEnd()); });
		}

		case EProfilePropertyType::Array_Float:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));

			// Pre-trim since FStringView{} doesn't have IsNumeric
			for (auto& Str : ElementStrings)
			{
				Str.TrimStartAndEndInline();
			}

			return Algo::AllOf(ElementStrings, [](const FString& Str) { return Str.IsNumeric(); });
		}

		case EProfilePropertyType::Array_Double:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));

			// Pre-trim since FStringView{} doesn't have IsNumeric
			for (auto& Str : ElementStrings)
			{
				Str.TrimStartAndEndInline();
			}

			return Algo::AllOf(ElementStrings, [](const FString& Str) { return Str.IsNumeric(); });
		}

		case EProfilePropertyType::Array_Bool:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));
			return Algo::AllOf(ElementStrings,
				[](const FString& Str) { return CleverTapProperty_IsBool(FStringView{ Str }.TrimStartAndEnd()); });
		}

		case EProfilePropertyType::Array_String:
		{
			return true;
		}
	}

	UE_LOG(
		LogCleverTapSample, Error, TEXT("Unhandled EProfilePropertyType: %d"), static_cast<uint8>(Property.ValueType));
	return false;
}
