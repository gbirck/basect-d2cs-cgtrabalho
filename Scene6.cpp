#include "Scene6.h"

struct PointLightData {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

static PointLightData gPointLights[] = {
	// SM_Light_bulb – central frente
	{ glm::vec3(0.444f,  2.169f,  0.561f),
	  glm::vec3(0.08f, 0.065f, 0.03f),
	  glm::vec3(2.5f, 2.1f, 1.3f),
	  glm::vec3(1.0f, 0.95f, 0.8f),
	  1.0f, 0.22f, 0.08f },

	  // SM_Light_bulb001 – direita
	  { glm::vec3(2.938f,  2.169f,  0.561f),
		glm::vec3(0.08f, 0.065f, 0.03f),
		glm::vec3(2.5f, 2.1f, 1.3f),
		glm::vec3(1.0f, 0.95f, 0.8f),
		1.0f, 0.22f, 0.08f },

		// SM_Light_bulb002 – esquerda
		{ glm::vec3(-1.451f,  2.169f,  0.561f),
		  glm::vec3(0.08f, 0.065f, 0.03f),
		  glm::vec3(2.5f, 2.1f, 1.3f),
		  glm::vec3(1.0f, 0.95f, 0.8f),
		  1.0f, 0.22f, 0.08f },

		  // SM_Light_bulb003 – corredor fundo
		  { glm::vec3(-1.451f,  2.169f, -5.951f),
			glm::vec3(0.08f, 0.065f, 0.03f),
			glm::vec3(2.5f, 2.1f, 1.3f),
			glm::vec3(1.0f, 0.95f, 0.8f),
			1.0f, 0.22f, 0.08f },
};
static const int NUM_POINT_LIGHTS = 4;

Scene6::Scene6()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(0.0f, 3.0f, 20.0f));
	pCamera->MovementSpeed = 100.0f;

	// Cria o Timer
	pTimer = NULL;
	pTimer = new CTimer();
	pTimer->Init();

	// Cria gerenciador de impressão de texto na tela
	pText = NULL;
	pText = new CText();
	pText->InitTextManager();

	// Cria os shaders
	pShader = NULL;
	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");
	pShader->LoadShader("Assets", "Scenes/Scene6/Assets.vert", "Scenes/Scene6/Assets.frag");
	pShader->LoadShader("Skybox", "Scenes/Scene6/skybox.vert", "Scenes/Scene6/skybox.frag");
	pShader->LoadShader("Blend", "Scenes/Scene6/Blend.vert", "Scenes/Scene6/Blend.frag");
	pShader->LoadShader("Depth", "Scenes/Scene6/depth.vert", "Scenes/Scene6/depth.frag");

	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureLinear(0, "Scenes/Scene6/old_mosaic_floor_diff_1k.png");


	// Cria o modelo exportado do 3DMAX
	pModel = NULL;
	pModel = new CLoadAssets("Scenes/Scene6/Scene6.obj");

	// Cria o Skybox
	pSkybox = NULL;
	pSkybox = new CSkybox({
		"Scenes/Scene6/right.bmp",
		"Scenes/Scene6/left.bmp",
		"Scenes/Scene6/top.bmp",
		"Scenes/Scene6/bottom.bmp",
		"Scenes/Scene6/front.bmp",
		"Scenes/Scene6/back.bmp" });

	// Cria o shadow map
	pShadowMap = NULL;
	pShadowMap = new CShadowMap(1024, 1024);

	// 
	pPlane1.generatePlane(2.0f, 2.0f);
	pPlane1.setupBuffers();

	pPlane2.generatePlane(2.0f, 2.0f);
	pPlane2.setupBuffers();
}

Scene6::~Scene6()
{
	pPlane1.cleanup();
	pPlane2.cleanup();

	if (pShadowMap)
	{
		delete pShadowMap;
		pShadowMap = NULL;
	}

	if (pSkybox)
	{
		delete pSkybox;
		pSkybox = NULL;
	}

	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}

	if (pCamera)
	{
		delete pCamera;
		pCamera = NULL;
	}

	if (pShader)
	{
		delete pShader;
		pShader = NULL;
	}

	if (pGridAxis)
	{
		delete pGridAxis;
		pGridAxis = NULL;
	}

	if (pTextures)
	{
		delete pTextures;
		pTextures = NULL;
	}

	if (pTimer)
	{
		delete pTimer;
		pTimer = NULL;
	}

	if (pText)
	{
		delete pText;
		pText = NULL;
	}


}

void Scene6::DrawScene()
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

	// Create transformations 
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0);

	// Matriz do modelo principal (mesma usada no pass normal, precisamos dela aqui também)
	glm::mat4 assetsModel = glm::mat4(1.0f);
	assetsModel = glm::rotate(assetsModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	assetsModel = glm::scale(assetsModel, glm::vec3(0.03f));

	glm::vec4 lampPositionsMax[] = {
	glm::vec4(14.793f,  18.696f, 72.313f, 1.0f),
	glm::vec4(97.918f,  18.696f, 72.313f, 1.0f),
	glm::vec4(-48.365f,  18.696f, 72.313f, 1.0f),
	glm::vec4(-48.365f,-198.375f, 72.313f, 1.0f),
	};

	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		glm::vec4 posWorld = assetsModel * lampPositionsMax[i];
		gPointLights[i].position = glm::vec3(posWorld);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// j. Sombras Projetadas - PASS 1: renderiza a cena do ponto de vista da luz [0]
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float near_plane = 1.0f, far_plane = 50.0f;
	glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
	glm::vec3 lightPos = gPointLights[0].position;
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	pShadowMap->BeginRender();

	pShader->Use("Depth");
	pShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	pShader->SetMat4("model", assetsModel);
	pModel->Draw(pShader->GetProgram("Depth"));

	pShadowMap->EndRender(SCR_WIDTH, SCR_HEIGHT);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PASS 2: render normal da cena
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Clear the color and depth buffers
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the 3D axis
	pShader->Use("Axis");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pGridAxis->DrawAxis();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (INÍCIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	pShader->Use("Assets");
	pShader->SetMat4("projection", projection);
	view = pCamera->GetViewMatrix();
	pShader->SetMat4("view", view);
	pShader->SetVec3("viewPos", pCamera->Position);
	pShader->SetMat4("model", assetsModel);
	pShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		std::string b = "pointLights[" + std::to_string(i) + "]";
		pShader->SetVec3((b + ".position").c_str(), gPointLights[i].position);
		pShader->SetVec3((b + ".ambient").c_str(), gPointLights[i].ambient);
		pShader->SetVec3((b + ".diffuse").c_str(), gPointLights[i].diffuse);
		pShader->SetVec3((b + ".specular").c_str(), gPointLights[i].specular);
		pShader->SetFloat((b + ".constant").c_str(), gPointLights[i].constant);
		pShader->SetFloat((b + ".linear").c_str(), gPointLights[i].linear);
		pShader->SetFloat((b + ".quadratic").c_str(), gPointLights[i].quadratic);
	}
	pShader->SetInt("numPointLights", NUM_POINT_LIGHTS);

	glActiveTexture(GL_TEXTURE2);
	pTextures->ApplyTexture(0);
	pShader->SetInt("uDetailMap", 2);
	pShader->SetFloat("detailScale", 4.0f);
	pShader->SetBool("useDetail", true);
	pShader->SetBool("useNormalMap", true);
	pShader->SetVec3("fogColor", glm::vec3(0.55f, 0.45f, 0.28f));
	pShader->SetFloat("fogDensity", 0.10f);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pSkybox->GetSkyboxTexture());
	pShader->SetInt("uEnvMap", 3);
	pShader->SetBool("useEnvMap", true);
	pShader->SetFloat("reflectivity", 0.2f);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, pShadowMap->GetDepthTexture());
	pShader->SetInt("uShadowMap", 4);
	pShader->SetBool("useShadow", true);

	pModel->Draw(pShader->GetProgram("Assets"));

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Desenha o Skybox
	pShader->Use("Skybox");
	pShader->SetMat4("projection", projection);
	view = glm::mat4(glm::mat3(pCamera->GetViewMatrix()));
	pShader->SetMat4("view", view);
	pSkybox->DrawSkybox();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// i. Transparência (Blending) - painel de "vidro" translúcido na frente da câmera inicial
	//
	// Opacos (DESTINATION) - painel sólido atrás, simula uma parede
	pShader->Use("Blend");
	pShader->SetMat4("projection", projection);
	view = pCamera->GetViewMatrix();
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", glm::vec4(0.6f, 0.6f, 0.65f, 1.0f));
	pShader->SetBool("isTexured", false);
	pPlane1.render();

	// Transparentes (SOURCE) - "vidro" azulado na frente do painel opaco
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	pShader->Use("Blend");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", glm::vec4(0.4f, 0.6f, 0.9f, 0.4f));
	pShader->SetBool("isTexured", false);
	pPlane2.render();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (FIM)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw text on the screen (disable depth test and use orthographic projection)
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 1.0f);

	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamFoward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);

	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene6::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	// Camera movement
	if (input.IsKeyPressed(GLFW_KEY_W))
		pCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_S))
		pCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_A))
		pCamera->ProcessKeyboard(LEFT, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_D))
		pCamera->ProcessKeyboard(RIGHT, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_E))
		pCamera->ProcessKeyboard(UP, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_Q))
		pCamera->ProcessKeyboard(DOWN, deltaTime);

	// Increase/decrease movement speed
	if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		pCamera->MovementSpeed = SPEED_MULTIPLIER;
	else
		pCamera->MovementSpeed = SPEED;

	// Process mouse movement
	glm::vec2 mouseDelta = input.GetMouseDelta();
	if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
	{
		pCamera->ProcessMouseMovement(mouseDelta.x, mouseDelta.y);
	}

	// Process scroll
	float scrollOffset = input.GetScrollOffset();
	if (scrollOffset != 0.0f)
	{
		pCamera->ProcessMouseScroll(scrollOffset);
	}
}