// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ViewModels/ChargedEventProductViewModel.h"
#include "ViewModels/EventPropertyViewModel.h"
#include "UObject/Interface.h"
#include "EventTabViewModelInterface.generated.h"

/**
 * Empty interface class for Unreal reflection
 */
UINTERFACE(Blueprintable)
class CLEVERTAPSAMPLE_API UEventTabViewModelInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Model the UI for the main menu's privacy tab
 */
class CLEVERTAPSAMPLE_API IEventTabViewModelInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	FString GetEventName() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void SetEventName(const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	TArray<FEventPropertyViewModel> GetEventProperties() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void AddEventProperty(FEventPropertyViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void SetEventPropertyAt(int32 Index, FEventPropertyViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void RemoveEventPropertyAt(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	bool IsRecordEventEnabled() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void RecordEvent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	FString GetLastRecordedEventData() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	TArray<FEventPropertyViewModel> GetChargedEventProperties() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void AddChargedEventProperty(FEventPropertyViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void SetChargedEventPropertyAt(int32 Index, FEventPropertyViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void RemoveChargedEventPropertyAt(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	TArray<FChargedEventProductViewModel> GetChargedEventProducts() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void AddChargedEventProduct(FChargedEventProductViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void SetChargedEventProductAt(int32 Index, FChargedEventProductViewModel Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void RemoveChargedEventProductAt(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	bool IsRecordChargedEventEnabled() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	void RecordChargedEvent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CleverTap|ViewModels|MainMenu|Event")
	FString GetLastRecordedChargedEventData() const;
};
