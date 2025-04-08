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
