glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)	 wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)   wglGetProcAddress("glClientActiveTextureARB");
	glLockArraysEXT		= (PFNGLLOCKARRAYSEXTPROC)		wglGetProcAddress("glLockArraysEXT");
	glUnlockArraysEXT	= (PFNGLUNLOCKARRAYSEXTPROC)	wglGetProcAddress("glUnlockArraysEXT");

glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress("glMultiTexCoord2fv");
 glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVARBPROC)wglGetProcAddress("glMultiTexCoord3fv");
glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVARBPROC)wglGetProcAddress("glMultiTexCoord4fv");
	
	// WGL_ARB_pbuffer
	wglCreatePbufferARB=(PFNWGLCREATEPBUFFERARBPROC) wglGetProcAddress("wglCreatePbufferARB");
	wglGetPbufferDCARB=(PFNWGLGETPBUFFERDCARBPROC) wglGetProcAddress("wglGetPbufferDCARB");
	wglReleasePbufferDCARB=(PFNWGLRELEASEPBUFFERDCARBPROC) wglGetProcAddress("wglReleasePbufferDCARB");
	wglDestroyPbufferARB=(PFNWGLDESTROYPBUFFERARBPROC) wglGetProcAddress("wglDestroyPbufferARB");
	wglQueryPbufferARB=(PFNWGLQUERYPBUFFERARBPROC) wglGetProcAddress("wglQueryPbufferARB");

	// WGL_ARB_pixel_format
	wglGetPixelFormatAttribivARB=(PFNWGLGETPIXELFORMATATTRIBIVARBPROC) wglGetProcAddress("wglGetPixelFormatAttribivARB");
	wglGetPixelFormatAttribfvARB=(PFNWGLGETPIXELFORMATATTRIBFVARBPROC) wglGetProcAddress("wglGetPixelFormatAttribfvARB");
	wglChoosePixelFormatARB=(PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");

	wglReleaseTexImageARB=(PFNWGLRELEASETEXIMAGEARBPROC) wglGetProcAddress("wglReleaseTexImageARB");
	wglBindTexImageARB=(PFNWGLBINDTEXIMAGEARBPROC) wglGetProcAddress("wglBindTexImageARB");

	// GL_NV_occlusion_query
	glGenOcclusionQueriesNV=(PFNGLGENOCCLUSIONQUERIESNVPROC)wglGetProcAddress("glGenOcclusionQueriesNV");
	glDeleteOcclusionQueriesNV=(PFNGLDELETEOCCLUSIONQUERIESNVPROC)wglGetProcAddress("glDeleteOcclusionQueriesNV");
	glIsOcclusionQueryNV=(PFNGLISOCCLUSIONQUERYNVPROC)wglGetProcAddress("glIsOcclusionQueryNV");
	glBeginOcclusionQueryNV=(PFNGLBEGINOCCLUSIONQUERYNVPROC)wglGetProcAddress("glBeginOcclusionQueryNV");
	glEndOcclusionQueryNV=(PFNGLENDOCCLUSIONQUERYNVPROC)wglGetProcAddress("glEndOcclusionQueryNV");
	glGetOcclusionQueryivNV=(PFNGLGETOCCLUSIONQUERYIVNVPROC)wglGetProcAddress("glGetOcclusionQueryivNV");
	glGetOcclusionQueryuivNV=(PFNGLGETOCCLUSIONQUERYUIVNVPROC)wglGetProcAddress("glGetOcclusionQueryuivNV");

	// GL_ARB_vertex_buffer_object
	glGenBuffersARB=(PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB=(PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	glBufferDataARB=(PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	glDeleteBuffersARB=(PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
        glBufferSubDataARB=(PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");

	glMapBufferARB=(PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB=(PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	// SHADER OBJECT
	glBindAttribLocationARB=(PFNGLBINDATTRIBLOCATIONARBPROC)wglGetProcAddress("glBindAttribLocationARB");

	glCreateProgramObjectARB=(PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
	glCreateShaderObjectARB=(PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
	glDeleteObjectARB=(PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
	glDetachObjectARB=(PFNGLDETACHOBJECTARBPROC)wglGetProcAddress("glDetachObjectARB");
	glAttachObjectARB=(PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");

	glShaderSourceARB=(PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB=(PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
	glLinkProgramARB=(PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
	glGetInfoLogARB=(PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
	glUseProgramObjectARB=(PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");

	glGetObjectParameterivARB=(PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
	glGetObjectParameterfvARB=(PFNGLGETOBJECTPARAMETERFVARBPROC)wglGetProcAddress("glGetObjectParameterfvARB");
	glGetUniformLocationARB=(PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");

	glUniform1fARB=(PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
	glUniform2fARB=(PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
	glUniform3fARB=(PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
	glUniform4fARB=(PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");

	glUniform1iARB=(PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	glUniform2iARB=(PFNGLUNIFORM2IARBPROC)wglGetProcAddress("glUniform2iARB");
	glUniform3iARB=(PFNGLUNIFORM3IARBPROC)wglGetProcAddress("glUniform3iARB");
	glUniform4iARB=(PFNGLUNIFORM4IARBPROC)wglGetProcAddress("glUniform4iARB");

	glUniform1fvARB=(PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1fvARB");
	glUniform2fvARB=(PFNGLUNIFORM2FVARBPROC)wglGetProcAddress("glUniform2fvARB");
	glUniform3fvARB=(PFNGLUNIFORM3FVARBPROC)wglGetProcAddress("glUniform3fvARB");
	glUniform4fvARB=(PFNGLUNIFORM4FVARBPROC)wglGetProcAddress("glUniform4fvARB");

	glUniform1ivARB=(PFNGLUNIFORM1IVARBPROC)wglGetProcAddress("glUniform1ivARB");
	glUniform2ivARB=(PFNGLUNIFORM2IVARBPROC)wglGetProcAddress("glUniform2ivARB");
	glUniform3ivARB=(PFNGLUNIFORM3IVARBPROC)wglGetProcAddress("glUniform3ivARB");
	glUniform4ivARB=(PFNGLUNIFORM4IVARBPROC)wglGetProcAddress("glUniform4ivARB");

	glUniformMatrix4fvARB=(PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress("glUniformMatrix4fvARB");

	// WGL_EXT_swap_control
	wglSwapIntervalEXT=(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	// GL_ATI_separate_stencil
	glStencilOpSeparateATI=(PFNGLSTENCILOPSEPARATEATIPROC)wglGetProcAddress("glStencilOpSeparateATI");
	glStencilFuncSeparateATI=(PFNGLSTENCILFUNCSEPARATEATIPROC)wglGetProcAddress("glStencilFuncSeparateATI");

	// GL_ARB_occlusion_query

	glGenQueriesARB=(PFNGLGENQUERIESARBPROC)wglGetProcAddress("glGenQueriesARB");
	glDeleteQueriesARB=(PFNGLDELETEQUERIESARBPROC)wglGetProcAddress("glDeleteQueriesARB");
	glIsQueryARB=(PFNGLISQUERYARBPROC)wglGetProcAddress("glIsQueryARB");
	glBeginQueryARB=(PFNGLBEGINQUERYARBPROC)wglGetProcAddress("glBeginQueryARB");
	glEndQueryARB=(PFNGLENDQUERYARBPROC)wglGetProcAddress("glEndQueryARB");
	glGetQueryivARB=(PFNGLGETQUERYIVARBPROC)wglGetProcAddress("glGetQueryivARB");
	glGetQueryObjectivARB=(PFNGLGETQUERYOBJECTIVARBPROC)wglGetProcAddress("glGetQueryObjectivARB");
	glGetQueryObjectuivARB=(PFNGLGETQUERYOBJECTUIVARBPROC)wglGetProcAddress("glGetQueryObjectuivARB");


	// WGL_ARB_make_current_read
	wglMakeContextCurrentARB=(PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");

	// EXT_framebuffer_object
	glIsRenderbufferEXT=(PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
	glBindRenderbufferEXT=(PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
	glDeleteRenderbuffersEXT=(PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT=(PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT=(PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT=(PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT=(PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT=(PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT=(PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT=(PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT=(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT=(PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT=(PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT=(PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT=(PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT=(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipmapEXT=(PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

wglGetExtensionsStringARB=(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");


// dds loading
    glCompressedTexImage2DARB=(PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");