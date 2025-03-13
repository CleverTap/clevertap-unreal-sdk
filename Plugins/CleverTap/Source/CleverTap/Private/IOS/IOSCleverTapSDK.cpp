// Copyright CleverTap All Rights Reserved.
#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapInstance.h"
#include "CleverTapLog.h"

#import <CleverTapSDK/CleverTap.h>

namespace {

class FIOSCleverTapInstance : public ICleverTapInstance
{
public:
	explicit FIOSCleverTapInstance(CleverTap* InNativeInstance)
		: NativeInstance{ InNativeInstance }
	{
	}

	FString GetCleverTapId() const override
	{
		return FString{ [NativeInstance profileGetCleverTapID] };
	}

private:
	CleverTap* NativeInstance{};
};

} // namespace (anonymous)

namespace CleverTapSDK { namespace IOS {

void FPlatformSDK::SetLogLevel(ECleverTapLogLevel Level)
{
	int const ObjCLevel = [Level] {
		switch (Level)
		{
			case ECleverTapLogLevel::Off:     return CleverTapLogOff;
			case ECleverTapLogLevel::Info:    return CleverTapLogInfo;
			case ECleverTapLogLevel::Debug:   return CleverTapLogDebug;
			case ECleverTapLogLevel::Verbose: return CleverTapLogDebug;
			default: {
				UE_LOG(LogCleverTap, Error,
					TEXT("Unhandled ECleverTapLogLevel value. Defaulting to ECleverTapLogLevel::Off")
				);
				return CleverTapLogOff;
			} break;
		}
	}();

	[CleverTap setDebugLevel:ObjCLevel];
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const UCleverTapConfig& Config
)
{
	CleverTap* const SharedInst = [CleverTap autoIntegrate];
	FPlatformSDK::SetLogLevel(Config.GetActiveLogLevel());
	return MakeUnique<FIOSCleverTapInstance>(SharedInst);
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const UCleverTapConfig& Config, const FString& CleverTapId
)
{
	CleverTap* const SharedInst = [CleverTap autoIntegrateWithCleverTapID: CleverTapId.GetNSString()];
	FPlatformSDK::SetLogLevel(Config.GetActiveLogLevel());
	return MakeUnique<FIOSCleverTapInstance>(SharedInst);
}

} } // namespace CleverTapSDK::IOS

