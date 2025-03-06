// Copyright CleverTap All Rights Reserved.

#include "CleverTapSubsystem.h"

#include "CleverTapConfig.h"
#include "CleverTapLog.h"
#include "CleverTapPlatformSDK.h"
#include "CleverTapUtilities.h"
#include "UObject/UObjectBase.h"

//==================================================================================================
// UCleverTapSubsystem::FCommonInstanceDeleter

void UCleverTapSubsystem::FCommonInstanceDeleter::operator()(FCleverTapInstance* Ptr) const
{
	if (Ptr != nullptr)
	{
		FCleverTapPlatformSDK::DestroyInstance(*Ptr);
	}
}

//==================================================================================================
// UCleverTapSubsystem

void UCleverTapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	CleverTap::Ignore(Collection);

	const UCleverTapConfig* const Config = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
	if (!ensure(IsValid(Config)))
	{
		UE_LOG(LogCleverTap, Error, TEXT("UCleverTapConfig was invalid. Subsystem will not be initialized."));
		return;
	}

	if (Config->bAutoInitializeCommonInstance)
	{
		InitializeCommonInstance(Config);
	}
}

void UCleverTapSubsystem::InitializeCommonInstance(const UCleverTapConfig* Config)
{
	if (!ensure(UObjectInitialized()))
	{
		UE_LOG(LogCleverTap, Error, TEXT("InitializeCleverTapSDK() called before UObject initialization. The"
			" default object for 'UCleverTapConfig' can't be accessed.")
		);
		return;
	}

	if (Config == nullptr)
	{
		Config = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
		if (!ensure(IsValid(Config)))
		{
			UE_LOG(LogCleverTap, Error, TEXT("UCleverTapConfig was invalid. Initialization will not occur."));
			return;
		}
	}

	UE_LOG(LogCleverTap, Log, TEXT("Initializing the common (shared) CleverTap instance"));

	FCleverTapInstance* const MaybeInstance = FCleverTapPlatformSDK::TryInitializeCommonInstance(*Config);
	if (MaybeInstance != nullptr)
	{
		CommonInstance.Reset(MaybeInstance);
	}
}

