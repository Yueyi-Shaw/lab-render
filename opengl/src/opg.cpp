#include "opg.h"

Render::Render()
{
    init();
}

Render::~Render()
{
    cleanup();
}

void Render::sceneui()
{
    static int last_scen_index = mCurrentIndex;
    if (ImGui::Begin("Scene Switcher"))
    {
        if (ImGui::Combo("ScenesList", &mCurrentIndex, mSceneNameList.data(), mSceneNameList.size()))
        {
            if (mCurrentIndex != last_scen_index)
            {
                mSceneManager.SwitchScene(mCurrentIndex);
                mScene = mSceneManager.GetCurrentScene();
                mScene->Init();
                last_scen_index = mCurrentIndex;
            }
        }
    }
    ImGui::End();
}

void Render::init()
{
    DEBUG_PRINTF("enter Render::init()\n");
    // init GLFW
    DEBUG_PRINTF("Render::init() load glfw\n");
    if (!glfwInit())
        DEBUG_PRINTF("Render::init() glfwInit() failed\n");
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    // Create a GLFW window
    mWindow = glfwCreateWindow(1280, 720, "OPG Example", NULL, NULL);
    if (!mWindow)
    {
        glfwTerminate();
        return;
    }
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetWindowSizeCallback(mWindow, window_size_callback);
    glfwSetKeyCallback(mWindow, key_callback);
    glfwSetCharCallback(mWindow, char_callback);
    glfwSetScrollCallback(mWindow, scroll_callback);
    glfwSetCursorPosCallback(mWindow, cursor_callback);
    glfwSetMouseButtonCallback(mWindow, mouse_button_callback);

    glfwMakeContextCurrent(mWindow);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        DEBUG_PRINTF("Failed to initialize OpenGL context\n");
        return;
    }
    // glad populates global constants after loading to indicate,
    // if a certain extension/version is available.
    DEBUG_PRINTF("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

    // Initialize ImGui
    DEBUG_PRINTF("Render::init() load imgui\n");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Load Scene
    std::shared_ptr<Scene1_1> scene1_1 = std::make_shared<Scene1_1>("Scene1_1");
    std::shared_ptr<Scene1_2> scene1_2 = std::make_shared<Scene1_2>("Scene1_2");
    mSceneManager.RegisterScene(scene1_1);
    mSceneManager.RegisterScene(scene1_2);
    mScene        = mSceneManager.GetCurrentScene();
    int scene_num = mSceneManager.GetSceneList().size();
    for (auto item : mSceneManager.GetSceneList())
    {
        const char *tmp = new char[item->GetName().length() + 1];
        std::strcpy(const_cast<char *>(tmp), item->GetName().c_str());
        mSceneNameList.push_back(tmp);
    }
    mScene->Init();
    mInitSuccess = true;
    DEBUG_PRINTF("Render::init() %i \n", mInitSuccess);
}

void Render::loop()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();

        // GL part
        mScene->GLRendering();

        // IMGUI part
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // TODO: draw imgui stuff here
        sceneui();
        mScene->ImguiRendering();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mWindow);
    }
}

void Render::cleanup()
{
    DEBUG_PRINTF("enter Render::cleanup()\n");

    for (const char *item : mSceneNameList)
    {
        delete[] item;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void Render::Start()
{
    if (!mInitSuccess)
    {
        ERR_PRINT("Render::Start() : opg render is uninitlized.")
        return;
    }
    loop();
}

int main(int argc, char **argv)
{
#ifdef DEBUG
    // init debug toolkits
    OPGToolkits toolkits;
#endif

    Render opg;
    opg.Start();
    return 0;
}