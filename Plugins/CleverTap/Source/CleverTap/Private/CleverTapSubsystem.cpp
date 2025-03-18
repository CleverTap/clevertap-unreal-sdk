// Copyright CleverTap All Rights Reserved.
#include "CleverTapSubsystem.h"

#include "CleverTapConfig.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapLog.h"
#include "CleverTapPlatformSDK.h"
#include "CleverTapUtilities.h"
#include "NullCleverTapInstance.h"
#include "UObject/UObjectBase.h"

//==================================================================================================
// UCleverTapSubsystem

namespace {

FNullCleverTapInstance& CommonNullInstance()
{
	static FNullCleverTapInstance Singleton;
	return Singleton;
}

const UCleverTapConfig* TryResolveCleverTapConfig(const UCleverTapConfig* MaybeExplicitConfig = nullptr)
{
	if (MaybeExplicitConfig)
	{
		return MaybeExplicitConfig;
	}

	if (!UObjectInitialized())
	{
		UE_LOG(LogCleverTap, Error, TEXT("InitializeSharedInstance() called before UObject initialization."
			" The default object for 'UCleverTapConfig' can't be accessed.")
		);
		return nullptr;
	}

	const UCleverTapConfig* const DefaultConfig
		= UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
	if (!IsValid(DefaultConfig))
	{
		return nullptr;
	}

	return DefaultConfig;
}

} // namespace (anonymous)

void UCleverTapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	CleverTapSDK::Ignore(Collection);

	const UCleverTapConfig* const Config = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
	if (!ensure(IsValid(Config)))
	{
		UE_LOG(LogCleverTap, Error, TEXT("UCleverTapConfig was invalid. Subsystem will not be initialized."));
		return;
	}

	if (Config->bAutoInitializeSharedInstance)
	{
		InitializeSharedInstance(Config);
	}
}

ICleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(const UCleverTapConfig* Config)
{
	if (SharedInstanceImpl != nullptr)
	{
		return *SharedInstanceImpl; // Already initialized
	}

	Config = TryResolveCleverTapConfig(Config);
	if (!IsValid(Config))
	{
		UE_LOG(LogCleverTap, Error, TEXT("UCleverTapConfig was invalid. Initialization will not occur."));
		return CommonNullInstance();
	}

	UE_LOG(LogCleverTap, Log, TEXT("Initializing the shared CleverTap instance"));

	return InitializeSharedInstance(FCleverTapInstanceConfig::FromCleverTapConfig(Config));
}

ICleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	if (SharedInstanceImpl != nullptr)
	{
		return *SharedInstanceImpl; // Already initialized
	}

	UE_LOG(LogCleverTap, Log, TEXT("Initializing the shared CleverTap instance"));

	SharedInstanceImpl = FCleverTapPlatformSDK::InitializeSharedInstance(Config);
	UE_CLOG(SharedInstanceImpl == nullptr, LogCleverTap, Fatal, TEXT("Failed to initialize the CleverTap shared instance"));
	return *SharedInstanceImpl;

}

ICleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(const FString& CleverTapId)
{
	if (SharedInstanceImpl != nullptr)
	{
		return *SharedInstanceImpl; // Already initialized
	}

	const UCleverTapConfig* const Config = TryResolveCleverTapConfig();
	if (!IsValid(Config))
	{
		UE_LOG(LogCleverTap, Error, TEXT("UCleverTapConfig was invalid. Initialization will not occur."));
		return CommonNullInstance();
	}

	return InitializeSharedInstance(*Config, CleverTapId);
}

ICleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(
	const UCleverTapConfig& Config, const FString& CleverTapId
)
{
	if (SharedInstanceImpl != nullptr)
	{
		return *SharedInstanceImpl; // Already initialized
	}

	return InitializeSharedInstance(FCleverTapInstanceConfig::FromCleverTapConfig(&Config), CleverTapId);
}

ICleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId
)
{
	if (SharedInstanceImpl != nullptr)
	{
		return *SharedInstanceImpl; // Already initialized
	}

	if (CleverTapId.IsEmpty())
	{
		UE_LOG(LogCleverTap, Warning, TEXT("InitializeSharedInstance() with CleverTap Id was passed an"
			" empty id. Defaulting to InitializeSharedInstance() without a custom CleverTap Id.")
		);
		return InitializeSharedInstance(Config);
	}

	UE_LOG(LogCleverTap, Log, TEXT("Initializing the shared CleverTap instance with CleverTap Id '%s'"),
		*CleverTapId
	);

	SharedInstanceImpl = FCleverTapPlatformSDK::InitializeSharedInstance(Config, CleverTapId);
	UE_CLOG(SharedInstanceImpl == nullptr, LogCleverTap, Fatal, TEXT("Failed to initialize the CleverTap shared instance"));
	return *SharedInstanceImpl;
}

bool UCleverTapSubsystem::IsSharedInstanceInitialized() const
{
	return SharedInstanceImpl.IsValid();
}

void UCleverTapSubsystem::SetLogLevel(ECleverTapLogLevel Level)
{
	FCleverTapPlatformSDK::SetLogLevel(Level);
}

ICleverTapInstance& UCleverTapSubsystem::SharedInstance()
{
	if (SharedInstanceImpl == nullptr)
	{
		return InitializeSharedInstance();
	}
	return *SharedInstanceImpl;
}

void UCleverTapSubsystem::BlueprintInitializeSharedInstance(const UCleverTapConfig* Config)
{
	InitializeSharedInstance(Config);
}

void UCleverTapSubsystem::BlueprintInitializeSharedInstanceWithId(
	const UCleverTapConfig* Config, const FString& CleverTapId
)
{
	if (Config)
	{
		InitializeSharedInstance(*Config, CleverTapId);
	}
	else
	{
		InitializeSharedInstance(CleverTapId);
	}
}

