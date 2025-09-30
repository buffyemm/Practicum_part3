// Practicum3.cpp : Defines the entry point for the application.
//
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "winmm.lib")
#include "framework.h"
#include "Practicum3.h"
#include "vector"



// создадим для удобства структуру window чтобы не носиться со всеми переменными
struct {

	HWND hWnd;
	int width, height;

}window;

// структура для спрайтов
struct sprite {
	float x, y, width, height, speed;
	bool inJump = false;
};

// класс по стуи тоже самое что и структура но со своими плюшками
class object_ {
public:
	sprite model;
	HBITMAP picture;

};

// создаем структуру 
struct Character {

	sprite model;
	HBITMAP picture;
};





std::vector<object_> item;

Character hero;

HBITMAP hBack = NULL; // создадим переменную для нашей картинки 


void InitWindow() {

	RECT r;
	GetClientRect(window.hWnd, &r);
	window.width = r.right - r.left;
	window.height = r.bottom - r.top;


}

void InitGame() {
	static float scale = 0.07;
	hero.model.speed = 20;
	hero.model.x = window.width / 2;
	hero.model.width = window.width * scale*(0.56);
	hero.model.height = window.height * scale;
	hero.model.y = window.height - hero.model.height;
	hero.picture = (HBITMAP)LoadImageW(NULL, L"A0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//item.push_back({window.width * scale, window.height - hero.model.height, window.width * })



}


auto DrawBitmap = [](HDC hdcDest, int x, int y, int w, int h, HBITMAP hBmp, bool transparent) {
	if (!hBmp) return;
	HDC hMemDC = CreateCompatibleDC(hdcDest);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	if (transparent) {

		TransparentBlt(hdcDest, x, y, w, h, hMemDC, 0, 0, w, h, RGB(0, 0, 0));

	}
	else {

		StretchBlt(hdcDest, x, y, w, h, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}
	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);
};


void ShowObject(HDC hMemDC) {

	DrawBitmap(hMemDC, hero.model.x, hero.model.y, hero.model.width, hero.model.height, hero.picture, false);


}

void ProcesImput() {

	static bool drop = false;
	static float jump = 0;

	float gravity = 30;
	if (GetAsyncKeyState('A')) hero.model.x -= hero.model.speed;
	if (GetAsyncKeyState('D')) {
		hero.model.x += hero.model.speed;
	}
	if (GetAsyncKeyState(VK_SPACE) && !hero.model.inJump) {

		drop = true;
		jump = 90;
		hero.model.inJump = true;

	}

	hero.model.y += gravity - jump;
	jump *= .9;
	hero.model.y = min((hero.model.y), (window.height - hero.model.height));

	if (hero.model.y + hero.model.height >= window.height)
		hero.model.inJump = false;
}


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
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), // функция узнает размер окна 
		NULL,
		NULL,
		hI,
		NULL
	);


	if (window.hWnd == NULL) return 0;


	InitWindow();
	InitGame();

	ShowWindow(window.hWnd, nCmdShow);

	SetTimer(window.hWnd, 1, 16, NULL);



	MSG msg = { };

	while (GetMessage(&msg, NULL, 0, 0) > 0) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


	switch (uMsg) {


	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {

			DestroyWindow(hwnd);

		}
		break;

	case WM_DESTROY: // когда уничтожается
		PostQuitMessage(0);
		return 0;


	case WM_CREATE: { // кейс когда создается окно 


		hBack = (HBITMAP)LoadImageW(NULL, L"les.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); // загружаем картинку в переменную HBITMAP 


		if (!hBack) MessageBoxW(hwnd, L"Не удалось!", L"ОШИБКА", MB_ICONERROR);

		break;
	}

	case WM_TIMER:
		if (wParam == 1) {

			InvalidateRect(hwnd, NULL, FALSE);
			ProcesImput();

		}
		break;

	case WM_PAINT: { // вывод на экран картинки 

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// 1. Создаём буфер в памяти
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hMemBmp = CreateCompatibleBitmap(hdc, window.width, window.height);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

		// 2. Рисуем ВСЁ в буфер
		// --- Фон ---
		if (hBack) {
			HDC hBackDC = CreateCompatibleDC(hMemDC);
			HBITMAP hOldBackBmp = (HBITMAP)SelectObject(hBackDC, hBack);
			BITMAP bmp;
			GetObject(hBack, sizeof(BITMAP), &bmp);
			StretchBlt(hMemDC, 0, 0, window.width, window.height, hBackDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			SelectObject(hBackDC, hOldBackBmp);
			DeleteDC(hBackDC);
		}

		// --- Платформа и герой ---

		ShowObject(hMemDC);


		// 3. Копируем готовый буфер на экран
		BitBlt(hdc, 0, 0, window.width, window.height, hMemDC, 0, 0, SRCCOPY);

		// 4. Очистка
		SelectObject(hMemDC, hOldBmp);
		DeleteObject(hMemBmp);
		DeleteDC(hMemDC);
		EndPaint(hwnd, &ps);
	}

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
