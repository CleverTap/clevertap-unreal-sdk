# Changelog
==========

## [1.3.0] - 2026-08-12

### Updated
- Android CleverTap SDK upgraded from **8.0.0** to **8.4.1** (local custom-patched AAR replaced with Maven Central release)
- iOS CleverTap SDK upgraded from **7.6.0** to **7.8.1** (framework built from source and bundled)

### Added
- `Unmute()` — resumes event recording and network traffic after the SDK has been muted. Android-only; no-op on iOS and Null/Editor. Added in Android SDK 8.1.0.
- `DismissPipInApp()` — dismisses the currently visible Picture-in-Picture (PIP) in-app notification, freeing the display slot for the next queued in-app. Full implementation on Android (SDK 8.4.1) and iOS (SDK 7.8.1); no-op on Null/Editor.
- `RecordDisplayUnitClickedEventForID(UnitID)` — no-op placeholder for API parity with Android SDK 8.4.1 / iOS SDK 7.8.1. Display Units not yet implemented in the Unreal plugin.
- `PauseSDK()` / `ResumeSDK()` — gaming-context aliases for `SetOffline(true/false)`. Call `PauseSDK()` at the start of latency-sensitive gameplay sequences (boss fights, cutscenes) to prevent SDK network calls from causing frame stutters; call `ResumeSDK()` when the sequence ends to flush queued events. Exposed as Blueprint nodes under `CleverTap|Gaming`.
- `GetVariants()` — returns the active A/B test variants currently assigned to the user as `TArray<TMap<FString,FString>>`. Each entry is a property dictionary for one variant (e.g. `"id"`, `"name"`). Returns an empty array if no variants are active. C++ only (TArray<TMap> is not Blueprint-serializable).

### Fixed — Android
- **Removed local AAR workaround**: The custom-patched `clevertap-android-sdk-8.4.0.aar` has been deleted. The SDK is now consumed from Maven Central (`com.clevertap.android:clevertap-android-sdk:8.4.1`), which includes the fragmentless banner support natively. The `AndroidLocalCleverTapSdkAarPath` config key is still supported for overriding to a local AAR if needed.
- **Fragment-based in-app notifications crashing on `GameActivity`**: Unreal Engine's `GameActivity` does not extend `AppCompatActivity`, causing fragment-based in-app renderers (header, footer) to throw `ClassCastException` at display time. Added `CLEVERTAP_INAPP_FRAGMENTLESS_BANNERS=1` metadata to `AndroidManifest.xml` via UPL to enable the SDK's fragment-free rendering path. Note: campaigns must use **HTML** banner type (not Native) for this path to apply.
- **FileProvider registration**: Added `androidx.core.content.FileProvider` declaration to `AndroidManifest.xml` via UPL, along with a `clevertap_file_provider_paths.xml` resource. Required for sharing private-storage file paths (e.g. PE file variables) on Android 7+.

### Fixed — Sample App
- **Login not persisted across restarts on Android**: `SaveGameToSlot` silently failed on Android 11+ due to scoped storage blocking writes outside the app's `files/` directory. Fixed by bypassing UE's `ISaveGameSystem` — save/load/delete now use `FPlatformMisc::GamePersistentDownloadDir()` (maps to `getExternalFilesDir()`) with `FFileHelper` for direct file I/O.
- **"Delete Login Info" not logging out**: `DeleteSaveState()` deleted the save file but `SyncSaveStateToViewModels` (bound to `OnViewModelChanged`) immediately re-created it when ViewModel changes fired during UI teardown. Fixed by adding `EndDisplay_MainMenu()` which unbinds the delegate before teardown, and calling `SetUIState(Login)` from `DeleteSaveState()` to navigate back to the Login screen.
- **SaveState null crash on first launch**: `CppDemonstrationHUD` now correctly handles a null result from `LoadGameFromSlot` (first launch, no save file) and falls back to the Login state instead of dereferencing a null pointer.
- **PE tab: file variable preview**: Added `OpenPEFileVariable()` to `CleverTapSampleBlueprintFunctionLibrary`. Opens a PE file variable using the OS viewer (QuickLook on iOS, FileProvider intent on Android).

### Internal
- Android: Media3 / ExoPlayer dependency block added as commented-out opt-in. Uncomment in `CleverTap_Android_UPL.xml` if PIP video in-app notifications are needed.

## [1.2.0] - 2026-06-08

### Added — Product Experiences (PE) Variables
Full PE Variables support on Android and iOS, matching the CleverTap Unity SDK's variable type coverage.

**Supported variable types:**
boolean
byte
short
int
long
float
double
String
File
Dictionary

### Fixed — iOS
- `enablePersonalization` is now called during SDK initialisation on iOS. This is required for PE Variables (and the Personalization API in general) to function — without it, variable definitions and fetch calls are silently ignored.

### Internal
- Android: added `androidx.lifecycle:lifecycle-runtime-ktx:2.6.2` and `kotlinx-coroutines-android:1.7.3` to the local-AAR dependency block. SDK 8.0.0 introduced `InAppTimerManager.kt` which uses Kotlin coroutines with lifecycle scope; Gradle does not resolve transitive dependencies for local AARs so these must be declared explicitly. `kotlin-stdlib` is intentionally omitted to prevent DEX method-count overflow (the Unreal build chain already bundles it).

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
