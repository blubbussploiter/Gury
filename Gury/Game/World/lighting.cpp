

#include "../Gury/Game/rbx.h"
#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Rendering/renderScene.h"

#include "lighting.h"

RTTR_REGISTRATION
{
    rttr::registration::class_ <RBX::Lighting>("Lighting")
         .constructor<>()
         .property("TimeOfDay", &RBX::Lighting::getTimeOfDay, &RBX::Lighting::setTimeOfDay)(rttr::metadata("Type", RBX::Data))
         .property("BottomAmbientV9", &RBX::Lighting::getBottomAmbient, &RBX::Lighting::setBottomAmbient)(rttr::metadata("Serializable", false))
         .property("Ambient", &RBX::Lighting::getAmbient, &RBX::Lighting::setAmbient)(rttr::metadata("Type", RBX::Appearance))
         .property("ClearColor", &RBX::Lighting::getClearColor, &RBX::Lighting::setClearColor)(rttr::metadata("Serializable", false))
         .property("SpotLightV9", &RBX::Lighting::getSpotLight, &RBX::Lighting::setSpotLight)(rttr::metadata("Serializable", false))
         .property("ColorShift_Bottom", &RBX::Lighting::getBottomAmbient, &RBX::Lighting::setBottomAmbient)(rttr::metadata("Type", RBX::Appearance))
         .property("ColorShift_Top", &RBX::Lighting::getTopAmbient, &RBX::Lighting::setTopAmbient)(rttr::metadata("Type", RBX::Appearance))
         .property("TopAmbientV9", &RBX::Lighting::getTopAmbient, &RBX::Lighting::setTopAmbient)(rttr::metadata("Serializable", false))
         .property("GeographicLatitude",&RBX::Lighting::getGeoLatitude, &RBX::Lighting::setGeoLatitude)(rttr::metadata("Type", RBX::Appearance));
}


RBX::Lighting* RBX::Lighting::get()
{
    return RBX::Datamodel::get()->lighting;
}

GameTime RBX::Time::fromString(const char* c)
{
    int hour = 0, minute = 0, second = 0;
    AMPM ampm;

    sscanf(c, "%d:%d:%d", &hour, &minute, &second);
    if (hour < 12)
        ampm = AM;
    else
        ampm = PM;

    return toSeconds(hour, minute, second, ampm);
}

std::string RBX::Time::toString(int seconds)
{
    std::string h, m, s;
    int hour, minute, second;

    hour = seconds / 3600;
    if (hour > 0)
    {

        minute = seconds % (hour * 60);
        second = (seconds % (hour * 3600)) - ((seconds % (hour * 60)) * 60);

        h = RBX::Format("%d", hour);
        m = RBX::Format("%d", minute);
        s = RBX::Format("%d", second);

        if (hour < 10)
        {
            h = RBX::Format("0%d", hour);
        }

        if (minute < 10)
        {
            m = RBX::Format("0%d", minute);
        }

        if (second < 10)
        {
            s = RBX::Format("0%d", second);
        }

        return RBX::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str());

    }
}

RBX::Lighting::Lighting()
{
    RBX::Render::RenderScene* view;
    view = RBX::Render::RenderScene::get();

    lighting = view->lighting;
    params = &view->params;

    setGeoLatitude(41.7f);
    setTimeOfDay("14:00:00");
    setSpotLight(Color3(0.59607846f, 0.53725493f, 0.40000001f));

    clear_color = Color4(0.0f, 0.0f, 0.0f);
    setTopAmbient(Color3(0.81960785f, 0.81568629f, 0.8509804f));
    setBottomAmbient(Color3(0.47843137f, 0.52549022f, 0.47058824f));

    setName("Lighting");
    setClassName("Lighting");
}