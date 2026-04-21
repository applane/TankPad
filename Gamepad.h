#ifndef __GAMEPAD_H__
#define __GAMEPAD_H__
#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER <= 1600
#define bool int
#define true 1
#define false 0
#else
#include <stdbool.h>
#endif

struct Gamepad_device {
	unsigned int deviceID;
	const char * description;
	int vendorID;
	int productID;
	unsigned int numAxes;
	unsigned int numButtons;
	float * axisStates;
	bool * buttonStates;
	void * privateData;
};

void Gamepad_init();
void Gamepad_shutdown();
unsigned int Gamepad_numDevices();
struct Gamepad_device * Gamepad_deviceAtIndex(unsigned int deviceIndex);
void Gamepad_detectDevices();
void Gamepad_processEvents();
void Gamepad_deviceAttachFunc(void (* callback)(struct Gamepad_device * device, void * context), void * context);
void Gamepad_deviceRemoveFunc(void (* callback)(struct Gamepad_device * device, void * context), void * context);
void Gamepad_buttonDownFunc(void (* callback)(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context), void * context);
void Gamepad_buttonUpFunc(void (* callback)(struct Gamepad_device * device, unsigned int buttonID, double timestamp, void * context), void * context);
void Gamepad_axisMoveFunc(void (* callback)(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, double timestamp, void * context), void * context);

#ifdef __cplusplus
}
#endif
#endif
