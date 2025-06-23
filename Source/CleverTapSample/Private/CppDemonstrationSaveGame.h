#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CppDemonstrationSaveGame.generated.h"

/**
 * Save state to persist login info
 */
UCLASS()
class UCppDemonstrationSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Email;

	UPROPERTY()
	FString Phone;

	UPROPERTY()
	FString Identity;

	UPROPERTY()
	FString CustomCleverTapId;

	UPROPERTY()
	bool bIsOptOut{};

	UPROPERTY()
	bool bIsOffline{};

	UPROPERTY()
	bool bIsNotRecordingNetInfo{};
};
