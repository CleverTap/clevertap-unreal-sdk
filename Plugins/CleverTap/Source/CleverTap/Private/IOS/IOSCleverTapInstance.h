// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapInstance.h"
#include "IOSCleverTapInstance.generated.h"

@class CleverTapSDKListener;
@class CleverTap;
@class NSDictionary;
@class NSString;
@class NSURL;
@class UIApplication;

/**
 * The IOS implementation of UCleverTapInstance
 */
UCLASS(NotBlueprintable)
class CLEVERTAP_API UIOSCleverTapInstance : public UCleverTapInstance {
  ~UIOSCleverTapInstance();

public:
  GENERATED_BODY()

  static UIOSCleverTapInstance* CreateFromNativeInstance(CleverTap *NativeInstance);

  static void HandleWillPresentNotification(NSDictionary *UserInfo);
  static void HandleDidReceiveNotificationResponse(NSDictionary *UserInfo);

  void CachePushPermissionStatus(bool bIsGranted);
  void HandleInAppNotificationButtonClicked(
      const FCleverTapProperties &ButtonProperties);
  void
  HandleInAppNotificationDismissed(const FCleverTapProperties &Extras,
                                   const FCleverTapProperties &ActionExtras);
  void HandleInAppNotificationShown(const FCleverTapProperties &Notification);
  void HandlePushNotificationTapped(const FCleverTapProperties &Extras);
  bool HandleUrl(FString Url, ECleverTapChannel Channel) const;
  void SetPushToken(const TArray<uint8> &Token);
  bool ShouldShowInAppNotification(const FCleverTapProperties &Extras) const;

  // <UCleverTapInstance>
  FString GetCleverTapId() override;

  void OnUserLogin(const FCleverTapProperties &Profile) override;
  void OnUserLoginWithCleverTapId(const FCleverTapProperties &Profile,
                                  const FString &CleverTapId) override;

  void PushProfile(const FCleverTapProperties &Profile) override;
  TOptional<FCleverTapPropertyValue> GetProperty(const FString &Key) override;
  void DecrementIntValue(const FString &Key, int Amount) override;
  void DecrementFloatValue(const FString &Key, float Amount) override;
  void IncrementIntValue(const FString &Key, int Amount) override;
  void IncrementFloatValue(const FString &Key, float Amount) override;
  void AddMultiValueForKey(const FString &Key, const FString &Value) override;
  void AddMultiValuesForKey(const FString &Key,
                            const TArray<FString> &Values) override;
  void RemoveMultiValueForKey(const FString &Key,
                              const FString &Value) override;
  void RemoveMultiValuesForKey(const FString &Key,
                               const TArray<FString> &Values) override;
  void RemoveValueForKey(const FString &Key) override;
  void SetMultiValuesForKey(const FString &Key,
                            const TArray<FString> Values) override;

  void PushEvent(const FString &EventName) override;
  void
  PushEventWithProperties(const FString &EventName,
                          const FCleverTapProperties &EventProperties) override;
  void PushChargedEvent(const FCleverTapProperties &ChargeDetails,
                        const TArray<FCleverTapProperties> &Items) override;

  ECleverTapPushPermissionStatus GetPushPermissionStatus() override;
  void PromptForPushPermission(bool bFallbackToSettings) override;
  void PromptForPushPermissionWithAlertPrimer(
      const FCleverTapPushPrimerAlertConfig &PushPrimerAlertConfig) override;
  void PromptForPushPermissionWithHalfInterstitialPrimer(
      const FCleverTapPushPrimerHalfInterstitialConfig
          &PushPrimerHalfInterstitialConfig) override;
  void EnableOnPushNotificationClicked() override;

  void EnableOnOpenUrl() override;
  void RegisterCleverTapUrlHandler(
      TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler) override;

  void RegisterInAppNotificationFilter(
      TUniqueFunction<bool(const FCleverTapProperties &)> Filter) override;
  void DiscardInAppNotifications() override;
  void ResumeInAppNotifications() override;
  void SuspendInAppNotifications() override;

  void SetOffline(bool bIsOffline) override;
  void SetOptOut(bool bIsOptingOut) override;
  void SetNetworkInformationRecording(bool bEnableCollection) override;

  bool
  LocalizeAndroidNotificationChannel(const FString &ChannelID,
                                     const FText &ChannelName,
                                     const FText &ChannelDescription) override;
  bool LocalizeAndroidNotificationChannelGroup(const FString &GroupID,
                                               const FText &GroupName) override;
  // </UCleverTapInstance>

private:
  bool IsRegisteredForPushNotificationClicked() const;
  void SetIsRegisteredForPushNotificationClicked();
  bool IsRegisteredForDeepLinkHandler() const;
  void SetIsRegisteredForDeepLinkHandler();
  void HandleOnOpenURL(UIApplication *App, NSURL *URL, NSString *Source,
                       id Annotation);

private:
  mutable FCriticalSection CriticalSection;
  CleverTap *NativeInstance{};
  CleverTapSDKListener *SDKListener{};
  FDelegateHandle OnURLOpenHandle;
  TUniqueFunction<bool(FString, ECleverTapChannel)> UrlHandler;
  TUniqueFunction<bool(const FCleverTapProperties &)> InAppNotificationFilter;
  TAtomic<uint8> PushPermissionStatus;
  uint8 StateFlags{};

  static TArray<UIOSCleverTapInstance *> AllInstances;
};
