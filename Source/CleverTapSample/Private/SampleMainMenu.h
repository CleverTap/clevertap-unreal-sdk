// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SampleMainMenu.generated.h"

class UCleverTapSubsystem;

USTRUCT(BlueprintType)
struct FCleverTapSampleKeyValuePair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;
};

/**
 * Base class for the sample menu UI blueprint
 */
UCLASS()
class USampleMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void InitializeSharedInstanceFromConfig();

	UFUNCTION(BlueprintCallable)
	void ExplicitlyInitializeSharedInstance(const FString& Id, const FString& Token, const FString& RegionCode);

	UFUNCTION(BlueprintCallable)
	void OnUserLogin(const FString& Name, const FString& Email, const FString& Identity);

	UFUNCTION(BlueprintCallable)
	void OnUserLoginWithCleverTapId(
		const FString& Name, const FString& Email, const FString& Identity, const FString& CleverTapId);

	UFUNCTION(BlueprintCallable)
	void PushProfile(const FString& Name, const FString& Email, const FString& Phone,
		const TArray<FCleverTapSampleKeyValuePair>& Params);

private:
	void PopulateUI() const;

private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* CleverTapIdText;

	UPROPERTY()
	UCleverTapSubsystem* CleverTapSys;
};
