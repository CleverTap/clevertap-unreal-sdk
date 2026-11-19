// Copyright CleverTap All Rights Reserved.
#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapLogLevel.h"
#include "CleverTapLog.h"
#include "CleverTapSdkVersion.h"
#include "IOS/IOSCleverTapInstance.h"

#import <CleverTapSDK/CleverTap.h>

namespace CleverTapSDK { namespace IOS {

static void RegisterUnrealSdk(CleverTap* Instance)
{
	// Name and version go into the "af" header of every request Core sends. Core keeps them in memory only,
	// so this runs on every launch, right after the instance is obtained.
	if (!Instance)
	{
		return;
	}
	NSString* const LibraryName = @CLEVERTAP_UNREAL_SDK_NAME;
	[Instance setLibrary:LibraryName];
	if (CLEVERTAP_UNREAL_SDK_VERSION > 0)
	{
		[Instance setCustomSdkVersion:LibraryName version:CLEVERTAP_UNREAL_SDK_VERSION];
	}
}

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	int const ObjCLevel = [Level] {
		switch (Level)
		{
			case ECleverTapLogLevel::Off:
				return CleverTapLogOff;
			case ECleverTapLogLevel::Info:
				return CleverTapLogInfo;
			case ECleverTapLogLevel::Debug:
				return CleverTapLogDebug;
			case ECleverTapLogLevel::Verbose:
				return CleverTapLogDebug; // CleverTap IOS doesn't have LogLevel Verbose
			default:
			{
				UE_LOG(LogCleverTap, Error,
					TEXT("Unhandled ECleverTapLogLevel value. Defaulting to ECleverTapLogLevel::Off"));
				return CleverTapLogOff;
			}
			break;
		}
	}();

	[CleverTap setDebugLevel:ObjCLevel];
}

UCleverTapInstance* FPlatformSDK::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	NSString* AccountId = Config.ProjectId.GetNSString();
	NSString* Token = Config.ProjectToken.GetNSString();
	NSString* Region = Config.RegionCode.GetNSString();
	[CleverTap setCredentialsWithAccountID:AccountId token:Token region:Region];

	[CleverTap enablePersonalization];
	CleverTap* const SharedInst = [CleverTap sharedInstance];
	RegisterUnrealSdk(SharedInst);
	return UIOSCleverTapInstance::CreateFromNativeInstance(SharedInst);
}

UCleverTapInstance* FPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	NSString* AccountId = Config.ProjectId.GetNSString();
	NSString* Token = Config.ProjectToken.GetNSString();
	NSString* Region = Config.RegionCode.GetNSString();
	[CleverTap setCredentialsWithAccountID:AccountId token:Token region:Region];

	[CleverTap enablePersonalization];
	CleverTap* const SharedInst = [CleverTap sharedInstanceWithCleverTapID:CleverTapId.GetNSString()];
	RegisterUnrealSdk(SharedInst);
	return UIOSCleverTapInstance::CreateFromNativeInstance(SharedInst);
}

void FPlatformSDK::SetRemoteNotificationToken(UCleverTapInstance& Instance, const TArray<uint8>& Token)
{
	auto& IOSInstance = static_cast<UIOSCleverTapInstance&>(Instance);
	IOSInstance.SetPushToken(Token);
}

}} // namespace CleverTapSDK::IOS
