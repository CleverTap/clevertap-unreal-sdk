// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapLogLevel.h"
#include "CleverTapProperties.h"
#include "CleverTapInstanceConfig.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

JNIEnv* GetJNIEnv();
jclass LoadJavaClass(JNIEnv* Env, const char* ClassPath);
jclass GetCleverTapAPIClass(JNIEnv* Env);
jobject GetJavaApplication(JNIEnv* Env);

void RegisterCleverTapLifecycleCallbacks(JNIEnv* Env);

jobject CreateCleverTapInstanceConfig(
	JNIEnv* Env, const FString& AccountId, const FString& AccountToken, const FString& AccountRegion);
void SetDefaultConfig(JNIEnv* Env, jobject ConfigInstance);

jobject CreateCleverTapInstanceConfig(const FCleverTapInstanceConfig& Config);
void SetDefaultConfig(JNIEnv* Env, const FCleverTapInstanceConfig& Config);

jobject GetDefaultInstance(JNIEnv* Env);
jobject GetDefaultInstance(JNIEnv* Env, const FString& CleverTapId);

bool SetDebugLevel(JNIEnv* Env, ECleverTapLogLevel Level);
void ChangeCredentials(JNIEnv* Env, const FString& AccountId, const FString& Token, const FString& Region);
void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile);
void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile, const FString& CleverTapID);
void PushProfile(JNIEnv* Env, jobject CleverTapInstance, jobject Profile);

void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString& EventName);
void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString& EventName, jobject Actions);
void PushChargedEvent(JNIEnv* Env, jobject CleverTapInstance, jobject Actions, jobject Items);

FString GetCleverTapID(JNIEnv* Env, jobject CleverTapInstance);

jobject ConvertCleverTapPropertiesToJavaMap(JNIEnv* Env, const FCleverTapProperties& Properties);
jobject ConvertArrayOfCleverTapPropertiesToJavaArrayOfMap(JNIEnv* Env, const TArray<FCleverTapProperties>& Array);

}}} // namespace CleverTapSDK::Android::JNI
