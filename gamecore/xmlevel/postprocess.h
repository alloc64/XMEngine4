/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _POSTPROCESS_H
#define _POSTPROCESS_H

class PostProcess {
public:
    bool CreateFBO(void);

    void RenderToFBO(void);

    void RenderLevel(void);

    bool CreateFinalShader(void);


    GLuint depthMapID;
    GLuint colorMapID;
    GLuint positionMapID;
    GLuint normalMapID;
    GLuint noiseMapID;
    GLuint FBOid;

    GLuint iUniformCameraPosition;
    GLuint iUniformPositionMap;
    GLuint iUniformDiffuseMap;
    GLuint iUniformNormalMap;
    GLuint iUniformNoiseMap;
    GLhandleARB deferredPH;

    GLhandleARB finalPH;
    GLuint iUniformColorMap;
    GLuint iUniformDepthMap;
    GLuint iUniformAOMap;
    GLuint iUniformTime;
    GLuint aoMapID;
    GLuint finalFBOid;

private:
};


inline bool LoadFBOFunctions(void) {
    if (!CheckExtension("EXT_framebuffer_object")) return false;

    glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) wglGetProcAddress("glIsFramebufferEXT");
    glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebufferEXT");
    glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
    glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFramebuffersEXT");
    glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
    glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
    glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC) wglGetProcAddress("glGenerateMipmapEXT");
    glDrawBuffers = (PFNGLDRAWBUFFERSPROC) wglGetProcAddress("glDrawBuffers");
    glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress("glGenRenderbuffersEXT");
    glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
    glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress("glBindRenderbufferEXT");
    glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) wglGetProcAddress("glRenderbufferStorageEXT");
    glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) wglGetProcAddress(
            "glFramebufferRenderbufferEXT");

    if (!glIsFramebufferEXT || !glBindFramebufferEXT || !glDeleteFramebuffersEXT ||
        !glGenFramebuffersEXT || !glCheckFramebufferStatusEXT ||
        !glFramebufferTexture2DEXT || !glGenerateMipmapEXT || !glDrawBuffers ||
        !glGenRenderbuffersEXT || !glBindRenderbufferEXT || !glRenderbufferStorageEXT) {
        printf("error: cannot load EXT_framebuffer_object functions!\n");
        return false;
    }

    return true;
}

inline bool LoadVBOFunctions() {
    glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
    glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
    glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
    glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
    glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
    glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC) wglGetProcAddress("glBufferSubDataARB");
    glMapBufferARB = (PFNGLMAPBUFFERARBPROC) wglGetProcAddress("glMapBufferARB");
    glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress("glUnmapBufferARB");

    if (!glGenBuffersARB || !glBindBufferARB || !glBufferDataARB || !glDeleteBuffersARB ||
        !glBufferDataARB || !glBufferSubDataARB || !glMapBufferARB || !glUnmapBufferARB) {
        printf("error: cannot load GL_ARB_vertex_buffer_object functions, its awesome ! :(\n");
        return false;
    }
    return true;
}


#endif
