#include "stdafx.h"
#include "GameLoad.h"
#include "GDIplusHeader.h"
#include "InterfaceLoad.h"
#include <vector>
using namespace std;
#define MAX_LOADSTRING 100
#define WM_SOCKET		104
#define ID_SEND 9669

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;
SOCKET Socket = NULL;
SOCKADDR_IN SockAddr;
INT iHPShip2 = 2;
INT iHPShip31 = 3;
INT iHPShip32 = 3;
INT iHPShip4 = 4;
INT iHPShip5 = 5;
INT iPort = 1234;
INT iSignalRecv = NULL;
INT iXShot, iYShot;
INT iShipID = 0;
CHAR cIP[MAX_LOADSTRING];
TCHAR szMessage[10000];
TCHAR szIP[MAX_LOADSTRING];
TCHAR szTitle[MAX_LOADSTRING] = TEXT("Bataille Navale");
TCHAR szWindowClass[MAX_LOADSTRING] = TEXT("Bataille Navale");
HINSTANCE hInst;
HWND hWndMain;
HWND hEditIn;
HWND hEditOut;
HWND hButton;
HWND hButtonPlay;
HWND hButtonExit;
HWND hButtonReady;
HWND hButtonQuit;
HWND hPBShip2;
HWND hPBShip32;
HWND hPBShip31;
HWND hPBShip4;
HWND hPBShip5;
HWND hEditIP;
HWND hEditInfo;
BOOL bConnected = FALSE;
BOOL bTurn = FALSE;
BOOL bInGame = FALSE;
BOOL bMoving = FALSE;
BOOL bMovable = TRUE;
BOOL bPlaying = FALSE;
vector<POINT> pShooted;
vector<vector<INT>> iMatrix;
vector<INT> ShipState;
vector<Ship> ListShip;
HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
HFONT myFont = CreateFont(80, 0, 0, 0,
	FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Showcard Gothic");
HFONT myFont2 = CreateFont(30, 0, 0, 0,
	FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Showcard Gothic");

WNDPROC oldEditOutProc;
VOID CreateBeginInterface(HWND);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, INT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR    lpCmdLine, _In_ INT       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (INT)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BATTLESHIP));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
	ListShip.resize(7);
	ShipState.resize(7);

	for (INT i = 0; i < 7; i++)
	{
		ShipState[i] = 0;
	}

	iMatrix.resize(10);
	for (INT i = 0; i < 10; i++)
	{
		iMatrix[i].resize(10);
	}
	for (INT i = 0; i < 10; i++)
	{
		for (INT j = 0; j < 10; j++)
			iMatrix[i][j] = 0;
	}

	DrawShip(GetDC(hWndMain), iMatrix, ListShip);
	hInst = hInstance;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	hWndMain = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWndMain)
		return FALSE;

	ShowWindow(hWndMain, SW_MAXIMIZE);
	UpdateWindow(hWndMain);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	INT x, y;

	switch (message)
	{
		case WM_CREATE:
		{
			// Tạo giao diện
			CreateBeginInterface(hWnd);

			// Tạo socket
			WSADATA WsaDat;
			INT nResult = WSAStartup(MAKEWORD(2, 2), &WsaDat);
			if (nResult != 0)
			{
				MessageBox(hWnd, TEXT("Winsock initialization failed"), TEXT("Bataille Navale"), MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			Socket = socket(AF_INET, SOCK_STREAM, 0);
			if (Socket == INVALID_SOCKET)
			{
				MessageBox(hWnd, TEXT("Socket creation failed"), TEXT("Bataille Navale"), MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			nResult = WSAAsyncSelect(Socket, hWnd, WM_SOCKET, (FD_CLOSE | FD_READ | FD_CONNECT));
			if (nResult)
			{
				MessageBox(hWnd, TEXT("WSAAsyncSelect failed"), TEXT("Bataille Navale"), MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			SockAddr.sin_port = htons(iPort);
			SockAddr.sin_family = AF_INET;
		}
		break;

		case WM_SOCKET:
			if (WSAGETSELECTERROR(lParam))
			{
				MessageBox(hWnd, TEXT("Connection to server failed"), TEXT("Bataille Navale"), MB_OK | MB_ICONERROR);
				if (bConnected)
				{
					closesocket(Socket);
					SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				}
				break;
			}

			bConnected = TRUE;
			switch (WSAGETSELECTEVENT(lParam))
			{
				case FD_READ:
				{
					// Nhận tín hiệu
					if (iSignalRecv == NULL)
					{
						TCHAR szSignal[3];
						recv(Socket, (char*)szSignal, sizeof(INT), 0);
						iSignalRecv = _ttoi(szSignal);
						break;
					}

					switch (iSignalRecv)
					{
						// Tín hiệu 1: nhận thông điệp chat
						case 1:
						{
							TCHAR szIncoming[1024];
							ZeroMemory(szIncoming, sizeof(szIncoming));
							
							// Nhận thông điệp chat
							INT inDataLength = recv(Socket, (char*)szIncoming, 1024, 0);
							if (_tcslen(szIncoming) >= 1)
							{
								StringCchPrintf(szMessage, inDataLength + 12, TEXT("Other: %s\r\n"), szIncoming);

								INT idx = GetWindowTextLength(hEditIn);
								SendMessage(hEditIn, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
								SendMessage(hEditIn, EM_REPLACESEL, 0, (LPARAM)&szMessage);
								SendMessage(hEditIn, EM_LINESCROLL, 0, 1);
							}
						}
						break;

						// Tín hiệu 2: nhận tọa độ và gửi xác nhận
						case 2:
						{
							TCHAR szRecv[10];
							ZeroMemory(szRecv, sizeof(szRecv));
							// Nhận tọa độ
							recv(Socket, (char*)szRecv, 10, 0);
							INT iXrecv = szRecv[0] - 48;
							INT iYrecv = szRecv[2] - 48;
							if (iXrecv > 10 || iYrecv > 10 || iXrecv < 0 || iYrecv < 0)
								break;

							// Kiểm tra tọa độ
							INT iConfirm = CheckEnemyShot(GetDC(hWnd), iXrecv, iYrecv, iMatrix, iHPShip2, iHPShip31, iHPShip32,iHPShip4, iHPShip5);
							SendMessage(hPBShip2, PBM_SETPOS, iHPShip2, 0);
							SendMessage(hPBShip31, PBM_SETPOS, iHPShip31, 0);
							SendMessage(hPBShip32, PBM_SETPOS, iHPShip32, 0);
							SendMessage(hPBShip4, PBM_SETPOS, iHPShip4, 0);
							SendMessage(hPBShip5, PBM_SETPOS, iHPShip5, 0);

							if (iConfirm == 1)
							{
								bTurn = FALSE;
								SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)TEXT("Opponent's turn"));
							}
							else
							{
								bTurn = TRUE;
								SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)TEXT("Your turn"));
							}

							// Kiểm tra còn tàu hay không
							BOOL bWin = CheckMatrix(iMatrix);
							if (!bWin)
							{
								INT iSignalSend = 4;
								TCHAR szSign[5];
								_itot_s(iSignalSend, szSign, 9);
								send(Socket, (char*)szSign, sizeof(INT), 0);
								send(Socket, (char*)szSign, sizeof(INT), 0);

								MessageBox(hWnd, TEXT("You lose!"), TEXT("Bataille Navale"), NULL);
								closesocket(Socket);
								SendMessage(hWnd, WM_DESTROY, NULL, NULL);
							}

							INT iSignalSend = 3;
							TCHAR szSign[5];
							_itot_s(iSignalSend, szSign, 9);
							// Gửi tín hiệu
							send(Socket, (char*)szSign, sizeof(INT), 0);

							// Gửi xác nhận
							TCHAR szConfirm[5];
							_itot_s(iConfirm, szConfirm, 9);
							send(Socket, (char*)szConfirm, sizeof(INT), 0);
						}
						break;

						// Tín hiệu 3: nhận xác nhận
						case 3:
						{
							INT iConfirm;
							TCHAR szConfirm[3];
							// Nhận xác nhận
							recv(Socket, (char*)szConfirm, sizeof(INT), 0);
							iConfirm = _ttoi(szConfirm);
							if (iConfirm == 1)
								DrawMyShotEffect(GetDC(hWnd), iXShot, iYShot, 1);
							else if (iConfirm == 0)
								DrawMyShotEffect(GetDC(hWnd), iXShot, iYShot, 0);
							else
								break;

							if (iConfirm == 1)
							{
								bTurn = TRUE;
								SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)TEXT("Your turn"));
							}
							else
							{
								bTurn = FALSE;
								SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)TEXT("Opponent's turn"));
							}
						}
						break;

						// Tín hiệu 4: nhận lượt
						case 4:
						{
							TCHAR szTurn[5];
							// Nhận lượt
							recv(Socket, (char*)szTurn, sizeof(INT), 0);
							bTurn = _ttoi(szTurn);

							if (bTurn)							
								SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)TEXT("Your turn"));									  
							else									  
								SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)TEXT("Opponent's turn"));									  
						}
						break;

						// Tín hiệu 5: nhận kết quả chiến thắng
						case 5:
						{
							MessageBox(hWnd, TEXT("You win!"), TEXT("Bataille Navale"), NULL);
							closesocket(Socket);
							SendMessage(hWnd, WM_DESTROY, NULL, NULL);
						}
						break;
					}
					iSignalRecv = NULL;
				}
				break;

				case FD_CLOSE:
				{
					MessageBox(hWnd, TEXT("Server closed connection"), TEXT("Bataille Navale"), MB_ICONINFORMATION | MB_OK);
					closesocket(Socket);
					SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				}
				break;

				case FD_CONNECT:
				{
					TCHAR szBuffer[MAX_LOADSTRING];
					StringCchPrintf(szBuffer, MAX_LOADSTRING, TEXT("Connected to server %s\r\nWait for opponent"), szIP);
					SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)szBuffer);
					ShowWindow(hEditIP, SW_HIDE);
					ShowWindow(hButtonReady, SW_HIDE);
					ShowWindow(hEditIn, SW_SHOW);
					ShowWindow(hEditOut, SW_SHOW);
					ShowWindow(hButton, SW_SHOW);
					bPlaying = TRUE;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;

			}
			break;

		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			switch (wmId)
			{
				case ID_EXIT:
					PostQuitMessage(0);
					break;

				case ID_PLAY:
					ShowWindow(hButtonPlay, SW_HIDE);
					ShowWindow(hButtonExit, SW_HIDE);
					ShowWindow(hButtonReady, SW_SHOW);
					ShowWindow(hPBShip2, SW_SHOW);
					ShowWindow(hPBShip32, SW_SHOW);
					ShowWindow(hPBShip31, SW_SHOW);
					ShowWindow(hPBShip4, SW_SHOW);
					ShowWindow(hPBShip5, SW_SHOW);
					ShowWindow(hEditIP, SW_SHOW);
					ShowWindow(hEditInfo, SW_SHOW);
					InvalidateRect(hWnd, NULL, TRUE);
					bInGame = TRUE;
					break;

				case ID_READY:
				{
					ZeroMemory(szIP, MAX_LOADSTRING);
					ZeroMemory(cIP, MAX_LOADSTRING);
					INT tlen = SendMessage(hEditIP, WM_GETTEXTLENGTH, 0, 0) + 1;
					SendMessage(hEditIP, WM_GETTEXT, tlen, (LPARAM)&szIP);
					wcstombs(cIP, szIP, wcslen(szIP) + 1);
					SendMessage(hEditIP, WM_SETTEXT, 0, (LPARAM)L"");
					SockAddr.sin_addr.s_addr = inet_addr(cIP);
					connect(Socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
				}
				break;

				case ID_SEND:
				{
					TCHAR szBuffer[1024];
					ZeroMemory(szBuffer, sizeof(szBuffer));
					INT tlen = SendMessage(hEditOut, WM_GETTEXTLENGTH, 0, 0) + 1;
					if (tlen > 1)
					{
						INT iSignalSend = 1;
						TCHAR szSign[5];
						_itot_s(iSignalSend, szSign, 9);
						// Gửi tín hiệu
						send(Socket, (char*)szSign, sizeof(INT), 0);

						SendMessage(hEditOut, WM_GETTEXT, tlen, (LPARAM)&szBuffer);
						// Gửi thông điệp chat
						send(Socket, (char*)szBuffer, sizeof(szBuffer), 0);

						SendMessage(hEditOut, WM_SETTEXT, NULL, (LPARAM)L"");
						StringCchPrintf(szMessage, tlen + 8, TEXT("Me: %s\r\n"), szBuffer);

						INT idx = GetWindowTextLength(hEditIn);
						SendMessage(hEditIn, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
						SendMessage(hEditIn, EM_REPLACESEL, 0, (LPARAM)&szMessage);
						SendMessage(hEditIn, EM_LINESCROLL, 0, 1);
					}
				}
				break;

				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			if (!bInGame)
				DrawImageBeginInterface(hdc);
			else
			{
				DrawInGameInterface(hdc);
				DrawShipForPB(hdc);
				DrawMap(L"Image/Map/BlueSea.png", hdc);
				if (!bPlaying)
					RedrawShip(hWnd, iMatrix, 1, 1, ListShip, bMovable);
				else
					DrawMinimap(hdc, iMatrix, ListShip);
			}
			EndPaint(hWnd, &ps);
		}
		break;

		case WM_LBUTTONDOWN:
			x = GET_X_LPARAM(lParam);
			y = GET_Y_LPARAM(lParam);
			if (!bPlaying)
			{
				CheckShipClicked(x, y, iMatrix, iShipID);
			}
			else
			{
				if (bTurn)
				{
					if (x >= 0 && x <= 600 && y >= 0 && y <= 600)
					{
						POINT p;
						iXShot = x / 60;
						iYShot = y / 60;
						p.x = iXShot;
						p.y = iYShot;

						BOOL bShooted = CheckPointShooted(pShooted, p);

						if (!bShooted)
						{
							pShooted.push_back(p);
							Fire(iXShot, iYShot, Socket);
						}
					}
				}
			}
			break;

		case WM_LBUTTONUP:
			x = GET_X_LPARAM(lParam);
			y = GET_Y_LPARAM(lParam);
			if (!bPlaying)
			{
				if (!bMoving)
				{
					ChangeShipDirection(hWnd, x, y, iMatrix, ShipState, ListShip, bMovable);
				}
				if (!bMovable)
				{
					RedrawShip(hWnd, iMatrix, x, y, ListShip, bMovable);
				}
				bMoving = FALSE;
			}
			break;

		case WM_MOUSEMOVE:
			if (wParam & MK_LBUTTON)
			{
				bMoving = TRUE;
				x = GET_X_LPARAM(lParam);
				y = GET_Y_LPARAM(lParam);
				if (!bPlaying)
				{
					MoveShip(hWnd, x, y, iShipID, iMatrix, ListShip, ShipState, bMovable);
				}
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK subEditOutProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_RETURN:
				SendMessage(hWndMain, WM_COMMAND, ID_SEND, 0);
				break;
			}
		default:
			return CallWindowProc(oldEditOutProc, wnd, msg, wParam, lParam);
	}
	return 0;
}

VOID CreateBeginInterface(HWND hWnd)
{
	ZeroMemory(szMessage, sizeof(szMessage));

	hButtonPlay = CreateWindow(
		TEXT("BUTTON"),
		TEXT("PLAY"),
		WS_TABSTOP | WS_VISIBLE |
		WS_CHILD | BS_DEFPUSHBUTTON,
		200,
		600,
		300,
		100,
		hWnd,
		(HMENU)ID_PLAY,
		GetModuleHandle(NULL),
		NULL);
	SendMessage(hButtonPlay, WM_SETFONT, WPARAM(myFont), TRUE);

	hButtonExit = CreateWindow(
		TEXT("BUTTON"),
		TEXT("EXIT"),
		WS_TABSTOP | WS_VISIBLE |
		WS_CHILD | BS_DEFPUSHBUTTON,
		900,
		600,
		300,
		100,
		hWnd,
		(HMENU)ID_EXIT,
		GetModuleHandle(NULL),
		NULL);
	SendMessage(hButtonExit, WM_SETFONT, WPARAM(myFont), TRUE);

	hButtonReady = CreateWindow(
		TEXT("BUTTON"),
		TEXT("READY"),
		WS_TABSTOP | WS_VISIBLE |
		WS_CHILD | BS_DEFPUSHBUTTON,
		150,
		600,
		300,
		100,
		hWnd,
		(HMENU)ID_READY,
		GetModuleHandle(NULL),
		NULL);
	SendMessage(hButtonReady, WM_SETFONT, WPARAM(myFont), TRUE);
	ShowWindow(hButtonReady, SW_HIDE);

	hEditOut = CreateWindowEx(WS_EX_CLIENTEDGE,
		TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE |
		ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		950,
		690,
		315,
		60,
		hWnd,
		(HMENU)NULL,
		GetModuleHandle(NULL),
		NULL);
	if (!hEditOut)
		MessageBox(hWnd, TEXT("Could not create outgoing edit box."), TEXT("Bataille Navale"), MB_OK | MB_ICONERROR);
	SendMessage(hEditOut, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	SendMessage(hEditOut, EM_SETCUEBANNER, FALSE, (LPARAM)TEXT("Type message here..."));
	ShowWindow(hEditOut, SW_HIDE);
	oldEditOutProc = (WNDPROC)SetWindowLongPtr(hEditOut, GWLP_WNDPROC, (LONG_PTR)subEditOutProc);

	hEditIn = CreateWindowEx(WS_EX_CLIENTEDGE,
		TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
		WS_BORDER | ES_LEFT | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY,
		950,
		390,
		415,
		300,
		hWnd,
		(HMENU)NULL,
		GetModuleHandle(NULL),
		NULL);
	if (!hEditIn)
		MessageBox(hWnd, TEXT("Could not create incoming edit box."), TEXT("Bataille Navale"), MB_OK | MB_ICONERROR);
	SendMessage(hEditIn, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	ShowWindow(hEditIn, SW_HIDE);

	hButton = CreateWindow(
		L"BUTTON",
		L"Send",
		WS_TABSTOP | WS_VISIBLE |
		WS_CHILD | BS_DEFPUSHBUTTON,
		1265,
		690,
		100,
		60,
		hWnd,
		(HMENU)ID_SEND,
		GetModuleHandle(NULL),
		NULL);
	SendMessage(hButton, WM_SETFONT, WPARAM(myFont2), TRUE);
	ShowWindow(hButton, SW_HIDE);

	hButtonReady = CreateWindow(
		TEXT("BUTTON"),
		TEXT("READY"),
		WS_TABSTOP | WS_VISIBLE |
		WS_CHILD | BS_DEFPUSHBUTTON,
		150,
		600,
		300,
		100,
		hWnd,
		(HMENU)ID_READY,
		GetModuleHandle(NULL),
		NULL);
	SendMessage(hButtonReady, WM_SETFONT, WPARAM(myFont), TRUE);
	ShowWindow(hButtonReady, SW_HIDE);

	hPBShip5 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL,
		WS_CHILD | WS_VISIBLE,
		625,
		200,
		250,
		25,
		hWnd, NULL, hInst, NULL);
	ShowWindow(hPBShip5, SW_HIDE);

	hPBShip4 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL,
		WS_CHILD | WS_VISIBLE,
		625,
		300,
		200,
		25,
		hWnd, NULL, hInst, NULL);
	ShowWindow(hPBShip4, SW_HIDE);

	hPBShip31 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL,
		WS_CHILD | WS_VISIBLE,
		625,
		400,
		150,
		25,
		hWnd, NULL, hInst, NULL);
	ShowWindow(hPBShip31, SW_HIDE);

	hPBShip32 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL,
		WS_CHILD | WS_VISIBLE,
		625,
		500,
		150,
		25,
		hWnd, NULL, hInst, NULL);
	ShowWindow(hPBShip32, SW_HIDE);

	hPBShip2 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL,
		WS_CHILD | WS_VISIBLE,
		625,
		600,
		100,
		25,
		hWnd, NULL, hInst, NULL);
	ShowWindow(hPBShip2, SW_HIDE);

	SendMessage(hPBShip2, PBM_SETRANGE, 0, MAKELPARAM(0, 2));
	SendMessage(hPBShip31, PBM_SETRANGE, 0, MAKELPARAM(0, 3));
	SendMessage(hPBShip32, PBM_SETRANGE, 0, MAKELPARAM(0, 3));
	SendMessage(hPBShip4, PBM_SETRANGE, 0, MAKELPARAM(0, 4));
	SendMessage(hPBShip5, PBM_SETRANGE, 0, MAKELPARAM(0, 5));
	SendMessage(hPBShip2, PBM_SETPOS, iHPShip2, 0);
	SendMessage(hPBShip31, PBM_SETPOS, iHPShip31, 0);
	SendMessage(hPBShip32, PBM_SETPOS, iHPShip32, 0);
	SendMessage(hPBShip4, PBM_SETPOS, iHPShip4, 0);
	SendMessage(hPBShip5, PBM_SETPOS, iHPShip5, 0);

	hEditIP = CreateWindowEx(WS_EX_CLIENTEDGE,
		TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE | ES_CENTER |
		ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		625,
		650,
		250,
		25,
		hWnd,
		(HMENU)NULL,
		GetModuleHandle(NULL),
		NULL);
	ShowWindow(hEditIP, SW_HIDE);
	SendMessage(hEditIP, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	SendMessage(hEditIP, EM_SETCUEBANNER, FALSE, (LPARAM)TEXT("Type IP here..."));

	hEditInfo = CreateWindowEx(WS_EX_CLIENTEDGE,
		TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE | ES_CENTER |
		WS_BORDER | ES_LEFT | ES_MULTILINE |
		ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY,
		625,
		30,
		300,
		100,
		hWnd,
		(HMENU)NULL,
		GetModuleHandle(NULL),
		NULL);
	if (!hEditInfo)
		MessageBox(hWnd, TEXT("Could not create information edit box."), TEXT("Bataille Navale"), MB_OK | MB_ICONERROR);
	SendMessage(hEditInfo, WM_SETFONT, (WPARAM)myFont2, MAKELPARAM(FALSE, 0));
	SendMessage(hEditInfo, WM_SETTEXT, NULL, (LPARAM)TEXT("Attempting to connect to server...\r\n"));
	ShowWindow(hEditInfo, SW_HIDE);
	EnableWindow(hEditInfo, FALSE);
}