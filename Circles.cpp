#include "Circles.h"
#include <cmath>

// Draw 8 symmetric points of the circle using circle symmetry around the center (xc, yc)
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

// Direct circle drawing algo using the circle equation: x² + y² = R²
void CircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color) {

    // Start from the top point of the circle
    int x = 0, y = R;

    // Draw initial symmetric points
    Draw8Points(hdc, xc, yc, x, y, color);

    // Continue until x reaches y (first octant only)
    while (x < y) {
        x++;
        
        // Calculate y from circle equation
        y = round(sqrt(R * R - x * x));

        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

// Polar circle drawing algo using the polar equation of a circle: x = R * cos(theta), y = R * sin(theta)
void CirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color) {

    //Small angle step for smooth drawing
    double dtheta = 1.0 / R;

    // Draw only first octant (from 0 to PI/4)
    for (double theta = 0; theta < 0.785; theta += dtheta) { 

        //Conversion to cartesian 
        int x = round(R * cos(theta));
        int y = round(R * sin(theta));

        Draw8Points(hdc, xc, yc, x, y, color);
    }
}
// Iterative polar circle algo avoiding repeated sin/cos calculations
void CircleIterativePolar(HDC hdc, int xc, int yc, int R, COLORREF color) {

    // Rotation angle increment
    double dtheta = 1.0 / R;
    double ct = cos(dtheta), st = sin(dtheta);
    
    // Start Point (R, 0)
    double x = R, y = 0;

    Draw8Points(hdc, xc, yc, R, 0, color);

    // Rotate point iteratively
    while (x > y) {

         // Rotation transformation
        double x1 = x * ct - y * st;
        y = x * st + y * ct;
        x = x1;

        Draw8Points(hdc, xc, yc, round(x), round(y), color);
    }
}

// Midpoint circle algo using decision parameter to choose next pixel
void CircleMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color) {
    // Start point (top)
    int x = 0, y = R;

    // First decision parameter
    int d = 1 - R;
    Draw8Points(hdc, xc, yc, x, y, color);

    // first octant
    while (x < y) {
        // move horizontal
        if (d < 0) d += 2 * x + 3;
        else {
            // move diagonally down
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}

// Modified midpoint circle algo using incremental updates
void CircleModifiedMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color) {
    //Start point
    int x = 0, y = R;

    // First decision parameter
    int d = 1 - R;

    // Increment values 
    int d1 = 3;
    int d2 = 5 - 2 * R;

    Draw8Points(hdc, xc, yc, x, y, color);
    //draw first octant
    while (x < y) {
        // move horizontal
        if (d < 0) {
            d += d1;
            d2 += 2;
            d1 += 2;
        } else {
            // move diagonally down
            d += d2;
            d2 += 4;
            d1 += 2;
            y--;
        }
        x++;
        Draw8Points(hdc, xc, yc, x, y, color);
    }
}
