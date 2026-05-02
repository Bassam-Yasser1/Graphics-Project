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
#define IDM_T5_ALL          2004

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
    AppendMenu(hT5, MF_STRING, IDM_T5_DIRECT,   L"Draw Ellipse (&Direct)");
    AppendMenu(hT5, MF_STRING, IDM_T5_POLAR,    L"Draw Ellipse (&Polar)");
    AppendMenu(hT5, MF_STRING, IDM_T5_MIDPOINT, L"Draw Ellipse (&Midpoint)");
    AppendMenu(hT5, MF_SEPARATOR, 0, NULL);
    AppendMenu(hT5, MF_STRING, IDM_T5_ALL,      L"Draw &All (compare)");
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

    // ---- Example from the template: click to draw an ellipse
    //      and add it to the shape list (Task 1 persistence) -----
    case WM_LBUTTONDOWN:
    {
        // Get click position and use it as the centre for a new ellipse
        int mx = LOWORD(lp);
        int my = HIWORD(lp);

        // Add to shape list so Save/Load will include it
        ShapeRecord rec;
        rec.type = ShapeType::ELLIPSE;
        rec.x1 = mx - 80; rec.y1 = my - 50;
        rec.x2 = mx + 80; rec.y2 = my + 50;
        g_shapes.push_back(rec);

        hdc = GetDC(hwnd);
        Ellipse(hdc, rec.x1, rec.y1, rec.x2, rec.y2);
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

        // Task 5 – Ellipse Algorithms
        case IDM_T5_DIRECT:
        {
            hdc = GetDC(hwnd);
            DrawEllipseDirect(hdc, ELLIPSE_CX, ELLIPSE_CY,
                              ELLIPSE_A, ELLIPSE_B, RGB(0, 0, 220));
            ReleaseDC(hwnd, hdc);
            break;
        }
        case IDM_T5_POLAR:
        {
            hdc = GetDC(hwnd);
            DrawEllipsePolar(hdc, ELLIPSE_CX, ELLIPSE_CY,
                             ELLIPSE_A, ELLIPSE_B, RGB(0, 180, 0));
            ReleaseDC(hwnd, hdc);
            break;
        }
        case IDM_T5_MIDPOINT:
        {
            hdc = GetDC(hwnd);
            DrawEllipseMidpoint(hdc, ELLIPSE_CX, ELLIPSE_CY,
                                ELLIPSE_A, ELLIPSE_B, RGB(220, 0, 0));
            ReleaseDC(hwnd, hdc);
            break;
        }
        case IDM_T5_ALL:
        {
            hdc = GetDC(hwnd);
            DrawAllEllipses(hdc, ELLIPSE_CX, ELLIPSE_CY,
                            ELLIPSE_A, ELLIPSE_B, ELLIPSE_OFF);
            ReleaseDC(hwnd, hdc);
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
