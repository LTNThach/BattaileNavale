#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <Commctrl.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define IDC_EDIT_IN		100
#define WM_SOCKET		101

INT iClientSendSignal = NULL;
INT iClientRecvConfirm = NULL;
INT iSignalRecv = NULL;
HWND hWndMain = NULL;
HWND hEditIn = NULL;
INT iPort = 1234;
CONST INT iNMaxClients = 2;
INT iNClient = 0;
TCHAR szMessage[10000];
sockaddr sockAddrClient;
SOCKET Socket[iNMaxClients];
SOCKET ServerSocket = NULL;

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nShowCmd)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = hInst;
	wClass.lpfnWndProc = (WNDPROC)WinProc;
	wClass.lpszClassName = L"Window Class";
	wClass.lpszMenuName = NULL;
	wClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wClass))
	{
		INT nResult = GetLastError();
		MessageBox(NULL, L"Window class creation failed\r\nError code:", L"Window Class Failed", MB_ICONERROR);
	}

		hWndMain = CreateWindowEx(NULL,
		L"Window Class",
		L"Bataille Navale Serveur",
		WS_OVERLAPPEDWINDOW,
		200,
		200,
		640,
		480,
		NULL,
		NULL,
		hInst,
		NULL);

	if (!hWndMain)
	{
		INT nResult = GetLastError();
		MessageBox(NULL, L"Window creation failed\r\nError code:", L"Window Creation Failed", MB_ICONERROR);
	}

	ShowWindow(hWndMain, nShowCmd);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	switch (msg)
	{
		case WM_CREATE:
		{
			GetClientRect(hWnd, &rect);
			ZeroMemory(szMessage, sizeof(szMessage));

			// Tạo 1 edit control để hiển thị các dòng chat
			hEditIn = CreateWindowEx(WS_EX_CLIENTEDGE,
									TEXT("EDIT"),
									TEXT(""),
									WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
									WS_BORDER | ES_LEFT | ES_MULTILINE |
									ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY,
									rect.top,
									rect.left,
									rect.right - rect.left,
									rect.bottom - rect.top,
									hWnd,
									(HMENU)IDC_EDIT_IN,
									GetModuleHandle(NULL),
									NULL);

			if (!hEditIn)
			{
				MessageBox(hWnd, L"Could not create incoming edit box.", L"Error", MB_OK | MB_ICONERROR);
				break;
			}

			HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEditIn, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
			SendMessage(hEditIn, WM_SETTEXT, NULL, (LPARAM)L"Waiting for client to connect...\r\n");		

			WSADATA WsaDat;
			INT nResult = WSAStartup(MAKEWORD(2, 2), &WsaDat);
			if (nResult != 0)
			{
				MessageBox(hWnd, L"Winsock initialization failed", L"Critical Error", MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			// Tạo socket
			SOCKADDR_IN SockAddr;
			SockAddr.sin_port = htons(iPort);
			SockAddr.sin_family = AF_INET;
			SockAddr.sin_addr.s_addr = INADDR_ANY;

			ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (ServerSocket == INVALID_SOCKET)
			{
				MessageBox(hWnd, L"Socket creation failed", L"Critical Error", MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			if (bind(ServerSocket, (LPSOCKADDR)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR)
			{
				MessageBox(hWnd, L"Unable to bind socket", L"Error", MB_OK);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			nResult = WSAAsyncSelect(ServerSocket, hWnd, WM_SOCKET, (FD_CLOSE | FD_ACCEPT | FD_READ));

			if (nResult)
			{
				MessageBox(hWnd, L"WSAAsyncSelect failed", L"Critical Error", MB_ICONERROR);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}

			if (listen(ServerSocket, 100) == SOCKET_ERROR)
			{
				MessageBox(hWnd, L"Unable to listen!", L"Error", MB_OK);
				SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				break;
			}
		}
		break;

		case WM_DESTROY:
		{
			shutdown(ServerSocket, SD_BOTH);
			closesocket(ServerSocket);
			WSACleanup();
			PostQuitMessage(0);
			return 0;
		}
		break;

		case WM_SOCKET:
		{
			switch (WSAGETSELECTEVENT(lParam))
			{
				// Khi nhận được thông tin
				case FD_READ:
				{
					TCHAR szIncoming[1024];
					INT X = -1, Y = -1;
					INT inDataLength;
					for (INT n = 0; n < iNMaxClients; n++)
					{
						// Nhận tín hiệu
						if (iSignalRecv == NULL)
						{
							iClientSendSignal = n;
							TCHAR szSignalRecv[3];
							recv(Socket[n], (char*)szSignalRecv, sizeof(INT), 0);
							iSignalRecv = _ttoi(szSignalRecv);
							continue;
						}

						// Xử lý từng tín hiệu
						if (n == iClientSendSignal)
						{
							switch (iSignalRecv)
							{
								// Tin hiệu 1: nhận thông điệp chat
								case 1:
								{				
									ZeroMemory(szIncoming, sizeof(szIncoming));

									// Nhận thông điệp chat	
									inDataLength = recv(Socket[n], (char*)szIncoming, 1024, 0);
									if (inDataLength <= 0)
										continue;
														
									if (inDataLength != -1 && _tcslen(szIncoming) >= 1)
									{
										StringCchPrintf(szMessage, inDataLength + 15, TEXT("Client %d: %s\r\n"), n + 1, szIncoming);

										INT idx = GetWindowTextLength(hEditIn);
										SendMessage(hEditIn, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
										SendMessage(hEditIn, EM_REPLACESEL, 0, (LPARAM)&szMessage);
									}

									// Gửi thông điệp chat cho các client khác
									for (INT j = 0; j <= iNClient; j++)
									{
										if (iClientSendSignal == j)
											continue;

										// Gửi tín hiệu
										INT iSignalSend = 1;
										TCHAR szSignalSend[5];
										_itot_s(iSignalSend, szSignalSend, 9);
										send(Socket[j], (char*)szSignalSend, sizeof(INT), 0);

										// Gửi thông điệp chat
										send(Socket[j], (CHAR*)szIncoming, sizeof(szIncoming), 0);
									}
								}
								break;

								// Tín hiệu 2: nhận tọa độs
								case 2:
								{
									TCHAR szRecv[10];
									// Nhận tọa độ
									recv(Socket[n], (char*)szRecv, 10, 0);
									iClientRecvConfirm = n;

									// Gửi tọa độ đến client khác
									for (INT i = 0; i < iNClient; i++)
									{
										if (iClientSendSignal == i)
											continue;
										// Gửi tín hiệu
										INT iSignalSend = 2;
										TCHAR szSignalSend[5];
										_itot_s(iSignalSend, szSignalSend, 9);
										send(Socket[i], (char*)szSignalSend, sizeof(INT), 0);

										// Gửi tọa độ
										send(Socket[i], (CHAR*)szRecv, sizeof(szRecv), 0);
									}
								}
								break;
											
								// Tín hiệu 3: nhận xác nhận (bắn trúng hay hụt)
								case 3:
								{
									INT iConfirm;
									TCHAR szConfirm[5];
									// Nhận xác nhận
									recv(Socket[n], (char*)szConfirm, sizeof(INT), 0);
									iConfirm = _ttoi(szConfirm);

									// Gửi xác nhận cho client đã gửi tọa độ và cần nhận confirm
									INT iSignalSend = 3;
									TCHAR szSignalSend[5];
									_itot_s(iSignalSend, szSignalSend, 9);
									// Gửi tín hiệu
									send(Socket[iClientRecvConfirm], (char*)szSignalSend, sizeof(INT), 0);

									_itot_s(iConfirm, szConfirm, 9);
									// Gửi xác nhận
									send(Socket[iClientRecvConfirm], (char*)szConfirm, sizeof(INT), 0);
								}
								break;

								// Tín hiệu 4: nhận tín hiệu thua cuộc từ 1 client
								case 4:
								{
									// Gửi tín hiệu chiến thắng cho client khác
									for (INT j = 0; j < iNClient; j++)
									{
										if (j == iClientSendSignal)
										{
											INT iSignalSend = 5;
											TCHAR szSignalSend[5];
											_itot_s(iSignalSend, szSignalSend, 9);
											// Gửi tín hiệu
											send(Socket[iClientRecvConfirm], (char*)szSignalSend, sizeof(INT), 0);
											send(Socket[iClientRecvConfirm], (char*)szSignalSend, sizeof(INT), 0);
										}
									}
								}
								break;
							}
						}
						else
							continue;
						iSignalRecv = NULL;
						iClientSendSignal = NULL;
					}
				}
				break;

				// Khi một client ngừng kết nôi
				case FD_CLOSE:
				{
					MessageBox(hWnd, L"Client closed connection", L"Connection closed!", MB_ICONINFORMATION | MB_OK);
					INT iSignalSend = 5;
					TCHAR szSignalSend[5];
					_itot_s(iSignalSend, szSignalSend, 9);
					// Gửi tín hiệu ngừng kết nối cho client khác
					for (INT j = 0; j < iNClient; j++)
					{
						send(Socket[j], (char*)szSignalSend, sizeof(INT), 0);
						send(Socket[j], (char*)szSignalSend, sizeof(INT), 0);
					}
					// Đóng server và thoát chương trình
					shutdown(ServerSocket, SD_BOTH);
					closesocket(ServerSocket);
					WSACleanup();
					PostQuitMessage(0);
				}
				break;
				
				// Khi chấp nhận kết nối của 1 client
				case FD_ACCEPT:
				{
					srand(time(NULL));
					if (iNClient < iNMaxClients)
					{
						INT size = sizeof(sockaddr);
						Socket[iNClient] = accept(wParam, &sockAddrClient, &size);

						if (Socket[iNClient] == INVALID_SOCKET)
						{
							INT iret = WSAGetLastError();
							WSACleanup();
						}

						INT idx = GetWindowTextLength(hEditIn);
						SendMessage(hEditIn, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
						SendMessage(hEditIn, EM_REPLACESEL, 0, (LPARAM)TEXT("Client connected!!!\r\n"));
					}

					iNClient++;

					// Khi đã đủ 2 client
					if (iNClient == 2)
					{
						// Gửi tín hiệu lượt cho các client
						for (INT i = 0; i < iNClient; i++)
						{
							INT iSignalSend = 4;
							TCHAR szSignalSend[5];
							_itot_s(iSignalSend, szSignalSend, 9);
							send(Socket[i], (char*)szSignalSend, sizeof(INT), 0);
						}

						// Chọn ngẫu nhiên lượt
						BOOL turn = rand() % 2;
						TCHAR szTurn[5];
						_itot_s(turn, szTurn, 9);
						// Gửi lượt cho client đầu tiên
						send(Socket[0], (char*)szTurn, sizeof(INT), 0);

						turn = !turn;
						_itot_s(turn, szTurn, 9);
						// Gửi lượt cho client tiếp theo
						send(Socket[1], (char*)szTurn, sizeof(INT), 0);
					}
				}
				break;
			}
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}