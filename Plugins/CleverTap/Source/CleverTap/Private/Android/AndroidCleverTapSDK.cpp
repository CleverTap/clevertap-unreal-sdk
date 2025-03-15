#include "Android/AndroidCleverTapSDK.h"
#include "CleverTapPlatformSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapLog.h"

#include "Android/AndroidApplication.h"


namespace CleverTapSDK { namespace Android {

static bool InitCleverTap() {
    UE_LOG(LogCleverTap, Error, TEXT("CleverTapSDK::Android::::InitCleverTap()"));

    JNIEnv* env = FAndroidApplication::GetJavaEnv();
    if (env == nullptr) {
        UE_LOG(LogCleverTap, Error, TEXT("FAndroidApplication::GetJavaEnv() returned nullptr"));
        return false;
    }

    jobject activity = FAndroidApplication::GetGameActivityThis();
    jclass activityClass = env->GetObjectClass(activity);
    if (!activityClass) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't get GameActivity class"));
        return false;
    }
    jmethodID getClassLoader = env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    if (!getClassLoader) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't get getClassLoader method from GameActivity"));
        return false;
    }
    jobject classLoader = env->CallObjectMethod(activity, getClassLoader);
    if (!classLoader) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't get ClassLoader from GameActivity"));
        return false;
    }
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    if (!classLoaderClass) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't find ClassLoader class"));
        return false;
    }
    jmethodID loadClass = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    if (!loadClass) {
        UE_LOG(LogCleverTap, Error, TEXT("Couldn't find ClassLoader loadClass method"));
        return false;
    }

    jstring className = env->NewStringUTF("com.clevertap.android.sdk.CleverTapAPI");
    jclass cleverTapAPIClass = (jclass)env->CallObjectMethod(classLoader, loadClass, className);


//    jclass cleverTapAPIClass = env->FindClass("com/clevertap/android/sdk/CleverTapAPI");
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

