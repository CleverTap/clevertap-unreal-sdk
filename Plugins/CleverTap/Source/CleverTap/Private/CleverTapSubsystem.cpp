// Copyright CleverTap All Rights Reserved.
#include "CleverTapSubsystem.h"

#include "CleverTapConfig.h"
#include "CleverTapInstanceConfig.h"
#include "CleverTapLog.h"
#include "CleverTapPlatformSDK.h"
#include "CleverTapUtilities.h"
#include "Misc/CoreDelegates.h"
#include "NullCleverTapInstance.h"
#include "UObject/UObjectBase.h"

//==================================================================================================
// UCleverTapSubsystem

namespace {

const UCleverTapConfig* TryResolveCleverTapConfig(const UCleverTapConfig* MaybeExplicitConfig = nullptr)
{
	if (MaybeExplicitConfig)
	{
		return MaybeExplicitConfig;
	}

	if (!UObjectInitialized())
	{
		UE_LOG(LogCleverTap, Error,
			TEXT("InitializeSharedInstance() called before UObject initialization."
				 " The default object for 'UCleverTapConfig' can't be accessed."));
		return nullptr;
	}

	const UCleverTapConfig* const DefaultConfig = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
	if (!IsValid(DefaultConfig))
	{
		return nullptr;
	}

	return DefaultConfig;
}

} // namespace

void UCleverTapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

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

	AddRemoteNotificationTokenListener();
}

UCleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(const UCleverTapConfig* Config)
{
	if (IsValid(SharedInstanceImpl))
	{
		return *SharedInstanceImpl; // Already initialized
	}

	Config = TryResolveCleverTapConfig(Config);
	if (!IsValid(Config))
	{
		UE_LOG(LogCleverTap, Error, TEXT("UCleverTapConfig was invalid. Initialization will not occur."));
		return NullInstance();
	}

	return InitializeSharedInstance(FCleverTapInstanceConfig::FromCleverTapConfig(Config));
}

UCleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(const FCleverTapInstanceConfig& Config)
{
	if (IsValid(SharedInstanceImpl))
	{
		return *SharedInstanceImpl; // Already initialized
	}

	UE_LOG(LogCleverTap, Log, TEXT("Initializing the shared CleverTap instance"));

	SharedInstanceImpl = FCleverTapPlatformSDK::InitializeSharedInstance(Config);
	UE_CLOG(
		!IsValid(SharedInstanceImpl), LogCleverTap, Fatal, TEXT("Failed to initialize the CleverTap shared instance"));

	if (SavedRemoteNotificationToken.Num() > 0)
	{
		FCleverTapPlatformSDK::SetRemoteNotificationToken(*SharedInstanceImpl, SavedRemoteNotificationToken);
	}

	return *SharedInstanceImpl;
}

UCleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(const FString& CleverTapId)
{
	if (IsValid(SharedInstanceImpl))
	{
		return *SharedInstanceImpl; // Already initialized
	}

	const UCleverTapConfig* const Config = TryResolveCleverTapConfig();
	if (!IsValid(Config))
	{
		UE_LOG(LogCleverTap, Error, TEXT("UCleverTapConfig was invalid. Initialization will not occur."));
		return NullInstance();
	}

	return InitializeSharedInstance(*Config, CleverTapId);
}

UCleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(
	const UCleverTapConfig& Config, const FString& CleverTapId)
{
	if (IsValid(SharedInstanceImpl))
	{
		return *SharedInstanceImpl; // Already initialized
	}

	return InitializeSharedInstance(FCleverTapInstanceConfig::FromCleverTapConfig(&Config), CleverTapId);
}

UCleverTapInstance& UCleverTapSubsystem::InitializeSharedInstance(
	const FCleverTapInstanceConfig& Config, const FString& CleverTapId)
{
	if (IsValid(SharedInstanceImpl))
	{
		return *SharedInstanceImpl; // Already initialized
	}

	if (CleverTapId.IsEmpty())
	{
		UE_LOG(LogCleverTap, Warning,
			TEXT("InitializeSharedInstance() with CleverTap Id was passed an"
				 " empty id. Defaulting to InitializeSharedInstance() without a custom CleverTap Id."));
		return InitializeSharedInstance(Config);
	}

	UE_LOG(LogCleverTap, Log, TEXT("Initializing the shared CleverTap instance with CleverTap Id '%s'"), *CleverTapId);

	SharedInstanceImpl = FCleverTapPlatformSDK::InitializeSharedInstance(Config, CleverTapId);
	UE_CLOG(
		!IsValid(SharedInstanceImpl), LogCleverTap, Fatal, TEXT("Failed to initialize the CleverTap shared instance"));

	if (SavedRemoteNotificationToken.Num() > 0)
	{
		FCleverTapPlatformSDK::SetRemoteNotificationToken(*SharedInstanceImpl, SavedRemoteNotificationToken);
	}

	return *SharedInstanceImpl;
}

UCleverTapInstance& UCleverTapSubsystem::NullInstance()
{
	if (!IsValid(NullInstanceImpl))
	{
		NullInstanceImpl = UNullCleverTapInstance::Create();
	}
	return *NullInstanceImpl;
}

bool UCleverTapSubsystem::IsSharedInstanceInitialized() const
{
	return IsValid(SharedInstanceImpl);
}

void UCleverTapSubsystem::SetLogLevel(ECleverTapLogLevel Level)
{
	FCleverTapPlatformSDK::SetLogLevel(Level);
}

UCleverTapInstance& UCleverTapSubsystem::SharedInstance()
{
	if (IsSharedInstanceInitialized() == false)
	{
		return InitializeSharedInstance();
	}
	return *SharedInstanceImpl;
}

UCleverTapInstance* UCleverTapSubsystem::BlueprintSharedInstance()
{
	if (IsSharedInstanceInitialized() == false)
	{
		return &InitializeSharedInstance();
	}
	return SharedInstanceImpl;
}

void UCleverTapSubsystem::BlueprintInitializeSharedInstance(const UCleverTapConfig* Config)
{
	InitializeSharedInstance(Config);
}

void UCleverTapSubsystem::BlueprintInitializeSharedInstanceWithId(
	const UCleverTapConfig* Config, const FString& CleverTapId)
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

void UCleverTapSubsystem::AddRemoteNotificationTokenListener()
{
	FCoreDelegates::ApplicationRegisteredForRemoteNotificationsDelegate.AddUObject(
		this, &UCleverTapSubsystem::OnRegisteredForRemoteNotifications);
}

void UCleverTapSubsystem::OnRegisteredForRemoteNotifications(TArray<uint8> Token)
{
	SavedRemoteNotificationToken = MoveTemp(Token);

	if (SharedInstanceImpl)
	{
		FCleverTapPlatformSDK::SetRemoteNotificationToken(*SharedInstanceImpl, SavedRemoteNotificationToken);
	}
}

UCleverTapInstance* UCleverTapSubsystemBlueprintLibrary::SharedCleverTapInstance()
{
	UCleverTapSubsystem* CleverTapSubsystem = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	check(CleverTapSubsystem);
	return &CleverTapSubsystem->SharedInstance();
}
