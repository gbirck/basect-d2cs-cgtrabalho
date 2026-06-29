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

	// Clear the color and depth buffers
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create transformations 
	// Note: In a real application, you would typically calculate the projection and view matrices 
	// once per frame and pass them to your shaders
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the 3D grid
	//pShader->Use("Grid");
	//pShader->SetMat4("uProj", projection);
	//pShader->SetMat4("uView", view);
	//pShader->SetFloat("uGridSpacing", 1.0f);
	//pShader->SetFloat("uFadeStart", 18.f);
	//pShader->SetFloat("uFadeEnd", 100.f);
	//pShader->SetVec3("uCamPos", pCamera->Position);
	//pGridAxis->DrawGrid();
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

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.03f));
	pShader->SetMat4("model", model);

	glm::vec4 lampPositionsMax[] = {
	glm::vec4(14.793f,  18.696f, 72.313f, 1.0f),
	glm::vec4(97.918f,  18.696f, 72.313f, 1.0f),
	glm::vec4(-48.365f,  18.696f, 72.313f, 1.0f),
	glm::vec4(-48.365f,-198.375f, 72.313f, 1.0f),
	};

	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		glm::vec4 posWorld = model * lampPositionsMax[i];
		gPointLights[i].position = glm::vec3(posWorld);

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

	pModel->Draw(pShader->GetProgram("Assets"));

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Desenha o Skybox
	pShader->Use("Skybox");
	pShader->SetMat4("projection", projection);
	view = glm::mat4(glm::mat3(pCamera->GetViewMatrix()));
	pShader->SetMat4("view", view);
	pSkybox->DrawSkybox();

	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Opacos (DESTINATION)
	/*pShader->Use("Blend");
	pShader->SetMat4("projection", projection);
	view = pCamera->GetViewMatrix();
	pShader->SetMat4("view", view);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	pShader->SetMat4("model", model);
	pShader->SetVec4("objColor", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	pShader->SetBool("isTexured", false);
	pPlane1.render();*/
	

	// Transparentes (SOURCE)
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//pShader->Use("Blend");
	//pShader->SetMat4("projection", projection);
	//view = pCamera->GetViewMatrix();
	//pShader->SetMat4("view", view);
	//model = glm::mat4(1.0);
	//model = glm::translate(model, glm::vec3(0.0f, 5.0f, 1.0f));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	//pShader->SetMat4("model", model);
	//pShader->SetVec4("objColor", glm::vec4(0.0f, 0.0f, 1.0f, 0.6f));
	//pShader->SetBool("isTexured", false);
	//pPlane2.render();

	//glDisable(GL_BLEND);


	// Transparentes (SOURCE)
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.5f);

	//pShader->Use("Blend");
	//pShader->SetMat4("projection", projection);
	//view = pCamera->GetViewMatrix();
	//pShader->SetMat4("view", view);
	//model = glm::mat4(1.0);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	//pShader->SetMat4("model", model);
	//pShader->SetVec4("objColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	//glActiveTexture(GL_TEXTURE0);
	//pTextures->ApplyTexture(0);
	//pShader->SetInt("Texture0", 0);
	//pShader->SetBool("isTexured", true);

	//pPlane2.render();

	//glDisable(GL_ALPHA_TEST);
	//glDisable(GL_BLEND);


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


