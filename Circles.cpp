#include "Circles.h"
#include <cmath>

void Draw8Points(HDC hdc, int xc, int yc, int x, int y, COLORREF color) {
    SetPixel(hdc, xc + x, yc + y, color);
    SetPixel(hdc, xc - x, yc + y, color);
    SetPixel(hdc, xc + x, yc - y, color);
    SetPixel(hdc, xc - x, yc - y, color);
    SetPixel(hdc, xc + y, yc + x, color);
    SetPixel(hdc, xc - y, yc + x, color);
    SetPixel(hdc, xc + y, yc - x, color);
    SetPixel(hdc, xc - y, yc - x, color);
}

void CircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color) {
    int x = 0, y = R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y) {
        x++;
        y = round(sqrt(R * R - x * x));
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color) {
    double dtheta = 1.0 / R;
    for (double theta = 0; theta < 0.785; theta += dtheta) { // 0.785 is PI/4
        int x = round(R * cos(theta));
        int y = round(R * sin(theta));
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CircleIterativePolar(HDC hdc, int xc, int yc, int R, COLORREF color) {
    double dtheta = 1.0 / R;
    double ct = cos(dtheta), st = sin(dtheta);
    double x = R, y = 0;
    Draw8Points(hdc, xc, yc, R, 0, color);
    while (x > y) {
        double x1 = x * ct - y * st;
        y = x * st + y * ct;
        x = x1;
        Draw8Points(hdc, xc, yc, round(x), round(y), color);
    }
}

void CircleMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color) {
    int x = 0, y = R;
    int d = 1 - R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y) {
        if (d < 0) d += 2 * x + 3;
        else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CircleModifiedMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color) {
    int x = 0, y = R;
    int d = 1 - R;
    int d1 = 3;
    int d2 = 5 - 2 * R;
    Draw8Points(hdc, xc, yc, x, y, color);
    while (x < y) {
        if (d < 0) {
            d += d1;
            d2 += 2;
            d1 += 2;
        } else {
            d += d2;
            d2 += 4;
            d1 += 2;
            y--;
        }
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}
