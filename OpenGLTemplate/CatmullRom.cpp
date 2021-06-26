#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom() {}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
		
	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);
	glm::vec3 x = a + b * t + c * t2 + d * t3;

	return x;

}

void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	// Curved Path

	// Identify mountains and smoothly elevate the road until end of mountain where it should connect with the road again
	m_controlPoints.push_back(glm::vec3(-700.0f, -19.8, -200.0f));
	m_controlPoints.push_back(glm::vec3(-650.0f, -15.2, -500.0f));
	m_controlPoints.push_back(glm::vec3(-400.28f, -15.1, -500.98f));
	m_controlPoints.push_back(glm::vec3(-200.0f, -18.6, -780.0f));
	m_controlPoints.push_back(glm::vec3(0.0f, -15.0, -850.0f));
	m_controlPoints.push_back(glm::vec3(320.0f, -51.7, -850.0f));
	m_controlPoints.push_back(glm::vec3(400.0f, -20.4, -800.0f));
	m_controlPoints.push_back(glm::vec3(400.0f, -10.4, -600.0f));
	m_controlPoints.push_back(glm::vec3(400.0f, -12.0, -400.0f));
	m_controlPoints.push_back(glm::vec3(500.0f, -17.3, -300.0f));
	m_controlPoints.push_back(glm::vec3(700.0f, -30.7, -300.0f));
	m_controlPoints.push_back(glm::vec3(800.0f, -39.1, -200.0f));
	m_controlPoints.push_back(glm::vec3(820.0f, -28.5, -120.0f));
	m_controlPoints.push_back(glm::vec3(700.0f, -26.8, -100.0f));
	m_controlPoints.push_back(glm::vec3(400.0f, 32.6, -100.0f));
	m_controlPoints.push_back(glm::vec3(200.0f, 72.4, -100.0f));
	m_controlPoints.push_back(glm::vec3(100.0f, 36.0, -200.0f));
	m_controlPoints.push_back(glm::vec3(100.0f, 3.9, -400.0f)); 
	m_controlPoints.push_back(glm::vec3(-40.0f, -17.0, -400.73f));
	m_controlPoints.push_back(glm::vec3(-300.56f, 23.5, -400.83f));
	m_controlPoints.push_back(glm::vec3(-300.0f, -2.7, -100.0f));
	m_controlPoints.push_back(glm::vec3(-200.0f, 28.8, 100.0f));
	m_controlPoints.push_back(glm::vec3(0.0f, 79.4, 100.0f));
	m_controlPoints.push_back(glm::vec3(100.0f, 33.5, 200.0f));
	m_controlPoints.push_back(glm::vec3(0.0f, 24.6, 300.0f));
	m_controlPoints.push_back(glm::vec3(-300.0f, -5.7, 300.0f));
	m_controlPoints.push_back(glm::vec3(-500.0f, -21.2, 300.0f));
	m_controlPoints.push_back(glm::vec3(-600.0f, -8.9, 200.0f));
	m_controlPoints.push_back(glm::vec3(-700.0f, -22.1, 100.0f));


}

// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}

// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size() - 1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}

// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}

void CCatmullRom::CreateCentreline()
{

	SetControlPoints(); // control points
	UniformlySampleControlPoints(500); // sample 500 points along the centreline

	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) {
		vbo.AddData(&m_centrelinePoints.at(i), sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}	

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


}

void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	for (int i = 0; i < m_centrelinePoints.size(); ++i){
		if (i == m_centrelinePoints.size() - 1){
			glm::vec3 T = glm::normalize(m_centrelinePoints[0] - m_centrelinePoints[i]);
			glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 B = glm::normalize(glm::cross(N, T));
			m_leftOffsetPoints.push_back(m_centrelinePoints[i] - (m_path_width / 2) * N);
			m_rightOffsetPoints.push_back(m_centrelinePoints[i] + (m_path_width / 2) * N);
		}
		else{
			glm::vec3 T = glm::normalize(m_centrelinePoints[i + 1] - m_centrelinePoints[i]);
			glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 B = glm::normalize(glm::cross(N, T));
			m_leftOffsetPoints.push_back(m_centrelinePoints[i] - (m_path_width / 2) * N);
			m_rightOffsetPoints.push_back(m_centrelinePoints[i] + (m_path_width / 2) * N);
		}
	}

	// VAO-VBO LEFT OFFSET CURVE
	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	// Create a VBO
	CVertexBufferObject vbo_left;
	vbo_left.Create();
	vbo_left.Bind();
	glm::vec2 texCoord_left(0.0f, 0.0f);
	glm::vec3 normal_left(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_leftOffsetPoints.size(); i++) {
		vbo_left.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
		vbo_left.AddData(&texCoord_left, sizeof(glm::vec2));
		vbo_left.AddData(&normal_left, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo_left.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride_left = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_left, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride_left, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride_left, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	// VAO & VBO RIGHT OFFSET CURVE
	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	// Create a VBO
	CVertexBufferObject vbo_right;
	vbo_right.Create();
	vbo_right.Bind();
	glm::vec2 texCoord_right(0.0f, 0.0f);
	glm::vec3 normal_right(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_rightOffsetPoints.size(); i++) {
		vbo_right.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		vbo_right.AddData(&texCoord_right, sizeof(glm::vec2));
		vbo_right.AddData(&normal_right, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo_right.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride_right = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_right, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride_right, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride_right, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


}

void CCatmullRom::CreateTrack()
{

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

	// Use VAO to store state associated with vertices

	m_texture.Load("resources\\textures\\track3.jpg"); // from http://texturelib.com/texture/?path=/Textures/wood/burnt/wood_burnt_0004 05/05/2021
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	// Swap tex1 & tex4 to change from horizontal to vertical or viceversa
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	glm::vec2 tex1(1.0f, 1.0f);
	glm::vec2 tex2(1.0f, 0.0f);
	glm::vec2 tex3(0.0f, 1.0f);
	glm::vec2 tex4(0.0f, 0.0f);

	for (int i = 0; i < m_centrelinePoints.size(); ++i)
	{
		if (i == m_centrelinePoints.size() - 1) // for the first point - loop
		{
			// Adding triangle data to VBO 
			vbo.AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle
			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle
			vbo.AddData(&m_centrelinePoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			//Triangle
			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

		}
		else
		{
			// Adding data to VBO (four triangles everytime we move on the path)
			vbo.AddData(&m_leftOffsetPoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			// Triangle
			vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			// Triangle
			vbo.AddData(&m_centrelinePoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex1, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			// Triangle
			vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex2, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
			vbo.AddData(&tex4, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;

			vbo.AddData(&m_rightOffsetPoints[i + 1], sizeof(glm::vec3));
			vbo.AddData(&tex3, sizeof(glm::vec2));
			vbo.AddData(&normal, sizeof(glm::vec3));
			m_vertexCount += 1;
		}
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride_right = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_right, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride_right, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride_right, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it

	glLineWidth(3.0f);
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());

	

}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffsetPoints.size());
}

void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	glBindVertexArray(m_vaoTrack);	
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	

}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);

