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
