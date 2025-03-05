// Copyright CleverTap All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Templates/UniquePtr.h"
#include "CleverTapSubsystem.generated.h"

class FCleverTapInstance;
class FCleverTapInstanceConfig;
class UCleverTapConfig;

/**
 * A UEngineSubsystem for interaction with the CleverTap SDK
 */
UCLASS(BlueprintType, ClassGroup=CleverTap)
class CLEVERTAP_API UCleverTapSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the common (shared) CleverTap instance.
	 */
	UFUNCTION(BlueprintCallable)
	void InitializeCommonInstance(const UCleverTapConfig* CommonConfig = nullptr);

	// <UEngineSubsystem>
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// </UEngineSubsystem>

private:
	struct FCommonInstanceDeleter
	{
		void operator()(FCleverTapInstance* Ptr) const;
	};
	TUniquePtr<FCleverTapInstance, FCommonInstanceDeleter> CommonInstance;
};

