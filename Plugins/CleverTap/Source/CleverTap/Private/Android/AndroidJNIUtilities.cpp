// Copyright CleverTap All Rights Reserved.
#include "Android/AndroidJNIUtilities.h"

#include "CleverTapLog.h"
#include "CleverTapUtilities.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

static bool bCrashOnJNIException = false;

void SetCrashOnJNIException(bool CrashOnException)
{
	bCrashOnJNIException = CrashOnException;
}

bool ShouldCrashOnJNIException()
{
	return bCrashOnJNIException;
}

bool ExceptionThrown(JNIEnv* Env)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNI Error: Env is null!"));
		return true;
	}
	return Env->ExceptionCheck();
}

bool HandleExceptionOrError(JNIEnv* Env, bool bIsError, const FString& Context)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNI Error: Env is null! %s"), *Context);
		return true;
	}
	bool bIsException = Env->ExceptionCheck();
	if (!bIsError && !bIsException)
	{
		return false; // No error, nothing to handle
	}

	if (bIsException)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNI Exception! %s"), *Context);
		Env->ExceptionDescribe();
		if (bCrashOnJNIException)
		{
			UE_LOG(LogCleverTap, Fatal, TEXT("Crashing due to JNI Exception"));
		}
		else
		{
			Env->ExceptionClear();
		}
	}
	else
	{
		// an error with no exception
		UE_LOG(LogCleverTap, Error, TEXT("JNI Error! %s"), *Context);
	}

	//
	return true;
}

bool HandleException(JNIEnv* Env, const FString& Context)
{
	return HandleExceptionOrError(Env, false, Context);
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
	jobject Activity = FAndroidApplication::GetGameActivityThis();
	jclass ActivityClass = Env->GetObjectClass(Activity);
	if (HandleExceptionOrError(Env, !ActivityClass, TEXT("GetGameActivityThis() failed")))
	{
		return nullptr;
	}

	jmethodID GetClassLoaderMethod = Env->GetMethodID(ActivityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if (HandleExceptionOrError(
			Env, !GetClassLoaderMethod, TEXT("Failed to get getClassLoader method from GameActivity")))
	{
		return nullptr;
	}

	jobject ClassLoader = Env->CallObjectMethod(Activity, GetClassLoaderMethod);
	if (HandleExceptionOrError(Env, !ClassLoader, TEXT("Failed to get ClassLoader from GameActivity")))
	{
		return nullptr;
	}

	jclass ClassLoaderClass = Env->FindClass("java/lang/ClassLoader");
	if (HandleExceptionOrError(Env, !ClassLoaderClass, TEXT("Failed to find ClassLoader class")))
	{
		return nullptr;
	}

	jmethodID LoadClass = Env->GetMethodID(ClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if (HandleExceptionOrError(Env, !LoadClass, TEXT("Failed to get ClassLoader LoadClass method")))
	{
		return nullptr;
	}

	jstring ClassName = Env->NewStringUTF(ClassPath);
	jclass FoundClass = (jclass)Env->CallObjectMethod(ClassLoader, LoadClass, ClassName);
	if (ExceptionThrown(Env) || !FoundClass)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Failed to load class: %s"), *FString(ClassPath));
		HandleExceptionOrError(Env, !FoundClass, TEXT("LoadClass"));
		return nullptr;
	}

	return FoundClass;
}

FString GetJClassName(JNIEnv* Env, jclass Class)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return "";
	}
	if (!Class)
	{
		UE_LOG(LogCleverTap, Error, TEXT("Can't get name of null class!"));
		return "";
	}
	jclass ClassClass = Env->GetObjectClass(Class); // java.lang.Class
	if (HandleExceptionOrError(Env, !ClassClass, TEXT("GetObjectClass")))
	{
		return TEXT("");
	}
	jmethodID GetNameMethod = Env->GetMethodID(ClassClass, "getName", "()Ljava/lang/String;");
	if (HandleExceptionOrError(Env, !GetNameMethod, TEXT("GetMethodID getName")))
	{
		return TEXT("");
	}
	jstring NameString = (jstring)Env->CallObjectMethod(Class, GetNameMethod);
	if (HandleException(Env, TEXT("Class.getName")))
	{
		return TEXT("");
	}

	const char* NameChars = Env->GetStringUTFChars(NameString, nullptr);
	FString Result = UTF8_TO_TCHAR(NameChars);
	Env->ReleaseStringUTFChars(NameString, NameChars);
	Env->DeleteLocalRef(NameString);
	Env->DeleteLocalRef(ClassClass);

	return Result;
}

jmethodID GetMethodID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return nullptr;
	}
	jmethodID MethodId = Env->GetMethodID(Class, Name, Signature);
	if (ExceptionThrown(Env) || !MethodId)
	{
		HandleExceptionOrError(Env, !MethodId,
			FString::Printf(TEXT("GetMethodID %s %hs %hs failed"), *GetJClassName(Env, Class), Name, Signature));
		return nullptr;
	}
	return MethodId;
}

jmethodID GetStaticMethodID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return nullptr;
	}
	jmethodID MethodId = Env->GetStaticMethodID(Class, Name, Signature);
	if (ExceptionThrown(Env) || !MethodId)
	{
		HandleExceptionOrError(Env, !MethodId,
			FString::Printf(TEXT("GetStaticMethodID %s %hs %hs failed"), *GetJClassName(Env, Class), Name, Signature));
		return nullptr;
	}
	return MethodId;
}

jfieldID GetStaticFieldID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return nullptr;
	}
	jfieldID FieldId = Env->GetStaticFieldID(Class, Name, Signature);
	if (ExceptionThrown(Env) || !FieldId)
	{
		HandleExceptionOrError(Env, !FieldId,
			FString::Printf(TEXT("GetStaticFieldID %s %hs %hs failed"), *GetJClassName(Env, Class), Name, Signature));
		return nullptr;
	}
	return FieldId;
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
	if (HandleExceptionOrError(Env, !ActivityClass, TEXT("GetObjectClass ActivityClass failed.")))
	{
		return nullptr;
	}

	jmethodID GetApplicationMethod = GetMethodID(Env, ActivityClass, "getApplication", "()Landroid/app/Application;");
	if (!GetApplicationMethod)
	{
		return nullptr;
	}

	jobject Application = Env->CallObjectMethod(Activity, GetApplicationMethod);
	if (HandleExceptionOrError(Env, !Application, TEXT("GameActivity.getApplication() failed.")))
	{
		return nullptr;
	}

	return Application;
}

FString JavaObjectToString(JNIEnv* Env, jobject JavaObject)
{
	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return TEXT("<null env>");
	}
	if (!JavaObject)
	{
		UE_LOG(LogCleverTap, Warning, TEXT("JavaObjectToString: JavaObject is null"));
		return TEXT("<null>");
	}

	jclass ObjectClass = Env->GetObjectClass(JavaObject);
	if (HandleExceptionOrError(Env, !ObjectClass, TEXT("GetObjectClass()")))
	{
		return TEXT("<error>");
	}
	jmethodID ToStringMethod = GetMethodID(Env, ObjectClass, "toString", "()Ljava/lang/String;");
	if (!ToStringMethod)
	{
		return TEXT("<error>");
	}
	jstring JavaStr = (jstring)Env->CallObjectMethod(JavaObject, ToStringMethod);
	if (HandleExceptionOrError(Env, !JavaStr, TEXT("JavaObjectToString object.toString()")))
	{
		return TEXT("<error>");
	}

	const char* UTFChars = Env->GetStringUTFChars(JavaStr, nullptr);
	FString Result(UTFChars);
	Env->ReleaseStringUTFChars(JavaStr, UTFChars);
	Env->DeleteLocalRef(JavaStr);
	Env->DeleteLocalRef(ObjectClass);
	return Result;
}

FString JavaStringArrayToString(JNIEnv* Env, jobjectArray Array)
{
	// todo make this handle arrays of anything, not just strings, rename to JavaArrayToString

	if (!Env)
	{
		UE_LOG(LogCleverTap, Error, TEXT("JNIEnv is null!"));
		return TEXT("<null env>");
	}
	jsize Length = Env->GetArrayLength(Array);
	if (HandleException(Env, TEXT("GetArrayLength()")))
	{
		return TEXT("<error>");
	}

	FString Output = TEXT("[");

	for (jsize i = 0; i < Length; ++i)
	{
		jstring JStr = (jstring)Env->GetObjectArrayElement(Array, i);
		if (HandleException(Env, TEXT("GetObjectArrayElement()")))
		{
			return TEXT("<error>");
		}

		const char* UTF = Env->GetStringUTFChars(JStr, nullptr);
		if (HandleException(Env, TEXT("GetStringUTFChars()")))
		{
			return TEXT("<error>");
		}
		FString Entry = UTF ? UTF8_TO_TCHAR(UTF) : TEXT("<null>");
		Env->ReleaseStringUTFChars(JStr, UTF);
		Env->DeleteLocalRef(JStr);

		Output += Entry;
		if (i < Length - 1)
		{
			Output += TEXT(", ");
		}
	}

	Output += TEXT("]");
	return Output;
}

}}} // namespace CleverTapSDK::Android::JNI
