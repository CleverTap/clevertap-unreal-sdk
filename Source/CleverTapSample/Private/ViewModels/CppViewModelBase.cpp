// Copyright CleverTap All Rights Reserved.
#include "ViewModels/CppViewModelBase.h"

#include "UserWidgetView.h"

UUserWidgetView* UCppViewModelBase::GetAttachedView() const
{
	return AttachedView;
}

void UCppViewModelBase::AttachToView(UUserWidgetView* View)
{
	if (AttachedView == View)
	{
		return;
	}

	UUserWidgetView* const OldView = AttachedView;
	AttachedView = View;

	if (OldView != AttachedView && OldView != nullptr)
	{
		OldView->SetViewModel(nullptr);
	}
	if (AttachedView != nullptr)
	{
		AttachedView->SetViewModel(TScriptInterface<IViewModelInterface>{ this });
	}
}

FOnViewModelChanged& UCppViewModelBase::OnViewModelChanged()
{
	return OnViewModelChangedDelegate;
}

void UCppViewModelBase::PropogateViewModelChanged(TScriptInterface<IViewModelInterface> ViewModel)
{
	if (OnViewModelChangedDelegate.IsBound())
	{
		OnViewModelChangedDelegate.Broadcast(ViewModel);
	}
}

void UCppViewModelBase::TryNotifyViewModelChanged()
{
	if (OnViewModelChangedDelegate.IsBound())
	{
		OnViewModelChangedDelegate.Broadcast(TScriptInterface<IViewModelInterface>{ this });
	}
}

void UCppViewModelBase::AddOnViewModelChangedDelegate_Implementation(const FOnViewModelChangedBP& InDelegate)
{
	OnViewModelChangedDelegate.AddUnique(InDelegate);
}

void UCppViewModelBase::RemoveOnViewModelChangedDelegate_Implementation(const FOnViewModelChangedBP& InDelegate)
{
	if (OnViewModelChangedDelegate.Contains(InDelegate))
	{
		OnViewModelChangedDelegate.Remove(InDelegate);
	}
}
