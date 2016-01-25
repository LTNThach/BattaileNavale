#include "InterfaceLoad.h"
#include "stdafx.h"
#include "GDIplusHeader.h"

VOID DrawImageBeginInterface(HDC hdc)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Interface/BeginInterface.jpg");
	graphics.DrawImage(image, 0, 0, 1366, 750);
}

VOID DrawInGameInterface(HDC hdc)
{
	Graphics graphics(hdc);
	Image* image = new Image(L"Image/Interface/InGameInterface.jpg");
	graphics.DrawImage(image, 0, 0, 1366, 750);
}