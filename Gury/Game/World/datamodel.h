#pragma once

#include "../Gury/Game/Gui/GuiRoot.h"
#include "../Gury/Kernel/kernel.h"
#include "../Gury/Game/Rendering/renderMesh.h"

namespace RBX
{
	class Workspace;
	class RunService;
	class ControllerService;
	class ThumbnailGenerator;
	class ScriptContext;
	class Lighting;
	class Scene;
	class SoundService;
	class SelectionService;
	class JointsService;

	namespace Network { class Players; }
	namespace Lua { class YieldingThreads; }
	namespace Render { class RenderScene; }

	class Datamodel : 
		public RBX::Instance
	{
	public:

		Workspace* workspace;
		RunService* runService;
		JointsService* jointService;
		ControllerService* controllerService;
		ThumbnailGenerator* thumbnailGenerator;
		ScriptContext* scriptContext;
		SoundService* soundService;
		SelectionService* selectionService;
		Lighting* lighting;
		Scene* scene;
		RBX::Render::Mesh* globalMesh;
		RBX::Render::RenderScene* renderScene;
		RBX::Gurnel* physicsKernel;

		Gui::GuiHint* message;
		Gui::GuiRoot* guiRoot;

		RBX::Network::Players* players;
		Lua::YieldingThreads* yieldingThreads;

		bool uiBrickcount;
	public:

		static Datamodel* get();

		void loadContent(std::string contentId);
		void saveContent(std::string contentId);

		void setMessage(std::string msg) { message->text = msg; }
		void setMessageBrickCount() { uiBrickcount = !uiBrickcount; }

		bool messageBrickCount() { return uiBrickcount; }

		void fillExplorerWindow();
		void emptyExplorerWindow();

		void addToExplorerWindow(RBX::Instance* i);
		void removeFromExplorerWindow(RBX::Instance* i);

		void open();
		void close();

		void step(double deltaTime);

		static void descendentAdded(RBX::Instance* i);
		static void descendentRemoved(RBX::Instance* i);

		Datamodel()
		{
			setClassName("Datamodel");
			setName("<<<ROOT>>>");

			onDescendentAdded.connect(descendentAdded);
			onDescendentRemoved.connect(descendentRemoved);
			
			message = new Gui::GuiHint();
			isParentLocked = 1;
		}

		RTTR_ENABLE(RBX::Instance)
	};
}