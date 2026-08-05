#pragma once

#include "ViewModels/CppViewModelBase.h"
#include "ViewModels/PETabViewModelInterface.h"
#include "CppPETabViewModel.generated.h"

/** Internal enum tracking the type of each defined variable so we call the right getter on refresh. */
enum class EPEVarType : uint8
{
	String,
	Int,
	Int64,
	Float,
	Double,
	Bool,
	StringMap,
	File,
};

UCLASS(NotBlueprintable)
class UCppPETabViewModel : public UCppViewModelBase, public IPETabViewModelInterface
{
	GENERATED_BODY()

public:
	void Edit(TFunctionRef<void(MutableContext&, UCppPETabViewModel&)> EditFn);
	UCppPETabViewModel& SetFetchStatus(MutableContext&, const FString& Status);
	UCppPETabViewModel& SetVariableDisplayValue(MutableContext&, const FString& Name, const FString& Value);
	UCppPETabViewModel& SetVariablesDefined(MutableContext&, bool bDefined);

	// UObject overrides
	void PostInitProperties() override;
	void BeginDestroy() override;

private:
	// IPETabViewModelInterface implementations
	void DefineVariables_Implementation() override;
	void SyncVariables_Implementation() override;
	void FetchVariables_Implementation() override;
	void PrintVariables_Implementation() override;
	void GetVariants_Implementation() override;
	void IncrementVariable_Implementation(const FString& Name) override;
	void DecrementVariable_Implementation(const FString& Name) override;
	FString GetVariableDisplayValue_Implementation(const FString& Name) const override;
	FString GetFetchStatus_Implementation() const override;
	TArray<FString> GetVariableNames_Implementation() const override;
	bool AreVariablesDefined_Implementation() const override;

	/** Register one variable with the SDK, tracking name/type/default for later refresh. */
	void RegisterStringVar(const FString& Name, const FString& Default);
	void RegisterIntVar(const FString& Name, int32 Default);
	void RegisterInt64Var(const FString& Name, int64 Default);
	void RegisterFloatVar(const FString& Name, float Default);
	void RegisterDoubleVar(const FString& Name, double Default);
	void RegisterBoolVar(const FString& Name, bool Default);
	void RegisterStringMapVar(const FString& Name, const TMap<FString, FString>& Default);
	void RegisterFileVar(const FString& Name);

	/** Refresh all display values using the correct typed getter for each variable. */
	void RefreshDisplayValues(MutableContext& Ctx);

	// CleverTap delegate callbacks
	UFUNCTION()
	void OnVariablesFetched(bool bSuccess);

	UFUNCTION()
	void OnVariablesChanged();

private:
	TMap<FString, FString>    VariableDisplayValues;
	TArray<FString>           VariableNames;
	TMap<FString, EPEVarType> VariableTypes;
	FString FetchStatus   = TEXT("Not fetched yet");
	bool    bDefined      = false;
};
