#pragma once
#include <windows.h>
#include <vector>

struct Point { int x, y; };


/** 
 * Clips a single point against a rectangular window.
 * The point is only drawn if x1 <= x <= x2 and y1 <= y <= y2.
 */
void ClipPointRect(HDC hdc, int x, int y, int x1, int y1, int x2, int y2, COLORREF color);

/** 
 * Clips a line using the Cohen-Sutherland algorithm.
 * Uses OutCodes to determine if the line is inside, outside, or needs clipping.
 * Also used for Square clipping by ensuring the input window has equal dimensions.
 */
void ClipLineRect(HDC hdc, int x1, int y1, int x2, int y2, int winX1, int winY1, int winX2, int winY2);



/** 
 * Clips a point against a circular boundary.
 * Calculated using the distance formula: (x-xc)^2 + (y-yc)^2 <= R^2.
 */
void ClipPointCircle(HDC hdc, int x, int y, int xc, int yc, int R, COLORREF color);

/** 
 * Clips a line against a circular window.
 * Only the segments of the line falling within the radius R of center (xc, yc) are drawn
 */
void ClipLineCircle(HDC hdc, int x1, int y1, int x2, int y2, int xc, int yc, int R);
