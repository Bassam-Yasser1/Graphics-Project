#include "SmileyFace.h"
#include "Circles.h"

void DrawSmiley(HDC hdc, int xc, int yc, bool isHappy) {
    // 1. Draw Face Outline & Eyes
    CircleMidpoint(hdc, xc, yc, 60, RGB(0, 0, 0));       // Face
    CircleMidpoint(hdc, xc - 20, yc - 20, 7, RGB(0, 0, 0)); // Left Eye
    CircleMidpoint(hdc, xc + 20, yc - 20, 7, RGB(0, 0, 0)); // Right Eye
    
    // 2. Draw Nose
    MoveToEx(hdc, xc, yc - 5, NULL); 
    LineTo(hdc, xc, yc + 15);
    
    // 3. Draw Mouth (The Arc)
    if (isHappy) {
        // A smile uses the BOTTOM half of the ellipse
        // Start point is left, end point is right
        Arc(hdc, xc - 30, yc + 10, xc + 30, yc + 40, 
                 xc - 30, yc + 25, xc + 30, yc + 25);
    } else {
        // A frown uses the TOP half of the ellipse
        // Start point is right, end point is left (reverses direction)
        Arc(hdc, xc - 30, yc + 20, xc + 30, yc + 50, 
                 xc + 30, yc + 35, xc - 30, yc + 35);
    }
}
