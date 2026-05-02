// task1_file_menu.cpp
// Task 1 – File Menu: Clear / Save / Load shapes
#include "task1_file_menu.h"
#include <commdlg.h>   // GetSaveFileName / GetOpenFileName
#include <fstream>
#include <sstream>
#include "task5_ellipse_algorithms.h"
#pragma comment(lib, "comdlg32.lib")

// Definition of the global shape list shared across the application.
std::vector<ShapeRecord> g_shapes;

// -------------------------------------------------------
// Helper: build the OPENFILENAME structure used by both
//         the Save and Open dialogs.
// -------------------------------------------------------
static OPENFILENAME BuildOFN(HWND hwnd, wchar_t* szFile, bool forSave)
{
    OPENFILENAME ofn{};
    ofn.lStructSize  = sizeof(ofn);
    ofn.hwndOwner    = hwnd;
    ofn.lpstrFile    = szFile;
    ofn.nMaxFile     = MAX_PATH;
    ofn.lpstrFilter  = L"Shape Files (*.shp)\0*.shp\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt  = L"shp";
    ofn.Flags        = forSave
                         ? (OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT)
                         : (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
    return ofn;
}

// -------------------------------------------------------
// Helper: map ShapeType enum ↔ string token
// -------------------------------------------------------
static const char* ShapeTypeToStr(ShapeType t)
{
    switch (t) { 
        case ShapeType::ELLIPSE_DIRECT:   return "ELLIPSE_DIRECT"; 
        case ShapeType::ELLIPSE_POLAR:    return "ELLIPSE_POLAR"; 
        case ShapeType::ELLIPSE_MIDPOINT: return "ELLIPSE_MIDPOINT"; 
    }
    return "UNKNOWN";
}
static ShapeType StrToShapeType(const std::string& s)
{
    if (s == "ELLIPSE_DIRECT")   return ShapeType::ELLIPSE_DIRECT;
    if (s == "ELLIPSE_POLAR")    return ShapeType::ELLIPSE_POLAR;
    if (s == "ELLIPSE_MIDPOINT") return ShapeType::ELLIPSE_MIDPOINT;
    return ShapeType::ELLIPSE_DIRECT; // default / fallback
}

// -------------------------------------------------------
// RedrawShapes – called from WM_PAINT after invalidation
// -------------------------------------------------------
void RedrawShapes(HDC hdc)
{
    for (const auto& sh : g_shapes)
    {
        int cx = (sh.x1 + sh.x2) / 2;
        int cy = (sh.y1 + sh.y2) / 2;
        int a = abs(sh.x2 - sh.x1) / 2;
        int b = abs(sh.y2 - sh.y1) / 2;

        switch (sh.type)
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
    }
}

// -------------------------------------------------------
// a) Clear – remove all shapes and repaint
// -------------------------------------------------------
void ClearScreen(HWND hwnd)
{
    g_shapes.clear();
    InvalidateRect(hwnd, NULL, TRUE);   // erase background + repaint
    UpdateWindow(hwnd);
}

// -------------------------------------------------------
// b) Save – serialise g_shapes to a user-chosen file
// Format per line:  <type> <x1> <y1> <x2> <y2>
// -------------------------------------------------------
void SaveToFile(HWND hwnd)
{
    wchar_t szFile[MAX_PATH] = {};
    OPENFILENAME ofn = BuildOFN(hwnd, szFile, /*forSave=*/true);

    if (!GetSaveFileName(&ofn))
        return;  // user cancelled

    // Convert wchar path to narrow string for fstream
    char narrowPath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, szFile, -1, narrowPath, MAX_PATH, NULL, NULL);

    std::ofstream ofs(narrowPath);
    if (!ofs)
    {
        MessageBox(hwnd, L"Could not open file for writing.", L"Save Error", MB_ICONERROR);
        return;
    }

    for (const auto& sh : g_shapes)
        ofs << ShapeTypeToStr(sh.type)
            << " " << sh.x1 << " " << sh.y1
            << " " << sh.x2 << " " << sh.y2 << "\n";

    ofs.close();
    MessageBox(hwnd, L"Shapes saved successfully.", L"Save", MB_ICONINFORMATION);
}

// -------------------------------------------------------
// c) Load – deserialise shapes from a user-chosen file
// -------------------------------------------------------
void LoadFromFile(HWND hwnd)
{
    wchar_t szFile[MAX_PATH] = {};
    OPENFILENAME ofn = BuildOFN(hwnd, szFile, /*forSave=*/false);

    if (!GetOpenFileName(&ofn))
        return;  // user cancelled

    char narrowPath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, szFile, -1, narrowPath, MAX_PATH, NULL, NULL);

    std::ifstream ifs(narrowPath);
    if (!ifs)
    {
        MessageBox(hwnd, L"Could not open file for reading.", L"Load Error", MB_ICONERROR);
        return;
    }

    g_shapes.clear();

    std::string line;
    while (std::getline(ifs, line))
    {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string token;
        ShapeRecord rec{};

        ss >> token >> rec.x1 >> rec.y1 >> rec.x2 >> rec.y2;
        rec.type = StrToShapeType(token);
        g_shapes.push_back(rec);
    }

    // Repaint with loaded shapes
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}
