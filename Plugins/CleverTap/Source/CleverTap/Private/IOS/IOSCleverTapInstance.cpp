// Copyright CleverTap All Rights Reserved.
#include "IOSCleverTapInstance.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "CleverTapConfig.h"
#include "CleverTapInstance.h"
#include "CleverTapLog.h"
#include "CleverTapSubsystem.h"
#include "CleverTapUtilities.h"
#include "IOS/IOSAppDelegate.h"
#include "IOS/URLFilterList.h"
#include "Misc/CoreDelegates.h"

#import <CleverTapSDK/CleverTap.h>
#import <CleverTapSDK/CleverTapInAppNotificationDelegate.h>
#import <CleverTapSDK/CleverTapPushNotificationDelegate.h>
#import <CleverTapSDK/CleverTapURLDelegate.h>
#import <CleverTapSDK/CleverTap+InAppNotifications.h>
#import <CleverTapSDK/CTLocalInApp.h>
#import <CleverTapSDK/CTVar.h>
#import <CleverTapSDK/CleverTap+CTVar.h>
#import <objc/runtime.h>
#import <UserNotifications/UserNotifications.h>

//==================================================================================================
// CleverTapSDKListener

// TODO: Not exposed
@interface CleverTapSDKListener :
	NSObject <CleverTapInAppNotificationDelegate, CleverTapPushNotificationDelegate,
		CleverTapURLDelegate /*, CleverTapPushPermissionDelegate*/>
- (instancetype)initWithCppInstance:(UIOSCleverTapInstance*)Instance;
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

	for (const auto& Entry : Properties.Map)
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
	ResultRef.Map.Reserve(ResultRef.Map.Num() + NumElements);

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
			ResultRef.Map.Add(FString{ NewPrefix }, MoveTemp(MaybeUnrealValue));
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
			UE_LOG(LogCleverTap, Warning, TEXT("Unhandled NSDictionary entry for key '%s': %s"), *FString{ NewPrefix },
				*FString{ NSStringFromClass([Value class]) });
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

constexpr uint8 CTSTATE_FLAGS_REGISTERED_FOR_PUSH = 0x1;
constexpr uint8 CTSTATE_FLAGS_OPENURL_ENABLED = 0x2;

constexpr int8 PUSH_PERM_STATUS_UNKNOWN = 0;
constexpr int8 PUSH_PERM_STATUS_GRANTED = 1;
constexpr int8 PUSH_PERM_STATUS_DENIED = 2;

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
	NSLog(@"[CleverTap] EnsurePushNotificationMonitoring running on AppDelegate class: %@", NSStringFromClass(AppDelegateClass));

	// Intercept application:didRegisterForRemoteNotificationsWithDeviceToken: so that the APNs
	// device token is forwarded directly to CleverTap.
	//
	// Strategy:
	//   1. If UE's AppDelegate already implements this method, swizzle it so we piggyback on
	//      the existing implementation and still call the original.
	//   2. If UE's AppDelegate does NOT implement it (some UE 5.x versions omit it), add our
	//      own implementation directly so iOS knows to call it at all.
	//
	// We use NSLog for diagnostics here because UE_LOG goes to stdout (not visible in macOS
	// Console app), while NSLog goes to the unified logging system (IS visible in Console app).
	SEL TokenSel = @selector(application:didRegisterForRemoteNotificationsWithDeviceToken:);
	// void (id, SEL, UIApplication*, NSData*)
	const char* const TokenMethodTypeEncoding = "v@:@@";
	Method OriginalTokenMethod = class_getInstanceMethod(AppDelegateClass, TokenSel);
	if (OriginalTokenMethod)
	{
		// Method exists — swizzle it so we intercept the call and then forward to the original.
		NSLog(@"[CleverTap] Swizzling application:didRegisterForRemoteNotificationsWithDeviceToken:");
		const char* OrigTokenMethodArgs = method_getTypeEncoding(OriginalTokenMethod);
		NSMethodSignature* OrigTokenSig = [NSMethodSignature signatureWithObjCTypes:OrigTokenMethodArgs];
		NSInvocation* OrigTokenInvocation = [NSInvocation invocationWithMethodSignature:OrigTokenSig];

		id NewTokenBlock = ^(id Obj, UIApplication* App, NSData* DeviceToken) {
		  NSLog(@"[CleverTap] APNs token received via swizzle (%lu bytes)", (unsigned long)[DeviceToken length]);
		  UIOSCleverTapInstance::HandleRemoteNotificationToken(DeviceToken);
		  // Forward onto the original UE implementation so UE's own token handling still works.
		  [OrigTokenInvocation setArgument:&App atIndex:2];
		  [OrigTokenInvocation setArgument:&DeviceToken atIndex:3];
		  [OrigTokenInvocation invokeWithTarget:Obj];
		};
		IMP NewTokenImp = imp_implementationWithBlock(NewTokenBlock);

		SEL NewTokenSel = sel_registerName("_ct_didRegisterForRemoteNotificationsWithDeviceToken_swizzled");
		class_addMethod(AppDelegateClass, NewTokenSel, NewTokenImp, OrigTokenMethodArgs);
		OrigTokenInvocation.selector = NewTokenSel;
		method_exchangeImplementations(
			OriginalTokenMethod, class_getInstanceMethod(AppDelegateClass, NewTokenSel));
	}
	else
	{
		// Method does NOT exist in UE's AppDelegate — add it outright so iOS calls it.
		NSLog(@"[CleverTap] application:didRegisterForRemoteNotificationsWithDeviceToken: not found on AppDelegate — adding it directly");
		id NewTokenBlock = ^(id Obj, UIApplication* App, NSData* DeviceToken) {
		  NSLog(@"[CleverTap] APNs token received via injected method (%lu bytes)", (unsigned long)[DeviceToken length]);
		  UIOSCleverTapInstance::HandleRemoteNotificationToken(DeviceToken);
		};
		IMP NewTokenImp = imp_implementationWithBlock(NewTokenBlock);
		const BOOL bAdded = class_addMethod(AppDelegateClass, TokenSel, NewTokenImp, TokenMethodTypeEncoding);
		NSLog(@"[CleverTap] Injected token method: %@", bAdded ? @"SUCCESS" : @"FAILED (already exists in superclass?)");
	}

	// Also intercept application:didFailToRegisterForRemoteNotificationsWithError: so we can
	// surface registration failures clearly. Without this, failures are silent.
	SEL FailTokenSel = @selector(application:didFailToRegisterForRemoteNotificationsWithError:);
	Method OriginalFailTokenMethod = class_getInstanceMethod(AppDelegateClass, FailTokenSel);
	if (OriginalFailTokenMethod)
	{
		const char* OrigFailArgs = method_getTypeEncoding(OriginalFailTokenMethod);
		NSMethodSignature* OrigFailSig = [NSMethodSignature signatureWithObjCTypes:OrigFailArgs];
		NSInvocation* OrigFailInvocation = [NSInvocation invocationWithMethodSignature:OrigFailSig];

		id NewFailBlock = ^(id Obj, UIApplication* App, NSError* Error) {
		  NSLog(@"[CleverTap] ERROR: didFailToRegisterForRemoteNotificationsWithError: %@", Error);
		  [OrigFailInvocation setArgument:&App atIndex:2];
		  [OrigFailInvocation setArgument:&Error atIndex:3];
		  [OrigFailInvocation invokeWithTarget:Obj];
		};
		IMP NewFailImp = imp_implementationWithBlock(NewFailBlock);
		SEL NewFailSel = sel_registerName("_ct_didFailToRegisterForRemoteNotifications_swizzled");
		class_addMethod(AppDelegateClass, NewFailSel, NewFailImp, OrigFailArgs);
		OrigFailInvocation.selector = NewFailSel;
		method_exchangeImplementations(
			OriginalFailTokenMethod, class_getInstanceMethod(AppDelegateClass, NewFailSel));
	}
	else
	{
		// Add it so any future registration failure is logged.
		id NewFailBlock = ^(id Obj, UIApplication* App, NSError* Error) {
		  NSLog(@"[CleverTap] ERROR: didFailToRegisterForRemoteNotificationsWithError: %@", Error);
		};
		IMP NewFailImp = imp_implementationWithBlock(NewFailBlock);
		class_addMethod(AppDelegateClass, FailTokenSel, NewFailImp, "v@:@@");
	}

	// Swizzle willPresentNotification and didReceiveNotificationResponse.
	// Note: in UE the AppDelegate also implements UNUserNotificationCenterDelegate, so
	// AppDelegateClass and NotifCenterDelClass are the same class. We fall back to AppDelegateClass
	// if the notification center delegate is not yet set (can happen during early startup).
	id NotifCenterDelegate = [UNUserNotificationCenter currentNotificationCenter].delegate;
	Class NotifCenterDelClass = (NotifCenterDelegate != nil) ? [NotifCenterDelegate class] : AppDelegateClass;

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
		  UIOSCleverTapInstance::HandleWillPresentNotification(Notification.request.content.userInfo);

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
		  UIOSCleverTapInstance::HandleDidReceiveNotificationResponse(Response.notification.request.content.userInfo);

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

const CleverTapSDK::IOS::FURLFilterList& GetSharedURLFilterList()
{
	static CleverTapSDK::IOS::FURLFilterList FilterList = [] {
		check(IsInGameThread());

		const UCleverTapConfig* const Config = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
		check(Config != nullptr);
		return CleverTapSDK::IOS::FURLFilterList{ *Config };
	}();
	return FilterList;
}

} // namespace

@implementation CleverTapSDKListener
{
	UIOSCleverTapInstance* CppInstance;
}

- (instancetype)initWithCppInstance:(UIOSCleverTapInstance*)Instance
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

- (void)inAppNotificationButtonTappedWithCustomExtras:(NSDictionary*)CustomExtras
{
	AsyncTask(ENamedThreads::GameThread, [Extras = ConvertFromNSDictionary(CustomExtras), Inst = CppInstance]() {
		Inst->HandleInAppNotificationButtonClicked(Extras);
	});
}

@end

//==================================================================================================
// UIOSCleverTapInstance

TArray<UIOSCleverTapInstance*> UIOSCleverTapInstance::AllInstances{};

UIOSCleverTapInstance* UIOSCleverTapInstance::CreateFromNativeInstance(CleverTap* NativeInstance)
{
	auto* const Instance = NewObject<UIOSCleverTapInstance>(GEngine->GetEngineSubsystem<UCleverTapSubsystem>());
	Instance->NativeInstance = NativeInstance;
	Instance->Variables = [NSMutableDictionary new];
	Instance->SDKListener = [[CleverTapSDKListener alloc] initWithCppInstance:Instance];
	Instance->PushPermissionStatus = static_cast<uint8>(ECleverTapPushPermissionStatus::Unknown);

	AllInstances.Add(Instance);

	if (NativeInstance != nil)
	{
		[NativeInstance suspendInAppNotifications];
		[NativeInstance setUrlDelegate:Instance->SDKListener];
		[NativeInstance setInAppNotificationDelegate:Instance->SDKListener];

		// TODO: Not exposed
		// [NativeInstance setPushPermissionDelegate:Instance->SDKListener];

		// Query the real iOS notification permission status so GetPushPermissionStatus()
		// returns an accurate value immediately after initialization (not always Unknown).
		UIOSCleverTapInstance* RawInstance = Instance;
		[[UNUserNotificationCenter currentNotificationCenter]
			getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings* Settings) {
			const bool bIsGranted = (Settings.authorizationStatus == UNAuthorizationStatusAuthorized
				|| Settings.authorizationStatus == UNAuthorizationStatusProvisional);
			RawInstance->CachePushPermissionStatus(bIsGranted);
		}];

		if (NativeInstance == [CleverTap sharedInstance])
		{
			Instance->OnURLOpenHandle =
				FIOSCoreDelegates::OnOpenURL.AddUObject(Instance, &UIOSCleverTapInstance::HandleOnOpenURL);
		}

		// Forward the APNs device token to CleverTap so push notifications can be sent.
		// UE fires ApplicationRegisteredForRemoteNotificationsDelegate when iOS delivers
		// the token via application:didRegisterForRemoteNotificationsWithDeviceToken:.
		Instance->OnPushTokenHandle =
			FCoreDelegates::ApplicationRegisteredForRemoteNotificationsDelegate.AddLambda(
				[RawInstance](TArray<uint8> Token) { RawInstance->SetPushToken(Token); });

		// Fix race condition: UE calls registerForRemoteNotifications at startup, which can
		// deliver the APNs token BEFORE the delegate above is subscribed — causing the token
		// to be lost. Calling registerForRemoteNotifications again forces iOS to re-deliver
		// any already-registered token to our newly-registered listener.
		// This is safe to call at any time; it is a no-op if not already registered.
		dispatch_async(dispatch_get_main_queue(), ^{
			[[UIApplication sharedApplication] registerForRemoteNotifications];
		});

		// Automatically request push permission if configured to do so.
		const UCleverTapConfig* const DefaultConfig =
			UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
		if (DefaultConfig && DefaultConfig->bIOSAutoRequestPushPermission)
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				UNUserNotificationCenter* Center = [UNUserNotificationCenter currentNotificationCenter];
				UNAuthorizationOptions Options =
					UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge;
				[Center requestAuthorizationWithOptions:Options
									 completionHandler:^(BOOL Granted, NSError* Error) {
					if (Error != nil)
					{
						UE_LOG(LogCleverTap, Warning, TEXT("Auto push permission request error: %s"),
							*FString([Error localizedDescription]));
					}
					dispatch_async(dispatch_get_main_queue(), ^{
						if (Granted)
						{
							[[UIApplication sharedApplication] registerForRemoteNotifications];
						}
						RawInstance->CachePushPermissionStatus(Granted);
						RawInstance->OnPushPermissionResponse.Broadcast(Granted);
					});
				}];
			});
		}
	}

	EnsurePushNotificationMonitoring();

	// Construction of the shared filter list requires us to be on the GameThread,
	//  but usage of it doesn't. We pre-construct it here so that it can be used to
	//  filter URLs on all threads.
	CleverTapSDK::Ignore = GetSharedURLFilterList();

	return Instance;
}

UIOSCleverTapInstance::~UIOSCleverTapInstance()
{
	if (OnURLOpenHandle.IsValid())
	{
		FIOSCoreDelegates::OnOpenURL.Remove(OnURLOpenHandle);
	}

	if (OnPushTokenHandle.IsValid())
	{
		FCoreDelegates::ApplicationRegisteredForRemoteNotificationsDelegate.Remove(OnPushTokenHandle);
	}

	[SDKListener release];
	AllInstances.RemoveSingleSwap(this);
}

void UIOSCleverTapInstance::HandleWillPresentNotification(NSDictionary* UserInfo)
{
	for (UIOSCleverTapInstance* Inst : AllInstances)
	{
		[Inst->NativeInstance handleNotificationWithData:UserInfo openDeepLinksInForeground:YES];
	}
}

void UIOSCleverTapInstance::HandleDidReceiveNotificationResponse(NSDictionary* UserInfo)
{
	for (UIOSCleverTapInstance* Inst : AllInstances)
	{
		[Inst->NativeInstance handleNotificationWithData:UserInfo];
	}
}

void UIOSCleverTapInstance::HandleRemoteNotificationToken(NSData* DeviceToken)
{
	if (DeviceToken == nil || [DeviceToken length] == 0)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("HandleRemoteNotificationToken: received nil or empty token"));
		return;
	}

	const NSUInteger TokenLen = [DeviceToken length];
	TArray<uint8> TokenArray;
	TokenArray.AddUninitialized(static_cast<int32>(TokenLen));
	[DeviceToken getBytes:TokenArray.GetData() length:TokenLen];

	NSLog(@"[CleverTap] HandleRemoteNotificationToken: forwarding %d-byte APNs token to %d CleverTap instance(s)",
		TokenArray.Num(), AllInstances.Num());

	for (UIOSCleverTapInstance* Inst : AllInstances)
	{
		Inst->SetPushToken(TokenArray);
	}
}

void UIOSCleverTapInstance::SetPushToken(const TArray<uint8>& Token)
{
	check(NativeInstance != nil);
	NSLog(@"[CleverTap] SetPushToken: sending %d-byte token to CleverTap native SDK", Token.Num());
	[NativeInstance setPushToken:[NSData dataWithBytes:Token.GetData() length:Token.Num()]];
}

FString UIOSCleverTapInstance::GetCleverTapId()
{
	check(NativeInstance != nil);
	return FString{ [NativeInstance profileGetCleverTapID] };
}

void UIOSCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile)
{
	check(NativeInstance != nil);
	[NativeInstance onUserLogin:ConvertToNSDictionary(Profile)];
}

void UIOSCleverTapInstance::OnUserLoginWithCleverTapId(const FCleverTapProperties& Profile, const FString& CleverTapId)
{
	check(NativeInstance != nil);
	[NativeInstance onUserLogin:ConvertToNSDictionary(Profile) withCleverTapID:CleverTapId.GetNSString()];
}

void UIOSCleverTapInstance::PushProfile(const FCleverTapProperties& Profile)
{
	check(NativeInstance != nil);
	[NativeInstance profilePush:ConvertToNSDictionary(Profile)];
}

TOptional<FCleverTapPropertyValue> UIOSCleverTapInstance::GetProperty(const FString& Key)
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

void UIOSCleverTapInstance::DecrementIntValue(const FString& Key, int Amount)
{
	check(NativeInstance != nil);
	[NativeInstance profileDecrementValueBy:[NSNumber numberWithInt:Amount] forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::DecrementFloatValue(const FString& Key, float Amount)
{
	check(NativeInstance != nil);
	[NativeInstance profileDecrementValueBy:[NSNumber numberWithDouble:Amount] forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::IncrementIntValue(const FString& Key, int Amount)
{
	check(NativeInstance != nil);
	[NativeInstance profileIncrementValueBy:[NSNumber numberWithInt:Amount] forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::IncrementFloatValue(const FString& Key, float Amount)
{
	check(NativeInstance != nil);
	[NativeInstance profileIncrementValueBy:[NSNumber numberWithDouble:Amount] forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::AddMultiValueForKey(const FString& Key, const FString& Value)
{
	check(NativeInstance != nil);
	[NativeInstance profileAddMultiValue:Value.GetNSString() forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::AddMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
{
	check(NativeInstance != nil);
	[NativeInstance profileAddMultiValues:ConvertToNSArray(Values) forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::RemoveMultiValueForKey(const FString& Key, const FString& Value)
{
	check(NativeInstance != nil);
	[NativeInstance profileRemoveMultiValue:Value.GetNSString() forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
{
	check(NativeInstance != nil);
	[NativeInstance profileRemoveMultiValues:ConvertToNSArray(Values) forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::RemoveValueForKey(const FString& Key)
{
	check(NativeInstance != nil);
	[NativeInstance profileRemoveValueForKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::SetMultiValuesForKey(const FString& Key, const TArray<FString> Values)
{
	check(NativeInstance != nil);
	[NativeInstance profileSetMultiValues:ConvertToNSArray(Values) forKey:Key.GetNSString()];
}

void UIOSCleverTapInstance::PushEvent(const FString& EventName)
{
	check(NativeInstance != nil);
	[NativeInstance recordEvent:EventName.GetNSString()];
}

void UIOSCleverTapInstance::PushEventWithProperties(const FString& EventName, const FCleverTapProperties& Actions)
{
	check(NativeInstance != nil);
	[NativeInstance recordEvent:EventName.GetNSString() withProps:ConvertToNSDictionary(Actions)];
}

void UIOSCleverTapInstance::PushChargedEvent(
	const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items)
{
	check(NativeInstance != nil);
	[NativeInstance recordChargedEventWithDetails:ConvertToNSDictionary(ChargeDetails)
										 andItems:ConvertToNSArray(Items)];
}

ECleverTapPushPermissionStatus UIOSCleverTapInstance::GetPushPermissionStatus()
{
	const uint8 StatusValue = PushPermissionStatus.Load();
	return static_cast<ECleverTapPushPermissionStatus>(StatusValue);
}

void UIOSCleverTapInstance::PromptForPushPermission(bool bFallbackToSettings)
{
	check(NativeInstance != nil);

	UNUserNotificationCenter* Center = [UNUserNotificationCenter currentNotificationCenter];
	UNAuthorizationOptions Options =
		UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge;

	UIOSCleverTapInstance* RawSelf = this;

	[Center requestAuthorizationWithOptions:Options
						  completionHandler:^(BOOL Granted, NSError* Error) {
		if (Error != nil)
		{
			UE_LOG(LogCleverTap, Warning, TEXT("Push permission request error: %s"),
				*FString([Error localizedDescription]));
		}

		dispatch_async(dispatch_get_main_queue(), ^{
			if (Granted)
			{
				[[UIApplication sharedApplication] registerForRemoteNotifications];
			}

			RawSelf->CachePushPermissionStatus(Granted);
			RawSelf->OnPushPermissionResponse.Broadcast(Granted);
		});
	}];
}

void UIOSCleverTapInstance::PromptForPushPermissionWithAlertPrimer(
	const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig)
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

void UIOSCleverTapInstance::PromptForPushPermissionWithHalfInterstitialPrimer(
	const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig)
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
	[localInAppBuilder setFallbackToSettings:ConvertToNSValue(PushPrimerHalfInterstitialConfig.bFallbackToSettings)];

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

void UIOSCleverTapInstance::EnableOnPushNotificationClicked()
{
	check(NativeInstance != nil);

	if (IsRegisteredForPushNotificationClicked())
	{
		return;
	}
	SetIsRegisteredForPushNotificationClicked();

	[NativeInstance setPushNotificationDelegate:SDKListener];
}

void UIOSCleverTapInstance::EnableOnOpenUrl()
{
	if (!IsOpenURLEnabled())
	{
		SetOpenURLEnabled();

		auto BroadcastQueuedUrls = [this] {
			FScopeLock Lck{ &CriticalSection };

			for (const auto& Url : OpenURLQueue)
			{
				OnOpenUrl.Broadcast(Url);
			}
			OpenURLQueue.Empty();
		};

		if (IsInGameThread())
		{
			BroadcastQueuedUrls();
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, MoveTemp(BroadcastQueuedUrls));
		}
	}
}

void UIOSCleverTapInstance::RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> InUrlHandler)
{
	check(NativeInstance != nil);

	{
		FScopeLock Lck{ &CriticalSection };
		UrlHandler = MoveTemp(InUrlHandler);
	}
}

void UIOSCleverTapInstance::RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter)
{
	FScopeLock Lck{ &CriticalSection };
	InAppNotificationFilter = MoveTemp(Filter);
}

void UIOSCleverTapInstance::DiscardInAppNotifications()
{
	check(NativeInstance != nil);
	[NativeInstance discardInAppNotifications];
}

void UIOSCleverTapInstance::ResumeInAppNotifications()
{
	check(NativeInstance != nil);
	[NativeInstance resumeInAppNotifications];
}

void UIOSCleverTapInstance::SuspendInAppNotifications()
{
	check(NativeInstance != nil);
	[NativeInstance suspendInAppNotifications];
}

void UIOSCleverTapInstance::Unmute()
{
	UE_LOG(LogCleverTap, Log, TEXT("Unmute() — not supported on iOS SDK"));
}

void UIOSCleverTapInstance::RecordDisplayUnitClickedEventForID(const FString& UnitID)
{
	CleverTapSDK::Ignore(UnitID);
}

void UIOSCleverTapInstance::SetOffline(bool bIsOffline)
{
	check(NativeInstance != nil);
	[NativeInstance setOffline:bIsOffline ? YES : NO];
}

void UIOSCleverTapInstance::SetOptOut(bool bIsOptingOut)
{
	check(NativeInstance != nil);
	[NativeInstance setOptOut:bIsOptingOut ? YES : NO];
}

void UIOSCleverTapInstance::SetNetworkInformationRecording(bool bEnableCollection)
{
	check(NativeInstance != nil);
	[NativeInstance enableDeviceNetworkInfoReporting:bEnableCollection ? YES : NO];
}

bool UIOSCleverTapInstance::LocalizeAndroidNotificationChannel(
	const FString& ChannelID, const FText& ChannelName, const FText& ChannelDescription)
{
	CleverTapSDK::Ignore(ChannelID, ChannelName, ChannelDescription);
	return false;
}

bool UIOSCleverTapInstance::LocalizeAndroidNotificationChannelGroup(const FString& GroupID, const FText& GroupName)
{
	CleverTapSDK::Ignore(GroupID, GroupName);
	return false;
}

bool UIOSCleverTapInstance::IsRegisteredForPushNotificationClicked() const
{
	const uint8 Flags = StateFlags.Load();
	return (Flags & CTSTATE_FLAGS_REGISTERED_FOR_PUSH) != 0;
}

void UIOSCleverTapInstance::SetIsRegisteredForPushNotificationClicked()
{
	StateFlags |= CTSTATE_FLAGS_REGISTERED_FOR_PUSH;
}

bool UIOSCleverTapInstance::IsOpenURLEnabled() const
{
	const uint8 Flags = StateFlags.Load();
	return (Flags & CTSTATE_FLAGS_OPENURL_ENABLED) != 0;
}

void UIOSCleverTapInstance::SetOpenURLEnabled()
{
	StateFlags |= CTSTATE_FLAGS_OPENURL_ENABLED;
}

void UIOSCleverTapInstance::CachePushPermissionStatus(bool bIsGranted)
{
	const ECleverTapPushPermissionStatus Status =
		bIsGranted ? ECleverTapPushPermissionStatus::Granted : ECleverTapPushPermissionStatus::NotGranted;
	PushPermissionStatus.Store(static_cast<uint8>(Status));
}

void UIOSCleverTapInstance::HandlePushNotificationTapped(const FCleverTapProperties& Extras)
{
	// Broadcast the tap
	this->OnPushNotificationClicked.Broadcast(Extras);
}

bool UIOSCleverTapInstance::HandleUrl(FString Url, ECleverTapChannel Channel)
{
	if (GetSharedURLFilterList().IsFilteredURL(Url))
	{
		UE_LOG(LogCleverTap, Log, TEXT("URL %s was filtered by shared configuration deep link requirements"), *Url);
		return false;
	}

	// Even with a UrlHandler installed we want to make sure OnOpenUrl is always invoked
	if (IsOpenURLEnabled())
	{
		AsyncTask(ENamedThreads::GameThread, [this, Url] { OnOpenUrl.Broadcast(Url); });

		// Lock not needed for the above, but we need it now for the UrlHandler
		{
			FScopeLock Lck{ &CriticalSection };

			if (UrlHandler)
			{
				return UrlHandler(MoveTemp(Url), Channel);
			}
		}
	}
	else
	{
		FScopeLock Lck{ &CriticalSection };

		// It's possible that after we get the lock the state has changed so check again just in case
		if (IsOpenURLEnabled())
		{
			AsyncTask(ENamedThreads::GameThread, [this, Url] { OnOpenUrl.Broadcast(Url); });
		}
		else
		{
			OpenURLQueue.Add(Url);
		}

		if (UrlHandler)
		{
			return UrlHandler(MoveTemp(Url), Channel);
		}
	}

	return true;
}

bool UIOSCleverTapInstance::ShouldShowInAppNotification(const FCleverTapProperties& Extras) const
{
	FScopeLock Lck{ &CriticalSection };

	if (InAppNotificationFilter)
	{
		return InAppNotificationFilter(Extras);
	}

	return true;
}

void UIOSCleverTapInstance::HandleInAppNotificationDismissed(
	const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras)
{
	OnInAppNotificationDismissed.Broadcast(Extras, ActionExtras);
}

void UIOSCleverTapInstance::HandleInAppNotificationShown(const FCleverTapProperties& Notification)
{
	OnInAppNotificationShown.Broadcast(Notification);
}

void UIOSCleverTapInstance::HandleInAppNotificationButtonClicked(const FCleverTapProperties& ButtonProperties)
{
	OnInAppNotificationButtonClicked.Broadcast(ButtonProperties);
}

void UIOSCleverTapInstance::HandleOnOpenURL(UIApplication* App, NSURL* URL, NSString* Source, id Annotation)
{
	CleverTapSDK::Ignore(App, Source, Annotation);

	FString URLStr{ URL.absoluteString };
	if (GetSharedURLFilterList().IsFilteredURL(URLStr))
	{
		UE_LOG(LogCleverTap, Log, TEXT("URL %s was filtered by shared configuration deep link requirements"), *URLStr);
		return;
	}

	[NativeInstance handleOpenURL:URL sourceApplication:Source];
}
//Product Experience
void UIOSCleverTapInstance::DefineStringVariable(const FString& Name, const FString& DefaultValue)
{
	NSString* NSName = Name.GetNSString();
	NSString* NSDefault = DefaultValue.GetNSString();
	CTVar* Var = [NativeInstance defineVar:NSName withString:NSDefault];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineStringVariable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}
void UIOSCleverTapInstance::DefineIntVariable(const FString& Name, int32 DefaultValue)
{
	NSString* NSName = Name.GetNSString();
	CTVar* Var = [NativeInstance defineVar:NSName withInt:DefaultValue];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineIntVariable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}
void UIOSCleverTapInstance::DefineInt64Variable(const FString& Name, int64 DefaultValue)
{
	NSString* NSName = Name.GetNSString();
	CTVar* Var = [NativeInstance defineVar:NSName withLongLong:(long long)DefaultValue];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineInt64Variable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}
void UIOSCleverTapInstance::DefineFloatVariable(const FString& Name, float DefaultValue)
{
	NSString* NSName = Name.GetNSString();
	CTVar* Var = [NativeInstance defineVar:NSName withFloat:DefaultValue];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineFloatVariable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}
void UIOSCleverTapInstance::DefineDoubleVariable(const FString& Name, double DefaultValue)
{
	NSString* NSName = Name.GetNSString();
	CTVar* Var = [NativeInstance defineVar:NSName withDouble:DefaultValue];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineDoubleVariable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}
void UIOSCleverTapInstance::DefineBoolVariable(const FString& Name, bool DefaultValue)
{
	NSString* NSName = Name.GetNSString();
	CTVar* Var = [NativeInstance defineVar:NSName withBool:DefaultValue ? YES : NO];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineBoolVariable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}
void UIOSCleverTapInstance::DefineStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue)
{
	NSString* NSName = Name.GetNSString();
	NSMutableDictionary<NSString*, NSString*>* NSDefault = [NSMutableDictionary new];
	for (const auto& Pair : DefaultValue)
	{
		NSDefault[Pair.Key.GetNSString()] = Pair.Value.GetNSString();
	}
	CTVar* Var = [NativeInstance defineVar:NSName withDictionary:NSDefault];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineStringMapVariable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}
void UIOSCleverTapInstance::DefineFileVariable(const FString& Name)
{
	NSString* NSName = Name.GetNSString();
	CTVar* Var = [NativeInstance defineFileVar:NSName];
	if (Var == nil) { UE_LOG(LogCleverTap, Warning, TEXT("DefineFileVariable: SDK returned nil for '%s'"), *Name); return; }
	Variables[NSName] = Var;
}

void UIOSCleverTapInstance::FetchVariables()
{
	UE_LOG(LogCleverTap, Log, TEXT("FetchVariables() — requesting server values"));
	TWeakObjectPtr<UIOSCleverTapInstance> WeakSelf(this);

	// Register onVariablesChanged BEFORE kicking off the fetch to eliminate any race
	// where the SDK fires the callback before we've subscribed.
	// Guard: onVariablesChanged is additive — register only once per instance.
	if (!bVariablesChangedRegistered)
	{
		bVariablesChangedRegistered = true;
		[NativeInstance onVariablesChanged:^{
			UE_LOG(LogCleverTap, Log, TEXT("FetchVariables() — onVariablesChanged fired"));
			AsyncTask(ENamedThreads::GameThread, [WeakSelf]() {
				if (UIOSCleverTapInstance* Self = WeakSelf.Get())
				{
					Self->OnVariablesChanged.Broadcast();
				}
			});
		}];
	}

	[NativeInstance fetchVariables:^(BOOL Success) {
		UE_LOG(LogCleverTap, Log, TEXT("FetchVariables() — callback: success=%s"), Success ? TEXT("YES") : TEXT("NO"));
		AsyncTask(ENamedThreads::GameThread, [WeakSelf, Success]() {
			if (UIOSCleverTapInstance* Self = WeakSelf.Get())
			{
				Self->OnVariablesFetched.Broadcast((bool)Success);
			}
		});
	}];
}

void UIOSCleverTapInstance::SyncVariables()
{
	// Pass isProduction:YES so variable definitions appear in the Production section
	// of the CleverTap dashboard — the same section where Android-synced variables show.
	// (isProduction:NO sends to the separate Development slot which is not visible by default.)
	UE_LOG(LogCleverTap, Log, TEXT("SyncVariables() — uploading %d variable definition(s) to dashboard"),
		Variables != nil ? (int)[Variables count] : 0);
	[NativeInstance syncVariables:YES];
}

FString UIOSCleverTapInstance::GetStringVariable(const FString& Name, const FString& DefaultValue) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil || ![Var.value isKindOfClass:[NSString class]]) return DefaultValue;
	return FString((NSString*)Var.value);
}
int32 UIOSCleverTapInstance::GetIntVariable(const FString& Name, int32 DefaultValue) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil || ![Var.value isKindOfClass:[NSNumber class]]) return DefaultValue;
	return [(NSNumber*)Var.value intValue];
}
int64 UIOSCleverTapInstance::GetInt64Variable(const FString& Name, int64 DefaultValue) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil || ![Var.value isKindOfClass:[NSNumber class]]) return DefaultValue;
	return (int64)[(NSNumber*)Var.value longLongValue];
}
float UIOSCleverTapInstance::GetFloatVariable(const FString& Name, float DefaultValue) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil || ![Var.value isKindOfClass:[NSNumber class]]) return DefaultValue;
	return [(NSNumber*)Var.value floatValue];
}
double UIOSCleverTapInstance::GetDoubleVariable(const FString& Name, double DefaultValue) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil || ![Var.value isKindOfClass:[NSNumber class]]) return DefaultValue;
	return (double)[(NSNumber*)Var.value doubleValue];
}
bool UIOSCleverTapInstance::GetBoolVariable(const FString& Name, bool DefaultValue) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil || ![Var.value isKindOfClass:[NSNumber class]]) return DefaultValue;
	return [(NSNumber*)Var.value boolValue];
}
TMap<FString, FString> UIOSCleverTapInstance::GetStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil || ![Var.value isKindOfClass:[NSDictionary class]]) return DefaultValue;
	NSDictionary* NSDict = (NSDictionary*)Var.value;
	TMap<FString, FString> Result;
	for (NSString* Key in NSDict)
	{
		id Val = NSDict[Key];
		// Values defined via DefineStringMapVariable are always NSString; fall back to description for safety
		NSString* ValStr = [Val isKindOfClass:[NSString class]] ? (NSString*)Val : [Val description];
		FString ValFStr = (ValStr != nil) ? FString(ValStr) : TEXT("");
		Result.Add(FString(Key), ValFStr);
	}
	return Result;
}
FString UIOSCleverTapInstance::GetFileVariablePath(const FString& Name) const
{
	CTVar* Var = Variables[Name.GetNSString()];
	if (Var == nil) return TEXT("");
	NSString* Path = Var.fileValue;
	return (Path != nil) ? FString(Path) : TEXT("");
}

TArray<TMap<FString, FString>> UIOSCleverTapInstance::GetVariants()
{
	TArray<TMap<FString, FString>> Result;
	NSArray<NSDictionary<NSString*, id>*>* Variants = [NativeInstance variants];
	if (!Variants || Variants.count == 0) return Result;

	NSError* Error = nil;
	NSData* JsonData = [NSJSONSerialization dataWithJSONObject:Variants options:0 error:&Error];
	if (Error || !JsonData) return Result;

	NSString* JsonNSString = [[NSString alloc] initWithData:JsonData encoding:NSUTF8StringEncoding];
	if (!JsonNSString) return Result;

	FString JsonString = FString(JsonNSString);
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonArray)) return Result;

	for (const TSharedPtr<FJsonValue>& Element : JsonArray)
	{
		const TSharedPtr<FJsonObject>* ObjPtr;
		if (!Element->TryGetObject(ObjPtr)) continue;
		TMap<FString, FString> Entry;
		for (const auto& KV : (*ObjPtr)->Values)
		{
			FString StrVal;
			if (!KV.Value->TryGetString(StrVal))
				StrVal = KV.Value->AsString();
			Entry.Add(KV.Key, StrVal);
		}
		Result.Add(Entry);
	}
	return Result;
}
