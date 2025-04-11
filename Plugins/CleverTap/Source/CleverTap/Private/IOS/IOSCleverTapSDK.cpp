// Copyright CleverTap All Rights Reserved.
#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapInstance.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapLog.h"

#import <CleverTapSDK/CleverTap.h>

namespace {

template <typename T>
auto ConvertToNSValue(T Value)
{
	return @(Value);
}

auto ConvertToNSValue(bool Value)
{
	return Value ? @YES : @NO;
}

NSString* ConvertToNSValue(const FString& Value)
{
	return Value.GetNSString();
}

template <typename T>
NSArray* ConvertToNSArray(const TArray<T>& Values)
{
	NSMutableArray* ObjCValues = [NSMutableArray arrayWithCapacity:Values.Num()];

	for (const T& Value : Values)
	{
		[ObjCValues addObject:ConvertToNSValue(Value)];
	}

	return ObjCValues;
}

NSDictionary* ConvertToNSDictionary(const FCleverTapProperties& Properties)
{
	NSMutableDictionary* result = [[NSMutableDictionary alloc] init];

	for (const FCleverTapProperties::ElementType& Entry : Properties)
	{
		NSString* Key = Entry.Key.GetNSString();
		switch (Entry.Value.GetIndex())
		{
			// int32, int64, float, double, bool, FString, FCleverTapDate, TArray<FString>
			case FCleverTapPropertyValue::IndexOfType<int32>():
			{
				result[Key] = ConvertToNSValue(Entry.Value.Get<int32>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<int64>():
			{
				result[Key] = ConvertToNSValue(Entry.Value.Get<int64>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<float>():
			{
				result[Key] = ConvertToNSValue(Entry.Value.Get<float>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<double>():
			{
				result[Key] = ConvertToNSValue(Entry.Value.Get<double>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<bool>():
			{
				result[Key] = ConvertToNSValue(Entry.Value.Get<bool>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<FString>():
			{
				result[Key] = ConvertToNSValue(Entry.Value.Get<FString>());
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

			case FCleverTapPropertyValue::IndexOfType<TArray<int32>>():
			{
				result[Key] = ConvertToNSArray(Entry.Value.Get<TArray<int32>>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<TArray<int64>>():
			{
				result[Key] = ConvertToNSArray(Entry.Value.Get<TArray<int64>>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<TArray<float>>():
			{
				result[Key] = ConvertToNSArray(Entry.Value.Get<TArray<float>>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<TArray<double>>():
			{
				result[Key] = ConvertToNSArray(Entry.Value.Get<TArray<double>>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<TArray<bool>>():
			{
				result[Key] = ConvertToNSArray(Entry.Value.Get<TArray<bool>>());
			}
			break;

			case FCleverTapPropertyValue::IndexOfType<TArray<FString>>():
			{
				result[Key] = ConvertToNSArray(Entry.Value.Get<TArray<FString>>());
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
