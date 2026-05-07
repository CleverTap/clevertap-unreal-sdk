# Compiling the Static Framework
## Compiling SDWebImage
- Clone the latest SDWebImage release.
```bash
git clone --depth=1 -b 5.21.1 https://github.com/SDWebImage/SDWebImage.git SDWebImage-5.21.1
```
- `cd SDWebImage-5.21.1`
- `xcodebuild -scheme SDWebImage SKIP_INSTALL=NO archive`
- Locate the output and `cd` to the Frameworks folder. It should be located somewhere in ***~/Library/Developer/Xcode/Archives/<today's date>/SDWebImage<timestamp>.xcarchive/Products/Library/Frameworks***
- `zip -r SDWebImage.framework.zip SDWebImage.framework`
- Move the zip to ***Plugins/CleverTap/Source/ThirdParty/IOS**
## Compiling CleverTapSDK to a Framework
- Check out the latest SDK release.
```bash
git clone --depth=1 -b 7.6.0 https://github.com/CleverTap/clevertap-ios-sdk.git sdk-7.6.0
```
- `cd sdk-7.6.0`
- `xcodebuild -scheme CleverTapSDK SKIP_INSTALL=NO archive`
- Locate the output and `cd` to the Frameworks folder. It should be located somewhere in ***~/Library/Developer/Xcode/Archives/<today's date>/CleverTapSDK<timestamp>.xcarchive/Products/Library/Frameworks***.
- `zip -r CleverTapSDK.framework.zip CleverTapSDK.framework`
- Move the zip to ***Plugins/CleverTap/Source/ThirdParty/IOS**
