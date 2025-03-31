// Copyright CleverTap All Rights Reserved.
#pragma once

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

/// Enable crashing on JNI exceptions.
//
// Typically this should be disabled, but during development it can be
// useful to enable crashing here to be highlight any JNI problems you may
// be encountering.
//
// When enabled, a JNI exception will terminate the program with a Fatal log.
// When disabled, the exception details will be logged as Errors and the program
// will continue.
//
void SetCrashOnJNIException(bool CrashOnException);
bool ShouldCrashOnJNIException();

bool ExceptionThrown(JNIEnv* Env);
bool HandleExceptionOrError(JNIEnv* Env, bool bIsError, const FString& Context);
bool HandleException(JNIEnv* Env, const FString& Context);

JNIEnv* GetJNIEnv();
jclass LoadJavaClass(JNIEnv* Env, const char* ClassPath);
FString GetJClassName(JNIEnv* Env, jclass Class);
jmethodID GetMethodID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature);
jmethodID GetStaticMethodID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature);
jfieldID GetStaticFieldID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature);
jobject GetJavaApplication(JNIEnv* Env);

FString JavaObjectToString(JNIEnv* Env, jobject JavaObject);

}}} // namespace CleverTapSDK::Android::JNI
