// Copyright CleverTap All Rights Reserved.
#include "IOS/IOSCleverTapSDK.h"

#include "CleverTapConfig.h"
#include "CleverTapInstance.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapLog.h"
#include "CleverTapUtilities.h"
#include "Misc/CoreDelegates.h"

#import <CleverTapSDK/CleverTap.h>
#import <CleverTapSDK/CleverTapInAppNotificationDelegate.h>
#import <CleverTapSDK/CleverTapPushNotificationDelegate.h>
#import <CleverTapSDK/CleverTapURLDelegate.h>
#import <CleverTapSDK/CTLocalInApp.h>
#import <objc/runtime.h>
#import <UserNotifications/UserNotifications.h>

namespace {
class FIOSCleverTapInstance;

void EnsurePushNotificationMonitoring();

} // namespace

// TODO: Not exposed
@interface CleverTapSDKListener :
	NSObject <CleverTapInAppNotificationDelegate, CleverTapPushNotificationDelegate,
		CleverTapURLDelegate /*, CleverTapPushPermissionDelegate*/>
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

bool TryConvertFromNSValue(id Value, FCleverTapPropertyValue* OutValue)
{
	check(OutValue != nullptr);

	if (Value == nil)
	{
		*OutValue = FCleverTapPropertyValue{};
		return false;
	}

	if ([Value isKindOfClass:[NSString class]])
	{
		OutValue->Emplace<FString>((NSString*)Value);
		return true;
	}

	if ([Value isKindOfClass:[NSNumber class]])
	{
		*OutValue = ConvertFromNSNumber((NSNumber*)Value);
		return true;
	}

	if ([Value isKindOfClass:[NSArray class]])
	{
		*OutValue = ConvertFromNSArray((NSArray*)Value);
		return true;
	}

	*OutValue = FCleverTapPropertyValue{};
	return false;
}

bool TryFlattenNSDictionary(NSString* KeyPrefix, NSDictionary* Dict, FCleverTapProperties& ResultRef)
{
	check(Dict != nil);

	const int32 NumElements = [Dict count];
	if (NumElements == 0)
	{
		return true;
	}
	ResultRef.Reserve(ResultRef.Num() + NumElements);

	bool bAllSuccessful = true;
	for (NSString* KeyPart in Dict)
	{
		id Value = Dict[KeyPart];
		if (Value == nil || [Value isEqual:[NSNull null]])
		{
			continue;
		}

		NSString* NewPrefix = (KeyPrefix == nil) ? KeyPart : [KeyPrefix stringByAppendingFormat:@".%@", KeyPart];

		FCleverTapPropertyValue MaybeUnrealValue;
		if (TryConvertFromNSValue(Value, &MaybeUnrealValue))
		{
			ResultRef.Add(FString{ NewPrefix }, MoveTemp(MaybeUnrealValue));
		}
		else if ([Value isKindOfClass:[NSDictionary class]])
		{
			if (!TryFlattenNSDictionary(NewPrefix, (NSDictionary*)Value, ResultRef))
			{
				bAllSuccessful = false;
			}
		}
		else
		{
			UE_LOG(LogCleverTap, Warning, TEXT("Unhandled NSDictionary entry for key '%s.%s': %s"),
				*FString{ NewPrefix }, *FString{ NSStringFromClass([Value class]) });
			bAllSuccessful = false;
		}
	}

	return bAllSuccessful;
}

FCleverTapProperties ConvertFromNSDictionary(NSDictionary* Dict)
{
	FCleverTapProperties Result;
	if (Dict == nil)
	{
		return Result;
	}

	TryFlattenNSDictionary(nil, Dict, Result);
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
	static void HandleWillPresentNotification(NSDictionary* UserInfo)
	{
		for (FIOSCleverTapInstance* Inst : AllInstances)
		{
			[Inst->NativeInstance handleNotificationWithData:UserInfo openDeepLinksInForeground:YES];
		}
	}

	static void HandleDidReceiveNotificationResponse(NSDictionary* UserInfo)
	{
		for (FIOSCleverTapInstance* Inst : AllInstances)
		{
			[Inst->NativeInstance handleNotificationWithData:UserInfo];
		}
	}

	explicit FIOSCleverTapInstance(CleverTap* InNativeInstance)
		: NativeInstance{ InNativeInstance }
		, SDKListener{ [[CleverTapSDKListener alloc] initWithCppInstance:this] }
		, PushPermissionStatus{ static_cast<uint8>(ECleverTapPushPermissionStatus::Unknown) }
	{
		AllInstances.Add(this);

		if (NativeInstance != nil)
		{
			[NativeInstance setInAppNotificationDelegate:SDKListener];

			// TODO: Not exposed
			// [NativeInstance setPushPermissionDelegate:SDKListener];

			// TODO: Not exposed
			// [NativeInstance getNotificationPermissionStatusWithCompletionHandler:^(UNAuthorizationStatus Status) {
			//   const bool bIsGranted =
			// 	  (Status != UNAuthorizationStatusNotDetermined && Status != UNAuthorizationStatusDenied);
			//   CachePushPermissionStatus(bIsGranted);
			// }];
		}

		EnsurePushNotificationMonitoring();
	}

	~FIOSCleverTapInstance()
	{
		[SDKListener release];
		AllInstances.RemoveSingleSwap(this);
	}

	// <ICleverTapInstance>
	FString GetCleverTapId() override
	{
		check(NativeInstance != nil);
		return FString{ [NativeInstance profileGetCleverTapID] };
	}

	void OnUserLogin(const FCleverTapProperties& Profile) override
	{
		check(NativeInstance != nil);
		[NativeInstance onUserLogin:ConvertToNSDictionary(Profile)];
	}

	void OnUserLogin(const FCleverTapProperties& Profile, const FString& CleverTapId) override
	{
		check(NativeInstance != nil);
		[NativeInstance onUserLogin:ConvertToNSDictionary(Profile) withCleverTapID:CleverTapId.GetNSString()];
	}

	void PushProfile(const FCleverTapProperties& Profile) override
	{
		check(NativeInstance != nil);
		[NativeInstance profilePush:ConvertToNSDictionary(Profile)];
	}

	void PushEvent(const FString& EventName) override
	{
		check(NativeInstance != nil);
		[NativeInstance recordEvent:EventName.GetNSString()];
	}

	void PushEvent(const FString& EventName, const FCleverTapProperties& Actions) override
	{
		check(NativeInstance != nil);
		[NativeInstance recordEvent:EventName.GetNSString() withProps:ConvertToNSDictionary(Actions)];
	}

	void PushChargedEvent(const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items) override
	{
		check(NativeInstance != nil);
		[NativeInstance recordChargedEventWithDetails:ConvertToNSDictionary(ChargeDetails)
											 andItems:ConvertToNSArray(Items)];
	}

	TOptional<FCleverTapPropertyValue> GetProperty(const FString& Key) override
	{
		check(NativeInstance != nil);

		id ProfileValue = [NativeInstance profileGet:Key.GetNSString()];

		FCleverTapPropertyValue MaybeValue;
		if (!TryConvertFromNSValue(ProfileValue, &MaybeValue))
		{
			UE_CLOG(ProfileValue != nil, LogCleverTap, Warning, TEXT("Unknown property value type: %s"),
				*FString{ NSStringFromClass([ProfileValue class]) });
			return TOptional<FCleverTapPropertyValue>{};
		}

		return TOptional<FCleverTapPropertyValue>{ MoveTemp(MaybeValue) };
	}

	void DecrementValue(const FString& Key, int Amount) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileDecrementValueBy:[NSNumber numberWithInt:Amount] forKey:Key.GetNSString()];
	}

	void DecrementValue(const FString& Key, double Amount) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileDecrementValueBy:[NSNumber numberWithDouble:Amount] forKey:Key.GetNSString()];
	}

	void IncrementValue(const FString& Key, int Amount) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileIncrementValueBy:[NSNumber numberWithInt:Amount] forKey:Key.GetNSString()];
	}

	void IncrementValue(const FString& Key, double Amount) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileIncrementValueBy:[NSNumber numberWithDouble:Amount] forKey:Key.GetNSString()];
	}

	void AddMultiValueForKey(const FString& Key, const FString& Value) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileAddMultiValue:Value.GetNSString() forKey:Key.GetNSString()];
	}

	void AddMultiValuesForKey(const FString& Key, const TArray<FString>& Values) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileAddMultiValues:ConvertToNSArray(Values) forKey:Key.GetNSString()];
	}

	void RemoveMultiValueForKey(const FString& Key, const FString& Value) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileRemoveMultiValue:Value.GetNSString() forKey:Key.GetNSString()];
	}

	void RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileRemoveMultiValues:ConvertToNSArray(Values) forKey:Key.GetNSString()];
	}

	void RemoveValueForKey(const FString& Key) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileRemoveValueForKey:Key.GetNSString()];
	}

	void SetMultiValuesForKey(const FString& Key, const TArray<FString> Values) override
	{
		check(NativeInstance != nil);
		[NativeInstance profileSetMultiValues:ConvertToNSArray(Values) forKey:Key.GetNSString()];
	}

	ECleverTapPushPermissionStatus GetPushPermissionStatus() override
	{
		const uint8 StatusValue = PushPermissionStatus.Load();
		return static_cast<ECleverTapPushPermissionStatus>(StatusValue);
	}

	void PromptForPushPermission(bool bFallbackToSettings) override
	{
		check(NativeInstance != nil);
		// TODO: Not exposed
		// [NativeInstance promptForPushPermission:ConvertToNSValue(bFallbackToSettings)];
		FPlatformMisc::RegisterForRemoteNotifications();
	}

	void PromptForPushPermission(const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig) override
	{
		check(NativeInstance != nil);

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
		check(NativeInstance != nil);

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
		check(NativeInstance != nil);

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

	bool LocalizeAndroidNotificationChannelGroup(const FString& GroupID, const FText& GroupName) override
	{
		CleverTapSDK::Ignore(GroupID, GroupName);
		return false;
	}

	void RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> InUrlHandler) override
	{
		check(NativeInstance != nil);

		if (!IsRegisteredForDeepLinkHandler())
		{
			[NativeInstance setUrlDelegate:SDKListener];
			SetIsRegisteredForDeepLinkHandler();
		}

		UrlHandler = MoveTemp(InUrlHandler);
	}

	void RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter) override
	{
		InAppNotificationFilter = MoveTemp(Filter);
	}

	void SetOffline(bool bIsOffline) override
	{
		check(NativeInstance != nil);
		[NativeInstance setOffline:bIsOffline ? YES : NO];
	}

	void SetOptOut(bool bIsOptingOut) override
	{
		check(NativeInstance != nil);
		[NativeInstance setOptOut:bIsOptingOut ? YES : NO];
	}

	void SetNetworkInformationRecording(bool bEnableCollection) override
	{
		check(NativeInstance != nil);
		[NativeInstance enableDeviceNetworkInfoReporting:bEnableCollection ? YES : NO];
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
		check(NativeInstance != nil);

		[NativeInstance setPushToken:[NSData dataWithBytes:Token.GetData() length:Token.Num()]];
	}

	void HandlePushNotificationTapped(const FCleverTapProperties& Extras)
	{
		// Broadcast the tap
		this->OnPushNotificationClicked.Broadcast(Extras);
	}

	bool HandleUrl(FString Url, ECleverTapChannel Channel) const
	{
		if (UrlHandler)
		{
			return UrlHandler(MoveTemp(Url), Channel);
		}

		return true;
	}

	bool ShouldShowInAppNotification(const FCleverTapProperties& Extras) const
	{
		if (InAppNotificationFilter)
		{
			return InAppNotificationFilter(Extras);
		}

		return true;
	}

	void HandleInAppNotificationDismissed(const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras)
	{
		OnInAppNotificationDismissed.Broadcast(Extras, ActionExtras);
	}

	void HandleInAppNotificationShown(const FCleverTapProperties& Notification)
	{
		OnInAppNotificationShown.Broadcast(Notification);
	}

private:
	CleverTap* NativeInstance{};
	CleverTapSDKListener* SDKListener{};
	TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler;
	TUniqueFunction<bool(const FCleverTapProperties&)> InAppNotificationFilter;
	TAtomic<uint8> PushPermissionStatus;
	uint8 StateFlags{};

	static TArray<FIOSCleverTapInstance*> AllInstances;
};

TArray<FIOSCleverTapInstance*> FIOSCleverTapInstance::AllInstances{};

void EnsurePushNotificationMonitoring()
{
#if !PLATFORM_TVOS
	static TAtomic<bool> bHasBeenInitialized{ false };
	if (bHasBeenInitialized.Load())
	{
		return;
	}

	bool bExpectFalse{ false };
	if (!bHasBeenInitialized.CompareExchange(bExpectFalse, true))
	{
		return;
	}

	UIApplication* SharedApp = [UIApplication sharedApplication];
	if (SharedApp == nil)
	{
		UE_LOG(LogCleverTap, Fatal, TEXT("Unable to access the shared UIApplication"));
		return;
	}

	id<UIApplicationDelegate> AppDelegate = SharedApp.delegate;
	Class AppDelegateClass = [AppDelegate class];

	// Swizzle willPresentNotification and didReceiveNotificationResponse
	Class NotifCenterDelClass = [[UNUserNotificationCenter currentNotificationCenter].delegate class];

	SEL PresentSel = @selector(userNotificationCenter:willPresentNotification:withCompletionHandler:);
	Method OriginalPresentMethod = class_getInstanceMethod(AppDelegateClass, PresentSel);
	if (OriginalPresentMethod)
	{
		UNNotificationPresentationOptions PresentOptions = UNNotificationPresentationOptionNone;

		const UCleverTapConfig* const DefaultConfig =
			UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
		if (DefaultConfig && DefaultConfig->bIOSPresentPushNotificationsInForeground)
		{
			PresentOptions = UNNotificationPresentationOptionBanner | UNNotificationPresentationOptionList;
		}

		const char* OrigMethodArgs = method_getTypeEncoding(OriginalPresentMethod);
		NSMethodSignature* OrigSig = [NSMethodSignature signatureWithObjCTypes:OrigMethodArgs];
		NSInvocation* OrigInvocation = [NSInvocation invocationWithMethodSignature:OrigSig];

		id NewPresentBlock = ^(id Obj, UNUserNotificationCenter* Center, UNNotification* Notification,
			void (^CompletionHandler)(UNNotificationPresentationOptions Options)) {
		  FIOSCleverTapInstance::HandleWillPresentNotification(Notification.request.content.userInfo);

		  // Forward onto the original implementation, but make the completion handler a no-op
		  void (^EmptyHandler)(UNNotificationPresentationOptions Options) =
			  ^(UNNotificationPresentationOptions IgnoredOpts) {};

		  [OrigInvocation setArgument:&Center atIndex:2];
		  [OrigInvocation setArgument:&Notification atIndex:3];
		  [OrigInvocation setArgument:&EmptyHandler atIndex:4];
		  [OrigInvocation invokeWithTarget:Obj];

		  // Invoke the actual completion handler with the options we want
		  CompletionHandler(PresentOptions);
		};
		IMP NewPresentImp = imp_implementationWithBlock(NewPresentBlock);

		SEL NewPresentSel = sel_registerName("_ct_willPresentNotification_swizzled");
		class_addMethod(NotifCenterDelClass, NewPresentSel, NewPresentImp, OrigMethodArgs);

		OrigInvocation.selector = NewPresentSel;

		method_exchangeImplementations(
			OriginalPresentMethod, class_getInstanceMethod(NotifCenterDelClass, NewPresentSel));
	}
	else
	{
		UE_LOG(LogCleverTap, Error,
			TEXT(
				"Failed to find instance method 'userNotificationCenter:willPresentNotification:withCompletionHandler'. Push notifications may not be correctly handled for CleverTap"));
	}

	SEL ReceiveSel = @selector(userNotificationCenter:didReceiveNotificationResponse:withCompletionHandler:);
	Method OriginalReceiveMethod = class_getInstanceMethod(AppDelegateClass, ReceiveSel);
	if (OriginalReceiveMethod)
	{
		const char* OrigMethodArgs = method_getTypeEncoding(OriginalReceiveMethod);
		NSMethodSignature* OrigSig = [NSMethodSignature signatureWithObjCTypes:OrigMethodArgs];
		NSInvocation* OrigInvocation = [NSInvocation invocationWithMethodSignature:OrigSig];

		id NewReceiveBlock = ^(
			id Obj, UNUserNotificationCenter* Center, UNNotificationResponse* Response, void (^CompletionHandler)()) {
		  FIOSCleverTapInstance::HandleDidReceiveNotificationResponse(Response.notification.request.content.userInfo);

		  [OrigInvocation setArgument:&Center atIndex:2];
		  [OrigInvocation setArgument:&Response atIndex:3];
		  [OrigInvocation setArgument:&CompletionHandler atIndex:4];
		  [OrigInvocation invokeWithTarget:Obj];
		};
		IMP NewReceiveImp = imp_implementationWithBlock(NewReceiveBlock);

		SEL NewReceiveSel = sel_registerName("_ct_didReceiveNotificationResponse_swizzled");
		class_addMethod(NotifCenterDelClass, NewReceiveSel, NewReceiveImp, OrigMethodArgs);

		OrigInvocation.selector = NewReceiveSel;

		method_exchangeImplementations(
			OriginalReceiveMethod, class_getInstanceMethod(NotifCenterDelClass, NewReceiveSel));
	}
	else
	{
		UE_LOG(LogCleverTap, Error,
			TEXT(
				"Failed to find instance method 'userNotificationCenter:didReceiveNotificationResponse:withCompletionHandler'. Push notifications may not be correctly handled for CleverTap"));
	}
#endif
}

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
		[Extras = MoveTemp(Extras), Inst = CppInstance]() { Inst->HandlePushNotificationTapped(Extras); });
}

- (BOOL)shouldShowInAppNotificationWithExtras:(NSDictionary*)Extras
{
	return CppInstance->ShouldShowInAppNotification(ConvertFromNSDictionary(Extras)) ? YES : NO;
}

- (void)inAppNotificationDismissedWithExtras:(NSDictionary*)InExtras andActionExtras:(NSDictionary*)InActionExtras
{
	FCleverTapProperties Extras = ConvertFromNSDictionary(InExtras);
	FCleverTapProperties ActionExtras = ConvertFromNSDictionary(InActionExtras);
	AsyncTask(ENamedThreads::GameThread,
		[Extras = MoveTemp(Extras), ActionExtras = MoveTemp(ActionExtras), Inst = CppInstance]() {
			Inst->HandleInAppNotificationDismissed(Extras, ActionExtras);
		});
}

- (void)inAppNotificationDidShow:(NSDictionary*)InNotification
{
	FCleverTapProperties Notification = ConvertFromNSDictionary(InNotification);
	AsyncTask(ENamedThreads::GameThread, [Notification = MoveTemp(Notification), Inst = CppInstance]() {
		Inst->HandleInAppNotificationShown(Notification);
	});
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

void FPlatformSDK::SetRemoteNotificationToken(ICleverTapInstance& Instance, const TArray<uint8>& Token)
{
	auto& IOSInstance = static_cast<FIOSCleverTapInstance&>(Instance);
	IOSInstance.SetPushToken(Token);
}

}} // namespace CleverTapSDK::IOS
