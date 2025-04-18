// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapJNI.h"

#include "Android/AndroidJNIUtilities.h"

#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

jclass GetCleverTapAPIClass(JNIEnv* Env)
{
	return LoadJavaClass(Env, "com.clevertap.android.sdk.CleverTapAPI");
}

void RegisterCleverTapLifecycleCallbacks(JNIEnv* Env)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::RegisterCleverTapLifecycleCallbacks()"));

	// Find ActivityLifecycleCallback class
	jclass LifecycleCallbackClass = LoadJavaClass(Env, "com/clevertap/android/sdk/ActivityLifecycleCallback");
	if (!LifecycleCallbackClass)
	{
		return;
	}

	// Get the static register(Application) method
	jmethodID RegisterMethod =
		GetStaticMethodID(Env, LifecycleCallbackClass, "register", "(Landroid/app/Application;)V");
	if (!RegisterMethod)
	{
		return;
	}

	jobject Application = GetJavaApplication(Env);
	if (!Application)
	{
		Env->DeleteLocalRef(LifecycleCallbackClass);
		return;
	}

	// Call register(Application)
	Env->CallStaticVoidMethod(LifecycleCallbackClass, RegisterMethod, Application);
	if (HandleException(Env, TEXT("ActivityLifecycleCallback.register failed!")))
	{
		// fall through
	}

	// Cleanup local references
	Env->DeleteLocalRef(Application);
	Env->DeleteLocalRef(LifecycleCallbackClass);
}

static void SetIdentityKeys(JNIEnv* Env, jobject ConfigInstance, const TArray<FString>& IdentityKeys)
{
	jclass StringClass = LoadJavaClass(Env, "java/lang/String");
	if (!StringClass)
	{
		return;
	}
	jclass ConfigClass = Env->GetObjectClass(ConfigInstance);
	if (HandleExceptionOrError(Env, !ConfigClass, TEXT("Getting CleverTapInstanceConfig class")))
	{
		return;
	}
	jmethodID SetKeysMethod = GetMethodID(Env, ConfigClass, "setIdentityKeys", "([Ljava/lang/String;)V");
	if (!SetKeysMethod)
	{
		return;
	}

	jobjectArray KeyArray = Env->NewObjectArray(IdentityKeys.Num(), StringClass, nullptr);
	if (HandleException(Env, TEXT("Creating String Array")))
	{
		return;
	}
	for (int32 i = 0; i < IdentityKeys.Num(); ++i)
	{
		jstring JKey = Env->NewStringUTF(TCHAR_TO_UTF8(*IdentityKeys[i]));
		if (HandleException(Env, TEXT("Creating String")))
		{
			return;
		}
		Env->SetObjectArrayElement(KeyArray, i, JKey);
		if (HandleException(Env, TEXT("Setting String array element")))
		{
			return;
		}
		Env->DeleteLocalRef(JKey);
	}
	Env->CallVoidMethod(ConfigInstance, SetKeysMethod, KeyArray);
	HandleException(Env, TEXT("CleverTapInstanceConfig.setIdentityKeys()"));
	Env->DeleteLocalRef(KeyArray);
}

static jobject CreateCleverTapInstanceConfig(JNIEnv* Env, const FCleverTapInstanceConfig& Config)
{
	// Find the CleverTapInstanceConfig class
	jclass ConfigClass = LoadJavaClass(Env, "com/clevertap/android/sdk/CleverTapInstanceConfig");
	if (!ConfigClass)
	{
		return nullptr;
	}

	// Get the static method ID for createInstance()
	jmethodID CreateMethod = GetStaticMethodID(Env, ConfigClass, "createInstance",
		"(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapInstanceConfig;");
	if (!CreateMethod)
	{
		Env->DeleteLocalRef(ConfigClass);
		return nullptr;
	}
	jobject Context = FAndroidApplication::GetGameActivityThis();

	// Convert FString parameters to Java Strings
	jstring JAccountId = Env->NewStringUTF(TCHAR_TO_UTF8(*Config.ProjectId));
	jstring JAccountToken = Env->NewStringUTF(TCHAR_TO_UTF8(*Config.ProjectToken));
	jstring JAccountRegion = Env->NewStringUTF(TCHAR_TO_UTF8(*Config.RegionCode));

	// Call createInstance and get the resulting object
	jobject ConfigInstance =
		Env->CallStaticObjectMethod(ConfigClass, CreateMethod, Context, JAccountId, JAccountToken, JAccountRegion);
	if (HandleExceptionOrError(Env, !ConfigInstance, TEXT("CleverTapInstanceConfig.createInstance() failed!")))
	{
		// keep going
	}
	Env->DeleteLocalRef(ConfigClass);
	Env->DeleteLocalRef(JAccountId);
	Env->DeleteLocalRef(JAccountToken);
	Env->DeleteLocalRef(JAccountRegion);

	// install the identity keys
	SetIdentityKeys(Env, ConfigInstance, Config.GetIdentityKeys());

	return ConfigInstance;
}

static void SetDefaultConfig(JNIEnv* Env, jobject ConfigInstance)
{
	// Find the CleverTapAPI class
	jclass CleverTapClass = LoadJavaClass(Env, "com/clevertap/android/sdk/CleverTapAPI");
	if (!CleverTapClass)
	{
		return;
	}

	// Get the field ID for defaultConfig (a static field)
	jfieldID DefaultConfigField =
		GetStaticFieldID(Env, CleverTapClass, "defaultConfig", "Lcom/clevertap/android/sdk/CleverTapInstanceConfig;");
	if (!DefaultConfigField)
	{
		Env->DeleteLocalRef(CleverTapClass);
		return;
	}

	// Set the static field CleverTapAPI.defaultConfig = ConfigInstance;
	Env->SetStaticObjectField(CleverTapClass, DefaultConfigField, ConfigInstance);
	if (HandleException(Env, TEXT("Failed to set CleverTapAPI.defaultConfig")))
	{
		// error logged; fall through
	}

	// Cleanup
	Env->DeleteLocalRef(CleverTapClass);
}

void SetDefaultConfig(JNIEnv* Env, const FCleverTapInstanceConfig& Config)
{
	jobject JavaConfig = CreateCleverTapInstanceConfig(Env, Config);
	if (!JavaConfig)
	{
		return;
	}
	SetDefaultConfig(Env, JavaConfig);
	Env->DeleteLocalRef(JavaConfig);
}

jobject GetDefaultInstance(JNIEnv* Env)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return nullptr;
	}
	jmethodID GetInstanceMethod = GetStaticMethodID(Env, CleverTapAPIClass, "getDefaultInstance",
		"(Landroid/content/Context;)Lcom/clevertap/android/sdk/CleverTapAPI;");
	if (!GetInstanceMethod)
	{
		Env->DeleteLocalRef(CleverTapAPIClass);
		return nullptr;
	}

	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jobject CleverTapInstance = Env->CallStaticObjectMethod(CleverTapAPIClass, GetInstanceMethod, Activity);
	if (HandleExceptionOrError(Env, !CleverTapInstance, TEXT("CleverTapAPI.getDefaultInstance() failed")))
	{
		Env->DeleteLocalRef(CleverTapAPIClass);
		return nullptr;
	}
	return CleverTapInstance;
}

jobject GetDefaultInstance(JNIEnv* Env, const FString& CleverTapId)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return nullptr;
	}
	jmethodID GetInstanceMethod = GetStaticMethodID(Env, CleverTapAPIClass, "getDefaultInstance",
		"(Landroid/content/Context;Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapAPI;");
	if (!GetInstanceMethod)
	{
		Env->DeleteLocalRef(CleverTapAPIClass);
		return nullptr;
	}
	jstring JCleverTapId = Env->NewStringUTF(TCHAR_TO_UTF8(*CleverTapId));
	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jobject CleverTapInstance =
		Env->CallStaticObjectMethod(CleverTapAPIClass, GetInstanceMethod, Activity, JCleverTapId);
	if (HandleExceptionOrError(
			Env, !CleverTapInstance, TEXT("CleverTapAPI.getDefaultInstance(context,cleverTapId) failed")))
	{
		Env->DeleteLocalRef(CleverTapAPIClass);
		return nullptr;
	}
	Env->DeleteLocalRef(CleverTapAPIClass);
	return CleverTapInstance;
}

static jobject JavaLogLevelFromString(JNIEnv* Env, const char* LogLevelName)
{
	jclass LogLevelClass = LoadJavaClass(Env, "com/clevertap/android/sdk/CleverTapAPI$LogLevel");
	if (!LogLevelClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find CleverTapAPI$LogLevel class"));
		return nullptr;
	}

	// Find the method to get the enum constant from the name
	jmethodID ValueOfMethod = GetStaticMethodID(
		Env, LogLevelClass, "valueOf", "(Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;");
	if (!ValueOfMethod)
	{
		Env->DeleteLocalRef(LogLevelClass);
		return nullptr;
	}
	// Get the enum constant from the name
	jstring JavaLogLevelName = Env->NewStringUTF(LogLevelName);
	jobject LogLevelEnumValue = Env->CallStaticObjectMethod(LogLevelClass, ValueOfMethod, JavaLogLevelName);
	if (ExceptionThrown(Env) || !LogLevelEnumValue)
	{
		HandleExceptionOrError(
			Env, !LogLevelEnumValue, FString::Printf(TEXT("Failed to get LogLevel enum for: %s"), *LogLevelName));
		LogLevelEnumValue = nullptr;
		// fall through
	}
	Env->DeleteLocalRef(JavaLogLevelName);
	Env->DeleteLocalRef(LogLevelClass);
	return LogLevelEnumValue;
}

static const char* CleverTapLogLevelName(ECleverTapLogLevel Level)
{
	switch (Level)
	{
		case ECleverTapLogLevel::Off:
			return "OFF";
		case ECleverTapLogLevel::Info:
			return "INFO";
		case ECleverTapLogLevel::Debug:
			return "DEBUG";
		case ECleverTapLogLevel::Verbose:
			return "VERBOSE";
		default:
		{
			UE_LOG(
				LogCleverTap, Error, TEXT("Unhandled ECleverTapLogLevel value. Defaulting to ECleverTapLogLevel::Off"));
			return "OFF";
		}
	}
}

bool SetDebugLevel(JNIEnv* Env, ECleverTapLogLevel Level)
{
	const char* LevelName = CleverTapLogLevelName(Level);
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::SetDebugLevel(%hs)"), LevelName);
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return false;
	}
	jmethodID SetDebugLogLevelMethod = GetStaticMethodID(
		Env, CleverTapAPIClass, "setDebugLevel", "(Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;)V");
	if (!SetDebugLogLevelMethod)
	{
		Env->DeleteLocalRef(CleverTapAPIClass);
		return false;
	}

	jobject JavaLogLevel = JavaLogLevelFromString(Env, LevelName);
	if (!JavaLogLevel)
	{
		Env->DeleteLocalRef(CleverTapAPIClass);
		return false;
	}

	Env->CallStaticVoidMethod(CleverTapAPIClass, SetDebugLogLevelMethod, JavaLogLevel);
	bool bFailed = HandleException(Env, TEXT("setDebugLevel"));
	Env->DeleteLocalRef(CleverTapAPIClass);
	Env->DeleteLocalRef(JavaLogLevel);
	return bFailed;
}

void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::OnUserLogin(Profile)"));
	UE_LOG(LogCleverTap, Log, TEXT("Profile: %s"), *JavaObjectToString(Env, Profile));
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}
	jmethodID OnUserLoginMethod = GetMethodID(Env, CleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!OnUserLoginMethod)
	{
		return;
	}

	// Call onUserLogin with the given profile
	Env->CallVoidMethod(CleverTapInstance, OnUserLoginMethod, Profile);
	if (HandleException(Env, TEXT("onUserLogin() failed")))
	{
		return;
	}
}

void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile, const FString& CleverTapID)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::OnUserLogin(Profile, CleverTapID)"));
	UE_LOG(
		LogCleverTap, Log, TEXT("CleverTapID: \"%s\", Profile: %s"), *CleverTapID, *JavaObjectToString(Env, Profile));

	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}

	// Get the OnUserLogin method with the correct signature (Map + String)
	jmethodID OnUserLoginMethod =
		GetMethodID(Env, CleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;Ljava/lang/String;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!OnUserLoginMethod)
	{
		return;
	}

	// Convert FString to jstring for CleverTapID
	jstring jCleverTapId = Env->NewStringUTF(TCHAR_TO_UTF8(*CleverTapID));
	if (HandleExceptionOrError(Env, !jCleverTapId, TEXT("Failed to convert CleverTapID to jstring")))
	{
		return;
	}

	// Call onUserLogin with profile and CleverTapID
	Env->CallVoidMethod(CleverTapInstance, OnUserLoginMethod, Profile, jCleverTapId);
	if (HandleException(Env, TEXT("onUserLogin(Profile, CleverTapID)")))
	{
		// fall through
	}

	// Clean up JNI references
	Env->DeleteLocalRef(jCleverTapId);
}

void PushProfile(JNIEnv* Env, jobject CleverTapInstance, jobject Profile)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::PushProfile()"));
	UE_LOG(LogCleverTap, Log, TEXT("Profile: %s"), *JavaObjectToString(Env, Profile));

	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}
	// Get the pushProfile method
	jmethodID PushProfileMethod = GetMethodID(Env, CleverTapAPIClass, "pushProfile", "(Ljava/util/Map;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!PushProfileMethod)
	{
		return;
	}

	// Call pushProfile with the given profile
	Env->CallVoidMethod(CleverTapInstance, PushProfileMethod, Profile);
	if (HandleException(Env, "pushProfile()"))
	{
		// already logged; fall through
	}
}

void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString& EventName)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::PushEvent(%s)"), *EventName);
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}
	// Get the pushProfile method
	jmethodID PushEventMethod = GetMethodID(Env, CleverTapAPIClass, "pushEvent", "(Ljava/lang/String;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!PushEventMethod)
	{
		return;
	}

	// convert the eventName
	jstring JavaEventName = Env->NewStringUTF(TCHAR_TO_UTF8(*EventName));

	// Call pushEvent
	Env->CallVoidMethod(CleverTapInstance, PushEventMethod, JavaEventName);
	if (HandleException(Env, "pushEvent()"))
	{
		// already logged; fall through
	}
}

void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString& EventName, jobject Actions)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::PushEvent(%s, Actions)"), *EventName);
	UE_LOG(LogCleverTap, Log, TEXT("EventName: '%s', Actions: %s"), *EventName, *JavaObjectToString(Env, Actions));

	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}
	// Get the pushProfile method
	jmethodID PushEventMethod =
		GetMethodID(Env, CleverTapAPIClass, "pushEvent", "(Ljava/lang/String;Ljava/util/Map;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!PushEventMethod)
	{
		return;
	}

	// convert the eventName
	jstring JavaEventName = Env->NewStringUTF(TCHAR_TO_UTF8(*EventName));

	// Call pushEvent
	Env->CallVoidMethod(CleverTapInstance, PushEventMethod, JavaEventName, Actions);
	if (HandleException(Env, "pushEvent(EventName,Actions"))
	{
		// already logged; fall through
	}

	Env->DeleteLocalRef(JavaEventName);
}

void PushChargedEvent(JNIEnv* Env, jobject CleverTapInstance, jobject ChargeDetails, jobject Items)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::PushChargedEvent()"));
	UE_LOG(LogCleverTap, Log, TEXT("ChargeDetails: '%s', Items: %s"), *JavaObjectToString(Env, ChargeDetails),
		*JavaObjectToString(Env, Items));

	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}
	// Get the pushProfile method
	jmethodID PushChargedEventMethod =
		GetMethodID(Env, CleverTapAPIClass, "pushChargedEvent", "(Ljava/util/HashMap;Ljava/util/ArrayList;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!PushChargedEventMethod)
	{
		return;
	}

	Env->CallVoidMethod(CleverTapInstance, PushChargedEventMethod, ChargeDetails, Items);
	if (HandleException(Env, "pushChargedEvent()"))
	{
		// already logged; fall through
	}
}

static void DecrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, jobject Amount)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	jmethodID DecrementMethod =
		GetMethodID(Env, CleverTapAPIClass, "decrementValue", "(Ljava/lang/String;Ljava/lang/Number;)V");
	if (!DecrementMethod)
	{
		return;
	}
	Env->DeleteLocalRef(CleverTapAPIClass);

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	Env->CallVoidMethod(CleverTapInstance, DecrementMethod, JavaKey, Amount);
	if (!HandleException(Env, "decrementValue()"))
	{
		// fall through
	}
	Env->DeleteLocalRef(JavaKey);
}

void DecrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, int Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::DecrementValue(%s,%d)"), *Key, Amount);
	jclass IntegerClass = LoadJavaClass(Env, "java/lang/Integer");
	if (!IntegerClass)
	{
		return;
	}
	jmethodID IntegerCtor = GetMethodID(Env, IntegerClass, "<init>", "(I)V");
	if (!IntegerCtor)
	{
		return;
	}
	jobject NumberObj = Env->NewObject(IntegerClass, IntegerCtor, Amount);
	if (!HandleExceptionOrError(Env, !NumberObj, "Constructing Integer"))
	{
		return;
	}
	DecrementValue(Env, CleverTapInstance, Key, NumberObj);
	Env->DeleteLocalRef(NumberObj);
	Env->DeleteLocalRef(IntegerClass);
}

void DecrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, double Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::DecrementValue(%s,%f)"), *Key, Amount);

	jclass DoubleClass = LoadJavaClass(Env, "java/lang/Double");
	if (!DoubleClass)
	{
		return;
	}
	jmethodID DoubleCtor = GetMethodID(Env, DoubleClass, "<init>", "(D)V");
	if (!DoubleCtor)
	{
		return;
	}
	jobject NumberObj = Env->NewObject(DoubleClass, DoubleCtor, Amount);
	if (!HandleExceptionOrError(Env, !NumberObj, "Constructing Double"))
	{
		return;
	}
	DecrementValue(Env, CleverTapInstance, Key, NumberObj);
	Env->DeleteLocalRef(NumberObj);
	Env->DeleteLocalRef(DoubleClass);
}

static void IncrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, jobject Amount)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	jmethodID IncrementMethod =
		GetMethodID(Env, CleverTapAPIClass, "incrementValue", "(Ljava/lang/String;Ljava/lang/Number;)V");
	if (!IncrementMethod)
	{
		return;
	}
	Env->DeleteLocalRef(CleverTapAPIClass);

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	Env->CallVoidMethod(CleverTapInstance, IncrementMethod, JavaKey, Amount);
	if (!HandleException(Env, "incrementValue()"))
	{
		// fall through
	}
	Env->DeleteLocalRef(JavaKey);
}

void IncrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, int Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::IncrementValue(%s,%d)"), *Key, Amount);
	jclass IntegerClass = LoadJavaClass(Env, "java/lang/Integer");
	if (!IntegerClass)
	{
		return;
	}
	jmethodID IntegerCtor = GetMethodID(Env, IntegerClass, "<init>", "(I)V");
	if (!IntegerCtor)
	{
		return;
	}
	jobject NumberObj = Env->NewObject(IntegerClass, IntegerCtor, Amount);
	if (!HandleExceptionOrError(Env, !NumberObj, "Constructing Integer"))
	{
		return;
	}
	IncrementValue(Env, CleverTapInstance, Key, NumberObj);
	Env->DeleteLocalRef(NumberObj);
	Env->DeleteLocalRef(IntegerClass);
}

void IncrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, double Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::IncrementValue(%s,%f)"), *Key, Amount);
	jclass DoubleClass = LoadJavaClass(Env, "java/lang/Double");
	if (!DoubleClass)
	{
		return;
	}
	jmethodID DoubleCtor = GetMethodID(Env, DoubleClass, "<init>", "(D)V");
	if (!DoubleCtor)
	{
		return;
	}
	jobject NumberObj = Env->NewObject(DoubleClass, DoubleCtor, Amount);
	if (!HandleExceptionOrError(Env, !NumberObj, "Constructing Double"))
	{
		return;
	}
	IncrementValue(Env, CleverTapInstance, Key, NumberObj);
	Env->DeleteLocalRef(NumberObj);
	Env->DeleteLocalRef(DoubleClass);
}

FString GetCleverTapID(JNIEnv* Env, jobject CleverTapInstance)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return TEXT("");
	}

	// Get the getCleverTapID() method ID
	jmethodID GetIDMethod = GetMethodID(Env, CleverTapAPIClass, "getCleverTapID", "()Ljava/lang/String;");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!GetIDMethod)
	{
		return TEXT("");
	}

	// Call getCleverTapID() and retrieve a Java string
	jstring JavaID = (jstring)Env->CallObjectMethod(CleverTapInstance, GetIDMethod);
	if (HandleExceptionOrError(Env, !JavaID, TEXT("getCleverTapID failed")))
	{
		return TEXT("");
	}

	// Convert Java string to Unreal FString
	const char* IDChars = Env->GetStringUTFChars(JavaID, nullptr);
	FString CleverTapID = FString(IDChars);
	Env->ReleaseStringUTFChars(JavaID, IDChars);

	// Cleanup
	Env->DeleteLocalRef(JavaID);

	return CleverTapID;
}

enum class ETimeZone
{
	UTC,
	Local
};

static jobject CreateJavaTimeZone(JNIEnv* Env, ETimeZone TimeZone)
{
	// Get TimeZone class & methods
	jclass TimeZoneClass = LoadJavaClass(Env, "java/util/TimeZone");
	if (!TimeZoneClass)
	{
		return nullptr;
	}
	jmethodID GetTimeZoneMethod =
		GetStaticMethodID(Env, TimeZoneClass, "getTimeZone", "(Ljava/lang/String;)Ljava/util/TimeZone;");
	jmethodID GetDefaultTimeZoneMethod = GetStaticMethodID(Env, TimeZoneClass, "getDefault", "()Ljava/util/TimeZone;");
	if (!GetTimeZoneMethod || !GetDefaultTimeZoneMethod)
	{
		Env->DeleteLocalRef(TimeZoneClass);
		return nullptr;
	}
	jobject JavaTimeZone = nullptr;
	switch (TimeZone)
	{
		case ETimeZone::Local:
		{
			jobject LocalTimeZone = Env->CallStaticObjectMethod(TimeZoneClass, GetDefaultTimeZoneMethod);
			bool bFailed = HandleExceptionOrError(Env, !LocalTimeZone, TEXT("Getting Local TimeZone"));
			if (!bFailed)
			{
				JavaTimeZone = LocalTimeZone;
			}
			break;
		}
		case ETimeZone::UTC:
		{
			jstring UtcString = Env->NewStringUTF("UTC");
			jobject UtcTimeZone = Env->CallStaticObjectMethod(TimeZoneClass, GetTimeZoneMethod, UtcString);
			bool bFailed = HandleExceptionOrError(Env, !UtcTimeZone, TEXT("Getting UTC TimeZone"));
			Env->DeleteLocalRef(UtcString);
			if (!bFailed)
			{
				JavaTimeZone = UtcTimeZone;
			}
			break;
		}

		default:
			UE_LOG(LogCleverTap, Error, TEXT("Invalid TimeZone enum!"));
			break;
	}
	Env->DeleteLocalRef(TimeZoneClass);

	return JavaTimeZone;
}

static jobject ConvertCleverTapDateToJavaDate(JNIEnv* Env, const FCleverTapDate& Date, ETimeZone TimeZone)
{
	// Find Java's Calendar class & methods we need
	jclass CalendarClass = LoadJavaClass(Env, "java/util/GregorianCalendar");
	if (!CalendarClass)
	{
		return nullptr;
	}
	jmethodID CalendarCtor = GetMethodID(Env, CalendarClass, "<init>", "(Ljava/util/TimeZone;)V");
	jmethodID SetMethod = GetMethodID(Env, CalendarClass, "set", "(IIIIII)V");
	jmethodID GetTimeMethod = GetMethodID(Env, CalendarClass, "getTime", "()Ljava/util/Date;");
	if (!CalendarCtor || !SetMethod || !GetTimeMethod)
	{
		return nullptr;
	}

	// Create the UTC Timezone for the calendar
	jobject JavaTimeZone = CreateJavaTimeZone(Env, TimeZone);
	if (!JavaTimeZone)
	{
		return nullptr;
	}

	// Construct a Calendar instance with UTC
	jobject JavaCalendar = Env->NewObject(CalendarClass, CalendarCtor, JavaTimeZone);
	bool bCalendarConstructionFailed = HandleExceptionOrError(Env, !JavaCalendar, TEXT("Calendar Constructor"));
	Env->DeleteLocalRef(JavaTimeZone);
	Env->DeleteLocalRef(CalendarClass);
	if (bCalendarConstructionFailed)
	{
		return nullptr;
	}

	// Set the date (year, month, day, hour=0, min=0, sec=0)
	Env->CallVoidMethod(JavaCalendar, SetMethod, Date.Year, Date.Month - 1, Date.Day, 0, 0, 0);
	if (HandleException(Env, TEXT("Calendar.set()")))
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed converting date to Java: Year=%d,Month=%d,Day=%d"), Date.Year,
			Date.Month, Date.Day);
		Env->DeleteLocalRef(JavaCalendar);
		return nullptr;
	}

	// Convert Calendar to Date
	jobject JavaDate = Env->CallObjectMethod(JavaCalendar, GetTimeMethod);
	bool bGetTimeFailed = HandleExceptionOrError(Env, !JavaDate, TEXT("Calendar getTime"));
	Env->DeleteLocalRef(JavaCalendar);
	if (bGetTimeFailed)
	{
		return nullptr;
	}
	return JavaDate;
}

jobject ConvertCleverTapPropertiesToJavaMap(JNIEnv* Env, const FCleverTapProperties& Properties)
{
	// HashMap Support
	jclass HashMapClass = LoadJavaClass(Env, "java/util/HashMap");
	jmethodID HashMapConstructor = GetMethodID(Env, HashMapClass, "<init>", "()V");
	jmethodID HashMapPut =
		GetMethodID(Env, HashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	if (!HashMapConstructor || !HashMapPut)
	{
		return nullptr;
	}

	// Integer support
	jclass IntegerClass = LoadJavaClass(Env, "java/lang/Integer");
	jmethodID IntegerConstructor = GetMethodID(Env, IntegerClass, "<init>", "(I)V");
	if (!IntegerConstructor)
	{
		return nullptr;
	}

	// Long support
	jclass LongClass = LoadJavaClass(Env, "java/lang/Long");
	jmethodID LongConstructor = GetMethodID(Env, LongClass, "<init>", "(J)V");
	if (!LongConstructor)
	{
		return nullptr;
	}

	// Double support
	jclass DoubleClass = LoadJavaClass(Env, "java/lang/Double");
	jmethodID DoubleConstructor = GetMethodID(Env, DoubleClass, "<init>", "(D)V");
	if (!DoubleConstructor)
	{
		return nullptr;
	}

	// Float Support
	jclass FloatClass = LoadJavaClass(Env, "java/lang/Float");
	jmethodID FloatConstructor = GetMethodID(Env, FloatClass, "<init>", "(F)V");
	if (!FloatConstructor)
	{
		return nullptr;
	}

	// Bool support
	jclass BooleanClass = LoadJavaClass(Env, "java/lang/Boolean");
	jmethodID BooleanConstructor = GetMethodID(Env, BooleanClass, "<init>", "(Z)V");
	if (!BooleanConstructor)
	{
		return nullptr;
	}

	// ArrayList support
	jclass ArrayListClass = LoadJavaClass(Env, "java/util/ArrayList");
	jmethodID ArrayListConstructor = GetMethodID(Env, ArrayListClass, "<init>", "()V");
	jmethodID ArrayListAdd = GetMethodID(Env, ArrayListClass, "add", "(Ljava/lang/Object;)Z");
	if (!ArrayListConstructor || !ArrayListAdd)
	{
		return nullptr;
	}

	// Construct a new java hashmap
	jobject JavaMap = Env->NewObject(HashMapClass, HashMapConstructor);
	if (HandleExceptionOrError(Env, !JavaMap, TEXT("HashMap Constructor")))
	{
		Env->DeleteLocalRef(HashMapClass);
		return nullptr;
	}

	// Iterate through Unreal's profile Map
	for (const auto& Property : Properties)
	{
		const FString& Key = Property.Key;
		const FCleverTapPropertyValue& Value = Property.Value;

		// Convert FString Key -> Java String
		jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
		jobject JavaValue = nullptr;

		// Handle different value types
		if (Value.IsType<int32>())
		{
			JavaValue = Env->NewObject(IntegerClass, IntegerConstructor, Value.Get<int32>());
		}
		else if (Value.IsType<int64>())
		{
			JavaValue = Env->NewObject(LongClass, LongConstructor, Value.Get<int64>());
		}
		else if (Value.IsType<double>())
		{
			JavaValue = Env->NewObject(DoubleClass, DoubleConstructor, Value.Get<double>());
		}
		else if (Value.IsType<float>())
		{
			JavaValue = Env->NewObject(FloatClass, FloatConstructor, Value.Get<float>());
		}
		else if (Value.IsType<bool>())
		{
			JavaValue = Env->NewObject(BooleanClass, BooleanConstructor, Value.Get<bool>());
		}
		else if (Value.IsType<FString>())
		{
			JavaValue = Env->NewStringUTF(TCHAR_TO_UTF8(*Value.Get<FString>()));
		}
		else if (Value.IsType<FCleverTapDate>())
		{
			JavaValue = ConvertCleverTapDateToJavaDate(Env, Value.Get<FCleverTapDate>(), ETimeZone::UTC);
		}
		else if (Value.IsType<TArray<int32>>())
		{
			// Convert TArray<int32> -> Java ArrayList<String>
			jobject JavaArrayList = Env->NewObject(ArrayListClass, ArrayListConstructor);
			if (!HandleExceptionOrError(Env, !JavaArrayList, "Constructing ArrayList"))
			{
				for (int32 Item : Value.Get<TArray<int32>>())
				{
					jstring JavaItem = Env->NewStringUTF(TCHAR_TO_UTF8(*FString::Printf(TEXT("%d"), Item)));
					Env->CallBooleanMethod(JavaArrayList, ArrayListAdd, JavaItem);
					if (HandleException(Env, "Adding to ArrayList"))
					{
						// failed but logged; keep going
					}
					Env->DeleteLocalRef(JavaItem);
				}
				JavaValue = JavaArrayList;
			}
		}
		else if (Value.IsType<TArray<int64>>())
		{
			// Convert TArray<int64> -> Java ArrayList<String>
			jobject JavaArrayList = Env->NewObject(ArrayListClass, ArrayListConstructor);
			if (!HandleExceptionOrError(Env, !JavaArrayList, "Constructing ArrayList"))
			{
				for (int64 Item : Value.Get<TArray<int64>>())
				{
					jstring JavaItem = Env->NewStringUTF(TCHAR_TO_UTF8(*FString::Printf(TEXT("%lld"), Item)));
					Env->CallBooleanMethod(JavaArrayList, ArrayListAdd, JavaItem);
					if (HandleException(Env, "Adding to ArrayList"))
					{
						// failed but logged; keep going
					}
					Env->DeleteLocalRef(JavaItem);
				}
				JavaValue = JavaArrayList;
			}
		}
		else if (Value.IsType<TArray<float>>())
		{
			// Convert TArray<float> -> Java ArrayList<String>
			jobject JavaArrayList = Env->NewObject(ArrayListClass, ArrayListConstructor);
			if (!HandleExceptionOrError(Env, !JavaArrayList, "Constructing ArrayList"))
			{
				for (float Item : Value.Get<TArray<float>>())
				{
					jstring JavaItem = Env->NewStringUTF(TCHAR_TO_UTF8(*FString::Printf(TEXT("%.7g"), Item)));
					Env->CallBooleanMethod(JavaArrayList, ArrayListAdd, JavaItem);
					if (HandleException(Env, "Adding to ArrayList"))
					{
						// failed but logged; keep going
					}
					Env->DeleteLocalRef(JavaItem);
				}
				JavaValue = JavaArrayList;
			}
		}
		else if (Value.IsType<TArray<double>>())
		{
			// Convert TArray<double> -> Java ArrayList<String>
			jobject JavaArrayList = Env->NewObject(ArrayListClass, ArrayListConstructor);
			if (!HandleExceptionOrError(Env, !JavaArrayList, "Constructing ArrayList"))
			{
				for (float Item : Value.Get<TArray<double>>())
				{
					jstring JavaItem = Env->NewStringUTF(TCHAR_TO_UTF8(*FString::Printf(TEXT("%.15g"), Item)));
					Env->CallBooleanMethod(JavaArrayList, ArrayListAdd, JavaItem);
					if (HandleException(Env, "Adding to ArrayList"))
					{
						// failed but logged; keep going
					}
					Env->DeleteLocalRef(JavaItem);
				}
				JavaValue = JavaArrayList;
			}
		}
		else if (Value.IsType<TArray<bool>>())
		{
			// Convert TArray<bool> -> Java ArrayList<String>
			jobject JavaArrayList = Env->NewObject(ArrayListClass, ArrayListConstructor);
			if (!HandleExceptionOrError(Env, !JavaArrayList, "Constructing ArrayList"))
			{
				jstring JavaFalseString = Env->NewStringUTF("false");
				jstring JavaTrueString = Env->NewStringUTF("true");
				for (bool Item : Value.Get<TArray<bool>>())
				{
					Env->CallBooleanMethod(JavaArrayList, ArrayListAdd, Item ? JavaTrueString : JavaFalseString);
					if (HandleException(Env, "Adding to ArrayList"))
					{
						// failed but logged; keep going
					}
				}
				Env->DeleteLocalRef(JavaFalseString);
				Env->DeleteLocalRef(JavaTrueString);
				JavaValue = JavaArrayList;
			}
		}
		else if (Value.IsType<TArray<FString>>())
		{
			// Convert TArray<FString> -> Java ArrayList<String>
			jobject JavaArrayList = Env->NewObject(ArrayListClass, ArrayListConstructor);
			if (!HandleExceptionOrError(Env, !JavaArrayList, "Constructing ArrayList"))
			{
				for (const FString& Item : Value.Get<TArray<FString>>())
				{
					jstring JavaItem = Env->NewStringUTF(TCHAR_TO_UTF8(*Item));
					Env->CallBooleanMethod(JavaArrayList, ArrayListAdd, JavaItem);
					if (HandleException(Env, "Adding to ArrayList"))
					{
						// failed but logged; keep going
					}
					Env->DeleteLocalRef(JavaItem);
				}
				JavaValue = JavaArrayList;
			}
		}
		else
		{
			UE_LOG(LogCleverTap, Error, TEXT("Unsupported FCleverTapPropertyValue type for key %s (type index %d)"),
				*Key, Value.GetIndex());
		}

		// catch exceptions & errors from any of the creation methods above
		bool bCreatedOkay = HandleExceptionOrError(Env, !JavaValue, TEXT("Creating Java value")) == false;
		if (bCreatedOkay)
		{
			// only add if we successfully created
			Env->CallObjectMethod(JavaMap, HashMapPut, JavaKey, JavaValue);
			if (HandleException(Env, "Adding Java value to Map"))
			{
				// failed but logged; keep going
			}
			Env->DeleteLocalRef(JavaValue);
		}

		Env->DeleteLocalRef(JavaKey);
	}

	Env->DeleteLocalRef(HashMapClass);
	Env->DeleteLocalRef(IntegerClass);
	Env->DeleteLocalRef(LongClass);
	Env->DeleteLocalRef(DoubleClass);
	Env->DeleteLocalRef(FloatClass);
	Env->DeleteLocalRef(BooleanClass);
	Env->DeleteLocalRef(ArrayListClass);

	return JavaMap;
}

jobject ConvertArrayOfCleverTapPropertiesToJavaArrayOfMap(JNIEnv* Env, const TArray<FCleverTapProperties>& Array)
{
	jclass ArrayListClass = LoadJavaClass(Env, "java/util/ArrayList");
	if (!ArrayListClass)
	{
		return nullptr;
	}
	jmethodID ArrayListCtor = GetMethodID(Env, ArrayListClass, "<init>", "()V");
	jmethodID AddMethod = GetMethodID(Env, ArrayListClass, "add", "(Ljava/lang/Object;)Z");
	if (!ArrayListCtor || !AddMethod)
	{
		return nullptr;
	}

	jobject JavaArray = Env->NewObject(ArrayListClass, ArrayListCtor);
	if (HandleExceptionOrError(Env, !JavaArray, TEXT("Constructing ArrayList")))
	{
		return nullptr;
	}

	for (const FCleverTapProperties& Item : Array)
	{
		jobject JavaItem = ConvertCleverTapPropertiesToJavaMap(Env, Item);
		if (!JavaItem)
		{
			// already logged that we had a problem; keep going
			continue;
		}
		Env->CallBooleanMethod(JavaArray, AddMethod, JavaItem);
		if (HandleException(Env, TEXT("Adding Item")))
		{
			// already logged that we had a problem; keep going
		}
		Env->DeleteLocalRef(JavaItem);
	}
	return JavaArray;
}

static jobject CreateUECleverTapListener(JNIEnv* Env, const char* ClassPath, void* NativeInstance)
{
	jclass ListenerClass = LoadJavaClass(Env, ClassPath);
	jmethodID ListenerConstructor = GetMethodID(Env, ListenerClass, "<init>", "(J)V");
	if (!ListenerConstructor)
	{
		return nullptr;
	}
	jobject Listener = Env->NewObject(ListenerClass, ListenerConstructor, jlong(NativeInstance));
	if (HandleExceptionOrError(Env, !Listener, "Creating Listener"))
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed creating listener of class \"%hs\""), ClassPath);
		Listener = nullptr;
	}
	Env->DeleteLocalRef(ListenerClass);
	return Listener;
}

bool RegisterPushPermissionResponseListener(JNIEnv* Env, jobject CleverTapInstance, void* NativeInstance)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	jmethodID RegisterListenerMethod =
		GetMethodID(Env, CleverTapAPIClass, "registerPushPermissionNotificationResponseListener",
			"(Lcom/clevertap/android/sdk/PushPermissionResponseListener;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!RegisterListenerMethod)
	{
		return false;
	}

	jobject Listener = JNI::CreateUECleverTapListener(
		Env, "com/clevertap/android/unreal/UECleverTapListeners$PushPermissionListener", NativeInstance);
	if (!Listener)
	{
		return false;
	}

	Env->CallVoidMethod(CleverTapInstance, RegisterListenerMethod, Listener);
	if (HandleException(Env, "registerPushPermissionNotificationResponseListener()"))
	{
		return false;
	}

	return true;
}

bool IsPushPermissionGranted(JNIEnv* Env, jobject CleverTapInstance)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	jmethodID IsGrantedMethod = GetMethodID(Env, CleverTapAPIClass, "isPushPermissionGranted", "()Z");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!IsGrantedMethod)
	{
		return false;
	}

	bool bGranted = Env->CallBooleanMethod(CleverTapInstance, IsGrantedMethod);
	if (HandleException(Env, "isPushPermissionGranted()"))
	{
		bGranted = false;
	}
	return bGranted;
}

void PromptForPushPermission(JNIEnv* Env, jobject CleverTapInstance, bool bFallbackToSettings)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	jmethodID PromptMethod = GetMethodID(Env, CleverTapAPIClass, "promptForPushPermission", "(Z)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!PromptMethod)
	{
		return;
	}

	Env->CallVoidMethod(CleverTapInstance, PromptMethod, bFallbackToSettings);
	if (HandleException(Env, "promptForPushPermission()"))
	{
		// fall through
	}
}

jobject CreatePushPrimerConfigJSON(JNIEnv* Env, const FCleverTapPushPrimerAlertConfig& PrimerConfig)
{
	jclass BridgeClass = LoadJavaClass(Env, "com/clevertap/android/unreal/UECleverTapBridge");
	jmethodID BuildMethod =
		GetStaticMethodID(Env, BridgeClass, "buildPushPrimerAlertConfig", "(Ljava/util/Map;)Lorg/json/JSONObject;");
	if (!BuildMethod)
	{
		return nullptr;
	}

	FCleverTapProperties ConfigProperties;
	ConfigProperties.Add(TEXT("TitleText"), PrimerConfig.TitleText.ToString());
	ConfigProperties.Add(TEXT("MessageText"), PrimerConfig.MessageText.ToString());
	ConfigProperties.Add(TEXT("PositiveButtonText"), PrimerConfig.PositiveButtonText.ToString());
	ConfigProperties.Add(TEXT("NegativeButtonText"), PrimerConfig.NegativeButtonText.ToString());
	ConfigProperties.Add(TEXT("FollowDeviceOrientation"), PrimerConfig.bFollowDeviceOrientation);
	ConfigProperties.Add(TEXT("FallbackToSettings"), PrimerConfig.bFallbackToSettings);
	jobject JavaMap = ConvertCleverTapPropertiesToJavaMap(Env, ConfigProperties);
	if (!JavaMap)
	{
		return nullptr;
	}

	jobject ResultJson = Env->CallStaticObjectMethod(BridgeClass, BuildMethod, JavaMap);
	if (HandleExceptionOrError(Env, !ResultJson, "buildPushPrimerAlertConfig()"))
	{
		ResultJson = nullptr;
	}
	Env->DeleteLocalRef(BridgeClass);
	Env->DeleteLocalRef(JavaMap);

	return ResultJson;
}

jobject CreatePushPrimerConfigJSON(JNIEnv* Env, const FCleverTapPushPrimerHalfInterstitialConfig& PrimerConfig)
{
	jclass BridgeClass = LoadJavaClass(Env, "com/clevertap/android/unreal/UECleverTapBridge");
	jmethodID BuildMethod = GetStaticMethodID(
		Env, BridgeClass, "buildPushPrimerHalfInterstitialConfig", "(Ljava/util/Map;)Lorg/json/JSONObject;");
	if (!BuildMethod)
	{
		return nullptr;
	}

	FCleverTapProperties ConfigProperties;
	ConfigProperties.Add(TEXT("TitleText"), PrimerConfig.TitleText.ToString());
	ConfigProperties.Add(TEXT("MessageText"), PrimerConfig.MessageText.ToString());
	ConfigProperties.Add(TEXT("PositiveButtonText"), PrimerConfig.PositiveButtonText.ToString());
	ConfigProperties.Add(TEXT("NegativeButtonText"), PrimerConfig.NegativeButtonText.ToString());
	ConfigProperties.Add(TEXT("FollowDeviceOrientation"), PrimerConfig.bFollowDeviceOrientation);
	ConfigProperties.Add(TEXT("FallbackToSettings"), PrimerConfig.bFallbackToSettings);
	if (PrimerConfig.ImageURL.IsEmpty() == false)
	{
		ConfigProperties.Add(TEXT("ImageURL"), PrimerConfig.ImageURL);
	}
	ConfigProperties.Add(TEXT("BackgroundColor"), ColorToHexString(PrimerConfig.BackgroundColor));
	ConfigProperties.Add(TEXT("ButtonBorderColor"), ColorToHexString(PrimerConfig.ButtonBorderColor));
	ConfigProperties.Add(TEXT("TitleTextColor"), ColorToHexString(PrimerConfig.TitleTextColor));
	ConfigProperties.Add(TEXT("MessageTextColor"), ColorToHexString(PrimerConfig.MessageTextColor));
	ConfigProperties.Add(TEXT("ButtonTextColor"), ColorToHexString(PrimerConfig.ButtonTextColor));
	ConfigProperties.Add(TEXT("ButtonBackgroundColor"), ColorToHexString(PrimerConfig.ButtonBackgroundColor));
	ConfigProperties.Add(TEXT("ButtonBorderRadius"), PrimerConfig.ButtonBorderRadius);

	jobject JavaMap = ConvertCleverTapPropertiesToJavaMap(Env, ConfigProperties);
	if (!JavaMap)
	{
		return nullptr;
	}

	jobject ResultJson = Env->CallStaticObjectMethod(BridgeClass, BuildMethod, JavaMap);
	if (HandleExceptionOrError(Env, !ResultJson, "buildPushPrimerHalfInterstitialConfig()"))
	{
		ResultJson = nullptr;
	}
	Env->DeleteLocalRef(BridgeClass);
	Env->DeleteLocalRef(JavaMap);

	return ResultJson;
}

void PromptPushPrimer(JNIEnv* Env, jobject CleverTapInstance, jobject PrimerConfigJSON)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	jmethodID PromptMethod = GetMethodID(Env, CleverTapAPIClass, "promptPushPrimer", "(Lorg/json/JSONObject;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (!PromptMethod)
	{
		return;
	}
	Env->CallVoidMethod(CleverTapInstance, PromptMethod, PrimerConfigJSON);
	if (HandleException(Env, "promptPushPrimer()"))
	{
		// fall through
	}
}

}}} // namespace CleverTapSDK::Android::JNI
