#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PETabViewModelInterface.generated.h"

UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UPETabViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

class CLEVERTAPSAMPLE_API IPETabViewModelInterface
{
	GENERATED_BODY()

public:
	// ---- The 4 action buttons ----

	/** Define all demo variables with their default values. Must be called before FetchVariables(). */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	void DefineVariables();

	/**
	 * Sync variable definitions to the CleverTap dashboard.
	 * Use during development so the dashboard recognises newly-defined variables.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	void SyncVariables();

	/** Fetch current variable values from the CleverTap server. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	void FetchVariables();

	/** Print all variable names and their current values to the log. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	void PrintVariables();

	/** Fetch and log active A/B test variants for the current user. Results printed to log, status updated in UI. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	void GetVariants();

	// ---- Per-variable increment/decrement (for testing round-trips) ----

	/**
	 * Increment the local display value of a numeric variable by 1 (int/float/double/bool toggle).
	 * Does not push to dashboard — use to verify that Fetch overrides local state with dashboard values.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	void IncrementVariable(const FString& Name);

	/** Decrement the local display value of a numeric variable by 1. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	void DecrementVariable(const FString& Name);

	// ---- Queries (for UI binding) ----

	/** Get the display string for a variable's current value. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	FString GetVariableDisplayValue(const FString& Name) const;

	/** Status text e.g. "Not fetched yet", "Fetching...", "Fetched successfully". */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	FString GetFetchStatus() const;

	/** All defined variable names in definition order. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	TArray<FString> GetVariableNames() const;

	/** True if DefineVariables() has been called. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|PE")
	bool AreVariablesDefined() const;

};
