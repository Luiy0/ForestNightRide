#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"
#include "HeightMapTerrain.h"

class COpenAssetImportMesh;
class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	// Centerline interpolates control path to make a smooth curve.
	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();

	void CreateTrack();
	void RenderTrack();

	int CurrentLap(float d); // Return the current lap (starting from 0) based on distance along the control curve.
	bool Sample(float d, glm::vec3 &p, glm::vec3 &up = _dummy_vector); // Return a point on the centreline based on a certain distance along the control curve.

	// Game client access
	vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	vector<glm::vec3> m_rightOffsetPoints;	// Right offset curve points
	vector<glm::vec3> m_centrelinePoints;	// Centreline points

	float fAccumulatedLength = 0.0f;

private:

	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);
	
	vector<float> m_distances;
	CTexture m_texture;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoTrack; 	

	static glm::vec3 _dummy_vector;
	vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlPoints2;
	vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	
	vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors

	unsigned int m_vertexCount;				// Number of vertices in the track VBO
	float m_path_width = 25.0f;

	CHeightMapTerrain terrainHeight;

};
