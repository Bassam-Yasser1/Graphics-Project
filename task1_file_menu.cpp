#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "task1_file_menu.h"
#include <commdlg.h>   
#include <cstdio>
#include <fstream>
#include <sstream>
#include "task5_ellipse_algorithms.h"
#include "Circles.h"
#include "Lines.h"
#include "Filling.h"
#include "Clipping.h"
#include "Curves.h"
#include "Preferences.h"
#include "SmileyFace.h"

#pragma comment(lib, "comdlg32.lib")

// global shape list
std::vector<ShapeRecord> g_shapes;

// OPENFILENAME struct for file dialogs, with appropriate flags for save vs open
static OPENFILENAME BuildOFN(HWND hwnd, wchar_t* szFile, bool forSave)
{
    OPENFILENAME ofn{}; // zero-initialize the struct
    ofn.lStructSize  = sizeof(ofn); // حجم ال struct
    ofn.hwndOwner    = hwnd;
    ofn.lpstrFile    = szFile; // buffer to receive the file path
    ofn.nMaxFile     = MAX_PATH; // maximum size of the file path
    ofn.lpstrFilter  = L"Shape Files (*.shp)\0*.shp\0All Files (*.*)\0*.*\0"; // filter for file types
    ofn.nFilterIndex = 1; // default to the first filter (Shape Files)
    ofn.lpstrDefExt  = L"shp"; // default extension if user doesn't specify
    ofn.Flags        = forSave // different flags for save vs open: save should prompt to overwrite, open should require existing file
                         ? (OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT)
                         : (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
    return ofn;
}

// convert between ShapeType enum and string for file I/O
// عشان لما يتعمل سيف يبقي بشكل سنرينج مفهوم مش رقم، ولما يتعمل لود نقدر نرجع من السترينج للنوع الصحيح
static const char* ShapeTypeToStr(ShapeType t)
{
    switch(t){
    case ShapeType::ELLIPSE_DIRECT:    return "ELLIPSE_DIRECT";
    case ShapeType::ELLIPSE_POLAR:     return "ELLIPSE_POLAR";
    case ShapeType::ELLIPSE_MIDPOINT:  return "ELLIPSE_MIDPOINT";
    case ShapeType::CIRCLE_DIRECT:     return "CIRCLE_DIRECT";
    case ShapeType::CIRCLE_POLAR:      return "CIRCLE_POLAR";
    case ShapeType::CIRCLE_IPOLAR:     return "CIRCLE_IPOLAR";
    case ShapeType::CIRCLE_MIDPOINT:   return "CIRCLE_MIDPOINT";
    case ShapeType::CIRCLE_MOD_MID:    return "CIRCLE_MOD_MID";
    case ShapeType::LINE_DDA:          return "LINE_DDA";
    case ShapeType::LINE_MIDPOINT:     return "LINE_MIDPOINT";
    case ShapeType::LINE_PARAMETRIC:   return "LINE_PARAMETRIC";
    case ShapeType::FILL_CIRC_LINES:     return "FILL_CIRC_LINES";
    case ShapeType::FILL_CIRC_CIRCLES:   return "FILL_CIRC_CIRCLES";
    case ShapeType::FILL_SQ_HERMITE:     return "FILL_SQ_HERMITE";
    case ShapeType::FILL_RECT_BEZIER:    return "FILL_RECT_BEZIER";
    case ShapeType::FILL_FLOOD_RECURSIVE:return "FILL_FLOOD_RECURSIVE";
    case ShapeType::FILL_FLOOD_ITERATIVE:return "FILL_FLOOD_ITERATIVE";
    case ShapeType::FACE_HAPPY:          return "FACE_HAPPY";
    case ShapeType::FACE_SAD:            return "FACE_SAD";
    case ShapeType::CONVEX_POLYGON:      return "CONVEX_POLYGON";
    case ShapeType::NONCONVEX_POLYGON:   return "NONCONVEX_POLYGON";
    case ShapeType::CARDINAL_SPLINE:     return "CARDINAL_SPLINE";
    case ShapeType::CLIP_POINT_RECT:     return "CLIP_POINT_RECT";
    case ShapeType::CLIP_LINE_RECT:      return "CLIP_LINE_RECT";
    case ShapeType::CLIP_POINT_CIRCLE:   return "CLIP_POINT_CIRCLE";
    case ShapeType::CLIP_LINE_CIRCLE:    return "CLIP_LINE_CIRCLE";
    case ShapeType::CLIP_POLYGON_RECT:   return "CLIP_POLYGON_RECT";

    default:                           return "UNKNOWN";
    }
}
    
// convert from string to ShapeType, returns ELLIPSE_DIRECT as default if no match
static ShapeType StrToShapeType(const std::string& s)
{
    if (s == "ELLIPSE_DIRECT")      return ShapeType::ELLIPSE_DIRECT;
    if (s == "ELLIPSE_POLAR")       return ShapeType::ELLIPSE_POLAR;
    if (s == "ELLIPSE_MIDPOINT")    return ShapeType::ELLIPSE_MIDPOINT;
    if (s == "CIRCLE_DIRECT")       return ShapeType::CIRCLE_DIRECT;
    if (s == "CIRCLE_POLAR")        return ShapeType::CIRCLE_POLAR;
    if (s == "CIRCLE_IPOLAR")       return ShapeType::CIRCLE_IPOLAR;
    if (s == "CIRCLE_MIDPOINT")     return ShapeType::CIRCLE_MIDPOINT;
    if (s == "CIRCLE_MOD_MID")      return ShapeType::CIRCLE_MOD_MID;
    if (s == "LINE_DDA")            return ShapeType::LINE_DDA;
    if (s == "LINE_MIDPOINT")       return ShapeType::LINE_MIDPOINT;
    if (s == "LINE_PARAMETRIC")     return ShapeType::LINE_PARAMETRIC;
    if (s == "FILL_CIRC_LINES")     return ShapeType::FILL_CIRC_LINES;
    if (s == "FILL_CIRC_CIRCLES")   return ShapeType::FILL_CIRC_CIRCLES;
    if (s == "FILL_SQ_HERMITE")     return ShapeType::FILL_SQ_HERMITE;
    if (s == "FILL_RECT_BEZIER")    return ShapeType::FILL_RECT_BEZIER;
    if (s == "FILL_FLOOD_RECURSIVE")return ShapeType::FILL_FLOOD_RECURSIVE;
    if (s == "FILL_FLOOD_ITERATIVE")return ShapeType::FILL_FLOOD_ITERATIVE;
    if (s == "FACE_HAPPY")          return ShapeType::FACE_HAPPY;
    if (s == "FACE_SAD")            return ShapeType::FACE_SAD;
    if (s == "CLIP_POINT_RECT")     return ShapeType::CLIP_POINT_RECT;
    if (s == "CLIP_LINE_RECT")      return ShapeType::CLIP_LINE_RECT;
    if (s == "CLIP_POINT_CIRCLE")   return ShapeType::CLIP_POINT_CIRCLE;
    if (s == "CLIP_LINE_CIRCLE")    return ShapeType::CLIP_LINE_CIRCLE;
    if (s == "CLIP_POLYGON_RECT")   return ShapeType::CLIP_POLYGON_RECT;
    if (s == "CONVEX_POLYGON")      return ShapeType::CONVEX_POLYGON;
    if (s == "NONCONVEX_POLYGON")   return ShapeType::NONCONVEX_POLYGON;
    if (s == "CARDINAL_SPLINE")     return ShapeType::CARDINAL_SPLINE;
    return ShapeType::ELLIPSE_DIRECT; // default / fallback
}

// redraws all shapes in g_shapes onto the given device context, called from WM_PAINT after invalidation
void RedrawShapes(HDC hdc)
{
    for (const auto& sh : g_shapes)
    {
        int cx = (sh.x1 + sh.x2) / 2;
        int cy = (sh.y1 + sh.y2) / 2;
        int a  = abs(sh.x2 - sh.x1) / 2;
        int b  = abs(sh.y2 - sh.y1) / 2;
        int R  = a; // for circles x1=cx, y1=cy, x2=cx+R, y2=cy

// Note: for some shapes (like lines) the x1,y1,x2,y2 fields are used differently, but it doesn't matter since each shape type only uses the fields in its own way
        switch (sh.type)
        {
        case ShapeType::ELLIPSE_DIRECT:   DrawEllipseDirect   (hdc,cx,cy,a,b,sh.color); break;
        case ShapeType::ELLIPSE_POLAR:    DrawEllipsePolar    (hdc,cx,cy,a,b,sh.color); break;
        case ShapeType::ELLIPSE_MIDPOINT: DrawEllipseMidpoint (hdc,cx,cy,a,b,sh.color); break;
        case ShapeType::CIRCLE_DIRECT:    CircleDirect        (hdc,sh.x1,sh.y1,sh.x2,sh.color); break;
        case ShapeType::CIRCLE_POLAR:     CirclePolar         (hdc,sh.x1,sh.y1,sh.x2,sh.color); break;
        case ShapeType::CIRCLE_IPOLAR:    CircleIterativePolar(hdc,sh.x1,sh.y1,sh.x2,sh.color); break;
        case ShapeType::CIRCLE_MIDPOINT:  CircleMidpoint      (hdc,sh.x1,sh.y1,sh.x2,sh.color); break;
        case ShapeType::CIRCLE_MOD_MID:   CircleModifiedMidpoint(hdc,sh.x1,sh.y1,sh.x2,sh.color); break;
        case ShapeType::LINE_DDA:         DrawLineDDA         (hdc,sh.x1,sh.y1,sh.x2,sh.y2,sh.color); break;
        case ShapeType::LINE_MIDPOINT:    DrawLineMidpoint    (hdc,sh.x1,sh.y1,sh.x2,sh.y2,sh.color); break;
        case ShapeType::LINE_PARAMETRIC:  DrawLineParametric  (hdc,sh.x1,sh.y1,sh.x2,sh.y2,sh.color); break;
        case ShapeType::FILL_CIRC_LINES:    FillCircleWithLines  (hdc, sh.x1, sh.y1, sh.x2, sh.y2, sh.color); break;
        case ShapeType::FILL_CIRC_CIRCLES:  FillCircleWithCircles(hdc, sh.x1, sh.y1, sh.x2, sh.y2, sh.color); break;
        case ShapeType::FILL_SQ_HERMITE:     FillSquareHermite   (hdc, sh.x1, sh.y1, sh.x2, sh.color); break;
        case ShapeType::FILL_RECT_BEZIER:    FillRectBezier      (hdc, sh.x1, sh.y1, sh.x2, sh.y2, sh.color); break;
        case ShapeType::FILL_FLOOD_RECURSIVE: FloodFillRecursive (hdc, sh.x1, sh.y1, sh.color, sh.x2); break;
        case ShapeType::FILL_FLOOD_ITERATIVE: FloodFillIterative (hdc, sh.x1, sh.y1, sh.color, sh.x2); break;
        case ShapeType::FACE_HAPPY:          DrawSmiley          (hdc, sh.x1, sh.y1, true); break;
        case ShapeType::FACE_SAD:            DrawSmiley          (hdc, sh.x1, sh.y1, false); break;
        case ShapeType::CONVEX_POLYGON:
        {// for polygons, the points vector contains the vertices, and x1,y1,x2,y2 are not used except maybe for bounding box info
            std::vector<FillPoint> poly;
            for (auto& p : sh.points)
                poly.push_back({p.first, p.second});
            FillConvexPolygon(hdc, poly, sh.color);
            MoveToEx(hdc, poly.back().x, poly.back().y, NULL);
            for (auto& p : poly) LineTo(hdc, p.x, p.y);
            break;
        }
        // for non-convex polygons store the vertices in points vector, and x1,y1,x2,y2 are not used except maybe for bounding box info
        case ShapeType::NONCONVEX_POLYGON:
        {
            std::vector<FillPoint> poly;
            for (auto& p : sh.points)
                poly.push_back({p.first, p.second});
            FillNonConvexPolygon(hdc, poly, sh.color);
            MoveToEx(hdc, poly.back().x, poly.back().y, NULL);
            for (auto& p : poly) LineTo(hdc, p.x, p.y);
            break;
        }
        // for cardinal spline, the control points are stored in the points vector, and x1,y1,x2,y2 are not used except maybe for bounding box info
        case ShapeType::CARDINAL_SPLINE:
        {
            std::vector<CurvePoint> cpts;
            for (auto& p : sh.points)
                cpts.push_back({p.first, p.second});
            DrawCardinalSpline(hdc, cpts, 0.0, 60, sh.color);
            break;
        }
        // for clipping shapes, the points vector contains the relevant points (like line endpoints or polygon vertices), and x1,y1,x2,y2 are used for clipping window or circle parameters
        case ShapeType::CLIP_POINT_RECT:
            ClipPointRect(hdc, sh.x1, sh.y1,
                          sh.x2, sh.y2,
                          sh.points[0].first, sh.points[0].second,
                          sh.color);
            break;
        case ShapeType::CLIP_POINT_CIRCLE:
            ClipPointCircle(hdc, sh.x1, sh.y1,
                           sh.x2, sh.y2,
                           sh.points[0].first,
                           sh.color);
            break;
        case ShapeType::CLIP_LINE_RECT:
            ClipLineRect(hdc, sh.x1, sh.y1, sh.x2, sh.y2,
                         sh.points[0].first, sh.points[0].second,
                         sh.points[1].first, sh.points[1].second);
            break;

        case ShapeType::CLIP_LINE_CIRCLE:
            ClipLineCircle(hdc, sh.x1, sh.y1, sh.x2, sh.y2,
                           sh.points[0].first, sh.points[0].second,
                           sh.points[1].first); // R stored in points[1].first
            break;

        case ShapeType::CLIP_POLYGON_RECT:
        {
            PointList poly;
            for (auto& p : sh.points)
                poly.push_back({p.first, p.second});
            // x1,y1 = window min, x2,y2 = window max
            PolygonClip(hdc, poly, sh.x1, sh.y1, sh.x2, sh.y2);
            break;
        }
        }
    }
}

//clear
void ClearScreen(HWND hwnd)
{
    g_shapes.clear();
    InvalidateRect(hwnd, NULL, TRUE);   // erase background + repaint
    UpdateWindow(hwnd);
}

//save
void SaveToFile(HWND hwnd)
{ // Open a Save File dialog and write the shape records to the chosen file in a simple text format.
    // Each line represents one shape, starting with the shape type and parameters, followed by the list of points if applicable.
    wchar_t szFile[MAX_PATH] = {}; // buffer to receive the file path
    OPENFILENAME ofn = BuildOFN(hwnd, szFile, /*forSave=*/true); 

    if (!GetSaveFileName(&ofn))
        return;  // user cancelled

    // Convert wchar path to narrow string for fstream
    // فتح الملف للكتابة و لو موجود هيعمل overwrite
    FILE* file = nullptr; 
    errno_t err = _wfopen_s(&file, szFile, L"w"); 
    if (err != 0 || !file)
    {
        MessageBox(hwnd, L"Could not open file for writing.", L"Save Error", MB_ICONERROR);
        return;
    }

    for (const auto& sh : g_shapes)
    {
        unsigned int pointCount = static_cast<unsigned int>(sh.points.size());
        fprintf(file, "%s %d %d %d %d %u %u",
                ShapeTypeToStr(sh.type), sh.x1, sh.y1, sh.x2, sh.y2,
                static_cast<unsigned int>(sh.color), pointCount);

        for (const auto& pt : sh.points)
            fprintf(file, " %d %d", pt.first, pt.second);

        fprintf(file, "\n");
    }

    fclose(file);
    MessageBox(hwnd, L"Shapes saved successfully.", L"Save", MB_ICONINFORMATION);
}

//load
void LoadFromFile(HWND hwnd)
{
    wchar_t szFile[MAX_PATH] = {};
    OPENFILENAME ofn = BuildOFN(hwnd, szFile, /*forSave=*/false);

    if (!GetOpenFileName(&ofn))
        return;  // user cancelled

    FILE* file = nullptr;
    errno_t err = _wfopen_s(&file, szFile, L"r"); // فتح الملف للقراءة
    if (err != 0 || !file)
    {
        MessageBox(hwnd, L"Could not open file for reading.", L"Load Error", MB_ICONERROR);
        return;
    }

    g_shapes.clear();

    char buffer[1024]; // buffer to read each line, assuming max line length is less than 1024 characters
    while (fgets(buffer, sizeof(buffer), file))
    {
        if (buffer[0] == '\n' || buffer[0] == '\0')
            continue;

        std::istringstream ss(buffer);
        std::string token;
        unsigned int color = 0;
        unsigned int pointCount = 0;
        ShapeRecord rec{};

        ss >> token >> rec.x1 >> rec.y1 >> rec.x2 >> rec.y2 >> color;
        if (!(ss >> pointCount))
            pointCount = 0;
        rec.type  = StrToShapeType(token);
        rec.color = static_cast<COLORREF>(color);
        rec.points.resize(pointCount);
        for (unsigned int i = 0; i < pointCount; ++i)
            ss >> rec.points[i].first >> rec.points[i].second;

        g_shapes.push_back(rec);
    }

    fclose(file);

    // Repaint with loaded shapes
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}
