#include "Audio.h"

#pragma comment(lib, "lib/fmod_vc.lib")


// DSP callback Used to increase volume
FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels)
{
	//This DSP filter increases the volume of event - Always ON
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;
	for (unsigned int sample = 0; sample < length; sample++)
		for (int chan = 0; chan < *outchannels; chan++)
			outbuffer[(sample * *outchannels) + chan] = inbuffer[(sample * inchannels) + chan] * 50.0f;
	return FMOD_OK;
}

// DSP callback - highpass filter
FMOD_RESULT F_CALLBACK DSPCallback2(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels)
{
	// Creating circular buffer
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;
	int sampleCount = 0;
	static float *buffer = NULL;
	const int sizeBuffer = 1024;
	
	// highpass coefficients from https://www.researchgate.net/figure/Filter-Coefficients-for-low-pass-and-high-pass-filter_tbl1_50384304
	float highCoef[] = { -0.0055441199344672806f, -0.074845619115308265f, 0.83799343436458384f, -0.074845619115308265f, -0.0055441199344672806f};

	// highpass coefficients from matlab filter designer
	float highCoef2[] = { -0.007204099620894590348674224600244997418, 0.013501199867095557000107319822745921556, 0.000282824632567476956773039331594077339,-0.004361070826930018261291532866152920178,-0.0044142711399555445428433131382917054  ,-0.002497628617405077584845418670056460542,-0.000195344854959845939772200562600801277, 0.001636075652182752320287772995754949079, 0.002623920164346447113368210324324536487, 0.002702198630829333054342855646723364771, 0.002008158742341557920912942236668641272, 0.000784240403134063308182577500815568783,-0.000625240858373468980611897816146438345,-0.001875342262125212922790895575531067152,-0.002643398486449209834614482161896376056,-0.002722219152489624774404752827194897691,-0.002052441841657894755490687543897365686,-0.000768499879201139371107109887759634148, 0.000830911412354150540754416098820911429, 0.002336681589032818355533560605863385717, 0.003326744890366320570734837147597318108, 0.003477127913064344613691680763167823898, 0.002662817686660483296190449209461803548, 0.001014843407676914010390190057364634413,-0.001090302024244999912552800402920638589,-0.003108957192393947861763781048693999765,-0.004462526452598939742666406260696021491,-0.004701115142451760958719475524958397727,-0.003639699444026417343017731909071699192,-0.001440317314871718528540567305640252016, 0.001405383326333753922499081312480484485, 0.004168004567605933068097368021653892356, 0.006063190599930884903956407327996203094, 0.006460249852926915233031213148251481471, 0.00508628628198103376750749404777707241 , 0.002132929779114999934569452122445909481,-0.00174687000388353452957901357223136074 ,-0.005577261032978958997519125517783322721,-0.008286350188584789763779347993022383889,-0.008991718021314876185812181574874557555,-0.00726154126763852575399127431410306599 ,-0.003292399508641371749795290213569387561, 0.002077187300609420472774502286483766511, 0.007521686753860641963531641351892176317, 0.011544481207502916425555739010633260477, 0.012852537045410790395028044486025464721, 0.010733133682703947797687504817076842301, 0.005309877934053713020856601900732130161,-0.002366792758338352557767603201455131057,-0.010489791008637215988241742081754637184,-0.016877021776799623620224366504771751352,-0.019501815004992061053368246348327375017,-0.017033139690430135898546737394099181984,-0.009273459839467793411338369935492664808, 0.002594994947632741581072801650975634402, 0.01610368189036957484172596366533980472 , 0.027874002500552351457852040539364679717, 0.034306922659694177868416886667546350509, 0.032287659820506998953337784996620030142, 0.019968681765383074905306770574497932103,-0.002745907599942399267278769769973223447,-0.033920109598087631874729908076915307902,-0.069880497154230239442718186637648614123,-0.105752238400384834804413003439549356699,-0.136226882746936256429748368645959999412,-0.156681311482443019755805835302453488111, 0.8361226232329929120012934617989230901  ,-0.156681311482443019755805835302453488111,-0.136226882746936256429748368645959999412,-0.105752238400384834804413003439549356699,-0.069880497154230239442718186637648614123,-0.033920109598087631874729908076915307902,-0.002745907599942399267278769769973223447, 0.019968681765383074905306770574497932103, 0.032287659820506998953337784996620030142, 0.034306922659694177868416886667546350509, 0.027874002500552351457852040539364679717, 0.01610368189036957484172596366533980472 , 0.002594994947632741581072801650975634402,-0.009273459839467793411338369935492664808,-0.017033139690430135898546737394099181984,-0.019501815004992061053368246348327375017,-0.016877021776799623620224366504771751352,-0.010489791008637215988241742081754637184,-0.002366792758338352557767603201455131057, 0.005309877934053713020856601900732130161, 0.010733133682703947797687504817076842301, 0.012852537045410790395028044486025464721, 0.011544481207502916425555739010633260477, 0.007521686753860641963531641351892176317, 0.002077187300609420472774502286483766511,-0.003292399508641371749795290213569387561,-0.00726154126763852575399127431410306599 ,-0.008991718021314876185812181574874557555,-0.008286350188584789763779347993022383889,-0.005577261032978958997519125517783322721,-0.00174687000388353452957901357223136074 , 0.002132929779114999934569452122445909481, 0.00508628628198103376750749404777707241 , 0.006460249852926915233031213148251481471, 0.006063190599930884903956407327996203094, 0.004168004567605933068097368021653892356, 0.001405383326333753922499081312480484485,-0.001440317314871718528540567305640252016,-0.003639699444026417343017731909071699192,-0.004701115142451760958719475524958397727,-0.004462526452598939742666406260696021491,-0.003108957192393947861763781048693999765,-0.001090302024244999912552800402920638589, 0.001014843407676914010390190057364634413, 0.002662817686660483296190449209461803548, 0.003477127913064344613691680763167823898, 0.003326744890366320570734837147597318108, 0.002336681589032818355533560605863385717, 0.000830911412354150540754416098820911429,-0.000768499879201139371107109887759634148,-0.002052441841657894755490687543897365686,-0.002722219152489624774404752827194897691,-0.002643398486449209834614482161896376056,-0.001875342262125212922790895575531067152,-0.000625240858373468980611897816146438345, 0.000784240403134063308182577500815568783, 0.002008158742341557920912942236668641272, 0.002702198630829333054342855646723364771, 0.002623920164346447113368210324324536487, 0.001636075652182752320287772995754949079,-0.000195344854959845939772200562600801277,-0.002497628617405077584845418670056460542,-0.0044142711399555445428433131382917054  ,-0.004361070826930018261291532866152920178, 0.000282824632567476956773039331594077339, 0.013501199867095557000107319822745921556,-0.007204099620894590348674224600244997418 };

	int sizeArrCoef = std::size(highCoef2);
	if (buffer == NULL)
		buffer = (float*)malloc(sizeBuffer * sizeof(float) * inchannels); // allocating memory

	// Follwing code from laboratory 6
	for (unsigned int sample = 0; sample < length; sample++) {
		for (int chan = 0; chan < *outchannels; chan++) {
			buffer[(sampleCount * inchannels + chan) % sizeBuffer] = inbuffer[sample * inchannels + chan];

			if (sampleCount < sizeArrCoef)
				outbuffer[sample * inchannels + chan] = 0.0f;
			else
				for (int x = 0; x < sizeArrCoef; x++) 					
					outbuffer[sample * inchannels + chan] += highCoef2[x] * buffer[((sampleCount - x) * inchannels + chan) % sizeBuffer];
		}
		sampleCount++;
	}
	return FMOD_OK;
}

// DSP callback - lowpass filter
FMOD_RESULT F_CALLBACK DSPCallback3(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels)
{
	// Creating circular buffer
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;
	int sampleCount = 0;
	static float *buffer = NULL;
	const int sizeBuffer = 1024; // samples in a channel

	// lowpass coefficients from https://www.keil.com/pack/doc/CMSIS/DSP/html/arm_fir_example_f32_8c-example.html
	float lowCoef[] = { -0.0018225230f, -0.0015879294f, 0.0000000000f, 0.0036977508f, 0.0080754303f, 0.0085302217f, -0.0000000000f, -0.0173976984f,
	-0.0341458607f, -0.0333591565f, 0.0000000000f, 0.0676308395f, 0.1522061835f, 0.2229246956f, 0.2504960933f, 0.2229246956f,
	0.1522061835f, 0.0676308395f, 0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, 0.0085302217f,
	0.0080754303f, 0.0036977508f, 0.0000000000f, -0.0015879294f, -0.0018225230f };

	// Using coefficients from matlab filter designer
	float lowCoef2[] = { 0.000163838510522119207160965381753214842, 0.000195534011082332156690813396160422144, 0.000300076790932915602339720040347970098, 0.000428996408171279067396419737434598574, 0.000580292862211500377542239270667323581, 0.000748920836113655449123305274383710639, 0.00092672743770837011107605052373514809 , 0.001102248609817547920705638198057840782, 0.001260320797573686018439031997218080505, 0.001382999395050821256347717280732467771, 0.001449254759375197352394559757726710814, 0.001436517164927079157413225551920277212, 0.001321284298366463740709297880471240205, 0.0010809631513932090561558307939549195  , 0.000695212237471302127791994429628630314, 0.000147873490674615439260247695685279723,-0.000571154691442126143802182625108798675,-0.001464112472831976749509941271298885113,-0.002523587238343269045087735591437194671,-0.00373130216629024083485610674415511312 ,-0.005057011882496552541976786443456148845,-0.006458189539105950449648929634349769913,-0.007880082638649626297633332683290063869,-0.009256656546275072311447118522664823104,-0.010512184373652283531086482071259524673,-0.011563571236401915323654954192988952855,-0.012323377744929133945084309686990309274,-0.012703294426110683587594074595017445972,-0.012618212011272233324943314869415189605,-0.011990410018686451648628477073543763254,-0.01075397216856461071743389368293719599 ,-0.008858885614756176890560723791168129537,-0.00627484052966957063512953851613929146 ,-0.00299435341182472574209083582275070512 , 0.000964896977120100461135232539078288028, 0.005558694009882814493495040153447916964, 0.010716096725427469438240990484700887464, 0.016340386124894988595368516826056293212, 0.022311335094458269179273912641292554326, 0.028488561118177040121235421565870638005, 0.034716022562040371735037069811369292438, 0.040827356931240357806878193969168933108, 0.04665192283874235179563783049161429517 , 0.052021256824910849791976374945079442114, 0.056775612393150362566807132225221721455, 0.060770370464888104344858277272578561679, 0.063881884206101219314533068427408579737, 0.06601262275882446850516771519323810935 , 0.067095195723927192332780577999074012041, 0.067095195723927192332780577999074012041, 0.06601262275882446850516771519323810935 , 0.063881884206101219314533068427408579737, 0.060770370464888104344858277272578561679, 0.056775612393150362566807132225221721455, 0.052021256824910849791976374945079442114, 0.04665192283874235179563783049161429517 , 0.040827356931240357806878193969168933108, 0.034716022562040371735037069811369292438, 0.028488561118177040121235421565870638005, 0.022311335094458269179273912641292554326, 0.016340386124894988595368516826056293212, 0.010716096725427469438240990484700887464, 0.005558694009882814493495040153447916964, 0.000964896977120100461135232539078288028,-0.00299435341182472574209083582275070512 ,-0.00627484052966957063512953851613929146 ,-0.008858885614756176890560723791168129537,-0.01075397216856461071743389368293719599 ,-0.011990410018686451648628477073543763254,-0.012618212011272233324943314869415189605,-0.012703294426110683587594074595017445972,-0.012323377744929133945084309686990309274,-0.011563571236401915323654954192988952855,-0.010512184373652283531086482071259524673,-0.009256656546275072311447118522664823104,-0.007880082638649626297633332683290063869,-0.006458189539105950449648929634349769913,-0.005057011882496552541976786443456148845,-0.00373130216629024083485610674415511312 ,-0.002523587238343269045087735591437194671,-0.001464112472831976749509941271298885113,-0.000571154691442126143802182625108798675, 0.000147873490674615439260247695685279723, 0.000695212237471302127791994429628630314, 0.0010809631513932090561558307939549195  , 0.001321284298366463740709297880471240205, 0.001436517164927079157413225551920277212, 0.001449254759375197352394559757726710814, 0.001382999395050821256347717280732467771, 0.001260320797573686018439031997218080505, 0.001102248609817547920705638198057840782, 0.00092672743770837011107605052373514809 , 0.000748920836113655449123305274383710639, 0.000580292862211500377542239270667323581, 0.000428996408171279067396419737434598574, 0.000300076790932915602339720040347970098, 0.000195534011082332156690813396160422144, 0.000163838510522119207160965381753214842 };

	const int sizeArrCoef = std::size(lowCoef2);
	if (buffer == NULL)
		buffer = (float*)malloc(sizeBuffer * sizeof(float) * inchannels); // allocating memory

	// Following code from laboratory 6 - Averaging
	for (unsigned int sample = 0; sample < length; sample++) {
		for (int chan = 0; chan < *outchannels; chan++) {

			// Storing data in the circular buffer using input buffer
			buffer[(sampleCount * inchannels + chan) % sizeBuffer] = inbuffer[sample * inchannels + chan]; // Problem with writable size

			if (sampleCount < sizeArrCoef)
				outbuffer[sample * inchannels + chan] = 0.0f;
			else
				for (int x = 0; x < sizeArrCoef; x++)
					// Using convolution and coefficients to create filtered signals
					outbuffer[sample * inchannels + chan] += lowCoef2[x] * ((buffer[((sampleCount - (x - 1)) * inchannels + chan) % sizeBuffer] 
															+ buffer[((sampleCount - (x - 2)) * inchannels + chan) % sizeBuffer]
															+ buffer[((sampleCount - (x - 3)) * inchannels + chan) % sizeBuffer]
															+ buffer[((sampleCount - (x - 4)) * inchannels + chan) % sizeBuffer]) / 4);
		}
		sampleCount++;
	}
	return FMOD_OK;
}

CAudio::CAudio(){}

CAudio::~CAudio(){}

bool CAudio::Initialise()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// Initialise the system
	//result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	result = m_FmodSystem->init(32, FMOD_INIT_3D_RIGHTHANDED, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// Set 3D settings
	result = m_FmodSystem->set3DSettings(3.0f, 3.0f, 3.0f);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// Create the DSP effect - apply to event sound or music
	{
		FMOD_DSP_DESCRIPTION dspdesc;
		memset(&dspdesc, 0, sizeof(dspdesc));

		strncpy_s(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
		dspdesc.numinputbuffers = 1;
		dspdesc.numoutputbuffers = 1;
		dspdesc.read = DSPCallback;

		result = m_FmodSystem->createDSP(&dspdesc, &m_dsp);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	// Create the DSP effect 2 - apply to event sound or music
	{
		FMOD_DSP_DESCRIPTION dspdesc2;
		memset(&dspdesc2, 0, sizeof(dspdesc2));

		strncpy_s(dspdesc2.name, "My second DSP unit", sizeof(dspdesc2.name));
		dspdesc2.numinputbuffers = 1;
		dspdesc2.numoutputbuffers = 1;
		dspdesc2.read = DSPCallback2;

		result = m_FmodSystem->createDSP(&dspdesc2, &m_dsp2);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	// Create the DSP effect 3 - apply to event sound or music
	{
		FMOD_DSP_DESCRIPTION dspdesc3;
		memset(&dspdesc3, 0, sizeof(dspdesc3));

		strncpy_s(dspdesc3.name, "My third DSP unit", sizeof(dspdesc3.name));
		dspdesc3.numinputbuffers = 1;
		dspdesc3.numoutputbuffers = 1;
		dspdesc3.read = DSPCallback3;

		result = m_FmodSystem->createDSP(&dspdesc3, &m_dsp3);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	return true;

}

// Load an event sound
bool CAudio::LoadEventSound(const char *filename, bool loop)
{
	if (loop == true)
		result = m_FmodSystem->createSound(filename, FMOD_LOOP_NORMAL, 0, &m_eventSound);
	else
		result = m_FmodSystem->createSound(filename, FMOD_LOOP_OFF, 0, &m_eventSound);

	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;

}

// Play an event sound
bool CAudio::PlayEventSound(glm::vec3 p)
{
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, &m_eventChannel);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// play through 3D channel
	m_eventChannel->setMode(FMOD_3D);
	m_eventChannel->addDSP(0, m_dsp);
	m_eventChannel->set3DDopplerLevel(500.f);
	m_eventChannel->set3DOcclusion(0.9, 1.0);
	// set the updated position to be the car position
	result = m_eventChannel->set3DAttributes(&carPos, 0, 0);
	m_eventChannel->getFrequency(&m_frequency);

	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	return true;
}

// Stopping car engine sound
void CAudio::StopEventSound() { m_eventChannel->stop(); }

// Toggle filters for the engine sound
void CAudio::ToogleSoundEventFilter() {

	RemoveFilters();
	if (m_eventSoundFilterActive == false) {
		m_eventChannel->addDSP(0, m_dsp2);
		m_eventChannel->removeDSP(m_dsp3);
		m_eventSoundFilterActive = true;
	}
	else {
		m_eventChannel->removeDSP(m_dsp2);
		m_eventChannel->addDSP(0, m_dsp3);
		m_eventSoundFilterActive = false;
	}

}

// Disabling filter effects for engine sound
void CAudio::RemoveFilters() { m_eventChannel->removeDSP(m_dsp2); m_eventChannel->removeDSP(m_dsp3); }

// Load a music stream
bool CAudio::LoadMusicStream(const char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	// create a low-pass filter DSP object
	result = m_FmodSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_musicFilter);

	if (result != FMOD_OK)
		return false;

	// you can start the DSP in an inactive state
	m_musicFilter->setActive(false);

	return true;


}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	//m_musicChannel->addDSP(0, m_dsp);

	// Set the volume lower
	result = m_musicChannel->setVolume(m_musicVolume);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// connecting the music filter to the music stream
	// 1) Get the DSP head and it's input
	m_musicChannel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &m_musicDSPHead);
	m_musicDSPHead->getInput(0, &m_musicDSPHeadInput, NULL);
	// 2) Disconnect them
	m_musicDSPHead->disconnectFrom(m_musicDSPHeadInput);
	// 3) Add input to the music head from the filter
	result = m_musicDSPHead->addInput(m_musicFilter);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	// 4) Add input to the filter head music DSP head input
	result = m_musicFilter->addInput(m_musicDSPHeadInput);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	// set the DSP object to be active
	m_musicFilter->setActive(true);
	// initially set the cutoff to a high value
	m_musicFilter->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 22000);
	// this state is used for toggling
	m_musicFilterActive = false;

	return true;
}

// Changing lowpass cutoff frequencies for music
void CAudio::ToggleMusicFilter()
{
	// called externally from Game::ProcessEvents
	// toggle the effect on/off
	m_musicFilterActive = !m_musicFilterActive;
	if (m_musicFilterActive) {
		// set the parameter to a low value
		m_musicFilter->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 700);
	}
	else {
		// set the parameter to a high value
		// you could also use m_musicFilter->setBypass(true) instead...
		m_musicFilter->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 22000);
	}
}

// Increase music volume
void CAudio::IncreaseMusicVolume()
{
	// called externally from Game::ProcessEvents
	// increment the volume
	m_musicVolume += .5f;
	if (m_musicVolume > 10)
		m_musicVolume = 10.0f;
	m_musicChannel->setVolume(m_musicVolume);
}

// Decrease music volume
void CAudio::DecreaseMusicVolume()
{
	// called externally from Game::ProcessEvents
	// deccrement the volume
	m_musicVolume -= 0.5f;
	if (m_musicVolume < 0)
		m_musicVolume = 0.0f;
	m_musicChannel->setVolume(m_musicVolume);
}

// Updating positions, speed, and FMOD system
void CAudio::Update(CCamera *m_pCamera, glm::vec3 p, float speed)
{

	// Updating car sound position
	ToFMODVector(p, &carPos);
	m_eventChannel->set3DAttributes(&carPos, 0, 0);

	// Applying engine filter - checking on every frame
	EngineFilter(speed);

	// 5) update the listener's position with the camera position
	glm::vec3 cam = m_pCamera->GetPosition();
	glm::vec3 up = m_pCamera->GetUpVector();
	glm::vec3 forward = glm::cross(m_pCamera->GetStrafeVector(), m_pCamera->GetUpVector());

	ToFMODVector(cam, &camPos);
	ToFMODVector(up, &camUP);
	ToFMODVector(forward, &camForw);

	result = m_FmodSystem->set3DListenerAttributes(0, &camPos, NULL, &camForw, &camUP);
	FmodErrorCheck(result);

	m_Carspeed = speed * 100000; // speed values modified to fit certain frequency ranges
	m_eventChannel->setFrequency(m_frequency + m_Carspeed); // changing engine sound frequency according to Carspeed
	
	m_FmodSystem->update();

}

// Check for error
void CAudio::FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

// Converter
void CAudio::ToFMODVector(glm::vec3 &glVec3, FMOD_VECTOR *fmodVec)
{
	fmodVec->x = glVec3.x;
	fmodVec->y = glVec3.y;
	fmodVec->z = glVec3.z;
}

// Applying filters based on car speed
void CAudio::EngineFilter(float speed){
	if (speed * 1000 >= 100){ // car speed multipled by 1000 to match the speed in the game information panel
		m_eventChannel->removeDSP(m_dsp3);
		m_eventChannel->addDSP(0, m_dsp2);
	}
	else if (speed * 1000 <= 30 ){
		m_eventChannel->removeDSP(m_dsp2);
		m_eventChannel->addDSP(0, m_dsp3);
	}
	else{
		m_eventChannel->removeDSP(m_dsp2);
		m_eventChannel->removeDSP(m_dsp3);
	}
}