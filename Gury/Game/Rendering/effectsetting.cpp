#include "effectsetting.h"

#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/World/lighting.h"

void RBX::Render::EffectSettings::applyEffects(RenderDevice* rd, LightingParameters& pout, LightingRef& lout)
{
	RBX::Lighting* lighting = RBX::Lighting::get();
	polish(rd, *lighting->params, pout, lighting->lighting, lout);
}

void RBX::Render::EffectSettings::polish(RenderDevice* rd, const LightingParameters& pin, LightingParameters& pout, const LightingRef& lin, LightingRef& lout)
{
	lout = toneMap->prepareLighting(lin);
	pout = toneMap->prepareLightingParameters(pin);
}