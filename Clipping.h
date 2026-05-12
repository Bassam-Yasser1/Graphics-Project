#pragma once
#include <windows.h>
#include <vector>

struct Point { int x, y; };

typedef std::vector<Point> PointList;

void ClipPointRect(HDC hdc, int x, int y, int x1, int y1, int x2, int y2, COLORREF color);

void ClipLineRect(HDC hdc, int x1, int y1, int x2, int y2, int winX1, int winY1, int winX2, int winY2);

void ClipPointCircle(HDC hdc, int x, int y, int xc, int yc, int R, COLORREF color);

void ClipLineCircle(HDC hdc, int x1, int y1, int x2, int y2, int xc, int yc, int R);

PointList ClipWithEdge(PointList p, int edge, bool (*IsIn)(Point, int), Point (*Intersect)(Point, Point, int));

void PolygonClip(HDC hdc, PointList p, int xleft, int ytop, int xright, int ybottom);
