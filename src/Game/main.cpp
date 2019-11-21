#include <stdio.h>

#include "Message/CoreSystem.hpp"

#include "Window/Window.hpp"
#include "Resource/ResourceManager.hpp"
#include "Console.hpp"

#include "Resource/MemoryManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Render/RenderManager.hpp"
#include "Input.hpp"
#include "UI/UIRenderer.hpp"
#include "OptionsPane.hpp"

/* Core Systems */
Window g_MainWindow;
OptionsPane g_OptionWindow;
FileSystem g_FileSystem;
//ResourceManager g_ResourceManager;
SceneManager g_SceneManager;
RenderManager g_RenderManager;
UIRenderer g_UIRenderer;

void globalHandleMessage(Message msg);
u8 gameState = 1;
u8 framerateLimit = 1;
u8 frameCount = 0;

int main(int argc, char* argv[]) {
    EnsureDataTypeSize();

    for (int n = 0; n < 5; n++) {
        printf(".");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    printf("\n");

    // Create core systems
    MessageBus::create();
    MessageBus::setGlobalMessageHandleCallback(globalHandleMessage);

    MessageBus::registerSystem(g_MainWindow.create());
    MessageBus::registerSystem(g_FileSystem.create());
    MessageBus::registerSystem(g_ResourceManager.create());
    MessageBus::registerSystem(g_SceneManager.create());
    MessageBus::registerSystem(g_RenderManager.create());
    MessageBus::registerSystem(g_UIRenderer.create());
    g_OptionWindow.create(&g_MainWindow, true);
    g_OptionWindow.redraw();

    Message::listMessageTypes();

    // Other sets
    g_ResourceManager.setFileSystem(&g_FileSystem);

    /* TESTING START */
    EnvironmentMap::InitVAO();
    g_SceneManager.loadScenes(&g_ResourceManager); // Load dummy scene to test
    g_ResourceManager.createGrid(.5, 41, 10);
    /*  TESTING END  */

    // Create console thread that listens for commands
    Console::startListening(false);

    g_MainWindow.makeCurrent();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(10);
    //glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    //glClearColor(0.716f, 0.68f, 0.652f, 1.0f);
    //glClearColor(255.0f/255.0f, 248.0f/255.0f, 231.0f/255.0f, 1.0f);//Cosmic Latte, too bright :(
    glClearColor(0, 0, 0, 1);

    auto nextFrameStart = std::chrono::system_clock::now();
    auto frameStart = nextFrameStart;
    auto frameEnd = frameStart + std::chrono::microseconds(1);
    f32 dt = 0;

    Console::logMessage("Starting message loop.");
    bool done = false;
    while (!done) {
        frameStart = frameEnd;
        nextFrameStart = frameStart + std::chrono::microseconds(16666);

        if (g_MainWindow.shouldClose()) {
            done = true;
            break;
        }

        MessageBus::processEntireQueue();
        Input::pollKeys(g_MainWindow.m_glfwWindow);

        if (gameState) {
            g_SceneManager.update(dt);
            g_RenderManager.renderScene(&g_MainWindow, g_SceneManager.getCurrentScene());
        } else {
            g_RenderManager.renderScene(&g_MainWindow, g_SceneManager.getCurrentScene());
        }

        g_UIRenderer.renderScene(&g_MainWindow, g_SceneManager.getCurrentScene());

        g_MainWindow.swapAndPoll();

        frameEnd = std::chrono::system_clock::now();
        long long k = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart).count();
        dt = static_cast<f32>(k) / 1000000.0;
        g_RenderManager.lastFrameTime(k);

        if (framerateLimit) {
            std::this_thread::sleep_until(nextFrameStart);
        }
    }
    Console::logMessage("Game loop done, quitting...");

    glfwTerminate();

    //std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Destroy all subsystems
    g_ResourceManager.destroy();
    g_FileSystem.destroy();
    Console::destroy();
    g_MainWindow.destroy();
    g_RenderManager.destroy();
    g_UIRenderer.destroy();
    //g_ConfigManager.destroy();

    // End threads
    Console::rejoin();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return 0;
}

void globalHandleMessage(Message msg) {
    if (msg.isType("InputKey")) {
        using dt = Message::Datatype;
        dt key = msg.data[0];
        dt scancode = msg.data[1];
        dt action = msg.data[2];
        dt mods = msg.data[3];
        

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
            Console::logMessage("Switching game context");

            gameState = gameState ? 0 : 1;
        }

        if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            Console::logMessage("Toggling framerate limiting");

            framerateLimit = framerateLimit ? 0 : 1;
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            Console::logMessage("Quitting");
            g_MainWindow.close();
        }
    }

    if (msg.isType("InputMouseButton")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt button = msg.data[0];
        dt action = msg.data[1];
        dt mods = msg.data[2];
        dt xPos = msg.data[3];
        dt yPos = msg.data[4];

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            //Console::logMessage("Clicked: " + std::to_string(xPos) + ", " + std::to_string(yPos));
            g_OptionWindow.click(xPos, yPos);

            //vec3 worldPos = camera->getWorldPos(xPos, yPos);
        }
    }
}

bool Scene::recognizeCustomEntity(std::string entType) {
    return false;
}

void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss) {
    return;
}