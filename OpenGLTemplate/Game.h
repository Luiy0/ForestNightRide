#pragma once

#include "Common.h"
#include "GameWindow.h"
#include <vector>
#include <math.h>

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CCube;
class CTetrahedron;
class CHeightMapTerrain;
class CTorus;
class CFrameBufferObject;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();
	void RenderScene(int pass);

	// Pointers to game objects.  They will get allocated in Game::Initialise()
		
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	COpenAssetImportMesh *m_pTreeMesh;
	COpenAssetImportMesh *m_pCarMesh;
	COpenAssetImportMesh *m_pBarrierMesh;
	COpenAssetImportMesh *m_pTrunk;
	COpenAssetImportMesh *m_obstacle;
	COpenAssetImportMesh *m_billboard;

	CSphere *m_pSphere;
	CCube *m_pCube;
	CCube *m_pCubeY;
	CCube *m_pCubeR;
	CTetrahedron *m_pTetrahedron;
	CHeightMapTerrain *m_pHeightmapTerrain;
	CTorus *m_pTorus;
	CTorus *m_pTorus2;
	CSkybox *m_pSkybox;
	CPlane *m_pPlanarTerrain;
	CPlane *m_pPlane;

	CFreeTypeFont *m_pFtFont;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CHighResolutionTimer *m_pTimer;
	CAudio *m_pAudio;
	CAudio *m_pAudio2;
	CCatmullRom *m_pCatmullRom;
	CFrameBufferObject *m_pFBO;
	

	// Some other member variables
	glm::vec3 T, T2, N, B, p, pNext, d, distanceDisplay;
	double m_dt, speed = 0.05f, m_rotY, shakeTimer = 2.0f;
	int m_framesPerSecond;
	bool m_appActive, infoToogle = false, engineToogle = false, shake = false;
	float m_currentDistance, m_totalTime = 0;
	int currentCam = 0, carLeft = 0, carRight = 0, light = 0, crash = 0;
	bool collision = false;

	vector<glm::vec3> obstaclePos;
	


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayText();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;


};
