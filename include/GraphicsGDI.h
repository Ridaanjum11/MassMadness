#ifndef GRAPHICS_GDI_H
#define GRAPHICS_GDI_H

#include <windows.h>
#include "Simulation.h"

class GraphicsGDI {
private:
    HWND hwnd;
    HDC hdc;
    HBITMAP hBitmap;
    HDC memDC;
    bool running;
    
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static GraphicsGDI* instance;
    
public:
    GraphicsGDI();
    ~GraphicsGDI();
    
    void handleEvents(Simulation* sim);
    void render(Simulation* sim);
    void renderCrowdCity(Simulation* sim);
    bool isOpen() { return running; }
    
    void DrawCircle(HDC hdc, int x, int y, int radius, COLORREF color);
};

#endif // GRAPHICS_GDI_H

