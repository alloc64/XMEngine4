/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

extern Window window;
extern Shader shader;
extern xmLevel level;
extern Camera camera;
extern Gui gui;

bool PostProcess::CreateFBO() {
    if (!LoadFBOFunctions()) return false;
    int x = window.w;
    int y = window.h;
    GLuint depthRBO;

    glGenFramebuffersEXT(1, &FBOid);
    glGenRenderbuffersEXT(1, &depthRBO);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOid);
    {
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRBO);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, x, y);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRBO);

        {
            glGenTextures(1, &colorMapID);
            glBindTexture(GL_TEXTURE_2D, colorMapID);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, gui.bUseAO || gui.bUseDOF ? GL_RGBA16F_ARB : GL_RGBA8, x, y, 0, GL_RGBA,
                         GL_FLOAT, 0);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorMapID, 0);

            if (gui.bUseAO || gui.bUseDOF) {
                glGenTextures(1, &normalMapID);
                glBindTexture(GL_TEXTURE_2D, normalMapID);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, x, y, 0, GL_RGBA, GL_FLOAT, 0);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, normalMapID, 0);
            }
        }
    }
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        printf("FBO Error: %d\n", status);
        system("pause");
        return false;
    }

    const char *fragmentShader = "\n\
    \n\
    uniform sampler2D NormalMap;\n\
    uniform sampler2D NoiseMap;\n\
    \n\
    float LinearizeDepth(float depth, float zNear, float zFar)\n\
    {\n\
        return (depth-zNear)/(zFar-zNear);\n\
    }\n\
    \n\
    float ComputeAO(void)\n\
    {\n\
        #define SAMPLES 16\n\
        float strength = 0.07;\n\
        float offset = 18.0;\n\
        float falloff = 0.00000002;\n\
        float rad = 0.006;\n\
        float totStrength = 1.38;\n\
        const float invSamples = -1.38/10.0;\n\
        vec3 pSphere[16] = vec3[](vec3(0.53812504, 0.18565957, -0.43192),vec3(0.13790712, 0.24864247, 0.44301823),vec3(0.33715037, 0.56794053, -0.005789503),vec3(-0.6999805, -0.04511441, -0.0019965635),vec3(0.06896307, -0.15983082, -0.85477847),vec3(0.056099437, 0.006954967, -0.1843352),vec3(-0.014653638, 0.14027752, 0.0762037),vec3(0.010019933, -0.1924225, -0.034443386),vec3(-0.35775623, -0.5301969, -0.43581226),vec3(-0.3169221, 0.106360726, 0.015860917),vec3(0.010350345, -0.58698344, 0.0046293875),vec3(-0.08972908, -0.49408212, 0.3287904),vec3(0.7119986, -0.0154690035, -0.09183723),vec3(-0.053382345, 0.059675813, -0.5411899),vec3(0.035267662, -0.063188605, 0.54602677),vec3(-0.47761092, 0.2847911, -0.0271716));\n\
        vec3 fres = normalize((texture2D(NoiseMap,gl_TexCoord[0].xy * offset).xyz * 2.0) - vec3(1.0));\n\
        vec4 currentPixelSample = texture2D(NormalMap, gl_TexCoord[0].xy);\n\
        float currentPixelDepth = LinearizeDepth(currentPixelSample.a, 1.0, 400.0);\n\
        if(currentPixelDepth <= 0.06) return 1.0; //kvuli zbrani\n\
        \n\
        vec3 ep = vec3(gl_TexCoord[0].xy, currentPixelDepth);\n\
        vec3 norm = currentPixelSample.xyz;\n\
        \n\
        float bl = 0.0;\n\
        float radD = rad/currentPixelDepth;\n\
        \n\
        float occluderDepth;\n\
        for(int i=0; i < SAMPLES; i++)\n\
        {\n\
          vec3 ray = radD*reflect(pSphere[i],fres);\n\
          vec4 occluderFragment = texture2D(NormalMap,ep.xy + sign(dot(ray,norm) )*ray.xy);\n\
          float depthDifference = currentPixelDepth-LinearizeDepth(occluderFragment.a, 1.0, 400.0);\n\
          bl += step(falloff,depthDifference)*(1.0-dot(occluderFragment.xyz,norm))*(1.0-smoothstep(falloff,strength,depthDifference));\n\
        }\n\
        \n\
        return clamp(0.8+bl*invSamples, 0.0, 1.0);\n\
    }\n\
    \n\
    \n\
    void main(void)\n\
    {\n\
        gl_FragData[0].x = ComputeAO();\n\
    }\n\
    ";

    GLhandleARB deferredFS = shader.LoadShaderFromArray("deferred lighting fragment shader",
                                                        (const GLcharARB **) &fragmentShader, GL_FRAGMENT_SHADER_ARB);

    if (!deferredFS) {
        printf("error: couldn't create deferred render fragment shader handle!\n");
        return false;
    }

    if (!(deferredPH = glCreateProgramObjectARB())) {
        printf("error: cannot create shader handle!\n");
        return false;
    }

    glAttachObjectARB(deferredPH, deferredFS);
    glLinkProgramARB(deferredPH);
    glValidateProgramARB(deferredPH);

    iUniformDiffuseMap = glGetUniformLocationARB(deferredPH, "DiffuseMap");
    iUniformNormalMap = glGetUniformLocationARB(deferredPH, "NormalMap");
    iUniformNoiseMap = glGetUniformLocationARB(deferredPH, "NoiseMap");

    noiseMapID = texture.LoadTexture("textures/ss/noise.dds", 0, 1);
    if (noiseMapID == -1) return false;

    printf("ok: loading finalizing shader!\n");
    if (!CreateFinalShader()) return false;

    return true;
}

bool PostProcess::CreateFinalShader(void) {
    int x = window.w / 2;
    int y = window.h / 2;

    if (gui.bUseAO) {
        glGenFramebuffersEXT(1, &finalFBOid);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, finalFBOid);
        {
            {
                glGenTextures(1, &aoMapID);
                glBindTexture(GL_TEXTURE_2D, aoMapID);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_FLOAT, 0);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, aoMapID, 0);

            }
        }
        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
            printf("FBO Error: %d\n", status);
            system("pause");
            return false;
        }
    }

    const char *fragmentShader = "\n\
    #version 120\n\
    \n\
    uniform sampler2D ColorMap;\n\
    uniform sampler2D DepthMap;\n\
    uniform sampler2D AOMap;\n\
    uniform float time;\n\
    \n\
    vec2 screen = vec2(%d.0, %d.0);\n\
    vec2 texel = (1.0 / screen);\n\
    \n\
    %s\n\
    %s\n\
    //#define _FILMGRAIN //asi necham natvrdo zapnutej\n\
    \n\
    float LinearizeDepth(float depth, float zNear, float zFar)\n\
    {\n\
        return clamp((depth-zNear)/(zFar-zNear), 0.0, 1.0);\n\
    }\n\
    \n\
    vec4 GetBlurredScene(in sampler2D tex, in vec2 texCoord)\n\
    {\n\
       float weightSum = 70.0 + 2.0 * (1.0 + 8.0 + 28.0 + 56.0);\n\
       vec4 sum = vec4(0.0);\n\
       \n\
       sum += texture2D(tex, vec2(texCoord.x - 4.0*texel.x, texCoord.y - 4.0*texel.x)) * 1.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x - 3.0*texel.x, texCoord.y - 3.0*texel.x)) * 8.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x - 2.0*texel.x, texCoord.y - 2.0*texel.x)) * 28.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x - texel.x, texCoord.y - texel.x)) * 56.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x, texCoord.y)) * 70.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x + texel.x, texCoord.y + texel.x)) * 56.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x + 2.0*texel.x, texCoord.y + 2.0*texel.x)) * 28.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x + 3.0*texel.x, texCoord.y + 3.0*texel.x)) * 8.0 / weightSum;\n\
       sum += texture2D(tex, vec2(texCoord.x + 4.0*texel.x, texCoord.y + 4.0*texel.x)) * 1.0 / weightSum;\n\
       \n\
       return sum;\n\
    }\n\
    \n\
    void main(void)\n\
    {\n\
        float ao=0.0;\n\
        vec4 final = texture2D(ColorMap, gl_TexCoord[0].xy);\n\
        \n\
        #ifdef _FILMGRAIN\n\
            float x = gl_TexCoord[0].x * gl_TexCoord[0].y * time * 0.001;\n\
            x = mod(x, 13.0) * mod(x, 123.0);	\n\
            float dx = mod(x, 0.01);\n\
            float r = mod(x, 255.0);\n\
            float g = mod(x*100.0, 255.0);\n\
            float b = mod(x*0.001, 255.0);\n\
            \n\
            vec4 noise = vec4(clamp(0.5 + vec3(dx) * 100.0, 0.0, 1.0), 1.0);\n\
            if(noise.x < 0.95 && noise.y < 0.95 && noise.z < 0.95) noise *= vec4(r/255.0,g/255.0,b/255.0, 1.0)+0.5;\n\
            final = clamp(noise+0.4, 0.0, 1.0) * final;\n\
        #endif\n\
        \n\
        #ifdef _AO\n\
            for(int j = -3; j < 3; j++)\n\
            {\n\
                ao += texture2D(AOMap, gl_TexCoord[0].xy + vec2(j, j)*0.001).x * 0.15;\n\
            }\n\
            ao += 0.4;\n\
            final *= vec4(vec3(ao), 1.0);\n\
        #endif\n\
        \n\
        #ifdef _DOF\n\
            float depth = LinearizeDepth(texture2D(DepthMap, gl_TexCoord[0].xy).a, 1.0, 2000.0);\n\
            #ifdef _AO\n\
                final = mix(final, GetBlurredScene(ColorMap, gl_TexCoord[0].xy) * ao, depth);\n\
            #else\n\
                final = mix(final, GetBlurredScene(ColorMap, gl_TexCoord[0].xy), depth);\n\
            #endif\n\
            \n\
        #endif\n\
        \n\
        gl_FragColor = final;\n\
    }\n\
    ";
    char *f = new char[strlen(fragmentShader) * 2];
    sprintf(f, fragmentShader, window.w, window.h, gui.bUseAO ? "#define _AO" : " ",
            gui.bUseDOF ? "#define _DOF" : " ");

    GLhandleARB finalFS = shader.LoadShaderFromArray("finalizing fragment shader", (const GLcharARB **) &f,
                                                     GL_FRAGMENT_SHADER_ARB);

    if (f) delete f;

    if (!finalFS) {
        printf("error: couldn't create final shader fragment shader handle!\n");
        return false;
    }

    if (!(finalPH = glCreateProgramObjectARB())) {
        printf("error: cannot create shader handle!\n");
        return false;
    }

    glAttachObjectARB(finalPH, finalFS);
    glLinkProgramARB(finalPH);
    glValidateProgramARB(finalPH);

    iUniformColorMap = glGetUniformLocationARB(finalPH, "ColorMap");
    iUniformDepthMap = glGetUniformLocationARB(finalPH, "DepthMap");
    iUniformAOMap = glGetUniformLocationARB(finalPH, "AOMap");
    iUniformTime = glGetUniformLocationARB(finalPH, "time");

    return true;
}

void PostProcess::RenderToFBO() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOid);
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (gui.bUseAO || gui.bUseDOF) {
            GLenum buffers[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
            glDrawBuffers(2, buffers);
        } else {
            GLenum buffers[] = {GL_COLOR_ATTACHMENT0_EXT};
            glDrawBuffers(1, buffers);
        }

        level.Draw();
    }
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void PostProcess::RenderLevel(void) {
    //FIXME: jenom kvuli zrychleni na moji pomale 7150GF
    //level.Draw();
    //return;
    if (gui.bUseShadows) level.RenderToShadowMap();
    RenderToFBO();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, window.w, 0, window.h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    {
        if (gui.bUseAO) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, finalFBOid);
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                GLenum buffers[] = {GL_COLOR_ATTACHMENT0_EXT};
                glDrawBuffers(1, buffers);

                glUseProgramObjectARB(deferredPH);
                {
                    glActiveTextureARB(GL_TEXTURE0_ARB);
                    glBindTexture(GL_TEXTURE_2D, normalMapID);
                    glUniform1iARB(iUniformNormalMap, 0);

                    glActiveTextureARB(GL_TEXTURE1_ARB);
                    glBindTexture(GL_TEXTURE_2D, noiseMapID);
                    glUniform1iARB(iUniformNoiseMap, 1);

                    float quad[] =
                            {
                                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                    2.0f, 0.0f, window.w, 0.0f, 0.0f,
                                    2.0f, 2.0f, window.w, window.h, 0.0f,
                                    0.0f, 2.0f, 0.0f, window.h, 0.0f
                            };

                    glInterleavedArrays(GL_T2F_V3F, 0, quad);
                    glDrawArrays(GL_QUADS, 0, 4);
                }
                glUseProgramObjectARB(0);
            }
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }
        //FINAL PASS
        {
            glUseProgramObjectARB(finalPH);
            {
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glBindTexture(GL_TEXTURE_2D, colorMapID);
                glUniform1iARB(iUniformColorMap, 0);
                glUniform1fARB(iUniformTime, timer.GetSystemTime());

                if (gui.bUseAO) {
                    glActiveTextureARB(GL_TEXTURE1_ARB);
                    glBindTexture(GL_TEXTURE_2D, aoMapID);
                    glUniform1iARB(iUniformAOMap, 1);
                }

                if (gui.bUseDOF) {
                    glActiveTextureARB(GL_TEXTURE2_ARB);
                    glBindTexture(GL_TEXTURE_2D, normalMapID);
                    glUniform1iARB(iUniformDepthMap, 2);
                }

                float quad[] =
                        {
                                0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                1.0f, 0.0f, window.w, 0.0f, 0.0f,
                                1.0f, 1.0f, window.w, window.h, 0.0f,
                                0.0f, 1.0f, 0.0f, window.h, 0.0f
                        };

                glInterleavedArrays(GL_T2F_V3F, 0, quad);
                glDrawArrays(GL_QUADS, 0, 4);
            }
            glUseProgramObjectARB(0);
        }
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
