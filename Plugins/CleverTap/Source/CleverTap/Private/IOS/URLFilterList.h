// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

class UCleverTapConfig;

namespace CleverTapSDK { namespace IOS {

class FURLFilterList
{
public:
	explicit FURLFilterList(const UCleverTapConfig& Config);

	bool IsFilteredURL(const FString& URL) const;

private:
	struct FRequiredParts
	{
		FString Scheme;
		FString HostName;
		FString PathPrefix;
	};
	TArray<FRequiredParts> Requirements;
};

}} // namespace CleverTapSDK::IOS
