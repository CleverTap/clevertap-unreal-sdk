// Copyright CleverTap All Rights Reserved.
#include "CleverTapConfig.h"

ECleverTapLogLevel UCleverTapConfig::GetActiveLogLevel() const
{
#if UE_BUILD_SHIPPING
	return ShippingLogLevel;
#else
	return DevelopmentLogLevel;
#endif
}

