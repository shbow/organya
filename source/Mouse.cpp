#include "Setting.h"
#include "Mouse.h"
void MouseData::GetMousePosition(long *x,long *y)
{
	GetCursorPos(&point);
	ScreenToClient(hWnd,&point);
	*x = point.x;
	*y = point.y;
}
