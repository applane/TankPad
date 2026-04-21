#pragma once

enum class ELocText
{
	ActionColumn,
	ButtonColumn,
	CameraSens,
	ResetDefault,
	ResetDefaultQ,
	CloseAppQ,
	GamepadNotFound,
	GamepadLabel,
	InvertCameraY,
	AssignButton,
	AssignButtonTip,
	SvgAssignHelp,
	SingleKey,
	MAX
};

void SetCurrentLanguage(BYTE lang);

LPCTSTR GetActionName(EAction action);

LPCTSTR GetButtonName(EButton button);

EButton GetButtonByName(LPCTSTR localName);

LPCTSTR GetLocalText(ELocText id);