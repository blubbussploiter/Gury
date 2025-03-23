#pragma once

#include <vector>

#include "../Gury/Game/Objects/ISelectable.h"
#include "../Gury/Game/Objects/instance.h"

#include "../Gury/Game/Network/Player/Mouse.h"

namespace RBX
{
	class SelectionService : public RBX::Instance, public RBX::Service<SelectionService>
	{
	public:

		Instances selection;

		Vector2 selectionDragBoxStart, selectionDragBoxEnd;
		Vector2 worldSelectStart, worldSelectEnd;

		bool clicked, down;
		bool selectionAllowed;
		bool multiSelect;

		bool isSelected(Instance* i);

		Instance* getPossibleSelectedItem();

		Instances selectionAsInstances();
		ISelectableArray instancesAsSelection(Instances i);
		void selectArray(Instances instances);

		void renderDragBox(RenderDevice* rd);

		void renderSelected(RenderDevice* rd, ISelectable* i);
		void renderSelection(RenderDevice* d);

		void removeSelected();

		void dragSelect();

		void update(UserInput* ui);

		bool doSelect(Instance* p, bool multiSelect);
		void deselect(RBX::Instance* pv)
		{
			if (pv)
			{
				if (isSelected(pv))
				{
					selection.erase(std::remove(selection.begin(), selection.end(), pv));
				}
			}
		}

		void deselectAll()
		{
			selection.clear();
		}

		void selectInstance(RBX::Instance* instance, bool selectInExplorer=1);

		SelectionService()
		{
			selectionAllowed = true;
			setClassName("Selection");
			setName("Selection");
		}

		static SelectionService* get();
	};
}