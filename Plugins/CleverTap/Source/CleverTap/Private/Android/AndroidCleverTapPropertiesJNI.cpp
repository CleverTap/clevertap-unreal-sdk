// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidCleverTapPropertiesJNI.h"

#include "Android/AndroidJNIUtilities.h"

#include "AndroidJNIUtilities.h"
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
	static jclass TimeZoneClass = CacheClass(Env, "java/util/TimeZone");
	static jmethodID GetTimeZoneMethod =
		GetStaticMethodID(Env, TimeZoneClass, "getTimeZone", "(Ljava/lang/String;)Ljava/util/TimeZone;");
	static jmethodID GetDefaultTimeZoneMethod =
		GetStaticMethodID(Env, TimeZoneClass, "getDefault", "()Ljava/util/TimeZone;");
	if (!GetTimeZoneMethod || !GetDefaultTimeZoneMethod)
	{
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

	return JavaTimeZone;
}

static jobject ConvertCleverTapDateToJavaDate(JNIEnv* Env, const FCleverTapDate& Date, ETimeZone TimeZone)
{
	// Find Java's Calendar class & methods we need
	static jclass CalendarClass = CacheClass(Env, "java/util/GregorianCalendar");
	static jmethodID CalendarCtor = GetMethodID(Env, CalendarClass, "<init>", "(Ljava/util/TimeZone;)V");
	static jmethodID SetMethod = GetMethodID(Env, CalendarClass, "set", "(IIIIII)V");
	static jmethodID GetTimeMethod = GetMethodID(Env, CalendarClass, "getTime", "()Ljava/util/Date;");
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
	static jclass HashMapClass = CacheClass(Env, "java/util/HashMap");
	static jmethodID HashMapConstructor = GetMethodID(Env, HashMapClass, "<init>", "()V");
	static jmethodID HashMapPut =
		GetMethodID(Env, HashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
	if (!HashMapConstructor || !HashMapPut)
	{
		return nullptr;
	}

	// Integer support
	static jclass IntegerClass = CacheClass(Env, "java/lang/Integer");
	static jmethodID IntegerConstructor = GetMethodID(Env, IntegerClass, "<init>", "(I)V");
	if (!IntegerConstructor)
	{
		return nullptr;
	}

	// Long support
	static jclass LongClass = CacheClass(Env, "java/lang/Long");
	static jmethodID LongConstructor = GetMethodID(Env, LongClass, "<init>", "(J)V");
	if (!LongConstructor)
	{
		return nullptr;
	}

	// Double support
	static jclass DoubleClass = CacheClass(Env, "java/lang/Double");
	static jmethodID DoubleConstructor = GetMethodID(Env, DoubleClass, "<init>", "(D)V");
	if (!DoubleConstructor)
	{
		return nullptr;
	}

	// Float Support
	static jclass FloatClass = CacheClass(Env, "java/lang/Float");
	static jmethodID FloatConstructor = GetMethodID(Env, FloatClass, "<init>", "(F)V");
	if (!FloatConstructor)
	{
		return nullptr;
	}

	// Bool support
	static jclass BooleanClass = CacheClass(Env, "java/lang/Boolean");
	static jmethodID BooleanConstructor = GetMethodID(Env, BooleanClass, "<init>", "(Z)V");
	if (!BooleanConstructor)
	{
		return nullptr;
	}

	// ArrayList support
	static jclass ArrayListClass = CacheClass(Env, "java/util/ArrayList");
	static jmethodID ArrayListConstructor = GetMethodID(Env, ArrayListClass, "<init>", "()V");
	static jmethodID ArrayListAdd = GetMethodID(Env, ArrayListClass, "add", "(Ljava/lang/Object;)Z");
	if (!ArrayListConstructor || !ArrayListAdd)
	{
		return nullptr;
	}

	// Construct a new java hashmap
	jobject JavaMap = Env->NewObject(HashMapClass, HashMapConstructor);
	if (HandleExceptionOrError(Env, !JavaMap, TEXT("HashMap Constructor")))
	{
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

	return JavaMap;
}

jobject ConvertArrayOfCleverTapPropertiesToJavaArrayOfMap(JNIEnv* Env, const TArray<FCleverTapProperties>& Array)
{
	static jclass ArrayListClass = CacheClass(Env, "java/util/ArrayList");
	static jmethodID ArrayListCtor = GetMethodID(Env, ArrayListClass, "<init>", "()V");
	static jmethodID AddMethod = GetMethodID(Env, ArrayListClass, "add", "(Ljava/lang/Object;)Z");
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

		// Get the Key for this Entry
		jstring JavaKey = (jstring)Env->CallObjectMethod(Entry, GetKeyMethod);
		if (HandleExceptionOrError(Env, !JavaKey, TEXT("Iterator Entry getKey()!")))
		{
			continue;
		}
		// Convert Key
		const char* KeyChars = Env->GetStringUTFChars(JavaKey, nullptr);
		FString Key = FString(UTF8_TO_TCHAR(KeyChars));
		Env->ReleaseStringUTFChars(JavaKey, KeyChars);
		Env->DeleteLocalRef(JavaKey);

		// Get Value for this entry
		jobject JavaValue = Env->CallObjectMethod(Entry, GetValueMethod);
		if (HandleExceptionOrError(Env, !JavaValue, TEXT("Iterator Entry getValue()!")))
		{
			continue;
		}
		// Convert Value
		FCleverTapPropertyValue Value = ConvertJavaObjectToCleverTapPropertyValue(Env, JavaValue);
		Env->DeleteLocalRef(JavaValue);

		// Add to native Map if unique
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
	// Integer support
	static jclass IntegerClass = CacheClass(Env, "java/lang/Integer");
	static jmethodID GetIntValue = GetMethodID(Env, IntegerClass, "intValue", "()I");

	// Long support
	static jclass LongClass = CacheClass(Env, "java/lang/Long");
	static jmethodID GetLongValue = GetMethodID(Env, LongClass, "longValue", "()J");

	// Double support
	static jclass DoubleClass = CacheClass(Env, "java/lang/Double");
	static jmethodID GetDoubleValue = GetMethodID(Env, DoubleClass, "doubleValue", "()D");

	// Float Support
	static jclass FloatClass = CacheClass(Env, "java/lang/Float");
	static jmethodID GetFloatValue = GetMethodID(Env, FloatClass, "floatValue", "()F");

	// Bool support
	static jclass BooleanClass = CacheClass(Env, "java/lang/Boolean");
	static jmethodID GetBooleanValue = GetMethodID(Env, BooleanClass, "booleanValue", "()Z");

	// String support
	static jclass StringClass = CacheClass(Env, "java/lang/String");

	// Collection support
	static jclass CollectionClass = CacheClass(Env, "java/util/Collection");
	static jmethodID CollectionIteratorMethod = GetMethodID(Env, CollectionClass, "iterator", "()Ljava/util/Iterator;");

	// Iterator support
	static jclass IteratorClass = CacheClass(Env, "java/util/Iterator");
	static jmethodID HasNextMethod = GetMethodID(Env, IteratorClass, "hasNext", "()Z");
	static jmethodID NextMethod = GetMethodID(Env, IteratorClass, "next", "()Ljava/lang/Object;");

	// Make sure we got everything
	if (!GetIntValue || !GetLongValue || !GetDoubleValue || !GetFloatValue || !GetBooleanValue || !StringClass
		|| !CollectionIteratorMethod || !HasNextMethod || !NextMethod)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Missing Vital Methods!"));
		return FCleverTapPropertyValue();
	}

	if (Env->IsInstanceOf(JavaValue, IntegerClass))
	{
		int32 Result = Env->CallIntMethod(JavaValue, GetIntValue);
		HandleException(Env, TEXT("intValue()"));
		return FCleverTapPropertyValue(Result);
	}

	if (Env->IsInstanceOf(JavaValue, LongClass))
	{
		int64 Result = Env->CallLongMethod(JavaValue, GetLongValue);
		HandleException(Env, TEXT("longValue()"));
		return FCleverTapPropertyValue(Result);
	}

	if (Env->IsInstanceOf(JavaValue, DoubleClass))
	{
		double Result = Env->CallDoubleMethod(JavaValue, GetDoubleValue);
		HandleException(Env, TEXT("doubleValue()"));
		return FCleverTapPropertyValue(Result);
	}

	if (Env->IsInstanceOf(JavaValue, FloatClass))
	{
		double Result = Env->CallFloatMethod(JavaValue, GetFloatValue);
		HandleException(Env, TEXT("floatValue()"));
		return FCleverTapPropertyValue(Result);
	}

	if (Env->IsInstanceOf(JavaValue, BooleanClass))
	{
		bool Result = Env->CallBooleanMethod(JavaValue, GetBooleanValue);
		HandleException(Env, TEXT("floatValue()"));
		return FCleverTapPropertyValue(Result);
	}

	if (Env->IsInstanceOf(JavaValue, StringClass))
	{
		jstring JavaString = (jstring)JavaValue;
		const char* ValueChars = Env->GetStringUTFChars(JavaString, nullptr);
		FString Result = FString(UTF8_TO_TCHAR(ValueChars));
		Env->ReleaseStringUTFChars(JavaString, ValueChars);
		return FCleverTapPropertyValue(Result);
	}

	// todo we need the Java date thing here too

	if (Env->IsInstanceOf(JavaValue, CollectionClass))
	{
		// Convert all colllections to TArray<FString>
		TArray<FString> StringArray;
		jobject Iterator = Env->CallObjectMethod(JavaValue, CollectionIteratorMethod);
		if (HandleExceptionOrError(Env, !Iterator, "getting Collection Iterator"))
		{
			return FCleverTapPropertyValue(StringArray);
		}
		while (1)
		{
			bool HasNext = Env->CallBooleanMethod(Iterator, HasNextMethod);
			if (HandleException(Env, TEXT("Iterator.hasNext()")) || !HasNext)
			{
				break;
			}
			jobject Element = Env->CallObjectMethod(Iterator, NextMethod);
			if (HandleExceptionOrError(Env, !Element, TEXT("Iterator.next()")))
			{
				break;
			}
			StringArray.Add(JavaObjectToString(Env, Element));
			Env->DeleteLocalRef(Element);
		}
		Env->DeleteLocalRef(Iterator);
		return FCleverTapPropertyValue(StringArray);
	}

	//  we made it to here without recogonizing a handled type; convert to string as a fallback
	UE_LOG(LogCleverTap, Error, TEXT("Unsupported property type '%s', converting to string!"),
		*GetClassName(Env, Env->GetObjectClass(JavaValue)));

	return FCleverTapPropertyValue(JavaObjectToString(Env, JavaValue));
}

}}} // namespace CleverTapSDK::Android::JNI
