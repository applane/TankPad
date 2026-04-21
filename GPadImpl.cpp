#include "pch.h"
#include "GPadImpl.h"
#include <atomic>

int GP_POOL_INTERVAL = 11;

const int DETECT_INTERVAL = 1000; // ms

bool dPadLeftDown = false;
bool dPadRightDown = false;
bool dPadUpDown = false;
bool dPadDownDown = false;

inline void HandleDPad(IGamepadCallback* pCallback, EButton buttonID, 
							bool condition, bool& downValue)
{
	if (condition)
	{
		if (!downValue)
			pCallback->ButtonDown(buttonID);
		downValue = true;
	}
	else if (downValue)
	{
		pCallback->ButtonUp(buttonID);
		downValue = false;
	}
}

const EButton dualShockButtonID[] =
{ EButton::Rect_X, EButton::X_A, EButton::Circle_B, EButton::Triangle_Y,
  EButton::L1, EButton::R1, EButton::L2, EButton::R2, EButton::Share, EButton::Options,
  EButton::L3, EButton::R3, EButton::PS, EButton::TouchDown
};

const EButton xboxButtonID[] =
{ EButton::DPadUp, EButton::DPadDown, EButton::DPadLeft, EButton::DPadRight,
  EButton::Options, EButton::Share, EButton::L3, EButton::R3, EButton::L1, EButton::R2,
  EButton::X_A, EButton::Circle_B, EButton::Rect_X, EButton::Triangle_Y, EButton::PS
};

#define DS_LSTICK_AXIS_H 3
#define DS_LSTICK_AXIS_V 2
#define DS_RSTICK_AXIS_H 1
#define DS_RSTICK_AXIS_V 0
#define DS_DPAD_AXIS_H 4
#define DS_DPAD_AXIS_V 5

#define XB_LSTICK_AXIS_H 0
#define XB_LSTICK_AXIS_V 1
#define XB_RSTICK_AXIS_H 2
#define XB_RSTICK_AXIS_V 3
#define XB_LTRIG_AXIS 4
#define XB_RTRIG_AXIS 5

UINT GamepadThread(LPVOID pParam);
bool IsDualShock(Gamepad_device* device);

void onButtonDown(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) 
{
	if (IsDualShock(device) && buttonID < sizeof(dualShockButtonID))
	{
		((IGamepadCallback*)(context))->ButtonDown(dualShockButtonID[buttonID]);
	}
	else if (buttonID < sizeof(xboxButtonID))
	{
		((IGamepadCallback*)(context))->ButtonDown(xboxButtonID[buttonID]);
	}
}

void onButtonUp(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) 
{
	if (IsDualShock(device) && buttonID < sizeof(dualShockButtonID))
	{
		((IGamepadCallback*)(context))->ButtonUp(dualShockButtonID[buttonID]);
	}
	else if (buttonID < sizeof(xboxButtonID))
	{
		((IGamepadCallback*)(context))->ButtonUp(xboxButtonID[buttonID]);
	}
}

void DualShockAxisMoved(struct Gamepad_device* device, unsigned int axisID, float value, IGamepadCallback* pCallback)
{
	// sticks
	if (axisID == DS_LSTICK_AXIS_V)
		pCallback->LeftStick(EAxis::Vertical, value);
	else if (axisID == DS_LSTICK_AXIS_H)
		pCallback->LeftStick(EAxis::Horizontal, value);
	else if (axisID == DS_RSTICK_AXIS_V)
		pCallback->RightStick(EAxis::Vertical, value);
	else if (axisID == DS_RSTICK_AXIS_H)
		pCallback->RightStick(EAxis::Horizontal, value);
	// D-PAD LEFT
	else if (axisID == DS_DPAD_AXIS_H)
	{
		HandleDPad(pCallback, EButton::DPadLeft, value < 0, dPadLeftDown);
		HandleDPad(pCallback, EButton::DPadRight, value > 0, dPadRightDown);
	}
	else if (axisID == DS_DPAD_AXIS_V)
	{
		HandleDPad(pCallback, EButton::DPadUp, value < 0, dPadUpDown);
		HandleDPad(pCallback, EButton::DPadDown, value > 0, dPadDownDown);
	}
}

void XboxAxisMoved(struct Gamepad_device* device, unsigned int axisID, float value, IGamepadCallback* pCallback)
{
	// sticks
	if (axisID == XB_LSTICK_AXIS_V)
		pCallback->LeftStick(EAxis::Vertical, value);
	else if (axisID == XB_LSTICK_AXIS_H)
		pCallback->LeftStick(EAxis::Horizontal, value);
	else if (axisID == XB_RSTICK_AXIS_V)
		pCallback->RightStick(EAxis::Vertical, value);
	else if (axisID == XB_RSTICK_AXIS_H)
		pCallback->RightStick(EAxis::Horizontal, value);
	// TRIGGERS
	else if (axisID == XB_LTRIG_AXIS) // left 0 - 1
	{
		HandleDPad(pCallback, EButton::L2, value > 0, dPadLeftDown);
	}
	else if (axisID == XB_RTRIG_AXIS) // right 0 - 1
	{
		HandleDPad(pCallback, EButton::R2, value > 0, dPadRightDown);
	}
}


void onAxisMoved(struct Gamepad_device* device, unsigned int axisID, float value, float lastValue, double timestamp, void* context) 
{
	IGamepadCallback* pCallback = (IGamepadCallback*)(context);

	if (IsDualShock(device)) // PS Dualshock
	{
		DualShockAxisMoved(device, axisID, value, pCallback);
	}
	else // XBOX
	{
		XboxAxisMoved(device, axisID, value, pCallback);
	}
}

void onDeviceAttached(struct Gamepad_device* device, void* context) 
{
	if (!context) return;
	if (Gamepad_numDevices() > 1) return; // only one gamepad

	SGamepadData data;

	data.isDualShock = IsDualShock(device);

	if (IsDualShock(device))
	{
		data.description = _T("DualShock compatible");
	}
	else 
		data.description = _T("Xbox compatible");

	((IGamepadCallback*)(context))->DeviceAttached(data);
}

void onDeviceRemoved(Gamepad_device* device, void* context) 
{
	if (!context) return;
	if (Gamepad_numDevices() > 1) return; // only one gamepad

	((IGamepadCallback*)(context))->DeviceRemoved();
}

bool IsDualShock(Gamepad_device* device)
{
	return !(device->vendorID == 0x45E); // xbox hardcoded XInput

}

static double currentTime() 
{
	static LARGE_INTEGER frequency;
	LARGE_INTEGER currentTime;

	if (frequency.QuadPart == 0) {
		QueryPerformanceFrequency(&frequency);
	}
	QueryPerformanceCounter(&currentTime);

	return (double)currentTime.QuadPart / frequency.QuadPart;
}

void InitGamepad(IGamepadCallback* pCallback)
{
	AfxBeginThread(GamepadThread, pCallback);
}

int processEvents = TRUE;
static CEvent gamepadThreadExited;

UINT GamepadThread(LPVOID pParam)
{
	IGamepadCallback* pCallback = (IGamepadCallback*)pParam;

	Gamepad_deviceAttachFunc(onDeviceAttached, (void*)pCallback);
	Gamepad_deviceRemoveFunc(onDeviceRemoved, (void*)pCallback);
	Gamepad_buttonDownFunc(onButtonDown, (void*)pCallback);
	Gamepad_buttonUpFunc(onButtonUp, (void*)pCallback);
	Gamepad_axisMoveFunc(onAxisMoved, (void*)pCallback);
	Gamepad_init();

	int needDetect = 0;

	double prevTime = currentTime();

	while (processEvents)
	{
		Sleep(GP_POOL_INTERVAL);

		if (needDetect >= DETECT_INTERVAL && Gamepad_numDevices() == 0)
		{
			Gamepad_detectDevices();
			needDetect = 0;
		}

		Gamepad_processEvents();

		needDetect += GP_POOL_INTERVAL;
	}

	gamepadThreadExited.SetEvent();

	TRACE(L"Gamepad Thread Exited\n");
	return TRUE;
}

void ShutdownGamepad()
{
	processEvents = FALSE;

	CSingleLock lock(&gamepadThreadExited);
	TRACE(L"Wainting for Gamepad Thread\n");

	Gamepad_shutdown();
}