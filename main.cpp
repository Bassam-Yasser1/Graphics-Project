// main.cpp
// Entry point for the Computer Graphics project.
// Links Task 1 (File Menu), Task 5 (Ellipse Algorithms),
// Lines, Filling, Curves, Clipping, SmileyFace, and Preferences.
//
// Menu layout:
//   File          | Clear / Save / Load / Exit
//   Task 5        | Direct / Polar / Midpoint Ellipse      (1 click)
//   Circles       | Direct / Polar / Iterative / Midpoint / Modified   (2 clicks)
//   Lines         | DDA / Midpoint / Parametric            (2 clicks)
//   Filling       | Circle-Lines / Circle-Circles          (1 click + quarter dialog)
//                 | Square-Hermite / Rect-Bezier           (2 clicks)
//                 | Convex / Non-Convex polygon            (N clicks + Right-click)
//                 | Flood Fill Recursive / Iterative       (1 click)
//   Curves        | Cardinal Spline                        (N clicks + Right-click)
//   Clipping      | Rectangle / Square / Circle Window
//   Smiley Faces  | Happy / Sad
//   Preferences   | White Background / Change Cursor / Choose Color

#define _UNICODE
#define UNICODE
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
#include "Lines.h"
#include "Filling.h"
#include "Curves.h"
#include "Preferences.h"

// -------------------------------------------------------
// Menu IDs
// -------------------------------------------------------
// File
#define IDM_FILE_CLEAR      1001
#define IDM_FILE_SAVE       1002
#define IDM_FILE_LOAD       1003
#define IDM_FILE_EXIT       1004

// Task 5 - Ellipses
#define IDM_T5_DIRECT       2001
#define IDM_T5_POLAR        2002
#define IDM_T5_MIDPOINT     2003

// Circles (unchanged from original)
#define IDM_CIRC_DIRECT     3001
#define IDM_CIRC_POLAR      3002
#define IDM_CIRC_IPOLAR     3003
#define IDM_CIRC_MIDPOINT   3004
#define IDM_CIRC_MOD_MID    3005

// Clipping
#define IDM_CLIP_RECT       4001
#define IDM_CLIP_SQUARE     4002
#define IDM_CLIP_CIRCLE     4003

// Smiley Faces
#define IDM_FACE_HAPPY      5001
#define IDM_FACE_SAD        5002

// Lines
#define IDM_LINE_DDA        6001
#define IDM_LINE_MIDPOINT   6002
#define IDM_LINE_PARAMETRIC 6003

// Filling
#define IDM_FILL_CIRC_LINES   7001
#define IDM_FILL_CIRC_CIRCLES 7002
#define IDM_FILL_SQ_HERMITE   7003
#define IDM_FILL_RECT_BEZIER  7004
#define IDM_FILL_CONVEX       7005
#define IDM_FILL_NONCONVEX    7006
#define IDM_FILL_FLOOD_REC    7007
#define IDM_FILL_FLOOD_ITER   7008

// Curves
#define IDM_CURVE_CARDINAL    8001

// Preferences
#define IDM_PREF_BG_WHITE       9001
#define IDM_PREF_CURSOR_ARROW   9002
#define IDM_PREF_CURSOR_CROSS   9003
#define IDM_PREF_CURSOR_HAND    9004
#define IDM_PREF_CURSOR_IBEAM   9005
#define IDM_PREF_COLOR          9006

// -------------------------------------------------------
// Global state
// -------------------------------------------------------
static int  g_currentSelection = IDM_T5_DIRECT;
static std::vector<Point> g_mouseClicks;
static ShapeType g_currentType = ShapeType::ELLIPSE_DIRECT;

// Active drawing colour - all algorithms use this
static COLORREF g_drawColor = RGB(0, 0, 0);

// Quarter for circle-fill functions (1-4)
static int g_fillQuarter = 1;

// -------------------------------------------------------
// Helper: ask fill quarter via cascaded MessageBox
// -------------------------------------------------------
static int AskFillQuarter(HWND hwnd)
{
    int choice = MessageBox(hwnd,
        L"Select filling quarter:\n"
        L"Yes    = Quarter 1 (Top-Right)\n"
        L"No     = Quarter 2 (Top-Left)\n"
        L"Cancel = Quarter 3 or 4...",
        L"Fill Quarter", MB_YESNOCANCEL | MB_ICONQUESTION);

    if      (choice == IDYES) return 1;
    else if (choice == IDNO)  return 2;
    else
    {
        int c2 = MessageBox(hwnd,
            L"Yes = Quarter 3 (Bottom-Left)\nNo  = Quarter 4 (Bottom-Right)",
            L"Fill Quarter", MB_YESNO | MB_ICONQUESTION);
        return (c2 == IDYES) ? 3 : 4;
    }
}

// -------------------------------------------------------
// CreateAppMenu
// -------------------------------------------------------
static HMENU CreateAppMenu()
{
    HMENU hMenuBar = CreateMenu();

    // --- File ---
    HMENU hFile = CreatePopupMenu();
    AppendMenu(hFile, MF_STRING,    IDM_FILE_CLEAR, L"&Clear");
    AppendMenu(hFile, MF_STRING,    IDM_FILE_SAVE,  L"&Save...");
    AppendMenu(hFile, MF_STRING,    IDM_FILE_LOAD,  L"&Load...");
    AppendMenu(hFile, MF_SEPARATOR, 0,              NULL);
    AppendMenu(hFile, MF_STRING,    IDM_FILE_EXIT,  L"E&xit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, L"&File");

    // --- Task 5 - Ellipses ---
    HMENU hT5 = CreatePopupMenu();
    AppendMenu(hT5, MF_STRING, IDM_T5_DIRECT,   L"Select Direct Ellipse");
    AppendMenu(hT5, MF_STRING, IDM_T5_POLAR,    L"Select Polar Ellipse");
    AppendMenu(hT5, MF_STRING, IDM_T5_MIDPOINT, L"Select Midpoint Ellipse");
    CheckMenuItem(hT5, IDM_T5_DIRECT, MF_CHECKED);
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hT5, L"Ellipses");

    // --- Circles (identical to original) ---
    HMENU hCirc = CreatePopupMenu();
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_DIRECT,   L"Direct Circle");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_POLAR,    L"Polar Circle");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_IPOLAR,   L"Iterative Polar");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_MIDPOINT, L"Midpoint Circle");
    AppendMenu(hCirc, MF_STRING, IDM_CIRC_MOD_MID,  L"Modified Midpoint");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hCirc, L"&Circles");

    // --- Lines ---
    HMENU hLine = CreatePopupMenu();
    AppendMenu(hLine, MF_STRING, IDM_LINE_DDA,        L"DDA Line");
    AppendMenu(hLine, MF_STRING, IDM_LINE_MIDPOINT,   L"Midpoint Line");
    AppendMenu(hLine, MF_STRING, IDM_LINE_PARAMETRIC, L"Parametric Line");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hLine, L"&Lines");

    // --- Filling ---
    HMENU hFill = CreatePopupMenu();
    AppendMenu(hFill, MF_STRING,    IDM_FILL_CIRC_LINES,   L"Fill Circle with Lines (Quarter)");
    AppendMenu(hFill, MF_STRING,    IDM_FILL_CIRC_CIRCLES, L"Fill Circle with Circles (Quarter)");
    AppendMenu(hFill, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFill, MF_STRING,    IDM_FILL_SQ_HERMITE,   L"Fill Square - Hermite [Vertical]");
    AppendMenu(hFill, MF_STRING,    IDM_FILL_RECT_BEZIER,  L"Fill Rectangle - Bezier [Horizontal]");
    AppendMenu(hFill, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFill, MF_STRING,    IDM_FILL_CONVEX,       L"Convex Polygon Fill");
    AppendMenu(hFill, MF_STRING,    IDM_FILL_NONCONVEX,    L"Non-Convex Polygon Fill");
    AppendMenu(hFill, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFill, MF_STRING,    IDM_FILL_FLOOD_REC,    L"Flood Fill - Recursive");
    AppendMenu(hFill, MF_STRING,    IDM_FILL_FLOOD_ITER,   L"Flood Fill - Iterative (Stack)");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFill, L"Fi&lling");

    // --- Curves ---
    HMENU hCurv = CreatePopupMenu();
    AppendMenu(hCurv, MF_STRING, IDM_CURVE_CARDINAL, L"Cardinal Spline");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hCurv, L"C&urves");

    // --- Clipping ---
    HMENU hClip = CreatePopupMenu();
    AppendMenu(hClip, MF_STRING, IDM_CLIP_RECT,   L"Rectangle Window");
    AppendMenu(hClip, MF_STRING, IDM_CLIP_SQUARE, L"Square Window");
    AppendMenu(hClip, MF_STRING, IDM_CLIP_CIRCLE, L"Circle Window (Bonus)");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hClip, L"Cli&pping");

    // --- Smiley Faces ---
    HMENU hFace = CreatePopupMenu();
    AppendMenu(hFace, MF_STRING, IDM_FACE_HAPPY, L"Happy Face");
    AppendMenu(hFace, MF_STRING, IDM_FACE_SAD,   L"Sad Face");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFace, L"&Smiley Faces");

    // --- Preferences (NEW) ---
    HMENU hPref = CreatePopupMenu();
    AppendMenu(hPref, MF_STRING,    IDM_PREF_BG_WHITE, L"White Background");
    AppendMenu(hPref, MF_SEPARATOR, 0, NULL);
    // Cursor sub-menu
    HMENU hCursor = CreatePopupMenu();
    AppendMenu(hCursor, MF_STRING, IDM_PREF_CURSOR_ARROW, L"Arrow");
    AppendMenu(hCursor, MF_STRING, IDM_PREF_CURSOR_CROSS, L"Cross");
    AppendMenu(hCursor, MF_STRING, IDM_PREF_CURSOR_HAND,  L"Hand");
    AppendMenu(hCursor, MF_STRING, IDM_PREF_CURSOR_IBEAM, L"I-Beam");
    AppendMenu(hPref, MF_POPUP, (UINT_PTR)hCursor, L"Change Cursor");
    AppendMenu(hPref, MF_SEPARATOR, 0, NULL);
    AppendMenu(hPref, MF_STRING,    IDM_PREF_COLOR, L"Choose Drawing Color...");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hPref, L"&Preferences");

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
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        RedrawShapes(hdc);
        EndPaint(hwnd, &ps);
        break;
    }

    // ---- Left click ----------------------------------------
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lp);
        int my = HIWORD(lp);
        hdc = GetDC(hwnd);

        Point p;
        p.x = (double)mx;
        p.y = (double)my;
        g_mouseClicks.push_back(p);

        // ---- TASK 5: ELLIPSES (2 clicks: centre then radius point) ----
        if (g_currentSelection >= 2001 && g_currentSelection <= 2003)
        {
            if (g_mouseClicks.size() == 2)
            {
                int a = (int)abs(g_mouseClicks[1].x - g_mouseClicks[0].x); // horizontal radius
                int b = (int)abs(g_mouseClicks[1].y - g_mouseClicks[0].y); // vertical radius
                int cx = (int)g_mouseClicks[0].x;
                int cy = (int)g_mouseClicks[0].y;

                if (a < 1) a = 1;
                if (b < 1) b = 1;

                ShapeRecord rec;
                rec.type = g_currentType;
                rec.x1 = cx - a; rec.y1 = cy - b;
                rec.x2 = cx + a; rec.y2 = cy + b;
                g_shapes.push_back(rec);

                if (g_currentSelection == IDM_T5_DIRECT)   DrawEllipseDirect  (hdc, cx, cy, a, b, g_drawColor);
                if (g_currentSelection == IDM_T5_POLAR)    DrawEllipsePolar   (hdc, cx, cy, a, b, g_drawColor);
                if (g_currentSelection == IDM_T5_MIDPOINT) DrawEllipseMidpoint(hdc, cx, cy, a, b, g_drawColor);
                g_mouseClicks.clear();
            }
        }

        // ---- CIRCLES: 2 clicks (centre, radius point) ------
        // Preserved exactly from the original main.cpp
        else if (g_currentSelection >= 3001 && g_currentSelection <= 3005)
        {
            if (g_mouseClicks.size() == 2)
            {
                int R  = (int)sqrt(pow(g_mouseClicks[1].x - g_mouseClicks[0].x, 2) +
                                   pow(g_mouseClicks[1].y - g_mouseClicks[0].y, 2));
                int xc = (int)g_mouseClicks[0].x;
                int yc = (int)g_mouseClicks[0].y;

                if (g_currentSelection == IDM_CIRC_DIRECT)   CircleDirect          (hdc, xc, yc, R, g_drawColor);
                if (g_currentSelection == IDM_CIRC_POLAR)    CirclePolar           (hdc, xc, yc, R, g_drawColor);
                if (g_currentSelection == IDM_CIRC_MIDPOINT) CircleMidpoint        (hdc, xc, yc, R, g_drawColor);
                if (g_currentSelection == IDM_CIRC_IPOLAR)   CircleIterativePolar  (hdc, xc, yc, R, g_drawColor);
                if (g_currentSelection == IDM_CIRC_MOD_MID)  CircleModifiedMidpoint(hdc, xc, yc, R, g_drawColor);
                g_mouseClicks.clear();
            }
        }

        // ---- LINES: 2 clicks (P1, P2) ----------------------
        else if (g_currentSelection >= 6001 && g_currentSelection <= 6003)
        {
            if (g_mouseClicks.size() == 2)
            {
                int x1 = (int)g_mouseClicks[0].x, y1 = (int)g_mouseClicks[0].y;
                int x2 = (int)g_mouseClicks[1].x, y2 = (int)g_mouseClicks[1].y;

                if (g_currentSelection == IDM_LINE_DDA)        DrawLineDDA       (hdc, x1, y1, x2, y2, g_drawColor);
                if (g_currentSelection == IDM_LINE_MIDPOINT)   DrawLineMidpoint  (hdc, x1, y1, x2, y2, g_drawColor);
                if (g_currentSelection == IDM_LINE_PARAMETRIC) DrawLineParametric(hdc, x1, y1, x2, y2, g_drawColor);
                g_mouseClicks.clear();
            }
        }

        // ---- FILL CIRCLE WITH LINES: 1 click ---------------
        else if (g_currentSelection == IDM_FILL_CIRC_LINES)
        {
            ReleaseDC(hwnd, hdc);
            g_fillQuarter = AskFillQuarter(hwnd);
            hdc = GetDC(hwnd);
            FillCircleWithLines(hdc, mx, my, 80, g_fillQuarter, g_drawColor);
            g_mouseClicks.clear();
        }

        // ---- FILL CIRCLE WITH CIRCLES: 1 click -------------
        else if (g_currentSelection == IDM_FILL_CIRC_CIRCLES)
        {
            ReleaseDC(hwnd, hdc);
            g_fillQuarter = AskFillQuarter(hwnd);
            hdc = GetDC(hwnd);
            FillCircleWithCircles(hdc, mx, my, 80, g_fillQuarter, g_drawColor);
            g_mouseClicks.clear();
        }

        // ---- FILL SQUARE HERMITE: 2 clicks (TL, BR) --------
        else if (g_currentSelection == IDM_FILL_SQ_HERMITE)
        {
            if (g_mouseClicks.size() == 2)
            {
                int x0   = (int)(std::min)(g_mouseClicks[0].x, g_mouseClicks[1].x);
                int y0   = (int)(std::min)(g_mouseClicks[0].y, g_mouseClicks[1].y);
                int side = (int)(std::max)(
                    std::abs(g_mouseClicks[1].x - g_mouseClicks[0].x),
                    std::abs(g_mouseClicks[1].y - g_mouseClicks[0].y));
                FillSquareHermite(hdc, x0, y0, side, g_drawColor);
                HBRUSH hOld = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
                Rectangle(hdc, x0, y0, x0 + side, y0 + side);
                SelectObject(hdc, hOld);
                g_mouseClicks.clear();
            }
        }

        // ---- FILL RECT BEZIER: 2 clicks (TL, BR) -----------
        else if (g_currentSelection == IDM_FILL_RECT_BEZIER)
        {
            if (g_mouseClicks.size() == 2)
            {
                int x0 = (int)(std::min)(g_mouseClicks[0].x, g_mouseClicks[1].x);
                int y0 = (int)(std::min)(g_mouseClicks[0].y, g_mouseClicks[1].y);
                int w  = (int)std::abs(g_mouseClicks[1].x - g_mouseClicks[0].x);
                int h  = (int)std::abs(g_mouseClicks[1].y - g_mouseClicks[0].y);
                FillRectBezier(hdc, x0, y0, w, h, g_drawColor);
                HBRUSH hOld = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
                Rectangle(hdc, x0, y0, x0 + w, y0 + h);
                SelectObject(hdc, hOld);
                g_mouseClicks.clear();
            }
        }

        // ---- CONVEX / NON-CONVEX: collect vertices ---------
        // Right-click triggers the actual fill
        else if (g_currentSelection == IDM_FILL_CONVEX ||
                 g_currentSelection == IDM_FILL_NONCONVEX)
        {
            Ellipse(hdc, mx - 3, my - 3, mx + 3, my + 3); // vertex dot feedback
        }

        // ---- FLOOD FILL: 1 click (seed point) --------------
        else if (g_currentSelection == IDM_FILL_FLOOD_REC ||
                 g_currentSelection == IDM_FILL_FLOOD_ITER)
        {
            // Sample the color at the seed point - this is what gets replaced
            COLORREF targetColor = GetPixel(hdc, mx, my);
            // Don't fill if we clicked on the border or already filled area
            if (targetColor != g_drawColor)
            {
                if (g_currentSelection == IDM_FILL_FLOOD_REC)
                    FloodFillRecursive(hdc, mx, my, g_drawColor, targetColor);
                else
                    FloodFillIterative(hdc, mx, my, g_drawColor, targetColor);
            }
            g_mouseClicks.clear();
        }

        // ---- CARDINAL SPLINE: collect control points -------
        // Right-click triggers draw
        else if (g_currentSelection == IDM_CURVE_CARDINAL)
        {
            Ellipse(hdc, mx - 3, my - 3, mx + 3, my + 3); // control point dot
        }

        // ---- CLIPPING (unchanged from original) ------------
        else if (g_currentSelection >= 4001 && g_currentSelection <= 4003)
        {
            if (g_mouseClicks.size() == 2)
            {
                int xmin = (int)(std::min)(g_mouseClicks[0].x, g_mouseClicks[1].x);
                int ymin = (int)(std::min)(g_mouseClicks[0].y, g_mouseClicks[1].y);
                int xmax = (int)(std::max)(g_mouseClicks[0].x, g_mouseClicks[1].x);
                int ymax = (int)(std::max)(g_mouseClicks[0].y, g_mouseClicks[1].y);

                if (g_currentSelection == IDM_CLIP_RECT)
                {
                    Rectangle(hdc, xmin, ymin, xmax, ymax);
                }
                else if (g_currentSelection == IDM_CLIP_SQUARE)
                {
                    int side = (std::max)(abs(xmax - xmin), abs(ymax - ymin));
                    Rectangle(hdc, xmin, ymin, xmin + side, ymin + side);
                }
                else if (g_currentSelection == IDM_CLIP_CIRCLE)
                {
                    int R = (int)sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2));
                    Ellipse(hdc, xmin - R, ymin - R, xmin + R, ymin + R);
                }
                printf(">> Clipping window set. Click shape points, then Right-click to clip.\n");
            }
        }

        // ---- SMILEY FACES: 1 click -------------------------
        else if (g_currentSelection == IDM_FACE_HAPPY ||
                 g_currentSelection == IDM_FACE_SAD)
        {
            DrawSmiley(hdc, mx, my, (g_currentSelection == IDM_FACE_HAPPY));
            g_mouseClicks.clear();
        }

        ReleaseDC(hwnd, hdc);
        break;
    }

    // ---- Right click: finalise multi-click operations ------
    case WM_RBUTTONDOWN:
    {
        hdc = GetDC(hwnd);

        // Cardinal Spline: draw when >= 2 points collected
        if (g_currentSelection == IDM_CURVE_CARDINAL && g_mouseClicks.size() >= 2)
        {
            std::vector<CurvePoint> cpts;
            for (auto& p : g_mouseClicks)
                cpts.push_back({ (int)p.x, (int)p.y });
            DrawCardinalSpline(hdc, cpts, 0.0, 60, g_drawColor);
            g_mouseClicks.clear();
            printf(">> Cardinal Spline drawn through %d control points.\n", (int)cpts.size());
        }

        // Polygon fill: finalise on right-click
        else if ((g_currentSelection == IDM_FILL_CONVEX ||
                  g_currentSelection == IDM_FILL_NONCONVEX) &&
                 g_mouseClicks.size() >= 3)
        {
            std::vector<FillPoint> poly;
            for (auto& p : g_mouseClicks)
                poly.push_back({ (int)p.x, (int)p.y });

            if (g_currentSelection == IDM_FILL_CONVEX)
                FillConvexPolygon   (hdc, poly, g_drawColor);
            else
                FillNonConvexPolygon(hdc, poly, g_drawColor);

            // Draw outline over the fill
            MoveToEx(hdc, poly.back().x, poly.back().y, NULL);
            for (auto& p : poly) LineTo(hdc, p.x, p.y);

            g_mouseClicks.clear();
            printf(">> Polygon filled.\n");
        }

        // Clipping right-click handler (unchanged from original)
        else if (g_currentSelection >= 4001 && g_currentSelection <= 4003 &&
                 g_mouseClicks.size() >= 3)
        {
            int xmin = (int)(std::min)((int)g_mouseClicks[0].x, (int)g_mouseClicks[1].x);
            int ymin = (int)(std::min)((int)g_mouseClicks[0].y, (int)g_mouseClicks[1].y);
            int xmax = (int)(std::max)((int)g_mouseClicks[0].x, (int)g_mouseClicks[1].x);
            int ymax = (int)(std::max)((int)g_mouseClicks[0].y, (int)g_mouseClicks[1].y);

            if (g_currentSelection == IDM_CLIP_SQUARE)
            {
                int side = (std::max)(abs(xmax - xmin), abs(ymax - ymin));
                xmax = xmin + side; ymax = ymin + side;
            }
            int R = (int)sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2));

            // CASE A: point clipping
            if (g_mouseClicks.size() == 3)
            {
                int px = (int)g_mouseClicks[2].x;
                int py = (int)g_mouseClicks[2].y;
                bool inside = false;

                if (g_currentSelection == IDM_CLIP_CIRCLE)
                    inside = (sqrt(pow(px - xmin, 2) + pow(py - ymin, 2)) <= R);
                else
                    inside = (px >= xmin && px <= xmax && py >= ymin && py <= ymax);

                if (inside) SetPixel(hdc, px, py, RGB(255, 0, 0));
                printf(">> Point clipped.\n");
            }
            // CASE B: line clipping
            else if (g_mouseClicks.size() == 4)
            {
                int x1 = (int)g_mouseClicks[2].x, y1 = (int)g_mouseClicks[2].y;
                int x2 = (int)g_mouseClicks[3].x, y2 = (int)g_mouseClicks[3].y;

                if (g_currentSelection == IDM_CLIP_CIRCLE)
                {
                    double cx = (double)g_mouseClicks[0].x;
                    double cy = (double)g_mouseClicks[0].y;
                    double dx = (double)(x2 - x1), dy = (double)(y2 - y1);
                    double a  = dx * dx + dy * dy;
                    double b  = 2 * (dx * (x1 - cx) + dy * (y1 - cy));
                    double c  = pow(x1 - cx, 2) + pow(y1 - cy, 2) - (double)R * R;
                    double det = b * b - 4 * a * c;

                    if (det >= 0)
                    {
                        double sq  = sqrt(det);
                        double ts  = (std::max)(0.0, (std::min)(1.0, (-b - sq) / (2 * a)));
                        double te  = (std::max)(0.0, (std::min)(1.0, (-b + sq) / (2 * a)));
                        if (ts < te)
                        {
                            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
                            SelectObject(hdc, hPen);
                            MoveToEx(hdc, (int)(x1 + ts * dx), (int)(y1 + ts * dy), NULL);
                            LineTo  (hdc, (int)(x1 + te * dx), (int)(y1 + te * dy));
                            DeleteObject(hPen);
                        }
                    }
                }
                else
                {
                    ClipLineRect(hdc, x1, y1, x2, y2, xmin, ymin, xmax, ymax);
                }
            }
            // CASE C: polygon clipping
            else if (g_mouseClicks.size() >= 5 && g_currentSelection != IDM_CLIP_CIRCLE)
            {
                std::vector<Point> vlist;
                for (size_t i = 2; i < g_mouseClicks.size(); i++)
                {
                    Point tp; tp.x = g_mouseClicks[i].x; tp.y = g_mouseClicks[i].y;
                    vlist.push_back(tp);
                }

                int edges[] = { xmin, ymin, xmax, ymax };
                for (int i = 0; i < 4; i++)
                {
                    std::vector<Point> nextList;
                    if (vlist.empty()) break;
                    Point v1 = vlist.back();
                    for (Point v2 : vlist)
                    {
                        bool v1_in, v2_in; Point inter;
                        if      (i == 0) { v1_in = v1.x >= edges[i]; v2_in = v2.x >= edges[i]; }
                        else if (i == 1) { v1_in = v1.y >= edges[i]; v2_in = v2.y >= edges[i]; }
                        else if (i == 2) { v1_in = v1.x <= edges[i]; v2_in = v2.x <= edges[i]; }
                        else             { v1_in = v1.y <= edges[i]; v2_in = v2.y <= edges[i]; }

                        if (v1_in != v2_in)
                        {
                            if (i == 0 || i == 2) {
                                inter.x = edges[i];
                                inter.y = v1.y + (edges[i] - v1.x) * (v2.y - v1.y) / (v2.x - v1.x);
                            } else {
                                inter.y = edges[i];
                                inter.x = v1.x + (edges[i] - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
                            }
                        }
                        if      (!v1_in && v2_in)  { nextList.push_back(inter); nextList.push_back(v2); }
                        else if ( v1_in && v2_in)    nextList.push_back(v2);
                        else if ( v1_in && !v2_in)   nextList.push_back(inter);
                        v1 = v2;
                    }
                    vlist = nextList;
                }

                if (!vlist.empty())
                {
                    Point p1 = vlist.back();
                    for (const auto& p2 : vlist)
                    {
                        MoveToEx(hdc, (int)p1.x, (int)p1.y, NULL);
                        LineTo  (hdc, (int)p2.x, (int)p2.y);
                        p1 = p2;
                    }
                }
                printf(">> Polygon clipped.\n");
            }

            g_mouseClicks.clear();
        }

        ReleaseDC(hwnd, hdc);
        break;
    }

    // ---- Menu commands -------------------------------------
    case WM_COMMAND:
    {
        int id = LOWORD(wp);
        switch (id)
        {
        // File
        case IDM_FILE_CLEAR: ClearScreen(hwnd); break;
        case IDM_FILE_SAVE:  SaveToFile(hwnd);  break;
        case IDM_FILE_LOAD:  LoadFromFile(hwnd); break;
        case IDM_FILE_EXIT:  DestroyWindow(hwnd); break;

        // ---- Preferences -----------------------------------
        case IDM_PREF_BG_WHITE:
            PrefSetWhiteBackground(hwnd);
            printf(">> Background set to white.\n");
            break;

        case IDM_PREF_CURSOR_ARROW:
            PrefSetCursor(hwnd, IDC_ARROW);
            printf(">> Cursor: Arrow\n");
            break;

        case IDM_PREF_CURSOR_CROSS:
            PrefSetCursor(hwnd, IDC_CROSS);
            printf(">> Cursor: Cross\n");
            break;

        case IDM_PREF_CURSOR_HAND:
            PrefSetCursor(hwnd, IDC_HAND);
            printf(">> Cursor: Hand\n");
            break;

        case IDM_PREF_CURSOR_IBEAM:
            PrefSetCursor(hwnd, IDC_IBEAM);
            printf(">> Cursor: I-Beam\n");
            break;

        case IDM_PREF_COLOR:
            g_drawColor = PrefChooseColor(hwnd, g_drawColor);
            printf(">> Drawing color: R=%d G=%d B=%d\n",
                   GetRValue(g_drawColor),
                   GetGValue(g_drawColor),
                   GetBValue(g_drawColor));
            break;

        // All drawing-tool selections
        default:
            g_currentSelection = id;
            g_mouseClicks.clear();

            // Ellipse type tracking for Task 1 persistence
            if (g_currentSelection == IDM_T5_DIRECT)   g_currentType = ShapeType::ELLIPSE_DIRECT;
            if (g_currentSelection == IDM_T5_POLAR)    g_currentType = ShapeType::ELLIPSE_POLAR;
            if (g_currentSelection == IDM_T5_MIDPOINT) g_currentType = ShapeType::ELLIPSE_MIDPOINT;

            // Refresh ellipse checkmarks
            {
                HMENU hMenu = GetMenu(hwnd);
                CheckMenuItem(hMenu, IDM_T5_DIRECT,
                    (g_currentSelection == IDM_T5_DIRECT)   ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_T5_POLAR,
                    (g_currentSelection == IDM_T5_POLAR)    ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_T5_MIDPOINT,
                    (g_currentSelection == IDM_T5_MIDPOINT) ? MF_CHECKED : MF_UNCHECKED);
            }
            break;
        }
        break;
    }

    case WM_CLOSE:   DestroyWindow(hwnd); break;
    case WM_DESTROY: PostQuitMessage(0);  break;
    default:         return DefWindowProc(hwnd, mcode, wp, lp);
    }
    return 0;
}

// -------------------------------------------------------
// WinMain
// -------------------------------------------------------
int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR c, int nsh)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    printf("=== Computer Graphics Project ===\n");
    printf("Preferences menu: change background, cursor shape, or drawing color.\n");

    WNDCLASS wc{};
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance     = h;
    wc.lpfnWndProc   = WndProc;
    wc.lpszClassName = L"GraphicsProjectClass";
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        L"GraphicsProjectClass",
        L"Computer Graphics - All Tasks Integrated",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 720,
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
