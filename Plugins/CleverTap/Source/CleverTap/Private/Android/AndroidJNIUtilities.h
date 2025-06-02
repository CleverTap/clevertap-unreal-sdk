// Copyright CleverTap All Rights Reserved.
#pragma once

#include "Android/AndroidApplication.h"

/** Java Native Interface (JNI) Utilities.
 *
 *  Utility wrappers to reduce the complexity & boilerplate from JNI wrapper implementations.
 *
 *  If ShouldCrashOnJNIException() is enabled, failing calls will crash with detailed logs; otherwise, failures will log
 * and return nullptr/false.
 */
namespace CleverTapSDK { namespace Android { namespace JNI {

/** Get the current JNI environment.
 *
 * Note: JNI Environments are thread-specific!
 */
JNIEnv* GetJNIEnv();

/** Enable crashing on JNI exceptions.
 *
 * Typically this should be disabled, but during development it can be
 * useful to enable crashing here to be highlight any JNI problems you may
 * be encountering.
 *
 * When enabled, a JNI exception will terminate the program with a Fatal log.
 * When disabled, the exception details will be logged as Errors and the program
 * will continue.
 */
void SetCrashOnJNIException(bool CrashOnException);

/** Returns true if we should let the application crash when it encounteres a JNI exception.
 *
 *  This defaults false for shipping builds, true for all others.
 */
bool ShouldCrashOnJNIException();

/** Returns true if the given JNIEnv has thrown an exception that needs to be handled.
 */
bool ExceptionThrown(JNIEnv* Env);

/** Handle an exception in the JNIEnv or an error condition.
 *  Returns true if an exception was handled or an error condition reported, false if all is well.
 *
 *  Most JNIEnv methods can throw a JNI exception. If these are not checked & cleared before the next call,
 *  then the application will crash.
 *
 *  Call this function after any JNI method that could possibly return an error condition to cleanly handle it.
 *  For example:
 *
 *      jobject JavaCalendar = Env->NewObject(CalendarClass, CalendarCtor, JavaTimeZone);
 *      bool bCalendarConstructionFailed = HandleExceptionOrError(Env, !JavaCalendar, TEXT("Calendar Constructor"));
 */
bool HandleExceptionOrError(JNIEnv* Env, bool bIsError, const FString& Context);

/** Handle any exception in the JNIEnv, logging the failure context.
 *  Returns true if an exception was handled, false if all is well.
 *
 *  Most JNIEnv methods can throw a JNI exception. If these are not checked & cleared before the next call,
 *  then the application will crash.
 *
 *  Call this function after any JNI method that can't also return an error condition to cleanly handle it:
 *  for example:
 *
 *      bool HasNext = Env->CallBooleanMethod(Iterator, HasNextMethod);
 *      if (HandleException(Env, TEXT("iterator->hasNext()")))
 *
 */
bool HandleException(JNIEnv* Env, const FString& Context);

/** Loads & caches a java class as a GlobalRef.
 *
 *  Returns nullptr if the class cannot be found and ShouldCrashOnJNIException() is false.
 *
 *  Returned jclass is valid for the life of the JVM; suitable for persisting as a static.
 *  Do not release the returned reference!
 *
 *  Efficient if the class is already cached.
 */
jclass CacheClass(JNIEnv* Env, const char* ClassPath);

/** Loads a java class and returns a LocalRef.
 *
 *  Returns nullptr if the class cannot be found and ShouldCrashOnJNIException() is false.
 *
 *  LocalRefs are not suitable for long term storage; call Env->DeleteLocalRef(ReturnedClass) to release.
 */
jclass LoadClass(JNIEnv* Env, const char* ClassPath);

/** Returns the name of the given class as an Unreal FString */
FString GetClassName(JNIEnv* Env, jclass Class);

/** Wrapper for JNIEnv::GetMethodID() with built-in exception & error handling.
 *
 *  Returns nullptr if the method cannot be found and ShouldCrashOnJNIException() is false.
 */
jmethodID GetMethodID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature);

/** Wrapper for JNIEnv::GetStaticMethodID() with built-in exception & error handling.
 *
 *  Returns nullptr if the method cannot be found and ShouldCrashOnJNIException() is false.
 */
jmethodID GetStaticMethodID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature);

/** Wrapper for JNIEnv::GetStaticFieldID() with built-in exception & error handling.
 *
 *  Returns nullptr if the method cannot be found and ShouldCrashOnJNIException() is false.
 */
jfieldID GetStaticFieldID(JNIEnv* Env, jclass Class, const char* Name, const char* Signature);

/** Wrapper for access to the Java Application instance.
 *
 *  Returns nullptr if the method cannot be found and ShouldCrashOnJNIException() is false.
 */
jobject GetJavaApplication(JNIEnv* Env);

/** Access to the java bridge class that provides java based helpers */
jclass GetBridgeClass(JNIEnv* Env);

/** Converts any Java object to its default string representation as an Unreal FString */
FString JavaObjectToString(JNIEnv* Env, jobject JavaObject);

/** Converts an unreal TArray<FString> to a Java ArrayList<String> */
jobject StringArrayToJavaArrayList(JNIEnv* Env, const TArray<FString>& StringArray);

/** Converts a Java JSONObject to a flattened Map<String, Object>.
 *
 *  Nested objects are flattened using dot-separated keys (e.g., "foo.bar").
 *  Arrays are converted to ArrayList<String> where possible.
 *  Preserves native Java types (String, Number, Boolean)
 */
jobject ConvertJavaJsonObjectToFlatMap(JNIEnv* Env, jobject JavaJsonObject);

}}} // namespace CleverTapSDK::Android::JNI
