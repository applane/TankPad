#pragma once
#include <cmath>
#include "GPadImpl.h"
#include "mix.h"

const float MOVE_DEAD_ZONE = 0.40f;
const int CAMERA_SENS_CENTER = 5;
const float CAMERA_DEAD_ZONE = 0.15f;

enum class EAction
{
	Move = 0,		// WASD
	Camera,			// Mouse move
	StartBattle,
	Fire,			// Left mouse 
	SniperMode,		// Shift
	ZoomIn,			// Wheel forward
	ZoomOut,		// Wheel backward
	AutoAim_HoldTurret,	// Right mouse / Hold right mouse
	AutoAimOff,		// E
	Shell1,			// 1
	Shell2,			// 2
	Shell3,			// 3
	Consumable1,	// 4
	Consumable2,	// 5
	Consumable3,	// 6
	Consumable4,	// 7
	Reload,			// C
	CruiseForward,	// R
	CruiseBackward,	// F
	Handbrake,		// Space
	Menu,			// ESC
	HidePlayers,
	MAX
};

struct SActionParams
{
	EAxis axis;
	float value;
	SActionParams(EAxis axis, float value)
	{
		this->axis = axis;
		this->value = value;
	}
};

class Mapping
{
public:
	Mapping();

	// properties
	bool		inverseY = false;
	int			cameraSensCoeff = CAMERA_SENS_CENTER;
	bool		singlePress = true;

	// gamepad event accepters
	void ButtonDown(EButton buttonID);
	void ButtonUp(EButton buttonID);
	void LeftStick(EAxis axis, float value);
	void RightStick(EAxis axis, float value);

	void LoadSettings();
	void SaveSettings();

	LPCTSTR GetActionName(int index);
	void SetActionButton(int actionIndex, EButton button);
	EButton GetActionButton(int actionIndex);
	EButton GetActionButton(EAction action);
	int GetActionCount() { return (int)EAction::MAX; }

	void ResetToDefaultButtons();

	bool IsDeadZone(float value) { return (abs(value) <= CAMERA_DEAD_ZONE); }

	// diagnostics
	int lastCameraMoveInPixels = 0;
protected:
	typedef void(Mapping::* ActionMethod)(SActionParams* params);

	struct SActionMap
	{
		EAction action;
		EButton button;
		EButton defButton;
		Mapping::ActionMethod actionDown;
		Mapping::ActionMethod actionUp;

		SActionMap(EAction action, EButton button, Mapping::ActionMethod actionDown,
			Mapping::ActionMethod actionUp = NULL)
		{
			this->action = action;
			this->button = button;
			defButton = button;
			this->actionDown = actionDown;
			this->actionUp = actionUp;
		}
	};

	EButton		moveStick = EButton::LeftStick;
	SActionMap map[EAction::MAX];
	bool moveLeftDown = false;
	bool moveRightDown = false;
	bool moveUpDown = false;
	bool moveDownDown = false;

	// move vehicle
	void Move(SActionParams* params);
	void HandleMoveStick(BYTE key, bool condition, bool& downValue);
	// move camera
	void MoveCamera(SActionParams* params);
	void MoveCameraLinear(SActionParams* params);
	void FireDown(SActionParams* params);
	void FireUp(SActionParams* params);
	void SniperMode(SActionParams* params);
	void ZoomIn(SActionParams* params);
	void ZoomOut(SActionParams* params);
	void AutoAimDown(SActionParams* params);
	void AutoAimUp(SActionParams* params);
	void AutoAimOff(SActionParams* params);
	void Shell1(SActionParams* params);
	void Shell2(SActionParams* params);
	void Shell3(SActionParams* params);
	void Consumable1(SActionParams* params);
	void Consumable2(SActionParams* params);
	void Consumable3(SActionParams* params);
	void Consumable4(SActionParams* params);
	void CruiseForward(SActionParams* params);
	void CruiseBackward(SActionParams* params);
	void HandbrakeDown(SActionParams* params);
	void HandbrakeUp(SActionParams* params);
	void Menu(SActionParams* params);
	void Reload(SActionParams* params);
	void StartBattle(SActionParams* params);
	void HidePlayers(SActionParams* params);

	SActionMap* GetAction(EButton button);
};

