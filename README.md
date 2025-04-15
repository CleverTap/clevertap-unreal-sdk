# Plugin Integration
## Clone and Copy
Clone the sample application repository into a temporary folder to get access to the CleverTap Unreal plugin.
```bash
git clone --depth 1 https://github.com/CleverTap/clevertap-unreal-sdk.git
```
Copy the `clevertap-unreal-sdk\Plugins\CleverTap` folder to your Unreal project's Plugin folder.
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
will automatically initialize the CleverTap SDK and the default shared instance.

> [!NOTE]
> Deferred initialization is only available on iOS. Android platforms must use auto initialization.<br/>
> On iOS, initialization can be deferred if `bAutoInitializeSharedInstance` is set to `false`. An explicit call to
> `UCleverTapSubsystem::InitializeSharedInstance()` can be made to initialize the default shared instance. Configuration
> parameters can either be pulled from the `UCleverTapConfig` INI configuration or explicitly specified using a
> `FCleverTapInstanceConfig` as shown below.
> ```cpp
> FCleverTapInstanceConfig Config;
> Config.ProjectId = /*Your Project Id*/;
> Config.ProjectToken = /*Your project token*/;
> Config.RegionCode = /*Region code*/;
> Config.LogLevel = /*ECleverTapLogLevel enum specifying your desired CleverTap SDK log verbosity*/;
> GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->InitializeSharedInstance(Config);
> ```

## Push Notification Configuration
CleverTap allows you to send push notifications to your applications from our dashboard. 
Each platform requires slightly different setup.

### Android - Configuring Firebase Messaging
To use the default CleverTap notification implementation with Firebase Messaging: 

1. Follow [these instructions](https://developer.clevertap.com/docs/android-push) to create your `google-services.json` and register the firebase credentials in the CleverTap dashboard.

2. Copy the `google-services.json` to your project directory.

3. In your project's `Config\DefaultEngine.ini` ensure `bAndroidIntegrateFirebase` is `True`
and `AndroidGoogleServicesJsonPath` gives the project-relative path to where you copied ` google-services.json` 

```ini
[/Script/CleverTap.CleverTapConfig]
bAndroidIntegrateFirebase=True
AndroidGoogleServicesJsonPath=Config/google-services.json
```

#### Custom Android Notification Handling
[Custom Android Push Notification Handling](https://developer.clevertap.com/docs/android-push#custom-android-push-notification-handling) 
can be achieved by setting `bAndroidIntegrateFirebase` to `False` and using `UPL` to implement your own custom scheme.

## User Profiles
### On User Login
The `OnUserLogin()` method can be used when a user is identifier and logs into the app. Upon first login this enriches the
initial "Anonymous" user profile with additional properties such as name, age, and email. See [Updating the User Profile](https://developer.clevertap.com/docs/concepts-user-profiles#updating-the-user-profile) for a list of predefined properties.
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

### Updating a User Profile with PushProfile()
The user's profile can be enriched with additional properties at any time using the `PushProfile()` method. This
supports arbitrary single value and multi-value properties like `OnUserLogin()`.
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

### Increment or Decrement Scalar Properties
If a given profile property is a `int32`, `int64`, `float`, or `double` then it can be incremented or decremented by an
arbitrary positive value.
```cpp
ICleverTapInstance& CleverTap = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
CleverTap.IncrementValue(TEXT("Score"), 50);
CleverTap.DecrementValue(TEXT("Score"), 100);
CleverTap.IncrementValue(TEXT("Score"), 5.5);
CleverTap.DecrementValue(TEXT("Score"), 3.14);
```

## Event Recording
### User Events
User Events can be recorded any time after initialization.
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

### Charged Events
Charged events are a special user event to record transaction details of a purchase. Each item in the purchase can be
recorded and enriched with custom properties.
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
