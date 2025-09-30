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


		if (!hBack) MessageBoxW(hwnd, L"Не удалось!", L"ОШИБКА", MB_ICONERROR);  // если картинка не была загруженна в переменную, то выдает ошибку 
		break;
	}

	case WM_PAINT: { // вывод на экран картинки 

		PAINTSTRUCT ps; // создаем структуру ps, по сути это некий набор кистей 
		HDC hdc = BeginPaint(hwnd, &ps); //  начинаем рисовать

		HDC hMemDC = CreateCompatibleDC(hdc); // созаем контекст памяти чтобы в него передать изображение и потом обратно закинуть его на экран. По сути это второй холст в памяти
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBack); //сохраняем старый битмап, который был в контексте памяти, чтобы потом восстановить
		BITMAP bmp; // создаем переменную и в нее загоняем размеры картинки нижней строчкой, но это если нам нужен натуральный размер
		GetObject(hBack, sizeof(BITMAP), &bmp); // узнаем размер картинки
		BitBlt(hdc, 0, 0, 400, 400, hMemDC, 0, 0, SRCCOPY);// перекидываем из нашего контекста памяти, там уже находитьсяя картинка, и мы выплевываем ее на основоной холст, обратите внимание на размер

		// Используем реальные размеры вместо 400x400
		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY); можно сделать так, используя реальные размеры картинки


		SelectObject(hMemDC, hOldBitmap); // восстанавливаем старый битмап обратно в контекст памяти
		DeleteDC(hMemDC); // удаляем контекст в памяти
		DeleteObject(hBack); // очищаем картинку

		EndPaint(hwnd, &ps); // заканчиваю рисовать
		break;
	}

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
