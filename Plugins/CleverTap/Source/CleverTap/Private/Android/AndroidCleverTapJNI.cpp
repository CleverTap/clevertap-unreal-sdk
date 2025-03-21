// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapJNI.h"

#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

static bool s_bCatchJNIExceptions = true; // todo hook this somewhere better

static bool JNIExceptionThrown(JNIEnv* Env, const FString& Context)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return true;
	}

	if (Env->ExceptionCheck())
	{
		UE_LOG(LogTemp, Error, TEXT("JNI Exception in %s"), *Context);

		if (s_bCatchJNIExceptions)
		{
			Env->ExceptionDescribe(); // Log stack trace
			Env->ExceptionClear();	  // Prevent crash
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Fatal, TEXT("JNI Exception in %s - Crashing due to strict mode"), *Context);
			Env->ExceptionDescribe();
			return true; // Let Unreal crash naturally
		}
	}
	return false;
}

JNIEnv* GetJNIEnv()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (Env == nullptr)
	{
		UE_LOG(LogCleverTap, Error, TEXT("FAndroidApplication::GetJavaEnv() returned nullptr!"));
	}
	return Env;
}

jclass LoadJavaClass(JNIEnv* Env, const char* ClassPath)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return nullptr;
	}

	// todo there has to be a simpler way to do this!
	FString Context = TEXT("Get GameAtivity class");
	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jclass ActivityClass = Env->GetObjectClass(Activity);
	if (JNIExceptionThrown(Env, Context) || !ActivityClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
		return nullptr;
	}

	Context = TEXT("Get getClassLoader method from GameActivity");
	jmethodID GetClassLoaderMethod = Env->GetMethodID(ActivityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if (JNIExceptionThrown(Env, Context) || !GetClassLoaderMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
		return nullptr;
	}

	Context = TEXT("Get ClassLoader from GameActivity");
	jobject ClassLoader = Env->CallObjectMethod(Activity, GetClassLoaderMethod);
	if (JNIExceptionThrown(Env, Context) || !ClassLoader)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
		return nullptr;
	}

	Context = TEXT("Find ClassLoader class");
	jclass ClassLoaderClass = Env->FindClass("java/lang/ClassLoader");
	if (JNIExceptionThrown(Env, Context) || !ClassLoaderClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
		return nullptr;
	}

	Context = TEXT("Get ClassLoader LoadClass method");
	jmethodID LoadClass = Env->GetMethodID(ClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if (JNIExceptionThrown(Env, Context) || !LoadClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
		return nullptr;
	}

	jstring ClassName = Env->NewStringUTF(ClassPath);
	jclass FoundClass = (jclass)Env->CallObjectMethod(ClassLoader, LoadClass, ClassName);
	if (JNIExceptionThrown(Env, TEXT("LoadClass")) || !FoundClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to load class: %s"), *FString(ClassPath));
	}
	return FoundClass;
}

jobject GetJavaApplication(JNIEnv* Env)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return nullptr;
	}

	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jclass ActivityClass = Env->GetObjectClass(Activity);
	FString Context = TEXT("GetMethodId GameActivity.getApplication()");
	jmethodID GetApplicationMethod = Env->GetMethodID(ActivityClass, "getApplication", "()Landroid/app/Application;");
	if (JNIExceptionThrown(Env, Context) || !GetApplicationMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed."), *Context);
		return nullptr;
	}

	Context = TEXT("GameActivity.getApplication()");
	jobject Application = Env->CallObjectMethod(Activity, GetApplicationMethod);
	if (JNIExceptionThrown(Env, TEXT("LoadClass")) || !Application)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed."), *Context);
		return nullptr;
	}

	return Application;
}

void RegisterCleverTapLifecycleCallbacks(JNIEnv* Env)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::RegisterCleverTapLifecycleCallbacks()"));

	jobject Application = GetJavaApplication(Env);
	if (!Application)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to get Application instance!"));
		return;
	}

	// Find ActivityLifecycleCallback class
	jclass LifecycleCallbackClass = LoadJavaClass(Env, "com/clevertap/android/sdk/ActivityLifecycleCallback");
	if (!LifecycleCallbackClass)
	{
		return;
	}

	// Get the static register(Application) method
	jmethodID RegisterMethod =
		Env->GetStaticMethodID(LifecycleCallbackClass, "register", "(Landroid/app/Application;)V");
	if (!RegisterMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find ActivityLifecycleCallback#register method!"));
		return;
	}

	// Call register(Application)
	Env->CallStaticVoidMethod(LifecycleCallbackClass, RegisterMethod, Application);

	// Cleanup local references
	Env->DeleteLocalRef(Application);
	Env->DeleteLocalRef(LifecycleCallbackClass);
}

jclass GetCleverTapAPIClass(JNIEnv* Env)
{
	return LoadJavaClass(Env, "com.clevertap.android.sdk.CleverTapAPI");
}

jobject GetCleverTapInstance(JNIEnv* Env)
{
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return nullptr;
	}
	jmethodID GetInstanceMethod = Env->GetStaticMethodID(
		CleverTapAPIClass, "getDefaultInstance", "(Landroid/content/Context;)Lcom/clevertap/android/sdk/CleverTapAPI;");
	FString Context = TEXT("GetStaticMethodID CleverTapAPI.getDefaultInstance()");
	if (JNIExceptionThrown(Env, Context) || !GetInstanceMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed"), *Context);
		return nullptr;
	}

	Context = TEXT("getDefaultInstance()");
	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jobject CleverTapInstance = Env->CallStaticObjectMethod(CleverTapAPIClass, GetInstanceMethod, Activity);
	if (JNIExceptionThrown(Env, Context) || CleverTapInstance == nullptr)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed"), *Context);
		return nullptr;
	}
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
	FString Context = TEXT("Find CleverTapAPI$LogLevel.valueOf(String) method");
	jmethodID ValueOfMethod = Env->GetStaticMethodID(
		LogLevelClass, "valueOf", "(Ljava/lang/String;)Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;");
	if (JNIExceptionThrown(Env, Context) || !ValueOfMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
		Env->DeleteLocalRef(LogLevelClass);
		return nullptr;
	}
	// Get the enum constant from the name
	Context = TEXT("LogLevel.ValueOf(string)");
	jstring JavaLogLevelName = Env->NewStringUTF(LogLevelName);
	jobject LogLevelEnumValue = Env->CallStaticObjectMethod(LogLevelClass, ValueOfMethod, JavaLogLevelName);
	Env->DeleteLocalRef(JavaLogLevelName);
	Env->DeleteLocalRef(LogLevelClass);
	if (JNIExceptionThrown(Env, Context) || !LogLevelEnumValue)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to get LogLevel enum for: %s"), *LogLevelName);
		return nullptr;
	}
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
	jmethodID SetDebugLogLevelMethod = Env->GetStaticMethodID(
		CleverTapAPIClass, "setDebugLevel", "(Lcom/clevertap/android/sdk/CleverTapAPI$LogLevel;)V");
	if (JNIExceptionThrown(Env, TEXT("GetStaticMethodID - setDebugLevel")) || !SetDebugLogLevelMethod)
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
	Env->DeleteLocalRef(CleverTapAPIClass);
	Env->DeleteLocalRef(JavaLogLevel);
	if (JNIExceptionThrown(Env, TEXT("CallStaticVoidMethod - setDebugLevel")))
	{
		return false;
	}
	return true;
}

void ChangeCredentials(JNIEnv* Env, const FString& AccountId, const FString& Token, const FString& Region)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::ChangeCredentials()"));

	// Find the CleverTapAPI class
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}

	// Get the changeCredentials method ID
	FString Context = TEXT("Find CleverTapAPI changeCredentials method");
	jmethodID ChangeCredentialsMethod = Env->GetStaticMethodID(
		CleverTapAPIClass, "changeCredentials", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (JNIExceptionThrown(Env, Context) || !ChangeCredentialsMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
		return;
	}

	// Convert FString to jstring
	jstring jAccountId = Env->NewStringUTF(TCHAR_TO_UTF8(*AccountId));
	jstring jToken = Env->NewStringUTF(TCHAR_TO_UTF8(*Token));
	jstring jRegion = Env->NewStringUTF(TCHAR_TO_UTF8(*Region));

	// Call the method
	Env->CallStaticVoidMethod(CleverTapAPIClass, ChangeCredentialsMethod, jAccountId, jToken, jRegion);
	if (JNIExceptionThrown(Env, "ChangeCredentials"))
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to %s"), *Context);
	}

	// Cleanup local references
	Env->DeleteLocalRef(jAccountId);
	Env->DeleteLocalRef(jToken);
	Env->DeleteLocalRef(jRegion);
	Env->DeleteLocalRef(CleverTapAPIClass);
}

void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::OnUserLogin(Profile)"));
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}
	// Get the OnUserLogin method
	jmethodID OnUserLoginMethod = Env->GetMethodID(CleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (JNIExceptionThrown(Env, "GetMethodID onUserLogin"))
	{
		return;
	}

	// Call onUserLogin with the given profile
	Env->CallVoidMethod(CleverTapInstance, OnUserLoginMethod, Profile);
	if (JNIExceptionThrown(Env, "onUserLogin()"))
	{
		return;
	}
}

void OnUserLogin(JNIEnv* Env, jobject CleverTapInstance, jobject Profile, const FString& CleverTapID)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::OnUserLogin(Profile, CleverTapID)"));

	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}

	// Get the OnUserLogin method with the correct signature (Map + String)
	jmethodID OnUserLoginMethod =
		Env->GetMethodID(CleverTapAPIClass, "onUserLogin", "(Ljava/util/Map;Ljava/lang/String;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (JNIExceptionThrown(Env, "GetMethodID onUserLogin(Profile, CleverTapID)"))
	{
		return;
	}

	// Convert FString to jstring for CleverTapID
	jstring jCleverTapId = Env->NewStringUTF(TCHAR_TO_UTF8(*CleverTapID));
	if (!jCleverTapId)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("Failed to convert CleverTapID to jstring"));
		return;
	}

	// Call onUserLogin with profile and CleverTapID
	Env->CallVoidMethod(CleverTapInstance, OnUserLoginMethod, Profile, jCleverTapId);
	if (JNIExceptionThrown(Env, "onUserLogin(Profile, CleverTapID)"))
	{
		Env->DeleteLocalRef(jCleverTapId);
		return;
	}

	// Clean up JNI references
	Env->DeleteLocalRef(jCleverTapId);
}

void PushProfile(JNIEnv* Env, jobject CleverTapInstance, jobject Profile)
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::PushProfile()"));
	jclass CleverTapAPIClass = GetCleverTapAPIClass(Env);
	if (!CleverTapAPIClass)
	{
		return;
	}
	// Get the pushProfile method
	jmethodID PushProfileMethod = Env->GetMethodID(CleverTapAPIClass, "pushProfile", "(Ljava/util/Map;)V");
	Env->DeleteLocalRef(CleverTapAPIClass);
	if (JNIExceptionThrown(Env, "GetMethodID pushProfile"))
	{
		return;
	}

	// Call pushProfile with the given profile
	Env->CallVoidMethod(CleverTapInstance, PushProfileMethod, Profile);
	if (JNIExceptionThrown(Env, "pushProfile()"))
	{
		return;
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
	FString Context = TEXT("GetMethodID - getCleverTapID");
	jmethodID GetIDMethod = Env->GetMethodID(CleverTapAPIClass, "getCleverTapID", "()Ljava/lang/String;");
	if (JNIExceptionThrown(Env, Context) || !GetIDMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed"), *Context);
		Env->DeleteLocalRef(CleverTapAPIClass);
		return TEXT("");
	}

	// Call getCleverTapID() and retrieve a Java string
	Context = TEXT("CallObjectMethod - getCleverTapID");
	jstring JavaID = (jstring)Env->CallObjectMethod(CleverTapInstance, GetIDMethod);
	if (JNIExceptionThrown(Env, Context) || !JavaID)
	{
		UE_LOG(LogCleverTap, Error, TEXT("%s failed"), *Context);
		Env->DeleteLocalRef(CleverTapAPIClass);
		return TEXT("");
	}

	// Convert Java string to Unreal FString
	const char* IDChars = Env->GetStringUTFChars(JavaID, nullptr);
	FString CleverTapID = FString(IDChars);
	Env->ReleaseStringUTFChars(JavaID, IDChars);

	// Cleanup
	Env->DeleteLocalRef(JavaID);
	Env->DeleteLocalRef(CleverTapAPIClass);

	//	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::GetCleverTapID() = '%s'"), *CleverTapID);
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
	jmethodID GetInstanceMethod = Env->GetStaticMethodID(CalendarClass, "getInstance", "()Ljava/util/Calendar;");
	if (JNIExceptionThrown(Env, TEXT("GetStaticMethodID - Calendar getInstance")))
	{
		return nullptr;
	}
	jmethodID SetMethod = Env->GetMethodID(CalendarClass, "set", "(IIIIII)V");
	if (JNIExceptionThrown(Env, TEXT("GetMethodID - Calendar set")))
	{
		return nullptr;
	}
	jmethodID GetTimeMethod = Env->GetMethodID(CalendarClass, "getTime", "()Ljava/util/Date;");
	if (JNIExceptionThrown(Env, TEXT("GetMethodID - Calendar getTime")))
	{
		return nullptr;
	}

	// Create a Calendar instance
	jobject JavaCalendar = Env->CallStaticObjectMethod(CalendarClass, GetInstanceMethod);
	if (JNIExceptionThrown(Env, TEXT("Calendar GetInstanceMethod")) || !JavaCalendar)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to create Java Calendar instance"));
		return nullptr;
	}

	// Set the date (year, month, day, hour=0, min=0, sec=0)
	Env->CallVoidMethod(JavaCalendar, SetMethod, Date.Year, Date.Month - 1, Date.Day, 0, 0, 0);
	if (JNIExceptionThrown(Env, TEXT("Calendar Set")))
	{
		return nullptr;
	}

	// Convert Calendar to Date
	jobject JavaDate = Env->CallObjectMethod(JavaCalendar, GetTimeMethod);
	if (JNIExceptionThrown(Env, TEXT("Calendar getTime")))
	{
		return nullptr;
	}

	return JavaDate;
}

static void DebugLogJavaMap(JNIEnv* Env, jobject JavaMap)
{
	// todo make this generic toString()
	if (!JavaMap || !Env)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("DebugLogJavaMap: JavaMap is null"));
		return;
	}

	jclass MapClass = Env->FindClass("java/util/Map");
	jmethodID ToStringMethod = Env->GetMethodID(MapClass, "toString", "()Ljava/lang/String;");
	if (!ToStringMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to find Map.toString() method"));
		return;
	}

	jstring JavaString = (jstring)Env->CallObjectMethod(JavaMap, ToStringMethod);
	const char* CStr = Env->GetStringUTFChars(JavaString, nullptr);
	FString MapAsString(CStr);
	UE_LOG(LogCleverTap, Log, TEXT("Java Profile HashMap: %s"), *MapAsString);
	Env->ReleaseStringUTFChars(JavaString, CStr);
	Env->DeleteLocalRef(JavaString);
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

	jmethodID HashMapConstructor = Env->GetMethodID(HashMapClass, "<init>", "()V");
	if (JNIExceptionThrown(Env, TEXT("Get HashMap Constructor")))
	{
		return nullptr;
	}
	jobject JavaMap = Env->NewObject(HashMapClass, HashMapConstructor);
	if (JNIExceptionThrown(Env, TEXT("HashMap Constructor")))
	{
		return nullptr;
	}

	// Get the put() method for the HashMap
	jmethodID HashMapPut =
		Env->GetMethodID(HashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	if (JNIExceptionThrown(Env, TEXT("GetMethodID HashMap put")))
	{
		Env->DeleteLocalRef(JavaMap);
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
	if (JNIExceptionThrown(Env, TEXT("ConvertCleverTapPropertiesToJavaMap")))
	{
		return nullptr;
	}

	// DebugLogJavaMap(Env, JavaMap);

	return JavaMap;
}

bool InitCleverTap()
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTapSDK::Android::JNI::InitCleverTap()"));

	JNIEnv* Env = GetJNIEnv();
	if (Env == nullptr)
	{
		return false;
	}

	// This happens too late for the Android CleverTapSDK to be happy:
	//	RegisterCleverTapLifecycleCallbacks(Env);
	//	ChangeCleverTapCredentials(Env, project_id, Token, Region ));

	jobject CleverTapInstance = GetCleverTapInstance(Env);
	if (CleverTapInstance)
	{
		UE_LOG(LogCleverTap, Log, TEXT("CleverTap Initialized Successfully!"));
		return true;
	}
	else
	{
		UE_LOG(LogCleverTap, Error, TEXT("CleverTap Instance is NULL!"));
		return false;
	}
}

}}} // namespace CleverTapSDK::Android::JNI
