// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

using FCleverTapProfileValue = TVariant<int64, float, bool, FString, FDateTime, TArray<FString>>;
using FCleverTapProfile = TMap<FString, FCleverTapProfileValue>;

inline FCleverTapProfile GetExampleCleverTapProfile() // todo move this
{
	FCleverTapProfile profile;
	profile.Add("Name", FCleverTapProfileValue(TInPlaceType<FString>(), "Jack Montana"));	 // String
	profile.Add("Identity", FCleverTapProfileValue(TInPlaceType<int64>(), 61026032));		 // String or number
	profile.Add("Email", FCleverTapProfileValue(TInPlaceType<FString>(), "jack@gmail.com")); // email
	profile.Add("Phone",
		FCleverTapProfileValue(
			TInPlaceType<FString>(), "+14155551234")); // Phone (with the country code, starting with +)
	profile.Add("Gender", FCleverTapProfileValue(TInPlaceType<FString>(), "M")); // Can be either M or F
	profile.Add("DOB",
		FCleverTapProfileValue(TInPlaceType<FDateTime>(),
			FDateTime())); // Date of Birth. Set the Date object to the appropriate value first

	// optional fields. controls whether the user will be sent email, push etc.
	profile.Add("MSG-email", FCleverTapProfileValue(TInPlaceType<bool>(), false));	 // Disable email notifications
	profile.Add("MSG-push", FCleverTapProfileValue(TInPlaceType<bool>(), true));	 // Enable push notifications
	profile.Add("MSG-sms", FCleverTapProfileValue(TInPlaceType<bool>(), false));	 // Disable SMS notifications
	profile.Add("MSG-whatsapp", FCleverTapProfileValue(TInPlaceType<bool>(), true)); // Enable WhatsApp notifications

	TArray<FString> stuff;
	stuff.Add("bag");
	stuff.Add("shoes");
	profile.Add("MyStuff", FCleverTapProfileValue(TInPlaceType<TArray<FString>>(), stuff)); // ArrayList of Strings

	TArray<FString> otherStuff = { "Jeans", "Perfume" };
	profile["MyStuff"] = FCleverTapProfileValue(TInPlaceType<TArray<FString>>(), otherStuff);
	return profile;
}