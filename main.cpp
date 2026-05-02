// main.cpp
// Entry point for the Computer Graphics project.
// Links Task 1 (File Menu) and Task 5 (Ellipse Algorithms).
//
// Menu layout:
//   File
//     Clear          -> clears all drawn shapes
//     Save           -> saves shapes to a file
//     Load           -> loads shapes from a file
//     ─────────────
//     Exit
//   Task 5 – Ellipses
//     Draw (Direct)     -> DrawEllipseDirect
//     Draw (Polar)      -> DrawEllipsePolar
//     Draw (Midpoint)   -> DrawEllipseMidpoint
//     Draw All (side by side)
// Enable UNICODE support for consistent character handling
#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

// Include library for generic text mappings (_T macro)
#include <tchar.h>

#include <windows.h>
#include "task1_file_menu.h"
#include "task5_ellipse_algorithms.h"

// -------------------------------------------------------
// Menu IDs
// -------------------------------------------------------
#define IDM_FILE_CLEAR      1001
#define IDM_FILE_SAVE       1002
#define IDM_FILE_LOAD       1003
#define IDM_FILE_EXIT       1004

#define IDM_T5_DIRECT       2001
#define IDM_T5_POLAR        2002
#define IDM_T5_MIDPOINT     2003

// Global to track the currently selected drawing algorithm
static ShapeType g_currentType = ShapeType::ELLIPSE_DIRECT;

// Ellipse parameters used for Task 5 drawing
static const int ELLIPSE_CX = 400;   // centre x
static const int ELLIPSE_CY = 300;   // centre y
static const int ELLIPSE_A  = 150;   // semi-major axis (horizontal)
static const int ELLIPSE_B  = 100;   // semi-minor axis (vertical)
static const int ELLIPSE_OFF = 170;  // horizontal offset for "Draw All"

// -------------------------------------------------------
// CreateAppMenu – builds the menu bar programmatically
// -------------------------------------------------------
static HMENU CreateAppMenu()
{
    HMENU hMenuBar = CreateMenu();

    // --- File menu ---
    HMENU hFile = CreatePopupMenu();
    AppendMenu(hFile, MF_STRING,    IDM_FILE_CLEAR, L"&Clear");
    AppendMenu(hFile, MF_STRING,    IDM_FILE_SAVE,  L"&Save...");
    AppendMenu(hFile, MF_STRING,    IDM_FILE_LOAD,  L"&Load...");
    AppendMenu(hFile, MF_SEPARATOR, 0,              NULL);
    AppendMenu(hFile, MF_STRING,    IDM_FILE_EXIT,  L"E&xit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, L"&File");

    // --- Task 5 menu ---
    HMENU hT5 = CreatePopupMenu();
    AppendMenu(hT5, MF_STRING, IDM_T5_DIRECT,   L"Select Direct Ellipse");
    AppendMenu(hT5, MF_STRING, IDM_T5_POLAR,    L"Select Polar Ellipse");
    AppendMenu(hT5, MF_STRING, IDM_T5_MIDPOINT, L"Select Midpoint Ellipse");
    
    // Check the default item
    CheckMenuItem(hT5, IDM_T5_DIRECT, MF_CHECKED);

    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hT5,
               L"Task &5 – Ellipses");

    return hMenuBar;
}

// -------------------------------------------------------
// WndProc
// -------------------------------------------------------
LRESULT WINAPI WndProc(HWND hwnd, UINT mcode, WPARAM wp, LPARAM lp)
{
    HDC hdc;

    switch (mcode)
    {
    // ---- Repaint: redraw all saved shapes (Task 1) ------
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        RedrawShapes(hdc);
        EndPaint(hwnd, &ps);
        break;
    }

    // ---- Click to draw with the SELECTED algorithm (Task 1 & 5) -----
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lp);
        int my = HIWORD(lp);

        // Define a bounding box for the ellipse (80x50 radius)
        ShapeRecord rec;
        rec.type = g_currentType;
        rec.x1 = mx - 80; rec.y1 = my - 50;
        rec.x2 = mx + 80; rec.y2 = my + 50;
        
        // Add to shape list for persistence
        g_shapes.push_back(rec);

        // Draw it immediately
        hdc = GetDC(hwnd);
        int cx = mx;
        int cy = my;
        int a = 80;
        int b = 50;

        switch (g_currentType)
        {
        case ShapeType::ELLIPSE_DIRECT:
            DrawEllipseDirect(hdc, cx, cy, a, b, RGB(0, 0, 220));
            break;
        case ShapeType::ELLIPSE_POLAR:
            DrawEllipsePolar(hdc, cx, cy, a, b, RGB(0, 180, 0));
            break;
        case ShapeType::ELLIPSE_MIDPOINT:
            DrawEllipseMidpoint(hdc, cx, cy, a, b, RGB(220, 0, 0));
            break;
        }
        ReleaseDC(hwnd, hdc);
        break;
    }

    // ---- Menu commands -----------------------------------
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
        // Task 1 – File Menu
        case IDM_FILE_CLEAR:
            ClearScreen(hwnd);
            break;
        case IDM_FILE_SAVE:
            SaveToFile(hwnd);
            break;
        case IDM_FILE_LOAD:
            LoadFromFile(hwnd);
            break;
        case IDM_FILE_EXIT:
            DestroyWindow(hwnd);
            break;

        // Task 5 – Ellipse Algorithms (Selection Mode)
        case IDM_T5_DIRECT:
        case IDM_T5_POLAR:
        case IDM_T5_MIDPOINT:
        {
            // Update the global selection
            if (LOWORD(wp) == IDM_T5_DIRECT)   g_currentType = ShapeType::ELLIPSE_DIRECT;
            if (LOWORD(wp) == IDM_T5_POLAR)    g_currentType = ShapeType::ELLIPSE_POLAR;
            if (LOWORD(wp) == IDM_T5_MIDPOINT) g_currentType = ShapeType::ELLIPSE_MIDPOINT;

            // Update menu checkmarks
            HMENU hMenu = GetMenu(hwnd);
            CheckMenuItem(hMenu, IDM_T5_DIRECT,   (g_currentType == ShapeType::ELLIPSE_DIRECT)   ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_T5_POLAR,    (g_currentType == ShapeType::ELLIPSE_POLAR)    ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_T5_MIDPOINT, (g_currentType == ShapeType::ELLIPSE_MIDPOINT) ? MF_CHECKED : MF_UNCHECKED);
            break;
        }
        }
        break;

    // ---- Standard window lifecycle -----------------------
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, mcode, wp, lp);
    }
    return 0;
}

// -------------------------------------------------------
// WinMain – application entry point
// -------------------------------------------------------
int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR c, int nsh)
{
    WNDCLASS wc{};
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance     = h;
    wc.lpfnWndProc   = WndProc;
    wc.lpszClassName = L"GraphicsProjectClass";
    wc.lpszMenuName  = NULL;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        L"GraphicsProjectClass",
        L"Graphics Project – Task 1 & Task 5",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 650,
        NULL, NULL, h, NULL
    );

    // Attach the menu bar
    SetMenu(hwnd, CreateAppMenu());

    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}
