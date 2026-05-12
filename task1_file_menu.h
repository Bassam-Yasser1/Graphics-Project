#pragma once
#include <windows.h>
#include <vector>
#include <string>

// Supported shape types stored in the shape list
enum class ShapeType {
    ELLIPSE_DIRECT, ELLIPSE_POLAR, ELLIPSE_MIDPOINT,
    CIRCLE_DIRECT, CIRCLE_POLAR, CIRCLE_IPOLAR, CIRCLE_MIDPOINT, CIRCLE_MOD_MID,
    LINE_DDA, LINE_MIDPOINT, LINE_PARAMETRIC,
    FILL_CIRC_LINES, FILL_CIRC_CIRCLES,
    FILL_SQ_HERMITE, FILL_RECT_BEZIER,
    FILL_FLOOD_RECURSIVE, FILL_FLOOD_ITERATIVE,
    CONVEX_POLYGON, NONCONVEX_POLYGON,
    CARDINAL_SPLINE,
    FACE_HAPPY, FACE_SAD,
    CLIP_POINT_RECT, CLIP_POINT_CIRCLE,
    CLIP_LINE_RECT,  CLIP_LINE_CIRCLE,
    CLIP_POLYGON_RECT
};

struct ShapeRecord
{
    ShapeType type;
    int x1, y1, x2, y2;   // centre & bounding box for most shapes
    COLORREF color;    
    std::vector<std::pair<int,int>> points;    
};

// Global list of all shapes drawn on the canvas
extern std::vector<ShapeRecord> g_shapes;

//clear
// Erases all shapes from the internal list and repaints the window.
void ClearScreen(HWND hwnd);

//save
void SaveToFile(HWND hwnd);

//load
void LoadFromFile(HWND hwnd);

// redraws all shapes in g_shapes onto the given device context, called from WM_PAINT after invalidation
void RedrawShapes(HDC hdc);
