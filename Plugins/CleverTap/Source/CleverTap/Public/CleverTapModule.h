// Copyright CleverTap All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCleverTapModule : public IModuleInterface
{
public:
	// <IModuleInterface>
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// </IModuleInterface>
};

