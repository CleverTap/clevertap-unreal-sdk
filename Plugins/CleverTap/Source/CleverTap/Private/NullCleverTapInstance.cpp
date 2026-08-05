// Copyright CleverTap All Rights Reserved.
#include "NullCleverTapInstance.h"

#include "CleverTapLog.h"
#include "CleverTapSubsystem.h"
#include "CleverTapUtilities.h"

#include "UnrealEngine.h"

UNullCleverTapInstance* UNullCleverTapInstance::Create()
{
	UNullCleverTapInstance* Instance =
		NewObject<UNullCleverTapInstance>(GEngine->GetEngineSubsystem<UCleverTapSubsystem>());
	return Instance;
}

FString UNullCleverTapInstance::GetCleverTapId()
{
	return FString{};
}

void UNullCleverTapInstance::OnUserLogin(const FCleverTapProperties& Profile)
{
	CleverTapSDK::Ignore(Profile);
}

void UNullCleverTapInstance::OnUserLoginWithCleverTapId(const FCleverTapProperties& Profile, const FString& CleverTapId)
{
	CleverTapSDK::Ignore(Profile, CleverTapId);
}

void UNullCleverTapInstance::PushProfile(const FCleverTapProperties& Profile)
{
	CleverTapSDK::Ignore(Profile);
}

void UNullCleverTapInstance::PushEvent(const FString& EventName)
{
	CleverTapSDK::Ignore(EventName);
}

void UNullCleverTapInstance::PushEventWithProperties(const FString& EventName, const FCleverTapProperties& Actions)
{
	CleverTapSDK::Ignore(EventName, Actions);
}

void UNullCleverTapInstance::PushChargedEvent(
	const FCleverTapProperties& ChargeDetails, const TArray<FCleverTapProperties>& Items)
{
	CleverTapSDK::Ignore(ChargeDetails, Items);
}

TOptional<FCleverTapPropertyValue> UNullCleverTapInstance::GetProperty(const FString& Key)
{
	CleverTapSDK::Ignore(Key);
	return {};
}

void UNullCleverTapInstance::DecrementIntValue(const FString& Key, int Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void UNullCleverTapInstance::DecrementFloatValue(const FString& Key, float Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void UNullCleverTapInstance::IncrementIntValue(const FString& Key, int Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void UNullCleverTapInstance::IncrementFloatValue(const FString& Key, float Amount)
{
	CleverTapSDK::Ignore(Key, Amount);
}

void UNullCleverTapInstance::AddMultiValueForKey(const FString& Key, const FString& Value)
{
	CleverTapSDK::Ignore(Key, Value);
}

void UNullCleverTapInstance::AddMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
{
	CleverTapSDK::Ignore(Key, Values);
}

void UNullCleverTapInstance::RemoveMultiValueForKey(const FString& Key, const FString& Value)
{
	CleverTapSDK::Ignore(Key, Value);
}

void UNullCleverTapInstance::RemoveMultiValuesForKey(const FString& Key, const TArray<FString>& Values)
{
	CleverTapSDK::Ignore(Key, Values);
}

void UNullCleverTapInstance::RemoveValueForKey(const FString& Key)
{
	CleverTapSDK::Ignore(Key);
}

void UNullCleverTapInstance::SetMultiValuesForKey(const FString& Key, const TArray<FString> Values)
{
	CleverTapSDK::Ignore(Key, Values);
}

ECleverTapPushPermissionStatus UNullCleverTapInstance::GetPushPermissionStatus()
{
	return ECleverTapPushPermissionStatus::NotGranted;
}

void UNullCleverTapInstance::PromptForPushPermission(bool bFallbackToSettings)
{
	CleverTapSDK::Ignore(bFallbackToSettings);
}

void UNullCleverTapInstance::PromptForPushPermissionWithAlertPrimer(
	const FCleverTapPushPrimerAlertConfig& PushPrimerAlertConfig)
{
	CleverTapSDK::Ignore(PushPrimerAlertConfig);
}

void UNullCleverTapInstance::PromptForPushPermissionWithHalfInterstitialPrimer(
	const FCleverTapPushPrimerHalfInterstitialConfig& PushPrimerHalfInterstitialConfig)
{
	CleverTapSDK::Ignore(PushPrimerHalfInterstitialConfig);
}

void UNullCleverTapInstance::EnableOnPushNotificationClicked() {}

void UNullCleverTapInstance::EnableOnOpenUrl() {}

bool UNullCleverTapInstance::LocalizeAndroidNotificationChannel(
	const FString& ChannelID, const FText& ChannelName, const FText& ChannelDescription)
{
	CleverTapSDK::Ignore(ChannelID, ChannelName, ChannelDescription);
	return false;
}

bool UNullCleverTapInstance::LocalizeAndroidNotificationChannelGroup(const FString& GroupID, const FText& GroupName)
{
	CleverTapSDK::Ignore(GroupID, GroupName);
	return false;
}

void UNullCleverTapInstance::RegisterCleverTapUrlHandler(TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler)
{
	CleverTapSDK::Ignore(UrlHandler);
}

void UNullCleverTapInstance::RegisterInAppNotificationFilter(TUniqueFunction<bool(const FCleverTapProperties&)> Filter)
{
	CleverTapSDK::Ignore(Filter);
}

void UNullCleverTapInstance::DiscardInAppNotifications() {}
void UNullCleverTapInstance::ResumeInAppNotifications() {}
void UNullCleverTapInstance::SuspendInAppNotifications() {}

void UNullCleverTapInstance::SetOffline(bool bIsOffline)
{
	CleverTapSDK::Ignore(bIsOffline);
}

void UNullCleverTapInstance::SetOptOut(bool bIsOptingOut)
{
	CleverTapSDK::Ignore(bIsOptingOut);
}

void UNullCleverTapInstance::SetNetworkInformationRecording(bool bEnableCollection)
{
	CleverTapSDK::Ignore(bEnableCollection);
}

void UNullCleverTapInstance::DefineStringVariable(const FString& Name, const FString& DefaultValue)
	{ CleverTapSDK::Ignore(Name, DefaultValue); }

void UNullCleverTapInstance::DefineIntVariable(const FString& Name, int32 DefaultValue)
	{ CleverTapSDK::Ignore(Name, DefaultValue); }

void UNullCleverTapInstance::DefineInt64Variable(const FString& Name, int64 DefaultValue)
	{ CleverTapSDK::Ignore(Name, DefaultValue); }

void UNullCleverTapInstance::DefineFloatVariable(const FString& Name, float DefaultValue)
	{ CleverTapSDK::Ignore(Name, DefaultValue); }

void UNullCleverTapInstance::DefineDoubleVariable(const FString& Name, double DefaultValue)
	{ CleverTapSDK::Ignore(Name, DefaultValue); }

void UNullCleverTapInstance::DefineBoolVariable(const FString& Name, bool DefaultValue)
	{ CleverTapSDK::Ignore(Name, DefaultValue); }

void UNullCleverTapInstance::DefineStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue)
	{ CleverTapSDK::Ignore(Name, DefaultValue); }

void UNullCleverTapInstance::DefineFileVariable(const FString& Name)
	{ CleverTapSDK::Ignore(Name); }

void UNullCleverTapInstance::FetchVariables()
{
	// Immediately complete with failure so callers don't wait indefinitely on the Editor/null platform
	OnVariablesFetched.Broadcast(false);
}

void UNullCleverTapInstance::SyncVariables() {}

FString UNullCleverTapInstance::GetStringVariable(const FString& Name, const FString& DefaultValue) const
	{ CleverTapSDK::Ignore(Name); return DefaultValue; }

int32 UNullCleverTapInstance::GetIntVariable(const FString& Name, int32 DefaultValue) const
	{ CleverTapSDK::Ignore(Name); return DefaultValue; }

int64 UNullCleverTapInstance::GetInt64Variable(const FString& Name, int64 DefaultValue) const
	{ CleverTapSDK::Ignore(Name); return DefaultValue; }

float UNullCleverTapInstance::GetFloatVariable(const FString& Name, float DefaultValue) const
	{ CleverTapSDK::Ignore(Name); return DefaultValue; }

double UNullCleverTapInstance::GetDoubleVariable(const FString& Name, double DefaultValue) const
	{ CleverTapSDK::Ignore(Name); return DefaultValue; }

bool UNullCleverTapInstance::GetBoolVariable(const FString& Name, bool DefaultValue) const
	{ CleverTapSDK::Ignore(Name); return DefaultValue; }

TMap<FString, FString> UNullCleverTapInstance::GetStringMapVariable(const FString& Name, const TMap<FString, FString>& DefaultValue) const
	{ CleverTapSDK::Ignore(Name); return DefaultValue; }

FString UNullCleverTapInstance::GetFileVariablePath(const FString& Name) const
	{ CleverTapSDK::Ignore(Name); return TEXT(""); }

TArray<TMap<FString, FString>> UNullCleverTapInstance::GetVariants()
	{ return {}; }
