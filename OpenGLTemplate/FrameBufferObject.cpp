#include "Common.h"
#include "FrameBufferObject.h"
#include "./include/glm/gtc/matrix_transform.hpp"

CFrameBufferObject::CFrameBufferObject()
{
	m_uiFramebuffer = 0;
	m_uiDepthTexture = 0;
	m_uiColourTexture = 0;
}

CFrameBufferObject::~CFrameBufferObject()
{
	Release();
}

// Create an FBO with a texture attachment:  note this code comes mostly from the Superbible, 6th edition, page 367
bool CFrameBufferObject::Create(int a_iWidth, int a_iHeight)
{

	if (m_uiFramebuffer != 0) return false;

	// Create a framebuffer object and bind it
	glGenFramebuffers(1, &m_uiFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFramebuffer);

	// Create a texture for our colour buffer
	glGenTextures(1, &m_uiColourTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiColourTexture);
	// glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, a_iWidth, a_iHeight); // The Superbible suggests this, but it is OpenGL4.2 feature
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, a_iWidth, a_iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Create a sampler object and set texture properties.  Note here, we're mipmapping
	glGenSamplers(1, &m_uiSampler);
	SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Now, create a depth texture for the FBO
	glGenTextures(1, &m_uiDepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiDepthTexture);
	// glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, a_iWidth, a_iHeight);  // The Superbible suggests this, but it is OpenGL4.2 feature
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, a_iWidth, a_iHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	// Now attach the colour and depth textures to the FBO
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uiColourTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_uiDepthTexture, 0);

	// Tell OpenGL that we want to draw into the frambuffer's colour attachment
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	m_iWidth = a_iWidth;
	m_iHeight = a_iHeight;

	// Check completeness
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

// Bind the FBO so we can render to it
void CFrameBufferObject::Bind(bool bSetFullViewport)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFramebuffer);
	if (bSetFullViewport)glViewport(0, 0, m_iWidth, m_iHeight);

	glm::vec4 clearColour = glm::vec4(0.5, 0.5, 0.5, 1.0);
	float one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, &clearColour.r);
	glClearBufferfv(GL_DEPTH, 0, &one);

}

// Beind the frambuffer texture so it is active
void CFrameBufferObject::BindTexture(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_uiColourTexture);
	glBindSampler(iTextureUnit, m_uiSampler);

	glGenerateMipmap(GL_TEXTURE_2D);
}


// Beind the frambuffer texture so it is active
void CFrameBufferObject::BindDepth(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_uiDepthTexture);
	glBindSampler(iTextureUnit, m_uiSampler);

}

// Delete the framebuffer and all attachments
void CFrameBufferObject::Release()
{
	if (m_uiFramebuffer)
	{
		glDeleteFramebuffers(1, &m_uiFramebuffer);
		m_uiFramebuffer = 0;
	}

	glDeleteSamplers(1, &m_uiSampler);
	glDeleteTextures(1, &m_uiColourTexture);
	glDeleteTextures(1, &m_uiDepthTexture);
}

int CFrameBufferObject::GetWidth()
{
	return m_iWidth;
}

int CFrameBufferObject::GetHeight()
{
	return m_iHeight;
}

void CFrameBufferObject::SetSamplerObjectParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(m_uiSampler, parameter, value);
}

void CFrameBufferObject::SetSamplerObjectParameterf(GLenum parameter, float value)
{
	glSamplerParameterf(m_uiSampler, parameter, value);
}