
#include "../Gury/Game/Services/selection.h"
#include "../../Game/globalSounds.h"

#include "../robloxClipboard.h"

RBX::StudioClipboard* clipboard = new RBX::StudioClipboard();

void RBX::StudioClipboard::clearContent()
{
    for (size_t i = 0; i < content->size(); i++)
    {
        Instance* what = content->at(i);
        what->remove();
    }
    content->clear();
}

void RBX::StudioClipboard::copy(Instance* what)
{
    content->push_back(what->clone());
}

void RBX::StudioClipboard::cut(Instance* what)
{
    GlobalSounds::pageTurn->play();
    content->push_back(what);

    /* set parent to nil (for cutting) */
    what->setParent(0);
}

void RBX::StudioClipboard::copyMultiple(Instances* which)
{
    for (size_t i = 0; i < which->size(); i++)
    {
        Instance* what = which->at(i);
        if (what)
        {
            copy(what);
        }
    }
}

void RBX::StudioClipboard::cutMultiple(Instances* which)
{
    clearContent();
    SelectionService::get()->selection.clear();

    for (size_t i = 0; i < which->size(); i++)
    {
        Instance* what = which->at(i);
        if (what)
        {
            cut(what);
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
