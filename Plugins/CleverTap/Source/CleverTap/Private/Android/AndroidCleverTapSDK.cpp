// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"

#include "Android/AndroidCleverTapJNI.h"

#include "CleverTapInstance.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapPlatformSDK.h"
#include "CleverTapProfile.h"
#include "CleverTapProperties.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android {

class FAndroidCleverTapInstance : public ICleverTapInstance
{
public:
	FAndroidCleverTapInstance()
	{
		FPlatformSDK::SetLogLevel(ECleverTapLogLevel::Verbose); // todo delete this

		JNI::InitCleverTap();
		// todo store scoped intance here

		// todo move/delete this, it's only testing code:
		FCleverTapProperties Profile = GetExampleCleverTapProfile();
		OnUserLogin(Profile);
		PushProfile(Profile);
	}

	FString GetCleverTapId() const override
	{
		auto* Env = JNI::GetJNIEnv();
		return JNI::GetCleverTapID(Env, JNI::GetCleverTapInstance(Env));
	}

	void OnUserLogin(const FCleverTapProperties& Profile) const override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::OnUserLogin(Env, JNI::GetCleverTapInstance(Env), JavaProfile);
		// todo release JavaProfile here or use scoped container
	};

	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) const override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::OnUserLogin(Env, JNI::GetCleverTapInstance(Env), JavaProfile, CleverTapId);
		// todo release JavaProfile here or use scoped container
	}

	void PushProfile(const FCleverTapProperties& Profile) const override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::PushProfile(Env, JNI::GetCleverTapInstance(Env), JavaProfile);
		// todo release JavaProfile here or use scoped container
	}
};

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	JNIEnv* Env = JNI::GetJNIEnv();
	JNI::SetDebugLevel(Env, Level);
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
