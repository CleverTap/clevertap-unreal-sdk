// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapSDK.h"

#include "Android/AndroidCleverTapInstance.h"
#include "Android/AndroidCleverTapJNI.h"
#include "Android/AndroidJNIUtilities.h"

#include "CleverTapInstance.h"
#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android {

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	UE_LOG(LogCleverTap, Log, TEXT("SetLogLevel(%hs)"), JNI::CleverTapLogLevelJavaName(Level));
	JNI::SetDebugLevel(JNI::GetJNIEnv(), Level);
}

UCleverTapInstance* FPlatformSDK::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	JNIEnv* Env = JNI::GetJNIEnv();
	JNI::SetDefaultConfig(Env, Config);
	jobject Instance = JNI::GetDefaultInstance(Env);
	if (!Env || !Instance)
	{
		return nullptr;
	}
	return UAndroidCleverTapInstance::Create(Env, Instance);
}

UCleverTapInstance* FPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	JNIEnv* Env = JNI::GetJNIEnv();
	JNI::SetDefaultConfig(Env, Config);
	jobject Instance = JNI::GetDefaultInstance(Env, CleverTapId);
	if (!Env || !Instance)
	{
		return nullptr;
	}
	return UAndroidCleverTapInstance::Create(Env, Instance);
}

}} // namespace CleverTapSDK::Android
