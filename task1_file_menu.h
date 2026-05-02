#pragma once
#include <windows.h>
#include <vector>
#include <string>

// -------------------------------------------------------
// Task 1 – File Menu
//   a) Clear screen from shapes
//   b) Save all drawn shapes to a file
//   c) Load shapes from a file
// -------------------------------------------------------

// Supported shape types stored in the shape list
enum class ShapeType { ELLIPSE };

// A generic shape record (extend with more shape types as needed)
struct ShapeRecord
{
    ShapeType type;
    int x1, y1, x2, y2;      // bounding box / control points
};

// Global list of all shapes drawn on the canvas
extern std::vector<ShapeRecord> g_shapes;

// ---- a) Clear -------------------------------------------
// Erases all shapes from the internal list and repaints the window.
void ClearScreen(HWND hwnd);

// ---- b) Save --------------------------------------------
// Opens a Save-File dialog, then writes every shape record to the
// chosen file in a simple text format.
void SaveToFile(HWND hwnd);

// ---- c) Load --------------------------------------------
// Opens an Open-File dialog, reads shape records from the chosen
// file and repaints the window with the loaded shapes.
void LoadFromFile(HWND hwnd);

// Helper: redraws all shapes in g_shapes onto the given DC.
void RedrawShapes(HDC hdc);
