// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"
#include "CleverTapPlatformSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapInstance.h"
#include "CleverTapLog.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace {

static jclass LoadJavaClass(JNIEnv* env, const char* classPath)
{
	// todo there has to be a simpler way to do this!
	jobject activity = FAndroidApplication::GetGameActivityThis();
	jclass activityClass = env->GetObjectClass(activity);
	if (!activityClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Couldn't get GameActivity class"));
		return nullptr;
	}
	jmethodID getClassLoader = env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if (!getClassLoader)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Couldn't get getClassLoader method from GameActivity"));
		return nullptr;
	}
	jobject classLoader = env->CallObjectMethod(activity, getClassLoader);
	if (!classLoader)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Couldn't get ClassLoader from GameActivity"));
		return nullptr;
	}
	jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
	if (!classLoaderClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Couldn't find ClassLoader class"));
		return nullptr;
	}
	jmethodID loadClass = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if (!loadClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Couldn't find ClassLoader loadClass method"));
		return nullptr;
	}
	jstring className = env->NewStringUTF(classPath);
	jclass foundClass = (jclass)env->CallObjectMethod(classLoader, loadClass, className);
	if (!foundClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to load class: %s"), *FString(classPath));
	}
	return foundClass;
}

static jobject GetJavaApplication(JNIEnv* env)
{
	jobject activity = FAndroidApplication::GetGameActivityThis();
	jclass activityClass = env->GetObjectClass(activity);
	jmethodID getApplicationMethod = env->GetMethodID(activityClass, "getApplication", "()Landroid/app/Application;");

	jobject application = env->CallObjectMethod(activity, getApplicationMethod);
	if (application)
	{
		UE_LOG(LogCleverTap, Log, TEXT("Successfully got Java Application instance."));
	}
	else
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to get Java Application instance."));
	}
	return application;
}

static void RegisterCleverTapLifecycleCallbacks(JNIEnv* env)
{
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
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find ActivityLifecycleCallback class!"));
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

static jobject GetCleverTapInstance(JNIEnv* env)
{
	jclass cleverTapAPIClass = LoadJavaClass(env, "com.clevertap.android.sdk.CleverTapAPI");
	if (!cleverTapAPIClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("CleverTapAPI not found in JNI!"));
		return nullptr;
	}
	jmethodID getInstanceMethod = env->GetStaticMethodID(
		cleverTapAPIClass, "getDefaultInstance", "(Landroid/content/Context;)Lcom/clevertap/android/sdk/CleverTapAPI;");
	if (!getInstanceMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTap getDefaultInstance method!"));
		return nullptr;
	}
	jobject activity = FAndroidApplication::GetGameActivityThis();
	jobject cleverTapInstance = env->CallStaticObjectMethod(cleverTapAPIClass, getInstanceMethod, activity);
	if (cleverTapInstance == nullptr)
	{
		UE_LOG(LogCleverTap, Error, TEXT("CleverTap Instance is NULL!"));
	}
	return cleverTapInstance;
}

static void ChangeCleverTapCredentials(
	JNIEnv* env, const FString& accountId, const FString& token, const FString& region)
{
	// Find the CleverTapAPI class
	jclass cleverTapAPIClass = LoadJavaClass(env, "com.clevertap.android.sdk.CleverTapAPI");
	if (!cleverTapAPIClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTapAPI class!"));
		return;
	}

	// Get the changeCredentials method ID
	jmethodID changeCredentialsMethod = env->GetStaticMethodID(
		cleverTapAPIClass, "changeCredentials", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!changeCredentialsMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTapAPI changeCredentials method!"));
		return;
	}

	// Convert FString to jstring
	jstring jAccountId = env->NewStringUTF(TCHAR_TO_UTF8(*accountId));
	jstring jToken = env->NewStringUTF(TCHAR_TO_UTF8(*token));
	jstring jRegion = env->NewStringUTF(TCHAR_TO_UTF8(*region));

	// Call the method
	env->CallStaticVoidMethod(cleverTapAPIClass, changeCredentialsMethod, jAccountId, jToken, jRegion);

	// Cleanup local references
	env->DeleteLocalRef(jAccountId);
	env->DeleteLocalRef(jToken);
	env->DeleteLocalRef(jRegion);
	env->DeleteLocalRef(cleverTapAPIClass);

	UE_LOG(LogCleverTap, Log, TEXT("Successfully called CleverTapAPI changeCredentials"));
}

static void CallOnUserLogin(JNIEnv* env, jobject cleverTapInstance)
{
	UE_LOG(LogCleverTap, Log, TEXT("CallOnUserLogin"));

	// Get HashMap class and constructor
	jclass hashMapClass = env->FindClass("java/util/HashMap");
	jmethodID hashMapConstructor = env->GetMethodID(hashMapClass, "<init>", "()V");

	// Create an empty HashMap instance
	jobject emptyHashMap = env->NewObject(hashMapClass, hashMapConstructor);

	// Get the OnUserLogin method
	jclass cleverTapAPIClass = LoadJavaClass(env, "com.clevertap.android.sdk.CleverTapAPI");
	if (!cleverTapAPIClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("CleverTapAPI not found in JNI!"));
	}
	jmethodID onUserLoginMethod = env->GetMethodID(cleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;)V");

	// Call onUserLogin with an empty HashMap
	env->CallVoidMethod(cleverTapInstance, onUserLoginMethod, emptyHashMap);

	// Clean up local references
	env->DeleteLocalRef(emptyHashMap);
	env->DeleteLocalRef(hashMapClass);

	UE_LOG(LogCleverTap, Log, TEXT("Called CleverTap onUserLogin with an empty HashMap"));
}

static bool InitCleverTap()
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::::InitCleverTap()"));

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

		CallOnUserLogin(env, cleverTapInstance);
		return true;
	}
	else
	{
		UE_LOG(LogCleverTap, Error, TEXT("CleverTap Instance is NULL!"));
		return false;
	}
}

class FAndroidCleverTapInstance : public ICleverTapInstance
{
public:
	FAndroidCleverTapInstance() { InitCleverTap(); }

	FString GetCleverTapId() const override { return FString{}; } // todo hmm
};

} // namespace

namespace CleverTapSDK { namespace Android {

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	CleverTapSDK::Ignore(Level);
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(const UCleverTapConfig& Config)
{
	CleverTapSDK::Ignore(Config);
	return MakeUnique<FAndroidCleverTapInstance>();
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const UCleverTapConfig& Config, const FString& CleverTapId)
{
	CleverTapSDK::Ignore(Config, CleverTapId);
	return MakeUnique<FAndroidCleverTapInstance>();
}

}} // namespace CleverTapSDK::Android
