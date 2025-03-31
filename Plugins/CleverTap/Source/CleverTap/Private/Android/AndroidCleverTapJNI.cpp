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

jobject CreateCleverTapInstanceConfig(
	JNIEnv* Env, const FString& AccountId, const FString& AccountToken, const FString& AccountRegion)
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
	jstring JAccountId = Env->NewStringUTF(TCHAR_TO_UTF8(*AccountId));
	jstring JAccountToken = Env->NewStringUTF(TCHAR_TO_UTF8(*AccountToken));
	jstring JAccountRegion = Env->NewStringUTF(TCHAR_TO_UTF8(*AccountRegion));

	// Call createInstance and get the resulting object
	jobject ConfigInstance =
		Env->CallStaticObjectMethod(ConfigClass, CreateMethod, Context, JAccountId, JAccountToken, JAccountRegion);
	if (HandleExceptionOrError(Env, !ConfigInstance, TEXT("CleverTapInstanceConfig.createInstance() failed!")))
	{
		// keep going
	}

	// Cleanup local refs
	Env->DeleteLocalRef(ConfigClass);
	Env->DeleteLocalRef(JAccountId);
	Env->DeleteLocalRef(JAccountToken);
	Env->DeleteLocalRef(JAccountRegion);

	return ConfigInstance;
}

void SetDefaultConfig(JNIEnv* Env, jobject ConfigInstance)
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

jobject CreateCleverTapInstanceConfig(JNIEnv* Env, const FCleverTapInstanceConfig& Config)
{
	return CreateCleverTapInstanceConfig(Env, Config.ProjectId, Config.ProjectToken, Config.RegionCode);
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
	UE_LOG(LogCleverTap, Log, TEXT("Profile: { %s }"), *JavaObjectToString(Env, Profile));
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
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapID: \"%s\", Profile: { %s }"), *CleverTapID,
		*JavaObjectToString(Env, Profile));

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
	UE_LOG(LogCleverTap, Log, TEXT("Profile: { %s }"), *JavaObjectToString(Env, Profile));

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

static jobject ConvertCleverTapDateToJavaDate(JNIEnv* Env, const FCleverTapDate& Date)
{
	// Find Java's Calendar class & methods we need
	jclass CalendarClass = LoadJavaClass(Env, "java/util/Calendar");
	if (!CalendarClass)
	{
		return nullptr;
	}
	jmethodID GetInstanceMethod = GetStaticMethodID(Env, CalendarClass, "getInstance", "()Ljava/util/Calendar;");
	jmethodID GetTimeMethod = GetMethodID(Env, CalendarClass, "getTime", "()Ljava/util/Date;");
	jmethodID SetMethod = GetMethodID(Env, CalendarClass, "set", "(IIIIII)V");
	jmethodID SetTimeZoneMethod = GetMethodID(Env, CalendarClass, "setTimeZone", "(Ljava/util/TimeZone;)V");
	if (!GetInstanceMethod || !GetTimeMethod || !SetMethod || !SetTimeZoneMethod)
	{
		return nullptr;
	}

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
		return nullptr;
	}

	/**	TODO
		// Get UTC TimeZone instance
		jstring UtcString = Env->NewStringUTF("UTC");
		jobject UtcTimeZone = Env->CallStaticObjectMethod(TimeZoneClass, GetTimeZoneMethod, UtcString);
		Env->DeleteLocalRef(UtcString);
		if (JNIExceptionThrown(Env, TEXT("Getting TimeZone")) || !UtcTimeZone)
		{
			UE_LOG(LogCleverTap, Error, TEXT("Failed to get UTC timezone"));
			return nullptr;
		}

		// Get Local TimeZone instance
		jobject LocalTimeZone = Env->CallStaticObjectMethod(TimeZoneClass, GetDefaultTimeZoneMethod);
		if (JNIExceptionThrown(Env, TEXT("TimeZoneClass getDefault")))
		{
			return nullptr;
		}
	**/

	// Create a Calendar instance
	jobject JavaCalendar = Env->CallStaticObjectMethod(CalendarClass, GetInstanceMethod);
	if (HandleExceptionOrError(Env, !JavaCalendar, TEXT("Calendar getInstance")))
	{
		return nullptr;
	}

	/**	TODO
		Env->CallVoidMethod(JavaCalendar, SetTimeZoneMethod, LocalTimeZone);
		if (JNIExceptionThrown(Env, TEXT("Calendar setTimeZone")))
		{
			return nullptr;
		}
	 **/

	// Set the date (year, month, day, hour=0, min=0, sec=0)
	Env->CallVoidMethod(JavaCalendar, SetMethod, Date.Year, Date.Month - 1, Date.Day, 0, 0, 0);
	if (HandleException(Env, TEXT("Calendar Set")))
	{
		return nullptr;
	}

	// Convert Calendar to Date
	jobject JavaDate = Env->CallObjectMethod(JavaCalendar, GetTimeMethod);
	if (HandleException(Env, TEXT("Calendar getTime")))
	{
		return nullptr;
	}

	return JavaDate;
}

jobject ConvertCleverTapPropertiesToJavaMap(JNIEnv* Env, const FCleverTapProperties& Properties)
{
	// todo rework this to be simpler & more robust

	// Create a new Java HashMap
	jclass HashMapClass = LoadJavaClass(Env, "java/util/HashMap");
	if (!HashMapClass)
	{
		return nullptr;
	}

	jmethodID HashMapConstructor = GetMethodID(Env, HashMapClass, "<init>", "()V");
	jmethodID HashMapPut =
		GetMethodID(Env, HashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	if (!HashMapConstructor || !HashMapPut)
	{
		Env->DeleteLocalRef(HashMapClass);
		return nullptr;
	}
	jobject JavaMap = Env->NewObject(HashMapClass, HashMapConstructor);
	if (HandleExceptionOrError(Env, !JavaMap, TEXT("HashMap Constructor")))
	{
		Env->DeleteLocalRef(HashMapClass);
		return nullptr;
	}

	// Iterate through Unreal's profile Map
	for (const auto& Property : Properties)
	{
		FString Key = Property.Key;
		const FCleverTapPropertyValue& Value = Property.Value;

		// Convert FString Key -> Java String
		jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
		jobject JavaValue = nullptr;

		// Handle different value types
		if (Value.IsType<int64>())
		{
			jclass LongClass = LoadJavaClass(Env, "java/lang/Long");
			jmethodID LongConstructor = Env->GetMethodID(LongClass, "<init>", "(J)V");
			JavaValue = Env->NewObject(LongClass, LongConstructor, Value.Get<int64>());
		}
		else if (Value.IsType<double>())
		{
			jclass DoubleClass = LoadJavaClass(Env, "java/lang/Double");
			jmethodID DoubleConstructor = Env->GetMethodID(DoubleClass, "<init>", "(D)V");
			JavaValue = Env->NewObject(DoubleClass, DoubleConstructor, Value.Get<double>());
		}
		else if (Value.IsType<float>())
		{
			jclass FloatClass = LoadJavaClass(Env, "java/lang/Float");
			jmethodID FloatConstructor = Env->GetMethodID(FloatClass, "<init>", "(F)V");
			JavaValue = Env->NewObject(FloatClass, FloatConstructor, Value.Get<float>());
		}
		else if (Value.IsType<bool>())
		{
			jclass BooleanClass = LoadJavaClass(Env, "java/lang/Boolean");
			jmethodID BooleanConstructor = Env->GetMethodID(BooleanClass, "<init>", "(Z)V");
			JavaValue = Env->NewObject(BooleanClass, BooleanConstructor, Value.Get<bool>());
		}
		else if (Value.IsType<FString>())
		{
			JavaValue = Env->NewStringUTF(TCHAR_TO_UTF8(*Value.Get<FString>()));
		}
		else if (Value.IsType<FCleverTapDate>())
		{
			JavaValue = ConvertCleverTapDateToJavaDate(Env, Value.Get<FCleverTapDate>());
		}
		else if (Value.IsType<TArray<FString>>())
		{
			// Convert TArray<FString> -> Java ArrayList<String>
			jclass ArrayListClass = LoadJavaClass(Env, "java/util/ArrayList");
			jmethodID ArrayListConstructor = Env->GetMethodID(ArrayListClass, "<init>", "()V");
			jobject JavaArrayList = Env->NewObject(ArrayListClass, ArrayListConstructor);

			jmethodID ArrayListAdd = Env->GetMethodID(ArrayListClass, "add", "(Ljava/lang/Object;)Z");

			for (const FString& Item : Value.Get<TArray<FString>>())
			{
				jstring JavaItem = Env->NewStringUTF(TCHAR_TO_UTF8(*Item));
				Env->CallBooleanMethod(JavaArrayList, ArrayListAdd, JavaItem);
				Env->DeleteLocalRef(JavaItem);
			}

			JavaValue = JavaArrayList;
		}

		// Add to HashMap
		if (JavaValue)
		{
			Env->CallObjectMethod(JavaMap, HashMapPut, JavaKey, JavaValue);
			Env->DeleteLocalRef(JavaValue);
		}

		Env->DeleteLocalRef(JavaKey);
	}
	/*** todo robust here
		if (JNIExceptionThrown(Env, TEXT("ConvertCleverTapPropertiesToJavaMap")))
		{
			return nullptr;
		}
	**/

	// DebugLogJavaMap(Env, JavaMap);

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
		jobject JavaItemMap = ConvertCleverTapPropertiesToJavaMap(Env, Item);
		if (!JavaItemMap)
		{
			// already logged that we had a problem; keep going
			continue;
		}
		Env->CallBooleanMethod(JavaArray, AddMethod, JavaItemMap);
		if (HandleException(Env, TEXT("Adding Item")))
		{
			// already logged that we had a problem; keep going
		}
		Env->DeleteLocalRef(JavaItemMap);
	}
	return JavaArray;
}

}}} // namespace CleverTapSDK::Android::JNI
