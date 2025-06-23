// Copyright CleverTap All Rights Reserved.
#include "CppPrivacyTabViewModel.h"

#include "CleverTapInstance.h"
#include "CleverTapSubsystem.h"
#include "Engine.h"

UCppPrivacyTabViewModel::UCppPrivacyTabViewModel(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	bIsOptIn = false;
	bIsOffline = false;
	bIsNetworkRecording = false;
}

bool UCppPrivacyTabViewModel::GetOptIn() const
{
	return bIsOptIn;
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

UCppPrivacyTabViewModel& UCppPrivacyTabViewModel::SetOptIn(MutableContext&, bool Value)
{
	bIsOptIn = Value;
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

bool UCppPrivacyTabViewModel::GetOptIn_Implementation() const
{
	return bIsOptIn;
}

void UCppPrivacyTabViewModel::SetOptIn_Implementation(bool bInIsOptIn)
{
	Edit([=](MutableContext&, UCppPrivacyTabViewModel& VM) { VM.bIsOptIn = bInIsOptIn; });

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.SetOptOut(!bInIsOptIn);
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
