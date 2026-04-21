#include "pch.h"
#include "Mapping.h"
#include "mix.h"
#include "Language.h"
#include <string>

float CAMERA_MOVE_MAX = 10.0;

#define KEY_A 0x41
#define KEY_D 0x44
#define KEY_W 0x57
#define KEY_S 0x53
#define KEY_R 0x52
#define KEY_F 0x46
#define KEY_C 0x43
#define KEY_E 0x45
#define KEY_1 0x31
#define KEY_2 0x32
#define KEY_3 0x33
#define KEY_4 0x34
#define KEY_5 0x35
#define KEY_6 0x36
#define KEY_7 0x37

void PressKey(BYTE key);
void PressKeyDown(BYTE key);
void PressKeyUp(BYTE key);
void PressCtrlKey(BYTE key);
void MouseLeftClick();
void MouseLeftDown();
void MouseLeftUp();
void MouseMove(int dx, int dy);
void MouseWheelForward();
void MouseWheelBackward();
void MouseRightUp();
void MouseRightDown();

Mapping::Mapping() : 
map 
{
	{EAction::Move, EButton::LeftStick, NULL},
	{EAction::Camera, EButton::RightStick, NULL},
	{EAction::StartBattle, EButton::PS, &Mapping::StartBattle},
	{EAction::Fire, EButton::R2, &Mapping::FireDown, &Mapping::FireUp},
	{EAction::SniperMode, EButton::L2, &Mapping::SniperMode},
	{EAction::ZoomIn, EButton::R3, &Mapping::ZoomIn},
	{EAction::ZoomOut, EButton::L3, &Mapping::ZoomOut},
	{EAction::AutoAim_HoldTurret, EButton::R1, &Mapping::AutoAimDown, &Mapping::AutoAimUp},
	{EAction::AutoAimOff, EButton::None, &Mapping::AutoAimOff},
	{EAction::Shell1, EButton::DPadLeft, &Mapping::Shell1},
	{EAction::Shell2, EButton::DPadUp, &Mapping::Shell2},
	{EAction::Shell3, EButton::DPadRight, &Mapping::Shell3},
	{EAction::Consumable1, EButton::Rect_X, &Mapping::Consumable1},
	{EAction::Consumable2, EButton::Triangle_Y, &Mapping::Consumable2},
	{EAction::Consumable3, EButton::Circle_B, &Mapping::Consumable3},
	{EAction::Consumable4, EButton::None, &Mapping::Consumable4},
	{EAction::Reload, EButton::X_A, &Mapping::Reload},
	{EAction::CruiseForward, EButton::DPadDown, &Mapping::CruiseForward},
	{EAction::CruiseBackward, EButton::None, &Mapping::CruiseBackward},
	{EAction::Handbrake, EButton::L1, &Mapping::HandbrakeDown, &Mapping::HandbrakeUp},
	{EAction::Menu, EButton::Options, &Mapping::Menu},
	{EAction::HidePlayers, EButton::None, &Mapping::HidePlayers}
}
{}

void Mapping::ButtonDown(EButton buttonID)
{
	SActionMap* action = GetAction(buttonID);
	if (action == NULL || action->actionDown == NULL) return;

	(*this.*action->actionDown)(NULL);
}

void Mapping::ButtonUp(EButton buttonID)
{
	SActionMap* action = GetAction(buttonID);
	if (action == NULL || action->actionUp == NULL) return;

	(*this.*action->actionUp)(NULL);
}

Mapping::SActionMap* Mapping::GetAction(EButton button)
{
	for (int i = 0; i < (int)EAction::MAX; i++)
	{
		if (map[i].button == button)
			return &map[i];
	}

	return NULL;
}

void Mapping::LeftStick(EAxis axis, float value)
{
	if (moveStick == EButton::LeftStick)
		Move(&SActionParams(axis, value));
	else
		MoveCamera(&SActionParams(axis, value));
}

void Mapping::RightStick(EAxis axis, float value)
{
	if (moveStick == EButton::RightStick)
		Move(&SActionParams(axis, value));
	else
		MoveCamera(&SActionParams(axis, value));
}

void Mapping::Move(SActionParams* params)
{
	if (params->axis == EAxis::Horizontal)
	{
		HandleMoveStick(KEY_A, params->value <= -MOVE_DEAD_ZONE, moveLeftDown);
		HandleMoveStick(KEY_D, params->value >= MOVE_DEAD_ZONE, moveRightDown);
	}
	else
	{
		HandleMoveStick(KEY_W, params->value <= -MOVE_DEAD_ZONE, moveUpDown);
		HandleMoveStick(KEY_S, params->value >= MOVE_DEAD_ZONE, moveDownDown);
	}
}

void Mapping::MoveCamera(SActionParams* params)
{
	float CAMERA_EXP_A = 1.030f;
	CAMERA_EXP_A += (float)(cameraSensCoeff-CAMERA_SENS_CENTER) / 1000.0f;

	float value = abs(params->value);

	if (value <= CAMERA_DEAD_ZONE) return;

	// speed up camera movement at max positions of the stick
	if (value > 0.99) CAMERA_EXP_A += 0.007f;

	// get LIVE zone
	float liveZoneValue = abs(params->value) - CAMERA_DEAD_ZONE;

	// exponental movement depending on stick offset from its center
	liveZoneValue = pow(CAMERA_EXP_A, liveZoneValue * 100.0f);

	// restore sign 
	liveZoneValue = (params->value < 0) ? -liveZoneValue : liveZoneValue;

	int distanceInPixels = (int)(liveZoneValue);

	if (params->axis == EAxis::Horizontal)
	{
		MouseMove(distanceInPixels, 0);
	}
	else
	{
		if (inverseY)
			distanceInPixels = -distanceInPixels;

		MouseMove(0, distanceInPixels);
	}

	lastCameraMoveInPixels = distanceInPixels;
}

void Mapping::MoveCameraLinear(SActionParams* params)
{
	if (abs(params->value) <= CAMERA_DEAD_ZONE) return;

	float goodZoneValue = (params->value < 0) ? (params->value + CAMERA_DEAD_ZONE) :
												(params->value - CAMERA_DEAD_ZONE);

	if (params->axis == EAxis::Horizontal)
	{
		MouseMove((int)round(goodZoneValue * CAMERA_MOVE_MAX), 0);
	}
	else
	{
		if (inverseY)
			goodZoneValue = -goodZoneValue;

		MouseMove(0, (int)round(goodZoneValue * CAMERA_MOVE_MAX));
	}
}

void Mapping::HandleMoveStick(BYTE key, bool condition, bool& downValue)
{
	if (condition)
	{
		PressKeyDown(key);
		downValue = true;
	}
	else if (downValue)
	{
		PressKeyUp(key);
		downValue = false;
	}
}

void Mapping::StartBattle(SActionParams* params)
{
	int sx = GetSystemMetrics(SM_CXFULLSCREEN);
	int sy = GetSystemMetrics(SM_CYFULLSCREEN);
	
	CPoint pos;
	GetCursorPos(&pos);

	SetCursorPos(sx / 2, 15);

	MouseLeftClick();

	SetCursorPos(sx /2, (int)((float)sy * 5.f / 6.f));
}

void Mapping::FireDown(SActionParams* params)
{
	MouseLeftDown();
}

void Mapping::FireUp(SActionParams* params)
{
	MouseLeftUp();
}

void Mapping::SniperMode(SActionParams* params)
{
	PressKey(VK_SHIFT);
}

void Mapping::ZoomIn(SActionParams* params)
{
	MouseWheelForward();
}

void Mapping::ZoomOut(SActionParams* params)
{
	MouseWheelBackward();
}

void Mapping::AutoAimDown(SActionParams* params)
{
	MouseRightDown();
}

void Mapping::AutoAimUp(SActionParams* params)
{
	MouseRightUp();
}

void Mapping::AutoAimOff(SActionParams* params)
{
	PressKey(KEY_E);
}

void Mapping::Shell1(SActionParams* params)
{
	PressKey(KEY_1);
	if (singlePress)	PressKey(KEY_1);
}

void Mapping::Shell2(SActionParams* params)
{
	PressKey(KEY_2);
	if (singlePress)	PressKey(KEY_2);
}

void Mapping::Shell3(SActionParams* params)
{
	PressKey(KEY_3);
	if (singlePress)	PressKey(KEY_3);
}

void Mapping::Consumable1(SActionParams* params)
{
	PressKey(KEY_4);
}

void Mapping::Consumable2(SActionParams* params)
{
	PressKey(KEY_5);
}

void Mapping::Consumable3(SActionParams* params)
{
	PressKey(KEY_6);
}

void Mapping::Consumable4(SActionParams* params)
{
	PressKey(KEY_7);
}

void Mapping::CruiseForward(SActionParams* params)
{
	PressKey(KEY_R);
	PressKey(KEY_R);
	PressKey(KEY_R);
}

void Mapping::CruiseBackward(SActionParams* params)
{
	PressKey(KEY_F);
	PressKey(KEY_F);
	PressKey(KEY_F);
}

void Mapping::HandbrakeDown(SActionParams* params)
{
	PressKeyDown(VK_SPACE);
}

void Mapping::HandbrakeUp(SActionParams* params)
{
	PressKeyUp(VK_SPACE);
}

void Mapping::Menu(SActionParams* params)
{
	PressKey(VK_ESCAPE);
}

void Mapping::Reload(SActionParams* params)
{
	PressKey(KEY_C);
}

void Mapping::HidePlayers(SActionParams* params)
{
	PressCtrlKey(VK_TAB);
	PressCtrlKey(VK_TAB);
	PressCtrlKey(VK_TAB);
	PressCtrlKey(VK_TAB);
}

void Mapping::SaveSettings()
{
	std::wstring actionId;

	// main
	WriteConfigInt(_T("moveStick"), (int)moveStick);
	WriteConfigInt(_T("cameraSensPlus"), (int)cameraSensCoeff);
	WriteConfigBool(_T("inverseY"), inverseY);
	WriteConfigBool(_T("singlePress"), singlePress);

	// map
	for (int i = 0; i < (int)EAction::MAX; i++)
	{
		actionId = L"Action" + std::to_wstring(i);
		WriteConfigInt(actionId.c_str(), (int)map[i].button);
	}
}

void Mapping::LoadSettings()
{
	std::wstring actionId;

	// main
	moveStick = (EButton)ReadConfigInt(_T("moveStick"), (int)EButton::LeftStick);
	cameraSensCoeff = ReadConfigInt(_T("cameraSensPlus"), cameraSensCoeff);
	inverseY = ReadConfigBool(_T("inverseY"), inverseY);
	singlePress = ReadConfigBool(_T("singlePress"), singlePress);

	// map
	for (int i = 0; i < (int)EAction::MAX; i++)
	{
		actionId = L"Action" + std::to_wstring(i);
		map[i].button = (EButton)ReadConfigInt(actionId.c_str(), (int)map[i].button);
	}
}

LPCTSTR Mapping::GetActionName(int index)
{
	return ::GetActionName((EAction)index);
}

void Mapping::SetActionButton(int actionIndex, EButton button)
{
	map[actionIndex].button = button;
}

EButton Mapping::GetActionButton(int actionIndex)
{
	return map[actionIndex].button;
}

EButton Mapping::GetActionButton(EAction action)
{
	return GetActionButton((int)action);
}

void Mapping::ResetToDefaultButtons()
{
	for (int i = 0; i < GetActionCount(); i++)
	{
		map[i].button = map[i].defButton;
	}
}

int GetActionCount() { return (int)EAction::MAX; }

///////////////////////////////////////////////////////////////////////////////
void PressKey(BYTE key)
{
	keybd_event(key, MapVirtualKey(key, 0), 0, NULL);
	keybd_event(key, MapVirtualKey(key, 0), KEYEVENTF_KEYUP, NULL);
}

void PressCtrlKey(BYTE key)
{
	PressKeyDown(VK_LCONTROL);
	PressKey(key);
	PressKeyUp(VK_LCONTROL);
}

void PressKeyDown(BYTE key)
{
	keybd_event(key, MapVirtualKey(key, 0), 0, NULL);
}

void PressKeyUp(BYTE key)
{
	keybd_event(key, MapVirtualKey(key, 0), KEYEVENTF_KEYUP, NULL);
}

void MouseLeftDown()
{
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
}

void MouseLeftUp()
{
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
}

void MouseRightUp()
{
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
}

void MouseRightDown()
{
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
}

void MouseMove(int dx, int dy)
{
	mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, NULL);
}

void MouseWheelForward()
{
	mouse_event(MOUSEEVENTF_WHEEL, 0, 0, WHEEL_DELTA, NULL);
}

void MouseWheelBackward()
{
	mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -WHEEL_DELTA, NULL);
}

void MouseLeftClick()
{
	MouseLeftDown();
	Sleep(10);
	MouseLeftUp();
}
