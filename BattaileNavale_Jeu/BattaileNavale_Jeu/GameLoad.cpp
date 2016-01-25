#include "stdafx.h"
#include "GameLoad.h"
#include "GDIplusHeader.h"
#include <stack>
#include <time.h>

// Vẽ tàu để hiển thị máu
VOID DrawShipForPB(HDC hdc)
{
	Graphics graphics(hdc);
	Image* imageShip5 = new Image(L"Image/Ship/BigShip90.png");
	graphics.DrawImage(imageShip5, 625, 140, 250, 56);

	Image* imageShip4 = new Image(L"Image/Ship/Submarine90.png");
	graphics.DrawImage(imageShip4, 625, 240, 200, 56);

	Image* imageShip31 = new Image(L"Image/Ship/MediumShip90.png");
	graphics.DrawImage(imageShip31, 625, 340, 150, 56);

	Image* imageShip32 = new Image(L"Image/Ship/MediumShip90.png");
	graphics.DrawImage(imageShip32, 625, 440, 150, 56);

	Image* imageShip2 = new Image(L"Image/Ship/SmallShip90.png");
	graphics.DrawImage(imageShip2, 625, 540, 100, 56);
}

// Vẽ bản đồ nhỏ góc phải
VOID DrawMinimap(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Map/BlueSea.png");
	graphics.DrawImage(image, 1000, 0, 300, 300);

	RedrawShip5(hdc, iMatrix, ListShip, 2);
	RedrawShip31(hdc, iMatrix, ListShip, 2);
	RedrawShip32(hdc, iMatrix, ListShip, 2);
	RedrawShip4(hdc, iMatrix, ListShip, 2);
	RedrawShip2(hdc, iMatrix, ListShip, 2);
}

// Vẽ bản đồ lớn
VOID DrawMap(LPCWSTR szFileName, HDC hdc)
{
	Graphics graphics(hdc);
	Image* image = new Image(szFileName);
	graphics.DrawImage(image, 0, 0, 600, 600);
}

// Vẽ tất cả các tàu
VOID DrawShip(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	DrawShip5(hdc, iMatrix, ListShip);
	DrawShip31(hdc, iMatrix, ListShip);
	DrawShip32(hdc, iMatrix, ListShip);
	DrawShip4(hdc, iMatrix, ListShip);
	DrawShip2(hdc, iMatrix, ListShip);
}

// Vẽ lại tất cả các tàu
VOID RedrawShip(HWND hWnd, vector<vector<INT>> &iMatrix, INT x, INT y, vector<Ship> &ListShip, BOOL &bMovable)
{
	HDC hdc;
	if (x >= 0 && x <= 600 && y >= 0 && y <= 600)
	{
		hdc = GetDC(hWnd);
		DrawMap(L"Image/Map/BlueSea.png", hdc);
		RedrawShip5(hdc, iMatrix, ListShip, 1);
		RedrawShip31(hdc, iMatrix, ListShip, 1);
		RedrawShip32(hdc, iMatrix, ListShip, 1);
		RedrawShip4(hdc, iMatrix, ListShip, 1);
		RedrawShip2(hdc, iMatrix, ListShip, 1);
		ReleaseDC(hWnd, hdc);
	}
	bMovable = TRUE;
}

// Vẽ tàu khi chuột di chuyển
VOID MoveShip(HWND hWnd, INT x, INT y, INT iShipID, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, vector<INT> &ShipState, BOOL &bMovable)
{
	HDC hdc, memDC;
	if (x >= 0 && x <= 600 && y >= 0 && y <= 600)
	{
		hdc = GetDC(hWnd);
		memDC = CreateCompatibleDC(NULL);
		HBITMAP bmp = CreateCompatibleBitmap(hdc, 600, 600);
		SelectObject(memDC, bmp);
		/*-----------------------------------------------------------------------------*/
		DrawMap(L"Image/Map/BlueSea.png", memDC);
		switch (iShipID)
		{
			case 5:
				RedrawShip31(memDC, iMatrix, ListShip, 1);
				RedrawShip32(memDC, iMatrix, ListShip, 1);
				RedrawShip4(memDC, iMatrix, ListShip, 1);
				RedrawShip2(memDC, iMatrix, ListShip, 1);
				bMovable = MoveShip5(memDC, x, y, ShipState[0], iMatrix, ListShip);
				break;
			case 31:
				RedrawShip5(memDC, iMatrix, ListShip, 1);
				RedrawShip32(memDC, iMatrix, ListShip, 1);
				RedrawShip4(memDC, iMatrix, ListShip, 1);
				RedrawShip2(memDC, iMatrix, ListShip, 1);
				bMovable = MoveShip31(memDC, x, y, ShipState[1], iMatrix, ListShip);
				break;
			case 32:
				RedrawShip5(memDC, iMatrix, ListShip, 1);
				RedrawShip31(memDC, iMatrix, ListShip, 1);
				RedrawShip4(memDC, iMatrix, ListShip, 1);
				RedrawShip2(memDC, iMatrix, ListShip, 1);
				bMovable = MoveShip32(memDC, x, y, ShipState[2], iMatrix, ListShip);
				break;
			case 4:
				RedrawShip5(memDC, iMatrix, ListShip, 1);
				RedrawShip31(memDC, iMatrix, ListShip, 1);
				RedrawShip32(memDC, iMatrix, ListShip, 1);
				RedrawShip2(memDC, iMatrix, ListShip, 1);
				bMovable = MoveShip4(memDC, x, y, ShipState[3], iMatrix, ListShip);
				break;
			case 2:
				RedrawShip5(memDC, iMatrix, ListShip, 1);
				RedrawShip31(memDC, iMatrix, ListShip, 1);
				RedrawShip32(memDC, iMatrix, ListShip, 1);
				RedrawShip4(memDC, iMatrix, ListShip, 1);
				bMovable = MoveShip2(memDC, x, y, ShipState[4], iMatrix, ListShip);
				break;
			default:
				RedrawShip5(memDC, iMatrix, ListShip, 1);
				RedrawShip31(memDC, iMatrix, ListShip, 1);
				RedrawShip32(memDC, iMatrix, ListShip, 1);
				RedrawShip4(memDC, iMatrix, ListShip, 1);
				RedrawShip2(memDC, iMatrix, ListShip, 1);
				break;
		}

		BitBlt(hdc, 0, 0, 600, 600, memDC, 0, 0, SRCCOPY);
		DeleteObject(bmp);
		DeleteDC(memDC);
		ReleaseDC(hWnd, hdc);
	}
}

// Kiểm tra tàu được chọn
VOID CheckShipClicked(INT x, INT y, vector<vector<INT>> &iMatrix, INT &iShipID)
{
	if (x >= 0 && x <= 600 && y >= 0 && y <= 600)
	{
		switch (iMatrix[x / 60][y / 60])
		{
			case 5:
				iShipID = 5;
				break;

			case 31:
				iShipID = 31;
				break;

			case 32:
				iShipID = 32;
				break;

			case 4:
				iShipID = 4;
				break;

			case 2:
				iShipID = 2;
				break;

			default:
				iShipID = 0;
				break;
		}
	}
}

// Vẽ tàu 5
VOID DrawShip5(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Ship/BigShip.png");
	graphics.DrawImage(image, 2, 5, 56, 290);
	for (INT i = 0; i < 5; i++)
		iMatrix[0][i] = 5;
	ListShip[0].position.x = 0;
	ListShip[0].position.y = 0;
	ListShip[0].state = 0;
}

// Di chuyển tàu 5
BOOL MoveShip5(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	for (INT i = 0; i < 10; i++)
	{
		for (INT j = 0; j < 10; j++)
		if (iMatrix[i][j] == 5)
			iMatrix[i][j] = 0;
	}
	Graphics graphics(hdc);
	Image* image;
	if (iState == 0 || iState == 2)
	{
		if (iState == 0)
			image = new Image(L"Image/Ship/BigShip.png");
		else
			image = new Image(L"Image/Ship/BigShip180.png");
		x /= 60;
		y = (y / 60) - 2;
		if (y < 0)
			y = 0;
		if (y >= 5)
			y = 5;
		if (x >= 9)
			x = 9;
		for (INT i = y; i < y + 5; i++)
			if (iMatrix[x][i] == 0 || iMatrix[x][i] == 5)
				iMatrix[x][i] = 5;
		else
		{
			if (iState == 0)
				image = new Image(L"Image/Ship/BigShipError.png");
			else
				image = new Image(L"Image/Ship/BigShipError180.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 2, y + 5, 56, 290);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 2, y + 5, 56, 290);
		ListShip[0].position.x = x;
		ListShip[0].position.y = y;
		ListShip[0].state = iState;
	}
	else
	{
		if (iState == 1)
			image = new Image(L"Image/Ship/BigShip90.png");
		else
			image = new Image(L"Image/Ship/BigShip270.png");
		y /= 60;
		x = (x / 60) - 2;
		if (x < 0)
			x = 0;
		if (x >= 5)
			x = 5;
		if (y >= 9)
			y = 9;
		for (INT i = x; i < x + 5; i++)
			if (iMatrix[i][y] == 0 || iMatrix[i][y] == 5)
				iMatrix[i][y] = 5;
		else
		{
			if (iState == 1)
				image = new Image(L"Image/Ship/BigShipError90.png");
			else
				image = new Image(L"Image/Ship/BigShipError270.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 5, y + 2, 290, 56);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 5, y + 2, 290, 56);
		ListShip[0].position.x = x;
		ListShip[0].position.y = y;
		ListShip[0].state = iState;
	}
	return TRUE;
}

// Vẽ lại tàu 5
VOID RedrawShip5(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale)
{
	Graphics graphics(hdc);
	Image* image;
	INT x = ListShip[0].position.x;
	INT y = ListShip[0].position.y;
	if (ListShip[0].state == 0 || ListShip[0].state == 2)
	{
		if (ListShip[0].state == 0)
			image = new Image(L"Image/Ship/BigShip.png");
		else
			image = new Image(L"Image/Ship/BigShip180.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 2, y / iScale + 5, 56 / iScale, 290 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = y; i < y + 5; i++)
			iMatrix[x][i] = 5;
	}
	else
	{
		if (ListShip[0].state == 1)
			image = new Image(L"Image/Ship/BigShip90.png");
		else
			image = new Image(L"Image/Ship/BigShip270.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 5, y / iScale + 2, 290 / iScale, 56 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = x; i < x + 5; i++)
			iMatrix[i][y] = 5;
	}
}

// Vẽ tàu 3 thứ nhất
VOID DrawShip31(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Ship/MediumShip.png");
	graphics.DrawImage(image, 123, 3, 56, 170);
	for (INT i = 0; i < 3; i++)
		iMatrix[2][i] = 31;
	ListShip[1].position.x = 120;
	ListShip[1].position.y = 0;
	ListShip[1].state = 0;
}

// Di chuyển tàu 3 thứ nhất
BOOL MoveShip31(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	for (INT i = 0; i < 10; i++)
	{
		for (INT j = 0; j < 10; j++)
		if (iMatrix[i][j] == 31)
			iMatrix[i][j] = 0;
	}
	Graphics graphics(hdc);
	Image* image;
	if (iState == 0 || iState == 2)
	{
		if (iState == 0)
			image = new Image(L"Image/Ship/MediumShip.png");
		else
			image = new Image(L"Image/Ship/MediumShip180.png");
		x /= 60;
		y = (y / 60) - 1;
		if (y < 0)
			y = 0;
		if (y >= 7)
			y = 7;
		if (x >= 9)
			x = 9;
		for (INT i = y; i < y + 3; i++)
		if (iMatrix[x][i] == 0 || iMatrix[x][i] == 31)
			iMatrix[x][i] = 31;
		else
		{
			if (iState == 0)
				image = new Image(L"Image/Ship/MediumShipError.png");
			else
				image = new Image(L"Image/Ship/MediumShipError180.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 56, 170);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 56, 170);
		ListShip[1].position.x = x;
		ListShip[1].position.y = y;
		ListShip[1].state = iState;
	}
	else
	{
		if (iState == 1)
			image = new Image(L"Image/Ship/MediumShip90.png");
		else
			image = new Image(L"Image/Ship/MediumShip270.png");
		y /= 60;
		x = (x / 60) - 1;
		if (x < 0)
			x = 0;
		if (x >= 7)
			x = 7;
		if (y >= 9)
			y = 9;
		for (INT i = x; i < x + 3; i++)
		if (iMatrix[i][y] == 0 || iMatrix[i][y] == 31)
			iMatrix[i][y] = 31;
		else
		{
			if (iState == 1)
				image = new Image(L"Image/Ship/MediumShipError90.png");
			else
				image = new Image(L"Image/Ship/MediumShipError270.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 170, 56);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 170, 56);
		ListShip[1].position.x = x;
		ListShip[1].position.y = y;
		ListShip[1].state = iState;
	}
	return TRUE;
}

// Vẽ lại tàu 3 thứ nhất
VOID RedrawShip31(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale)
{
	Graphics graphics(hdc);
	Image* image;
	INT x = ListShip[1].position.x;
	INT y = ListShip[1].position.y;
	if (ListShip[1].state == 0 || ListShip[1].state == 2)
	{
		if (ListShip[1].state == 0)
			image = new Image(L"Image/Ship/MediumShip.png");
		else
			image = new Image(L"Image/Ship/MediumShip180.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 56 / iScale, 170 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = y; i < y + 3; i++)
			iMatrix[x][i] = 31;
	}
	else
	{
		if (ListShip[1].state == 1)
			image = new Image(L"Image/Ship/MediumShip90.png");
		else
			image = new Image(L"Image/Ship/MediumShip270.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 170 / iScale, 56 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = x; i < x + 3; i++)
			iMatrix[i][y] = 31;
	}
}

// Vẽ tàu 3 thứ hai
VOID DrawShip32(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Ship/MediumShip.png");
	graphics.DrawImage(image, 183, 3, 56, 170);
	for (INT i = 0; i < 3; i++)
		iMatrix[3][i] = 32;
	ListShip[2].position.x = 180;
	ListShip[2].position.y = 0;
	ListShip[2].state = 0;
}

// Di chuyển tàu 3 thứ hai
BOOL MoveShip32(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	for (INT i = 0; i < 10; i++)
	{
		for (INT j = 0; j < 10; j++)
		if (iMatrix[i][j] == 32)
			iMatrix[i][j] = 0;
	}
	Graphics graphics(hdc);
	Image* image;
	if (iState == 0 || iState == 2)
	{
		if (iState == 0)
			image = new Image(L"Image/Ship/MediumShip.png");
		else
			image = new Image(L"Image/Ship/MediumShip180.png");
		x /= 60;
		y = (y / 60) - 1;
		if (y < 0)
			y = 0;
		if (y >= 7)
			y = 7;
		if (x >= 9)
			x = 9;
		for (INT i = y; i < y + 3; i++)
		if (iMatrix[x][i] == 0 || iMatrix[x][i] == 32)
			iMatrix[x][i] = 32;
		else
		{
			if (iState == 0)
				image = new Image(L"Image/Ship/MediumShipError.png");
			else
				image = new Image(L"Image/Ship/MediumShipError180.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 56, 170);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 56, 170);
		ListShip[2].position.x = x;
		ListShip[2].position.y = y;
		ListShip[2].state = iState;
	}
	else
	{
		if (iState == 1)
			image = new Image(L"Image/Ship/MediumShip90.png");
		else
			image = new Image(L"Image/Ship/MediumShip270.png");
		y /= 60;
		x = (x / 60) - 1;
		if (x < 0)
			x = 0;
		if (x >= 7)
			x = 7;
		if (y >= 9)
			y = 9;
		for (INT i = x; i < x + 3; i++)
		if (iMatrix[i][y] == 0 || iMatrix[i][y] == 32)
			iMatrix[i][y] = 32;
		else
		{
			if (iState == 1)
				image = new Image(L"Image/Ship/MediumShipError90.png");
			else
				image = new Image(L"Image/Ship/MediumShipError270.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 170, 56);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 170, 56);
		ListShip[2].position.x = x;
		ListShip[2].position.y = y;
		ListShip[2].state = iState;
	}
	return TRUE;
}

// Vẽ lại tàu 3 thứ hai
VOID RedrawShip32(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale)
{
	Graphics graphics(hdc);
	Image* image;
	INT x = ListShip[2].position.x;
	INT y = ListShip[2].position.y;
	if (ListShip[2].state == 0 || ListShip[2].state == 2)
	{
		if (ListShip[2].state == 0)
			image = new Image(L"Image/Ship/MediumShip.png");
		else
			image = new Image(L"Image/Ship/MediumShip180.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 56 / iScale, 170 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = y; i < y + 3; i++)
			iMatrix[x][i] = 32;
	}
	else
	{
		if (ListShip[2].state == 1)
			image = new Image(L"Image/Ship/MediumShip90.png");
		else
			image = new Image(L"Image/Ship/MediumShip270.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 170 / iScale, 56 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = x; i < x + 3; i++)
			iMatrix[i][y] = 32;
	}
}

// Vẽ tàu 4
VOID DrawShip4(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Ship/Submarine.png");
	graphics.DrawImage(image, 63, 3, 56, 230);
	for (INT i = 0; i < 4; i++)
		iMatrix[1][i] = 4;
	ListShip[3].position.x = 60;
	ListShip[3].position.y = 0;
	ListShip[3].state = 0;
}

// Di chuyển tàu 4
BOOL MoveShip4(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	for (INT i = 0; i < 10; i++)
	{
		for (INT j = 0; j < 10; j++)
		if (iMatrix[i][j] == 4)
			iMatrix[i][j] = 0;
	}
	Graphics graphics(hdc);
	Image* image;
	if (iState == 0 || iState == 2)
	{
		if (iState == 0)
			image = new Image(L"Image/Ship/Submarine.png");
		else
			image = new Image(L"Image/Ship/Submarine180.png");
		x /= 60;
		y = (y / 60) - 1;
		if (y < 0)
			y = 0;
		if (y >= 6)
			y = 6;
		if (x >= 9)
			x = 9;
		for (INT i = y; i < y + 4; i++)
		if (iMatrix[x][i] == 0 || iMatrix[x][i] == 4)
			iMatrix[x][i] = 4;
		else
		{
			if (iState == 0)
				image = new Image(L"Image/Ship/SubmarineError.png");
			else
				image = new Image(L"Image/Ship/SubmarineError180.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 56, 230);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 56, 230);
		ListShip[3].position.x = x;
		ListShip[3].position.y = y;
		ListShip[3].state = iState;
	}
	else
	{
		if (iState == 1)
			image = new Image(L"Image/Ship/Submarine90.png");
		else
			image = new Image(L"Image/Ship/Submarine270.png");
		y /= 60;
		x = (x / 60) - 1;
		if (x < 0)
			x = 0;
		if (x >= 6)
			x = 6;
		if (y >= 9)
			y = 9;
		for (INT i = x; i < x + 4; i++)
		if (iMatrix[i][y] == 0 || iMatrix[i][y] == 4)
			iMatrix[i][y] = 4;
		else
		{
			if (iState == 1)
				image = new Image(L"Image/Ship/SubmarineError90.png");
			else
				image = new Image(L"Image/Ship/SubmarineError270.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 230, 56);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 230, 56);
		ListShip[3].position.x = x;
		ListShip[3].position.y = y;
		ListShip[3].state = iState;
	}
	return TRUE;
}

// Vẽ lại tàu 4
VOID RedrawShip4(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale)
{
	Graphics graphics(hdc);
	Image* image;
	INT x = ListShip[3].position.x;
	INT y = ListShip[3].position.y;
	if (ListShip[3].state == 0 || ListShip[3].state == 2)
	{
		if (ListShip[3].state == 0)
			image = new Image(L"Image/Ship/Submarine.png");
		else
			image = new Image(L"Image/Ship/Submarine180.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 56 / iScale, 230 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = y; i < y + 4; i++)
			iMatrix[x][i] = 4;
	}
	else
	{
		if (ListShip[3].state == 1)
			image = new Image(L"Image/Ship/Submarine90.png");
		else
			image = new Image(L"Image/Ship/Submarine270.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 230 / iScale, 56 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = x; i < x + 4; i++)
			iMatrix[i][y] = 4;
	}
}

// Vẽ tàu 2
VOID DrawShip2(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Ship/SmallShip.png");
	graphics.DrawImage(image, 243, 3, 56, 110);
	for (INT i = 0; i < 2; i++)
		iMatrix[4][i] = 2;
	ListShip[4].position.x = 240;
	ListShip[4].position.y = 0;
	ListShip[4].state = 0;
}

// Di chuyển tàu 2
BOOL MoveShip2(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip)
{
	for (INT i = 0; i < 10; i++)
	{
		for (INT j = 0; j < 10; j++)
		if (iMatrix[i][j] == 2)
			iMatrix[i][j] = 0;
	}
	Graphics graphics(hdc);
	Image* image;
	if (iState == 0 || iState == 2)
	{
		if (iState == 0)
			image = new Image(L"Image/Ship/SmallShip.png");
		else
			image = new Image(L"Image/Ship/SmallShip180.png");
		x /= 60;
		y = y / 60;
		if (y < 0)
			y = 0;
		if (y >= 8)
			y = 8;
		if (x >= 9)
			x = 9;
		for (INT i = y; i < y + 2; i++)
		if (iMatrix[x][i] == 0 || iMatrix[x][i] == 2)
			iMatrix[x][i] = 2;
		else
		{
			if (iState == 0)
				image = new Image(L"Image/Ship/SmallShipError.png");
			else
				image = new Image(L"Image/Ship/SmallShipError180.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 56, 110);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 56, 110);
		ListShip[4].position.x = x;
		ListShip[4].position.y = y;
		ListShip[4].state = iState;
	}
	else
	{
		if (iState == 1)
			image = new Image(L"Image/Ship/SmallShip90.png");
		else
			image = new Image(L"Image/Ship/SmallShip270.png");
		y /= 60;
		x = x / 60;
		if (x < 0)
			x = 0;
		if (x >= 8)
			x = 8;
		if (y >= 9)
			y = 9;
		for (INT i = x; i < x + 2; i++)
		if (iMatrix[i][y] == 0 || iMatrix[i][y] == 2)
			iMatrix[i][y] = 2;
		else
		{
			if (iState == 1)
				image = new Image(L"Image/Ship/SmallShipError90.png");
			else
				image = new Image(L"Image/Ship/SmallShipError270.png");
			x *= 60;
			y *= 60;
			graphics.DrawImage(image, x + 3, y + 3, 110, 56);
			return FALSE;
		}
		x *= 60;
		y *= 60;
		graphics.DrawImage(image, x + 3, y + 3, 110, 56);
		ListShip[4].position.x = x;
		ListShip[4].position.y = y;
		ListShip[4].state = iState;
	}
	return TRUE;
}

// Vẽ lại tàu 2
VOID RedrawShip2(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale)
{
	Graphics graphics(hdc);
	Image* image;
	INT x = ListShip[4].position.x;
	INT y = ListShip[4].position.y;
	if (ListShip[4].state == 0 || ListShip[4].state == 2)
	{
		if (ListShip[4].state == 0)
			image = new Image(L"Image/Ship/SmallShip.png");
		else
			image = new Image(L"Image/Ship/SmallShip180.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 56 / iScale, 110 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = y; i < y + 2; i++)
			iMatrix[x][i] = 2;
	}
	else
	{
		if (ListShip[4].state == 1)
			image = new Image(L"Image/Ship/SmallShip90.png");
		else
			image = new Image(L"Image/Ship/SmallShip270.png");
		graphics.DrawImage(image, x / iScale + ((iScale - 1) * 1000) + 3, y / iScale + 3, 110 / iScale, 56 / iScale);
		x /= 60;
		y /= 60;
		for (INT i = x; i < x + 2; i++)
			iMatrix[i][y] = 2;
	}
}

// Xoay tàu
VOID ChangeShipDirection(HWND hWnd, INT x, INT y, vector<vector<INT>> &iMatrix, vector<INT> &ShipState, vector<Ship> &ListShip, BOOL &bMovable)
{
	HDC hdc;
	if (x >= 0 && x <= 600 && y >= 0 && y <= 600)
	{
		switch (iMatrix[x / 60][y / 60])
		{
			case 5:
			{
				ShipState[0] += 1;
				if (ShipState[0] == 4)
					ShipState[0] = 0;
				hdc = GetDC(hWnd);
				DrawMap(L"Image/Map/BlueSea.png", hdc);
				RedrawShip31(hdc, iMatrix, ListShip, 1);
				RedrawShip32(hdc, iMatrix, ListShip, 1);
				RedrawShip4(hdc, iMatrix, ListShip, 1);
				RedrawShip2(hdc, iMatrix, ListShip, 1);
				bMovable = MoveShip5(hdc, x, y, ShipState[0], iMatrix, ListShip);
				if (!bMovable)
					ShipState[0] -= 1;
				if (ShipState[0] == -1)
					ShipState[0] = 3;
				ReleaseDC(hWnd, hdc);
			}
			break;

			case 31:
			{
				ShipState[1] += 1;
				if (ShipState[1] == 4)
					ShipState[1] = 0;
				hdc = GetDC(hWnd);
				DrawMap(L"Image/Map/BlueSea.png", hdc);
				RedrawShip5(hdc, iMatrix, ListShip, 1);
				RedrawShip32(hdc, iMatrix, ListShip, 1);
				RedrawShip4(hdc, iMatrix, ListShip, 1);
				RedrawShip2(hdc, iMatrix, ListShip, 1);
				bMovable = MoveShip31(hdc, x, y, ShipState[1], iMatrix, ListShip);
				if (!bMovable)
					ShipState[1] -= 1;
				if (ShipState[1] == -1)
					ShipState[1] = 3;
				ReleaseDC(hWnd, hdc);
			}
			break;

			case 32:
			{
				ShipState[2] += 1;
				if (ShipState[2] == 4)
					ShipState[2] = 0;
				hdc = GetDC(hWnd);
				DrawMap(L"Image/Map/BlueSea.png", hdc);
				RedrawShip5(hdc, iMatrix, ListShip, 1);
				RedrawShip31(hdc, iMatrix, ListShip, 1);
				RedrawShip4(hdc, iMatrix, ListShip, 1);
				RedrawShip2(hdc, iMatrix, ListShip, 1);
				bMovable = MoveShip32(hdc, x, y, ShipState[2], iMatrix, ListShip);
				if (!bMovable)
					ShipState[2] -= 1;
				if (ShipState[2] == -1)
					ShipState[2] = 3;
				ReleaseDC(hWnd, hdc);
			}
			break;

			case 4:
			{
				ShipState[3] += 1;
				if (ShipState[3] == 4)
					ShipState[3] = 0;
				hdc = GetDC(hWnd);
				DrawMap(L"Image/Map/BlueSea.png", hdc);
				RedrawShip5(hdc, iMatrix, ListShip, 1);
				RedrawShip31(hdc, iMatrix, ListShip, 1);
				RedrawShip32(hdc, iMatrix, ListShip, 1);
				RedrawShip2(hdc, iMatrix, ListShip, 1);
				bMovable = MoveShip4(hdc, x, y, ShipState[3], iMatrix, ListShip);
				if (!bMovable)
					ShipState[3] -= 1;
				if (ShipState[3] == -1)
					ShipState[3] = 3;
				ReleaseDC(hWnd, hdc);
			}
			break;

			case 2:
			{
				ShipState[4] += 1;
				if (ShipState[4] == 4)
					ShipState[4] = 0;
				hdc = GetDC(hWnd);
				DrawMap(L"Image/Map/BlueSea.png", hdc);
				RedrawShip5(hdc, iMatrix, ListShip, 1);
				RedrawShip31(hdc, iMatrix, ListShip, 1);
				RedrawShip32(hdc, iMatrix, ListShip, 1);
				RedrawShip4(hdc, iMatrix, ListShip, 1);
				bMovable = MoveShip2(hdc, x, y, ShipState[4], iMatrix, ListShip);
				if (!bMovable)
					ShipState[4] -= 1;
				if (ShipState[4] == -1)
					ShipState[4] = 3;
				ReleaseDC(hWnd, hdc);
			}
			break;

			default:
			{
				hdc = GetDC(hWnd);
				RedrawShip5(hdc, iMatrix, ListShip, 1);
				RedrawShip31(hdc, iMatrix, ListShip, 1);
				RedrawShip32(hdc, iMatrix, ListShip, 1);
				RedrawShip4(hdc, iMatrix, ListShip, 1);
				RedrawShip2(hdc, iMatrix, ListShip, 1);
				ReleaseDC(hWnd, hdc);
			}
			break;
		}
	}
}

// Vẻ hiệu ứng khi bắn
VOID DrawMyShotEffect(HDC hdc, INT x, INT y, BOOL bChecked)
{
	Graphics graphics(hdc);
	Image* image;
	if (bChecked)
		image = new Image(L"Image/Effect/ex.png");
	else
		image = new Image(L"Image/Effect/ripple.png");
	graphics.DrawImage(image, x * 60 + 2, y * 60 + 2, 56, 56);
}

// Vẻ hiệu ứng khi bị bắn
VOID DrawEnemyShotEffect(HDC hdc, INT x, INT y, BOOL bChecked)
{
	Graphics graphics(hdc);
	Image* image;
	if (bChecked)
		image = new Image(L"Image/Effect/ex.png");
	else
		image = new Image(L"Image/Effect/ripple.png");
	graphics.DrawImage(image, x * 30 + 1001, y * 30 + 1, 26, 26);
}

// Kiểm tra khi bị bắn
INT CheckEnemyShot(HDC hdc, INT x, INT y, vector<vector<INT>> &iMatrix, INT& HPShip2, INT& HPShip31, INT& HPShip32, INT& HPShip4, INT& HPShip5)
{
	if (iMatrix[x][y] != 0 && iMatrix[x][y] != -1 && iMatrix[x][y] != -2)
	{
		if (iMatrix[x][y] == 2)
			HPShip2--;
		else if (iMatrix[x][y] == 31)
			HPShip31--;
		else if (iMatrix[x][y] == 32)
			HPShip32--;
		else if (iMatrix[x][y] == 4)
			HPShip4--;
		else if (iMatrix[x][y] == 5)
			HPShip5--;
		iMatrix[x][y] = -1;
		DrawEnemyShotEffect(hdc, x, y, 1);
		return 1;
	}
	else if (iMatrix[x][y] == -1 || iMatrix[x][y] == -2)
		return 2;
	else
	{
		iMatrix[x][y] = -2;
		DrawEnemyShotEffect(hdc, x, y, 0);
		return 0;
	}
}

// Bắn
VOID Fire(INT x, INT y, SOCKET Socket)
{
	TCHAR szBuffer[5];
	StringCchPrintf(szBuffer, 5, TEXT("%d,%d"), x, y);

	INT iSignalSend = 2;
	TCHAR szSignalSend[5];
	_itot_s(iSignalSend, szSignalSend, 9);
	// Gửi tín hiệu cho server
	send(Socket, (char*)szSignalSend, sizeof(INT), 0);
	
	// Gửi tọa độ cho server
	send(Socket, (char*)szBuffer, sizeof(szBuffer), 0);
}

// Kiểm tra hết tàu hay chưa
BOOL CheckMatrix(vector<vector<INT>> &iMatrix)
{
	for (INT i = 0; i < 10; i++)
	{
		for (INT j = 0; j < 10; j++)
		{
			if (iMatrix[i][j] == 2 || iMatrix[i][j] == 4 || iMatrix[i][j] == 5 || iMatrix[i][j] == 31 || iMatrix[i][j] == 32)
				return TRUE;
		}
	}
	return FALSE;
}

// Kiểm tra điểm đó đã bắn chưa
BOOL CheckPointShooted(vector<POINT> pShooted, POINT p)
{
	for (INT i = 0; i < pShooted.size(); i++)
	{
		if (p.x == pShooted[i].x && p.y == pShooted[i].y)
			return TRUE;
	}
	return FALSE;
}