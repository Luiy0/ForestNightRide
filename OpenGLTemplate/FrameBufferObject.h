#pragma once

#include "Texture.h"
#include "Shaders.h"

class CFrameBufferObject
{
public:
	CFrameBufferObject();
	~CFrameBufferObject();

	// Create a framebuffer object with a texture of a given size
	bool Create(int a_iWidth, int a_iHeight);

	// Bind the FBO for rendering to texture
	void Bind(bool bSetFullViewport = true);

	// Bind the texture (usually on a 2nd or later pass in a multi-pass rendering technique)
	void BindTexture(int iTextureUnit);

	// Bind the depth (usually on a 2nd or later pass in a multi-pass rendering technique)
	void BindDepth(int iTextureUnit);

	// Delete the framebuffer
	void Release();

	// Set methods for the sampler object
	void SetSamplerObjectParameter(GLenum parameter, GLenum value);
	void SetSamplerObjectParameterf(GLenum parameter, float value);


	// Get the width, height
	int GetWidth();
	int GetHeight();

private:


	int m_iWidth, m_iHeight;
	UINT m_uiFramebuffer;
	UINT m_uiColourTexture;
	UINT m_uiDepthTexture;
	UINT m_uiSampler;


};