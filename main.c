#include <Windows.h>
#include <stdio.h>

static const char CLASS_NAME[] = "WheelDelta-6746812B-E74E-4DEA-9B84-EF7BAF5B6CEC";

static POINT g_lastDelta;
static POINT g_minDelta;
static POINT g_maxDelta;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			if (hDC)
			{
				SaveDC(hDC);

				SelectObject(hDC, GetStockObject(SYSTEM_FONT));
				SetTextAlign(hDC, TA_TOP | TA_LEFT);

				char text[100];
				snprintf(text, sizeof text, "x=%ld y=%ld (min: x=%ld y=%ld; max: x=%ld y=%ld)",
					g_lastDelta.x, g_lastDelta.y,
					g_minDelta.x, g_minDelta.y,
					g_maxDelta.x, g_maxDelta.y);

				TextOut(hDC, 0, 0, text, (int)strlen(text));

				RestoreDC(hDC, -1);

				EndPaint(hWnd, &ps);
			}
		}
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		g_lastDelta.x = 0;
		g_lastDelta.y = 0;
		goto redraw;

	case WM_MOUSEWHEEL:
		g_lastDelta.y = GET_WHEEL_DELTA_WPARAM(wParam);
		goto redraw;

	case WM_MOUSEHWHEEL:
		g_lastDelta.x = GET_WHEEL_DELTA_WPARAM(wParam);
	redraw:;
		g_minDelta.x = min(g_minDelta.x, g_lastDelta.x);
		g_minDelta.y = min(g_minDelta.y, g_lastDelta.y);
		g_maxDelta.x = max(g_maxDelta.x, g_lastDelta.x);
		g_maxDelta.y = max(g_maxDelta.y, g_lastDelta.y);
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RegisterClassEx(&(WNDCLASSEX) {
		sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, &WndProc, 0, 0, GetModuleHandle(NULL),
			LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW), GetStockObject(WHITE_BRUSH),
			NULL, CLASS_NAME, LoadIcon(NULL, IDI_APPLICATION),
	});
	HWND hWnd = CreateWindow(CLASS_NAME, "Wheel Delta", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, -1, CW_USEDEFAULT,
		-1, NULL, NULL, GetModuleHandle(NULL), 0);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hWnd);
	hWnd = NULL;

	return 0;
}
