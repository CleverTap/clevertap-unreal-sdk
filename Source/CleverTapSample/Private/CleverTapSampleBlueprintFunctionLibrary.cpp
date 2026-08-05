// Copyright CleverTap All Rights Reserved.
#include "CleverTapSampleBlueprintFunctionLibrary.h"

#include "Algo/AllOf.h"
#include "CleverTapInstance.h"
#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"
#include "Engine.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Math/NumericLimits.h"
#include "Misc/Optional.h"
#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#elif PLATFORM_IOS
#import <QuickLook/QuickLook.h>
#import <UIKit/UIKit.h>

@interface UECTFilePreviewSource : NSObject <QLPreviewControllerDataSource>
@property (nonatomic, strong) NSURL* fileURL;
+ (NSString*)extensionFromMagicBytes:(NSString*)path;
@end
@implementation UECTFilePreviewSource
- (NSInteger)numberOfPreviewItemsInPreviewController:(QLPreviewController*)c { return 1; }
- (id<QLPreviewItem>)previewController:(QLPreviewController*)c previewItemAtIndex:(NSInteger)i { return _fileURL; }
+ (NSString*)extensionFromMagicBytes:(NSString*)path {
    NSFileHandle* fh = [NSFileHandle fileHandleForReadingAtPath:path];
    NSData* header = [fh readDataOfLength:12];
    [fh closeFile];
    if (header.length < 4) return nil;
    const uint8_t* b = (const uint8_t*)header.bytes;
    if (b[0]==0x47 && b[1]==0x49 && b[2]==0x46) return @"gif";          // GIF8
    if (b[0]==0xFF && b[1]==0xD8) return @"jpg";                         // JPEG
    if (b[0]==0x89 && b[1]==0x50 && b[2]==0x4E && b[3]==0x47) return @"png"; // PNG
    if (header.length>=12 && b[0]==0x52 && b[1]==0x49 && b[2]==0x46 && b[3]==0x41
        && b[8]==0x57 && b[9]==0x45 && b[10]==0x42 && b[11]==0x50) return @"webp";
    return nil;
}
@end
#endif

namespace {
TOptional<int64> CleverTapProperty_TryParseInt64(FStringView Str)
{
	if (Str.IsEmpty())
	{
		return TOptional<int64>{};
	}

	const bool bIsExplicitlyNegative = Str.StartsWith(TCHAR{ '-' });
	if (bIsExplicitlyNegative || Str.StartsWith(TCHAR{ '+' }))
	{
		Str.RemovePrefix(1);
	}

	if (Str.IsEmpty())
	{
		return TOptional<int64>{};
	}

	if (!Algo::AllOf(Str, FChar::IsDigit))
	{
		return TOptional<int64>{};
	}

	int64 const ParsedValue = FCString::Atoi64(Str.GetData());
	int64 const AsInt64 = bIsExplicitlyNegative ? -ParsedValue : ParsedValue;
	return TOptional<int64>{ AsInt64 };
}

bool CleverTapProperty_IsInt32(FStringView Str)
{
	const TOptional<int64> MaybeValue = CleverTapProperty_TryParseInt64(Str);
	if (!MaybeValue)
	{
		return false;
	}

	const int64 AsInt64 = MaybeValue.GetValue();
	return AsInt64 >= TNumericLimits<int32>::Min() && AsInt64 <= TNumericLimits<int32>::Max();
}

bool CleverTapProperty_IsInt64(FStringView Str)
{
	const TOptional<int64> MaybeValue = CleverTapProperty_TryParseInt64(Str);
	if (!MaybeValue)
	{
		return false;
	}

	if (Str.StartsWith('+'))
	{
		Str.RemovePrefix(1);
	}

	const int64 AsInt64 = MaybeValue.GetValue();
	return LexToString(AsInt64) == Str;
}

bool CleverTapProperty_IsBool(FStringView Str)
{
	return Str.Equals(TEXT("true"), ESearchCase::IgnoreCase) || Str.Equals(TEXT("false"), ESearchCase::IgnoreCase);
}

bool CleverTapProperty_IsDate(FStringView Str)
{
	// Expect date formatted as a YYYY-MM-DD string
	int32 SepIdx{};
	if (!Str.FindChar(TCHAR{ '-' }, SepIdx))
	{
		return false;
	}
	const FStringView YearPart = Str.Left(SepIdx);
	Str.RemovePrefix(SepIdx + 1);

	if (!Str.FindChar(TCHAR{ '-' }, SepIdx))
	{
		return false;
	}
	const FStringView MonthPart = Str.Left(SepIdx);
	const FStringView DayPart = Str.RightChop(SepIdx + 1);

	// Validate year format of 4 digits
	if (YearPart.IsEmpty() || YearPart.Len() > 4 || !Algo::AllOf(YearPart, FChar::IsDigit))
	{
		return false;
	}

	// Validate month format of 2 digits
	if (MonthPart.IsEmpty() || MonthPart.Len() > 2 || !Algo::AllOf(MonthPart, FChar::IsDigit))
	{
		return false;
	}

	// Validate day format of 2 digits
	if (DayPart.IsEmpty() || DayPart.Len() > 2 || !Algo::AllOf(DayPart, FChar::IsDigit))
	{
		return false;
	}

	// Atoi is fine here as we know there's a '-' character between month and day bits
	const int32 MonthValue = FCString::Atoi(MonthPart.GetData());
	if (MonthValue <= 0 || MonthValue > 12)
	{
		return false;
	}

	// Atoi is fine here as we know there's a null terminator due to it being from a FString
	const int32 DayValue = FCString::Atoi(DayPart.GetData());
	constexpr int64 DAYS_PER_MONTH[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (DayValue <= 0 || DayValue > DAYS_PER_MONTH[MonthValue])
	{
		return false;
	}

	// Non-Leap years have 28 days, so check if we matched 29 for the month of Februrary 29 above
	if (MonthValue == 2 && DayValue == DAYS_PER_MONTH[2])
	{
		// Atoi is fine here as we know there's a '-' character between year and month bits
		const int32 YearValue = FCString::Atoi(YearPart.GetData());
		if (!FDateTime::IsLeapYear(YearValue))
		{
			return false;
		}
	}

	return true;
}

} // namespace

void UCleverTapSampleBlueprintFunctionLibrary::OpenPEFileVariable(const FString& VariableName)
{
	auto& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	const FString FilePath = CT.GetFileVariablePath(VariableName);
	if (FilePath.IsEmpty())
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("PE: File variable '%s' has no path (not yet downloaded)"), *VariableName);
		return;
	}

	// Non-file variables return their string value from GetFileVariablePath — filter them out
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		UE_LOG(LogCleverTapSample, Log, TEXT("PE: '%s' is not a file variable or file not yet downloaded"), *VariableName);
		return;
	}

	FPlatformApplicationMisc::ClipboardCopy(*FilePath);
	UE_LOG(LogCleverTapSample, Log, TEXT("PE: File path copied to clipboard: %s"), *FilePath);

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass BridgeClass = FAndroidApplication::FindJavaClass("com/clevertap/android/unreal/UECleverTapBridge");
		if (BridgeClass)
		{
			jmethodID OpenFileMethod = Env->GetStaticMethodID(BridgeClass, "openFile",
				"(Landroid/content/Context;Ljava/lang/String;)V");
			if (OpenFileMethod)
			{
				jobject Context = FAndroidApplication::GetGameActivityThis();
				jstring JFilePath = Env->NewStringUTF(TCHAR_TO_UTF8(*FilePath));
				Env->CallStaticVoidMethod(BridgeClass, OpenFileMethod, Context, JFilePath);
				Env->DeleteLocalRef(JFilePath);
				if (Env->ExceptionCheck())
				{
					Env->ExceptionDescribe();
					Env->ExceptionClear();
					UE_LOG(LogCleverTapSample, Warning, TEXT("PE: openFile JNI threw an exception"));
				}
			}
			Env->DeleteLocalRef(BridgeClass);
		}
	}
#elif PLATFORM_IOS
	NSString* NSPath = [NSString stringWithUTF8String:TCHAR_TO_UTF8(*FilePath)];
	// Detect extension from magic bytes — CleverTap stores files without extension
	NSString* ext = [UECTFilePreviewSource extensionFromMagicBytes:NSPath];
	NSURL* PreviewURL;
	if (ext.length > 0) {
		// Copy to a temp path with the correct extension so QLPreviewController can identify the type
		NSString* TempPath = [NSTemporaryDirectory() stringByAppendingPathComponent:
			[[[NSUUID UUID] UUIDString] stringByAppendingPathExtension:ext]];
		[[NSFileManager defaultManager] copyItemAtPath:NSPath toPath:TempPath error:nil];
		PreviewURL = [NSURL fileURLWithPath:TempPath];
	} else {
		PreviewURL = [NSURL fileURLWithPath:NSPath];
	}
	dispatch_async(dispatch_get_main_queue(), ^{
		static UECTFilePreviewSource* PreviewSource;
		PreviewSource = [[UECTFilePreviewSource alloc] init];
		PreviewSource.fileURL = PreviewURL;
		QLPreviewController* Preview = [[QLPreviewController alloc] init];
		Preview.dataSource = PreviewSource;
		UIViewController* Root = [UIApplication sharedApplication].keyWindow.rootViewController;
		while (Root.presentedViewController) { Root = Root.presentedViewController; }
		[Root presentViewController:Preview animated:YES completion:nil];
	});
#endif
}

bool UCleverTapSampleBlueprintFunctionLibrary::IsEventPropertyValueValid(const FEventPropertyViewModel& Property)
{
	switch (Property.ValueType)
	{
		case EEventPropertyType::String:
		{
			return true;
		}

		case EEventPropertyType::Int32:
		{
			return CleverTapProperty_IsInt32(Property.ValueString);
		}

		case EEventPropertyType::Int64:
		{
			return CleverTapProperty_IsInt64(Property.ValueString);
		}

		case EEventPropertyType::Float:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EEventPropertyType::Double:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EEventPropertyType::Bool:
		{
			return CleverTapProperty_IsBool(Property.ValueString);
		}

		case EEventPropertyType::Date:
		{
			return CleverTapProperty_IsDate(Property.ValueString);
		}
	}

	UE_LOG(LogCleverTapSample, Error, TEXT("Unhandled EEventPropertyType: %d"), static_cast<uint8>(Property.ValueType));
	return false;
}

bool UCleverTapSampleBlueprintFunctionLibrary::IsProfilePropertyValueValid(const FProfilePropertyViewModel& Property)
{
	switch (Property.ValueType)
	{
		case EProfilePropertyType::String:
		{
			return true;
		}

		case EProfilePropertyType::Int32:
		{
			return CleverTapProperty_IsInt32(Property.ValueString);
		}

		case EProfilePropertyType::Int64:
		{
			return CleverTapProperty_IsInt64(Property.ValueString);
		}

		case EProfilePropertyType::Float:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EProfilePropertyType::Double:
		{
			// For float/double, we don't bother with precision
			return Property.ValueString.IsNumeric();
		}

		case EProfilePropertyType::Bool:
		{
			return CleverTapProperty_IsBool(Property.ValueString);
		}

		case EProfilePropertyType::Date:
		{
			return CleverTapProperty_IsDate(Property.ValueString);
		}

		case EProfilePropertyType::Array_Int32:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));
			return Algo::AllOf(ElementStrings,
				[](const FString& Str) { return CleverTapProperty_IsInt32(FStringView{ Str }.TrimStartAndEnd()); });
		}

		case EProfilePropertyType::Array_Int64:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));
			return Algo::AllOf(ElementStrings,
				[](const FString& Str) { return CleverTapProperty_IsInt64(FStringView{ Str }.TrimStartAndEnd()); });
		}

		case EProfilePropertyType::Array_Float:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));

			// Pre-trim since FStringView{} doesn't have IsNumeric
			for (auto& Str : ElementStrings)
			{
				Str.TrimStartAndEndInline();
			}

			return Algo::AllOf(ElementStrings, [](const FString& Str) { return Str.IsNumeric(); });
		}

		case EProfilePropertyType::Array_Double:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));

			// Pre-trim since FStringView{} doesn't have IsNumeric
			for (auto& Str : ElementStrings)
			{
				Str.TrimStartAndEndInline();
			}

			return Algo::AllOf(ElementStrings, [](const FString& Str) { return Str.IsNumeric(); });
		}

		case EProfilePropertyType::Array_Bool:
		{
			TArray<FString> ElementStrings;
			Property.ValueString.ParseIntoArray(ElementStrings, TEXT(","));
			return Algo::AllOf(ElementStrings,
				[](const FString& Str) { return CleverTapProperty_IsBool(FStringView{ Str }.TrimStartAndEnd()); });
		}

		case EProfilePropertyType::Array_String:
		{
			return true;
		}
	}

	UE_LOG(
		LogCleverTapSample, Error, TEXT("Unhandled EProfilePropertyType: %d"), static_cast<uint8>(Property.ValueType));
	return false;
}
