#include <kromakit/Control.h>

#include <algorithm>
#include <cstdlib>

#include <kromakit/DUIWindow.h>
#include <kromakit/Logging.h>

void Control::OnUsed()
{
}

DUIWindow* Control::GetRootWindow()
{
	if (parent == nullptr) {
		Logging::Log("Control::GetRootWindow failed at '%s' due to having a nullptr `parent`\n",
			GetControlName().value_or("<name unset>").c_str());
		return nullptr;
	}

	return parent->GetRootWindow();
}

void Control::TriggerOnWindowLoaded()
{
	for (auto i = Children.rbegin();
		i != Children.rend(); i++)
	{
		Control* ctrl = *i;
		ctrl->hasLoaded = true;
		ctrl->TriggerOnWindowLoaded();
	}

	OnLoad();
}

void Control::RemoveControl(Control* control)
{
	if (control == nullptr)
		return;

	bool removedAny = false;

	auto at = std::find(
		Children.begin(),
		Children.end(),
		control);

	if (at != Children.end()) {
		const DUIWindow* rootWindow = nullptr;
		if (parent != nullptr || dynamic_cast<DUIWindow*>(this) != nullptr)
			rootWindow = GetRootWindow();

		if (rootWindow != nullptr) {
			const_cast<DUIWindow*>(rootWindow)->OnControlRemoved(control);
		}

		if (control->parent == this)
			control->parent = nullptr;

		Children.erase(at);
		removedAny = true;
	}

	if (control->owner == this) {
		auto ownedAt = std::find_if(
			ownedChildren.begin(),
			ownedChildren.end(),
			[control](const std::shared_ptr<Control>& child) {
				return child.get() == control;
			});

		if (ownedAt != ownedChildren.end()) {
			control->owner = nullptr;
			ownedChildren.erase(ownedAt);
			removedAny = true;
		}
	}

	if (removedAny) {
		MarkLayoutDirty();
		MarkVisualDirty();
	}
}

void Control::ClearControls()
{
	for (auto& ctrl : Children)
	{
		if (ctrl != nullptr &&
			ctrl->parent == this) {
			ctrl->parent = nullptr;
		}
	}

	for (auto& ctrl : ownedChildren)
	{
		if (ctrl == nullptr)
			continue;

		if (ctrl->parent == this)
			ctrl->parent = nullptr;

		if (ctrl->owner == this)
			ctrl->owner = nullptr;
	}

	Children.clear();
	ownedChildren.clear();
	MarkLayoutDirty();
	MarkVisualDirty();
}

void Control::AssignOwnID()
{
	this->ControlID = rand() % 1000000 + 10000;
}

void Control::OnLoad()
{
	Logging::Log("Control Loaded (0x%X '%s')\n",
		ControlID, GetControlName().value_or("<name unset>").c_str());
}

void Control::InternalOnLoad()
{
	OnLoad();
}
