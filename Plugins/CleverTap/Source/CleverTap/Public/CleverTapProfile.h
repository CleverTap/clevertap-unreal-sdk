// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"

inline FCleverTapProperties GetExampleCleverTapProfile() // todo move this
{
	FCleverTapProperties Profile;
	Profile.Add("Name", "Jack Montana");			 // String
	Profile.Add("Identity", 61026032);				 // String or number
	Profile.Add("Email", "jack@gmail.com");			 // email
	Profile.Add("Phone", "+14155551234");			 // Phone (with the country code, starting with +)
	Profile.Add("Gender", "M");						 // Can be either M or F
	Profile.Add("DOB", FCleverTapDate(1953, 3, 13)); // Date of Birth.

	// optional fields. controls whether the user will be sent email, push etc.
	Profile.Add("MSG-email", false);   // Disable email notifications
	Profile.Add("MSG-push", true);	   // Enable push notifications
	Profile.Add("MSG-sms", false);	   // Disable SMS notifications
	Profile.Add("MSG-whatsapp", true); // Enable WhatsApp notifications

	TArray<FString> stuff;
	stuff.Add("bag");
	stuff.Add("shoes");
	Profile.Add("MyStuff", stuff); // ArrayList of Strings

	TArray<FString> otherStuff = { "Jeans", "Perfume" };
	Profile["MyStuff"] = otherStuff;
	return Profile;
}