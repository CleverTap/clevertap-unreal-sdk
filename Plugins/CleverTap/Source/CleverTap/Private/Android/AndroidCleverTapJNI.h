// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapLogLevel.h"
#include "CleverTapProfile.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

JNIEnv* GetJNIEnv();
jclass LoadJavaClass(JNIEnv* Env, const char* ClassPath);
jclass GetCleverTapAPIClass(JNIEnv* Env);
jobject GetJavaApplication(JNIEnv* Env);

void RegisterCleverTapLifecycleCallbacks(JNIEnv* Env);
jobject GetCleverTapInstance(JNIEnv* Env);
bool SetDebugLevel(JNIEnv* Env, ECleverTapLogLevel Level);
void ChangeCredentials(JNIEnv* Env, const FString& AccountId, const FString& Token, const FString& Region);
void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile);
void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile, const FString& CleverTapID);
void PushProfile(JNIEnv* Env, jobject CleverTapInstance, jobject Profile);

void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString &EventName );
void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString &EventName, jobject Actions );
void PushChargedEvent(JNIEnv* Env, jobject CleverTapInstance, jobject Actions, jobject Items );

FString GetCleverTapID(JNIEnv* Env, jobject CleverTapInstance);

jobject ConvertCleverTapPropertiesToJavaMap(JNIEnv* Env, const FCleverTapProperties& Properties);

bool InitCleverTap();

}}} // namespace CleverTapSDK::Android::JNI
