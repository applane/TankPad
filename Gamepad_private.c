#include "Gamepad.h"
#include "Gamepad_private.h"
#include <stdlib.h>

void (* Gamepad_deviceAttachCallback)(struct Gamepad_device * device, void * context) = NULL;
void (* Gamepad_deviceRemoveCallback)(struct Gamepad_device * device, void * context) = NULL;
void (* Gamepad_buttonDownCallback)(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context) = NULL;
void (* Gamepad_buttonUpCallback)(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context) = NULL;
void (* Gamepad_axisMoveCallback)(struct Gamepad_device * device, unsigned int buttonID, float value, float lastValue, double timestamp, void * context) = NULL;
void * Gamepad_deviceAttachContext = NULL;
void * Gamepad_deviceRemoveContext = NULL;
void * Gamepad_buttonDownContext = NULL;
void * Gamepad_buttonUpContext = NULL;
void * Gamepad_axisMoveContext = NULL;

void Gamepad_deviceAttachFunc(void (* callback)(struct Gamepad_device * device, void * context), void * context) {
	Gamepad_deviceAttachCallback = callback;
	Gamepad_deviceAttachContext = context;
}

void Gamepad_deviceRemoveFunc(void (* callback)(struct Gamepad_device * device, void * context), void * context) {
	Gamepad_deviceRemoveCallback = callback;
	Gamepad_deviceRemoveContext = context;
}

void Gamepad_buttonDownFunc(void (* callback)(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context), void * context) {
	Gamepad_buttonDownCallback = callback;
	Gamepad_buttonDownContext = context;
}

void Gamepad_buttonUpFunc(void (* callback)(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context), void * context) {
	Gamepad_buttonUpCallback = callback;
	Gamepad_buttonUpContext = context;
}

void Gamepad_axisMoveFunc(void (* callback)(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, double timestamp, void * context), void * context) {
	Gamepad_axisMoveCallback = callback;
	Gamepad_axisMoveContext = context;
}
