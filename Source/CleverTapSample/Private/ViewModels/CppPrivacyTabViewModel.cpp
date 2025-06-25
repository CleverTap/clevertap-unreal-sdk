// Copyright CleverTap All Rights Reserved.
#include "CppPrivacyTabViewModel.h"

#include "CleverTapInstance.h"
#include "CleverTapSubsystem.h"
#include "Engine.h"

UCppPrivacyTabViewModel::UCppPrivacyTabViewModel(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	bIsOptOut = false;
	bIsOffline = false;
	bIsNetworkRecording = true;
}

bool UCppPrivacyTabViewModel::GetOptOut() const
{
	return bIsOptOut;
}

bool UCppPrivacyTabViewModel::GetOffline() const
{
	return bIsOffline;
}

bool UCppPrivacyTabViewModel::GetNetworkRecording() const
{
	return bIsNetworkRecording;
}

void UCppPrivacyTabViewModel::Edit(TFunctionRef<void(MutableContext&, UCppPrivacyTabViewModel&)> EditFn)
{
	Super::Edit<UCppPrivacyTabViewModel>(EditFn);
}

UCppPrivacyTabViewModel& UCppPrivacyTabViewModel::SetOptOut(MutableContext&, bool Value)
{
	bIsOptOut = Value;
	return *this;
}

UCppPrivacyTabViewModel& UCppPrivacyTabViewModel::SetOffline(MutableContext&, bool Value)
{
	bIsOffline = Value;
	return *this;
}

UCppPrivacyTabViewModel& UCppPrivacyTabViewModel::SetNetworkRecording(MutableContext&, bool Value)
{
	bIsNetworkRecording = Value;
	return *this;
}

bool UCppPrivacyTabViewModel::GetOptOut_Implementation() const
{
	return bIsOptOut;
}

void UCppPrivacyTabViewModel::SetOptOut_Implementation(bool bInIsOptOut)
{
	Edit([=](MutableContext&, UCppPrivacyTabViewModel& VM) { VM.bIsOptOut = bInIsOptOut; });

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.SetOptOut(bInIsOptOut);
}

bool UCppPrivacyTabViewModel::GetOffline_Implementation() const
{
	return bIsOffline;
}

void UCppPrivacyTabViewModel::SetOffline_Implementation(bool bInIsOffline)
{
	Edit([=](MutableContext&, UCppPrivacyTabViewModel& VM) { VM.bIsOffline = bInIsOffline; });

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.SetOffline(bInIsOffline);
}

bool UCppPrivacyTabViewModel::GetNetworkRecording_Implementation() const
{
	return bIsNetworkRecording;
}

void UCppPrivacyTabViewModel::SetNetworkRecording_Implementation(bool bInIsRecording)
{
	Edit([=](MutableContext&, UCppPrivacyTabViewModel& VM) { VM.bIsNetworkRecording = bInIsRecording; });

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.SetNetworkInformationRecording(bInIsRecording);
}
