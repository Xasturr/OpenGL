#define GLEW_STATIC
#include "GL/glew.h"

#include "framework.h"
#include "WindowsProject1.h"
#include "Model.h"
#include "camera.h"

#include "windowsx.h"
#include <iostream>

#define MAX_LOADSTRING 100
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define CAMERA_SPEED 0.01

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];     
WCHAR szWindowClass[MAX_LOADSTRING];         
HDC dc;                                     
HGLRC rc;                                   
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool wPressed = false;
bool aPressed = false;
bool sPressed = false;
bool dPressed = false;
bool lBtnPressed = false;
bool firstMouse = true;
double lastXPos;
double lastYPos;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

bool onWindowInit(HWND hWnd);
void display(std::vector<Model*> models, Shader& shader);
void updateCameraPos();
void setGlViewport(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));
    glEnable(GL_DEPTH_TEST);
    MSG msg;
    Model* model = new Model("D:/Study/Prog/WindowsProject1/WindowsProject1/resources/objects/backpack/backpack.obj");
    Model* model1 = new Model("D:/Study/Prog/WindowsProject1/WindowsProject1/resources/objects/backpack/backpack.obj");
    
    model->getPosition();

    //glm::mat4 myMatrix = glm::translate(glm::mat4(), glm::vec3(10.0f, 0.0f, 0.0f));
    //glm::vec4 myVector(10.0f, 10.0f, 10.0f, 0.0f);
    //glm::vec4 transformedVector = myMatrix * myVector;

    glm::vec3 v = glm::vec3(8.0f, 0.0f, 0.0f);
    model1->setPosition(v);

    model1->getPosition();
    //Model* model2 = new Model("D:/Study/Prog/WindowsProject1/WindowsProject1/resources/objects/backpack/backpack.obj");

    //D:/Skull.stl
    Shader ourShader("Shaders/4.2.texture.vs", "Shaders/4.2.texture.fs");

    std::vector<Model*> models = { model, model1 };

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        display(models, ourShader);
    }

    for (auto model : models)
        delete model;

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    //break;
    case WM_CREATE:
    {
        return onWindowInit(hWnd);
    }
    case WM_DESTROY:
    {
        ReleaseDC(hWnd, dc);
        wglDeleteContext(rc);
        PostQuitMessage(0);
        return 0;
    } 
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        setGlViewport(hWnd);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_TIMER:
    {
        updateCameraPos();
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }
    case WM_MOUSEMOVE:
    {
        const auto xPos = GET_X_LPARAM(lParam);
        const auto yPos = GET_Y_LPARAM(lParam);

        if ((xPos || yPos) && lBtnPressed)
        {
            if (firstMouse)
            {
                lastXPos = xPos;
                lastYPos = yPos;
                firstMouse = false;
            }

            auto xOffset = xPos - lastXPos;
            auto yOffset = lastYPos - yPos;
            lastXPos = xPos;
            lastYPos = yPos;

            camera.ProcessMouseMovement(xOffset, yOffset);
        }

        break;
    }
    case WM_LBUTTONDOWN:
    {
        const auto xPos = GET_X_LPARAM(lParam);
        const auto yPos = GET_Y_LPARAM(lParam);

        if (xPos || yPos)
        {
            lBtnPressed = true;
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        lBtnPressed = false;
        firstMouse = true;
        break;
    }
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
            case 0x57:
            {
                wPressed = true;
                break;
            }
            case 0x41:
            {
                aPressed = true;
                break;
            }
            case 0x53:
            {
                sPressed = true;
                break;
            }
            case 0x44:
            {
                dPressed = true;
                break;
            }
        }
        break;
    }
    case WM_KEYUP:
    {
        switch (wParam)
        {
            case 0x57:
            {
                wPressed = false;
                break;
            }
            case 0x41:
            {
                aPressed = false;
                break;
            }
            case 0x53:
            {
                sPressed = false;
                break;
            }
            case 0x44:
            {
                dPressed = false;
                break;
            }
        }
        break;
    }
    case WM_SYSKEYDOWN:
        break;

    case WM_SYSCHAR:
        break;

    case WM_SYSKEYUP:
        break;

    case WM_CHAR:
        break;
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool onWindowInit(HWND hWnd)
{
    PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
    1,                     // version number  
    PFD_DRAW_TO_WINDOW |   // support window  
        PFD_SUPPORT_OPENGL |   // support OpenGL  
        PFD_DOUBLEBUFFER,      // double buffered  
    PFD_TYPE_RGBA,         // RGBA type  
    24,                    // 24-bit color depth  
    0, 0, 0, 0, 0, 0,      // color bits ignored  
    0,                     // no alpha buffer  
    0,                     // shift bit ignored  
    0,                     // no accumulation buffer  
    0, 0, 0, 0,            // accum bits ignored  
    32,                    // 32-bit z-buffer      
    0,                     // no stencil buffer  
    0,                     // no auxiliary buffer  
    PFD_MAIN_PLANE,        // main layer  
    0,                     // reserved  
    0, 0, 0                // layer masks ignored  
    };
    dc = GetDC(hWnd);
    int pf = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, pf, &pfd);
    rc = wglCreateContext(dc);
    wglMakeCurrent(dc, rc);
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        return false;
    }
    setGlViewport(hWnd);
    SetTimer(hWnd, 1, 20, NULL);
    return true;
}

void updateCameraPos()
{
    if (wPressed)
        camera.ProcessKeyboard(FORWARD, CAMERA_SPEED);
    if (aPressed)
        camera.ProcessKeyboard(LEFT, CAMERA_SPEED);
    if (sPressed)
        camera.ProcessKeyboard(BACKWARD, CAMERA_SPEED);
    if (dPressed)
        camera.ProcessKeyboard(RIGHT, CAMERA_SPEED);
}

void display(std::vector<Model*> models, Shader& shader)
{
    glClearColor(0.3f, 1.f, 0.3f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& model : models)
        model->Draw(shader, camera);

    glEnd();
    glFlush();
    SwapBuffers(dc);
}

void setGlViewport(HWND hWnd)
{
    RECT rect;
    GetWindowRect(hWnd, &rect);
    auto horSize = rect.right - rect.left;
    auto verSize = rect.bottom - rect.top;
    if (horSize >= verSize)
    {
        auto offset = (horSize - verSize) / 2;
        glViewport(offset, 0, verSize, verSize);
    }
    else
    {
        auto offset = (verSize - horSize) / 2;
        glViewport(0, offset, horSize, horSize);
    }
}