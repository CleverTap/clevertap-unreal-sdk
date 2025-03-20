// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapJNI.h"

#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

static bool s_bCatchJNIExceptions = true; // todo hook this somewhere better

static bool JNIExceptionThrown(JNIEnv* env, const FString& context)
{
	if (env->ExceptionCheck())
	{
		UE_LOG(LogTemp, Error, TEXT("JNI Exception in %s"), *context);

		if (s_bCatchJNIExceptions)
		{
			env->ExceptionDescribe(); // Log stack trace
			env->ExceptionClear();	  // Prevent crash
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Fatal, TEXT("JNI Exception in %s - Crashing due to strict mode"), *context);
			env->ExceptionDescribe();
			return true; // Let Unreal crash naturally
		}
	}
	return false;
}

jclass LoadJavaClass(JNIEnv* env, const char* classPath)
{
	// todo there has to be a simpler way to do this!
	FString context = TEXT("Get GameAtivity class");
	jobject activity = FAndroidApplication::GetGameActivityThis();
	jclass activityClass = env->GetObjectClass(activity);
	if (JNIExceptionThrown(env, context) || !activityClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
		return nullptr;
	}

	context = TEXT("Get getClassLoader method from GameActivity");
	jmethodID getClassLoaderMethod = env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if (JNIExceptionThrown(env, context) || !getClassLoaderMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
		return nullptr;
	}

	context = TEXT("Get ClassLoader from GameActivity");
	jobject classLoader = env->CallObjectMethod(activity, getClassLoaderMethod);
	if (JNIExceptionThrown(env, context) || !classLoader)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
		return nullptr;
	}

	context = TEXT("Find ClassLoader class");
	jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
	if (JNIExceptionThrown(env, context) || !classLoaderClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
		return nullptr;
	}

	context = TEXT("Get ClassLoader loadClass method");
	jmethodID loadClass = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if (JNIExceptionThrown(env, context) || !loadClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
		return nullptr;
	}

	jstring className = env->NewStringUTF(classPath);
	jclass foundClass = (jclass)env->CallObjectMethod(classLoader, loadClass, className);
	if (JNIExceptionThrown(env, TEXT("LoadClass")) || !foundClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to load class: %s"), *FString(classPath));
	}
	return foundClass;
}

jobject GetJavaApplication(JNIEnv* env)
{
	jobject activity = FAndroidApplication::GetGameActivityThis();
	jclass activityClass = env->GetObjectClass(activity);
	FString context = TEXT("GetMethodId GameActivity.getApplication()");
	jmethodID getApplicationMethod = env->GetMethodID(activityClass, "getApplication", "()Landroid/app/Application;");
	if (JNIExceptionThrown(env, context) || !getApplicationMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed."), *context);
		return nullptr;
	}

	context = TEXT("GameActivity.getApplication()");
	jobject application = env->CallObjectMethod(activity, getApplicationMethod);
	if (JNIExceptionThrown(env, TEXT("LoadClass")) || !application)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed."), *context);
		return nullptr;
	}

	return application;
}

void RegisterCleverTapLifecycleCallbacks(JNIEnv* env)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::RegisterCleverTapLifecycleCallbacks()"));

	if (!env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return;
	}

	jobject application = GetJavaApplication(env);
	if (!application)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to get application instance!"));
		return;
	}

	// Find ActivityLifecycleCallback class
	jclass lifecycleCallbackClass = LoadJavaClass(env, "com/clevertap/android/sdk/ActivityLifecycleCallback");
	if (!lifecycleCallbackClass)
	{
		return;
	}

	// Get the static register(Application) method
	jmethodID registerMethod =
		env->GetStaticMethodID(lifecycleCallbackClass, "register", "(Landroid/app/Application;)V");
	if (!registerMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find ActivityLifecycleCallback#register method!"));
		return;
	}

	// Call register(application)
	env->CallStaticVoidMethod(lifecycleCallbackClass, registerMethod, application);

	// Cleanup local references
	env->DeleteLocalRef(application);
	env->DeleteLocalRef(lifecycleCallbackClass);

	UE_LOG(LogCleverTap, Log, TEXT("Successfully registered CleverTap lifecycle callbacks via JNI"));
}

jclass GetCleverTapAPIClass(JNIEnv* env)
{
	return LoadJavaClass(env, "com.clevertap.android.sdk.CleverTapAPI");
}

jobject GetCleverTapInstance(JNIEnv* env)
{
	jclass cleverTapAPIClass = GetCleverTapAPIClass(env);
	if (!cleverTapAPIClass)
	{
		return nullptr;
	}
	jmethodID getInstanceMethod = env->GetStaticMethodID(
		cleverTapAPIClass, "getDefaultInstance", "(Landroid/content/Context;)Lcom/clevertap/android/sdk/CleverTapAPI;");
	FString context = TEXT("GetStaticMethodID CleverTapAPI.getDefaultInstance()");
	if (JNIExceptionThrown(env, context) || !getInstanceMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed"), *context);
		return nullptr;
	}

	context = TEXT("getDefaultInstance()");
	jobject activity = FAndroidApplication::GetGameActivityThis();
	jobject cleverTapInstance = env->CallStaticObjectMethod(cleverTapAPIClass, getInstanceMethod, activity);
	if (JNIExceptionThrown(env, context) || cleverTapInstance == nullptr)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed"), *context);
		return nullptr;
	}
	return cleverTapInstance;
}

static jobject JavaLogLevelLevelFromString(JNIEnv* env, const FString& logLevelName)
{
	jclass LogLevelClass = LoadJavaClass(env, "com/clevertap/android/sdk/CleverTapAPI$LogLevel");
	if (!LogLevelClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTapAPI$LogLevel class"));
		return nullptr;
	}

	// Find the method to get the enum constant from the name
	FString context = TEXT("Find CleverTapAPI$LogLevel.valueOf(String) method");
	jmethodID ValueOfMethod = env->GetStaticMethodID(
		LogLevelClass, "valueOf", "(Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;");
	if (JNIExceptionThrown(env, context) || !ValueOfMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
		env->DeleteLocalRef(LogLevelClass);
		return nullptr;
	}
	// Get the enum constant from the name
	context = TEXT("LogLevel.ValueOf(string)");
	jstring JavaLogLevelName = env->NewStringUTF(TCHAR_TO_UTF8(*logLevelName));
	jobject LogLevelEnumValue = env->CallStaticObjectMethod(LogLevelClass, ValueOfMethod, JavaLogLevelName);
	env->DeleteLocalRef(JavaLogLevelName);
	env->DeleteLocalRef(LogLevelClass);
	if (JNIExceptionThrown(env, context) || !LogLevelEnumValue)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to get LogLevel enum for: %s"), *logLevelName);
		return nullptr;
	}
	return LogLevelEnumValue;
}

const char* CleverTapLogLevelName(ECleverTapLogLevel Level)
{
	switch (Level)
	{
		case ECleverTapLogLevel::Off:
			return "OFF";
		case ECleverTapLogLevel::Info:
			return "INFO";
		case ECleverTapLogLevel::Debug:
			return "DEBUG";
		case ECleverTapLogLevel::Verbose:
			return "VERBOSE";
		default:
		{
			UE_LOG(
				LogCleverTap, Error, TEXT("Unhandled ECleverTapLogLevel value. Defaulting to ECleverTapLogLevel::Off"));
			return "OFF";
		}
	}
}

bool SetDebugLevel(JNIEnv* env, const FString& logLevelName)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::SetDebugLevel(%s)"), *logLevelName);
	jclass cleverTapAPIClass = GetCleverTapAPIClass(env);
	if (!cleverTapAPIClass)
	{
		return false;
	}
	jmethodID SetDebugLogLevelMethod = env->GetStaticMethodID(
		cleverTapAPIClass, "setDebugLevel", "(Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;)V");
	if (JNIExceptionThrown(env, TEXT("GetStaticMethodID - setDebugLevel")) || !SetDebugLogLevelMethod)
	{
		env->DeleteLocalRef(cleverTapAPIClass);
		return false;
	}

	jobject javaLogLevel = JavaLogLevelLevelFromString(env, logLevelName);
	if (!javaLogLevel)
	{
		env->DeleteLocalRef(cleverTapAPIClass);
		return false;
	}

	env->CallStaticVoidMethod(cleverTapAPIClass, SetDebugLogLevelMethod, javaLogLevel);
	env->DeleteLocalRef(cleverTapAPIClass);
	env->DeleteLocalRef(javaLogLevel);
	if (JNIExceptionThrown(env, TEXT("CallStaticVoidMethod - setDebugLevel")))
	{
		return false;
	}
	return true;
}

void ChangeCredentials(JNIEnv* env, const FString& accountId, const FString& token, const FString& region)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::ChangeCredentials()"));

	// Find the CleverTapAPI class
	jclass cleverTapAPIClass = GetCleverTapAPIClass(env);
	if (!cleverTapAPIClass)
	{
		return;
	}

	// Get the changeCredentials method ID
	FString context = TEXT("Find CleverTapAPI changeCredentials method");
	jmethodID changeCredentialsMethod = env->GetStaticMethodID(
		cleverTapAPIClass, "changeCredentials", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (JNIExceptionThrown(env, context) || !changeCredentialsMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
		return;
	}

	// Convert FString to jstring
	jstring jAccountId = env->NewStringUTF(TCHAR_TO_UTF8(*accountId));
	jstring jToken = env->NewStringUTF(TCHAR_TO_UTF8(*token));
	jstring jRegion = env->NewStringUTF(TCHAR_TO_UTF8(*region));

	// Call the method
	env->CallStaticVoidMethod(cleverTapAPIClass, changeCredentialsMethod, jAccountId, jToken, jRegion);
	if (JNIExceptionThrown(env, "ChangeCredentials"))
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *context);
	}

	// Cleanup local references
	env->DeleteLocalRef(jAccountId);
	env->DeleteLocalRef(jToken);
	env->DeleteLocalRef(jRegion);
	env->DeleteLocalRef(cleverTapAPIClass);
}

void OnUserLogin(JNIEnv* env, jobject cleverTapInstance)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::OnUserLogin()"));

	// Get HashMap class and constructor
	jclass hashMapClass = env->FindClass("java/util/HashMap");
	jmethodID hashMapConstructor = env->GetMethodID(hashMapClass, "<init>", "()V");

	// Create an empty HashMap instance
	jobject emptyHashMap = env->NewObject(hashMapClass, hashMapConstructor);

	// Get the OnUserLogin method
	jclass cleverTapAPIClass = GetCleverTapAPIClass(env);
	if (!cleverTapAPIClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("CleverTapAPI not found in JNI!"));
		return;
	}
	jmethodID onUserLoginMethod = env->GetMethodID(cleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;)V");

	// Call onUserLogin with an empty HashMap
	env->CallVoidMethod(cleverTapInstance, onUserLoginMethod, emptyHashMap);

	// Clean up local references
	env->DeleteLocalRef(emptyHashMap);
	env->DeleteLocalRef(hashMapClass);
	env->DeleteLocalRef(cleverTapAPIClass);

	UE_LOG(LogCleverTap, Log, TEXT("Called CleverTap onUserLogin with an empty HashMap"));
}

bool InitCleverTap()
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::InitCleverTap()"));

	JNIEnv* env = FAndroidApplication::GetJavaEnv();
	if (env == nullptr)
	{
		UE_LOG(LogCleverTap, Error, TEXT("FAndroidApplication::GetJavaEnv() returned nullptr"));
		return false;
	}

	// This happens too late for the Android CleverTapSDK to be happy:
	//	RegisterCleverTapLifecycleCallbacks(env);
	//	ChangeCleverTapCredentials(env, project_id, token, region ));

	jobject cleverTapInstance = GetCleverTapInstance(env);
	if (cleverTapInstance)
	{
		UE_LOG(LogCleverTap, Log, TEXT("CleverTap Initialized Successfully!"));

		OnUserLogin(env, cleverTapInstance);
		return true;
	}
	else
	{
		UE_LOG(LogCleverTap, Error, TEXT("CleverTap Instance is NULL!"));
		return false;
	}
}

}}} // namespace CleverTapSDK::Android::JNI
