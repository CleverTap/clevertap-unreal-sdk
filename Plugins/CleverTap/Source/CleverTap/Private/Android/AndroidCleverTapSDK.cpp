// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"

#include "Android/AndroidCleverTapJNI.h"

#include "CleverTapPlatformSDK.h"
#include "CleverTapInstance.h"
#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android {

class FAndroidCleverTapInstance : public ICleverTapInstance
{
public:
	FAndroidCleverTapInstance()
	{
		CleverTapSDK::Android::JNI::InitCleverTap();

		CleverTapSDK::Android::FPlatformSDK::SetLogLevel(ECleverTapLogLevel::Off);
		CleverTapSDK::Android::FPlatformSDK::SetLogLevel(ECleverTapLogLevel::Info);
		CleverTapSDK::Android::FPlatformSDK::SetLogLevel(ECleverTapLogLevel::Debug);
		CleverTapSDK::Android::FPlatformSDK::SetLogLevel(ECleverTapLogLevel::Verbose);
		CleverTapSDK::Android::FPlatformSDK::SetLogLevel(ECleverTapLogLevel(123));
	}

	FString GetCleverTapId() const override { return FString{}; } // todo hmm
};

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	JNIEnv* env = FAndroidApplication::GetJavaEnv();
	if (env == nullptr)
	{
		UE_LOG(LogCleverTap, Error, TEXT("FAndroidApplication::GetJavaEnv() returned nullptr"));
		return;
	}

	JNI::SetLogLevel(env, JNI::CleverTapLogLevelName(Level));
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	CleverTapSDK::Ignore(Config);
	return MakeUnique<FAndroidCleverTapInstance>();
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	CleverTapSDK::Ignore(Config, CleverTapId);
	return MakeUnique<FAndroidCleverTapInstance>();
}

}} // namespace CleverTapSDK::Android
