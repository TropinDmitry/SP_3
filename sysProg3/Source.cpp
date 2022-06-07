#pragma comment(lib, "d2d1")
#include <windows.h>
#include <math.h>
#include <d2d1.h>
#include "resource.h"

#define NUM    1000
#define TWOPI  (2 * 3.14159)
#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

enum MenuChoice { Sin, Cos, Tan, CTan };

MenuChoice currentChoice = MenuChoice::Sin;
ID2D1Factory* g_pD2DFactory = NULL; // Direct2D factory
ID2D1HwndRenderTarget* g_pRenderTarget = NULL; // Render target
ID2D1SolidColorBrush* g_pBlackBrush = NULL; // A black brush, reflect the line color

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CreateD2DResource(HWND hWnd)
{
    if (!g_pRenderTarget)
    {
        HRESULT hr;

        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);

        // Obtain the size of the drawing area
        RECT rc;
        GetClientRect(hWnd, &rc);

        // Create a Direct2D render target
        hr = g_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                hWnd,
                D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
            ),
            &g_pRenderTarget
        );

        // Create a brush
        hr = g_pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            &g_pBlackBrush
        );
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("SineWave");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);    
    wndclass.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
    wndclass.lpszClassName = szAppName;

    RegisterClass(&wndclass);

    hwnd = CreateWindow(szAppName, TEXT("Sine Wave Using Polyline"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    CreateD2DResource(hwnd);
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);    

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int  cxClient, cyClient;
    HDC         hdc;
    int         i;
    PAINTSTRUCT ps;
    POINT       apt[NUM];

    switch (message)
    {
    case WM_SIZE:
    {
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
        if (g_pRenderTarget != NULL) {
            g_pRenderTarget->Resize(size);
        }
        return 0;
    }
    case WM_COMMAND:
    {
        InvalidateRect(hwnd, NULL, TRUE);
        int wmid = LOWORD(wParam);
        switch (wmid)
        {
        case ID_FUNCS_SIN:
            currentChoice = MenuChoice::Sin;
            break;
        case ID_FUNCS_COS:
            currentChoice = MenuChoice::Cos;
            break;
        case ID_FUNCS_TAN:
            currentChoice = MenuChoice::Tan;
            break;
        case ID_FUNCS_CTAN:
            currentChoice = MenuChoice::CTan;
            break;
        }
        UpdateWindow(hwnd);
        break;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);        

        g_pRenderTarget->BeginDraw();

        g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        g_pRenderTarget->DrawLine(
            D2D1::Point2F(0, cyClient / 2),
            D2D1::Point2F(cxClient, cyClient / 2),
            g_pBlackBrush, 0.5f
        );
        
        switch (currentChoice)
        {
        case MenuChoice::Sin:                        
            for (i = 0; i < NUM; i++)
            {                          
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - sin(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - sin(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );                                
            }
            g_pRenderTarget->EndDraw();
            break;
        case MenuChoice::Cos:
            for (i = 0; i < NUM; i++)
            {
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - cos(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - cos(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );
            }
            g_pRenderTarget->EndDraw();            
            break;
        case MenuChoice::Tan:
            for (i = 0; i < NUM; i++)
            {
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - tan(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - tan(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );
            }
            g_pRenderTarget->EndDraw();            
            break;
        case MenuChoice::CTan:
            for (i = 0; i < NUM; i++)
            {
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - 1 / tan(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - 1 / tan(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );
            }
            g_pRenderTarget->EndDraw();            
            break;
        }        
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);        
        break;
    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}