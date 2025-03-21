// Copyright CleverTap All Rights Reserved.
#include "NullCleverTapInstance.h"

FString FNullCleverTapInstance::GetCleverTapId() const
{
	return FString{};
}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile) const {}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) const {}

void FNullCleverTapInstance::PushProfile(const FCleverTapProperties& Profile) const {}
