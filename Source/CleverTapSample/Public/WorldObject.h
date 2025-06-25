// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "WorldObject.generated.h"

/**
 * Base class that allows for blueprint function calls, unlike UObject
 */
UCLASS(Blueprintable)
class CLEVERTAPSAMPLE_API UWorldObject : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Initialize();

	// <UObject>
	UWorld* GetWorld() const override;
	void PostInitProperties() override;
	// </UObject>
};
