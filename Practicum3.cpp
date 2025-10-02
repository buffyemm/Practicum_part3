// Practicum3.cpp : Defines the entry point for the application.
//
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "winmm.lib")
#include "framework.h"
#include "Practicum3.h"
#include "vector"
#include <string>

using namespace std;

bool activ = false;
HBITMAP menu = (HBITMAP)LoadImageW(NULL, L"menu.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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

// сделал класс производный клас, который наследует object_
class portal_ : public object_ {
public:
	int target;

	portal_(float x, float y, float width, float height, LPCWSTR name, int temp) { //конструктор класса

		model.x = x;
		model.y = y;
		model.width = width;
		model.height = height;
		picture = (HBITMAP)LoadImageW(NULL, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		target = temp;
	}

};

// создаем структуру 
struct Character {

	sprite model;
	HBITMAP picture;
	int current_loc = 0;
	vector<object_> item;

};


struct location_ {

	vector<object_> item;
	HBITMAP hBack; // создадим переменную для нашей картинки 
	vector<portal_> portal;

};

Character hero;

location_ room[2];

bool Check_collise(sprite first, sprite second) {  // проверка коллизии 

	if (first.x <= second.x + second.width && +
		first.x + first.width >= second.x &&
		first.y <= second.y + second.height &&
		first.y + first.height >= second.y) {

		return true;
	}

	return false;


}

struct {

	POINT p;

	bool collise_mouse(sprite first) {

		if (p.x >= first.x &&
			p.x <= first.x + first.width &&
			p.y <= first.y + first.height &&
			p.y >= first.y)

	return true;

		else
			return false;

	}


}mouse;


void InitWindow() { // инициализация структуры window

	RECT r;
	GetClientRect(window.hWnd, &r);
	window.width = r.right - r.left;
	window.height = r.bottom - r.top;


}

void InitGame() {

	static float scale = 0.104; // для экрана 
	// инициализирую переменные в hero, дальше будем делать конструктор.
	hero.model.speed = 20;
	hero.model.x = window.width / 2;
	hero.model.width = window.width * scale*(0.377);
	hero.model.height = window.height * scale;
	hero.model.y = window.height - hero.model.height;
	hero.picture = (HBITMAP)LoadImageW(NULL, L"A0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// закидывамем предметы в локациию, логика такая же, как в текстовой адвенчуре
	room[0].item.push_back({{window.width * scale, window.height - hero.model.height, window.width * scale * (0.377f), window.height * scale * (0.377f), 0},
	(HBITMAP)LoadImageW(NULL, L"sword.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)});

	room[1].item.push_back({ {window.width * scale, window.height - hero.model.height, window.width * scale * (0.377f), window.height * scale * (0.377f), 0},
	(HBITMAP)LoadImageW(NULL, L"axe.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE) });

	room[0].item.push_back({ {hero.model.width, window.height - hero.model.height, window.width * scale * (0.377f), window.height * scale * (0.377f), 0},
	(HBITMAP)LoadImageW(NULL, L"key.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE) });

	room[1].item.push_back({ {window.width - (window.width * scale), window.height - hero.model.height, window.width * scale * (0.5f), window.height * scale * (0.5f), 0},
	(HBITMAP)LoadImageW(NULL, L"bow.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE) });

	// закидываем порталы
	room[0].portal.emplace_back(window.width - hero.model.width, window.height - hero.model.height * 2, hero.model.width, hero.model.height, L"portal.bmp", 1);
	
	room[1].portal.emplace_back(window.width * (0.5f), window.height - hero.model.height * 3, hero.model.width, hero.model.height, L"portal.bmp", 0);


}

// отрисовка
auto DrawBitmap = [](HDC hdcDest, int x, int y, int w, int h, HBITMAP hBmp, bool transparent) {
	if (!hBmp) return;
	HDC hMemDC = CreateCompatibleDC(hdcDest);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	if (transparent) {

		TransparentBlt(hdcDest, x, y, w, h, hMemDC, 0, 0, w, h, RGB(0, 0, 0));// прозрачность

	}
	else {

		StretchBlt(hdcDest, x, y, w, h, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY); // растягивание на фул экран
	}
	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);
};

// отрисовка всех объектов
void ShowObject(HDC hMemDC) {

	// задник
	DrawBitmap(hMemDC, 0, 0, window.width, window.height, room[hero.current_loc].hBack, false);

	// отрисовка предметов
	for (auto i : room[hero.current_loc].item) {

		DrawBitmap(hMemDC, i.model.x, i.model.y, i.model.width, i.model.height, i.picture, false);

	}
	// порталы
	for (auto p : room[hero.current_loc].portal) {

		DrawBitmap(hMemDC, p.model.x, p.model.y, p.model.width, p.model.height, p.picture, true);

	}

	// герой
	DrawBitmap(hMemDC, hero.model.x, hero.model.y, hero.model.width, hero.model.height, hero.picture, true);

	// предметы игрока
	const int ITEM_SPACING = 10;
	int xPos = 100;
	if (!hero.item.empty()) {

		for (auto& item : hero.item) {
			DrawBitmap(hMemDC, xPos, 100,
				item.model.width, item.model.height, item.picture, false);
			xPos += item.model.width + ITEM_SPACING;
		}
	}


	if (activ) {

		float imageWidthPercent = 0.30f;  // 10% ширины экрана
		float imageHeightPercent = 0.45f; // 15% высоты экрана

		int imageWidth = (int)(window.width * imageWidthPercent);
		int imageHeight = (int)(window.height * imageHeightPercent);

		// Центрирование
		int centerX = (window.width - imageWidth) / 2;
		int centerY = (window.height - imageHeight) / 2;

		DrawBitmap(hMemDC, centerX, centerY, imageWidth, imageHeight, menu, false);
	}


}

// логика поралов
void Portal_Logic() {

	for (auto p : room[hero.current_loc].portal) {

		if (Check_collise(hero.model, p.model)) {

			activ = true;


			/*hero.current_loc = p.target;
			hero.model.x = hero.model.width;*/
		}
	}

}



// опрос клавиатуры
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

// границы комнаты, чтобы чел не ушел
void Proces_room() {


	if (hero.model.x <= window.width - window.width)
		hero.model.x = 0;

	if (hero.model.x >= window.width - hero.model.width)

		hero.model.x = window.width - hero.model.width;

}


void Process_game() {

	GetCursorPos(&mouse.p);
	ProcesImput();
	Proces_room();
	Portal_Logic();
	
}

void Menu() {



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

	SetTimer(window.hWnd, 1, 16, NULL);// ставим таймер на 16 милесикунд~60фпс



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


	case WM_LBUTTONDOWN:

		for (int i = 0; i < room[hero.current_loc].item.size(); i++) {
			if (mouse.collise_mouse(room[hero.current_loc].item[i].model)) {

				hero.item.emplace_back(room[hero.current_loc].item[i]);
				room[hero.current_loc].item.erase(room[hero.current_loc].item.cbegin() + i);

			}

		}
			break;

	case WM_DESTROY: // когда уничтожается
		PostQuitMessage(0);
		return 0;


	case WM_CREATE: { // кейс когда создается окно 


		room[0].hBack = (HBITMAP)LoadImageW(NULL, L"les.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); // загружаем картинку в переменную HBITMAP 
		room[1].hBack = (HBITMAP)LoadImageW(NULL, L"test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); // загружаем картинку в переменную HBITMAP 


		if (!room[0].hBack || !room[0].hBack) MessageBoxW(hwnd, L"Не удалось!", L"ОШИБКА", MB_ICONERROR);

		break;
	}

	case WM_TIMER:
		if (wParam == 1) { // у каждого таймера есть свой айди, и если таймер под айдишником 1 закончился, то мы запускаем то что ниже

			InvalidateRect(hwnd, NULL, FALSE); // перерисовка всего окна
			Process_game();

		}
		break;

	case WM_PAINT: { // вывод на экран картинки 

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// 1. Создаём буфер в памяти
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hMemBmp = CreateCompatibleBitmap(hdc, window.width, window.height);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);


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
