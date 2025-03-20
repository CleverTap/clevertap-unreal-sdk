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
		FPlatformSDK::SetLogLevel(ECleverTapLogLevel::Verbose);

		JNI::InitCleverTap();
		// todo store scoped intance here

		FCleverTapProfile profile;
		OnUserLogin(profile);
	}

	FString GetCleverTapId() const override
	{
		auto* env = JNI::GetJNIEnv();
		return JNI::GetCleverTapID(env, JNI::GetCleverTapInstance(env));
	}

	void OnUserLogin(const FCleverTapProfile& profile) const override
	{
		auto* env = JNI::GetJNIEnv();
		JNI::OnUserLogin(env, JNI::GetCleverTapInstance(env), JNI::ConvertProfileToJavaMap(env, profile));
	};

	void OnUserLogin(const FCleverTapProfile& profile, const FString& cleverTapId) const override
	{
		auto* env = JNI::GetJNIEnv();
		JNI::OnUserLogin(env, JNI::GetCleverTapInstance(env), JNI::ConvertProfileToJavaMap(env, profile), cleverTapId);
	}
};

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	JNIEnv* env = FAndroidApplication::GetJavaEnv();
	if (env == nullptr)
	{
		UE_LOG(LogCleverTap, Error, TEXT("FAndroidApplication::GetJavaEnv() returned nullptr"));
		return;
	}

	JNI::SetDebugLevel(env, JNI::CleverTapLogLevelName(Level));
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
