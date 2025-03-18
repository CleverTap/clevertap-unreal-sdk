// Copyright CleverTap All Rights Reserved.
#include "CleverTapModule.h"

#include "CleverTapConfig.h"
#include "CleverTapLog.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FCleverTapModule"

void FCleverTapModule::StartupModule()
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTap plugin startup"));

	ISettingsModule* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "CleverTap",
			LOCTEXT("CleverTapSettingsName", "CleverTap SDK"),
			LOCTEXT("CleverTapSettingsDescription", "Settings for the CleverTap SDK plugin"),
			GetMutableDefault<UCleverTapConfig>());
	}
}

void FCleverTapModule::ShutdownModule()
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTap plugin shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCleverTapModule, CleverTap)
