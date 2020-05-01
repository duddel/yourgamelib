/*
Copyright (c) 2019-2020 Alexander Scholz

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include <cmath>
#include <vector>
#include "miniaudio.h"
#include "yourgame/assetfile.h"
#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"
#include "yourgame/input.h"
#include "imgui.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"
#include "trafo.h"
#include "camera.h"
#include "glbuffer.h"
#include "glmesh.h"
#include "glshader.h"

namespace mygame
{

namespace
{
void maDataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);
void loadObj();
void initGlScene();
void initAudio();
void updateImgui(const yourgame::context &ctx);

// Audio
bool g_playAudio = false;
bool g_audioInitialized = false;
std::vector<uint8_t> g_audioData;
ma_decoder g_maDecoder;
ma_device_config g_maDeviceConfig;
ma_device g_maDevice;

// GL scene
double g_colorFadingT = 0.0;
ImVec4 g_clearColor = ImVec4(0.4f, 0.6f, 0.8f, 1.00f);
Trafo g_modelTrafo;
Camera g_camera;
GLMesh *g_mesh = nullptr;
GLShader *g_shader = nullptr;
const GLuint attrLocPosition = 3;

} // namespace

void init(const yourgame::context &ctx)
{
    g_camera.trafo()->lookAt(glm::vec3(0.0f, 2.0f, 3.0f),
                             glm::vec3(0.0f, 0.0f, 0.0f),
                             glm::vec3(0.0f, 1.0f, 0.0f));
    g_camera.setPerspective(75.0f, ctx.winAspectRatio, 0.1f, 10.0f);
    g_modelTrafo.rotateGlobal(0.5f, Trafo::AXIS::Z);

    initGlScene();

#ifdef YOURGAME_PLATFORM_ANDROID
    // arbitrary scale-up on android,
    // todo: DPI awareness
    ImGui::GetStyle().ScaleAllSizes(5.0f);
#endif
}

void update(const yourgame::context &ctx)
{
    g_modelTrafo.rotateLocal(0.03f, Trafo::AXIS::X);
    g_colorFadingT += (1.0 * ctx.deltaTimeS);

    if (yourgame::getInputi(yourgame::InputSource::YOURGAME_KEY_ESCAPE))
    {
        yourgame::notifyShutdown();
    }

    updateImgui(ctx);
}

void draw(const yourgame::context &ctx)
{
    g_camera.setAspect(ctx.winAspectRatio);
    glViewport(0, 0, ctx.winWidth, ctx.winHeight);

    float colFade = (float)((sin(g_colorFadingT) * 0.5) + 0.5);
    glClearColor(g_clearColor.x, g_clearColor.y, g_clearColor.z, g_clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_shader)
    {
        g_shader->useProgram();
        glUniform1f(g_shader->getUniformLocation("light"), colFade);
        auto mvp = g_camera.pMat() * g_camera.vMat() * g_modelTrafo.mat();
        glUniformMatrix4fv(g_shader->getUniformLocation("mMat"), 1, GL_FALSE, glm::value_ptr(mvp));
    }
    g_mesh->draw();
}

void shutdown(const yourgame::context &ctx)
{
    ma_device_uninit(&g_maDevice);
    ma_decoder_uninit(&g_maDecoder);
    g_audioInitialized = false;
}

namespace
{
void maDataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    if (!g_playAudio)
    {
        return;
    }

    if (ma_decoder_read_pcm_frames(&g_maDecoder, pOutput, frameCount) < frameCount)
    {
        g_playAudio = false;
    }
}

void loadObj()
{
    auto objData = yourgame::readAssetFile("monkey.obj");
    auto mtlData = yourgame::readAssetFile("monkey.mtl");

    std::string objStr(objData.begin(), objData.end());
    std::string mtlStr(mtlData.begin(), mtlData.end());

    tinyobj::ObjReader objRdr;
    tinyobj::ObjReaderConfig objRdrCfg;
    objRdr.ParseFromString(objStr, mtlStr, objRdrCfg);

    auto shapes = objRdr.GetShapes();
    auto attribs = objRdr.GetAttrib();

    std::vector<GLfloat> objPosData;
    for (int i = 0; i < attribs.vertices.size(); i++)
    {
        objPosData.push_back((GLfloat)attribs.vertices[i]);
    }

    std::vector<GLuint> objIdxData;
    for (auto const &idx : shapes[0].mesh.indices)
    {
        objIdxData.push_back((GLuint)(idx.vertex_index));
    }

    // created a GLMesh from parsed object file data
    auto vertPosSize = objPosData.size() * sizeof(objPosData[0]);
    auto vertIdxSize = objIdxData.size() * sizeof(objIdxData[0]);
    GLBuffer *posBuf = GLBuffer::make(GL_ARRAY_BUFFER, vertPosSize, objPosData.data(), GL_STATIC_DRAW);
    GLBuffer *idxBuf = GLBuffer::make(GL_ELEMENT_ARRAY_BUFFER, vertIdxSize, objIdxData.data(), GL_STATIC_DRAW);

    g_mesh = GLMesh::make(
        {{posBuf, attrLocPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)0}},
        {idxBuf, GL_UNSIGNED_INT, GL_TRIANGLES, (GLsizei)objIdxData.size()});
}

void initGlScene()
{
    loadObj();

    glEnable(GL_DEPTH_TEST);

    // shaders
#ifdef YOURGAME_GL_API_GLES
    auto vertCode = yourgame::readAssetFile("simple.es.vert");
    auto fragCode = yourgame::readAssetFile("simple.es.frag");
#else
    auto vertCode = yourgame::readAssetFile("simple.vert");
    auto fragCode = yourgame::readAssetFile("simple.frag");
#endif

    std::string vertShader(vertCode.begin(), vertCode.end());
    std::string fragShader(fragCode.begin(), fragCode.end());

    std::string shaderErrLog;
    g_shader = GLShader::make({{vertShader, GL_VERTEX_SHADER},
                               {fragShader, GL_FRAGMENT_SHADER}},
                              {{attrLocPosition, "posi"}},
                              {{0, "color"}},
                              shaderErrLog);
    if (!g_shader)
    {
        yourgame::loge("shader: %v", shaderErrLog);
    }
}

void initAudio()
{
    g_audioData = yourgame::readAssetFile("chirp.ogg");

    if (ma_decoder_init_memory_vorbis(&g_audioData[0], g_audioData.size(), NULL, &g_maDecoder) != MA_SUCCESS)
    {
        yourgame::loge("ma_decoder_init_memory_vorbis() failed");
    }

    g_maDeviceConfig = ma_device_config_init(ma_device_type_playback);
    g_maDeviceConfig.playback.format = g_maDecoder.outputFormat;
    g_maDeviceConfig.playback.channels = g_maDecoder.outputChannels;
    g_maDeviceConfig.sampleRate = g_maDecoder.outputSampleRate;
    g_maDeviceConfig.dataCallback = maDataCallback;
    g_maDeviceConfig.pUserData = NULL;

    if (ma_device_init(NULL, &g_maDeviceConfig, &g_maDevice) != MA_SUCCESS)
    {
        yourgame::loge("ma_device_init() failed");
    }

    if (ma_device_start(&g_maDevice) != MA_SUCCESS)
    {
        yourgame::loge("ma_device_start() failed");
    }

    g_audioInitialized = true;
}

void updateImgui(const yourgame::context &ctx)
{
    static bool showDemoAudio = false;
    static bool showDemoGl = true;
    static bool showLicense = false;
    static bool showImguiDemo = false;

    // Main Menu Bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                yourgame::notifyShutdown();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Demo"))
        {
            if (ImGui::MenuItem("GL", "", &showDemoGl))
            {
                showDemoGl = true;
            }
            if (ImGui::MenuItem("Audio", "", &showDemoAudio))
            {
                showDemoAudio = true;
            }
            if (ImGui::MenuItem("Imgui", "", &showImguiDemo))
            {
                showImguiDemo = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("License"))
            {
                showLicense = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showImguiDemo)
    {
        ImGui::ShowDemoWindow(&showImguiDemo);
    }

    // GL demo window
    if (showDemoGl)
    {
        ImGui::Begin("GL", &showDemoGl, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
        ImGui::ColorPicker3("clear color", (float *)&g_clearColor);

        // camera manipulation
        static int projMode = 0;
        ImGui::RadioButton("perspective", &projMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("orthographic", &projMode, 1);

        if (projMode == 0)
        {
            static float f1 = 75.0f;
            static float f2 = 0.1f;
            static float f3 = 10.0f;
            ImGui::DragFloatRange2("zNear/zFar", &f2, &f3, 0.05f, 0.1f, 10.0f);
            ImGui::SliderFloat("fovy", &f1, 10.0f, 180.0f);
            g_camera.setPerspective(f1, ctx.winAspectRatio, f2, f3);
        }
        else
        {
            static float f1 = 5.0f;
            static float f2 = 0.0f;
            static float f3 = 10.0f;
            ImGui::DragFloatRange2("zNear/zFar", &f2, &f3, 0.05f, 0.0f, 10.0f);
            ImGui::SliderFloat("height", &f1, 0.1f, 30.0f);
            g_camera.setOrthographic(f1, ctx.winAspectRatio, f2, f3);
        }

        ImGui::End();
    }

    // Audio demo window
    if (showDemoAudio)
    {
        if (!g_audioInitialized)
        {
            initAudio();
        }

        ImGui::Begin("Audio", &showDemoAudio, (ImGuiWindowFlags_NoCollapse));
        if (ImGui::Button("Play"))
        {
            ma_decoder_seek_to_pcm_frame(&g_maDecoder, 0); // "rewind" and start playback
            g_playAudio = true;
        }
        ImGui::End();
    }

    if (showLicense)
    {
#if defined(YOURGAME_PLATFORM_DESKTOP)
        static const char licFilename[] = "LICENSE_desktop.txt";
#elif defined(YOURGAME_PLATFORM_ANDROID)
        static const char licFilename[] = "LICENSE_android.txt";
#elif defined(YOURGAME_PLATFORM_WEB)
        static const char licFilename[] = "LICENSE_web.txt";
#endif

        auto licFileData = yourgame::readAssetFile(licFilename);
        std::string licStr(licFileData.begin(), licFileData.end());
        ImGui::SetNextWindowSizeConstraints(ImVec2((float)(ctx.winWidth) * 0.5f,
                                                   (float)(ctx.winHeight) * 0.5f),
                                            ImVec2((float)(ctx.winWidth),
                                                   (float)(ctx.winHeight)));
        ImGui::Begin("License", &showLicense, (ImGuiWindowFlags_NoCollapse));
        /* The following procedure allows displaying long wrapped text,
        whereas ImGui::TextWrapped() has a size limit and cuts the content. */
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextUnformatted(licStr.c_str());
        ImGui::PopTextWrapPos();
        ImGui::End();
    }
}

} // namespace

} // namespace mygame
