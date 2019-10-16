#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "Console.hpp"

GLFWwindow* createWindow();

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;

void consoleMain(int index, MessageBus& msgBus);

int main(int argc, char* argv[]) {
    MessageBus msgBus;
    Console console(&msgBus);

    // Create console thread that listens for commands
    auto console_thread = std::thread(&Console::listen, &console);

    // Initialize GLFW instance
    GLFWwindow* window = createWindow();
    if (window == NULL) {
        console.logMessage("Window failed to create");
        return -1;
    }

    // simulate typing in a command
    char text[] = { "type this and then press enter: " };
    for (int i = 0; i < 32; i++) {
        console.keyPress(CONSOLE_KEY::letter, text[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    console.keyPress(CONSOLE_KEY::enter, ' ');

    console.logMessage("Sleeping GLFW for 3 seconds");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    console.logMessage("Done sleeping");

    glfwTerminate();

    // Rejoin threads together
    console.killConsole();
    console_thread.join();


    /*printf("Starting message loop\n");
    int it = 0;
    bool done = false;
    while (!done) {
        if (msgBus.hasMessages) {
            Message msg = msgBus.PopQueue();

            switch (msg.type) {
            case MessageType::quit: {
                printf("Received quit message\n");
                done = true;
            } break;
            default:
                break;
            }
        }
        it++;
        if (it % 100 == 0) {
            printf("Iteration %d\n", it);
        }
        if (it == 2000) {
            done = true;
            printf("reached maximum iterations\n");
        }
    }
    printf("Ending message loop\n");
    */

    //console_thread.join();
    

    /*
    // Initialize GLFW instance
    GLFWwindow* window = createWindow();
    if (window == NULL) {
        printf("Window failed to create.\n");
        return -1;
    }

    printf("Sleeping for 3 seconds\n");
    auto sleepTime = std::chrono::system_clock::now();
    sleepTime += std::chrono::seconds(3);
    std::this_thread::sleep_until(sleepTime);
    printf("Done sleeping\n");

    glfwTerminate();*/
    
    //system("pause");
    //char c;
    //std::cin >> c;
    return 0;
}

struct process {
    PROCESS_INFORMATION p_info;
    STARTUPINFO s_info;
};

int create_process(process* p, const char* exe_path, char* cmd_line_args) {
    p->s_info.cb = sizeof(STARTUPINFO);

    return CreateProcessA(
        exe_path,
        cmd_line_args,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE, // <-- here
        NULL,
        NULL,
        &p->s_info,
        &p->p_info
    );
}

void consoleMain(int index, MessageBus& msgBus) {
    /*process p = { {0}, {0} };
    if (!create_process(&p, "cmd.exe", NULL)) {
        printf("Cannot create process\n");
    } else {
        printf("closing process 1\n");
        CloseHandle(p.p_info.hThread);
        CloseHandle(p.p_info.hProcess);
    }*/

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {10, 20};
    if (!SetConsoleCursorPosition(hConsole, coord)) {
        printf("Error!\n");
    }

    bool consoleDone = false;

    std::cout << "> ";
    std::string command;
    std::cin >> command;
    if (command.compare("quit") == 0) {
        Message msg;
        msg.type = MessageType::quit;
        msgBus._PostMessage(msg);
    }
}

void ErrorCallback(int, const char* err_str)
{
    printf("GLFW Error: %s\n", err_str);
}

GLFWwindow* createWindow() {
    // Register error callback first
    glfwSetErrorCallback(ErrorCallback);

    printf("Creating GLFW Window.\n");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "petey", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return NULL;
    }

    //set window position
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, mode->width * .32, mode->height / 2 - WINDOW_HEIGHT / 2);

    return window;
}