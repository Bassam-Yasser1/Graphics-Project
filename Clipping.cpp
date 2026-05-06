#include "Clipping.h"
#include <cmath>
#include <algorithm> 

using namespace std;

union OutCode {
    unsigned ALL : 4;
    struct { unsigned left : 1, right : 1, bottom : 1, top : 1; };
};

OutCode GetOutCode(int x, int y, int xmin, int ymin, int xmax, int ymax) {
    OutCode res; res.ALL = 0;
    if (x < xmin) res.left = 1; else if (x > xmax) res.right = 1;
    if (y < ymin) res.top = 1; else if (y > ymax) res.bottom = 1;
    return res;
}

void ClipLineRect(HDC hdc, int x1, int y1, int x2, int y2, int xmin, int ymin, int xmax, int ymax) {
    OutCode out1 = GetOutCode(x1, y1, xmin, ymin, xmax, ymax);
    OutCode out2 = GetOutCode(x2, y2, xmin, ymin, xmax, ymax);
    while (true) {
        if (!(out1.ALL | out2.ALL)) { 
            MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2); break;
        } else if (out1.ALL & out2.ALL) break; 
        else {
            double x, y;
            OutCode outout = out1.ALL ? out1 : out2;
            if (outout.top) {
                y = ymin;
                x = x1 + (double)(x2 - x1) * (ymin - y1) / (y2 - y1);
            } else if (outout.bottom) {
                y = ymax;
                x = x1 + (double)(x2 - x1) * (ymax - y1) / (y2 - y1);
            } else if (outout.right) {
                x = xmax;
                y = y1 + (double)(y2 - y1) * (xmax - x1) / (x2 - x1);
            } else if (outout.left) {
                x = xmin;
                y = y1 + (double)(y2 - y1) * (xmin - x1) / (x2 - x1);
            }
            
            if (outout.ALL == out1.ALL) { 
                x1 = (int)x; y1 = (int)y; 
                out1 = GetOutCode(x1, y1, xmin, ymin, xmax, ymax); 
            } else { 
                x2 = (int)x; y2 = (int)y; 
                out2 = GetOutCode(x2, y2, xmin, ymin, xmax, ymax); 
            }
        }
    }
}

void ClipLineCircle(HDC hdc, int x1, int y1, int x2, int y2, int xc, int yc, int R) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double ax = x1 - xc;
    double ay = y1 - yc;

    double a = dx * dx + dy * dy;
    double b = 2 * (ax * dx + ay * dy);
    double c = ax * ax + ay * ay - (double)R * R;
    double disc = b * b - 4 * a * c;

    if (disc < 0) return; 

    double u1 = (-b - sqrt(disc)) / (2 * a);
    double u2 = (-b + sqrt(disc)) / (2 * a);

    double tmin = max(0.0, min(u1, u2));
    double tmax = min(1.0, max(u1, u2));

    if (tmin < tmax) {
        int nx1 = x1 + tmin * dx;
        int ny1 = y1 + tmin * dy;
        int nx2 = x1 + tmax * dx;
        int ny2 = y1 + tmax * dy;
        MoveToEx(hdc, nx1, ny1, NULL);
        LineTo(hdc, nx2, ny2);
    }
}

// Intersection and Boundary Utilities
bool InLeft(Point v, int edge) { return v.x >= edge; }
bool InRight(Point v, int edge) { return v.x <= edge; }
bool InTop(Point v, int edge) { return v.y >= edge; }
bool InBottom(Point v, int edge) { return v.y <= edge; }

Point VIntersect(Point v1, Point v2, int xedge) {
    Point res;
    res.x = xedge;
    res.y = v1.y + (xedge - v1.x) * (v2.y - v1.y) / (v2.x - v1.x);
    return res;
}

Point HIntersect(Point v1, Point v2, int yedge) {
    Point res;
    res.y = yedge;
    res.x = v1.x + (yedge - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
    return res;
}

// Generic Sutherland-Hodgman Iteration
PointList ClipWithEdge(PointList p, int edge, bool (*IsIn)(Point, int), Point (*Intersect)(Point, Point, int)) {
    PointList OutList;
    if (p.empty()) return OutList;
    Point v1 = p.back();
    bool v1_in = IsIn(v1, edge);
    for (int i = 0; i < (int)p.size(); i++) {
        Point v2 = p[i];
        bool v2_in = IsIn(v2, edge);
        if (!v1_in && v2_in) {
            OutList.push_back(Intersect(v1, v2, edge));
            OutList.push_back(v2);
        } else if (v1_in && v2_in) {
            OutList.push_back(v2);
        } else if (v1_in && !v2_in) {
            OutList.push_back(Intersect(v1, v2, edge));
        }
        v1 = v2; v1_in = v2_in;
    }
    return OutList;
}
