// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ViewModels/ViewModelInterface.h"
#include "CppViewModelBase.generated.h"

class UUserWidgetView;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewModelChanged, TScriptInterface<IViewModelInterface>, ViewModel);

/**
 * Base class that all view models share to have a link back to connected views for property changes
 */
UCLASS(NotBlueprintable)
class CLEVERTAPSAMPLE_API UCppViewModelBase : public UObject, public IViewModelInterface
{
	GENERATED_BODY()

public:
	class MutableContext
	{
		MutableContext() = default;
		MutableContext(MutableContext&&) = delete;
		MutableContext(MutableContext const&) = delete;
		MutableContext& operator=(MutableContext&&) = delete;
		MutableContext& operator=(MutableContext const&) = delete;
	};

	UUserWidgetView* GetAttachedView() const;
	void AttachToView(UUserWidgetView* View);

	FOnViewModelChanged& OnViewModelChanged();

protected:
	template <typename T>
	void Edit(TFunctionRef<void(MutableContext&, T& ViewModel)> EditFn)
	{
		MutableContext Ctx{};
		EditFn(Ctx, *static_cast<T*>(this));
		TryNotifyViewModelChanged();
	}

	UFUNCTION() // UFUNCTION needed for delegate binding
	void PropogateViewModelChanged(TScriptInterface<IViewModelInterface> ViewModel);

private:
	void TryNotifyViewModelChanged();

	// <IViewModelInterface>
	void AddOnViewModelChangedDelegate_Implementation(const FOnViewModelChangedBP& InDelegate) override;
	void RemoveOnViewModelChangedDelegate_Implementation(const FOnViewModelChangedBP& Handle) override;
	// </IViewModelInterface>

private:
	UPROPERTY()
	UUserWidgetView* AttachedView;

	UPROPERTY()
	FOnViewModelChanged OnViewModelChangedDelegate;
};
