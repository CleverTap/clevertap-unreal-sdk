# Configuration
## General Configuration
The following values need to be set or added to `Config/DefaultEngine.ini`
```ini
[/Script/CleverTap.CleverTapConfig]
ProjectId= ; Project ID from CleverTap project dashboard -> Settings -> Project
ProjectToken= ; Project Token from CleverTap project dashboard -> Settings -> Project
RegionCode= ; Region found on CleverTap project dashboard -> Settings -> Project
```

## Android Configuration

## iOS Configuration
Signing information needs to be set up for the project. See [iOS Quick
Start](https://dev.epicgames.com/documentation/en-us/unreal-engine/ios-quick-start?application_version=4.27) for Unreal
4.27 and
[Provisioning Profiles and Signing Certificates](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-ios-tvos-and-ipados-provisioning-profiles-and-signing-certificates-for-unreal-engine-projects). Ensure that Push Notifications are enabled under your provisioning profile capabilities.
### Engine Patches for iOS
Apply `EnginePatches/iOSSavedRemoteNotifications.patch` and `EnginePatches/UE4.27_ExtensionSupport.patch` for full
CleverTap In-App/Push Notification support. The CTNotificationService app extension uses an app ID in the format of
`YOUR_APP_BUNDLE_ID.CTNotificationService` where `YOUR_APP_BUNDLE_ID` is the same app ID used for the main app. If you need a different signing certificate for this app ID you can set one in `Config/DefaultEngine.ini`.
```ini
[/Script/IOSRuntimeSettings.IOSRuntimeSettings]
MobileProvision_CTNotificationService=CleverTap_Sample_App_Services_Provisioning.mobileprovision
```

# Building
## Building for Android
## Building for iOS
For Unreal 4.27 you can reference the [Building Unreal Engine from Source](https://dev.epicgames.com/documentation/en-us/unreal-engine/building-unreal-engine-from-source?application_version=4.27). Additionally the commands used to build this project from the command line are documented here:
```bash
git clone --depth 1 -b 4.27-plus https://github.com/EpicGames/UnrealEngine.git ue4
cd ue4
./Setup.command
./GenerateProjectFiles.command

# Make sure the editor builds correctly from source with your development setup
./Engine/Build/BatchFiles/Mac/Build.sh -Mode=Build UE4Editor ShaderCompileWorker Mac Development
# Test the editor
open ./Engine/Binaries/Mac/UE4Editor.app/

git clone --depth 1 https://github.com/CleverTap/clevertap-unreal-sdk.git CleverTapSample
git checkout -b ue4.27-clevertap
echo "CleverTapSample" >> .gitignore
patch -p1 -u -i ./CleverTapSample/EnginePatches/iOSSavedRemoteNotifications.patch
patch -p1 -u -i ./CleverTapSample/EnginePatches/UE4.27_ExtensionSupport.patch
git commit -m "Unreal 4.27 Changes for CleverTap"
./Engine/Build/BatchFiles/Mac/Build.sh -Mode=Build CleverTapSampleEditor Mac Development

# Make configuration changes either directly
vim ./CleverTapSample/Config/DefaultEngine.ini
# Or through the editor's Project Settings window
open ./Engine/Binaries/Mac/UE4Editor.app/ --args CleverTapSample

# For a development build (C++ sample)
./Engine/Build/BatchFiles/RunUAT.command BuildCookRun -noP4 -utf8output -Project=CleverTapSample -Platform=IOS -targetplatform=IOS -clientconfig=Development -cmdline="/Game/Map_CleverTapCppSample" -compile -build -cook -stage -deploy

# For a development build (Blueprint sample)
./Engine/Build/BatchFiles/RunUAT.command BuildCookRun -noP4 -utf8output -Project=CleverTapSample -Platform=IOS -targetplatform=IOS -clientconfig=Development -cmdline="/Game/Map_CleverTapBlueprintSample" -compile -build -cook -stage -deploy
```
