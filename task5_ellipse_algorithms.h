#pragma once
#include <windows.h>

// draws the 4 symmetric points of the ellipse for a given (dx, dy) offset
// direst works by iterating over x and computing y, then iterating over y and computing x to avoid gaps
void DrawEllipseDirect(HDC hdc, int cx, int cy, int a, int b, COLORREF color);

// polar works by iterating over angle and computing (x,y) using parametric form, but needs careful step size to avoid gaps
void DrawEllipsePolar(HDC hdc, int cx, int cy, int a, int b, COLORREF color);

// midpoint uses decision parameters to step through the ellipse, but needs to handle two regions based on slope to avoid gaps  
void DrawEllipseMidpoint(HDC hdc, int cx, int cy, int a, int b, COLORREF color);

