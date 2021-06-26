/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)
 Modified on 07/05/2021 by student Luis Flores (20032745) 
*/

// Setup includes
#include "game.h"
#include "HighResolutionTimer.h"
#include "GameWindow.h"
#include "./include/gl/wglew.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "Cube.h"
#include "Tetrahedron.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "HeightMapTerrain.h"
#include "Torus.h"
#include "FrameBufferObject.h"
#include <iostream>

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pTreeMesh = NULL;
	m_pCarMesh = NULL;
	m_pBarrierMesh = NULL;
	m_pSphere = NULL;
	m_pCube = NULL;
	m_pCubeY = NULL;
	m_pCubeR = NULL;
	m_pTetrahedron = NULL;
	m_pTimer = NULL;
	m_pAudio = NULL;
	m_pAudio2 = NULL;
	m_pCatmullRom = NULL;
	m_pHeightmapTerrain = NULL;
	m_pTorus = NULL;
	m_pTorus = NULL;
	m_pTrunk = NULL;
	m_obstacle = NULL;
	m_pFBO = NULL;
	m_pPlane = NULL;
	m_billboard = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;

}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pTreeMesh;
	delete m_pCarMesh;
	delete m_pBarrierMesh;
	delete m_pSphere;
	delete m_pCube;
	delete m_pCubeY;
	delete m_pCubeR;
	delete m_pAudio;
	delete m_pAudio2;
	delete m_pCatmullRom;
	delete m_pTetrahedron;
	delete m_pHeightmapTerrain;
	delete m_pTorus;
	delete m_pTorus2;
	delete m_pTrunk;
	delete m_obstacle;
	delete m_pFBO;
	delete m_pPlane;
	delete m_billboard;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pAudio = new CAudio;
	m_pAudio2 = new CAudio;
	m_pFtFont = new CFreeTypeFont;
	m_pTimer = new CHighResolutionTimer;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pFBO = new CFrameBufferObject;
		
	m_pTreeMesh = new COpenAssetImportMesh;
	m_pCarMesh = new COpenAssetImportMesh;
	m_pBarrierMesh = new COpenAssetImportMesh;
	m_pTrunk = new COpenAssetImportMesh;
	m_obstacle = new COpenAssetImportMesh;
	m_billboard = new COpenAssetImportMesh;

	m_pSkybox = new CSkybox;
	m_pSphere = new CSphere;
	m_pCube = new CCube;
	m_pCubeY = new CCube;
	m_pCubeR = new CCube;
	m_pTetrahedron = new CTetrahedron;
	m_pHeightmapTerrain = new CHeightMapTerrain;
	m_pTorus = new CTorus;
	m_pTorus2 = new CTorus;
	m_pPlane = new CPlane;
	m_pCatmullRom = new CCatmullRom;
	
	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create the skybox
	// Skybox downloaded from http://haxor.thelaborat.org/resources/texture/skybox/nightsky/
	m_pSkybox->Create(2500.0f);
	
	// Create plane for FBO TV
	m_pPlane->Create("resources\\textures\\", "GrassBright.bmp", 40.0f, 20.0f, 1.0f); // TV

	// Load fonts
	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pTreeMesh->Load("resources\\models\\Tree\\lowpolytree.obj"); // Downloaded from https://free3d.com/3d-model/low-poly-tree-73217.html on 16/03/2021
	m_pCarMesh->Load("resources\\models\\Car\\Truck.obj"); // Jeep mesh obtained from AGT module
	m_pBarrierMesh->Load("resources\\models\\Barrier\\Road Barrier 01\\Road Barrier 01a.obj"); //Downloaded from https://free3d.com/3d-model/road-barriers-998950.html on 16/03/2021
	m_pTrunk->Load("resources\\models\\Trunk\\Trunk.obj");// Downloaded from https://free3d.com/3d-model/trunk-wood-342814.html on 02/04/2021
	m_obstacle->Load("resources\\models\\Crate\\container.obj"); // Downloaded from https://free3d.com/3d-model/container-v1--229361.html 03/05/2021
	m_billboard->Load("resources\\models\\Billboard\\bilboard.obj"); // Free 3d https://free3d.com/3d-model/bilboard-v1--29854.html

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "GrassBright.bmp", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	
	// Create a cube (solid colors as textures) - Indexed geometry
	m_pCube->Create("resources\\textures\\", "GrassBright.bmp", 30.0f, 50.0f, 10.0f, 1.0f); // From lab
	m_pCubeY->Create("resources\\textures\\", "yellow.jpg", 10.0f, 30.0f, 20.0f, 1.0f); // https://www.publicdomainpictures.net/pictures/210000/nahled/yellow-texture-background-1490218047ia6.jpg
	m_pCubeR->Create("resources\\textures\\", "red.jpg", 10.0f, 70.0f, 20.0f, 1.0f); // https://www.publicdomainpictures.net/pictures/210000/velka/red-texture-background-14902179577fa.jpg

	// Create tetrahedron - Non indexed
	m_pTetrahedron->Create("resources\\textures\\", "Rock.jpg", 200.0f, 400.0, 200.0f, 50.0); // Downloaded from https://live.staticflickr.com/28/42366328_2dba7ae11b_b_d.jpg

	// Create torus
	m_pTorus->Create("resources\\textures\\", "Tile41a.jpg", 0.5f, 0.2f, 100);
	m_pTorus2->Create("resources\\textures\\", "stoneWall.jpg", 0.5f, 0.2f, 100); // https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.stockio.com%2Ffree-photo%2Fstone-wall-moss-texture&psig=AOvVaw2i2QivRUmmwExfoBxgM0fT&ust=1620454308965000&source=images&cd=vfe&ved=0CA0QjhxqFwoTCMCl__n0tvACFQAAAAAdAAAAABAJ
	
	// Create the heightmap terrain  (textures obtained from module laboratory)
	m_pHeightmapTerrain->Create("resources\\textures\\terrainHeightMap200.bmp", "resources\\textures\\GrassBright.bmp", glm::vec3(0, 0, 0), 2000.0f, 2000.0f, 100.0f);

	//calculate control points first, display in HUD XYZ coordinates 
	// Create road
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateTrack();

	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise(); m_pAudio2->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\carIdle.wav", true);		// Royalty free sound from freesound.org https://freesound.org/people/InspectorJ/sounds/345558/
	m_pAudio2->LoadEventSound("resources\\Audio\\crash.wav", false);		https://freesound.org/people/Eponn/sounds/420356/
	m_pAudio->LoadMusicStream("resources\\Audio\\forest.wav");				// Royalty free music from https://freesound.org/people/sethlind/sounds/332722/
	m_pAudio->PlayMusicStream();

	// Create FBO
	m_pFBO->Create(width, height);
	
}

// Render method runs repeatedly in a loop
void Game::Render(){

	m_pFBO->Bind();
	RenderScene(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderScene(1);

	// Draw the 2D graphics after the 3D graphics
	DisplayText();
	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());

}

void Game::RenderScene(int pass) {

	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("t", (float)m_dt);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	// Light 1 - White - CAR
	// Position of light source *in world coordinates*
	glm::vec4 lightPosition1;
	if (carRight == 1) // Move car to the left using TNB
		lightPosition1 = glm::vec4(7.0 + p.x, p.y + 10.0f, p.z + 7.0f, 1);
	if (carLeft == 1) // Move car to the right using TNB
		lightPosition1 = glm::vec4(p.x - 7.0f, p.y + 10.0f, p.z - 7.0f, 1);
	if (carLeft == 0 && carRight == 0) // Move car to centerline
		lightPosition1 = glm::vec4(p.x, p.y + 10.0f, p.z, 1);

	pMainProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix * glm::vec3(T.x, -0.25 * glm::atan(B.z, B.y), T.z)));
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("light1.exponent", 15.0f); // angular attenuation
	pMainProgram->SetUniform("light1.cutoff", 30.0f);
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 25.0f);	// Shininess material property

	// Light 2 -  Red
	glm::vec4 lightPosition2(10.0f, 195.0f, 10.0f, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light2.position", viewMatrix * lightPosition2);
	pMainProgram->SetUniform("light2.La", glm::vec3(1.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("light2.Ld", glm::vec3(1.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("light2.Ls", glm::vec3(1.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("light2.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.0, -1.0, 0.0)));
	pMainProgram->SetUniform("light2.exponent", 20.0f);
	pMainProgram->SetUniform("light2.cutoff", 30.0f);
	pMainProgram->SetUniform("material2.Ma", glm::vec3(0.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material2.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material2.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material2.shininess", 15.0f);	// Shininess material property

	// Light 3 - Green
	glm::vec4 lightPosition3(30.0f, 175.0f, 130.0f, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light3.position", viewMatrix * lightPosition3);
	pMainProgram->SetUniform("light3.La", glm::vec3(0.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light3.Ld", glm::vec3(0.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light3.Ls", glm::vec3(0.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light3.direction", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pMainProgram->SetUniform("light3.exponent", 20.0f);
	pMainProgram->SetUniform("light3.cutoff", 30.0f);
	pMainProgram->SetUniform("material3.Ma", glm::vec3(0.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material3.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material3.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material3.shininess", 15.0f);	// Shininess material property

	// Light 4 - Yellow
	glm::vec4 lightPosition4(50.0f, 185.0f, 0.0f, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light4.position", viewMatrix * lightPosition4);
	pMainProgram->SetUniform("light4.La", glm::vec3(1.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light4.Ld", glm::vec3(1.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light4.Ls", glm::vec3(1.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light4.direction", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pMainProgram->SetUniform("light4.exponent", 20.0f);
	pMainProgram->SetUniform("light4.cutoff", 30.0f);
	pMainProgram->SetUniform("material4.Ma", glm::vec3(0.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material4.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material4.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material4.shininess", 15.0f);	// Shininess material property

	// Light 5 - Yellow
	glm::vec4 lightPosition5(-700.0f, 180.0f, -200.0f, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light5.position", viewMatrix * lightPosition5);
	pMainProgram->SetUniform("light5.La", glm::vec3(1.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light5.Ld", glm::vec3(1.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light5.Ls", glm::vec3(1.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light5.direction", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pMainProgram->SetUniform("light5.exponent", 20.0f);
	pMainProgram->SetUniform("light5.cutoff", 30.0f);
	pMainProgram->SetUniform("material5.Ma", glm::vec3(0.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material5.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material5.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material5.shininess", 15.0f);	// Shininess material property

	// Light 6 - Blueish MOON
	glm::vec4 lightPosition6 = glm::vec4(700.f, 500.f, -900.f, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light6.position", viewMatrix * lightPosition6); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light6.La", glm::vec3(0.0f, 0.33f, 0.65f));		// Ambient colour of light
	pMainProgram->SetUniform("light6.Ld", glm::vec3(0.0f, 0.33f, 0.65f));		// Diffuse colour of light
	pMainProgram->SetUniform("light6.Ls", glm::vec3(0.0f, 0.53f, 0.85f));		// Specular colour of light
	pMainProgram->SetUniform("light6.direction", glm::normalize(viewNormalMatrix * glm::vec3(-0.75f, -0.45f, 0.75f)));
	pMainProgram->SetUniform("light6.exponent", 10.0f); // angular attenuation
	pMainProgram->SetUniform("light6.cutoff", 60.0f);
	pMainProgram->SetUniform("material6.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material6.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material6.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material6.shininess", 15.0f);	// Shininess material property

	// Light 7 -  Sphere white light
	glm::vec4 lightPosition7(-500.0f, 50.0, 430.0f, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light7.position", viewMatrix * lightPosition7);
	pMainProgram->SetUniform("light7.La", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light7.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light7.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light7.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.0, -1.0, 0.0)));
	pMainProgram->SetUniform("light7.exponent", 20.0f);
	pMainProgram->SetUniform("light7.cutoff", 30.0f);
	pMainProgram->SetUniform("material7.Ma", glm::vec3(0.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material7.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material7.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material7.shininess", 15.0f);	// Shininess material property

	// Render the skybox with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	glm::vec3 vEye = m_pCamera->GetPosition(); 	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Turn on/off vehicle spotlight
	if (light == 0) {
		// White light
		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.0f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance
	}
	else if (light == 1) {
		// White light
		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.0f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	}


	// Render heightmap terrain (Must be after vehicle spotlight code)
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pHeightmapTerrain->Render();
	modelViewMatrixStack.Pop();

	// Render the green cube
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(0.0f, 135.0f, 0.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCube->Render();
	modelViewMatrixStack.Pop();

	// Render the yellow cube
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(20.0f, 115.0f, 0.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCubeY->Render();
	modelViewMatrixStack.Pop();

	// Render the red cube
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(0.0f, 105.0f, 10.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCubeR->Render();
	modelViewMatrixStack.Pop();

	// Render crates
	obstaclePos.push_back(glm::vec3(400.0f, -10.8, -400.0f));
	obstaclePos.push_back(glm::vec3(700.0f, -26.8, -100.0f + 7.f));
	obstaclePos.push_back(glm::vec3(700.0f, -26.8, -100.0f - 7.f));
	obstaclePos.push_back(glm::vec3(100.0f, 33.5, 200.0f));		

	for (int x = 0; x < 4; x++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(obstaclePos[x].x, obstaclePos[x].y + 5.f, obstaclePos[x].z);
		modelViewMatrixStack.RotateY(m_rotY);
		modelViewMatrixStack.RotateX(90.0f);
		modelViewMatrixStack.Scale(glm::vec3(0.07));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_obstacle->Render();
		modelViewMatrixStack.Pop();
	}

	// Render the tetrahedrons
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(-500.0f, -30.0f, -200.0f);
	modelViewMatrixStack.Scale(0.6);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pTetrahedron->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(40.0f, 40.0f, -130.0f);
	modelViewMatrixStack.Scale(0.5);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pTetrahedron->Render();
	modelViewMatrixStack.Pop();

	// Render torus
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(-750.0f, 0.0f, -450.0f);
	modelViewMatrixStack.RotateX(T.x); modelViewMatrixStack.RotateY(p.y); modelViewMatrixStack.RotateZ(T.z);
	modelViewMatrixStack.Scale(25);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pTorus->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(-700.0f, -15.0f, -200.0f);
	modelViewMatrixStack.RotateX(glm::radians(90.f));
	modelViewMatrixStack.RotateY(m_rotY);
	modelViewMatrixStack.Scale(75);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pTorus2->Render();
	modelViewMatrixStack.Pop();

	// Render Jeep
	modelViewMatrixStack.Push();
	if (carRight == 1) // Move car to the left using TNB
		modelViewMatrixStack.Translate(glm::vec3(7.f * N.x + p.x, p.y, 7.f * N.z + p.z));
	else if (carLeft == 1) // Move car to the right using TNB
		modelViewMatrixStack.Translate(glm::vec3(-7.f * N.x + p.x, p.y, -7.f * N.z + p.z));
	else //(carLeft == 0 && carRight == 0) // Move car to centerline
		modelViewMatrixStack.Translate(glm::vec3(p.x, p.y, p.z));

	modelViewMatrixStack.RotateY(glm::atan(T.x, T.z));	// Left/Right rotation
	modelViewMatrixStack.RotateX(glm::radians(180.f));	// Orient the car up/down
	modelViewMatrixStack.RotateX(glm::atan(B.z, B.y));	// Up/Down rotation
	modelViewMatrixStack.RotateZ(glm::radians(180.f));	// Orient the car sideways
	modelViewMatrixStack.Scale(5.0f);
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCarMesh->Render();
	modelViewMatrixStack.Pop();

	// Render Track
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	//m_pCatmullRom->RenderCentreline(); // Rendering path
	//m_pCatmullRom->RenderOffsetCurves(); // Rendering offset curves
	m_pCatmullRom->RenderTrack(); // Render Track
	modelViewMatrixStack.Pop();

	// Render barriers and trunks
	for (int x = 0; x < m_pCatmullRom->m_rightOffsetPoints.size(); x = x + 5) {
		if (x == m_pCatmullRom->m_rightOffsetPoints.size() - 1)
			T2 = glm::normalize(m_pCatmullRom->m_rightOffsetPoints[0] - m_pCatmullRom->m_rightOffsetPoints[x]);
		else
			T2 = glm::normalize(m_pCatmullRom->m_rightOffsetPoints[x + 1] - m_pCatmullRom->m_rightOffsetPoints[x]);

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pCatmullRom->m_rightOffsetPoints[x]);
		modelViewMatrixStack.RotateY(glm::radians(90.0f));
		modelViewMatrixStack.RotateY(glm::atan(T2.x, T2.z));
		modelViewMatrixStack.Scale(3.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrierMesh->Render();
		modelViewMatrixStack.Translate(0.0f, -6.1f, 0.0f);
		modelViewMatrixStack.RotateX(glm::radians(90.f));
		modelViewMatrixStack.Scale(glm::vec3(4.0f, 4.0f, 6.0f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTrunk->Render();
		modelViewMatrixStack.Pop();

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_pCatmullRom->m_leftOffsetPoints[x]);
		modelViewMatrixStack.RotateY(glm::radians(90.0f));
		modelViewMatrixStack.RotateY(glm::atan(T2.x, T2.z));
		modelViewMatrixStack.Scale(3.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrierMesh->Render();
		modelViewMatrixStack.Translate(-1.0f, -6.1f, -1.0f);
		modelViewMatrixStack.RotateX(glm::radians(90.f));
		modelViewMatrixStack.Scale(glm::vec3(4.0f, 4.0f, 6.0f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTrunk->Render();
		modelViewMatrixStack.Pop();

	}

	// Render trees at specific locations
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(70.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(70.0f, 0.0f, 363.0f)) + 30.0f, -363.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(30.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(30.0f, 0.0f, -440.0f)) + 15.0f, -440.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(10.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(10.0f, 0.0f, -440.0f)) + 15.0f, -440.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-30.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-30.0f, 0.0f, -440.0f)) + 15.0f, -440.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-50.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-50.0f, 0.0f, -440.0f)) + 15.0f, -440.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-90.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-90.0f, 0.0f, -440.0f)) + 15.0f, -440.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-270.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-270.0f, 0.0f, -300.0f)) + 15.0f, -300.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-270.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-270.0f, 0.0f, -280.0f)) + 25.0f, -280.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-270.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-270.0f, 0.0f, -250.0f)) + 25.0f, -250.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-280.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-280.0f, 0.0f, -240.0f)) + 25.0f, -240.0f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-270.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-270.0f, 0.0f, -200.0f)) + 20.0f, -200.0f);
		modelViewMatrixStack.Scale(15.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-190.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-190.0f, 0.0f, -350.0f)) + 35.0f, -350.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-790.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-790.0f, 0.0f, 150.0f)) + 35.0f, 150.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-660.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-660.0f, 0.0f, -30.f)) + 20.0f, -30.f);
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(90.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(90.0f, 0.0f, 370.0f)) + 35.0f, 370.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(630.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(630.0f, 0.0f, -200.0f)) + 35.0f, -200.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(290.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(290.0f, 0.0f, -900.0f)) + 35.0f, -900.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(700.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(700.0f, 0.0f, -480.0f)) + 35.0f, -480.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-330.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-330.0f, 0.0f, -730.0f)) + 35.0f, -730.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-330.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-330.0f, 0.0f, -475.0f)) + 35.0f, -475.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-775.0f, m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-775.0f, 0.0f, -320.0f)) + 35.0f, -320.0f);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTreeMesh->Render();
		modelViewMatrixStack.Pop();

	}


	if (pass == 1) {

		// Render the plane for the TV
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(420.f, 0.f, -155.f);
			modelViewMatrixStack.RotateY(glm::radians(-35.f));
			modelViewMatrixStack.RotateX(glm::radians(-90.f));
			modelViewMatrixStack.Scale(10.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_billboard->Render();
		modelViewMatrixStack.Pop();
		
		glDisable(GL_CULL_FACE);
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(420.f, 40.f, -155.f));
			modelViewMatrixStack.RotateY(glm::radians(55.f));	
			modelViewMatrixStack.RotateX(glm::radians(-90.f));					
			modelViewMatrixStack.Scale(1.4f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pFBO->BindTexture(0);
			m_pPlane->Render(false);
		modelViewMatrixStack.Pop();
		glEnable(GL_CULL_FACE);

		// Render the sphere
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(-500.0f, 25.0, 430.0f));
			modelViewMatrixStack.RotateX(glm::radians(90.f));
			modelViewMatrixStack.RotateZ(m_rotY);
			modelViewMatrixStack.Scale(30.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			pMainProgram->SetUniform("bUseTexture", true);
			m_pFBO->BindTexture(0);
			m_pSphere->Render(); // texture bind disabled in sphere.cpp
		modelViewMatrixStack.Pop();
	}
}

// Update method runs repeatedly with the Render method
void Game::Update(){

	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_pCamera->Update(m_dt + speed);	
	m_rotY += 0.005f * m_dt;

	// Add variable m_cameraSpeed to control camera (car) speed based on user input.
	m_currentDistance += m_dt * (speed);
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 1.0f, pNext);
	
	// TNB Frame
	T = glm::normalize(glm::vec3(pNext - p));
	N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
	B = glm::normalize(glm::cross(N, T));

	// Crate-Jeep collision
	for (int x = 0; x < obstaclePos.size(); x++) {
		if (carRight == 1)
			d = obstaclePos[x] - glm::vec3(7.f * N.x + p.x, obstaclePos[x].y, 7.f * N.z + p.z);
		else if (carLeft == 1)
			d = obstaclePos[x] - glm::vec3(-7.f * N.x + p.x, obstaclePos[x].y, -7.f * N.z + p.z);
		else
			d = obstaclePos[x] - glm::vec3(p.x, obstaclePos[x].y, p.z);

		if (glm::length(d) < 2.f) {
			m_pAudio2->PlayEventSound(glm::vec3(p));
			distanceDisplay = d;
			m_dt = 0;
			m_currentDistance = 0;
			shake = true;
		}
	}

	// Set the camera view
	if (currentCam == 0) {
		if (carRight == 1)
			m_pCamera->Set(glm::vec3(p.x + 7.0f * N.x, p.y + 6.0f, p.z + 7.0f * N.z), (N * 7.f + p + 25.f * T), glm::vec3(0, 1, 0)); // First person camera
		else if (carLeft == 1)
			m_pCamera->Set(glm::vec3(p.x - 7.0f * N.x, p.y + 6.0f, p.z -7.0f * N.z), (N * -7.f + p + 25.f * T), glm::vec3(0, 1, 0)); // First person camera
		else
			m_pCamera->Set(glm::vec3(p.x + 1.0f * T.x, p.y + 6.0f, p.z + 1.0f * T.z), (p + 25.f * T), glm::vec3(0, 1, 0)); // First person camera 
	}		
	if(currentCam == 1)
		m_pCamera->Set(glm::vec3(p.x, p.y + 50.0f, p.z), (p + 1.f* T), glm::vec3(0, 1, 0)); // Top view camera
	if(currentCam == 2)
		m_pCamera->Set(glm::vec3(p.x - 30.f * T.x, p.y + 15.f, p.z - 30.f * T.z), (p + 25.f * T), glm::vec3(0, 1, 0)); // Third person camera

	// Camera shake
	if (shake == true) {
		shakeTimer -= m_dt / 1000;
		glm::vec3 pos = m_pCamera->GetPosition();
		m_pCamera->Set(glm::vec3(pos.x + glm::sin(m_dt), pos.y, pos.z + glm::sin(m_dt)), m_pCamera->GetView(), glm::vec3(0, 1, 0));
		if (shakeTimer < 0) {
			shake = false;
			crash += 1;
			shakeTimer = 2.0f;
		}
	}

	// Audio
	m_pAudio->Update(m_pCamera, p, speed);
	m_pAudio2->Update(m_pCamera, p, speed);
}
 
void Game::DisplayText(){

	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;
	m_totalTime += m_elapsedTime;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }
	
	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	m_pFtFont->Render(20, height - 40, 20, "MPH: %.1f", speed * 1000);
	m_pFtFont->Render(20, height - 60, 20, "LAPS: %u", m_pCatmullRom->CurrentLap(m_currentDistance));
	m_pFtFont->Render(20, height - 80, 20, "TIME: %.1f", m_totalTime / 10000);
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pFtFont->Render(20, height - 100, 20, "CRASHES: %d", crash);

	if (m_framesPerSecond > 0 && infoToogle == true) {		
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		m_pFtFont->Render(20, height - 120, 20, "POSITION CAR: %.1f %.1f", p.x, p.z);
		m_pFtFont->Render(20, height - 140, 20, "CAMERA POSITION: %.1f %.1f %.1f", m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z);
		m_pFtFont->Render(20, height - 160, 20, "HEIGHT CENTER: %.1f", m_pHeightmapTerrain->ReturnGroundHeight(p));
		m_pFtFont->Render(20, height - 180, 20, "HEIGHT LEFT: %.1f", m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(-10.0f * N.x + p.x, p.y, -10.0f * N.z + p.z)));
		m_pFtFont->Render(20, height - 200, 20, "HEIGHT RIGHT: %.1f", m_pHeightmapTerrain->ReturnGroundHeight(glm::vec3(10.0f * N.x + p.x, p.y, 10.0f * N.z + p.z)));
		m_pFtFont->Render(20, height - 220, 20, "ROTATION X: %.3f", glm::atan(B.z, B.y));
		m_pFtFont->Render(20, height - 240, 20, "COLLISION DISTANCE XYZ: %.1f %.1f %.1f", distanceDisplay.x, distanceDisplay.y, distanceDisplay.z);
		glEnable(GL_DEPTH_TEST);
	}	
}

void Game::GameLoop()
{
	// Fixed timer
	m_dt = m_pTimer->Elapsed();
	if (m_dt > 1000.0 / (double) Game::FPS) {
		m_pTimer->Start();
		Update(); Render(); 
	}
}

WPARAM Game::Execute() 
{
	m_pTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();
	m_pTimer->Start();
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();
	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_UP:
			if (speed * 1000 >= 100)
				speed = 0.1;
			else
				speed += 0.01f; // *1000
			break;
		case VK_DOWN:
			if (speed * 1000 <= -100)
				speed = -0.1;
			else
				speed -= 0.01f;
			break;
		case VK_RIGHT:
			if (carRight < 1) {
				carLeft -= 1;
				carRight += 1;
			}
			break;
		case VK_LEFT:
			if (carLeft < 1) {
				carRight -= 1;
				carLeft += 1;
			}
			break;
		case '1':
			if (engineToogle == false) {
				m_pAudio->PlayEventSound(p);
				engineToogle = true;
			}
			else if (engineToogle == true){
				m_pAudio->StopEventSound();
				engineToogle = false;
			}
			break;
		case '2': 
			m_pAudio->ToggleMusicFilter();
			break;
		case '3':
			m_pAudio->ToogleSoundEventFilter();
			break;
		case '4':
			m_pAudio->RemoveFilters();
			break;
		case VK_HOME:
			m_pAudio->IncreaseMusicVolume();
			break;
		case VK_END:
			m_pAudio->DecreaseMusicVolume();
			break;
		case 'C':
			currentCam++;
			if (currentCam > 3)
				currentCam = 0;
			break;
		case 'I':
			if (infoToogle == false)
				infoToogle = true;
			else if (infoToogle == true)
				infoToogle = false;
			break;
		case 'L':
			if (light == 0)
				light = 1;
			else if (light == 1)
				light = 0;
			break;
		case VK_F1:
			m_pAudio->PlayEventSound(p);
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
