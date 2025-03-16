#include "Android/AndroidCleverTapSDK.h"
#include "CleverTapPlatformSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapLog.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android {


static jclass LoadJavaClass(JNIEnv* env, const char* classPath)
{
    jobject activity = FAndroidApplication::GetGameActivityThis();
    jclass activityClass = env->GetObjectClass(activity);
    if (!activityClass) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't get GameActivity class"));
        return nullptr;
    }
    jmethodID getClassLoader = env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    if (!getClassLoader) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't get getClassLoader method from GameActivity"));
        return nullptr;
    }
    jobject classLoader = env->CallObjectMethod(activity, getClassLoader);
    if (!classLoader) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't get ClassLoader from GameActivity"));
        return nullptr;
    }
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    if (!classLoaderClass) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't find ClassLoader class"));
        return nullptr;
    }
    jmethodID loadClass = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    if (!loadClass) {
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

/**
static bool RegisterCleverTapLifecycle( JNIEnv* env )
{
    jclass callbackClass = LoadJavaClass(env, "com.clevertap.android.sdk.ActivityLifecycleCallback");
    if (!callbackClass)
    {
        UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTap ActivityLifecycleCallback!"));
        return false;
    }
    jmethodID registerMethod = env->GetStaticMethodID( callbackClass, "register", "(Landroid/app/Application;)V" );
    if (!registerMethod)
    {
        UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTap ActivityLifecycleCallback register() method!"));
        return false;
    }

    jobject application = FAndroidApplication::GetApplication();
    env->CallStaticVoidMethod(callbackClass, registerMethod, application);

    UE_LOG(LogCleverTap, Log, TEXT("CleverTap lifecycle registered via JNI!"));
    return true;
}

**/

static bool InitCleverTap() {
    UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::::InitCleverTap()"));

    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    if (env == nullptr) {
        UE_LOG(LogCleverTap, Error, TEXT("FAndroidApplication::GetJavaEnv() returned nullptr"));
        return false;
    }
 /*   
    bool registeredOkay = RegisterCleverTapLifecycle(env);
    if( registeredOkay == false ) {
        // keep going even if the lifecycle didn't register properly        
    }
        */
 
    jclass cleverTapAPIClass = LoadJavaClass(env, "com.clevertap.android.sdk.CleverTapAPI");
    if (!cleverTapAPIClass)
    {
        UE_LOG(LogCleverTap, Error, TEXT("CleverTapAPI not found in JNI!"));
        return false;
    }
    jmethodID getInstanceMethod = env->GetStaticMethodID(
        cleverTapAPIClass,
        "getDefaultInstance",
        "(Landroid/content/Context;)Lcom/clevertap/android/sdk/CleverTapAPI;"
    );
    if (!getInstanceMethod)
    {
        UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTap getDefaultInstance method!"));
        return false;
    }

    jobject activity = FAndroidApplication::GetGameActivityThis();
    jobject cleverTapInstance = env->CallStaticObjectMethod(cleverTapAPIClass, getInstanceMethod, activity);
    if (cleverTapInstance)
    {
        UE_LOG(LogCleverTap, Log, TEXT("CleverTap Initialized Successfully!"));
        return true;

    }
    else
    {
        UE_LOG(LogCleverTap, Error, TEXT("CleverTap Instance is NULL!"));
        return false;
    }
}



FCleverTapInstance* FPlatformSDK::TryInitializeCommonInstance(const UCleverTapConfig& Config)
{
    if (InitCleverTap()) {
        return new FCleverTapInstance{};
    }

	return nullptr;
}

void FPlatformSDK::DestroyInstance(FCleverTapInstance& Inst)
{
    delete &Inst;
}

} } // namespace CleverTapSDK::Android

