#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CSkybox.h"
#include "Engine/CLoadAssets.h"
#include "Engine/CPrimitives.h"
#include "CShadowMap.h"

class Scene6 : public CSceneBase
{
public:
		Scene6();
		~Scene6();

		virtual void DrawScene();
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);


private:

	// Camera
	CCamera* pCamera;

	// Axis
	CGridAxis* pGridAxis;

	// Shader
	CShader* pShader;

	// Textures
	CTextures* pTextures;

	// Timer
	CTimer* pTimer;			

	// Texto
	CText* pText;		

	// Assets
	CLoadAssets* pModel;

	// Skybox
	CSkybox* pSkybox;

	CShadowMap* pShadowMap;

	CPrimitives pPlane1, pPlane2;

};

