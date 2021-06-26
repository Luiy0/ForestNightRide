#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"

class CTorus
{
private:
	GLuint m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;

	int numVertices;
	int numIndices;

	std::vector<glm::uint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> sTangents;
	std::vector<glm::vec3> tTangents;

	float toRadians(float degrees);

public:
	CTorus();
	~CTorus();
	void Create(string sDirectory, string sFilename, float inner, float outer, int prec);
	void Render();
	void Release();
};