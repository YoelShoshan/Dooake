#ifndef CRIMSON_RENDERER_STATE_H
#define CRIMSON_RENDERER_STATE_H

#pragma once

#include "ce.h"

#include "Defines.h"
/*m_RendererState.m_dwDepthFunc = CR_LESS_OR_EQUAL;
	
	glEnable (GL_DEPTH_TEST);	
	m_RendererState.m_bDepthTest=true;


	glEnable(GL_CULL_FACE);
	m_RendererState.m_bCull=true;


	glCullFace(GL_BACK);
	m_RendererState.m_dwCullFunc = CR_BACK;*/

struct tRendererState
{
	tRendererState():m_bDepthTest(true),m_bDepthWrite(true),
		m_dwDepthFunc(CR_LESS_OR_EQUAL),
		m_cBlendSource(CR_BF_ONE),
		m_cBlendTarget(CR_BF_ZERO),
		m_bCull(true),
		bScissorRect(false),
		m_dwCullFunc(CR_BACK),
		m_dwAlphaFunc(CR_ALWAYS),
		m_dwAlphaRefValue(0){};

	bool IsOpaque()
	{
		if (m_cBlendSource != CR_BF_ONE)
			return false;

		if (m_cBlendTarget != CR_BF_ZERO)
			return false;

		return true;
	}

	bool  m_bDepthTest;
	DWORD m_dwDepthFunc;
	bool m_bDepthWrite;

	bool  m_bCull; // only used by opengl
	DWORD m_dwCullFunc;

	DWORD m_dwAlphaFunc;
	DWORD m_dwAlphaRefValue;

	char m_cBlendSource;
	char m_cBlendTarget;

	bool bScissorRect;
};


#endif