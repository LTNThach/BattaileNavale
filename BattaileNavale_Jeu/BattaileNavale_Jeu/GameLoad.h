#include <vector>
using namespace std;

struct Ship
{
	POINT position;
	INT state;
};

VOID DrawShipForPB(HDC hdc);
VOID DrawMap(LPCWSTR szFileName, HDC hWinDC);
VOID DrawMinimap(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
VOID DrawShip(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
VOID RedrawShip(HWND hWnd, vector<vector<INT>> &iMatrix, INT x, INT y, vector<Ship> &ListShip, BOOL &bMovable);
VOID MoveShip(HWND hWnd, INT x, INT y, INT iShipID, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, vector<INT> &ShipState, BOOL &bMovable);
VOID CheckShipClicked(INT x, INT y, vector<vector<INT>> &iMatrix, INT &iShipID);
VOID DrawShip5(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
VOID DrawShip31(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
VOID DrawShip32(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
VOID DrawShip4(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
VOID DrawShip2(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
BOOL MoveShip5(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
BOOL MoveShip31(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
BOOL MoveShip32(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
BOOL MoveShip4(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
BOOL MoveShip2(HDC hdc, INT x, INT y, INT iState, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip);
VOID RedrawShip5(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale);
VOID RedrawShip31(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale);
VOID RedrawShip32(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale);
VOID RedrawShip4(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale);
VOID RedrawShip2(HDC hdc, vector<vector<INT>> &iMatrix, vector<Ship> &ListShip, INT iScale);
VOID ChangeShipDirection(HWND hWnd, INT x, INT y, vector<vector<INT>> &iMatrix, vector<INT> &ShipState, vector<Ship> &ListShip, BOOL &bMovable);
INT CheckEnemyShot(HDC hdc, INT x, INT y, vector<vector<INT>> &iMatrix, INT& HPShip2, INT& HPShip31, INT& HPShip32, INT& HPShip4, INT& HPShip5);
VOID Fire(INT x, INT y, SOCKET Socket);
VOID DrawEnemyShotEffect(HDC hdc, INT x, INT y, BOOL bChecked);
VOID DrawMyShotEffect(HDC hdc, INT x, INT y, BOOL bChecked);
BOOL CheckMatrix(vector<vector<INT>> &iMatrix);
BOOL CheckPointShooted(vector<POINT> pShooted, POINT p);