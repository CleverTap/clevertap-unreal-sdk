# Plugin Integration

## Getting the Plugin

### Option A — Download from GitHub Releases (Recommended)
1. Go to the [Releases page](https://github.com/CleverTap/clevertap-unreal-sdk/releases) and download `CleverTap-UnrealPlugin-v*.*.*.zip`.
2. Extract the zip — it contains a single `CleverTap` folder.
3. Copy the `CleverTap` folder into your Unreal project's `Plugins/` directory (create it if it doesn't exist).

### Option B — Clone the Repository
Clone the sample application repository into a temporary folder to get access to the CleverTap Unreal plugin.
```bash
git clone --depth 1 https://github.com/CleverTap/clevertap-unreal-sdk.git
```
Copy the `clevertap-unreal-sdk\Plugins\CleverTap` folder to your Unreal project's `Plugins/` directory.
## Integration
### Generic Integration
Add the following to the `Plugins` section of your Unreal project's `.uproject` file.
```json
{
	"Plugins": [
		{
			"Name": "CleverTap",
			"Enabled": true
		}
	]
}
```
### C++ Integration
Add the following to the C++ gameplay module's `PrivateDependencyModuleNames` property in each `*.Target.cs` file's class constructor.
```csharp
PrivateDependencyModuleNames.Add("CleverTap");
```
If you use any of the public CleverTap types in your module's public types then you should add it to the `PublicDependencyModuleNames` property instead.
## Configuration
In your project's `Config\DefaultEngine.ini` file add the following block at the end.
```ini
[/Script/CleverTap.CleverTapConfig]
ProjectId= ;Add your project Id here. This is found on your project's CleverTap dashboard
ProjectToken= ;Add your project token here. This is found on your project's CleverTap dashboard
RegionCode= ;Reference https://developer.clevertap.com/docs/idc#ios to determine what region code string to put here
```
Alternatively you can edit the Plugin's settings in the UE4Editor.

## Initialization
By default, with `bAutoInitializeSharedInstance` set to `true` in `Config\DefaultEngine.ini`, the `UCleverTapSubsystem`
will automatically initialize the CleverTap SDK and the default shared instance. It is recommended not to set this to
`false` as it is only currently partially supported on iOS.

## Encryption of PII Data
PII data is stored across the SDK and could be sensitive information. 
You can enable encryption for PII data such as Email, Identity, Name, and Phone.
Currently, two levels of encryption are supported:
- None: All stored data is in plaintext (the default)
- Medium: PII data is encrypted completely

The only way to set the encryption level for the default instance is from `Config/DefaultEngine.ini`:
```ini
[/Script/CleverTap.CleverTapConfig]
EncryptionLevel=Medium
```

Different instances can have different encryption levels. To set an encryption level for an additional instance, set the `EncryptionLevel` field in the `FCleverTapInstanceConfig` used to create it.


## Push Notification Configuration
CleverTap allows you to send push notifications to your applications from our dashboard, once the user has granted permission in response to a call to `PromptForPushPermission()`. 

Each platform requires slightly different setup.

### Android - Configure Firebase Cloud Messaging (FCM)
To use the default CleverTap notification implementation with Firebase:

1. Follow [these instructions](https://developer.clevertap.com/docs/android-push) to create and register your firebase credentials in the CleverTap dashboard.

2. Copy the generated `google-services.json` to somewhere in your project directory (e.g. `Config`).

3. In your project's `Config/DefaultEngine.ini` ensure `bAndroidIntegrateFirebase` is `True` and `AndroidGoogleServicesJsonPath` is the project-relative path to where you copied `google-services.json`. 

```ini
[/Script/CleverTap.CleverTapConfig]
bAndroidIntegrateFirebase=True
AndroidGoogleServicesJsonPath=Config/Android/google-services.json
```

### Android - Configure Push Notification Channels 

Android requires push notifications to be delivered via pre-defined notification channels. These channels let users customize how different types of notifications behave (e.g., sound, vibration, visibility).

Because channels must be registered during Java's `GameApplication.onCreate()` - before Unreal Engine has initialized - they cannot be created dynamically from C++. Instead, they must be preconfigured in your project’s `Config/DefaultEngine.ini`.

#### Basic Setup 
Define up to 100 channels in your DefaultEngine.ini. Only the ID field is required - this is the unique identifier for the channel.

```ini
[/Script/CleverTap.CleverTapConfig]
AndroidNotificationChannelSlot1=ID="general" | Name="General" | Description="General Notifications" | Importance=IMPORTANCE_DEFAULT | bShowBadge=True
AndroidNotificationChannelSlot2=ID="news" | Name="News Updates" | Description="Important news and alerts" | Importance=IMPORTANCE_HIGH | bShowBadge=True | Sound="news_alert.wav"
```
Valid Importance values: `IMPORTANCE_NONE`, `IMPORTANCE_MIN`, `IMPORTANCE_LOW`, `IMPORTANCE_DEFAULT`, `IMPORTANCE_HIGH`, `IMPORTANCE_MAX`.

The sound files must be included in the APK's `res/raw`; see the `AndroidSoundsDir` setting for more information.


> [!NOTE]
> - The plugin’s `CleverTap_Android_UPL.xml` reads these entries and injects the required Java into `GameApplication.onCreate()`. Errors in the `ini` syntax will cause the compilation of `GameApplication.java` to fail.
> - The `AndroidNotificationChannelSlot` settings can only be edited directly in your project’s `Config/DefaultEngine.ini`; they are **not** available in the `Project Settings` GUI.

#### Channel Localization 
You can set the channel’s display name and description at runtime using Unreal's localization system. If you use runtime localization, there's no need to specify `Name` or `Description` in the `.ini`.

```ini
[/Script/CleverTap.CleverTapConfig]
AndroidNotificationChannelSlot1=ID="general" | Importance=IMPORTANCE_DEFAULT | bShowBadge=True
AndroidNotificationChannelSlot2=ID="news" | Importance=IMPORTANCE_HIGH | bShowBadge=True
```

##### C++
```c++
CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
CleverTap.LocalizeAndroidNotificationChannel(TEXT("general"),
        NSLOCTEXT("CleverTapSample", "ChannelName_general", "General"),
        NSLOCTEXT("CleverTapSample", "ChannelDesc_general", "General Notifications"));
```

##### Blueprint
![Calling LocalizeAndroidNotificationChannel in Blueprint](/Docs/Images/BP_LocalizeAndroidNotificationChannel.png)

#### Channel Grouping 
You can define up to 10 notification channel groups to categorize related channels.
Each group is declared in your `.ini`, and channels can reference them using `Group="group_id"`.

```ini
[/Script/CleverTap.CleverTapConfig]
AndroidNotificationChannelGroupSlot1=ID="general" | Name="General"
AndroidNotificationChannelSlot1=ID="general" | Group="general" | Importance=IMPORTANCE_DEFAULT | bShowBadge=True
```

Group names can be localized at runtime, just like channels.
Call `LocalizeAndroidNotificationChannelGroup()` early in startup and again after locale changes.

##### C++
```c++
CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
CleverTap.LocalizeAndroidNotificationChannelGroup( TEXT("general"), 
        NSLOCTEXT("CleverTapSample", "ChannelGroupName_general", "General"));
```

##### Blueprint
![Calling LocalizeAndroidNotificationChannelGroup in Blueprint](/Docs/Images/BP_LocalizeAndroidNotificationChannelGroup.png)

### Default Android Notification Channel
You can define a specific notification channel that CleverTap will use if the channel provided in the push payload is not registered by your app. This ensures that push notifications are displayed consistently even if the app's notification channels are not set up.

In case the SDK does not find the default channel ID specified in the manifest, it will automatically fall back to using a default channel called `Miscellaneous`. This ensures that push notifications are still delivered, even if no specific default channel is specified in the manifest.

To specify your app’s preferred default channel:
```ini
[/Script/CleverTap.CleverTapConfig]
AndroidDefaultNotificationChannel=general
```

### Android Small Notification Icon
By default, our SDK uses the app's icon for both the notification icon and the notification bar icon; however, since Android 5, all non-alpha channels are ignored while drawing the main notification icon. 

You can supply the project-relative path to an alpha-only png to use instead:
```ini
[/Script/CleverTap.CleverTapConfig]
AndroidSmallNotificationIconPath=Config/Android/sample_small_notification_icon.png
```

Note that the base filename must contain only lowercase letters (`a`-`z`), digits (`0`-`9`), or underscores (`_`).

For more information about the image requirements, see https://developer.clevertap.com/docs/android-push#set-the-small-notification-icon

### Android Sound and Image Resources
Additional Images and Sounds can be included in the APK for direct use by Android & CleverTap.

These are not part of Unreal's regular asset system. They are copied directly into the APK and must follow Android resource rules. Filenames must contain only lowercase letters (`a`-`z`), digits (`0`-`9`), or underscores (`_`).

Android supports .mp3, .ogg, and .wav files for playing custom sounds.

```ini
[/Script/CleverTap.CleverTapConfig]
AndroidImagesDir=Config/Android/Images
AndroidSoundsDir=Config/Android/Sounds
```

### Custom Android Notification Handling
Due to Android’s restriction of allowing only one `FirebaseMessagingService`, it cannot coexist cleanly with other Unreal plugins that declare their own FCM service (e.g. the Unreal Firebase plugin).

If you’re already using another Firebase plugin and also require CleverTap push features, you’ll need to disable the CleverTap Firebase integration with `bAndroidIntegrateFirebase=False`, and replace the other plug-in’s FCM service with a custom multiplexer implemented in Java.

For example:
```java
public class UnifiedMessagingService extends SomeOtherPluginMessagingService {
    @Override
    public void onMessageReceived(RemoteMessage message) {
        if (isCleverTapMessage(message)) {
            new CTFcmMessageHandler().createNotification(getApplicationContext(), message);
        } else {
            // Let the base class handle it
            super.onMessageReceived(message);
        }
    }

    private boolean isCleverTapMessage(RemoteMessage message) {
        Map<String, String> data = message.getData();
        return data != null && data.containsKey("wzrk_pn"); // CleverTap magic key
    }
}
```

For more information, see [Custom Android Push Notification Handling](https://developer.clevertap.com/docs/android-push#custom-android-push-notification-handling) 


### Android OpenUrl Configuration
To handle incoming app links, enable `bIntegrateOpenUrl` and define up to four slot-based URL filters in your project’s `.ini` file.

```ini
[/Script/CleverTap.CleverTapConfig]
bIntegrateOpenUrl=True

; Example Slot1 - catches clevertap-unreal-sample://
DeepLinkSchemeFilterSlot1=clevertap-unreal-sample 

; Example Slot2 - catches clevertap://unreal.com/sample
DeepLinkSchemeFilterSlot2=clevertap
AndroidIntentFilterSlot2_Host=unreal.com
AndroidIntentFilterSlot2_PathPrefix=/sample

; Example Slot3 - catches http://clevertap.com/unreal-sample, 
; but requires digital verification that the app is approved for this domain
; See https://developer.android.com/training/app-links/verify-android-applinks
DeepLinkSchemeFilterSlot3=http
AndroidIntentFilterSlot3_Host=clevertap.com
AndroidIntentFilterSlot3_PathPrefix=/unreal-sample
AndroidIntentFilterSlot3_AutoVerify=True
```

> [!NOTE] Http/Https schemes require digital verification via `assetlinks.json` hosted on the target domain.
> See: https://developer.android.com/training/app-links/verify-android-applinks

You can simulate clicking on a deep link with `adb`:
```
adb shell am start -a android.intent.action.VIEW -d "clevertap-unreal-sample://test/path"
```

Disable OpenUrlActivity integration if you need full control over your app’s deep link handling, and plan to define your own activity with complex intent filters via custom UPL rules. 
If your custom activity forwards the intent to the `GameActivity`, it will still be routed to the `OnOpenURL` delegate.

### iOS OpenUrl Configuration
To handle incoming app links, enable `bIntegrateOpenUrl` and define up to four slot-based URL scheme filters in your project’s `.ini` file.

```ini
[/Script/CleverTap.CleverTapConfig]
bIntegrateOpenUrl=True

; Example Slot1 - catches clevertap-unreal-sample://
DeepLinkSchemeFilterSlot1=clevertap-unreal-sample 

; Example Slot2 - catches clevertap://
DeepLinkSchemeFilterSlot2=clevertap

; Example Slot3 - catches http://
DeepLinkSchemeFilterSlot3=http
```
To further filter the allowed links use the `RegisterCleverTapUrlHandler()` method to register a filter or do custom
URL handling.

### iOS - Configuring Apple Push Notifications (APNs)
1. Follow the [CleverTap guide](https://developer.clevertap.com/docs/push-notifications-ios#step-1-configure-push-notifications) to set up APNs for your app.
2. In your project's `Config/DefaultEngine.ini` ensure `bEnableRemoteNotificationsSupport` is `True` in the `[/Script/IOSRuntimeSettings.IOSRuntimeSettings]` section.
3. If you would like push notifications to appear when your app is in the foreground make sure that `bIOSPresentPushNotificationsInForeground` is set to `True` in the `[/Script/CleverTap.CleverTapConfig]` section of your project's `Config/DefaultEngine.ini`.
4. By default, push permission must be requested manually via `PromptForPushPermission()`. To instead request permission automatically on first launch, set `bIOSAutoRequestPushPermission` to `True`:

```ini
[/Script/CleverTap.CleverTapConfig]
bIOSPresentPushNotificationsInForeground=True
bIOSAutoRequestPushPermission=True
```

#### iOS - Optional Engine changes for Push Notifications that launch the App
On Windows, a patch program equivalent can be installed using winget.
```powershell
winget install GnuWin32.Patch
```

##### iOS Engine Patch - Push Notification Callback Invocation When App is Closed
If you're App isn't in the foreground or background then Unreal will not forward the push notification to CleverTap unless you make engine changes to `Engine/Source/Runtime/ApplicationCore/Private/IOS/IOSAppDelegate.cpp`.  An example of such changes can be found in the `EnginePatches/iOSSavedRemoteNotifications.patch` patch file. This can be applied in the Unreal root directory using the linux patch command.
```bash
UnrealEngine % patch -p1 -u -i /path/to/CleverTapSample/EnginePatches/iOSSavedRemoteNotifications.patch
```

##### iOS Engine Patch - Rich Push Notification Support
[Rich Push Notifications](https://developer.clevertap.com/docs/rich-push-notifications) require a [Notification Service
Extension](https://developer.apple.com/reference/usernotifications/unnotificationserviceextension). Unfortunately
Unreal Engine doesn't support app extensions without modification to its build process. An example patch that adds
support for app extensions can be found in the
[EnginePatches/UE4.27_ExtensionSupport.patch](EnginePatches/UE4.27_ExtensionSupport.patch) patch file. This can
be applied in the Unreal root directory using the linux patch command.
```bash
UnrealEngine % patch -p1 -u -i /path/to/CleverTapSample/EnginePatches/UE4.27_ExtensionSupport.patch
```

Once patched, the extension located at [Plugins/CleverTap/Source/ThirdParty/IOS/Extensions/CTNotificationService](Plugins/CleverTap/Source/ThirdParty/IOS/Extensions/CTNotificationService) will be included in the build. A custom signing provision can be specified for the extension via changes to the `Config/DefaultEngine.ini` file.
```ini
[/Script/IOSRuntimeSettings.IOSRuntimeSettings]
MobileProvision_CTNotificationService=YourProvisioning.mobileprovision
```
Please make sure the specified .mobileprovision file is installed before building.

> [!NOTE]
> This engine patch adds support for arbitrary app extensions located in `{ProjectFolder}/Build/IOS/Extensions` or any
> `{ProjectFolder}/Plugins/{PluginNameHere}/Source/ThirdParty/IOS/Extensions` folders. It only supports UE4.27 and has
> only been tested against the CTNotificationService extension provided here.

### Enabling OnPushNotificationClicked
After setting up your shared CleverTap instance the `OnPushNotificationClicked` delegate can be enabled by calling `EnableOnPushNotificationClicked()`. On iOS, if the engine patch has been applied for push notifications that launch the app then this can trigger attempting to broadcast the push notification that launched the app. Therefore a delegate listener should be added before calling `EnableOnPushNotificationClicked()`.

#### C++
```c++
CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
CleverTap.OnPushNotificationClicked.AddLambda([](const FCleverTapProperties& NotificationPayload)
	{
		UE_LOG(LogTemp, Log, TEXT("Push notification was tapped"));
	});
CleverTap.EnableOnPushNotificationClicked();
```

#### Blueprint
![Binding OnPushNotificationClicked in Blueprint](/Docs/Images/BP_OnPushNotificationClicked.png)

## In-App Notifications
In-app notifications are initially suspended. When your callbacks are set up then call
`ICleverTapInstance::ResumeInAppNotifications()` to begin receiving in-app notifications.

The payload parameters for all in-app notifications callbacks have their nested objects flattened as dot separated keys
in the property map so that the object `{"a": { "b": 3.14 }}` would have a single entry of
`Payload[FString{TEXT("a.b")}] == FCleverTapPropertyValue{ 3.14 }`. 

### Callbacks
#### OnInAppNotificationShown
When an in-app notification is displayed this callback will be invoked with and the notification payload is passed as a
parameter.

##### C++
```c++
CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
CleverTap.OnInAppNotificationShown.AddLambda([](const FCleverTapProperties& Payload)
    {
		UE_LOG(LogTemp, Log, TEXT("An in-app notification was shown: %s"), *ToDebugString(Payload));
    });
```

##### Blueprint
![Binding OnInAppNotificationShown in Blueprint](/Docs/Images/BP_OnInAppNotificationShown.png)

#### OnInAppNotificationButtonClicked
When the user clicks on an in-app notification button this callback will be invoked with the key/value pairs associated
with that button.

##### C++
```c++
CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
CleverTap.OnInAppNotificationButtonClicked.AddLambda([](const FCleverTapProperties& Payload)
    {
		UE_LOG(LogTemp, Log, TEXT("An in-app notification button clicked: %s"), *ToDebugString(Payload));
    });
```

##### Blueprint
![Binding OnInAppNotificationButtonClicked in Blueprint](/Docs/Images/BP_OnInAppNotificationButtonClicked.png)

#### OnInAppNotificationDismissed
When an in-app notification is dismissed by the user this callback will be invoked with the notification payload and any
extra key/value pairs specified for the in-app notification in the CleverTap dashboard.
```c++
CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
CleverTap.OnInAppNotificationDismissed.AddLambda([](const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras)
    {
		UE_LOG(LogTemp, Log, TEXT("An in-app notification was dismissed. Extras: %s, ActionExtras: %s"),
            *ToDebugString(Extras), *ToDebugString(ActionExtras));
    });
```

##### Blueprint
![Binding OnInAppNotificationDismissed in Blueprint](/Docs/Images/BP_OnInAppNotificationDismissed.png)

### Suspend, Resume, and Discard In-App Notifications
In-app notifications initially start suspended. If you like to discard any notifications that were queued while they
were suspended then you can invoke `ICleverTapInstance::DiscardInAppNotifications()` to do so. Notifications can be
resumed by calling `ICleverTapInstance::ResumeInAppNotifications()`. Any queued notification that happened while
suspended, and which has not been explicitly discarded, will be presented to the user. If you would like to suspend
notifications again then call `ICleverTapInstance::SuspendInAppNotifications()`.

#### Blueprint
![Calling SuspendInAppNotifications -> DiscardInAppNotifications -> ResumeInAppNotifications in Blueprint](/Docs/Images/BP_SuspendDiscardResume_InAppNotifications.png)

### Dismiss PIP In-App Notification

Picture-in-Picture (PIP) in-app notifications can appear as floating overlays while your game is running. If you need to programmatically dismiss a visible PIP in-app — for example, when starting a cutscene or a critical gameplay sequence — call `DismissPipInApp()`. This frees the in-app display slot, allowing the next queued in-app to appear once the slot is available again.

```cpp
ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
CleverTap.DismissPipInApp();
```

Calling `DismissPipInApp()` when no PIP in-app is currently visible is a no-op.

## Gaming Performance — PauseSDK / ResumeSDK

During latency-sensitive gameplay sequences (boss fights, loading screens, cutscenes), use `PauseSDK()` to stop CleverTap from uploading events. Events are still recorded locally while paused and will be uploaded when `ResumeSDK()` is called — no data is lost.

Both methods are exposed as Blueprint nodes under the `CleverTap|Gaming` category.

### C++

```cpp
ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

// Before a boss fight / cutscene / loading screen
CleverTap.PauseSDK();

// ... gameplay runs with no SDK network traffic ...

// When the sequence ends — queued events are flushed immediately
CleverTap.ResumeSDK();
```

`PauseSDK()` and `ResumeSDK()` are thin aliases for `SetOffline(true)` and `SetOffline(false)`.

## SDK Control — Unmute

`Unmute()` resumes event recording and network traffic after the CleverTap SDK has been muted server-side (via a dashboard mute operation). This is an Android-only feature; calling it on iOS or in the Editor is a no-op.

```cpp
ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
CleverTap.Unmute();
```

## Product Experiences (Variables)

Product Experiences (PE) Variables let you define named, typed variables in your game code and override their values remotely from the CleverTap dashboard — without a code change or resubmission. Typical uses include tuning gameplay parameters, running A/B tests, and toggling features.

### How it works

1. **Define** — register each variable with a name, type, and default value. Call at startup, before `FetchVariables`.
2. **Sync** (development only) — send definitions to the dashboard so operators can see and override them.
3. **Fetch** — pull the current server overrides. Bind `OnVariablesFetched` / `OnVariablesChanged` before calling this.
4. **Get** — read the effective value (server override if available, otherwise the code default).

### Quick Start (C++)

```cpp
UCleverTapInstance& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

// Step 1 — bind delegates before defining so you never miss a callback
CT.OnVariablesFetched.AddDynamic(this, &UMyClass::OnFetched);
CT.OnVariablesChanged.AddDynamic(this, &UMyClass::OnChanged);

// Step 2 — define variables (do this once at startup)
CT.DefineStringVariable(TEXT("welcome_msg"),  TEXT("Hello!"));
CT.DefineIntVariable(TEXT("spawn_count"),     5);
CT.DefineFloatVariable(TEXT("gravity_scale"), 1.0f);
CT.DefineBoolVariable(TEXT("new_ui_enabled"), false);
CT.DefineStringMapVariable(TEXT("theme"),     { {TEXT("bg"), TEXT("#FFFFFF")} });
CT.DefineFileVariable(TEXT("banner_image"));   // path populated after fetch

// Step 3 — sync definitions to dashboard (development builds only)
CT.SyncVariables();

// Step 4 — fetch server overrides (call at startup and/or on demand)
CT.FetchVariables();
```

```cpp
// Step 5 — read values inside OnVariablesChanged (or after OnVariablesFetched fires with bSuccess=true)
void UMyClass::OnChanged()
{
    FString Msg   = CT.GetStringVariable(TEXT("welcome_msg"),  TEXT("Hello!"));
    int32   Count = CT.GetIntVariable(TEXT("spawn_count"),     5);
    float   Grav  = CT.GetFloatVariable(TEXT("gravity_scale"), 1.0f);
    bool    NewUI = CT.GetBoolVariable(TEXT("new_ui_enabled"), false);
}
```

### Supported Variable Types

| Define method | Get method | C++ type |
|---|---|---|
| `DefineStringVariable` | `GetStringVariable` | `FString` |
| `DefineIntVariable` | `GetIntVariable` | `int32` |
| `DefineInt64Variable` | `GetInt64Variable` | `int64` |
| `DefineFloatVariable` | `GetFloatVariable` | `float` |
| `DefineDoubleVariable` | `GetDoubleVariable` | `double` |
| `DefineBoolVariable` | `GetBoolVariable` | `bool` |
| `DefineStringMapVariable` | `GetStringMapVariable` | `TMap<FString,FString>` |
| `DefineFileVariable` | `GetFileVariablePath` | file path (`FString`) |
| `DefineShortVariable` *(convenience)* | `GetIntVariable` | `int32` — clamped to [-32768, 32767] |
| `DefineByteVariable` *(convenience)* | `GetIntVariable` | `int32` — clamped to [0, 255] |


### Delegates

| Delegate | Fires when |
|---|---|
| `OnVariablesFetched(bool bSuccess)` | The `FetchVariables()` HTTP call completes. `bSuccess=false` means timeout or error — local defaults remain. |
| `OnVariablesChanged` | One or more variable values changed (after a successful fetch or server push). |

Always bind delegates **before** calling `FetchVariables()` to avoid missing a callback if values are resolved from cache.

### SyncVariables — requirements

`SyncVariables()` is a **development-only tool**. Two conditions must be met for it to work:

1. **Development build** — both Android and iOS enforce this at runtime. Shipping builds silently no-op.
2. **Test Profile enabled** on the CleverTap dashboard — the server uses this as a backend gate to accept sync requests.

`FetchVariables()` has **no such restrictions** and works in all build configurations including Shipping.

### Notes

- Variables are **immutable after first define** — calling `DefineIntVariable("x", 99)` after `DefineIntVariable("x", 10)` is a no-op. The default is fixed at the first registration.
- `SyncVariables()` sends the definitions to the dashboard. It does **not** clear or override existing server-side values; those must be reset from the dashboard (Product Experiences → Variables).
- **Float precision:** float variables are stored as `double` in the SDK internally. `GetFloatVariable()` casts back to `float` at the JNI boundary (Android), which can introduce minor IEEE 754 noise (e.g. `67.3 → 67.300003f`). Use `GetDoubleVariable()` if you need full precision.
- **File variables:** `GetFileVariablePath()` returns an empty string until `FetchVariables()` has completed and the file has been downloaded. Always check for empty before use.
- **Variable naming:** use unique prefixes to avoid collisions with variables already defined on the same CleverTap account from other sessions or platforms.

For the full reference including all API signatures, dashboard workflow, and common issues, see [Docs/ProductExperiencesVariables.md](Docs/ProductExperiencesVariables.md).

### GetVariants

`GetVariants()` returns the A/B test variants currently active for the user. Each entry in the returned array is a property map for one variant (typically containing keys like `"id"` and `"name"` as set up in the CleverTap dashboard).

```cpp
ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

TArray<TMap<FString, FString>> Variants = CleverTap.GetVariants();
for (const TMap<FString, FString>& Variant : Variants)
{
    const FString* VariantId = Variant.Find(TEXT("id"));
    UE_LOG(LogTemp, Log, TEXT("Active variant: %s"), VariantId ? *VariantId : TEXT("(no id)"));
}
```

Returns an empty array if no variants are currently assigned to the user. This method is C++ only — `TArray<TMap>` is not Blueprint-serializable.

## User Profiles
### On User Login
The `OnUserLogin()` method can be used when a user is identifier and logs into the app. Upon first login this enriches the
initial "Anonymous" user profile with additional properties such as name, age, and email. See [Updating the User Profile](https://developer.clevertap.com/docs/concepts-user-profiles#updating-the-user-profile) for a list of predefined properties.

#### C++
```cpp
FCleverTapProperties Profile;
Profile.Add(TEXT("Name"), TEXT("Jack Montana"));    // String
Profile.Add(TEXT("Identity"), 61026032);            // String or Number
Profile.Add(TEXT("Email"), TEXT("jack@gmail.com")); // Email string
Profile.Add(TEXT("Phone"), TEXT("+14155551234"));   // with country code, starting with +
Profile.Add(TEXT("Gender"), TEXT("M"));             // Can be either M or F
Profile.Add(TEXT("DOB"), FCleverTapDate(1953, 3, 13));

Profile.Add(TEXT("MSG-email"), false);   // Disable email notifications
Profile.Add(TEXT("MSG-push"), true);     // Enable push notifications
Profile.Add(TEXT("MSG-sms"), false);     // Disable SMS notifications
Profile.Add(TEXT("MSG-whatsapp"), true); // Enable WhatsApp notifications

TArray<FString> Stuff;
Stuff.Add(TEXT("bag"));
Stuff.Add(TEXT("shoes"));
Profile.Add(TEXT("MyStuff"), Stuff); // Multi-Value array of string support

ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
CleverTap.OnUserLogin(Profile);
```

#### Blueprint
![Calling OnUserLogin in Blueprint](/Docs/Images/BP_OnUserLogin.png)

### Updating a User Profile with PushProfile()
The user's profile can be enriched with additional properties at any time using the `PushProfile()` method. This
supports arbitrary single value and multi-value properties like `OnUserLogin()`.

#### C++
```cpp
FCleverTapProperties Profile;
// Update an existing property
Profile.Add(TEXT("MSG-push"), false);

// Add new properties
Profile.Add(TEXT("Tz"), TEXT("Asia/Kolkata"));
Profile.Add(TEXT("Plan Type"), TEXT("Silver"));
Profile.Add(TEXT("Score"), 100);

ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
CleverTap.PushProfile(Profile);
```

#### Blueprint
![Calling PushProfile in Blueprint](/Docs/Images/BP_PushProfile.png)

### Increment or Decrement Scalar Properties
If a given profile property is a `int32`, `int64`, `float`, or `double` then it can be incremented or decremented by an
arbitrary positive value.

#### C++
```cpp
ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
CleverTap.IncrementValue(TEXT("Score"), 50);
CleverTap.DecrementValue(TEXT("Score"), 100);
CleverTap.IncrementValue(TEXT("Score"), 5.5);
CleverTap.DecrementValue(TEXT("Score"), 3.14);
```

#### Blueprint
![Calling IncrementValue or DecrementValue in Blueprint](/Docs/Images/BP_IncrementDecrementValue.png)

## Event Recording
### User Events
User Events can be recorded any time after initialization.

#### C++
```cpp
ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

// event without properties
CleverTap.PushEvent(TEXT("Event No Props"));

// event with properties
FCleverTapProperties Actions;
Actions.Add("Product Name", "Casio Chronograph Watch");
Actions.Add("Category", "Mens Accessories");
Actions.Add("Price", 59.99);
CleverTap.PushEvent(TEXT("Product viewed"), Actions);
```
Event values can be any type that the `FCleverTapPropertyValue` variant type supports (`int32`, `int64`, `double`, `float`, `bool`, `const ANSICHAR*`, `FString`, or `FCleverTapDate`).

#### Blueprint
![Calling PushEvent in Blueprint](/Docs/Images/BP_PushEvent.png)
![Calling PushEventWithProperties in Blueprint](/Docs/Images/BP_PushEventWithProperties.png)

### Charged Events
Charged events are a special user event to record transaction details of a purchase. Each item in the purchase can be
recorded and enriched with custom properties.

#### C++
```cpp
// charge event
FCleverTapProperties ChargeDetails;
ChargeDetails.Add("Amount", 300);
ChargeDetails.Add("Payment Mode", "Credit card");
ChargeDetails.Add("Charged ID", 24052014);

FCleverTapProperties Item1;
Item1.Add("Product category", "books");
Item1.Add("Book name", "The Millionaire next door");
Item1.Add("Quantity", 1);

FCleverTapProperties Item2;
Item2.Add("Product category", "books");
Item2.Add("Book name", "Achieving inner zen");
Item2.Add("Quantity", 1);

FCleverTapProperties Item3;
Item3.Add("Product category", "books");
Item3.Add("Book name", "Chuck it, let's do it");
Item3.Add("Quantity", 5);

TArray<FCleverTapProperties> Items;
Items.Add(Item1);
Items.Add(Item2);
Items.Add(Item3);

ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
CleverTap.PushChargedEvent(ChargeDetails, Items);
```

#### Blueprint
![Calling PushChargedEvent in Blueprint](/Docs/Images/BP_PushChargedEvent.png)
