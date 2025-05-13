// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"

#include "Android/AndroidApplication.h"

namespace CleverTapSDK { namespace Android { namespace JNI {

enum class ETimeZone
{
	UTC,
	Local
};

jobject ConvertCleverTapPropertiesToJavaMap(JNIEnv* Env, const FCleverTapProperties& Properties);
FCleverTapProperties ConvertJavaMapToCleverTapProperties(JNIEnv* Env, jobject JavaMap);
FCleverTapProperties ConvertJavaCTInAppNotificationToCleverTapProperties(JNIEnv* Env, jobject JavaCTInAppNotification);

jobject ConvertArrayOfCleverTapPropertiesToJavaArrayOfMap(JNIEnv* Env, const TArray<FCleverTapProperties>& Array);

FCleverTapPropertyValue ConvertJavaObjectToCleverTapPropertyValue(JNIEnv* Env, jobject JavaValue);

jobject ConvertCleverTapDateToJavaDate(JNIEnv* Env, const FCleverTapDate& Date, ETimeZone TimeZone = ETimeZone::UTC);
FCleverTapDate ConvertJavaDateToCleverTapDate(JNIEnv* Env, jobject JavaDate, ETimeZone TimeZone = ETimeZone::UTC);

}}} // namespace CleverTapSDK::Android::JNI
