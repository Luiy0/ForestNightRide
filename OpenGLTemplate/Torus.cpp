#include "Common.h"
#include "Torus.h"

CTorus::CTorus() {}
CTorus::~CTorus() {}

// Create the plane, including its geometry, texture mapping, normal, and colour
void CTorus::Create(string directory, string filename, float inner, float outer, int prec)
{
	// Load the texture
	m_texture.Load(directory + filename, true);

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a VBO
	m_vbo.Create();
	m_vbo.Bind();
		
	numVertices = (prec + 1) * (prec + 1);
	numIndices = prec * prec * 6;
	for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
	for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { sTangents.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { tTangents.push_back(glm::vec3()); }
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

	// calculate first ring
	for (int i = 0; i < prec + 1; i++) {
		float amt = toRadians(i * 360.0f / prec);

		glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 initPos(rMat * glm::vec4(outer, 0.0f, 0.0f, 1.0f));

		vertices[i] = glm::vec3(initPos + glm::vec3(inner, 0.0f, 0.0f));
		texCoords[i] = glm::vec2(0.0f, ((float)i / (float)prec));

		rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
		tTangents[i] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));

		sTangents[i] = glm::vec3(glm::vec3(0.0f, 0.0f, -1.0f));
		normals[i] = glm::cross(tTangents[i], sTangents[i]);
	}
	// rotate the first ring about Y to get the other rings
	for (int ring = 1; ring < prec + 1; ring++) {
		for (int i = 0; i < prec + 1; i++) {
			float amt = (float)toRadians((float)ring * 360.0f / (prec));

			glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			vertices[ring * (prec + 1) + i] = glm::vec3(rMat * glm::vec4(vertices[i], 1.0f));

			texCoords[ring * (prec + 1) + i] = glm::vec2((float)ring * 2.0f / (float)prec, texCoords[i].t);
			if (texCoords[ring * (prec + 1) + i].s > 1.0) texCoords[ring * (prec + 1) + i].s -= 1.0f;

			rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			sTangents[ring * (prec + 1) + i] = glm::vec3(rMat * glm::vec4(sTangents[i], 1.0f));

			rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			tTangents[ring * (prec + 1) + i] = glm::vec3(rMat * glm::vec4(tTangents[i], 1.0f));

			rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			normals[ring * (prec + 1) + i] = glm::vec3(rMat * glm::vec4(normals[i], 1.0f));
		}
	}
	// calculate triangle indices
	for (int ring = 0; ring < prec; ring++) {
		for (int i = 0; i < prec; i++) {
			indices[((ring * prec + i) * 2) * 3 + 0] = ring * (prec + 1) + i;
			indices[((ring * prec + i) * 2) * 3 + 1] = (ring + 1) * (prec + 1) + i;
			indices[((ring * prec + i) * 2) * 3 + 2] = ring * (prec + 1) + i + 1;
			indices[((ring * prec + i) * 2 + 1) * 3 + 0] = ring * (prec + 1) + i + 1;
			indices[((ring * prec + i) * 2 + 1) * 3 + 1] = (ring + 1) * (prec + 1) + i;
			indices[((ring * prec + i) * 2 + 1) * 3 + 2] = (ring + 1) * (prec + 1) + i + 1;
		}
	}

	for (int i = 0; i < numIndices; i++) { m_vbo.AddIndexData(&indices[i], sizeof(glm::uint)); }
	for (int i = 0; i < numVertices; i++) {
		m_vbo.AddVertexData(&vertices[i], sizeof(glm::vec3));
		m_vbo.AddVertexData(&texCoords[i], sizeof(glm::vec2));
		m_vbo.AddVertexData(&normals[i], sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations (interleaved)
	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

// Render the plane as a triangle strip
void CTorus::Render()
{
	glDisable(GL_CULL_FACE);
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glEnable(GL_CULL_FACE);

}

// Release resources
void CTorus::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

float CTorus::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }