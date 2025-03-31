// Copyright CleverTap All Rights Reserved.
#include "CleverTapModule.h"

#include "CleverTapConfig.h"
#include "CleverTapLog.h"

#define LOCTEXT_NAMESPACE "FCleverTapModule"

void FCleverTapModule::StartupModule()
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTap plugin startup"));
}

void FCleverTapModule::ShutdownModule()
{
	UE_LOG(LogCleverTap, Log, TEXT("CleverTap plugin shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCleverTapModule, CleverTap)
