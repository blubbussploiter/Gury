#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Objects/instance.h"
#include "../../Services/content.h"

namespace RBX
{

	enum BinType
	{
		SCRIPT_BIN,
		GAMETOOL_BIN,
		GRAB_BIN,
		CLONE_BIN,
		HAMMER_BIN,
		SLINGSHOT_BIN,
		ROCKET_BIN,
		LASER_BIN
	};

	class BackpackItem;

	class HopperBin : public RBX::Instance
	{
	public:

		Content textureId;

		BinType binType;
		bool active;

		void setBinType(BinType binType) {
			this->binType = binType;
			switch (binType) {
			case SCRIPT_BIN: {
				break;
			}
			case GAMETOOL_BIN: {
				setTextureId(Content::fromStoredContent("rbxasset://Textures\\GameTool.png"));
				break;
			}
			case GRAB_BIN: {
				setTextureId(Content::fromStoredContent("rbxasset://Textures\\Grab.png"));
				break;
			}
			case CLONE_BIN: {
				setTextureId(Content::fromStoredContent("rbxasset://Textures\\Clone.png"));
				break;
			}
			case HAMMER_BIN: {
				setTextureId(Content::fromStoredContent("rbxasset://Textures\\Hammer.png"));
				break;
			}
			case SLINGSHOT_BIN: {
				setTextureId(Content::fromStoredContent("rbxasset://Textures\\Slingshot.png"));
				break;
			}
			case ROCKET_BIN: {
				setTextureId(Content::fromStoredContent("rbxasset://Textures\\Rocket.png"));
				break;
			}
			case LASER_BIN: {
				setTextureId(Content::fromStoredContent("rbxasset://Textures\\Laser.png"));
				break;
			}
			}
			onChanged(this, getPropertyByName("BinType"));
		}

		BinType getBinType() {
			return binType;
		}

		void setTextureId(Content content) {
			textureId = content;
			onChanged(this, getPropertyByName("TextureId"));
		}

		Content getTextureId() {
			return textureId;
		}

		virtual void update(RenderDevice* rd, G3D::UserInput* ui) {}
		virtual void activate() {}
		virtual void deactivate() {}

		RBX_CLONE_DEF(HopperBin)

		HopperBin()
		{
			setName("HopperBin");
			setClassName("HopperBin");
			binType = SCRIPT_BIN;
			active = 0;
		}
		virtual ~HopperBin() { deactivate(); };
		RTTR_ENABLE(RBX::Instance);
	};
}