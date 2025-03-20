// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapLogLevel.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

JNIEnv* GetJNIEnv();
jclass LoadJavaClass(JNIEnv* env, const char* classPath);
jclass GetCleverTapAPIClass(JNIEnv* env);
jobject GetJavaApplication(JNIEnv* env);

void RegisterCleverTapLifecycleCallbacks(JNIEnv* env);
jobject GetCleverTapInstance(JNIEnv* env);
const char* CleverTapLogLevelName(ECleverTapLogLevel Level);
bool SetDebugLevel(JNIEnv* env, const FString& logLevelName);
void ChangeCredentials(JNIEnv* env, const FString& accountId, const FString& token, const FString& region);
bool InitCleverTap();
void OnUserLogin(JNIEnv* env, jobject cleverTapInstance, jobject profile );
void OnUserLogin(JNIEnv* env, jobject cleverTapInstance, jobject profile, const FString& cleverTapID );
FString GetCleverTapID(JNIEnv* env, jobject cleverTapInstance);

}}} // namespace CleverTapSDK::Android::JNI
