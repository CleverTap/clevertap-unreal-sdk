// Copyright CleverTap All Rights Reserved.
#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapInstance.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapLog.h"

#import <CleverTapSDK/CleverTap.h>

namespace {

NSDictionary* ConvertToNSDictionary(const FCleverTapProperties& Properties)
{
	NSMutableDictionary* result = [[NSMutableDictionary alloc] init];

	for (const FCleverTapProperties::ElementType& Entry : Properties)
	{
		NSString* Key = Entry.Key.GetNSString();
		switch (Entry.Value.GetIndex())
		{
			// int64, float, double, bool, FString, FCleverTapDate, TArray<FString>
			case FCleverTapPropertyValue::IndexOfType<int64>():
			{
				result[Key] = @(Entry.Value.Get<int64>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<float>():
			{
				result[Key] = @(Entry.Value.Get<float>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<double>():
			{
				result[Key] = @(Entry.Value.Get<double>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<bool>():
			{
				result[Key] = (Entry.Value.Get<bool>() ? @YES : @NO);
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<FString>():
			{
				result[Key] = Entry.Value.Get<FString>().GetNSString();
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<FCleverTapDate>():
			{
				const FCleverTapDate& Date = Entry.Value.Get<FCleverTapDate>();

				NSDateComponents* ObjCDate = [[NSDateComponents alloc] init];
				ObjCDate.day = Date.Day;
				ObjCDate.month = Date.Month;
				ObjCDate.year = Date.Year;
				result[Key] = [[NSCalendar currentCalendar] dateFromComponents:ObjCDate];
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<TArray<FString>>():
			{
				const TArray<FString>& Values = Entry.Value.Get<TArray<FString>>();
				NSMutableArray* ObjCValues = [NSMutableArray arrayWithCapacity:Values.Num()];

				for (const FString& StrValue : Values)
				{
					[ObjCValues addObject:StrValue.GetNSString()];
				}

				result[Key] = ObjCValues;
			}
			break;
		}
	}

	return result;
}

NSArray* ConvertToNSArray(const TArray<FCleverTapProperties>& Items)
{
	NSMutableArray* result = [NSMutableArray arrayWithCapacity:Items.Num()];

	for (const FCleverTapProperties& Properties : Items)
	{
		[result addObject:ConvertToNSDictionary(Properties)];
	}

	return result;
}

class FIOSCleverTapInstance : public ICleverTapInstance
{
public:
	explicit FIOSCleverTapInstance(CleverTap* InNativeInstance) : NativeInstance{ InNativeInstance } {}

	// <ICleverTapInstance>
	FString GetCleverTapId() override { return FString{ [NativeInstance profileGetCleverTapID] }; }

	void OnUserLogin(const FCleverTapProperties& Profile) override
	{
		[NativeInstance onUserLogin:ConvertToNSDictionary(Profile)];
	}

	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) override
	{
		[NativeInstance onUserLogin:ConvertToNSDictionary(Profile) withCleverTapID:CleverTapId.GetNSString()];
	}

	void PushProfile(const FCleverTapProperties& Profile) override
	{
		[NativeInstance profilePush:ConvertToNSDictionary(Profile)];
	}

	void PushEvent(const FString& EventName) override { [NativeInstance recordEvent:EventName.GetNSString()]; }

	void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) override
	{
		[NativeInstance recordEvent:EventName.GetNSString() withProps:ConvertToNSDictionary(Actions)];
	}

	void PushChargedEvent(const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) override
	{
		[NativeInstance recordChargedEventWithDetails:ConvertToNSDictionary(ChargeDetails)
											 andItems:ConvertToNSArray(Items)];
	}

	void DecrementValue(const FString& Key, int Amount) override
	{
		[NativeInstance profileDecrementValueBy:[NSNumber numberWithInt:Amount] forKey:Key.GetNSString()];
	}

	void DecrementValue(const FString& Key, double Amount) override
	{
		[NativeInstance profileDecrementValueBy:[NSNumber numberWithDouble:Amount] forKey:Key.GetNSString()];
	}

	void IncrementValue(const FString& Key, int Amount) override
	{
		[NativeInstance profileIncrementValueBy:[NSNumber numberWithInt:Amount] forKey:Key.GetNSString()];
	}

	void IncrementValue(const FString& Key, double Amount) override
	{
		[NativeInstance profileIncrementValueBy:[NSNumber numberWithDouble:Amount] forKey:Key.GetNSString()];
	}
	// </ICleverTapInstance>

private:
	CleverTap* NativeInstance{};
};

} // namespace

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

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	NSString* AccountId = Config.ProjectId.GetNSString();
	NSString* Token = Config.ProjectToken.GetNSString();
	NSString* Region = Config.RegionCode.GetNSString();
	[CleverTap setCredentialsWithAccountID:AccountId token:Token region:Region];

	CleverTap* const SharedInst = [CleverTap sharedInstance];
	return MakeUnique<FIOSCleverTapInstance>(SharedInst);
}

TUniquePtr<ICleverTapInstance> FPlatformSDK::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	FPlatformSDK::SetLogLevel(Config.LogLevel);

	NSString* AccountId = Config.ProjectId.GetNSString();
	NSString* Token = Config.ProjectToken.GetNSString();
	NSString* Region = Config.RegionCode.GetNSString();
	[CleverTap setCredentialsWithAccountID:AccountId token:Token region:Region];

	CleverTap* const SharedInst = [CleverTap sharedInstanceWithCleverTapID:CleverTapId.GetNSString()];
	return MakeUnique<FIOSCleverTapInstance>(SharedInst);
}

}} // namespace CleverTapSDK::IOS
