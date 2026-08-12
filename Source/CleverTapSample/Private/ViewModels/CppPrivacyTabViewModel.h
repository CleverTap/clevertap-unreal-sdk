// Copyright CleverTap All Rights Reserved.
#pragma once

#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/PrivacyTabViewModelInterface.h"
#include "CppPrivacyTabViewModel.generated.h"

/**
 * C++ implementation of the IPrivacyTabViewModelInterface
 */
UCLASS(NotBlueprintable)
class UCppPrivacyTabViewModel : public UCppViewModelBase, public IPrivacyTabViewModelInterface
{
	GENERATED_UCLASS_BODY()

public:
	bool GetOptOut() const;
	bool GetOffline() const;
	bool GetNetworkRecording() const;

	void Edit(TFunctionRef<void(MutableContext&, UCppPrivacyTabViewModel&)> EditFn);
	UCppPrivacyTabViewModel& SetOptOut(MutableContext&, bool Value);
	UCppPrivacyTabViewModel& SetOffline(MutableContext&, bool Value);
	UCppPrivacyTabViewModel& SetNetworkRecording(MutableContext&, bool Value);

private:
	// <IPrivacyTabViewModelInterface>
	bool GetOptOut_Implementation() const override;
	void SetOptOut_Implementation(bool bInIsOptOut) override;
	bool GetOffline_Implementation() const override;
	void SetOffline_Implementation(bool bInIsOffline) override;
	bool GetNetworkRecording_Implementation() const override;
	void SetNetworkRecording_Implementation(bool bInIsRecording) override;
	void PauseSDK_Implementation() override;
	void ResumeSDK_Implementation() override;
	// </IPrivacyTabViewModelInterface>

private:
	uint8 bIsOptOut : 1;
	uint8 bIsOffline : 1;
	uint8 bIsNetworkRecording : 1;
};
