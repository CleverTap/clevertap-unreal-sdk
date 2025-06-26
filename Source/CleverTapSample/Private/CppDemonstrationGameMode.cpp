// Copyright CleverTap All Rights Reserved.
#include "CppDemonstrationGameMode.h"

#include "CppDemonstrationHUD.h"

ACppDemonstrationGameMode::ACppDemonstrationGameMode(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	HUDClass = ACppDemonstrationHUD::StaticClass();
}
