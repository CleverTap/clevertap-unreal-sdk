// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

struct FCleverTapDate
{
	int32 Year;
	int32 Month;
	int32 Day;

	FCleverTapDate() : Year(0), Month(0), Day(0) {}
	FCleverTapDate(int32 InYear, int32 InMonth, int32 InDay) : Year(InYear), Month(InMonth), Day(InDay) {}
	FCleverTapDate(const FCleverTapDate& Other) = default;

	FString ToString() const { return FString::Printf(TEXT("%04d-%02d-%02d"), Year, Month, Day); }
};

class FCleverTapProfileValue
{
public:
	using VariantType = TVariant<int64, float, bool, FString, FCleverTapDate, TArray<FString>>;

	// Default constructors
	FCleverTapProfileValue() = default;

	// Type-specific constructors
	FCleverTapProfileValue(int Value) : _Value(TInPlaceType<int64>(), int64(Value)) {}
	FCleverTapProfileValue(int64 Value) : _Value(TInPlaceType<int64>(), Value) {}
	FCleverTapProfileValue(float Value) : _Value(TInPlaceType<float>(), Value) {}
	FCleverTapProfileValue(bool Value) : _Value(TInPlaceType<bool>(), Value) {}
	FCleverTapProfileValue(const char* Value) : _Value(TInPlaceType<FString>(), FString(Value)) {}
	FCleverTapProfileValue(const FString& Value) : _Value(TInPlaceType<FString>(), Value) {}
	FCleverTapProfileValue(FString&& Value) : _Value(TInPlaceType<FString>(), MoveTemp(Value)) {}
	FCleverTapProfileValue(const FCleverTapDate& Value) : _Value(TInPlaceType<FCleverTapDate>(), Value) {}
	FCleverTapProfileValue(const TArray<FString>& Value) : _Value(TInPlaceType<TArray<FString>>(), Value) {}
	FCleverTapProfileValue(TArray<FString>&& Value) : _Value(TInPlaceType<TArray<FString>>(), MoveTemp(Value)) {}

	// Copy constructor
	FCleverTapProfileValue(const FCleverTapProfileValue& Other) = default;

	// Move constructor
	FCleverTapProfileValue(FCleverTapProfileValue&& Other) noexcept = default;

	// Copy assignment operator
	FCleverTapProfileValue& operator=(const FCleverTapProfileValue& Other) = default;

	// Move assignment operator
	FCleverTapProfileValue& operator=(FCleverTapProfileValue&& Other) noexcept = default;

	// Destructor
	~FCleverTapProfileValue() = default;

	// Variant Methods
	template <typename U> bool IsType() const { return _Value.IsType<U>(); }
	template <typename U> U& Get() { return _Value.Get<U>(); }
	template <typename U> const U& Get() const { return _Value.Get<U>(); }
	template <typename U> U* TryGet() { return _Value.TryGet<U>(); }
	template <typename U> const U* TryGet() const { return _Value.TryGet<U>(); }
	template <typename U> void Set(typename TIdentity<U>::Type&& Value) { _Value.Set<U>(MoveTemp(Value)); }
	template <typename U> void Set(const typename TIdentity<U>::Type& Value) { _Value.Set<U>(Value); }
	template <typename U, typename... TArgs> void Emplace(TArgs&&... Args)
	{
		_Value.Emplace<U>(Forward<TArgs>(Args)...);
	}
	template <typename U> static constexpr SIZE_T IndexOfType() { return VariantType::IndexOfType<U>(); }
	SIZE_T GetIndex() const { return _Value.GetIndex(); }

private:
	VariantType _Value;
};

using FCleverTapProfile = TMap<FString, FCleverTapProfileValue>;

inline FCleverTapProfile GetExampleCleverTapProfile() // todo move this
{
	FCleverTapProfile profile;
	profile.Add("Name", "Jack Montana");			 // String
	profile.Add("Identity", 61026032);				 // String or number
	profile.Add("Email", "jack@gmail.com");			 // email
	profile.Add("Phone", "+14155551234");			 // Phone (with the country code, starting with +)
	profile.Add("Gender", "M");						 // Can be either M or F
	profile.Add("DOB", FCleverTapDate(1953, 3, 13)); // Date of Birth.

	// optional fields. controls whether the user will be sent email, push etc.
	profile.Add("MSG-email", false);   // Disable email notifications
	profile.Add("MSG-push", true);	   // Enable push notifications
	profile.Add("MSG-sms", false);	   // Disable SMS notifications
	profile.Add("MSG-whatsapp", true); // Enable WhatsApp notifications

	TArray<FString> stuff;
	stuff.Add("bag");
	stuff.Add("shoes");
	profile.Add("MyStuff", stuff); // ArrayList of Strings

	TArray<FString> otherStuff = { "Jeans", "Perfume" };
	profile["MyStuff"] = otherStuff;
	return profile;
}