// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapInstance.h"

/**
 * A CleverTap instance implementation that does nothing and returns default values.
 */
class FNullCleverTapInstance : public ICleverTapInstance
{
public:
	FString GetCleverTapId() const override;
};
