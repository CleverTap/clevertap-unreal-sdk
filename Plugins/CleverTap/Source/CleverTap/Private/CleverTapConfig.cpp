// Copyright CleverTap All Rights Reserved.
#include "CleverTapConfig.h"

#include "Runtime/Launch/Resources/Version.h"

ECleverTapLogLevel UCleverTapConfig::GetActiveLogLevel() const
{
#if UE_BUILD_SHIPPING
	return ShippingLogLevel;
#else
	return DevelopmentLogLevel;
#endif
}

FString UCleverTapConfig::DefaultAndroidUnrealSplashActivity()
{
#if ENGINE_MAJOR_VERSION < 4
	#error "unsupported engine version!"
#elif ENGINE_MAJOR_VERSION == 4
	return TEXT("com.epicgames.ue4.SplashActivity");
#else
	return TEXT("com.epicgames.unreal.SplashActivity");
#endif
}

FString UCleverTapConfig::DefaultAndroidUnrealSplashTheme()
{
#if ENGINE_MAJOR_VERSION < 4
	#error "unsupported engine version!"
#elif ENGINE_MAJOR_VERSION == 4
	return TEXT("@style/UE4SplashTheme");
#else
	return TEXT("@style/UnrealSplashTheme");
#endif
}
