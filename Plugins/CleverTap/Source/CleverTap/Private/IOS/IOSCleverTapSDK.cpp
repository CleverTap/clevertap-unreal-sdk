// Copyright CleverTap All Rights Reserved.
#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapLogLevel.h"
#include "CleverTapLog.h"
#include "IOS/IOSCleverTapInstance.h"

#import <CleverTapSDK/CleverTap.h>

namespace CleverTapSDK { namespace IOS {

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
	return UIOSCleverTapInstance::CreateFromNativeInstance(SharedInst);
}

void FPlatformSDK::SetRemoteNotificationToken(UCleverTapInstance& Instance, const TArray<uint8>& Token)
{
	auto& IOSInstance = static_cast<UIOSCleverTapInstance&>(Instance);
	IOSInstance.SetPushToken(Token);
}

}} // namespace CleverTapSDK::IOS
