#pragma once
#include "gamepad.h"

extern int GP_POOL_INTERVAL;

enum class EButton
{
	None = 0,
	DPadUp,
	DPadDown,
	DPadLeft,
	DPadRight,
	R1,
	R2,
	R3,
	L1,
	L2,
	L3,
	Options,
	Share,
	Triangle_Y,
	X_A,
	Rect_X,
	Circle_B,
	PS,
	TouchDown,
	LeftStick,
	RightStick,
	MAX
};

enum class EAxis
{
	Vertical,
	Horizontal
};

struct SGamepadData
{
	CString description;
	bool isDualShock;
	int buttonCount;
	const int* buttonID; // array[buttonCount] 
};

class IGamepadCallback
{
public:
	virtual void DeviceAttached(SGamepadData& device) = 0;
	virtual void DeviceRemoved() = 0;
	virtual bool NeedDetectDevice() = 0;
	virtual void ButtonDown(EButton buttonID) = 0;
	virtual void ButtonUp(EButton buttonID) = 0;
	virtual void LeftStick(EAxis axis, float value) = 0;
	virtual void RightStick(EAxis axis, float value) = 0;
};

void InitGamepad(IGamepadCallback *pCallback);
void ShutdownGamepad();
