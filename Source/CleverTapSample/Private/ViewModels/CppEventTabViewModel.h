// Copyright CleverTap All Rights Reserved.
#pragma once

#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/EventTabViewModelInterface.h"
#include "CppEventTabViewModel.generated.h"

/**
 * C++ implementation of the IEventTabViewModelInterface
 */
UCLASS(NotBlueprintable)
class UCppEventTabViewModel : public UCppViewModelBase, public IEventTabViewModelInterface
{
	GENERATED_UCLASS_BODY()

private:
	void Edit(TFunctionRef<void(MutableContext&, UCppEventTabViewModel&)> EditFn);

	// <IEventTabViewModelInterface>
	FString GetEventName_Implementation() const override;
	void SetEventName_Implementation(const FString& Value) override;
	TArray<FEventPropertyViewModel> GetEventProperties_Implementation() const override;
	void AddEventProperty_Implementation(FEventPropertyViewModel Value) override;
	void SetEventPropertyAt_Implementation(int32 Index, FEventPropertyViewModel Value) override;
	void RemoveEventPropertyAt_Implementation(int32 Index) override;
	bool IsRecordEventEnabled_Implementation() const override;
	void RecordEvent_Implementation() override;
	FString GetLastRecordedEventData_Implementation() const override;
	TArray<FEventPropertyViewModel> GetChargedEventProperties_Implementation() const override;
	void AddChargedEventProperty_Implementation(FEventPropertyViewModel Value) override;
	void SetChargedEventPropertyAt_Implementation(int32 Index, FEventPropertyViewModel Value) override;
	void RemoveChargedEventPropertyAt_Implementation(int32 Index) override;
	TArray<FChargedEventProductViewModel> GetChargedEventProducts_Implementation() const override;
	void AddChargedEventProduct_Implementation(FChargedEventProductViewModel Value) override;
	void SetChargedEventProductAt_Implementation(int32 Index, FChargedEventProductViewModel Value) override;
	void RemoveChargedEventProductAt_Implementation(int32 Index) override;
	bool IsRecordChargedEventEnabled_Implementation() const override;
	void RecordChargedEvent_Implementation() override;
	FString GetLastRecordedChargedEventData_Implementation() const override;
	// </IEventTabViewModelInterface>

private:
	FString EventName;
	TArray<FEventPropertyViewModel> EventProperties;
	FString LastRecordedEventData;

	TArray<FEventPropertyViewModel> ChargedEventProperties;
	TArray<FChargedEventProductViewModel> Products;
	FString LastRecordedChargedEventData;
};
