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
	bool bEnableOnPushNotificationClicked = false;
	TOptional<FCleverTapProperties> BufferedPushNotificationPayload;

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
		JNI::RegisterPushNotificationClickedListener(Env, JavaCleverTapInstance, this);
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

	void ReceivePushNotificationClicked(JNIEnv* Env, jobject JavaNotificationPayload)
	{
		UE_LOG(LogCleverTap, Log, TEXT("ReceivePushNotificationClicked(NotificationPayload=%s)"),
			*JNI::JavaObjectToString(Env, JavaNotificationPayload));

		FCleverTapProperties NotificationPayload =
			JNI::ConvertJavaMapToCleverTapProperties(Env, JavaNotificationPayload);
		if (bEnableOnPushNotificationClicked == false)
		{
			UE_LOG(LogCleverTap, Log,
				TEXT("-- EnableOnPushNotificationClicked() has not yet been called, buffering notification"));
			BufferedPushNotificationPayload = NotificationPayload;
		}
		else
		{
			OnPushNotificationClicked.Broadcast(NotificationPayload);
		}
	}

	void EnableOnPushNotificationClicked() override
	{
		if (bEnableOnPushNotificationClicked)
		{
			// already on, nothing to do
			return;
		}
		bEnableOnPushNotificationClicked = true;
		if (BufferedPushNotificationPayload.IsSet())
		{
			UE_LOG(LogCleverTap, Log, TEXT("EnableOnPushNotificationClicked() - broadcasting buffered notification"));
			OnPushNotificationClicked.Broadcast(BufferedPushNotificationPayload.GetValue());
			BufferedPushNotificationPayload.Reset();
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

/** Converts a raw NativeInstancePtr to either a valid FAndroidCleverTapInstance* or a nullptr.
 */
static CleverTapSDK::Android::FAndroidCleverTapInstance* CheckedInstancePtr(jlong NativeInstancePtr)
{
	using namespace CleverTapSDK::Android;
	auto* Instance = reinterpret_cast<FAndroidCleverTapInstance*>(NativeInstancePtr);
	return FAndroidCleverTapInstance::IsValid(Instance) ? Instance : nullptr;
}

//
// JNI Callbacks
//
// the java callbacks can come from any thread; to keep things simple we queue all work to the main game thread

extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListeners_00024PushPermissionListener_nativeOnPushPermissionResponse__JZ(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jboolean bGranted)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNI Error: Env is null in nativeOnPushPermissionResponse callback!"));
		return;
	}
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, bGranted]() {
		UE_LOG(LogCleverTap, Log, TEXT("OnPushPermissionResponse(NativeInstance=%lx, bGranted=%s)"), NativeInstancePtr,
			bGranted ? TEXT("TRUE") : TEXT("FALSE"));
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance)
		{
			Instance->OnPushPermissionResponse.Broadcast(bGranted);
		}
		else
		{
			UE_LOG(LogCleverTap, Error, TEXT("OnPushPermissionResponse received for invalid native instance!"));
		}
	});
}

extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListeners_00024PushNotificationListener_nativeOnNotificationClicked__JLjava_lang_Object_2(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject NotificationPayload)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNI Error: Env is null in nativeOnNotificationClicked callback!"));
		return;
	}
	jobject NotificationPayloadRef = NotificationPayload ? Env->NewGlobalRef(NotificationPayload) : nullptr;
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, NotificationPayloadRef]() {
		JNIEnv* GameThreadEnv = CleverTapSDK::Android::JNI::GetJNIEnv();
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance)
		{
			Instance->ReceivePushNotificationClicked(GameThreadEnv, NotificationPayloadRef);
		}
		else
		{
			UE_LOG(LogCleverTap, Warning, TEXT("OnPushNotificationClicked received for invalid native instance!"));
		}
		if (NotificationPayloadRef)
		{
			GameThreadEnv->DeleteGlobalRef(NotificationPayloadRef);
		}
	});
}
