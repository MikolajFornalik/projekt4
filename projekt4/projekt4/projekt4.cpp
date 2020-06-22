
#include "framework.h"
#include "Projekt4.h"

#define MAX_LOADSTRING 100

// Zmienne globalne:
HINSTANCE hInst;                                // bie¿¹ce wyst¹pienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytu³u
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna g³ównego
ULONG_PTR m_gdiplusToken;						//GDI +
HWND gButton1, gButtonLeft, gButtonRight, HackStatus, box1, box2, box3;
Rect HackRect;									//
bool attach = false;									//czy hak zaczepiony
const WORD ID_TIMER = 1;



// Przeka¿ dalej deklaracje funkcji do³¹czone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawCrane(HDC hdc);
void DrawCircle(HDC hdc);
void MoveHackLeft(HDC hdc);
void MoveHackRight(HDC hdc);
void MoveHackDown(HDC hdc);
void MoveHackUp(HDC hdc);
bool AllowDown(HDC hdc, int x, int y);
bool AllowLeft(HDC hdc, int Height);
bool AllowRight(HDC hdc, int Height);
bool AllowUp(HDC hdc);
bool AllowFree(HDC hdc);
bool CheckCorners(HDC hdc);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: W tym miejscu umieœæ kod.

	// Inicjuj ci¹gi globalne
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PROJEKT4, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Wykonaj inicjowanie aplikacji:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJEKT4));

	MSG msg;

	// G³ówna pêtla komunikatów:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNKCJA: MyRegisterClass()
//
//  PRZEZNACZENIE: Rejestruje klasê okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJEKT4));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJEKT4);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   PRZEZNACZENIE: Zapisuje dojœcie wyst¹pienia i tworzy okno g³ówne
//
//   KOMENTARZE:
//
//        W tej funkcji dojœcie wyst¹pienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyœwietlane okno g³ówne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Przechowuj dojœcie wyst¹pienia w naszej zmiennej globalnej

	//Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//Buttons
	//gButton1 = CreateWindowEx(0, L"BUTTON", L"Get Circle", WS_CHILD | WS_VISIBLE, 500, 100, 75, 50, hWnd, (HMENU)GETCIRCLE, hInstance, NULL);
	//gButtonLeft = CreateWindowEx(0, L"BUTTON", L"<<", WS_CHILD | WS_VISIBLE, 800, 100, 50, 50, hWnd, (HMENU)LEFT, hInstance, NULL);
	//gButtonRight = CreateWindowEx(0, L"BUTTON", L">>", WS_CHILD | WS_VISIBLE, 850, 100, 50, 50, hWnd, (HMENU)RIGHT, hInstance, NULL);
	HackStatus = CreateWindowEx(0, L"STATIC", L"Not attached", WS_CHILD | WS_VISIBLE | SS_LEFT, 600, 50, 100, 25, hWnd, (HMENU)HACKINFO, hInstance, NULL);
	box1 = CreateWindowEx(0, L"STATIC", L"strzalki - ruch", WS_CHILD | WS_VISIBLE | SS_LEFT, 600, 75, 100, 25, hWnd, NULL, hInstance, NULL);
	box2 = CreateWindowEx(0, L"STATIC", L"a - zaczep/odczep", WS_CHILD | WS_VISIBLE | SS_LEFT, 600, 100, 125, 25, hWnd, NULL, hInstance, NULL);
	box3 = CreateWindowEx(0, L"STATIC", L"n - nowe elementy", WS_CHILD | WS_VISIBLE | SS_LEFT, 600, 125, 125, 25, hWnd, NULL, hInstance, NULL);
	if (SetTimer(hWnd, ID_TIMER, 5, NULL) == 0)
		MessageBox(hWnd, L"B³¹d tworzenia timera", L"Timer", MB_ICONSTOP);

	return TRUE;
}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PRZEZNACZENIE: Przetwarza komunikaty dla okna g³ównego.
//
//  WM_COMMAND  - przetwarzaj menu aplikacji
//  WM_PAINT    - Maluj okno g³ówne
//  WM_DESTROY  - opublikuj komunikat o wyjœciu i wróæ
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Analizuj zaznaczenia menu:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case GETCIRCLE:
		{	HDC hdc;
		hdc = GetDC(hWnd);
		DrawCircle(hdc);
		ReleaseDC(hWnd, hdc);
		}
		break;
		case LEFT:
		{
			HDC hdc;
			hdc = GetDC(hWnd);
			MoveHackLeft(hdc);
			ReleaseDC(hWnd, hdc);
		}
		break;
		case RIGHT:
		{
			HDC hdc;
			hdc = GetDC(hWnd);
			MoveHackRight(hdc);
			ReleaseDC(hWnd, hdc);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Tutaj dodaj kod rysuj¹cy u¿ywaj¹cy elementu hdc...
		DrawCrane(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
	{
		switch ((int)wParam)
		{
		case VK_LEFT:
		{
			HDC hdc;
			hdc = GetDC(hWnd);
			MoveHackLeft(hdc);
			ReleaseDC(hWnd, hdc);
		}
		break;

		case VK_RIGHT:
		{
			HDC hdc;
			hdc = GetDC(hWnd);
			MoveHackRight(hdc);
			ReleaseDC(hWnd, hdc);
		}
		break;

		case VK_DOWN:
		{
			HDC hdc;
			hdc = GetDC(hWnd);
			MoveHackDown(hdc);
			ReleaseDC(hWnd, hdc);
		}
		break;

		case VK_UP:
		{
			HDC hdc;
			hdc = GetDC(hWnd);
			MoveHackUp(hdc);
			ReleaseDC(hWnd, hdc);
		}
		break;

		case 0x41:
		{HDC hdc = GetDC(hWnd);

			if (CheckCorners( hdc)) {
				
				if (!attach && AllowFree(hdc))
				{
					attach = true;
					SetWindowText(HackStatus, L"attach");
				}
				else if (AllowFree(hdc))
				{
					attach = false;
					SetWindowTextW(HackStatus, L"not attach");
				}
				ReleaseDC(hWnd, hdc);
			}
		}
		break;

		case 0x4e:
		{
			HDC hdc = GetDC(hWnd);
			DrawCircle(hdc);
			ReleaseDC(hWnd, hdc);
		}
		break;
		}
	}
	break;

	case WM_DESTROY:
		GdiplusShutdown(m_gdiplusToken);
		KillTimer(hWnd, ID_TIMER);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Procedura obs³ugi komunikatów dla okna informacji o programie.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void DrawCrane(HDC hdc)
{
	Graphics graphics(hdc);
	Pen MyPen(Color(255, 128, 0));
	SolidBrush MyBrush(Color(255, 128, 0));
	graphics.DrawRectangle(&MyPen, 250, 100, 99, 600);	//
	graphics.DrawRectangle(&MyPen, 100, 170, 700, 69);	//Dzwig
	graphics.FillRectangle(&MyBrush, 600, 240, 50, 50);	//Hak
	HackRect.X = 600;
	HackRect.Y = 240;
	HackRect.Width = 50;
	HackRect.Height = 50;
	Pen BlackPen(Color(0, 0, 0));
	graphics.DrawLine(&BlackPen, 0, 700, 800, 700);
}

void DrawCircle(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush MyBrush(Color(0, 255, 0));
	graphics.FillEllipse(&MyBrush, 400, 649, 50, 50);
	graphics.FillRectangle(&MyBrush, 500, 649, 50, 50);
}

void MoveHackLeft(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush MyBrush(Color(255, 128, 0));
	SolidBrush WhiteBrush(Color(255, 255, 255));
	Pen MyPen(Color(255, 128, 0));
	Pen WhitePen(Color(255, 255, 255));
	SolidBrush GreenBrush(Color(0, 255, 0));
	if (attach && RGB(0, 255, 0) == GetPixel(hdc, HackRect.X + 25, HackRect.Y + 50) && AllowLeft(hdc, HackRect.Height + 50))
	{
		graphics.FillRectangle(&WhiteBrush, HackRect.X, HackRect.Y + 50, 50, 50);
		graphics.FillRectangle(&WhiteBrush, HackRect);		//zamazywanie starego haka
		graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
		HackRect.Offset(-10, 0);
		graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
		graphics.FillRectangle(&MyBrush, HackRect);
		graphics.FillEllipse(&GreenBrush, HackRect.X, HackRect.Y + 49, 50, 50);
	}
	else if (AllowLeft(hdc, HackRect.Height) && !attach)
	{
		graphics.FillRectangle(&WhiteBrush, HackRect);
		graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
		HackRect.Offset(-10, 0);
		graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
		graphics.FillRectangle(&MyBrush, HackRect);
	}
}

void MoveHackRight(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush MyBrush(Color(255, 128, 0));
	SolidBrush WhiteBrush(Color(255, 255, 255));
	Pen MyPen(Color(255, 128, 0));
	Pen WhitePen(Color(255, 255, 255));
	SolidBrush GreenBrush(Color(0, 255, 0));
	if (attach && RGB(0, 255, 0) == GetPixel(hdc, HackRect.X + 25, HackRect.Y + 50) && AllowRight(hdc, HackRect.Height + 50))
	{
		graphics.FillRectangle(&WhiteBrush, HackRect.X, HackRect.Y + 50, 50, 50);
		graphics.FillRectangle(&WhiteBrush, HackRect);		//zamazywanie starego haka
		graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
		HackRect.Offset(10, 0);
		graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
		graphics.FillRectangle(&MyBrush, HackRect);
		graphics.FillEllipse(&GreenBrush, HackRect.X, HackRect.Y + 49, 50, 50);
	}
	else if (AllowRight(hdc, HackRect.Height) && !attach)
	{
		graphics.FillRectangle(&WhiteBrush, HackRect);		//zamazywanie starego haka
		graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
		HackRect.Offset(10, 0);
		graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
		graphics.FillRectangle(&MyBrush, HackRect);
	}
}

void MoveHackDown(HDC hdc)
{
	if (RGB(0, 0, 0) != GetPixel(hdc, HackRect.X, HackRect.Y + 50))
	{
		Graphics graphics(hdc);
		SolidBrush MyBrush(Color(255, 128, 0));
		SolidBrush WhiteBrush(Color(255, 255, 255));
		SolidBrush GreenBrush(Color(0, 255, 0));
		Pen MyPen(Color(255, 128, 0));
		Pen WhitePen(Color(255, 255, 255));
		if (attach && RGB(0, 255, 0) == GetPixel(hdc, HackRect.X + 25, HackRect.Y + 50) && AllowDown(hdc, HackRect.X, HackRect.Y + 100))
		{
			graphics.FillRectangle(&WhiteBrush, HackRect.X, HackRect.Y + 50, 50, 50);
			graphics.FillRectangle(&WhiteBrush, HackRect);
			graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
			HackRect.Offset(0, 10);
			graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
			graphics.FillRectangle(&MyBrush, HackRect);
			graphics.FillEllipse(&GreenBrush, HackRect.X, HackRect.Y + 49, 50, 50);
		}
		if (AllowDown(hdc, HackRect.X, HackRect.Y + 50))
		{
			graphics.FillRectangle(&WhiteBrush, HackRect);
			graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
			HackRect.Offset(0, 10);
			graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
			graphics.FillRectangle(&MyBrush, HackRect);
		}
	}
}

void MoveHackUp(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush MyBrush(Color(255, 128, 0));
	SolidBrush WhiteBrush(Color(255, 255, 255));
	SolidBrush GreenBrush(Color(0, 255, 0));
	Pen MyPen(Color(255, 128, 0));
	Pen WhitePen(Color(255, 255, 255));
	if (attach && RGB(0, 255, 0) == GetPixel(hdc, HackRect.X + 25, HackRect.Y + 50) && AllowUp(hdc))
	{
		graphics.FillRectangle(&WhiteBrush, HackRect.X, HackRect.Y + 49, 50, 50);
		graphics.FillRectangle(&WhiteBrush, HackRect);
		graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
		HackRect.Offset(0, -10);
		graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
		graphics.FillRectangle(&MyBrush, HackRect);
		graphics.FillEllipse(&GreenBrush, HackRect.X, HackRect.Y + 49, 50, 50);
	}
	else if (AllowUp(hdc))
	{
		graphics.FillRectangle(&WhiteBrush, HackRect);
		graphics.DrawLine(&WhitePen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);		//zamazywanie starej liny
		HackRect.Offset(0, -10);
		graphics.DrawLine(&MyPen, HackRect.X + 25, 240, HackRect.X + 25, HackRect.Y);
		graphics.FillRectangle(&MyBrush, HackRect);
	}
}

bool AllowDown(HDC hdc, int x, int y)
{
	for (int i = 0; i < HackRect.Width; i++)
	{
		if (RGB(255, 255, 255) != GetPixel(hdc, x + i, y)) return false;
	}
	return true;
}

bool AllowFree(HDC hdc)
{
	if (RGB(255, 255, 255) != GetPixel(hdc, HackRect.X + 25, HackRect.Y + 100) && RGB(0, 255, 0) != GetPixel(hdc, HackRect.X + 25, HackRect.Y + 200)) return true;
	return false;
}

bool AllowLeft(HDC hdc, int Height)
{
	
	for (int i = 0; i < Height; i++)
	{
		if (RGB(255, 255, 255) != GetPixel(hdc, HackRect.X - 1, HackRect.Y + i)) return false;
	}
	return true;
}

bool AllowRight(HDC hdc, int Height)
{
	if (HackRect.X + HackRect.Width >= 800) {
		return false;
	}
		for (int i = 0; i < Height; i++)
		{
			if (RGB(255, 255, 255) != GetPixel(hdc, HackRect.X + 51, HackRect.Y + i)) return false;
		}
	
	return true;
}

bool AllowUp(HDC hdc)
{
	for (int i = 0; i < HackRect.Width; i++)
	{
		if (RGB(255, 255, 255) != GetPixel(hdc, HackRect.X, HackRect.Y - 1)) return false;
	}
	return true;
}

bool CheckCorners(HDC hdc)
{
	if (RGB(255, 255, 255) == GetPixel(hdc, HackRect.X, HackRect.Y + 1 + HackRect.Height) && RGB(255, 255, 255) == GetPixel(hdc, HackRect.X + HackRect.Width, HackRect.Y + 1 + HackRect.Height) && RGB(255, 255, 255) == GetPixel(hdc, HackRect.X, HackRect.Y - 2 + 2 * HackRect.Height) && RGB(255, 255, 255) == GetPixel(hdc, HackRect.X + HackRect.Width, HackRect.Y - 2 + 2 * HackRect.Height)) return true;
	return false;
}