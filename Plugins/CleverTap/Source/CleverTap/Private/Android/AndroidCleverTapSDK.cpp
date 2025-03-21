// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"

#include "Android/AndroidCleverTapJNI.h"

#include "CleverTapInstance.h"
#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android {

class FAndroidCleverTapInstance : public ICleverTapInstance
{
public:
	FAndroidCleverTapInstance()
	{
		JNI::InitCleverTap();
		// todo store scoped intance here
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
		Env->DeleteLocalRef(JavaProfile);
	};

	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) const override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::OnUserLogin(Env, JNI::GetCleverTapInstance(Env), JavaProfile, CleverTapId);
		Env->DeleteLocalRef(JavaProfile);
	}

	void PushProfile(const FCleverTapProperties& Profile) const override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::PushProfile(Env, JNI::GetCleverTapInstance(Env), JavaProfile);
		Env->DeleteLocalRef(JavaProfile);
	}

	void PushEvent(const FString& EventName) const override
	{
		auto* Env = JNI::GetJNIEnv();
		JNI::PushEvent(Env, JNI::GetCleverTapInstance(Env), EventName);
	}

	void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) const override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaActions = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Actions);
		JNI::PushEvent(Env, JNI::GetCleverTapInstance(Env), EventName, JavaActions);
		Env->DeleteLocalRef(JavaActions);
	}

	void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) const override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaDetails = JNI::ConvertCleverTapPropertiesToJavaMap(Env, ChargeDetails);
		jobject JavaItems = JNI::ConvertArrayOfCleverTapPropertiesToJavaArrayOfMap(Env, Items);
		JNI::PushChargedEvent(Env, JNI::GetCleverTapInstance(Env), JavaDetails, JavaItems);
		Env->DeleteLocalRef(JavaDetails);
		Env->DeleteLocalRef(JavaItems);
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
