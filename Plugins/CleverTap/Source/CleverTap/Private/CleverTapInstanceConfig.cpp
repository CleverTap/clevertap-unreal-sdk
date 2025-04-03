// Copyright CleverTap All Rights Reserved.
#include "CleverTapInstanceConfig.h"

#include "CleverTapConfig.h"

FCleverTapInstanceConfig FCleverTapInstanceConfig::FromCleverTapConfig(const UCleverTapConfig* Config)
{
	if (Config == nullptr)
	{
		Config = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
	}
	check(Config != nullptr);

	FCleverTapInstanceConfig InstanceConfig{};
	InstanceConfig.ProjectId = Config->ProjectId;
	InstanceConfig.ProjectToken = Config->ProjectToken;
	InstanceConfig.RegionCode = Config->RegionCode;
	InstanceConfig.IdentityKeys = Config->IdentityKeys;
	InstanceConfig.LogLevel = Config->GetActiveLogLevel();
	return InstanceConfig;
}

TArray<FString> FCleverTapInstanceConfig::GetIdentityKeys() const
{
	TArray<FString> Keys;
	IdentityKeys.ParseIntoArray(Keys, TEXT(","), true);
	return Keys;
}
