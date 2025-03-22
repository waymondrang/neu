#include "WindowManager.h"
#include "core.h"
#include "application.h"

#include <vector>
#include <iostream>
#include <cassert>

#define CHECK_KEY_PRESSED(key) glfwGetKey(window, key) == GLFW_PRESS
#define CHECK_MOUSE_BUTTON_PRESSED(button) glfwGetMouseButton(window, button) == GLFW_PRESS

using std::vector;

WindowManager::WindowManager(Application *context)
{
    // glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // glfwSetWindowUserPointer(window, context);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor

    // if (wireframe)
    //     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    this->context = context;
}

void WindowManager::initialize()
{
    /////////////////////
    // Initialize glfw //
    /////////////////////

    if (!glfwInit())
    {
        Logger::error("Failed to initialize GLFW");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void WindowManager::createWindow(int width, int height, std::string title)
{
    this->width = width;
    this->height = height;

    ///////////////////
    // CREATE WINDOW //
    ///////////////////

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    
    if (window == NULL)
    {
        const char *error;
        glfwGetError(&error);
        Logger::error("Failed to create GLFW window: %s", error);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // glfwSwapInterval(0); // disable vsync

    /////////////////////
    // INIITIALIZE GLAD //
    /////////////////////

    // https://github.com/Dav1dde/glad/wiki/C
    assert(gladLoadGL((GLADloadfunc)glfwGetProcAddress));

    // glViewport(0, 0, width, height);    // set viewport size
    // glEnable(GL_FRAMEBUFFER_SRGB);      // enable sRGB color space
    glEnable(GL_DEPTH_TEST);            // enable depth testing
}

void WindowManager::startFrame()
{
    glfwPollEvents();
    updateInputStates();
}

void WindowManager::endFrame()
{
    glfwSwapBuffers(window);
}

float WindowManager::getDeltaTime()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return deltaTime;
}

int WindowManager::windowShouldClose() const
{
    return glfwWindowShouldClose(window);
}

void WindowManager::closeWindow()
{
    glfwSetWindowShouldClose(window, true);
}

void WindowManager::registerFramebufferSizeCallback(std::function<void(int, int)> callback)
{
    framebufferSizeCallbacks.push_back(callback);
}

void WindowManager::notifyFramebufferSizeCallbacks(int width, int height)
{
    for (auto &callback : framebufferSizeCallbacks)
    {
        callback(width, height);
    }
}

void WindowManager::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    Logger::debug("Framebuffer size changed to %dx%d\n", width, height);

    // todo: decouple opengl from window manager
    glViewport(0, 0, width, height);

    WindowManager *windowManager = static_cast<WindowManager *>(glfwGetWindowUserPointer(window));
    windowManager->notifyFramebufferSizeCallbacks(width, height);
}

/**
 * Via https://www.glfw.org/docs/3.3/input_guide.html:
 * void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
 */

void WindowManager::updateInputStates()
{
    /**
     * Update the cached keyboard states for this frame.
     */
    for (const auto &mapping: keyboardMap)
    {
        if (glfwGetKey(window, mapping.first) == GLFW_PRESS)
        {
            if (keyPressed[mapping.second])
            {
                keyRepeated[mapping.second] = true;
            }
            else
            {
                keyPressed[mapping.second] = true;
            }
        } else {
            keyPressed[mapping.second] = false;
            keyRepeated[mapping.second] = false;
        }
    }

    /**
     * Update the cached mouse button states for this frame.
     */
    for (const auto &mapping: mouseButtonMap)
    {
        if (glfwGetMouseButton(window, mapping.first) == GLFW_PRESS)
        {
            if (mouseButtonPressed[mapping.second])
            {
                mouseButtonRepeated[mapping.second] = true;
            }
            else
            {
                mouseButtonPressed[mapping.second] = true;
            }
        } else {
            mouseButtonPressed[mapping.second] = false;
            mouseButtonRepeated[mapping.second] = false;
        }
    }

    /**
     * Update the mouse position.
     * 
     * The alternative to this is to use a callback function.
     */
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouseX = static_cast<float>(x);
    mouseY = static_cast<float>(y);
}

bool WindowManager::toggleCursor()
{
    cursorDisabled = glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;
    glfwSetInputMode(window, GLFW_CURSOR, cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    return cursorDisabled;
}

/**
 * Todo: Move this code into Context (which should be renamed to Game) class.
 */

// void WindowManager::update(InputManager *inputManager)
// {
//     if (inputManager.isMiddleMousePressed() && !inputManager->isMiddleMouseRepeated)
//     {
//         toggleCursor();
//     }
// }

bool WindowManager::isKeyPressed(int key) const
{
    return keyPressed.at(key);
}

bool WindowManager::isKeyRepeated(int key) const
{
    return keyRepeated.at(key);
}

bool WindowManager::isMouseButtonPressed(int button) const
{
    return mouseButtonPressed.at(button);
}

bool WindowManager::isMouseButtonRepeated(int button) const
{
    return mouseButtonRepeated.at(button);
}

Vector2f WindowManager::getMousePosition() const
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return Vector2f(x, y);
}