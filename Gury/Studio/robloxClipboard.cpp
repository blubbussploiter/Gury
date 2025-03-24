
#include "../Gury/Game/Services/selection.h"
#include "../Game/globalSounds.h"

#include "robloxClipboard.h"

RBX::StudioClipboard* clipboard = new RBX::StudioClipboard();

void RBX::StudioClipboard::copy(Instance* instance)
{
    content->push_back(instance->clone());
}

void RBX::StudioClipboard::copyMultiple(Instances* instances)
{
    content->clear();
    for (size_t i = 0; i < instances->size(); i++)
    {
        Instance* instance = instances->at(i);
        if (instance)
        {
            copy(instance);
        }
    }
}

void RBX::StudioClipboard::cut(Instance* instance)
{
    GlobalSounds::pageTurn->play();
    content->push_back(instance);

    /* set parent to nil (for cutting) */
    instance->setParent(0);
}

void RBX::StudioClipboard::cutMultiple(Instances* instances)
{
    content->clear();
    for (size_t i = 0; i < instances->size(); i++)
    {
        Instance* instance = instances->at(i);
        if (instance)
        {
            cut(instance);
        }
    }
}

void RBX::StudioClipboard::paste(Instance* where)
{
    /* actual paste */

    for (size_t i = 0; i < content->size(); i++)
    {
        Instance* instance = content->at(i);
        if (instance && where
            && where != instance)
        {
            instance->setParent(where);
        }
    }

    /* look at + select */

    SelectionService::get()->selection.clear();
    SelectionService::get()->selectArray(*content);
    Camera::get()->lookAtSelected();

    content->clear();
}

RBX::StudioClipboard* RBX::StudioClipboard::get()
{
    return clipboard;
}
