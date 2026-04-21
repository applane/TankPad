#include "pch.h"
#include "Mapping.h"
#include "Language.h"
#include "gpadimpl.h"

static BYTE g_language = GetUserDefaultUILanguage() & 0x00FF; // LANG_RUSSIAN

static LPCTSTR localENG[(int)ELocText::MAX] =
{
	_T("Game Action"),
	_T("Gamepad Button (PS / Xbox)"),
	L"Camera Sensitivity",
	L"Reset...",
	L"Reset the buttons to the default settings?",
	L"Close the application?",
	L"Not detected",
	L"Gamepad:",
	L"Invert Y (it is recommended to use the game inversion settings)",
	L"Assign Button",
	L"Press a button or move a stick of the attached gamepad to assign it to the game action",
	L"Select a button to assign it to the game action.",
	L"Single press for shell type selection"
};

static LPCTSTR localRUS[(int)ELocText::MAX] =
{
	_T("Игровое действие"),
	_T("Кнопка геймпада (PS / Xbox)"),
	L"Чувствительность",
	L"По умолчанию...",
	L"Восстановить значения кнопок по умолчанию?",
	L"Закрыть приложение?",
	L"Не найден",
	L"Геймпад:",
	L"Вертик. инверсия (лучше использовать настройки инверсии игры)",
	L"Назначить кнопку",
	L"Нажмите кнопку или сдвиньте джойстик подсоединенного геймпада, чтобы назначить их для игрового действия",
	L"Выберите кнопку чтобы назначить ее для игрового действия.",
	L"Однократное нажатие для выбора типа снаряда"
};

static LPCTSTR actionENG[(int)EAction::MAX] =
{
	_T("Movement (WASD)"),
	_T("Camera (mouse)"),
	_T("Start Battle"),
	_T("Fire (LMB)"),
	_T("Sniper Mode"),
	_T("Zoom In"),
	_T("Zoom Out"),
	_T("Autoaim / Hold Turret (RMB)"),
	_T("Autoaim Off"),
	_T("Shell Standard (1)"),
	_T("Shell Premium (2)"),
	_T("Shell Hesh (3)"),
	_T("Consumable (4)"),
	_T("Consumable (5)"),
	_T("Consumable (6)"),
	_T("Consumable (7)"),
	_T("Gun Reload"),
	_T("Cruise-control Forward"),
	_T("Cruise-control Backward"),
	_T("Handbrake"),
	_T("Options Menu (Esc)"),
	L"Hide players list (Ctrl-Tab)"
};

static LPCTSTR actionRUS[(int)EAction::MAX] =
{
	_T("Передвижение (WASD)"),
	_T("Камера (mouse)"),
	_T("В бой!"),
	_T("Выстрелить (LBM)"),
	_T("Снайперский прицел"),
	_T("Приблизить"),
	_T("Отдалить"),
	_T("Автоприцел/удерж. башни (RMB)"),
	_T("Отключить автоприцел"),
	_T("Снаряд обычный (1)"),
	_T("Снаряд премиум (2)"),
	_T("Снаряд фугасный (3)"),
	_T("Снаряжение (4)"),
	_T("Снаряжение (5)"),
	_T("Снаряжение (6)"),
	_T("Снаряжение (7)"),
	_T("Перезагрузить кассету"),
	_T("Круиз-контроль вперед"),
	_T("Круиз-контроль назад"),
	_T("Ручной тормоз"),
	_T("Меню (Esc)"),
	L"Скрыть список игроков (Ctrl-Tab)"
};

static LPCTSTR buttonENG[(int)EButton::MAX] =
{
	_T("    "),
	_T("D-Pad Up"),
	_T("D-Pad Down"),
	_T("D-Pad Left"),
	_T("D-Pad Right"),
	_T("R1 / Right Bumper"),
	_T("R2 / Right Trigger"),
	_T("R3 / Right Stick Click"),
	_T("L1 / Left Bumper"),
	_T("L2 / Left Trigger"),
	_T("L3 / Left Stick Click"),
	_T("Options / Menu"),
	_T("Share / View"),
	_T("Triangle / Y"),
	_T("X / A"),
	_T("Square / X"),
	_T("Circle / B"),
	_T("PS / Xbox"),
	_T("Touch-Panel Press"),
	_T("Left Stick"),
	_T("Right Stick")
};

static LPCTSTR buttonRUS[(int)EButton::MAX] =
{
	_T("    "),
	_T("Перекрестие вверх"),
	_T("Перекрестие вниз"),
	_T("Перекрестие влево"),
	_T("Перекрестие вправо"),
	_T("R1 / Правый бампер"),
	_T("R2 / Правый триггер"),
	_T("R3 / Нажать правый джойстик"),
	_T("L1 / Левый бампер"),
	_T("L2 / Левый триггер"),
	_T("L3 / Нажать левый джойстик"),
	_T("Options / Menu"),
	_T("Share / View"),
	_T("Треугольник / Y"),
	_T("X / A"),
	_T("Квадрат / X"),
	_T("Круг / B"),
	_T("PS / X-Box"),
	_T("Нажать тач-панель"),
	_T("Левый джойстик"),
	_T("Правый джойстик")
};

LPCTSTR GetButtonName(EButton button)
{
	switch (g_language)
	{
	case LANG_RUSSIAN:
		return buttonRUS[(int)button];
	}

	return buttonENG[(int)button];
}

LPCTSTR GetLocalText(ELocText id)
{
	switch (g_language)
	{
	case LANG_RUSSIAN:
		return localRUS[(int)id];
	}

	return localENG[(int)id];
}

LPCTSTR GetActionName(EAction action)
{
	switch (g_language)
	{
		case LANG_RUSSIAN: 
			return actionRUS[(int)action];
	}

	return actionENG[(int)action];
}

EButton GetButtonByName(LPCTSTR localName)
{
	for (int i = 0; i < (int)EButton::MAX; i++)
	{
		if (_tcscmp(GetButtonName((EButton)i), localName) == 0)
			return (EButton)i;
	}

	return EButton::None;
}

void SetCurrentLanguage(BYTE lang)
{
	switch (lang)
	{
	case LANG_ENGLISH: g_language = lang; break;
	case LANG_RUSSIAN: g_language = lang; break;
	}
}