#include "Clipping.h"
#include <cmath>
#include <algorithm> 

using namespace std;

// OutCode structure used in Cohen-Sutherland line clipping algo
// Each bit represents whether the point lies outside a boundary
union OutCode {
    unsigned ALL : 4;
    struct { unsigned left : 1, right : 1, bottom : 1, top : 1; };
};

// Determines whether the point is inside or outside the clipping window
OutCode GetOutCode(int x, int y, int xmin, int ymin, int xmax, int ymax) {
    OutCode res; res.ALL = 0;
    // Check horizontal boundaries
    if (x < xmin) res.left = 1; 
    else if (x > xmax) res.right = 1;
    // Check vertical boundaries
    if (y < ymin) res.top = 1; 
    else if (y > ymax) res.bottom = 1;
    return res;
}

// Cohen-Sutherland line clipping algo
// Clips a line against a rectangular window defined by (xmin, ymin) and (xmax, ymax)
void ClipLineRect(HDC hdc, int x1, int y1, int x2, int y2, int xmin, int ymin, int xmax, int ymax) {
    
    // Compute outcodes for both endpoints
    OutCode out1 = GetOutCode(x1, y1, xmin, ymin, xmax, ymax);
    OutCode out2 = GetOutCode(x2, y2, xmin, ymin, xmax, ymax);
    while (true) {

         // Case 1: Both points inside
        if (!(out1.ALL | out2.ALL)) { 
            MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2); break;
        } 
        // Case 2: Both points outside in same region
        else if (out1.ALL & out2.ALL) break; 
        
        // Case 3: Line partially inside intersection with clipping boundary found
        else {
            double x, y;
            // Choose outside endpoint for intersection calculation
            OutCode outout = out1.ALL ? out1 : out2;
            // Clip against top boundary
            if (outout.top) {
                y = ymin;
                x = x1 + (double)(x2 - x1) * (ymin - y1) / (y2 - y1);
            } 
            // Clip against bottom boundary
            else if (outout.bottom) {
                y = ymax;
                x = x1 + (double)(x2 - x1) * (ymax - y1) / (x2 - x1);
            } 
            // Clip against right boundary
            else if (outout.right) {
                x = xmax;
                y = y1 + (double)(y2 - y1) * (xmax - x1) / (x2 - x1);
            } 
            // Clip against left boundary
            else if (outout.left) {
                x = xmin;
                y = y1 + (double)(y2 - y1) * (xmin - x1) / (x2 - x1);
            }
            // Replace the outside endpoint with the intersection point
            if (outout.ALL == out1.ALL) { 
                x1 = (int)x; y1 = (int)y; 
                out1 = GetOutCode(x1, y1, xmin, ymin, xmax, ymax); 
            } 
            // Update second endpoint of the line after finding its intersection
            else { 
                x2 = (int)x; y2 = (int)y; 
                out2 = GetOutCode(x2, y2, xmin, ymin, xmax, ymax); 
            }
        }
    }
}

// Draw point only if it lies inside the rectangle
void ClipPointRect(HDC hdc, int x, int y, int x1, int y1, int x2, int y2, COLORREF color) {
    if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        SetPixel(hdc, x, y, color);
    }
}

// Draw point if distance from center <= radius
void ClipPointCircle(HDC hdc, int x, int y, int xc, int yc, int R, COLORREF color) {
    double distSq = (double)(x - xc) * (x - xc) + (double)(y - yc) * (y - yc);
    if (distSq <= (double)R * R) {
        SetPixel(hdc, x, y, color);
    }
}

// Line clipping against circle using parametric line equation
void ClipLineCircle(HDC hdc, int x1, int y1, int x2, int y2, int xc, int yc, int R) {
    // Direction vector of the line
    double dx = x2 - x1;
    double dy = y2 - y1;

    // line relative to circle center
    double ax = x1 - xc;
    double ay = y1 - yc;

    // Quadratic equation coefficients
    double a = dx * dx + dy * dy;
    double b = 2 * (ax * dx + ay * dy);
    double c = ax * ax + ay * ay - (double)R * R;
    // Discriminant of the quadratic equation
    double disc = b * b - 4 * a * c;

    // Case of no intersection
    if (disc < 0) return; 

    // Compute intersection parameters to find the segment of the line inside the circle
    double u1 = (-b - sqrt(disc)) / (2 * a);
    double u2 = (-b + sqrt(disc)) / (2 * a);

    // segment range [0,1]
    double tmin = max(0.0, min(u1, u2));
    double tmax = min(1.0, max(u1, u2));

    // Draw the clipped line segment 
    if (tmin < tmax) {
        int nx1 = x1 + tmin * dx;
        int ny1 = y1 + tmin * dy;
        int nx2 = x1 + tmax * dx;
        int ny2 = y1 + tmax * dy;
        MoveToEx(hdc, nx1, ny1, NULL);
        LineTo(hdc, nx2, ny2);
    }
}

// Polygon Clipping Utilities
// Check if point is inside left edge
bool InLeft(Point v, int edge) { return v.x >= edge; }
// Check if point is inside right edge
bool InRight(Point v, int edge) { return v.x <= edge; }
// Check if point is inside top edge
bool InTop(Point v, int edge) { return v.y >= edge; }
// Check if point is inside bottom edge
bool InBottom(Point v, int edge) { return v.y <= edge; }

//intersection point between the line segment and a vertical clipping boundary (x = xedge)
Point VIntersect(Point v1, Point v2, int xedge) {
    Point res;
    // Intersection lies on the vertical edge
    res.x = xedge;
    // Compute corresponding y-coordinate using line equation
    res.y = v1.y + (xedge - v1.x) * (v2.y - v1.y) / (v2.x - v1.x);
    return res;
}

//intersection point between the line segment and a horizontal clipping boundary (y = yedge)
Point HIntersect(Point v1, Point v2, int yedge) {
    Point res;
    // Intersection lies on the horizontal edge
    res.y = yedge;
    // Compute corresponding x-coordinate using line equation
    res.x = v1.x + (yedge - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
    return res;
}

// Sutherland-Hodgman Iteration
PointList ClipWithEdge(PointList p, int edge, bool (*IsIn)(Point, int), Point (*Intersect)(Point, Point, int)) {
    //new polygon list
    PointList OutList;                             // function pointer checks whether a point is inside the clipping boundary.

    // Empty polygon case
    if (p.empty()) return OutList;

    // Start with last vertex
    Point v1 = p.back();

    bool v1_in = IsIn(v1, edge);

    // Iterate through each edge of the polygon
    for (int i = 0; i < (int)p.size(); i++) {
        Point v2 = p[i];
        bool v2_in = IsIn(v2, edge);

        // Outside -> Inside case: add intersection and current vertex
        if (!v1_in && v2_in) {
            OutList.push_back(Intersect(v1, v2, edge));
            OutList.push_back(v2);
        } 
        // Inside -> Inside case: add current vertex
        else if (v1_in && v2_in) {
            OutList.push_back(v2);
        } 
        // Inside -> Outside case: add intersection only
        else if (v1_in && !v2_in) {
            OutList.push_back(Intersect(v1, v2, edge));
        }
        //move to next edge
        v1 = v2; v1_in = v2_in;
    }
    return OutList;
}


// Clip polygon against rectangular window
void PolygonClip(HDC hdc, PointList p, int xleft, int ytop, int xright, int ybottom) {
    // Clip against all rectangle edges
    p = ClipWithEdge(p, xleft, InLeft, VIntersect);
    p = ClipWithEdge(p, ytop, InTop, HIntersect);
    p = ClipWithEdge(p, xright, InRight, VIntersect);
    p = ClipWithEdge(p, ybottom, InBottom, HIntersect);

    if (p.empty()) return;

    // Draw resulting polygon
    Point prev = p.back();

    for (const auto& curr : p) {
        MoveToEx(hdc, prev.x, prev.y, NULL);
        LineTo(hdc, curr.x, curr.y);
        prev = curr;
    }
}
