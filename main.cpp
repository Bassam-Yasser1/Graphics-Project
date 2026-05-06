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
#define _UNICODE
#define UNICODE

// Include library for generic text mappings (_T macro)
#include <tchar.h>
#include <vector>
#include <cmath>
#include <windows.h>
#include "task1_file_menu.h"
#include "task5_ellipse_algorithms.h"
#include "Circles.h"
#include "Clipping.h"
#include "SmileyFace.h"

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


#define IDM_CIRC_DIRECT     3001
#define IDM_CIRC_POLAR      3002
#define IDM_CIRC_IPOLAR     3003
#define IDM_CIRC_MIDPOINT   3004
#define IDM_CIRC_MOD_MID    3005

#define IDM_CLIP_RECT       4001
#define IDM_CLIP_SQUARE     4002
#define IDM_CLIP_CIRCLE     4003

#define IDM_FACE_HAPPY      5001
#define IDM_FACE_SAD        5002

// Global to track the currently selected drawing algorithm
static int g_currentSelection = IDM_T5_DIRECT; 
static std::vector<Point> g_mouseClicks;
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
    CheckMenuItem(hT5, IDM_T5_DIRECT, MF_CHECKED);
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hT5, L"Task &5 – Ellipses");
    
    // --- Circles menu ---
    HMENU hCirc = CreatePopupMenu();
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_DIRECT,   L"Direct Circle");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_POLAR,    L"Polar Circle");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_IPOLAR,   L"Iterative Polar");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_MIDPOINT, L"Midpoint Circle");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_MOD_MID,  L"Modified Midpoint");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hCirc, L"&Circles");

    // --- CLIPPING MENU ---
    HMENU hClip = CreatePopupMenu();
    AppendMenu(hClip, MF_STRING, IDM_CLIP_RECT,   L"Rectangle Window");
    AppendMenu(hClip, MF_STRING, IDM_CLIP_SQUARE, L"Square Window");
    AppendMenu(hClip, MF_STRING, IDM_CLIP_CIRCLE, L"Circle Window (Bonus)");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hClip, L"Cli&pping");

    // --- SMILEY FACES MENU ---
    HMENU hFace = CreatePopupMenu();
    AppendMenu(hFace, MF_STRING, IDM_FACE_HAPPY, L"Happy Face");
    AppendMenu(hFace, MF_STRING, IDM_FACE_SAD,   L"Sad Face");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFace, L"&Smiley Faces");
    
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

    // ---- Click to draw with the SELECTED algorithm -----
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lp);
        int my = HIWORD(lp);
        hdc = GetDC(hwnd);
        
        // Add current click to our point buffer
        g_mouseClicks.push_back({ mx, my });

        // --- TASK 5: ELLIPSES (1 Click) ---
        if (g_currentSelection >= 2001 && g_currentSelection <= 2003) 
        {
            ShapeRecord rec;
            rec.type = g_currentType;
            rec.x1 = mx - 80; rec.y1 = my - 50;
            rec.x2 = mx + 80; rec.y2 = my + 50;
            g_shapes.push_back(rec);

            if (g_currentSelection == IDM_T5_DIRECT) DrawEllipseDirect(hdc, mx, my, 80, 50, RGB(0, 0, 220));
            if (g_currentSelection == IDM_T5_POLAR)  DrawEllipsePolar(hdc, mx, my, 80, 50, RGB(0, 180, 0));
            if (g_currentSelection == IDM_T5_MIDPOINT) DrawEllipseMidpoint(hdc, mx, my, 80, 50, RGB(220, 0, 0));
            g_mouseClicks.clear();
        }
        
        // --- TASK: CIRCLES (2 Clicks: Center, Radius Point) ---
        else if (g_currentSelection >= 3001 && g_currentSelection <= 3005)
        {
            if (g_mouseClicks.size() == 2) {
                int R = (int)sqrt(pow(g_mouseClicks[1].x - g_mouseClicks[0].x, 2) + 
                                  pow(g_mouseClicks[1].y - g_mouseClicks[0].y, 2));
                int xc = g_mouseClicks[0].x, yc = g_mouseClicks[0].y;

                if (g_currentSelection == IDM_CIRC_DIRECT)   CircleDirect(hdc, xc, yc, R, RGB(0,0,0));
                if (g_currentSelection == IDM_CIRC_POLAR)    CirclePolar(hdc, xc, yc, R, RGB(0,0,0));
                if (g_currentSelection == IDM_CIRC_MIDPOINT) CircleMidpoint(hdc, xc, yc, R, RGB(0,0,0));
                if (g_currentSelection == IDM_CIRC_IPOLAR)   CircleIterativePolar(hdc, xc, yc, R, RGB(0,0,0));
                if (g_currentSelection == IDM_CIRC_MOD_MID)  CircleModifiedMidpoint(hdc, xc, yc, R, RGB(0,0,0));
                g_mouseClicks.clear();
            }
        }
        
        // --- TASK: CLIPPING (4 Clicks: 2 for Window, 2 for Line) ---
        else if (g_currentSelection >= 4001 && g_currentSelection <= 4003)
        {
            if (g_mouseClicks.size() == 4) {
                if (g_currentSelection == IDM_CLIP_RECT || g_currentSelection == IDM_CLIP_SQUARE)
                    ClipLineRect(hdc, g_mouseClicks[2].x, g_mouseClicks[2].y, g_mouseClicks[3].x, g_mouseClicks[3].y,
                                 g_mouseClicks[0].x, g_mouseClicks[0].y, g_mouseClicks[1].x, g_mouseClicks[1].y);
                
                else if (g_currentSelection == IDM_CLIP_CIRCLE) {
                    int R = (int)sqrt(pow(g_mouseClicks[1].x - g_mouseClicks[0].x, 2) + 
                                      pow(g_mouseClicks[1].y - g_mouseClicks[0].y, 2));
                    ClipLineCircle(hdc, g_mouseClicks[2].x, g_mouseClicks[2].y, g_mouseClicks[3].x, g_mouseClicks[3].y, 
                                   g_mouseClicks[0].x, g_mouseClicks[0].y, R);
                }
                g_mouseClicks.clear();
            }
        }
        
        // --- TASK: SMILEY FACES (1 Click) ---
        else if (g_currentSelection == IDM_FACE_HAPPY || g_currentSelection == IDM_FACE_SAD) {
            DrawSmiley(hdc, mx, my, (g_currentSelection == IDM_FACE_HAPPY));
            g_mouseClicks.clear();
        }

        ReleaseDC(hwnd, hdc);
        break;
    }

    // ---- Menu commands -----------------------------------
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
        case IDM_FILE_CLEAR: ClearScreen(hwnd); break;
        case IDM_FILE_SAVE:  SaveToFile(hwnd); break;
        case IDM_FILE_LOAD:  LoadFromFile(hwnd); break;
        case IDM_FILE_EXIT:  DestroyWindow(hwnd); break;

        // This handles ALL drawing tools (Ellipses, Circles, Clipping, Faces)
        default:
            g_currentSelection = LOWORD(wp); // Update which tool is active
            g_mouseClicks.clear();           // Reset points when switching tools

            // Handle Ellipse-specific Enum (for your Task 1 persistence)
            if (g_currentSelection == IDM_T5_DIRECT)   g_currentType = ShapeType::ELLIPSE_DIRECT;
            if (g_currentSelection == IDM_T5_POLAR)    g_currentType = ShapeType::ELLIPSE_POLAR;
            if (g_currentSelection == IDM_T5_MIDPOINT) g_currentType = ShapeType::ELLIPSE_MIDPOINT;

            // Refresh menu checkmarks for Task 5
            HMENU hMenu = GetMenu(hwnd);
            CheckMenuItem(hMenu, IDM_T5_DIRECT,   (g_currentSelection == IDM_T5_DIRECT)   ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_T5_POLAR,    (g_currentSelection == IDM_T5_POLAR)    ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_T5_MIDPOINT, (g_currentSelection == IDM_T5_MIDPOINT) ? MF_CHECKED : MF_UNCHECKED);
            break;
        }
        break;

    // ---- Standard window lifecycle -----------------------
        case WM_CLOSE:   DestroyWindow(hwnd); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default:         return DefWindowProc(hwnd, mcode, wp, lp);
    }
        return 0;
    }

// -------------------------------------------------------
// WinMain – application entry point
// -------------------------------------------------------
int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR c, int nsh)
{
    WNDCLASS wc{};
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance     = h;
    wc.lpfnWndProc   = WndProc;
    wc.lpszClassName = L"GraphicsProjectClass";
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"GraphicsProjectClass", L"Computer Graphics - All Tasks Integrated",
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 650,
                             NULL, NULL, h, NULL);

    SetMenu(hwnd, CreateAppMenu());
    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
