// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CleverTapProperties.h"
#include "Tickable.h"
#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/PushTabViewModelInterface.h"
#include "CppPushTabViewModel.generated.h"

enum class ECleverTapPushPermissionStatus : uint8;

/**
 * C++ implementation of the IPushTabViewModelInterface
 */
UCLASS(NotBlueprintable)
class UCppPushTabViewModel : public UCppViewModelBase, public FTickableGameObject, public IPushTabViewModelInterface
{
	GENERATED_BODY()

public:
	void Edit(TFunctionRef<void(MutableContext&, UCppPushTabViewModel&)> EditFn);
	UCppPushTabViewModel& SetPushNotificationData(MutableContext&, const FString& Value);
	UCppPushTabViewModel& SetPushNotificationPermissionStatus(MutableContext&, ECleverTapPushPermissionStatus Value);
	UCppPushTabViewModel& SetDeepLinkData(MutableContext&, const FString& Value);
	UCppPushTabViewModel& SetInAppShownData(MutableContext&, const FString& Value);
	UCppPushTabViewModel& SetInAppDismissedData(MutableContext&, const FString& Value);
	UCppPushTabViewModel& SetInAppDismissedActionData(MutableContext&, const FString& Value);
	UCppPushTabViewModel& SetInAppButtonData(MutableContext&, const FString& Value);

	// <UObject>
	void BeginDestroy() override;
	void PostInitProperties() override;
	// </UObject>
	//
	// <FTickableGameObject>
	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
	ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	// </FTickableGameObject>

private:
	// <IPushTabViewModelInterface>
	FString GetPushNotificationData_Implementation() const override;
	void ClearPushNotificationData_Implementation() override;
	FString GetPushNotificationPermissionStatus_Implementation() const override;
	void PromptForPushPermission_Implementation(EPushPermissionPrimerType PrimerType) override;
	FString GetDeepLinkData_Implementation() const override;
	void ClearDeepLinkData_Implementation() override;
	FString GetInAppShownData_Implementation() const override;
	FString GetInAppDismissedData_Implementation() const override;
	FString GetInAppDismissedActionData_Implementation() const override;
	FString GetInAppButtonData_Implementation() const override;
	void ClearInAppData_Implementation() override;
	// </IPushTabViewModelInterface>

	// Begin Push/InApp Notification Callbacks
	UFUNCTION()
	void OnPushPermissionResponse(bool bGranted);

	UFUNCTION()
	void OnPushNotificationClicked(const FCleverTapProperties& NotificationPayload);

	UFUNCTION()
	void OnOpenURL(const FString& Url);

	UFUNCTION()
	void OnInAppNotificationShown(const FCleverTapProperties& NotificationPayload);

	UFUNCTION()
	void OnInAppNotificationDismissed(const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras);

	UFUNCTION()
	void OnInAppNotificationButtonClicked(const FCleverTapProperties& ButtonPayload);
	// End Push/InApp Notification Callbacks

private:
	FString PushNotificationData;

	FString DeepLinkData;

	FString InAppShownData;
	FString InAppDismissedData;
	FString InAppDismissedActionData;
	FString InAppButtonData;

	ECleverTapPushPermissionStatus PushNotificationPermissionStatus{};
};
