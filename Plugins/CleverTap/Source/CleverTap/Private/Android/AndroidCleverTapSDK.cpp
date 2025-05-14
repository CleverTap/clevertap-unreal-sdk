// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"

#include "Android/AndroidCleverTapJNI.h"
#include "Android/AndroidCleverTapPropertiesJNI.h"
#include "Android/AndroidJNIUtilities.h"

#include "CleverTapInstance.h"
#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android {

static inline const TCHAR* DEBUG_TEXT(bool b)
{
	return b ? TEXT("TRUE") : TEXT("FALSE");
}

class FAndroidCleverTapInstance : public ICleverTapInstance
{
private:
	// Most execution happens on the main game thread, but JNI callbacks can come from arbitrary threads.
	// This critical section is used to co-ordinate access to the set of instances and their InAppNotificationFilters.
	static FCriticalSection CriticalSection;
	static TSet<FAndroidCleverTapInstance*> Instances;

	// per-instance data
	jobject JavaCleverTapInstance;
	bool bEnableOnPushNotificationClicked = false;
	TOptional<FCleverTapProperties> BufferedPushNotificationPayload;
	TUniqueFunction<bool(const FCleverTapProperties&)> InAppNotificationFilter = [](const FCleverTapProperties&) {
		return true;
	};

public:
	static bool IsValid(const FAndroidCleverTapInstance* Instance) { return Instances.Contains(Instance); }

	/** Converts a raw NativeInstancePtr to either a valid FAndroidCleverTapInstance* or a nullptr.
	 */
	static FAndroidCleverTapInstance* CheckedInstancePtr(jlong NativeInstancePtr)
	{
		auto* Instance = reinterpret_cast<FAndroidCleverTapInstance*>(NativeInstancePtr);
		if (FAndroidCleverTapInstance::IsValid(Instance))
		{
			return Instance;
		}
		else
		{
			UE_LOG(LogCleverTap, Warning, TEXT("CheckedInstancePtr: Invalid or expired NativeInstancePtr!"));
			return nullptr;
		}
	}

	FAndroidCleverTapInstance(JNIEnv* Env, jobject JavaCleverTapInstanceIn)
	{
		check(IsInGameThread());
		FScopeLock Lock(&CriticalSection);
		Instances.Add(this);
		if (!Env || !JavaCleverTapInstanceIn)
		{
			JavaCleverTapInstance = nullptr;
			return;
		}

		JavaCleverTapInstance = Env->NewGlobalRef(JavaCleverTapInstanceIn);
		JNI::SuspendInAppNotifications(Env, JavaCleverTapInstance);

		jobject ListenerInstance = JNI::CreateUECleverTapListener(Env, JavaCleverTapInstance, this);
		JNI::RegisterPushPermissionResponseListener(Env, JavaCleverTapInstance, ListenerInstance);
		JNI::SetPushNotificationClickedListener(Env, JavaCleverTapInstance, ListenerInstance);
		JNI::SetInAppNotificationListener(Env, JavaCleverTapInstance, ListenerInstance);
		JNI::SetInAppNotificationButtonListener(Env, JavaCleverTapInstance, ListenerInstance);
	}

	~FAndroidCleverTapInstance()
	{
		check(IsInGameThread());
		FScopeLock Lock(&CriticalSection);
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
		UE_LOG(LogCleverTap, Log, TEXT("OnUserLogin(Profile=%s)"), *ToDebugString(Profile));
		auto* Env = JNI::GetJNIEnv();
		if (Env)
		{
			jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
			JNI::OnUserLogin(Env, JavaCleverTapInstance, JavaProfile);
			Env->DeleteLocalRef(JavaProfile);
		}
	};

	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) override
	{
		UE_LOG(
			LogCleverTap, Log, TEXT("OnUserLogin(Profile=%s, CleverTapId=%s)"), *ToDebugString(Profile), *CleverTapId);
		auto* Env = JNI::GetJNIEnv();
		if (Env)
		{
			jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
			JNI::OnUserLogin(Env, JavaCleverTapInstance, JavaProfile, CleverTapId);
			Env->DeleteLocalRef(JavaProfile);
		}
	}

	void PushProfile(const FCleverTapProperties& Profile) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("PushProfile(Profile=%s)"), *ToDebugString(Profile));
		auto* Env = JNI::GetJNIEnv();
		if (Env)
		{
			jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
			JNI::PushProfile(Env, JavaCleverTapInstance, JavaProfile);
			Env->DeleteLocalRef(JavaProfile);
		}
	}

	void PushEvent(const FString& EventName) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("PushEvent(EventName=%s)"), *EventName);
		auto* Env = JNI::GetJNIEnv();
		JNI::PushEvent(Env, JavaCleverTapInstance, EventName);
	}

	void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("PushEvent(EventName=%s, Actions=%s)"), *EventName, *ToDebugString(Actions));
		auto* Env = JNI::GetJNIEnv();
		jobject JavaActions = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Actions);
		JNI::PushEvent(Env, JavaCleverTapInstance, EventName, JavaActions);
		Env->DeleteLocalRef(JavaActions);
	}

	void PushChargedEvent(const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("PushChargedEvent(ChargeDetails=%s, Items=%s)"), *ToDebugString(ChargeDetails),
			*ToDebugString(Items));
		auto* Env = JNI::GetJNIEnv();
		jobject JavaDetails = JNI::ConvertCleverTapPropertiesToJavaMap(Env, ChargeDetails);
		jobject JavaItems = JNI::ConvertArrayOfCleverTapPropertiesToJavaArrayOfMap(Env, Items);
		JNI::PushChargedEvent(Env, JavaCleverTapInstance, JavaDetails, JavaItems);
		Env->DeleteLocalRef(JavaDetails);
		Env->DeleteLocalRef(JavaItems);
	}

	TOptional<FCleverTapPropertyValue> GetProperty(const FString& Key) override
	{
		TOptional<FCleverTapPropertyValue> MaybeValue;
		auto* Env = JNI::GetJNIEnv();
		jobject JavaValue = JNI::GetProperty(Env, JavaCleverTapInstance, Key);
		if (JavaValue)
		{
			MaybeValue = JNI::ConvertJavaObjectToCleverTapPropertyValue(Env, JavaValue);
			Env->DeleteLocalRef(JavaValue);
		}
		UE_LOG(LogCleverTap, Log, TEXT("GetProperty(%s)=%s"), *Key, *ToDebugString(MaybeValue));
		return MaybeValue;
	}

	void DecrementValue(const FString& Key, int Amount) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("DecrementValue(%s, %d)"), *Key, Amount);
		JNI::DecrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}

	void DecrementValue(const FString& Key, double Amount) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("DecrementValue(%s, %f)"), *Key, Amount);
		JNI::DecrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}

	void IncrementValue(const FString& Key, int Amount) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("IncrementValue(%s, %d)"), *Key, Amount);
		JNI::IncrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}

	void IncrementValue(const FString& Key, double Amount) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("IncrementValue(%s, %f)"), *Key, Amount);
		JNI::IncrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
	}

	void AddMultiValueForKey(const FString& Key, const FString& Value) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("AddMultiValueForKey(%s, %s)"), *Key, *Value);
		JNI::AddMultiValueForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Value);
	}

	void AddMultiValuesForKey(const FString& Key, const TArray<FString>& Values) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("AddMultiValuesForKey(%s, %s)"), *Key,
			*ToDebugString(FCleverTapPropertyValue(Values)));
		JNI::AddMultiValuesForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Values);
	}

	void RemoveMultiValueForKey(const FString& Key, const FString& Value) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("RemoveMultiValueForKey(%s, %s)"), *Key, *Value);
		JNI::RemoveMultiValueForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Value);
	}

	void RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("RemoveMultiValuesForKey(%s, %s)"), *Key,
			*ToDebugString(FCleverTapPropertyValue(Values)));
		JNI::RemoveMultiValuesForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Values);
	}

	void RemoveValueForKey(const FString& Key) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("RemoveValueForKey(%s)"), *Key);
		JNI::RemoveValueForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key);
	}

	void SetMultiValuesForKey(const FString& Key, const TArray<FString> Values) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("SetMultiValuesForKey(%s, %s)"), *Key,
			*ToDebugString(FCleverTapPropertyValue(Values)));
		JNI::SetMultiValuesForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Values);
	}

	bool LocalizeAndroidNotificationChannel(
		const FString& ChannelId, const FText& ChannelName, const FText& ChannelDescription) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("LocalizeAndroidNotificationChannel(%s)"), *ChannelId);
		bool Success = JNI::LocalizeNotificationChannel(JNI::GetJNIEnv(), ChannelId, ChannelName, ChannelDescription);
		if (!Success)
		{
			UE_LOG(LogCleverTap, Error, TEXT("LocalizeAndroidNotificationChannel(%s) failed. Unknown ID?"), *ChannelId);
		}
		return Success;
	}

	bool LocalizeAndroidNotificationChannelGroup(const FString& GroupId, const FText& GroupName) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("LocalizeAndroidNotificationChannelGroup(%s)"), *GroupId);
		bool Success = JNI::LocalizeNotificationChannelGroup(JNI::GetJNIEnv(), GroupId, GroupName);
		if (!Success)
		{
			UE_LOG(
				LogCleverTap, Error, TEXT("LocalizeAndroidNotificationChannelGroup(%s) failed. Unknown ID?"), *GroupId);
		}
		return Success;
	}

	ECleverTapPushPermissionStatus GetPushPermissionStatus() override
	{
		if (JNI::IsPushPermissionGranted(JNI::GetJNIEnv(), JavaCleverTapInstance))
		{
			return ECleverTapPushPermissionStatus::Granted;
		}
		return ECleverTapPushPermissionStatus::NotGranted;
	}

	void PromptForPushPermission(bool bFallbackToSettings) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("PromptForPushPermission(bFallbackToSettings=%s)"),
			DEBUG_TEXT(bFallbackToSettings));
		JNI::PromptForPushPermission(JNI::GetJNIEnv(), JavaCleverTapInstance, bFallbackToSettings);
	}

	void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("PromptForPushPermission(AlertConfig)"));
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
		UE_LOG(LogCleverTap, Log, TEXT("PromptForPushPermission(HalfInterstitialConfig)"));
		auto* Env = JNI::GetJNIEnv();
		jobject PrimerConfig = JNI::CreatePushPrimerConfigJSON(Env, PushPrimerHalfInterstitialConfig);
		if (PrimerConfig)
		{
			JNI::PromptPushPrimer(Env, JavaCleverTapInstance, PrimerConfig);
			Env->DeleteLocalRef(PrimerConfig);
		}
	}

	static void BroadcastOnPushPermissionResponse(jlong NativeInstancePtr, bool bGranted)
	{
		UE_LOG(LogCleverTap, Log, TEXT("OnPushPermissionResponse(bGranted=%s)"), DEBUG_TEXT(bGranted));
		check(IsInGameThread());
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance)
		{
			Instance->OnPushPermissionResponse.Broadcast(bGranted);
		}
	}

	void BroadcastOnPushNotificationClicked(const FCleverTapProperties& NotificationProperties)
	{
		UE_LOG(LogCleverTap, Log, TEXT("OnPushNotificationClicked(%s)"), *ToDebugString(NotificationProperties));
		if (bEnableOnPushNotificationClicked == false)
		{
			UE_LOG(LogCleverTap, Log,
				TEXT("-- EnableOnPushNotificationClicked() has not yet been called, buffering notification"));
			BufferedPushNotificationPayload = NotificationProperties;
		}
		else
		{
			OnPushNotificationClicked.Broadcast(NotificationProperties);
		}
	}
	static void BroadcastOnPushNotificationClicked(
		jlong NativeInstancePtr, const FCleverTapProperties& NotificationProperties)
	{
		check(IsInGameThread());
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance)
		{
			Instance->BroadcastOnPushNotificationClicked(NotificationProperties);
		}
	}

	void EnableOnPushNotificationClicked() override
	{
		UE_LOG(LogCleverTap, Log, TEXT("EnableOnPushNotificationClicked()"));
		check(IsInGameThread());
		if (bEnableOnPushNotificationClicked)
		{
			// already on, nothing to do
			return;
		}
		bEnableOnPushNotificationClicked = true;
		if (BufferedPushNotificationPayload.IsSet())
		{
			UE_LOG(LogCleverTap, Log, TEXT("EnableOnPushNotificationClicked() - broadcasting buffered notification"));
			BroadcastOnPushNotificationClicked(BufferedPushNotificationPayload.GetValue());
			BufferedPushNotificationPayload.Reset();
		}
	}

	void EnableOnOpenUrl()
	{
		UE_LOG(LogCleverTap, Log, TEXT("EnableOnOpenUrl()"));
		JNI::EnableIntentNotifications(JNI::GetJNIEnv());
	}

	void RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("RegisterCleverTapUrlHandler()"));
		CleverTapSDK::Ignore(UrlHandler);
	}

	void RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("RegisterInAppNotificationFilter()"));
		check(IsInGameThread());
		FScopeLock Lock(&CriticalSection);
		InAppNotificationFilter = MoveTemp(Filter);
	}

	static bool BeforeShowInAppNotification(jlong NativeInstancePtr, const FCleverTapProperties& Extras)
	{
		UE_LOG(LogCleverTap, Log, TEXT("BeforeShowInAppNotification(%s)"), *ToDebugString(Extras));

		// this function can be called from any thread at any time, so
		// we lock the critical section to ensure no instances are created or destroyed,
		// nor filters changed, while we are trying to do this
		FScopeLock Lock(&CriticalSection);
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance == nullptr)
		{
			return true;
		}
		else
		{
			return Instance->InAppNotificationFilter(Extras);
		}
	}

	static void BroadcastOnInAppNotificationShown(jlong NativeInstancePtr, const FCleverTapProperties& Payload)
	{
		UE_LOG(LogCleverTap, Log, TEXT("OnInAppNotificationShown(%s)"), *ToDebugString(Payload));
		check(IsInGameThread());
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance)
		{
			Instance->OnInAppNotificationShown.Broadcast(Payload);
		}
	}

	static void BroadcastOnInAppNotificationDismissed(
		jlong NativeInstancePtr, const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras)
	{
		UE_LOG(LogCleverTap, Log, TEXT("OnInAppNotificationDismissed(Extras=%s, ActionExtras=%s)"),
			*ToDebugString(Extras), *ToDebugString(ActionExtras));
		check(IsInGameThread());
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance)
		{
			Instance->OnInAppNotificationDismissed.Broadcast(Extras, ActionExtras);
		}
	}

	static void BroadcastOnInAppNotificationButtonClicked(
		jlong NativeInstancePtr, const FCleverTapProperties& ButtonProperties)
	{
		UE_LOG(LogCleverTap, Log, TEXT("OnInAppNotificationButtonClicked(ButtonProperties=%s)"),
			*ToDebugString(ButtonProperties));
		check(IsInGameThread());
		auto* Instance = CheckedInstancePtr(NativeInstancePtr);
		if (Instance)
		{
			Instance->OnInAppNotificationButtonClicked.Broadcast(ButtonProperties);
		}
	}

	void DiscardInAppNotifications()
	{
		UE_LOG(LogCleverTap, Log, TEXT("DiscardInAppNotifications()"));
		JNI::DiscardInAppNotifications(JNI::GetJNIEnv(), JavaCleverTapInstance);
	}

	void ResumeInAppNotifications()
	{
		UE_LOG(LogCleverTap, Log, TEXT("ResumeInAppNotifications()"));
		JNI::ResumeInAppNotifications(JNI::GetJNIEnv(), JavaCleverTapInstance);
	}

	void SuspendInAppNotifications()
	{
		UE_LOG(LogCleverTap, Log, TEXT("SuspendInAppNotifications()"));
		JNI::SuspendInAppNotifications(JNI::GetJNIEnv(), JavaCleverTapInstance);
	}

	void SetOffline(bool bIsOffline) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("SetOffline(bIsOffline=%s)"), DEBUG_TEXT(bIsOffline));
		JNI::SetOffline(JNI::GetJNIEnv(), JavaCleverTapInstance, bIsOffline);
	}

	void SetOptOut(bool bIsOptingOut) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("SetOptOut(bIsOptingOut=%s)"), DEBUG_TEXT(bIsOptingOut));
		JNI::SetOptOut(JNI::GetJNIEnv(), JavaCleverTapInstance, bIsOptingOut);
	}

	void SetNetworkInformationRecording(bool bEnableCollection) override
	{
		UE_LOG(LogCleverTap, Log, TEXT("SetNetworkInformationRecording(bEnableCollection=%s)"),
			DEBUG_TEXT(bEnableCollection));
		JNI::EnableDeviceNetworkInfoReporting(JNI::GetJNIEnv(), JavaCleverTapInstance, bEnableCollection);
	}

	static void BroadcastOnOpenUrl(const FString& Url)
	{
		UE_LOG(LogCleverTap, Log, TEXT("OnOpenUrl(%s)"), *Url);
		for (auto& Instance : Instances)
		{
			Instance->OnOpenUrl.Broadcast(Url);
		}
	}
};

FCriticalSection FAndroidCleverTapInstance::CriticalSection;
TSet<FAndroidCleverTapInstance*> FAndroidCleverTapInstance::Instances;

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	UE_LOG(LogCleverTap, Log, TEXT("SetLogLevel(%hs)"), JNI::CleverTapLogLevelJavaName(Level));
	JNI::SetDebugLevel(JNI::GetJNIEnv(), Level);
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
// The callbacks from Java can come from any thread.
// The general pattern below  is to convert to native types in the
// calling thread and then queue the work to the main game thread.
//

extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnPushPermissionResponse__JZ(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jboolean bGranted)
{
	using namespace CleverTapSDK::Android;
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, bGranted]() {
		FAndroidCleverTapInstance::BroadcastOnPushPermissionResponse(NativeInstancePtr, bGranted);
	});
}

extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnPushNotificationClicked__JLjava_lang_Object_2(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject JavaNotificationPayload)
{
	using namespace CleverTapSDK::Android;
	FCleverTapProperties NativePayload = JNI::ConvertJavaMapToCleverTapProperties(Env, JavaNotificationPayload);
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, NativePayload = MoveTemp(NativePayload)]() {
		FAndroidCleverTapInstance::BroadcastOnPushNotificationClicked(NativeInstancePtr, NativePayload);
	});
}

extern "C" JNIEXPORT void JNICALL Java_com_clevertap_android_unreal_UECleverTapBridge_nativeOnOpenUrl(
	JNIEnv* Env, jclass Class, jobject JavaUrl)
{
	using namespace CleverTapSDK::Android;
	FString Url = JNI::JavaObjectToString(Env, JavaUrl);
	AsyncTask(ENamedThreads::GameThread, [Url]() { FAndroidCleverTapInstance::BroadcastOnOpenUrl(Url); });
}

extern "C" JNIEXPORT bool JNICALL
Java_com_clevertap_android_unreal_UECleverTapListener_nativeBeforeShowInAppNotification(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject Extras)
{
	// We need to return back to the calling thread if we want this notification shown or not,
	// So we can't just fire and forget an AsyncTask here, we need to cope with running from an arbitrary thread
	using namespace CleverTapSDK::Android;
	FCleverTapProperties NativeExtras = JNI::ConvertJavaMapToCleverTapProperties(Env, Extras);
	return FAndroidCleverTapInstance::BeforeShowInAppNotification(NativeInstancePtr, NativeExtras);
}

extern "C" JNIEXPORT void JNICALL Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnShowInAppNotification(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject ctInAppNotification)
{
	using namespace CleverTapSDK::Android;
	FCleverTapProperties NativePayload =
		JNI::ConvertJavaCTInAppNotificationToCleverTapProperties(Env, ctInAppNotification);
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, NativePayload = MoveTemp(NativePayload)]() {
		FAndroidCleverTapInstance::BroadcastOnInAppNotificationShown(NativeInstancePtr, NativePayload);
	});
}

extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnInAppNotificationDismissed(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject Extras, jobject ActionExtras)
{
	using namespace CleverTapSDK::Android;
	FCleverTapProperties NativeExtras = JNI::ConvertJavaMapToCleverTapProperties(Env, Extras);
	FCleverTapProperties NativeActionExtras = JNI::ConvertJavaMapToCleverTapProperties(Env, ActionExtras);
	AsyncTask(ENamedThreads::GameThread,
		[NativeInstancePtr, NativeExtras = MoveTemp(NativeExtras),
			NativeActionExtras = MoveTemp(NativeActionExtras)]() {
			FAndroidCleverTapInstance::BroadcastOnInAppNotificationDismissed(
				NativeInstancePtr, NativeExtras, NativeActionExtras);
		});
}

extern "C" JNIEXPORT void JNICALL Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnInAppButtonClick(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject JavaButtonProperties)
{
	using namespace CleverTapSDK::Android;
	FCleverTapProperties NativeButtonProperties = JNI::ConvertJavaMapToCleverTapProperties(Env, JavaButtonProperties);
	AsyncTask(
		ENamedThreads::GameThread, [NativeInstancePtr, NativeButtonProperties = MoveTemp(NativeButtonProperties)]() {
			FAndroidCleverTapInstance::BroadcastOnInAppNotificationButtonClicked(
				NativeInstancePtr, NativeButtonProperties);
		});
}
