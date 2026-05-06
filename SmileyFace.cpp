#include "SmileyFace.h"
#include "Circles.h"

void DrawSmiley(HDC hdc, int xc, int yc, bool isHappy) {
    // Head and Eyes
    CircleMidpoint(hdc, xc, yc, 60, RGB(0, 0, 0));
    CircleMidpoint(hdc, xc - 20, yc - 20, 7, RGB(0, 0, 0));
    CircleMidpoint(hdc, xc + 20, yc - 20, 7, RGB(0, 0, 0));
    
    // Nose
    MoveToEx(hdc, xc, yc - 5, NULL); LineTo(hdc, xc, yc + 15);
    
    // Mouth
    if (isHappy) {
        Arc(hdc, xc - 30, yc + 5, xc + 30, yc + 35, xc + 30, yc + 20, xc - 30, yc + 20);
    } else {
        Arc(hdc, xc - 30, yc + 15, xc + 30, yc + 45, xc - 30, yc + 30, xc + 30, yc + 30);
    }
}
