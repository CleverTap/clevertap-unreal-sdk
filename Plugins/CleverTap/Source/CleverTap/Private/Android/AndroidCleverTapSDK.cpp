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
private:
	static TSet<FAndroidCleverTapInstance*> Instances;

public:
	static bool IsValid(const FAndroidCleverTapInstance* Instance) { return Instances.Contains(Instance); }

	jobject JavaCleverTapInstance;

	FAndroidCleverTapInstance(JNIEnv* Env, jobject JavaCleverTapInstanceIn)
	{
		Instances.Add(this);
		if (!Env || !JavaCleverTapInstanceIn)
		{
			JavaCleverTapInstance = nullptr;
			return;
		}

		JavaCleverTapInstance = Env->NewGlobalRef(JavaCleverTapInstanceIn);
		JNI::RegisterPushPermissionResponseListener(Env, JavaCleverTapInstance, this);
	}

	~FAndroidCleverTapInstance()
	{
		auto* Env = JNI::GetJNIEnv();
		if (Env && JavaCleverTapInstance)
		{
			Env->DeleteGlobalRef(JavaCleverTapInstance);
		}

		Instances.Remove(this);
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

	bool IsPushPermissionGranted() override
	{
		return JNI::IsPushPermissionGranted(JNI::GetJNIEnv(), JavaCleverTapInstance);
	}

	void PromptForPushPermission(bool bFallbackToSettings) override
	{
		JNI::PromptForPushPermission(JNI::GetJNIEnv(), JavaCleverTapInstance, bFallbackToSettings);
	}
	void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject PrimerConfig = JNI::CreatePushPrimerConfigJSON(Env, PushPrimerAlertConfig);
		if (PrimerConfig)
		{
			JNI::PromptPushPrimer(Env, JavaCleverTapInstance, PrimerConfig);
			Env->DeleteLocalRef(PrimerConfig);
		}
	}
	void PromptForPushPermission(
		const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig) override
	{
		auto* Env = JNI::GetJNIEnv();
		jobject PrimerConfig = JNI::CreatePushPrimerConfigJSON(Env, PushPrimerHalfInterstitialConfig);
		if (PrimerConfig)
		{
			JNI::PromptPushPrimer(Env, JavaCleverTapInstance, PrimerConfig);
			Env->DeleteLocalRef(PrimerConfig);
		}
	}
};

TSet<FAndroidCleverTapInstance*> FAndroidCleverTapInstance::Instances;

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	JNIEnv* Env = JNI::GetJNIEnv();
	JNI::SetDebugLevel(Env, Level);

	// todo there's a per-instance SetDebugLevel we should call
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

//
// JNI Callbacks
//
extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListeners_00024PushPermissionListener_nativeOnPushPermissionResponse__JZ(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jboolean bGranted)
{
	// the java callbacks can come from any thread; to keep things simple we queue all work to the main game thread
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, bGranted]() {
		UE_LOG(LogCleverTap, Log, TEXT("OnPushPermissionResponse(NativeInstance=%lx, bGranted=%s)"), NativeInstancePtr,
			bGranted ? TEXT("TRUE") : TEXT("FALSE"));
		using namespace CleverTapSDK::Android;
		auto* Instance = reinterpret_cast<FAndroidCleverTapInstance*>(NativeInstancePtr);
		if (FAndroidCleverTapInstance::IsValid(Instance))
		{
			Instance->OnPushPermissionResponse.Broadcast(bGranted);
		}
		else
		{
			UE_LOG(LogCleverTap, Error, TEXT("Invalid native instance!"));
		}
	});
}
