// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SampleMainMenu.generated.h"

class UCleverTapSubsystem;

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

private:
	void PopulateUI() const;

private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* CleverTapIdText;

	UPROPERTY()
	UCleverTapSubsystem* CleverTapSys;
};
