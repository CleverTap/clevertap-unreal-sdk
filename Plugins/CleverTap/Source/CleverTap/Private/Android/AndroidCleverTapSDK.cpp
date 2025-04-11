// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"

#include "Android/AndroidCleverTapJNI.h"
#include "Android/AndroidJNIUtilities.h"

#include "CleverTapInstance.h"
#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android {

class FAndroidCleverTapInstance : public ICleverTapInstance
{
public:
	jobject JavaCleverTapInstance;

	FAndroidCleverTapInstance(JNIEnv* Env, jobject JavaCleverTapInstanceIn)
	{
		if (Env && JavaCleverTapInstanceIn)
		{
			JavaCleverTapInstance = Env->NewGlobalRef(JavaCleverTapInstanceIn);
		}
		else
		{
			JavaCleverTapInstance = nullptr;
		}
	}
	~FAndroidCleverTapInstance()
	{
		auto* Env = JNI::GetJNIEnv();
		if (Env && JavaCleverTapInstance)
		{
			Env->DeleteGlobalRef(JavaCleverTapInstance);
		}
	}

	FString GetCleverTapId() override
	{
		auto* Env = JNI::GetJNIEnv();
		return JNI::GetCleverTapID(Env, JavaCleverTapInstance);
	}

	void OnUserLogin(const FCleverTapProperties& Profile) override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::OnUserLogin(Env, JavaCleverTapInstance, JavaProfile);
		Env->DeleteLocalRef(JavaProfile);
	};

	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::OnUserLogin(Env, JavaCleverTapInstance, JavaProfile, CleverTapId);
		Env->DeleteLocalRef(JavaProfile);
	}

	void PushProfile(const FCleverTapProperties& Profile) override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::PushProfile(Env, JavaCleverTapInstance, JavaProfile);
		Env->DeleteLocalRef(JavaProfile);
	}

	void PushEvent(const FString& EventName) override
	{
		auto* Env = JNI::GetJNIEnv();
		JNI::PushEvent(Env, JavaCleverTapInstance, EventName);
	}

	void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaActions = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Actions);
		JNI::PushEvent(Env, JavaCleverTapInstance, EventName, JavaActions);
		Env->DeleteLocalRef(JavaActions);
	}

	void PushChargedEvent(const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject JavaDetails = JNI::ConvertCleverTapPropertiesToJavaMap(Env, ChargeDetails);
		jobject JavaItems = JNI::ConvertArrayOfCleverTapPropertiesToJavaArrayOfMap(Env, Items);
		JNI::PushChargedEvent(Env, JavaCleverTapInstance, JavaDetails, JavaItems);
		Env->DeleteLocalRef(JavaDetails);
		Env->DeleteLocalRef(JavaItems);
	}

	void DecrementValue(const FString& Key, int Amount) override
	{
		JNI::DecrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}

	void DecrementValue(const FString& Key, double Amount) override
	{
		JNI::DecrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}

	void IncrementValue(const FString& Key, int Amount) override
	{
		JNI::IncrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}

	void IncrementValue(const FString& Key, double Amount) override
	{
		JNI::IncrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}
};

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	JNIEnv* Env = JNI::GetJNIEnv();
	JNI::SetDebugLevel(Env, Level);
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	JNIEnv* Env = JNI::GetJNIEnv();
	JNI::SetDefaultConfig(Env, Config);
	jobject Instance = JNI::GetDefaultInstance(Env);
	if (!Env || !Instance)
	{
		return nullptr;
	}
	return MakeUnique<FAndroidCleverTapInstance>(Env, Instance);
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	JNIEnv* Env = JNI::GetJNIEnv();
	JNI::SetDefaultConfig(Env, Config);
	jobject Instance = JNI::GetDefaultInstance(Env, CleverTapId);
	if (!Env || !Instance)
	{
		return nullptr;
	}
	return MakeUnique<FAndroidCleverTapInstance>(Env, Instance);
}

}} // namespace CleverTapSDK::Android
