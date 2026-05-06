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
#include <algorithm>
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

        // --- TASK: CLIPPING HANDLER (L-Click to collect points) ---
        
        else if (g_currentSelection >= 4001 && g_currentSelection <= 4003)
        {
            // Clicks 1 & 2 define the Window
            if (g_mouseClicks.size() == 2) {
                int xmin = (std::min)(g_mouseClicks[0].x, g_mouseClicks[1].x);
                int ymin = (std::min)(g_mouseClicks[0].y, g_mouseClicks[1].y);
                int xmax = (std::max)(g_mouseClicks[0].x, g_mouseClicks[1].x);
                int ymax = (std::max)(g_mouseClicks[0].y, g_mouseClicks[1].y);

                if (g_currentSelection == IDM_CLIP_RECT) {
                    Rectangle(hdc, xmin, ymin, xmax, ymax);
                } 
                else if (g_currentSelection == IDM_CLIP_SQUARE) {
                    int side = (std::max)(abs(xmax - xmin), abs(ymax - ymin));
                    Rectangle(hdc, xmin, ymin, xmin + side, ymin + side);
                } 
                else if (g_currentSelection == IDM_CLIP_CIRCLE) {
                    int R = (int)sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2));
                    Ellipse(hdc, xmin - R, ymin - R, xmin + R, ymin + R);
                }
                printf(">> Window Set. Now click points for the Shape, then Right-Click to Clip.\n");
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

case WM_RBUTTONDOWN:
{
    if (g_currentSelection >= 4001 && g_currentSelection <= 4003 && g_mouseClicks.size() >= 3) 
    {
        HDC hdc = GetDC(hwnd);
        
        // 1. Setup Window Boundaries
        int xmin = (std::min)((int)g_mouseClicks[0].x, (int)g_mouseClicks[1].x);
        int ymin = (std::min)((int)g_mouseClicks[0].y, (int)g_mouseClicks[1].y);
        int xmax = (std::max)((int)g_mouseClicks[0].x, (int)g_mouseClicks[1].x);
        int ymax = (std::max)((int)g_mouseClicks[0].y, (int)g_mouseClicks[1].y);

        if (g_currentSelection == IDM_CLIP_SQUARE) {
            int side = (std::max)(abs(xmax - xmin), abs(ymax - ymin));
            xmax = xmin + side; ymax = ymin + side;
        }
        int R = (int)sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2));

        // --- CASE A: POINT CLIPPING (Exactly 3 clicks total) ---
        if (g_mouseClicks.size() == 3) {
            int px = (int)g_mouseClicks[2].x;
            int py = (int)g_mouseClicks[2].y;
            bool inside = false;

            if (g_currentSelection == IDM_CLIP_CIRCLE)
                inside = (sqrt(pow(px - xmin, 2) + pow(py - ymin, 2)) <= R);
            else
                inside = (px >= xmin && px <= xmax && py >= ymin && py <= ymax);

            if (inside) SetPixel(hdc, px, py, RGB(255, 0, 0));
            printf(">> Point Clipped.\n");
        }
        
        // --- CASE B: LINE CLIPPING (Exactly 4 clicks total) ---
        else if (g_mouseClicks.size() == 4) {
            int x1 = g_mouseClicks[2].x;
            int y1 = g_mouseClicks[2].y;
            int x2 = g_mouseClicks[3].x;
            int y2 = g_mouseClicks[3].y;

            if (g_currentSelection == IDM_CLIP_CIRCLE) {
                double cx = (double)g_mouseClicks[0].x; 
                double cy = (double)g_mouseClicks[0].y;
                
                double dx = (double)x2 - x1;
                double dy = (double)y2 - y1;

                // Quadratic coefficients
                double a = dx * dx + dy * dy;
                double b = 2 * (dx * (x1 - cx) + dy * (y1 - cy));
                double c = pow(x1 - cx, 2) + pow(y1 - cy, 2) - (double)R * R;
                
                double discriminant = b * b - 4 * a * c;

                if (discriminant >= 0) {
                    double sqrtDet = sqrt(discriminant);
                    double t1 = (-b - sqrtDet) / (2 * a);
                    double t2 = (-b + sqrtDet) / (2 * a);

                    // Clip the segment to the [0, 1] range
                    double tStart = (std::max)(0.0, (std::min)(1.0, t1));
                    double tEnd = (std::max)(0.0, (std::min)(1.0, t2));

                    if (tStart < tEnd) {
                        int ix1 = x1 + (int)(tStart * dx);
                        int iy1 = y1 + (int)(tStart * dy);
                        int ix2 = x1 + (int)(tEnd * dx);
                        int iy2 = y1 + (int)(tEnd * dy);

                        // Force the drawing color to Red so it's visible against the window
                        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
                        SelectObject(hdc, hPen);
                        
                        MoveToEx(hdc, ix1, iy1, NULL);
                        LineTo(hdc, ix2, iy2);
                        
                        DeleteObject(hPen);
                        printf(">> Line Clipped at (%d,%d)\n", ix1, iy1);
                    }
                }
            } else {
                // Rectangle/Square Clipping
                ClipLineRect(hdc, x1, y1, x2, y2, xmin, ymin, xmax, ymax);
            }
        }

        // --- CASE C: POLYGON CLIPPING (5+ clicks, Rect/Square Only) ---
        else if (g_mouseClicks.size() >= 5 && g_currentSelection != IDM_CLIP_CIRCLE) {
            std::vector<Point> vlist;
            for (size_t i = 2; i < g_mouseClicks.size(); i++) {
                Point tempP; tempP.x = (double)g_mouseClicks[i].x; tempP.y = (double)g_mouseClicks[i].y;
                vlist.push_back(tempP);
            }

            int edges[] = { xmin, ymin, xmax, ymax };
            for (int i = 0; i < 4; i++) {
                std::vector<Point> nextList;
                if (vlist.empty()) break;
                Point v1 = vlist.back();
                for (Point v2 : vlist) {
                    bool v1_in, v2_in; Point inter;
                    if (i == 0) { v1_in = v1.x >= edges[i]; v2_in = v2.x >= edges[i]; }
                    else if (i == 1) { v1_in = v1.y >= edges[i]; v2_in = v2.y >= edges[i]; }
                    else if (i == 2) { v1_in = v1.x <= edges[i]; v2_in = v2.x <= edges[i]; }
                    else { v1_in = v1.y <= edges[i]; v2_in = v2.y <= edges[i]; }

                    if (v1_in != v2_in) {
                        if (i == 0 || i == 2) { 
                            inter.x = edges[i]; 
                            inter.y = v1.y + (edges[i] - v1.x) * (v2.y - v1.y) / (v2.x - v1.x); 
                        } else { 
                            inter.y = edges[i]; 
                            inter.x = v1.x + (edges[i] - v1.y) * (v2.x - v1.x) / (v2.y - v1.y); 
                        }
                    }
                    if (!v1_in && v2_in) { nextList.push_back(inter); nextList.push_back(v2); }
                    else if (v1_in && v2_in) nextList.push_back(v2);
                    else if (v1_in && !v2_in) nextList.push_back(inter);
                    v1 = v2;
                }
                vlist = nextList;
            }
            if (!vlist.empty()) {
                Point p1 = vlist.back();
                for (const auto& p2 : vlist) {
                    MoveToEx(hdc, (int)p1.x, (int)p1.y, NULL);
                    LineTo(hdc, (int)p2.x, (int)p2.y);
                    p1 = p2;
                }
            }
            printf(">> Polygon Clipped.\n");
        }

        g_mouseClicks.clear(); 
        ReleaseDC(hwnd, hdc);
    }
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
    AllocConsole(); 
    freopen("CONOUT$", "w", stdout); 
    printf("Console Initialized. Graphics Project Running...\n");
    
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
