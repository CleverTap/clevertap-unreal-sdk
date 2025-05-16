// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapInstance.h"
#include "CleverTapLogLevel.h"
#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "CleverTapSubsystem.generated.h"

struct FCleverTapInstanceConfig;
class UCleverTapConfig;

/**
 * A UEngineSubsystem for interaction with the CleverTap SDK.
 *  Note: UCleverTapSubsystem must be initialized before a call to FPlatformMisc::RegisterForRemoteNotifications() is
 *  made. If any of the UCleverTapInstance push primer methods are used then RegisterForRemoteNotifications() doesn't
 *  need to be called.
 */
UCLASS(BlueprintType, ClassGroup = CleverTap)
class CLEVERTAP_API UCleverTapSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Explicitly initialize the shared CleverTap instance.
	 */
	UCleverTapInstance& InitializeSharedInstance(const UCleverTapConfig* Config = nullptr);

	/**
	 * Explicitly initialize the shared CleverTap instance.
	 */
	UCleverTapInstance& InitializeSharedInstance(const FCleverTapInstanceConfig& Config);

	/**
	 * Explicitly initialize the shared CleverTap instance with a custom CleverTap Id.
	 */
	UCleverTapInstance& InitializeSharedInstance(const FString& CleverTapId);

	/**
	 * Explicitly initialize the shared CleverTap instance with a custom CleverTap Id.
	 */
	UCleverTapInstance& InitializeSharedInstance(const UCleverTapConfig& Config, const FString& CleverTapId);

	/**
	 * Explicitly initialize the shared CleverTap instance with a custom CleverTap Id.
	 */
	UCleverTapInstance& InitializeSharedInstance(const FCleverTapInstanceConfig& Config, const FString& CleverTapId);

	/**
	 * Returns true if the shared instance has been initialized.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsSharedInstanceInitialized() const;

	/**
	 * Sets platform SDK's current log level and also adjust's the shared instance's log level.
	 */
	UFUNCTION(BlueprintCallable)
	void SetLogLevel(ECleverTapLogLevel Level);

	/**
	 * Get the shared CleverTap API instance. If the instance has not been initialized then
	 *  an attempt to initialize it will be made as if calling InitializeSharedInstance().
	 */
	UCleverTapInstance& SharedInstance();

	// <UEngineSubsystem>
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// </UEngineSubsystem>

private:
	/** Install the no-op implementation as the shared instance.
	 *  Used when there's porblems with the platform-specific initialization */
	UCleverTapInstance& InitializeSharedInstanceWithNullImplementation();

	/**
	 * Explicitly initialize the shared CleverTap instance.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "InitializeSharedInstance"))
	void BlueprintInitializeSharedInstance(const UCleverTapConfig* Config);

	/**
	 * Explicitly initialize the shared CleverTap instance with an optional custom CleverTap
	 *  Id.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "InitializeSharedInstanceWithId"))
	void BlueprintInitializeSharedInstanceWithId(const UCleverTapConfig* Config, const FString& CleverTapId);

	void AddRemoteNotificationTokenListener();
	void OnRegisteredForRemoteNotifications(TArray<uint8> Token);

private:
	UPROPERTY(Transient)
	UCleverTapInstance* SharedInstanceImpl = nullptr;

	UPROPERTY(Transient)
	TArray<uint8> SavedRemoteNotificationToken;
};
