#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include "./include/glm/gtc/type_ptr.hpp"
#include "Camera.h"
#include <vector>


class CAudio {
public:
	CAudio();
	~CAudio();
	bool Initialise();

	bool LoadEventSound(const char *filename, bool loop);
	bool PlayEventSound(glm::vec3 p);
	void StopEventSound();
	void ToogleSoundEventFilter();
	void RemoveFilters();
	void EngineFilter(float speed);

	bool LoadMusicStream(const char *filename);
	bool PlayMusicStream();
	void ToggleMusicFilter();
	void IncreaseMusicVolume();
	void DecreaseMusicVolume();
	
	void Update(CCamera *m_pCamera, glm::vec3 p, float speed);

	bool m_eventSoundFilterActive = false;

private:
	void FmodErrorCheck(FMOD_RESULT result);
	void ToFMODVector(glm::vec3 &glVec3, FMOD_VECTOR *fmodVec);

	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD

	// Car engine sound
	FMOD::Sound *m_eventSound;
	FMOD::Channel *m_eventChannel;

	// Background music
	FMOD::Sound *m_music;
	FMOD::DSP *m_musicFilter;
	FMOD::Channel *m_musicChannel;
	FMOD::DSP *m_musicDSPHead;
	FMOD::DSP *m_musicDSPHeadInput;

	bool m_musicFilterActive;
	float m_musicVolume = 1.0f;
	float m_Carspeed = -1000.0f;
	float m_frequency;

	FMOD::DSP *m_dsp; // For the DSP effect using DSPCallback
	FMOD::DSP *m_dsp2; // For the DSP effect using DSPCallback2
	FMOD::DSP *m_dsp3; // For the DSP effect using DSPCallback2

	// Camera & Car position vectors
	FMOD_VECTOR camPos;
	FMOD_VECTOR carPos;
	FMOD_VECTOR camUP;
	FMOD_VECTOR camForw;

};
