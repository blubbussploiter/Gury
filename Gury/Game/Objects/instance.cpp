

#include "../Gury/Studio/pch.h"
#include "../Gury/Main/framework.h"
#include "../Gury/Studio/MainFrm.h"

#include "../Gury/Game/strings.h"
#include "../Gury/Game/World/sounds.h"

#include "../Gury/Game/rbx.h"

#include "instance.h"

#undef contains

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Instance>("Instance")
		 .constructor<>()
		 .property("Name", &RBX::Instance::getName,
			&RBX::Instance::setName)(rttr::metadata("Type", RBX::Data))
		 .property_readonly("className", &RBX::Instance::getClassName)(rttr::metadata("Type", RBX::Data),rttr::metadata("Nonserializable", 0))
		 .property("archivable", &RBX::Instance::getArchivable, &RBX::Instance::setArchivable)(rttr::metadata("Type", RBX::Behavior))
		 .property("Parent", &RBX::Instance::getParent,
			 &RBX::Instance::setParent)(rttr::metadata("Nonserializable", 0))
		.property_readonly("Changed", &RBX::Instance::desc_onChanged)
		.property_readonly("ChildAdded", &RBX::Instance::desc_onChildAdded)
		.property_readonly("ChildRemoved", &RBX::Instance::desc_onChildRemoved)
		.property_readonly("DescendentRemoving", &RBX::Instance::desc_onDescendentRemoved)
		.property_readonly("DescendentAdded", &RBX::Instance::desc_onDescendentAdded)
		.property_readonly("AncestryChanged", &RBX::Instance::desc_onAncestryChanged)
		.method("remove", &RBX::Instance::remove)
		.method("clone", &RBX::Instance::clone)
		.method("children", &RBX::Instance::getChildren)
		.method("findFirstChild", &RBX::Instance::findFirstChild);
}

bool RBX::Instance::isAncestorOf(RBX::Instance* descendant)
{
	const RBX::Instance* v2; // eax

	v2 = descendant;
	if (!descendant)
		return 0;
	while (1)
	{
		v2 = v2->parent;
		if (v2 == this)
			break;
		if (!v2)
			return 0;
	}
	return 1;
}

bool RBX::Instance::contains(const RBX::Instance* child)
{
	const RBX::Instance* v2; // eax

	v2 = child;
	if (!child)
		return 0;
	while (v2 != this)
	{
		v2 = v2->parent;
		if (!v2)
			return 0;
	}
	return 1;
}

void RBX::Instance::signalOnDescendentAdded(RBX::Instance* beginParent, RBX::Instance* child)
{
	RBX::Instance* i;
	RBX::Instances* c;

	c = child->getChildren();

	for (i = beginParent; i; i = i->parent)
	{
		i->onDescendentAdded(i, child);
	}

	for (unsigned int i = 0; i < c->size(); i++)
	{
		RBX::Instance* in = c->at(i);
		child->signalOnDescendentAdded(child, in);
	}
}

void RBX::Instance::signalOnDescendentRemoved(RBX::Instance* newParent, RBX::Instance* child)
{
	RBX::Instance* i;
	RBX::Instances* c;

	c = child->getChildren();

	for (i = child; i; i = i->parent)
	{
		i->onDescendentRemoved(i, child);
	}

	for (unsigned int i = 0; i < c->size(); i++)
	{
		RBX::Instance* in = c->at(i);
		child->signalOnDescendentRemoved(child, in);
	}

}

void RBX::Instance::setParent(Instance* instance)
{
	RBX::Instance* oldParent;
	oldParent = parent;

	if (isParentLocked)
		return;

	if (instance == this)
	{
		throw std::runtime_error(RBX::Format("Attempt to set %s as its own parent", getName().c_str()));
	}

	if (oldParent != instance) {

		if (oldParent)
		{
			if (std::find(parent->getChildren()->begin(), parent->getChildren()->end(), this) != parent->getChildren()->end())
			{
				parent->signalOnDescendentRemoved(instance, this);
				parent->getChildren()->erase(std::remove(parent->getChildren()->begin(), parent->getChildren()->end(), this));
				oldParent->onChildRemoved(oldParent, this);
			}
		}
		parent = instance;

		if (parent)
		{
			parent->children->push_back(this);
			parent->onChildAdded(parent, this);

			//if (!parent->contains(oldParent))
				parent->signalOnDescendentAdded(parent, this);
		}

	}
	
	onChanged(this, "Parent");
}

void RBX::Instance::remove()
{
	if (!isParentLocked)
	{

		if (parent)
		{
			Instances parentChildren = *parent->children;
			parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), this), parentChildren.end());
		}

		for (RBX::Instance* i : *children)
		{
			i->remove();
		}

		onRemove();
		setParent(0);

	}
}

RBX::Instance* RBX::Instance::getParent()
{
	return parent;
}


RBX::Instance* RBX::Instance::findFirstChild(std::string name)
{
	Instance* child;
	RBX::Instances* children = getChildren();
	for (unsigned int i = 0; i < children->size(); i++)
	{
		child = children->at(i);
		if (child && child->getName() == name)
			return child;
	}
	return 0;
}

void RBX::Instance::onInstanceUpdateStudioView(RBX::Instance* instance, std::string property)
{
	ExplorerTreeView* classView = &CMainFrame::mainFrame->m_wndClassView;

	if (property == "Name")
	{
		std::string newName = instance->getName();
		HTREEITEM item = classView->GetInstance(instance);
		classView->m_wndClassView.SetItemText(item, newName.c_str());
	}

	if (property == "Parent")
	{
		classView->MoveInstanceParent(instance);
	}
}