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

bool SetDebugLevel(JNIEnv* Env, ECleverTapLogLevel Level);

bool LocalizeNotificationChannel(
	JNIEnv* Env, const FString& ChannelId, const FText& ChannelName, const FText& Description);

bool LocalizeNotificationChannelGroup(JNIEnv* Env, const FString& GroupId, const FText& GroupName);

jobject CreateUECleverTapListener(JNIEnv* Env, jobject CleverTapInstance, void* NativeInstance);
void RegisterPushPermissionResponseListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance);
void RegisterPushNotificationClickedListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance);

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
void AddMultiValuesForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const TArray<FString> Values);
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

void EnableIntentNotifications(JNIEnv* Env);

}}} // namespace CleverTapSDK::Android::JNI
