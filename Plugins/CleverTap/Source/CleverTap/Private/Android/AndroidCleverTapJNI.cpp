// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapJNI.h"

#include "Android/AndroidJNIUtilities.h"
#include "Android/AndroidCleverTapPropertiesJNI.h"

#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

jclass GetCleverTapAPIClass(JNIEnv* Env)
{
	static jclass CleverTapAPIClass = CacheClass(Env, "com.clevertap.android.sdk.CleverTapAPI");
	return CleverTapAPIClass;
}

static jclass GetBridgeClass(JNIEnv* Env)
{
	static jclass BridgeClass = CacheClass(Env, "com/clevertap/android/unreal/UECleverTapBridge");
	return BridgeClass;
}

static void SetIdentityKeys(JNIEnv* Env, jobject ConfigInstance, const TArray<FString>& IdentityKeys)
{
	static jclass StringClass = CacheClass(Env, "java/lang/String");
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

static jobject JavaEncryptionLevelFromString(JNIEnv* Env, const char* LevelName)
{
	static jclass EncryptionLevelClass = CacheClass(Env, "com/clevertap/android/sdk/cryption/EncryptionLevel");
	static jmethodID ValueOfMethod = GetStaticMethodID(Env, EncryptionLevelClass, "valueOf",
		"(Ljava/lang/String;)Lcom/clevertap/android/sdk/cryption/EncryptionLevel;");
	if (!ValueOfMethod)
	{
		return nullptr;
	}
	// Get the enum constant from the name
	jstring JavaLevelName = Env->NewStringUTF(LevelName);
	jobject LevelEnumValue = Env->CallStaticObjectMethod(EncryptionLevelClass, ValueOfMethod, JavaLevelName);
	if (ExceptionThrown(Env) || !LevelEnumValue)
	{
		HandleExceptionOrError(
			Env, !LevelEnumValue, FString::Printf(TEXT("Failed to get EncryptionLevel enum for: %s"), *LevelName));
		LevelEnumValue = nullptr;
		// fall through
	}
	Env->DeleteLocalRef(JavaLevelName);
	return LevelEnumValue;
}

/** returns the Java string for the corresponding enum value, so we can ask java for its value */
static const char* CleverTapEncryptionLevelJavaName(ECleverTapEncryptionLevel Level)
{
	switch (Level)
	{
		case ECleverTapEncryptionLevel::None:
			return "NONE";
		case ECleverTapEncryptionLevel::Medium:
			return "MEDIUM";
		default:
		{
			UE_LOG(LogCleverTap, Error,
				TEXT("Unhandled ECleverTapEncryptionLevel value. Defaulting to ECleverTapEncryptionLevel::None"));
			return "NONE";
		}
	}
}

static jobject CleverTapEncryptionLevelToJava(JNIEnv* Env, ECleverTapEncryptionLevel EncryptionLevel)
{
	return JavaEncryptionLevelFromString(Env, CleverTapEncryptionLevelJavaName(EncryptionLevel));
}

/** Returns the name of each ECleverTapLogLevel value in the Java */
static const char* CleverTapLogLevelJavaName(ECleverTapLogLevel LogLevel)
{
	switch (LogLevel)
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

static jobject CleverTapLogLevelToJava(JNIEnv* Env, ECleverTapLogLevel LogLevel)
{
	static jclass LogLevelClass = CacheClass(Env, "com/clevertap/android/sdk/CleverTapAPI$LogLevel");
	static jmethodID ValueOfMethod = GetStaticMethodID(
		Env, LogLevelClass, "valueOf", "(Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;");
	if (!ValueOfMethod)
	{
		return nullptr;
	}
	jstring JavaLevelName = Env->NewStringUTF(CleverTapLogLevelJavaName(LogLevel));
	jobject LogLevelEnumValue = Env->CallStaticObjectMethod(LogLevelClass, ValueOfMethod, JavaLevelName);
	if (ExceptionThrown(Env) || !LogLevelEnumValue)
	{
		HandleExceptionOrError(Env, !LogLevelEnumValue,
			FString::Printf(TEXT("Failed to get LogLevel enum for: %s"), *CleverTapLogLevelJavaName(LogLevel)));
		LogLevelEnumValue = nullptr;
		// fall through
	}
	Env->DeleteLocalRef(JavaLevelName);
	return LogLevelEnumValue;
}

static int CleverTapLogLevelToJavaInt(JNIEnv* Env, ECleverTapLogLevel Level)
{
	static jclass LogLevelClass = CacheClass(Env, "com/clevertap/android/sdk/CleverTapAPI$LogLevel");
	static jmethodID OrdinalMethod = Env->GetMethodID(LogLevelClass, "ordinal", "()I");

	jobject EnumObject = CleverTapLogLevelToJava(Env, Level);
	if (!EnumObject)
	{
		return 0;
	}
	jint Ordinal = Env->CallIntMethod(EnumObject, OrdinalMethod);
	if (HandleException(Env, "LogLevel.ordinal()"))
	{
		// fall through
	}
	Env->DeleteLocalRef(EnumObject);
	return Ordinal;
}

static jobject CreateCleverTapInstanceConfig(JNIEnv* Env, const FCleverTapInstanceConfig& Config)
{
	static jclass ConfigClass = CacheClass(Env, "com/clevertap/android/sdk/CleverTapInstanceConfig");
	static jmethodID CreateMethod = GetStaticMethodID(Env, ConfigClass, "createInstance",
		"(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapInstanceConfig;");
	static jmethodID SetEncryptionMethod =
		GetMethodID(Env, ConfigClass, "setEncryptionLevel", "(Lcom/clevertap/android/sdk/cryption/EncryptionLevel;)V");
	static jmethodID SetDebugLevelMethod = GetMethodID(Env, ConfigClass, "setDebugLevel", "(I)V");
	if (!CreateMethod || !SetEncryptionMethod || !SetDebugLevelMethod)
	{
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
	Env->DeleteLocalRef(JAccountId);
	Env->DeleteLocalRef(JAccountToken);
	Env->DeleteLocalRef(JAccountRegion);
	if (!ConfigInstance)
	{
		return nullptr;
	}

	// install the identity keys
	SetIdentityKeys(Env, ConfigInstance, Config.GetIdentityKeys());

	// set the encryption level
	jobject JavaEncryption = CleverTapEncryptionLevelToJava(Env, Config.EncryptionLevel);
	Env->CallVoidMethod(ConfigInstance, SetEncryptionMethod, JavaEncryption);
	HandleException(Env, TEXT("CleverTapInstanceConfig.setEncryptionLevel() failed!"));
	Env->DeleteLocalRef(JavaEncryption);

	// set the log level; this version needs the java-side int value instead of the enum
	int JavaLevelInt = CleverTapLogLevelToJavaInt(Env, Config.LogLevel);
	Env->CallVoidMethod(ConfigInstance, SetDebugLevelMethod, JavaLevelInt);
	HandleException(Env, TEXT("CleverTapInstanceConfig.setDebugLevel() failed!"));

	return ConfigInstance;
}

static void SetDefaultConfig(JNIEnv* Env, jobject ConfigInstance)
{
	// Find the CleverTapAPI class
	static jclass CleverTapClass = GetCleverTapAPIClass(Env);
	static jfieldID DefaultConfigField =
		GetStaticFieldID(Env, CleverTapClass, "defaultConfig", "Lcom/clevertap/android/sdk/CleverTapInstanceConfig;");
	if (!DefaultConfigField)
	{
		return;
	}

	// Set the static field CleverTapAPI.defaultConfig = ConfigInstance;
	Env->SetStaticObjectField(CleverTapClass, DefaultConfigField, ConfigInstance);
	if (HandleException(Env, TEXT("Failed to set CleverTapAPI.defaultConfig")))
	{
		// error logged; fall through
	}
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
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID GetInstanceMethod = GetStaticMethodID(Env, CleverTapAPIClass, "getDefaultInstance",
		"(Landroid/content/Context;)Lcom/clevertap/android/sdk/CleverTapAPI;");
	if (!GetInstanceMethod)
	{
		return nullptr;
	}

	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jobject CleverTapInstance = Env->CallStaticObjectMethod(CleverTapAPIClass, GetInstanceMethod, Activity);
	if (HandleExceptionOrError(Env, !CleverTapInstance, TEXT("CleverTapAPI.getDefaultInstance() failed")))
	{
		return nullptr;
	}
	return CleverTapInstance;
}

jobject GetDefaultInstance(JNIEnv* Env, const FString& CleverTapId)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID GetInstanceMethod = GetStaticMethodID(Env, CleverTapAPIClass, "getDefaultInstance",
		"(Landroid/content/Context;Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapAPI;");
	if (!GetInstanceMethod)
	{
		return nullptr;
	}
	jstring JCleverTapId = Env->NewStringUTF(TCHAR_TO_UTF8(*CleverTapId));
	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jobject CleverTapInstance =
		Env->CallStaticObjectMethod(CleverTapAPIClass, GetInstanceMethod, Activity, JCleverTapId);
	if (HandleExceptionOrError(
			Env, !CleverTapInstance, TEXT("CleverTapAPI.getDefaultInstance(context,cleverTapId) failed")))
	{
		CleverTapInstance = nullptr;
	}
	Env->DeleteLocalRef(JCleverTapId);
	return CleverTapInstance;
}

bool SetDebugLevel(JNIEnv* Env, ECleverTapLogLevel Level)
{
	const char* LevelName = CleverTapLogLevelJavaName(Level);
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::SetDebugLevel(%hs)"), LevelName);
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID SetDebugLogLevelMethod = GetStaticMethodID(
		Env, CleverTapAPIClass, "setDebugLevel", "(Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;)V");
	if (!SetDebugLogLevelMethod)
	{
		return false;
	}

	jobject JavaLogLevel = CleverTapLogLevelToJava(Env, Level);
	if (!JavaLogLevel)
	{
		return false;
	}

	Env->CallStaticVoidMethod(CleverTapAPIClass, SetDebugLogLevelMethod, JavaLogLevel);
	bool bFailed = HandleException(Env, TEXT("setDebugLevel"));
	Env->DeleteLocalRef(JavaLogLevel);
	return bFailed;
}

bool LocalizeNotificationChannel(
	JNIEnv* Env, const FString& ChannelId, const FText& ChannelName, const FText& Description)
{
	static jclass BridgeClass = GetBridgeClass(Env);
	static jmethodID LocalizeChannelMethod = GetStaticMethodID(Env, BridgeClass, "localizeNotificationChannel",
		"(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
	if (!LocalizeChannelMethod)
	{
		return false;
	}

	jobject Context = FAndroidApplication::GetGameActivityThis();
	jstring JChannelId = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelId));
	jstring JChannelName = Env->NewStringUTF(TCHAR_TO_UTF8(*ChannelName.ToString()));
	jstring JDescription = Env->NewStringUTF(TCHAR_TO_UTF8(*Description.ToString()));
	bool Success = Env->CallStaticBooleanMethod(
		BridgeClass, LocalizeChannelMethod, Context, JChannelId, JChannelName, JDescription);
	if (HandleException(Env, "localizeNotificationChannel()"))
	{
		Success = false;
	}
	Env->DeleteLocalRef(JChannelId);
	Env->DeleteLocalRef(JChannelName);
	Env->DeleteLocalRef(JDescription);
	return Success;
}

bool LocalizeNotificationChannelGroup(JNIEnv* Env, const FString& GroupId, const FText& GroupName)
{
	static jclass BridgeClass = GetBridgeClass(Env);
	static jmethodID LocalizeGroupMethod = GetStaticMethodID(Env, BridgeClass, "localizeNotificationChannelGroup",
		"(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Z");
	if (!LocalizeGroupMethod)
	{
		return false;
	}

	jobject Context = FAndroidApplication::GetGameActivityThis();
	jstring JGroupId = Env->NewStringUTF(TCHAR_TO_UTF8(*GroupId));
	jstring JGroupName = Env->NewStringUTF(TCHAR_TO_UTF8(*GroupName.ToString()));
	bool Success = Env->CallStaticBooleanMethod(BridgeClass, LocalizeGroupMethod, Context, JGroupId, JGroupName);
	if (HandleException(Env, "localizeNotificationChannelGroup()"))
	{
		Success = false;
	}
	Env->DeleteLocalRef(JGroupId);
	Env->DeleteLocalRef(JGroupName);
	return Success;
}

jobject CreateUECleverTapListener(JNIEnv* Env, jobject CleverTapInstance, void* NativeInstance)
{
	static const char* ClassPath = "com/clevertap/android/unreal/UECleverTapListener";
	static jclass ListenerClass = CacheClass(Env, ClassPath);
	static jmethodID ListenerConstructor = GetMethodID(Env, ListenerClass, "<init>", "(J)V");
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
	return Listener;
}

void RegisterPushPermissionResponseListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID RegisterListenerMethod =
		GetMethodID(Env, CleverTapAPIClass, "registerPushPermissionNotificationResponseListener",
			"(Lcom/clevertap/android/sdk/PushPermissionResponseListener;)V");
	if (!RegisterListenerMethod)
	{
		return;
	}

	Env->CallVoidMethod(CleverTapInstance, RegisterListenerMethod, ListenerInstance);
	HandleException(Env, "registerPushPermissionNotificationResponseListener()");
}

void RegisterPushNotificationClickedListener(JNIEnv* Env, jobject CleverTapInstance, jobject ListenerInstance)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID SetListenerMethod = GetMethodID(Env, CleverTapAPIClass, "setCTPushNotificationListener",
		"(Lcom/clevertap/android/sdk/pushnotification/CTPushNotificationListener;)V");
	if (!SetListenerMethod)
	{
		return;
	}

	Env->CallVoidMethod(CleverTapInstance, SetListenerMethod, ListenerInstance);
	HandleException(Env, "setCTPushNotificationListener()");
}

FString GetCleverTapID(JNIEnv* Env, jobject CleverTapInstance)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID GetIDMethod = GetMethodID(Env, CleverTapAPIClass, "getCleverTapID", "()Ljava/lang/String;");
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
	Env->DeleteLocalRef(JavaID);

	return CleverTapID;
}

void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::OnUserLogin(Profile)"));
	UE_LOG(LogCleverTap, Log, TEXT("Profile: %s"), *JavaObjectToString(Env, Profile));
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID OnUserLoginMethod = GetMethodID(Env, CleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;)V");
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

	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID OnUserLoginMethod =
		GetMethodID(Env, CleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;Ljava/lang/String;)V");
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

	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID PushProfileMethod = GetMethodID(Env, CleverTapAPIClass, "pushProfile", "(Ljava/util/Map;)V");
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
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID PushEventMethod = GetMethodID(Env, CleverTapAPIClass, "pushEvent", "(Ljava/lang/String;)V");
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
	Env->DeleteLocalRef(JavaEventName);
}

void PushEvent(JNIEnv* Env, jobject CleverTapInstance, const FString& EventName, jobject Actions)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::PushEvent(%s, Actions)"), *EventName);
	UE_LOG(LogCleverTap, Log, TEXT("EventName: '%s', Actions: %s"), *EventName, *JavaObjectToString(Env, Actions));

	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID PushEventMethod =
		GetMethodID(Env, CleverTapAPIClass, "pushEvent", "(Ljava/lang/String;Ljava/util/Map;)V");
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

	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID PushChargedEventMethod =
		GetMethodID(Env, CleverTapAPIClass, "pushChargedEvent", "(Ljava/util/HashMap;Ljava/util/ArrayList;)V");
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

jobject GetProperty(JNIEnv* Env, jobject CleverTapInstance, const FString& Key)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID GetPropertyMethod =
		GetMethodID(Env, CleverTapAPIClass, "getProperty", "(Ljava/lang/String;)Ljava/lang/Object;");
	if (!GetPropertyMethod)
	{
		return nullptr;
	}

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	jobject JavaValue = Env->CallObjectMethod(CleverTapInstance, GetPropertyMethod, JavaKey);
	if (HandleException(Env, "getProperty()"))
	{
		// already logged; fall through
	}
	Env->DeleteLocalRef(JavaKey);

	return JavaValue;
}

static void DecrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, jobject Amount)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID DecrementMethod =
		GetMethodID(Env, CleverTapAPIClass, "decrementValue", "(Ljava/lang/String;Ljava/lang/Number;)V");
	if (!DecrementMethod)
	{
		return;
	}

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
	static jclass IntegerClass = CacheClass(Env, "java/lang/Integer");
	static jmethodID IntegerCtor = GetMethodID(Env, IntegerClass, "<init>", "(I)V");
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
}

void DecrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, double Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::DecrementValue(%s,%f)"), *Key, Amount);

	static jclass DoubleClass = CacheClass(Env, "java/lang/Double");
	static jmethodID DoubleCtor = GetMethodID(Env, DoubleClass, "<init>", "(D)V");
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
}

static void IncrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, jobject Amount)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID IncrementMethod =
		GetMethodID(Env, CleverTapAPIClass, "incrementValue", "(Ljava/lang/String;Ljava/lang/Number;)V");
	if (!IncrementMethod)
	{
		return;
	}

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
	static jclass IntegerClass = CacheClass(Env, "java/lang/Integer");
	static jmethodID IntegerCtor = GetMethodID(Env, IntegerClass, "<init>", "(I)V");
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
}

void IncrementValue(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, double Amount)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::IncrementValue(%s,%f)"), *Key, Amount);
	static jclass DoubleClass = CacheClass(Env, "java/lang/Double");
	static jmethodID DoubleCtor = GetMethodID(Env, DoubleClass, "<init>", "(D)V");
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
}

void AddMultiValueForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const FString& Value)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID AddMultiValueMethod =
		GetMethodID(Env, CleverTapAPIClass, "addMultiValueForKey", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (!AddMultiValueMethod)
	{
		return;
	}

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	jstring JavaValue = Env->NewStringUTF(TCHAR_TO_UTF8(*Value));
	Env->CallVoidMethod(CleverTapInstance, AddMultiValueMethod, JavaKey, JavaValue);
	HandleException(Env, "addMultiValueForKey()");
	Env->DeleteLocalRef(JavaKey);
	Env->DeleteLocalRef(JavaValue);
}

void AddMultiValuesForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const TArray<FString> Values)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID AddMultiValuesMethod =
		GetMethodID(Env, CleverTapAPIClass, "addMultiValuesForKey", "(Ljava/lang/String;Ljava/util/ArrayList;)V");
	if (!AddMultiValuesMethod)
	{
		return;
	}

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	jobject JavaValues = StringArrayToJavaArrayList(Env, Values);
	Env->CallVoidMethod(CleverTapInstance, AddMultiValuesMethod, JavaKey, JavaValues);
	HandleException(Env, "addMultiValuesForKey()");
	Env->DeleteLocalRef(JavaKey);
	Env->DeleteLocalRef(JavaValues);
}

void RemoveMultiValueForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const FString& Value)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID RemoveMultiValueMethod =
		GetMethodID(Env, CleverTapAPIClass, "removeMultiValueForKey", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (!RemoveMultiValueMethod)
	{
		return;
	}

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	jstring JavaValue = Env->NewStringUTF(TCHAR_TO_UTF8(*Value));
	Env->CallVoidMethod(CleverTapInstance, RemoveMultiValueMethod, JavaKey, JavaValue);
	HandleException(Env, "removeMultiValueForKey()");
	Env->DeleteLocalRef(JavaKey);
	Env->DeleteLocalRef(JavaValue);
}

void RemoveMultiValuesForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const TArray<FString>& Values)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID RemoveMultiValuesMethod =
		GetMethodID(Env, CleverTapAPIClass, "removeMultiValuesForKey", "(Ljava/lang/String;Ljava/util/ArrayList;)V");
	if (!RemoveMultiValuesMethod)
	{
		return;
	}

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	jobject JavaValues = StringArrayToJavaArrayList(Env, Values);
	Env->CallVoidMethod(CleverTapInstance, RemoveMultiValuesMethod, JavaKey, JavaValues);
	HandleException(Env, "removeMultiValuesForKey()");
	Env->DeleteLocalRef(JavaKey);
	Env->DeleteLocalRef(JavaValues);
}

void RemoveValueForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID RemoveValueMethod =
		GetMethodID(Env, CleverTapAPIClass, "removeValueForKey", "(Ljava/lang/String;)V");
	if (!RemoveValueMethod)
	{
		return;
	}

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	Env->CallVoidMethod(CleverTapInstance, RemoveValueMethod, JavaKey);
	HandleException(Env, "removeMultiValueForKey()");
	Env->DeleteLocalRef(JavaKey);
}

void SetMultiValuesForKey(JNIEnv* Env, jobject CleverTapInstance, const FString& Key, const TArray<FString> Values)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID SetMultiValuesMethod =
		GetMethodID(Env, CleverTapAPIClass, "setMultiValuesForKey", "(Ljava/lang/String;Ljava/util/ArrayList;)V");
	if (!SetMultiValuesMethod)
	{
		return;
	}

	jstring JavaKey = Env->NewStringUTF(TCHAR_TO_UTF8(*Key));
	jobject JavaValues = StringArrayToJavaArrayList(Env, Values);
	Env->CallVoidMethod(CleverTapInstance, SetMultiValuesMethod, JavaKey, JavaValues);
	HandleException(Env, "setMultiValuesForKey()");
	Env->DeleteLocalRef(JavaKey);
	Env->DeleteLocalRef(JavaValues);
}

bool IsPushPermissionGranted(JNIEnv* Env, jobject CleverTapInstance)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID IsGrantedMethod = GetMethodID(Env, CleverTapAPIClass, "isPushPermissionGranted", "()Z");
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
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID PromptMethod = GetMethodID(Env, CleverTapAPIClass, "promptForPushPermission", "(Z)V");
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
	static jclass BridgeClass = GetBridgeClass(Env);
	static jmethodID BuildMethod =
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
	Env->DeleteLocalRef(JavaMap);

	return ResultJson;
}

jobject CreatePushPrimerConfigJSON(JNIEnv* Env, const FCleverTapPushPrimerHalfInterstitialConfig& PrimerConfig)
{
	static jclass BridgeClass = GetBridgeClass(Env);
	static jmethodID BuildMethod = GetStaticMethodID(
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

	Env->DeleteLocalRef(JavaMap);

	return ResultJson;
}

void PromptPushPrimer(JNIEnv* Env, jobject CleverTapInstance, jobject PrimerConfigJSON)
{
	static jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	static jmethodID PromptMethod = GetMethodID(Env, CleverTapAPIClass, "promptPushPrimer", "(Lorg/json/JSONObject;)V");
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
