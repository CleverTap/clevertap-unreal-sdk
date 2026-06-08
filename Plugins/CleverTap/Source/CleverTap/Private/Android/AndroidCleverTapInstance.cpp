// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapInstance.h"

#include "Android/AndroidCleverTapJNI.h"
#include "Android/AndroidCleverTapPropertiesJNI.h"
#include "Android/AndroidJNIUtilities.h"

#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"
#include "CleverTapSubsystem.h"

using namespace CleverTapSDK::Android;

static inline const TCHAR* DEBUG_TEXT(bool b)
{
	return b ? TEXT("TRUE") : TEXT("FALSE");
}

FCriticalSection UAndroidCleverTapInstance::CriticalSection;
TSet<UAndroidCleverTapInstance*> UAndroidCleverTapInstance::Instances;

UAndroidCleverTapInstance* UAndroidCleverTapInstance::CheckedInstancePtr(jlong NativeInstancePtr)
{
	auto* Instance = reinterpret_cast<UAndroidCleverTapInstance*>(NativeInstancePtr);
	if (UAndroidCleverTapInstance::IsValid(Instance))
	{
		return Instance;
	}
	else
	{
		UE_LOG(LogCleverTap, Warning, TEXT("CheckedInstancePtr: Invalid or expired NativeInstancePtr!"));
		return nullptr;
	}
}

UAndroidCleverTapInstance* UAndroidCleverTapInstance::Create(JNIEnv* Env, jobject JavaCleverTapInstance)
{
	auto* Instance = NewObject<UAndroidCleverTapInstance>(GEngine->GetEngineSubsystem<UCleverTapSubsystem>());
	Instance->Initialize(Env, JavaCleverTapInstance);
	return Instance;
}

void UAndroidCleverTapInstance::Initialize(JNIEnv* Env, jobject JavaCleverTapInstanceIn)
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

UAndroidCleverTapInstance::~UAndroidCleverTapInstance()
{
	check(IsInGameThread());
	FScopeLock Lock(&CriticalSection);
	auto* Env = JNI::GetJNIEnv();
	if (Env && JavaCleverTapInstance)
	{
		Env->DeleteGlobalRef(JavaCleverTapInstance);
	}
	if (Instances.Contains(this))
	{
		Instances.Remove(this);
	}
}

FString UAndroidCleverTapInstance::GetCleverTapId()
{
	auto* Env = JNI::GetJNIEnv();
	return JNI::GetCleverTapID(Env, JavaCleverTapInstance);
}

void UAndroidCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile)
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

void UAndroidCleverTapInstance::OnUserLoginWithCleverTapId(
	const FCleverTapProperties& Profile, const FString& CleverTapId)
{
	UE_LOG(LogCleverTap, Log, TEXT("OnUserLoginWithCleverTapId(Profile=%s, CleverTapId=%s)"), *ToDebugString(Profile),
		*CleverTapId);
	auto* Env = JNI::GetJNIEnv();
	if (Env)
	{
		jobject JavaProfile = JNI::ConvertCleverTapPropertiesToJavaMap(Env, Profile);
		JNI::OnUserLogin(Env, JavaCleverTapInstance, JavaProfile, CleverTapId);
		Env->DeleteLocalRef(JavaProfile);
	}
}

void UAndroidCleverTapInstance::PushProfile(const FCleverTapProperties& Profile)
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

void UAndroidCleverTapInstance::PushEvent(const FString& EventName)
{
	UE_LOG(LogCleverTap, Log, TEXT("PushEvent(EventName=%s)"), *EventName);
	auto* Env = JNI::GetJNIEnv();
	JNI::PushEvent(Env, JavaCleverTapInstance, EventName);
}

void UAndroidCleverTapInstance::PushEventWithProperties(
	const FString& EventName, const FCleverTapProperties& EventProperties)
{
	UE_LOG(LogCleverTap, Log, TEXT("PushEventWithProperties(EventName=%s, EventProperties=%s)"), *EventName,
		*ToDebugString(EventProperties));
	auto* Env = JNI::GetJNIEnv();
	jobject JavaMap = JNI::ConvertCleverTapPropertiesToJavaMap(Env, EventProperties);
	JNI::PushEvent(Env, JavaCleverTapInstance, EventName, JavaMap);
	Env->DeleteLocalRef(JavaMap);
}

void UAndroidCleverTapInstance::PushChargedEvent(
	const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items)
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

TOptional<FCleverTapPropertyValue> UAndroidCleverTapInstance::GetProperty(const FString& Key)
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

void UAndroidCleverTapInstance::DecrementIntValue(const FString& Key, int Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("DecrementIntValue(%s, %d)"), *Key, Amount);
	JNI::DecrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
}

void UAndroidCleverTapInstance::DecrementFloatValue(const FString& Key, float Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("DecrementFloatValue(%s, %f)"), *Key, Amount);
	JNI::DecrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, double(Amount));
}

void UAndroidCleverTapInstance::IncrementIntValue(const FString& Key, int Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("IncrementIntValue(%s, %d)"), *Key, Amount);
	JNI::IncrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Amount);
}

void UAndroidCleverTapInstance::IncrementFloatValue(const FString& Key, float Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("IncrementFloatValue(%s, %f)"), *Key, Amount);
	JNI::IncrementValue(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, double(Amount));
}

void UAndroidCleverTapInstance::AddMultiValueForKey(const FString& Key, const FString& Value)
{
	UE_LOG(LogCleverTap, Log, TEXT("AddMultiValueForKey(%s, %s)"), *Key, *Value);
	JNI::AddMultiValueForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Value);
}

void UAndroidCleverTapInstance::AddMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
{
	UE_LOG(
		LogCleverTap, Log, TEXT("AddMultiValuesForKey(%s, %s)"), *Key, *ToDebugString(FCleverTapPropertyValue(Values)));
	JNI::AddMultiValuesForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Values);
}

void UAndroidCleverTapInstance::RemoveMultiValueForKey(const FString& Key, const FString& Value)
{
	UE_LOG(LogCleverTap, Log, TEXT("RemoveMultiValueForKey(%s, %s)"), *Key, *Value);
	JNI::RemoveMultiValueForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Value);
}

void UAndroidCleverTapInstance::RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
{
	UE_LOG(LogCleverTap, Log, TEXT("RemoveMultiValuesForKey(%s, %s)"), *Key,
		*ToDebugString(FCleverTapPropertyValue(Values)));
	JNI::RemoveMultiValuesForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Values);
}

void UAndroidCleverTapInstance::RemoveValueForKey(const FString& Key)
{
	UE_LOG(LogCleverTap, Log, TEXT("RemoveValueForKey(%s)"), *Key);
	JNI::RemoveValueForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key);
}

void UAndroidCleverTapInstance::SetMultiValuesForKey(const FString& Key, const TArray<FString> Values)
{
	UE_LOG(
		LogCleverTap, Log, TEXT("SetMultiValuesForKey(%s, %s)"), *Key, *ToDebugString(FCleverTapPropertyValue(Values)));
	JNI::SetMultiValuesForKey(JNI::GetJNIEnv(), JavaCleverTapInstance, Key, Values);
}

bool UAndroidCleverTapInstance::LocalizeAndroidNotificationChannel(
	const FString& ChannelId, const FText& ChannelName, const FText& ChannelDescription)
{
	UE_LOG(LogCleverTap, Log, TEXT("LocalizeAndroidNotificationChannel(%s)"), *ChannelId);
	bool Success = JNI::LocalizeNotificationChannel(JNI::GetJNIEnv(), ChannelId, ChannelName, ChannelDescription);
	if (!Success)
	{
		UE_LOG(LogCleverTap, Error, TEXT("LocalizeAndroidNotificationChannel(%s) failed. Unknown ID?"), *ChannelId);
	}
	return Success;
}

bool UAndroidCleverTapInstance::LocalizeAndroidNotificationChannelGroup(const FString& GroupId, const FText& GroupName)
{
	UE_LOG(LogCleverTap, Log, TEXT("LocalizeAndroidNotificationChannelGroup(%s)"), *GroupId);
	bool Success = JNI::LocalizeNotificationChannelGroup(JNI::GetJNIEnv(), GroupId, GroupName);
	if (!Success)
	{
		UE_LOG(LogCleverTap, Error, TEXT("LocalizeAndroidNotificationChannelGroup(%s) failed. Unknown ID?"), *GroupId);
	}
	return Success;
}

ECleverTapPushPermissionStatus UAndroidCleverTapInstance::GetPushPermissionStatus()
{
	if (JNI::IsPushPermissionGranted(JNI::GetJNIEnv(), JavaCleverTapInstance))
	{
		return ECleverTapPushPermissionStatus::Granted;
	}
	return ECleverTapPushPermissionStatus::NotGranted;
}

void UAndroidCleverTapInstance::PromptForPushPermission(bool bFallbackToSettings)
{
	UE_LOG(LogCleverTap, Log, TEXT("PromptForPushPermission(bFallbackToSettings=%s)"), DEBUG_TEXT(bFallbackToSettings));
	JNI::PromptForPushPermission(JNI::GetJNIEnv(), JavaCleverTapInstance, bFallbackToSettings);
}

void UAndroidCleverTapInstance::PromptForPushPermissionWithAlertPrimer(
	const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig)
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

void UAndroidCleverTapInstance::PromptForPushPermissionWithHalfInterstitialPrimer(
	const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig)
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

void UAndroidCleverTapInstance::BroadcastOnPushPermissionResponse(jlong NativeInstancePtr, bool bGranted)
{
	UE_LOG(LogCleverTap, Log, TEXT("OnPushPermissionResponse(bGranted=%s)"), DEBUG_TEXT(bGranted));
	check(IsInGameThread());
	auto* Instance = CheckedInstancePtr(NativeInstancePtr);
	if (Instance)
	{
		Instance->OnPushPermissionResponse.Broadcast(bGranted);
	}
}

void UAndroidCleverTapInstance::BroadcastOnPushNotificationClicked(const FCleverTapProperties& NotificationProperties)
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
void UAndroidCleverTapInstance::BroadcastOnPushNotificationClicked(
	jlong NativeInstancePtr, const FCleverTapProperties& NotificationProperties)
{
	check(IsInGameThread());
	auto* Instance = CheckedInstancePtr(NativeInstancePtr);
	if (Instance)
	{
		Instance->BroadcastOnPushNotificationClicked(NotificationProperties);
	}
}

void UAndroidCleverTapInstance::EnableOnPushNotificationClicked()
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

void UAndroidCleverTapInstance::EnableOnOpenUrl()
{
	UE_LOG(LogCleverTap, Log, TEXT("EnableOnOpenUrl()"));
	JNI::EnableIntentNotifications(JNI::GetJNIEnv());
}

void UAndroidCleverTapInstance::RegisterCleverTapUrlHandler(
	TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler)
{
	UE_LOG(LogCleverTap, Log, TEXT("RegisterCleverTapUrlHandler()"));
	CleverTapSDK::Ignore(UrlHandler);
}

void UAndroidCleverTapInstance::RegisterInAppNotificationFilter(
	TUniqueFunction<bool(const FCleverTapProperties&)> Filter)
{
	UE_LOG(LogCleverTap, Log, TEXT("RegisterInAppNotificationFilter()"));
	check(IsInGameThread());
	FScopeLock Lock(&CriticalSection);
	InAppNotificationFilter = MoveTemp(Filter);
}

bool UAndroidCleverTapInstance::BeforeShowInAppNotification(jlong NativeInstancePtr, const FCleverTapProperties& Extras)
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

void UAndroidCleverTapInstance::BroadcastOnInAppNotificationShown(
	jlong NativeInstancePtr, const FCleverTapProperties& Payload)
{
	UE_LOG(LogCleverTap, Log, TEXT("OnInAppNotificationShown(%s)"), *ToDebugString(Payload));
	check(IsInGameThread());
	auto* Instance = CheckedInstancePtr(NativeInstancePtr);
	if (Instance)
	{
		Instance->OnInAppNotificationShown.Broadcast(Payload);
	}
}

void UAndroidCleverTapInstance::BroadcastOnInAppNotificationDismissed(
	jlong NativeInstancePtr, const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras)
{
	UE_LOG(LogCleverTap, Log, TEXT("OnInAppNotificationDismissed(Extras=%s, ActionExtras=%s)"), *ToDebugString(Extras),
		*ToDebugString(ActionExtras));
	check(IsInGameThread());
	auto* Instance = CheckedInstancePtr(NativeInstancePtr);
	if (Instance)
	{
		Instance->OnInAppNotificationDismissed.Broadcast(Extras, ActionExtras);
	}
}

void UAndroidCleverTapInstance::BroadcastOnInAppNotificationButtonClicked(
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

void UAndroidCleverTapInstance::DiscardInAppNotifications()
{
	UE_LOG(LogCleverTap, Log, TEXT("DiscardInAppNotifications()"));
	JNI::DiscardInAppNotifications(JNI::GetJNIEnv(), JavaCleverTapInstance);
}

void UAndroidCleverTapInstance::ResumeInAppNotifications()
{
	UE_LOG(LogCleverTap, Log, TEXT("ResumeInAppNotifications()"));
	JNI::ResumeInAppNotifications(JNI::GetJNIEnv(), JavaCleverTapInstance);
}

void UAndroidCleverTapInstance::SuspendInAppNotifications()
{
	UE_LOG(LogCleverTap, Log, TEXT("SuspendInAppNotifications()"));
	JNI::SuspendInAppNotifications(JNI::GetJNIEnv(), JavaCleverTapInstance);
}

void UAndroidCleverTapInstance::SetOffline(bool bIsOffline)
{
	UE_LOG(LogCleverTap, Log, TEXT("SetOffline(bIsOffline=%s)"), DEBUG_TEXT(bIsOffline));
	JNI::SetOffline(JNI::GetJNIEnv(), JavaCleverTapInstance, bIsOffline);
}

void UAndroidCleverTapInstance::SetOptOut(bool bIsOptingOut)
{
	UE_LOG(LogCleverTap, Log, TEXT("SetOptOut(bIsOptingOut=%s)"), DEBUG_TEXT(bIsOptingOut));
	JNI::SetOptOut(JNI::GetJNIEnv(), JavaCleverTapInstance, bIsOptingOut);
}

void UAndroidCleverTapInstance::SetNetworkInformationRecording(bool bEnableCollection)
{
	UE_LOG(
		LogCleverTap, Log, TEXT("SetNetworkInformationRecording(bEnableCollection=%s)"), DEBUG_TEXT(bEnableCollection));
	JNI::EnableDeviceNetworkInfoReporting(JNI::GetJNIEnv(), JavaCleverTapInstance, bEnableCollection);
}

void UAndroidCleverTapInstance::BroadcastOnOpenUrl(const FString& Url)
{
	UE_LOG(LogCleverTap, Log, TEXT("OnOpenUrl(%s)"), *Url);
	for (auto& Instance : Instances)
	{
		Instance->OnOpenUrl.Broadcast(Url);
	}
}

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
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, bGranted]() {
		UAndroidCleverTapInstance::BroadcastOnPushPermissionResponse(NativeInstancePtr, bGranted);
	});
}

extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnPushNotificationClicked__JLjava_lang_Object_2(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject JavaNotificationPayload)
{
	FCleverTapProperties NativePayload = JNI::ConvertJavaMapToCleverTapProperties(Env, JavaNotificationPayload);
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, NativePayload = MoveTemp(NativePayload)]() {
		UAndroidCleverTapInstance::BroadcastOnPushNotificationClicked(NativeInstancePtr, NativePayload);
	});
}

extern "C" JNIEXPORT void JNICALL Java_com_clevertap_android_unreal_UECleverTapBridge_nativeOnOpenUrl(
	JNIEnv* Env, jclass Class, jobject JavaUrl)
{
	FString Url = JNI::JavaObjectToString(Env, JavaUrl);
	AsyncTask(ENamedThreads::GameThread, [Url]() { UAndroidCleverTapInstance::BroadcastOnOpenUrl(Url); });
}

extern "C" JNIEXPORT bool JNICALL
Java_com_clevertap_android_unreal_UECleverTapListener_nativeBeforeShowInAppNotification(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject Extras)
{
	// We need to return back to the calling thread if we want this notification shown or not,
	// So we can't just fire and forget an AsyncTask here, we need to cope with running from an arbitrary thread
	FCleverTapProperties NativeExtras = JNI::ConvertJavaMapToCleverTapProperties(Env, Extras);
	return UAndroidCleverTapInstance::BeforeShowInAppNotification(NativeInstancePtr, NativeExtras);
}

extern "C" JNIEXPORT void JNICALL Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnShowInAppNotification(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject ctInAppNotification)
{
	FCleverTapProperties NativePayload =
		JNI::ConvertJavaCTInAppNotificationToCleverTapProperties(Env, ctInAppNotification);
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, NativePayload = MoveTemp(NativePayload)]() {
		UAndroidCleverTapInstance::BroadcastOnInAppNotificationShown(NativeInstancePtr, NativePayload);
	});
}

extern "C" JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnInAppNotificationDismissed(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject Extras, jobject ActionExtras)
{
	FCleverTapProperties NativeExtras = JNI::ConvertJavaMapToCleverTapProperties(Env, Extras);
	FCleverTapProperties NativeActionExtras = JNI::ConvertJavaMapToCleverTapProperties(Env, ActionExtras);
	AsyncTask(ENamedThreads::GameThread,
		[NativeInstancePtr, NativeExtras = MoveTemp(NativeExtras),
			NativeActionExtras = MoveTemp(NativeActionExtras)]() {
			UAndroidCleverTapInstance::BroadcastOnInAppNotificationDismissed(
				NativeInstancePtr, NativeExtras, NativeActionExtras);
		});
}

extern "C" JNIEXPORT void JNICALL Java_com_clevertap_android_unreal_UECleverTapListener_nativeOnInAppButtonClick(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jobject JavaButtonProperties)
{
	FCleverTapProperties NativeButtonProperties = JNI::ConvertJavaMapToCleverTapProperties(Env, JavaButtonProperties);
	AsyncTask(
		ENamedThreads::GameThread, [NativeInstancePtr, NativeButtonProperties = MoveTemp(NativeButtonProperties)]() {
			UAndroidCleverTapInstance::BroadcastOnInAppNotificationButtonClicked(
				NativeInstancePtr, NativeButtonProperties);
		});
}

//Product Experience

void UAndroidCleverTapInstance::DefineStringVariable(const FString& Name, const FString& DefaultValue)
{
	JNI::DefineStringVariable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name, DefaultValue);
}
void UAndroidCleverTapInstance::DefineIntVariable(const FString& Name, int32 DefaultValue)
{
	JNI::DefineIntVariable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name, DefaultValue);
}
void UAndroidCleverTapInstance::DefineInt64Variable(const FString& Name, int64 DefaultValue)
{
	JNI::DefineInt64Variable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name, DefaultValue);
}
void UAndroidCleverTapInstance::DefineFloatVariable(const FString& Name, float DefaultValue)
{
	JNI::DefineFloatVariable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name, DefaultValue);
}
void UAndroidCleverTapInstance::DefineDoubleVariable(const FString& Name, double DefaultValue)
{
	JNI::DefineDoubleVariable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name, DefaultValue);
}
void UAndroidCleverTapInstance::DefineBoolVariable(const FString& Name, bool DefaultValue)
{
	JNI::DefineBoolVariable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name, DefaultValue);
}
void UAndroidCleverTapInstance::DefineStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue)
{
	JNI::DefineStringMapVariable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name, DefaultValue);
}
void UAndroidCleverTapInstance::DefineFileVariable(const FString& Name)
{
	JNI::DefineFileVariable(JNI::GetJNIEnv(), JavaCleverTapInstance, Name);
}
void UAndroidCleverTapInstance::FetchVariables()
{
	JNI::FetchVariables(JNI::GetJNIEnv(), JavaCleverTapInstance,
		reinterpret_cast<jlong>(this));
}
void UAndroidCleverTapInstance::SyncVariables()
{
	JNI::SyncVariables(JNI::GetJNIEnv(), JavaCleverTapInstance);
}
FString UAndroidCleverTapInstance::GetStringVariable(const FString& Name, const FString& DefaultValue) const
{
	return JNI::GetStringVariable(JNI::GetJNIEnv(), Name, DefaultValue);
}
int32 UAndroidCleverTapInstance::GetIntVariable(const FString& Name, int32 DefaultValue) const
{
	return JNI::GetIntVariable(JNI::GetJNIEnv(), Name, DefaultValue);
}
int64 UAndroidCleverTapInstance::GetInt64Variable(const FString& Name, int64 DefaultValue) const
{
	return JNI::GetInt64Variable(JNI::GetJNIEnv(), Name, DefaultValue);
}
float UAndroidCleverTapInstance::GetFloatVariable(const FString& Name, float DefaultValue) const
{
	return JNI::GetFloatVariable(JNI::GetJNIEnv(), Name, DefaultValue);
}
double UAndroidCleverTapInstance::GetDoubleVariable(const FString& Name, double DefaultValue) const
{
	return JNI::GetDoubleVariable(JNI::GetJNIEnv(), Name, DefaultValue);
}
bool UAndroidCleverTapInstance::GetBoolVariable(const FString& Name, bool DefaultValue) const
{
	return JNI::GetBoolVariable(JNI::GetJNIEnv(), Name, DefaultValue);
}
TMap<FString, FString> UAndroidCleverTapInstance::GetStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue) const
{
	return JNI::GetStringMapVariable(JNI::GetJNIEnv(), Name, DefaultValue);
}
FString UAndroidCleverTapInstance::GetFileVariablePath(const FString& Name) const
{
	return JNI::GetFileVariablePath(JNI::GetJNIEnv(), Name);
}

void UAndroidCleverTapInstance::BroadcastOnVariablesFetched(jlong NativeInstancePtr, bool bSuccess)
{
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr, bSuccess]() {
		if (auto* Inst = CheckedInstancePtr(NativeInstancePtr))
			Inst->OnVariablesFetched.Broadcast(bSuccess);
	});
}
void UAndroidCleverTapInstance::BroadcastOnVariablesChanged(jlong NativeInstancePtr)
{
	AsyncTask(ENamedThreads::GameThread, [NativeInstancePtr]() {
		if (auto* Inst = CheckedInstancePtr(NativeInstancePtr))
			Inst->OnVariablesChanged.Broadcast();
	});
}

// JNI entry points — Java calls these
extern "C"
{
JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapBridge_nativeOnVariablesFetched__JZ(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr, jboolean Success)
{
	UAndroidCleverTapInstance::BroadcastOnVariablesFetched(NativeInstancePtr, (bool)Success);
}

JNIEXPORT void JNICALL
Java_com_clevertap_android_unreal_UECleverTapBridge_nativeOnVariablesChanged__J(
	JNIEnv* Env, jclass Class, jlong NativeInstancePtr)
{
	UAndroidCleverTapInstance::BroadcastOnVariablesChanged(NativeInstancePtr);
}
}
