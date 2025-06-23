#include "ViewModels/CppMainMenuViewModel.h"

#include "CppDemonstrationHUD.h"
#include "ViewModels/CppEventTabViewModel.h"
#include "ViewModels/CppPrivacyTabViewModel.h"
#include "ViewModels/CppProfileTabViewModel.h"
#include "ViewModels/CppPushTabViewModel.h"

UCppMainMenuViewModel::UCppMainMenuViewModel(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	ProfileTab = CreateDefaultSubobject<UCppProfileTabViewModel>("Profile Tab");
	ProfileTab->OnViewModelChanged().AddDynamic(this, &UCppMainMenuViewModel::PropogateViewModelChanged);

	PushTab = CreateDefaultSubobject<UCppPushTabViewModel>("Push Tab");
	PushTab->OnViewModelChanged().AddDynamic(this, &UCppMainMenuViewModel::PropogateViewModelChanged);

	EventTab = CreateDefaultSubobject<UCppEventTabViewModel>("Event Tab");
	EventTab->OnViewModelChanged().AddDynamic(this, &UCppMainMenuViewModel::PropogateViewModelChanged);

	PrivacyTab = CreateDefaultSubobject<UCppPrivacyTabViewModel>("Privacy Tab");
	PrivacyTab->OnViewModelChanged().AddDynamic(this, &UCppMainMenuViewModel::PropogateViewModelChanged);
}

UCppProfileTabViewModel* UCppMainMenuViewModel::GetProfileTab() const
{
	return ProfileTab;
}

UCppPushTabViewModel* UCppMainMenuViewModel::GetPushTab() const
{
	return PushTab;
}

UCppEventTabViewModel* UCppMainMenuViewModel::GetEventTab() const
{
	return EventTab;
}

UCppPrivacyTabViewModel* UCppMainMenuViewModel::GetPrivacyTab() const
{
	return PrivacyTab;
}

void UCppMainMenuViewModel::Edit(TFunctionRef<void(MutableContext&, UCppMainMenuViewModel&)> EditFn)
{
	Super::Edit<UCppMainMenuViewModel>(EditFn);
}

UCppMainMenuViewModel& UCppMainMenuViewModel::SetActiveTab(MutableContext&, EMainMenuTab Value)
{
	ActiveTab = Value;
	return *this;
}

EMainMenuTab UCppMainMenuViewModel::GetActiveTab_Implementation() const
{
	return ActiveTab;
}

TScriptInterface<IProfileTabViewModelInterface> UCppMainMenuViewModel::GetProfileTab_Implementation() const
{
	return ProfileTab;
}

TScriptInterface<IPushTabViewModelInterface> UCppMainMenuViewModel::GetPushTab_Implementation() const
{
	return PushTab;
}

TScriptInterface<IEventTabViewModelInterface> UCppMainMenuViewModel::GetEventTab_Implementation() const
{
	return EventTab;
}

TScriptInterface<IPrivacyTabViewModelInterface> UCppMainMenuViewModel::GetPrivacyTab_Implementation() const
{
	return PrivacyTab;
}

void UCppMainMenuViewModel::SetActiveTab_Implementation(EMainMenuTab Value)
{
	Edit([Value](MutableContext& Ctx, UCppMainMenuViewModel& VM) { VM.SetActiveTab(Ctx, Value); });
}
