// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapInstanceConfig.h"
#include "CleverTapLogLevel.h"
#include "CleverTapProperties.h"
#include "CleverTapPushPrimerConfig.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

jclass GetCleverTapAPIClass(JNIEnv* Env);
void SetDefaultConfig(JNIEnv* Env, const FCleverTapInstanceConfig& Config);

jobject GetDefaultInstance(JNIEnv* Env);
jobject GetDefaultInstance(JNIEnv* Env, const FString& CleverTapId);

const char* CleverTapLogLevelJavaName(ECleverTapLogLevel LogLevel);
bool SetDebugLevel(JNIEnv* Env, ECleverTapLogLevel Level);

bool LocalizeNotificationChannel(
	JNIEnv* Env, const FString& ChannelId, const FText& ChannelName, const FText& Description);

bool LocalizeNotificationChannelGroup(JNIEnv* Env, const FString& GroupId, const FText& GroupName);

jobject CreateUECleverTapListener(JNIEnv* Env, jobject CleverTapInstance, void* NativeInstance);
void RegisterPushPermissionResponseListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance);
void SetPushNotificationClickedListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance);
void SetInAppNotificationListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance);
void SetInAppNotificationButtonListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance);

void DiscardInAppNotifications(JNIEnv* Env, jobject CleverTapInstance);
void ResumeInAppNotifications(JNIEnv* Env, jobject CleverTapInstance);
void SuspendInAppNotifications(JNIEnv* Env, jobject CleverTapInstance);

FString GetCleverTapID(JNIEnv* Env, jobject CleverTapInstance);

void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile);
void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile, const FString& CleverTapID);
void PushProfile(JNIEnv* Env, jobject CleverTapInstance, jobject Profile);

void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString& EventName);
void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString& EventName, jobject Actions);
void PushChargedEvent(JNIEnv* Env, jobject CleverTapInstance, jobject Actions, jobject Items);

jobject GetProperty(JNIEnv* Env, jobject CleverTapInstance, const FString& Key);

void DecrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, int Amount);
void DecrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, double Amount);

void IncrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, int Amount);
void IncrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, double Amount);

void AddMultiValueForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const FString& Value);
void AddMultiValuesForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const TArray<FString>& Values);
void RemoveMultiValueForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const FString& Value);
void RemoveMultiValuesForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const TArray<FString>& Values);
void RemoveValueForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key);
void SetMultiValuesForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const TArray<FString> Values);

bool IsPushPermissionGranted(JNIEnv* Env, jobject CleverTapInstance);
void PromptForPushPermission(JNIEnv* Env, jobject CleverTapInstance, bool bFallbackToSettings);

jobject CreatePushPrimerConfigJSON(JNIEnv* Env, const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig);
jobject CreatePushPrimerConfigJSON(
	JNIEnv* Env, const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig);
void PromptPushPrimer(JNIEnv* Env, jobject CleverTapInstance, jobject PrimerConfigJSON);

void SetOffline(JNIEnv* Env, jobject CleverTapInstance, bool bIsOffline);
void SetOptOut(JNIEnv* Env, jobject CleverTapInstance, bool bIsOptingOut);
void EnableDeviceNetworkInfoReporting(JNIEnv* Env, jobject CleverTapInstance, bool bEnableCollection);

void EnableIntentNotifications(JNIEnv* Env);

// Product Experiences (Variables)
void DefineStringVariable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name, const FString& DefaultValue);
void DefineIntVariable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name, int32 DefaultValue);
void DefineInt64Variable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name, int64 DefaultValue);
void DefineFloatVariable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name, float DefaultValue);
void DefineDoubleVariable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name, double DefaultValue);
void DefineBoolVariable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name, bool DefaultValue);
void DefineStringMapVariable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name, const TMap<FString, FString>& DefaultValue);
void DefineFileVariable(JNIEnv* Env, jobject CleverTapInstance, const FString& Name);
void FetchVariables(JNIEnv* Env, jobject CleverTapInstance, jlong NativeInstancePtr);
void SyncVariables(JNIEnv* Env, jobject CleverTapInstance);
FString GetStringVariable(JNIEnv* Env, const FString& Name, const FString& DefaultValue);
int32 GetIntVariable(JNIEnv* Env, const FString& Name, int32 DefaultValue);
int64 GetInt64Variable(JNIEnv* Env, const FString& Name, int64 DefaultValue);
float GetFloatVariable(JNIEnv* Env, const FString& Name, float DefaultValue);
double GetDoubleVariable(JNIEnv* Env, const FString& Name, double DefaultValue);
bool GetBoolVariable(JNIEnv* Env, const FString& Name, bool DefaultValue);
TMap<FString, FString> GetStringMapVariable(JNIEnv* Env, const FString& Name, const TMap<FString, FString>& DefaultValue);
FString GetFileVariablePath(JNIEnv* Env, const FString& Name);

}}} // namespace CleverTapSDK::Android::JNI
