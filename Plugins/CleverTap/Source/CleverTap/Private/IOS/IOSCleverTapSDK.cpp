// Copyright CleverTap All Rights Reserved.
#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapInstance.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapLog.h"
#include "CleverTapUtilities.h"

#import <CleverTapSDK/CleverTap.h>
#import <CleverTapSDK/CleverTapPushNotificationDelegate.h>
#import <CleverTapSDK/CleverTapURLDelegate.h>
#import <CleverTapSDK/CTLocalInApp.h>

namespace {
class FIOSCleverTapInstance;
}

// TODO: Not exposed
@interface CleverTapSDKListener :
	NSObject <CleverTapPushNotificationDelegate, CleverTapURLDelegate /*, CleverTapPushPermissionDelegate*/>
- (instancetype)initWithCppInstance:(FIOSCleverTapInstance*)Instance;
@end

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

NSString* ConvertToNSValue(const FText& Value)
{
	return ConvertToNSValue(Value.ToString());
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
	NSMutableArray* Result = [NSMutableArray arrayWithCapacity:Items.Num()];

	for (const FCleverTapProperties& Properties : Items)
	{
		[Result addObject:ConvertToNSDictionary(Properties)];
	}

	return Result;
}

FCleverTapPropertyValue ConvertFromNSNumber(NSNumber* Value)
{
	if (Value == (void*)kCFBooleanTrue || Value == (void*)kCFBooleanFalse)
	{
		return FCleverTapPropertyValue{ Value == (void*)kCFBooleanTrue ? true : false };
	}

	const char* const ObjCType = [Value objCType];
	switch (*ObjCType)
	{
		case 'c':
		{
			const char Underlying = [Value charValue];
			return FCleverTapPropertyValue{ static_cast<int32>(Underlying) };
		}

		case 'C':
		{
			const unsigned char Underlying = [Value unsignedCharValue];
			return FCleverTapPropertyValue{ static_cast<int32>(Underlying) };
		}

		case 's':
		{
			const short Underlying = [Value shortValue];
			return FCleverTapPropertyValue{ static_cast<int32>(Underlying) };
		}

		case 'S':
		{
			const unsigned short Underlying = [Value unsignedShortValue];
			return FCleverTapPropertyValue{ static_cast<int32>(Underlying) };
		}

		case 'i':
		{
			const int Underlying = [Value intValue];
			return FCleverTapPropertyValue{ static_cast<int32>(Underlying) };
		}

		case 'I':
		{
			const unsigned int Underlying = [Value unsignedIntValue];
			return FCleverTapPropertyValue{ static_cast<int64>(Underlying) };
		}

		case 'l':
		{
			const long Underlying = [Value longValue];
			return FCleverTapPropertyValue{ static_cast<int64>(Underlying) };
		}

		case 'L':
		{
			const unsigned long Underlying = [Value unsignedLongValue];
			return FCleverTapPropertyValue{ static_cast<int64>(Underlying) };
		}

		case 'q':
		{
			const long long Underlying = [Value longLongValue];
			return FCleverTapPropertyValue{ static_cast<int64>(Underlying) };
		}

		case 'Q':
		{
			const unsigned long long Underlying = [Value unsignedLongLongValue];
			return FCleverTapPropertyValue{ static_cast<int64>(Underlying) };
		}

		case 'f':
		{
			const float Underlying = [Value floatValue];
			return FCleverTapPropertyValue{ Underlying };
		}

		case 'd':
		{
			const double Underlying = [Value doubleValue];
			return FCleverTapPropertyValue{ Underlying };
		}

		default:
		{
			UE_LOG(LogCleverTap, Error, TEXT("Unhandled Objective-C type: %s"), *FString{ ObjCType });
		}
		break;
	}

	return FCleverTapPropertyValue{};
}

TArray<FString> ConvertFromNSArray(NSArray* Arr)
{
	TArray<FString> Result{};
	if (Arr == nil)
	{
		return Result;
	}

	const int NumElems = [Arr count];
	if (NumElems == 0)
	{
		return Result;
	}

	Result.Reserve(NumElems);

	for (id ElemValue in Arr)
	{
		Result.Emplace(FString{ [NSString stringWithFormat:@"%@", ElemValue] });
	}

	return Result;
}

FCleverTapProperties ConvertFromNSDictionary(NSDictionary* Dict)
{
	FCleverTapProperties Result;
	if (Dict == nil)
	{
		return Result;
	}

	const int32 NumElements = [Dict count];
	if (NumElements == 0)
	{
		return Result;
	}
	Result.Reserve(NumElements);

	for (NSString* Key in Dict)
	{
		id Value = Dict[Key];
		if (Value == nil || [Value isEqual:[NSNull null]])
		{
			continue;
		}

		FString KeyString{ Key };

		if ([Value isKindOfClass:[NSString class]])
		{
			Result.Add(MoveTemp(KeyString), FCleverTapPropertyValue{ FString{ (NSString*)Value } });
		}
		else if ([Value isKindOfClass:[NSNumber class]])
		{
			Result.Add(MoveTemp(KeyString), ConvertFromNSNumber((NSNumber*)Value));
		}
		else if ([Value isKindOfClass:[NSArray class]])
		{
			Result.Add(MoveTemp(KeyString), ConvertFromNSArray((NSArray*)Value));
		}
		else
		{
			UE_LOG(LogCleverTap, Warning, TEXT("Unhandled NSDictionary value type: %s"),
				*FString{ NSStringFromClass([Value class]) });
		}
	}

	return Result;
}

class FIOSCleverTapInstance : public ICleverTapInstance
{
	static constexpr uint8 CTSTATE_FLAGS_REGISTERED_FOR_PUSH = 0x1;
	static constexpr uint8 CTSTATE_FLAGS_REGISTERED_FOR_DEEP_LINK = 0x2;

	static constexpr int8 PUSH_PERM_STATUS_UNKNOWN = 0;
	static constexpr int8 PUSH_PERM_STATUS_GRANTED = 1;
	static constexpr int8 PUSH_PERM_STATUS_DENIED = 2;

public:
	explicit FIOSCleverTapInstance(CleverTap* InNativeInstance)
		: NativeInstance{ InNativeInstance }
		, SDKListener{ [[CleverTapSDKListener alloc] initWithCppInstance:this] }
		, PushPermissionStatus{ static_cast<uint8>(ECleverTapPushPermissionStatus::Unknown) }
	{
		if (NativeInstance != nil)
		{
			// TODO: Not exposed
			// [NativeInstance setPushPermissionDelegate:SDKListener];

			// TODO: Not exposed
			// [NativeInstance getNotificationPermissionStatusWithCompletionHandler:^(UNAuthorizationStatus Status) {
			//   const bool bIsGranted =
			// 	  (Status != UNAuthorizationStatusNotDetermined && Status != UNAuthorizationStatusDenied);
			//   CachePushPermissionStatus(bIsGranted);
			// }];
		}
	}

	~FIOSCleverTapInstance() { [SDKListener release]; }

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

	ECleverTapPushPermissionStatus GetPushPermissionStatus() override
	{
		const uint8 StatusValue = PushPermissionStatus.Load();
		return static_cast<ECleverTapPushPermissionStatus>(StatusValue);
	}

	void PromptForPushPermission(bool bFallbackToSettings) override
	{
		// TODO: Not exposed
		// [NativeInstance promptForPushPermission:ConvertToNSValue(bFallbackToSettings)];
		FPlatformMisc::RegisterForRemoteNotifications();
	}

	void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) override
	{
		CTLocalInApp* localInAppBuilder =
			[[CTLocalInApp alloc] initWithInAppType:ALERT
										  titleText:ConvertToNSValue(PushPrimerAlertConfig.TitleText)
										messageText:ConvertToNSValue(PushPrimerAlertConfig.MessageText)
							followDeviceOrientation:ConvertToNSValue(PushPrimerAlertConfig.bFollowDeviceOrientation)
									positiveBtnText:ConvertToNSValue(PushPrimerAlertConfig.PositiveButtonText)
									negativeBtnText:ConvertToNSValue(PushPrimerAlertConfig.NegativeButtonText)];

		[localInAppBuilder setFallbackToSettings:ConvertToNSValue(PushPrimerAlertConfig.bFallbackToSettings)];

		// TODO: Not exposed
		// [NativeInstance promptPushPrimer:localInAppBuilder.getLocalInAppSettings]
	}

	void PromptForPushPermission(
		const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig) override
	{
		CTLocalInApp* localInAppBuilder = [[CTLocalInApp alloc]
				  initWithInAppType:HALF_INTERSTITIAL
						  titleText:ConvertToNSValue(PushPrimerHalfInterstitialConfig.TitleText)
						messageText:ConvertToNSValue(PushPrimerHalfInterstitialConfig.MessageText)
			followDeviceOrientation:ConvertToNSValue(PushPrimerHalfInterstitialConfig.bFollowDeviceOrientation)
					positiveBtnText:ConvertToNSValue(PushPrimerHalfInterstitialConfig.PositiveButtonText)
					negativeBtnText:ConvertToNSValue(PushPrimerHalfInterstitialConfig.NegativeButtonText)];

		if (!PushPrimerHalfInterstitialConfig.ImageURL.IsEmpty())
		{
			[localInAppBuilder setImageUrl:ConvertToNSValue(PushPrimerHalfInterstitialConfig.ImageURL)];
		}

		[localInAppBuilder setBtnBorderRadius:ConvertToNSValue(PushPrimerHalfInterstitialConfig.ButtonBorderRadius)];
		[localInAppBuilder
			setFallbackToSettings:ConvertToNSValue(PushPrimerHalfInterstitialConfig.bFallbackToSettings)];

		using CleverTapSDK::ColorToHexString;

		[localInAppBuilder
			setBackgroundColor:ConvertToNSValue(ColorToHexString(PushPrimerHalfInterstitialConfig.BackgroundColor))];
		[localInAppBuilder
			setBtnBorderColor:ConvertToNSValue(ColorToHexString(PushPrimerHalfInterstitialConfig.ButtonBorderColor))];
		[localInAppBuilder
			setTitleTextColor:ConvertToNSValue(ColorToHexString(PushPrimerHalfInterstitialConfig.TitleTextColor))];
		[localInAppBuilder
			setMessageTextColor:ConvertToNSValue(ColorToHexString(PushPrimerHalfInterstitialConfig.MessageTextColor))];
		[localInAppBuilder
			setBtnTextColor:ConvertToNSValue(ColorToHexString(PushPrimerHalfInterstitialConfig.ButtonTextColor))];
		[localInAppBuilder setBtnBackgroundColor:ConvertToNSValue(ColorToHexString(
													 PushPrimerHalfInterstitialConfig.ButtonBackgroundColor))];

		// TODO: Not exposed
		// [NativeInstance promptPushPrimer:localInAppBuilder.getLocalInAppSettings]
	}

	void EnableOnPushNotificationClicked() override
	{
		if (IsRegisteredForPushNotificationClicked())
		{
			return;
		}
		SetIsRegisteredForPushNotificationClicked();

		[NativeInstance setPushNotificationDelegate:SDKListener];
	}

	bool LocalizeAndroidNotificationChannel(
		const FString& ChannelID, const FText& ChannelName, const FText& ChannelDescription) override
	{
		CleverTapSDK::Ignore(ChannelID, ChannelName, ChannelDescription);
		return false;
	}

	void RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> InUrlHandler) override
	{
		if (!IsRegisteredForDeepLinkHandler())
		{
			[NativeInstance setUrlDelegate:SDKListener];
			SetIsRegisteredForDeepLinkHandler();
		}

		UrlHandler = MoveTemp(InUrlHandler);
	}
	// </ICleverTapInstance>

	bool IsRegisteredForPushNotificationClicked() const
	{
		return (StateFlags & CTSTATE_FLAGS_REGISTERED_FOR_PUSH) != 0;
	}

	void SetIsRegisteredForPushNotificationClicked() { StateFlags |= CTSTATE_FLAGS_REGISTERED_FOR_PUSH; }

	bool IsRegisteredForDeepLinkHandler() const { return (StateFlags & CTSTATE_FLAGS_REGISTERED_FOR_DEEP_LINK) != 0; }

	void SetIsRegisteredForDeepLinkHandler() { StateFlags |= CTSTATE_FLAGS_REGISTERED_FOR_DEEP_LINK; }

	void CachePushPermissionStatus(bool bIsGranted)
	{
		const ECleverTapPushPermissionStatus Status =
			bIsGranted ? ECleverTapPushPermissionStatus::Granted : ECleverTapPushPermissionStatus::NotGranted;
		PushPermissionStatus.Store(static_cast<uint8>(Status));
	}

	void SetPushToken(const TArray<uint8>& Token)
	{
		[NativeInstance setPushToken:[NSData dataWithBytes:Token.GetData() length:Token.Num()]];
	}

	bool HandleUrl(FString Url, ECleverTapChannel Channel) const
	{
		if (UrlHandler)
		{
			return UrlHandler(MoveTemp(Url), Channel);
		}

		return true;
	}

private:
	CleverTap* NativeInstance{};
	CleverTapSDKListener* SDKListener{};
	TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler;
	TAtomic<uint8> PushPermissionStatus;
	uint8 StateFlags{};
};

} // namespace

@implementation CleverTapSDKListener
{
	FIOSCleverTapInstance* CppInstance;
}

- (instancetype)initWithCppInstance:(FIOSCleverTapInstance*)Instance
{
	CppInstance = Instance;
	return self;
}

- (void)onPushPermissionResponse:(BOOL)Accepted
{
	const bool bIsGranted = Accepted ? true : false;
	AsyncTask(ENamedThreads::GameThread, [bIsGranted, Inst = CppInstance]() {
		Inst->CachePushPermissionStatus(bIsGranted);
		Inst->OnPushPermissionResponse.Broadcast(bIsGranted);
	});
}

- (BOOL)shouldHandleCleverTapURL:(NSURL*)Url forChannel:(CleverTapChannel)Channel
{
	const ECleverTapChannel Ch = [Channel] {
		switch (Channel)
		{
			case CleverTapPushNotification:
				return ECleverTapChannel::PushNotification;
			case CleverTapAppInbox:
				return ECleverTapChannel::AppInbox;
			case CleverTapInAppNotification:
				return ECleverTapChannel::InAppNotification;
			default:
			{
				UE_LOG(LogCleverTap, Error, TEXT("Unhandled CleverTapChannel value: %d"), static_cast<int32>(Channel));
				return ECleverTapChannel{};
			}
		}
	}();

	return CppInstance->HandleUrl(FString{ [Url absoluteString] }, Ch) ? YES : NO;
}

- (void)pushNotificationTappedWithCustomExtras:(NSDictionary*)CustomExtras
{
	FCleverTapProperties Extras = ConvertFromNSDictionary(CustomExtras);
	AsyncTask(ENamedThreads::GameThread,
		[Extras = MoveTemp(Extras), Inst = CppInstance]() { Inst->OnPushNotificationClicked.Broadcast(Extras); });
}

@end

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

	CleverTap* const SharedInst = [CleverTap autoIntegrate];
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

	CleverTap* const SharedInst = [CleverTap autoIntegrateWithCleverTapID:CleverTapId.GetNSString()];
	return MakeUnique<FIOSCleverTapInstance>(SharedInst);
}

void FPlatformSDK::SetRemoteNotificationToken(ICleverTapInstance& Instance, const TArray<uint8>& Token)
{
	auto& IOSInstance = static_cast<FIOSCleverTapInstance&>(Instance);
	IOSInstance.SetPushToken(Token);
}

}} // namespace CleverTapSDK::IOS
