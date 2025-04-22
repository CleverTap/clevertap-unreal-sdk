// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapPropertiesJNI.h"

#include "Android/AndroidJNIUtilities.h"

#include "CleverTapLog.h"
#include "CleverTapLogLevel.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

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

	// Create the Timezone for the calendar
	jobject JavaTimeZone = CreateJavaTimeZone(Env, TimeZone);
	if (!JavaTimeZone)
	{
		return nullptr;
	}

	// Construct a Calendar instance with given TimeZone
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

FCleverTapProperties ConvertJavaMapToCleverTapProperties(JNIEnv* Env, jobject JavaMap)
{
	FCleverTapProperties Properties;
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is nullptr!"));
		return Properties;
	}

	jclass MapClass = Env->GetObjectClass(JavaMap);
	if (HandleExceptionOrError(Env, !MapClass, TEXT("Getting class for Map!")))
	{
		return Properties;
	}
	jmethodID EntrySetMethod = GetMethodID(Env, MapClass, "entrySet", "()Ljava/util/Set;");
	if (!EntrySetMethod)
	{
		return Properties;
	}
	jobject EntrySet = Env->CallObjectMethod(JavaMap, EntrySetMethod);
	if (HandleExceptionOrError(Env, !EntrySet, TEXT("Getting entrySet from Map!")))
	{
		return Properties;
	}

	jclass SetClass = Env->GetObjectClass(EntrySet);
	if (HandleExceptionOrError(Env, !SetClass, TEXT("Getting class for EntrySet!")))
	{
		return Properties;
	}
	jmethodID IteratorMethod = GetMethodID(Env, SetClass, "iterator", "()Ljava/util/Iterator;");
	if (!IteratorMethod)
	{
		return Properties;
	}
	jobject Iterator = Env->CallObjectMethod(EntrySet, IteratorMethod);
	if (HandleExceptionOrError(Env, !Iterator, TEXT("Getting iterator for EntrySet!")))
	{
		return Properties;
	}

	jclass IteratorClass = Env->GetObjectClass(Iterator);
	if (HandleExceptionOrError(Env, !IteratorClass, TEXT("Getting class of Iterator!")))
	{
		return Properties;
	}

	jmethodID HasNextMethod = GetMethodID(Env, IteratorClass, "hasNext", "()Z");
	jmethodID NextMethod = GetMethodID(Env, IteratorClass, "next", "()Ljava/lang/Object;");
	if (!HasNextMethod || !NextMethod)
	{
		return Properties;
	}

	jclass EntryClass = nullptr;
	jmethodID GetKeyMethod = nullptr;
	jmethodID GetValueMethod = nullptr;

	while (true)
	{
		bool HasNext = Env->CallBooleanMethod(Iterator, HasNextMethod);
		if (HandleException(Env, TEXT("iterator->hasNext()")))
		{
			break;
		}
		if (!HasNext)
		{
			break;
		}

		jobject Entry = Env->CallObjectMethod(Iterator, NextMethod);
		if (HandleException(Env, TEXT("iterator->next()")))
		{
			break;
		}
		if (!Entry)
		{
			continue;
		}

		// Only look up once
		if (!EntryClass)
		{
			EntryClass = Env->GetObjectClass(Entry);
			if (HandleExceptionOrError(Env, !EntryClass, TEXT("Getting class for Map Set Iterator Entry")))
			{
				return Properties;
			}
			GetKeyMethod = GetMethodID(Env, EntryClass, "getKey", "()Ljava/lang/Object;");
			GetValueMethod = GetMethodID(Env, EntryClass, "getValue", "()Ljava/lang/Object;");
			if (!GetKeyMethod || !GetValueMethod)
			{
				return Properties;
			}
		}

		// Get the Key and Value for this Entry
		jstring JavaKey = (jstring)Env->CallObjectMethod(Entry, GetKeyMethod);
		if (HandleExceptionOrError(Env, !JavaKey, TEXT("Iterator Entry getKey()!")))
		{
			continue;
		}
		jobject JavaValue = Env->CallObjectMethod(Entry, GetValueMethod);
		if (HandleExceptionOrError(Env, !JavaValue, TEXT("Iterator Entry getValue()!")))
		{
			Env->DeleteLocalRef(JavaKey);
			continue;
		}

		// Convert java key
		const char* KeyChars = Env->GetStringUTFChars(JavaKey, nullptr);
		FString Key = FString(UTF8_TO_TCHAR(KeyChars));
		Env->ReleaseStringUTFChars(JavaKey, KeyChars);

		// Convert Java value
		FCleverTapPropertyValue Value = ConvertJavaObjectToCleverTapPropertyValue(Env, JavaValue);
		Env->DeleteLocalRef(JavaValue);

		// Add to native Map
		if (!Properties.Contains(Key))
		{
			Properties.Add(Key, Value);
		}
		else
		{
			UE_LOG(LogCleverTap, Error, TEXT("Java Map has duplicate key \"%s\"! Ignoring additional value!"), *Key);
		}
	}

	if (EntryClass)
	{
		Env->DeleteLocalRef(EntryClass);
	}
	Env->DeleteLocalRef(SetClass);
	Env->DeleteLocalRef(MapClass);
	Env->DeleteLocalRef(IteratorClass);

	return Properties;
}

FCleverTapPropertyValue ConvertJavaObjectToCleverTapPropertyValue(JNIEnv* Env, jobject JavaValue)
{
	FCleverTapPropertyValue Value;
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is nullptr!"));
		return Value;
	}

	// todo implement me!

	return Value;
}

}}} // namespace CleverTapSDK::Android::JNI
