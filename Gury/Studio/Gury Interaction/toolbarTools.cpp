
#include "../pch.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "../supergurymfc.h"
#include "../MainFrm.h"

#include "../../Main/framework.h"

#include "../Gury/Game/Network/Player/players.h"
#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Serializer/serializer.h"
#include "../Gury/Game/Services/scriptcontext.h"
#include "../Gury/Kernel/collisionTest.h"

#include "../Gury/Game/classes.h"

#include "../CInsertObjectDlg.h"
#include "../StudioTool.h"

static RBX::Sound* switch_wav = RBX::Sound::fromFile("rbxasset://sounds\\switch.wav");
static RBX::Sound* bass_wav = RBX::Sound::fromFile("rbxasset://sounds\\bass.wav");

void CsupergurymfcApp::OnInsertObject()
{
	if (RBX::RBXManager::isReady())
	{
		CInsertObjectDlg dialog;
		if (dialog.DoModal() == IDOK)
		{
			std::string classN;
			classN = dialog.CLASSTOINSERT;

			RBX::Instance* n = RBX::fromName(classN);
			if (!n)
			{
				std::string err = RBX::Format("Failed to insert object of type '%s'. Did you spell it correctly?", classN.c_str());
				MessageBox(0, err.c_str(), "Gury3d", MB_ICONWARNING);
			}
			else
			{
				RBX::Instance* parent = RBX::SelectionService::get()->getOneSelected();
				if (parent) {
					n->setParent(parent);
				}
				else {
					n->setParent(RBX::Workspace::get());
				}
				RBX::SelectionService::get()->selectInstance(n);
			}
		}
	}
}

void CsupergurymfcApp::OnInsertModel()
{
	if (RBX::RBXManager::isReady())
	{
		static TCHAR BASED_CODE szFilter[] = _T("ROBLOX Model Files (*.rbxm;*.rbxmx)|*.rbxm; *.rbxmx|All Files (*.*)|*.*||");

		CFileDialog dlgFile(TRUE, "Open", 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);

		if (dlgFile.DoModal() == IDOK)
		{
			CString file = dlgFile.GetPathName();
			RBX::Instances* instances = RBX::Serializer::loadInstances(file.GetString());

			if (instances->size() > 0)
			{
				RBX::Instance* at = instances->at(0);
				RBX::Instance* parent = RBX::Workspace::get();

				if (at && parent)
				{
					RBX::ModelInstance* model = dynamic_cast<RBX::ModelInstance*>(at);
					RBX::Camera* camera = RBX::Camera::get();

					at->setParent(parent);

					if (model)
					{
						if (!camera)
						{
							return;
						}

						RBX::PartInstance* primaryPart;
						primaryPart = model->getPrimaryPart();

						if (primaryPart)
						{
							camera->lookAt(primaryPart->getPosition());
						}
						RBX::SelectionService::get()->selectInstance(model);
					}
				}
			}
		}
	}
}

void CsupergurymfcApp::OnExecuteScript()
{
	if (!RBX::RBXManager::isReady())
	{
		OnFileNew();
	}

	static TCHAR BASED_CODE szFilter[] = _T("Scripts (*.rbxs;*.lua;*.txt)|*.rbxs; *.lua;*.txt|All Files (*.*)|*.*||");
	CFileDialog dlgFile(TRUE, "Open", 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);

	if (dlgFile.DoModal() == IDOK)
	{
		CString file = dlgFile.GetPathName();
		RBX::Datamodel* datamodel = RBX::Datamodel::get();

		if (datamodel && datamodel->scriptContext)
		{
			std::ifstream stream(file);
			std::string script, line = "";

			if (stream.good())
			{
				while (std::getline(stream, line))
				{
					script.append(line);
					script.append("\n");
				}
				stream.close();
			}

			datamodel->scriptContext->execute(script);
		}
	}
}

void CsupergurymfcApp::OnLoadCharacter()
{
	RBX::Network::Players* players;
	RBX::Network::Player* player;
	RBX::Datamodel* datamodel = RBX::Datamodel::get();

	if (!datamodel) return;
	players = datamodel->players;
	if (!players) return;

	player = players->localPlayer;
	if (player) player->loadCharacter();
}

void CsupergurymfcApp::OnRunRunService()
{
	RBX::Datamodel* datamodel = RBX::Datamodel::get();
	if (!datamodel) return;
	datamodel->runService->run();
}

void CsupergurymfcApp::OnPauseRunService()
{
	RBX::Datamodel* datamodel = RBX::Datamodel::get();
	if (!datamodel) return;
	datamodel->runService->pause();
}

void CsupergurymfcApp::OnResetRunService()
{
	RBX::Datamodel* datamodel = RBX::Datamodel::get();
	if (!datamodel) return;
	datamodel->runService->reset();
}

void CsupergurymfcApp::OnMoveUp()
{
    RBX::Instances selection = RBX::SelectionService::get()->selection;
	for (unsigned int i = 0; i < selection.size(); i++)
	{
		RBX::Instance* instance = selection.at(i);
		RBX::PVInstance* pv = RBX::toInstance<RBX::PVInstance>(instance);
		if (pv)
		{
			CoordinateFrame c = pv->getCoordinateFrame();
			CoordinateFrame p = c + c.upVector() * RBX::getAffectedFormFactor(pv);
			if (RBX::predict(pv, p))
			{
				pv->setCFrame(p);
			}
			else /* check if smaller than hitter, if it is, selected may get on top */
			{
				RBX::PVInstance* firstBrickUp = 0;
				float top = RBX::getDirectionalBrickInfo(c.translation, c.upVector(), pv, firstBrickUp);

				if (firstBrickUp)
				{
					Vector3 size0 = firstBrickUp->getSize();
					Vector3 size1 = pv->getSize();

					if (size0.y >= size1.y)
					{
						CoordinateFrame newCoord;
						newCoord = c + c.upVector() * top;
						pv->setCFrame(newCoord);
					}
					else
					{
						bass_wav->play();
						return;
					}
				}
				else
				{
					bass_wav->play();
					return;
				}
			}
		}
	}
	switch_wav->play();
}

void CsupergurymfcApp::OnMoveDown()
{
	RBX::Instances selection = RBX::SelectionService::get()->selection;
	for (unsigned int i = 0; i < selection.size(); i++)
	{
		RBX::Instance* instance = selection.at(i);
		RBX::PVInstance* pv = RBX::toInstance<RBX::PVInstance>(instance);
		if (pv)
		{
			CoordinateFrame c = pv->getCoordinateFrame();
			CoordinateFrame p = c - c.upVector() * RBX::getAffectedFormFactor(pv);
			if (RBX::predict(pv, p))
			{
				pv->setCFrame(p);
			}
			else
			{
				bass_wav->play();
				return;
			}
		}
	}
	switch_wav->play();
}

void CsupergurymfcApp::OnTiltLeft()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->tiltLeft(25);
}

void CsupergurymfcApp::OnTiltRight()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->tiltRight(25);
}

void CsupergurymfcApp::OnTiltUp()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->tiltUp();
}

void CsupergurymfcApp::OnTiltDown()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->tiltDown();
}

void CsupergurymfcApp::OnZoomIn()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->cam_zoom(1);
}

void CsupergurymfcApp::OnZoomOut()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->cam_zoom(0);
}

void CsupergurymfcApp::OnLookat()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->lookAtSelected();
	camera->switch3->play();
}

void CsupergurymfcApp::OnZoomExtents()
{
	RBX::Camera* camera = RBX::Camera::get();
	if (!camera) return;
	camera->zoomExtents();
	camera->switch3->play();
}

void CsupergurymfcApp::OnMouseToolSelected()
{
	RBX::Studio::setToolFromType(RBX::Studio::Mouse);
}
