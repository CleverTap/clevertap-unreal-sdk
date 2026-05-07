# Changelog
==========

## [1.1.0] - 2026-05-04

### Fixed — iOS push notifications
- **Push permission dialog never appeared**: `PromptForPushPermission` was calling `registerForRemoteNotifications` without first calling `requestAuthorizationWithOptions:`. iOS requires explicit authorization before registration; without it the call is a silent no-op. Now correctly requests authorization and only registers once the user grants permission.
- **Push permission status always showed UNKNOWN**: `GetPushPermissionStatus` was initialized to `Unknown` at startup and never queried the real iOS state. Now queries `getNotificationSettingsWithCompletionHandler:` at init so the returned status is accurate immediately.
- **APNs token never reached CleverTap (dashboard showed "Unsubscribed")**: The plugin had a `SetPushToken` implementation but nothing ever invoked it. Added a swizzle on `application:didRegisterForRemoteNotificationsWithDeviceToken:` in the AppDelegate — the same interception pattern used for other push callbacks — to forward the token directly to CleverTap. Also handles the case where UE 5.x does not implement this method by injecting it outright.
- **Race condition on reinstalls**: On reinstalls where permission was already granted, iOS delivers the APNs token at startup before CleverTap had subscribed to receive it, silently losing the token. Fixed by re-calling `registerForRemoteNotifications` after subscribing, forcing iOS to re-deliver the current token.
- **Silent registration failures**: Added swizzle on `application:didFailToRegisterForRemoteNotificationsWithError:` so any APNs registration failure is logged clearly instead of disappearing silently.
- **Notification swizzles failing when delegate not yet set**: `EnsurePushNotificationMonitoring` read `UNUserNotificationCenter.delegate` to determine which class to swizzle. If the delegate was nil at init time, the `willPresentNotification` and `didReceiveNotificationResponse` swizzles would silently fail. Now falls back to `AppDelegateClass` when nil.

### Added — iOS
- `bIOSAutoRequestPushPermission` config flag (default `false`): when enabled, push notification permission is requested automatically on first launch without requiring an explicit `PromptForPushPermission` call.

### Updated
- Android CleverTap SDK upgraded from 7.4.0 to **8.0.0**
- iOS CleverTap SDK upgraded from 7.2.0 to **7.6.0**

### Internal changes
- Updated temporary local AAR workaround — Android SDK now consumed via Maven Central by default and aar for the htmlBanners
- Plugin version bumped to 1.1.0

## [1.0.0] - 2025-07-17

### Added
- ProGuard/R8 rules support for release builds
- Local AAR override option via `AndroidLocalCleverTapSdkAarPath` for custom SDK builds
- Blueprint support for all CleverTap APIs
- Initial Unreal Engine 5 support
- Custom HTML header & footer in-app notifications on Android

## [0.0.3] - 2025-05-27

### Added
- In-app notification support (iOS and Android)
- `SuspendInAppNotifications`, `ResumeInAppNotifications`, `DiscardInAppNotifications`
- In-app notification delegates and button click callbacks

## [0.0.2] - 2025-05-12

### Added
- Push notification integration for iOS (APNs) and Android (FCM)
- `PromptForPushPermission`, `IsPushPermissionGranted`, `PromptPushPrimer`
- Push notification click callbacks
- iOS engine patch for full notification support when app is closed (`iOSSavedRemoteNotifications.patch`)

## [0.0.1] - 2025-04-10 (initial release)

### Added
- Android CleverTap SDK 7.4.0 (local AAR) and iOS CleverTap SDK 7.2.0
- User profile management (`OnUserLogin`, `PushProfile`, `GetCleverTapID`)
- Event recording (`PushEvent`, `PushChargedEvent`)
- Multi-value profile properties (add, remove, set)
- Increment / decrement numeric profile values
- Offline mode and opt-out controls
- Deep link handling
