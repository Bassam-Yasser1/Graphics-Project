#include "Clipping.h"

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
        if (!(out1.ALL | out2.ALL)) { // Accept
            MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2); break;
        } else if (out1.ALL & out2.ALL) break; // Reject
        else {
            int x, y;
            OutCode outout = out1.ALL ? out1 : out2;
            if (outout.left) { x = xmin; y = y1 + (xmin - x1) * (y2 - y1) / (x2 - x1); }
            else if (outout.right) { x = xmax; y = y1 + (xmax - x1) * (y2 - y1) / (x2 - x1); }
            else if (outout.top) { y = ymin; x = x1 + (ymin - y1) * (x2 - x1) / (y2 - y1); }
            else { y = ymax; x = x1 + (ymax - y1) * (x2 - x1) / (y2 - y1); }
            
            if (outout.ALL == out1.ALL) { x1 = x; y1 = y; out1 = GetOutCode(x1, y1, xmin, ymin, xmax, ymax); }
            else { x2 = x; y2 = y; out2 = GetOutCode(x2, y2, xmin, ymin, xmax, ymax); }
        }
    }
}

void ClipLineCircle(HDC hdc, int x1, int y1, int x2, int y2, int xc, int yc, int R) {
    // Simple point-in-circle check for start/end
    auto isIn = [&](int px, int py) {
        return (pow(px - xc, 2) + pow(py - yc, 2)) <= R * R;
    };
    if (isIn(x1, y1) && isIn(x2, y2)) {
        MoveToEx(hdc, x1, y1, NULL); LineTo(hdc, x2, y2);
    }
}
