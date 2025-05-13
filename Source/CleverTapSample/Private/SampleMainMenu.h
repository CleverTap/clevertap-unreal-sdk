// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
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

UCLASS(BlueprintType)
class UCleverTapSampleProductList : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddProduct(const TArray<FCleverTapSampleKeyValuePair>& Params);

	int ProductCount() const;
	TArrayView<const FCleverTapSampleKeyValuePair> GetProductParameters(int Index) const;

private:
	TArray<TArray<FCleverTapSampleKeyValuePair>> ProductParameters;
};

/**
 * Base class for the sample menu UI blueprint
 */
UCLASS()
class USampleMainMenu : public UUserWidget, public FTickableGameObject
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

	UFUNCTION(BlueprintCallable)
	void PushProfileDataTypeTest();

	UFUNCTION(BlueprintCallable)
	void PromptForPushPermissionWithoutPrimer();

	UFUNCTION(BlueprintCallable)
	void PromptForPushPermissionWithAlertPrimer();

	UFUNCTION(BlueprintCallable)
	void PromptForPushPermissionWithHalfInterstitialPrimer();

	UFUNCTION(BlueprintCallable)
	void RecordEvent(const FString& EventName, const TArray<FCleverTapSampleKeyValuePair>& Params);

	UFUNCTION(BlueprintCallable)
	void RecordChargedEvent(
		const TArray<FCleverTapSampleKeyValuePair>& Params, const UCleverTapSampleProductList* Products);

	// TODO: Move to ICleverTapInstance blueprint wrapper
	UFUNCTION(BlueprintCallable)
	void SetOptOut(bool bIsOptingOut);

	// TODO: Move to ICleverTapInstance blueprint wrapper
	UFUNCTION(BlueprintCallable)
	void SetNetworkInformationRecording(bool bEnableCollection);

	// <FTickableGameObject>
	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
	ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	// </FTickableGameObject>

private:
	void PopulateUI();
	void ConfigureSharedInstance();
	void OnPushPermissionResponse(bool bGranted);
	void OnPushNotificationClicked(const FCleverTapProperties& NotificationPayload);
	void OnInAppNotificationShown(const FCleverTapProperties& NotificationPayload);
	void OnInAppNotificationDismissed(const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras);
	void OnOpenUrl(const FString& Url);

private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* CleverTapIdText;

	UPROPERTY()
	UCleverTapSubsystem* CleverTapSys;

	FString LastSeenCleverTapId;
	bool bNeedsPushStatusRefresh{ false };

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidgetSwitcher* TabSwitcher;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* PushPermissionGrantedText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* PushNotificationClickedText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* DeepLinkText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* InAppShownText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* InAppDismissedText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* InAppDismissedActionText;
};
