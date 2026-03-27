#include <windows.h>
#include <cmath>
#include <iostream>
#include "GraphicsGDI.h"

GraphicsGDI* GraphicsGDI::instance = nullptr;

LRESULT CALLBACK GraphicsGDI::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        GraphicsGDI* pThis = reinterpret_cast<GraphicsGDI*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    
    GraphicsGDI* pThis = nullptr;
    if (msg != WM_CREATE) {
        pThis = reinterpret_cast<GraphicsGDI*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    switch (msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            if (pThis) pThis->running = false;
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (pThis) {
                switch (wParam) {
                    case VK_UP: break;
                    case VK_DOWN: break;
                    case VK_LEFT: break;
                    case VK_RIGHT: break;
                }
            }
            return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

GraphicsGDI::GraphicsGDI() : running(true), hwnd(nullptr), hdc(nullptr), memDC(nullptr), hBitmap(nullptr) {
    instance = this;
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "MassMadnessWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    RegisterClassA(&wc);
    
    hwnd = CreateWindowExA(
        0,
        "MassMadnessWindow",
        "Mass Madness - Crowd Simulation",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        820, 640,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        this
    );
    
    if (!hwnd) {
        std::cout << "ERROR: Failed to create window!" << std::endl;
        running = false;
        return;
    }
    
    hdc = GetDC(hwnd);
    memDC = CreateCompatibleDC(hdc);
    hBitmap = CreateCompatibleBitmap(hdc, 800, 600);
    SelectObject(memDC, hBitmap);
    
    std::cout << "Graphics initialized successfully (GDI)" << std::endl;
}

GraphicsGDI::~GraphicsGDI() {
    if (hBitmap) DeleteObject(hBitmap);
    if (memDC) DeleteDC(memDC);
    if (hdc) ReleaseDC(hwnd, hdc);
    if (hwnd) DestroyWindow(hwnd);
}

void GraphicsGDI::handleEvents(Simulation* sim) {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // New system uses player-based control, not leader movement
}

void GraphicsGDI::render(Simulation* sim) {
    // Old render function - use renderCrowdCity instead
    RECT rect = {0, 0, 800, 600};
    FillRect(memDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    BitBlt(hdc, 10, 10, 800, 600, memDC, 0, 0, SRCCOPY);
    Sleep(16);
}


void GraphicsGDI::DrawCircle(HDC hdc, int x, int y, int radius, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    SelectObject(hdc, brush);
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
    DeleteObject(brush);
}

void GraphicsGDI::renderCrowdCity(Simulation* sim) {
    // Clear screen (white)
    RECT rect = {0, 0, 800, 600};
    FillRect(memDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    
    // Color map for teams
    COLORREF teamColors[] = {
        RGB(255, 51, 51),   // Red
        RGB(51, 255, 51),   // Green
        RGB(51, 51, 255),   // Blue
        RGB(255, 255, 51)   // Yellow
    };
    
    // Draw all agents
    for (auto agent : sim->agents) {
        COLORREF color;
        if (agent->team == TEAM_NEUTRAL) {
            color = RGB(245, 245, 245);  // Off-white
        } else {
            color = teamColors[agent->team];
        }
        
        int r = (int)agent->radius;
        DrawCircle(memDC, (int)agent->x, (int)agent->y, r, color);
    }
    
    // Draw player indicators (larger circles around players)
    for (int i = 0; i < 4; i++) {
        if (sim->players[i]) {
            HPEN pen = CreatePen(PS_SOLID, 3, teamColors[i]);
            SelectObject(memDC, pen);
            SelectObject(memDC, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
            
            int r = 15;
            Ellipse(memDC,
                    (int)(sim->players[i]->x - r), (int)(sim->players[i]->y - r),
                    (int)(sim->players[i]->x + r), (int)(sim->players[i]->y + r));
            
            DeleteObject(pen);
        }
    }
    
    // Draw team stats at bottom
    HFONT font = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    SelectObject(memDC, font);
    SetTextColor(memDC, RGB(0, 0, 0));
    
    char statsText[256];
    sprintf_s(statsText, sizeof(statsText),
              "Red: %d  |  Green: %d  |  Blue: %d  |  Yellow: %d",
              sim->getTeamSize(TEAM_RED),
              sim->getTeamSize(TEAM_GREEN),
              sim->getTeamSize(TEAM_BLUE),
              sim->getTeamSize(TEAM_YELLOW));
    
    TextOutA(memDC, 20, 580, statsText, (int)strlen(statsText));
    
    DeleteObject(font);
    
    // Copy to screen
    BitBlt(hdc, 10, 10, 800, 600, memDC, 0, 0, SRCCOPY);
    
    Sleep(16); // ~60 FPS
}

