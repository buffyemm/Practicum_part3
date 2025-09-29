// Practicum3.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Practicum3.h"

// создадим для удобства структуру window чтобы не носиться со всеми переменными
struct {

	HWND hWnd;
	int width, height;

}window;

HBITMAP hBack = NULL; // создадим переменную для нашей картинки 


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hI, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	

	const wchar_t CLASS_NAME[] = L"KAMEN";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hI;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);


	window.hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"JOB IS DONE",
		WS_OVERLAPPEDWINDOW, // тут менять стиль окна попробуйте WS_POPUP
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
		NULL,
		NULL,
		hI,
		NULL
	);


	if (window.hWnd == NULL) return 0;



	ShowWindow(window.hWnd, nCmdShow);

	


	MSG msg = { };

	while (GetMessage(&msg, NULL, 0, 0) > 0) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


	switch (uMsg) {

	case WM_DESTROY: // когда уничтожается
		PostQuitMessage(0);
		return 0;


	case WM_CREATE: { // кейс когда создается окно 





		hBack = (HBITMAP)LoadImageW(NULL, L"les.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); // загружаем картинку в переменную HBITMAP 


		if (!hBack) MessageBoxW(hwnd, L"Не удалось!", L"ОШИБКА", MB_ICONERROR);
		break;
	}

	case WM_PAINT: { // вывод на экран картинки 

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBack);
		BITMAP bmp;
		GetObject(hBack, sizeof(BITMAP), &bmp);
		BitBlt(hdc, 0, 0, 400, 400, hMemDC, 0, 0, SRCCOPY);
		
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		DeleteObject(hBack);

		EndPaint(hwnd, &ps);
		break;
	}

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
