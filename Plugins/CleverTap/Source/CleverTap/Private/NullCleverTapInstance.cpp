// Copyright CleverTap All Rights Reserved.
#include "NullCleverTapInstance.h"

FString FNullCleverTapInstance::GetCleverTapId() const
{
	return FString{};
}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProfile& profile) const {}

void FNullCleverTapInstance::OnUserLogin(const FCleverTapProfile& profile, const FString& cleverTapId) const {}
