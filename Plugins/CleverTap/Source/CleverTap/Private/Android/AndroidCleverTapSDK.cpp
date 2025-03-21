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

		// event without properties
		PushEvent(TEXT("Event No Props"));

		// event with properties
		FCleverTapProperties Actions;
		Actions.Add("Product Name", "Casio Chronograph Watch");
		Actions.Add("Category", "Mens Accessories");
		Actions.Add("Price", 59.99);
		// Actions.put("Date", FCleverTapDate::Today() ); TODO
		PushEvent(TEXT("Product viewed"), Actions);
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
		// todo release JavaProfile here or use scoped container
	}

	void PushChargedEvent(
		const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) const override
	{
		// TODO
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
