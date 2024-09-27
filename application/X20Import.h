#ifndef X20ImportH
#define X20ImportH

#include <Windows.h>
#include "stdio.h"
#include <iostream>

class X20Import
{
public:
	X20Import();
	virtual ~X20Import();
	int __stdcall init(char* deviceName);
	int __stdcall close();
	int __stdcall writeCalibrationInfoToConsole();
	int __stdcall setCalibrationEntry(int value);
	int __stdcall integralGetValues(int channel, double* value, char* unit);
	int __stdcall measure(int channel);

private:
	int handle;
	HINSTANCE hDLLGOX20;
	bool __stdcall getProcAddresses(HINSTANCE *p_hLibrary, const char* p_dllName, int p_count, ...);

	int(__stdcall *GOMDX20_setPassword)(char* password);
	int(__stdcall *GOMDX20_getHandle)(char* deviceName, int* handle);
	int(__stdcall *GOMDX20_releaseHandle)(int handle);

	int(__stdcall *GOMDX20_selectCalibrationEntry)(int handle, int calibrationEntryNumber);
	int(__stdcall *GOMDX20_getSelectedCalibrationEntry)(int handle, int* calibrationEntryNumber);
	int(__stdcall *GOMDX20_getCalibrationEntryName)(int handle, int calibrationEntryNumber, char* calibrationName);

	int(__stdcall *GOMDX20_measureCW)(int handle, int channelNumber);
	int(__stdcall *GOMDX20_getCWValue)(int handle, int channelNumber, double* value);
	int(__stdcall *GOMDX20_getChannelUnitNumber)(int handle, int channelNumber, int* unitNumber);
	int(__stdcall *GOMDX20_getUnitString)(int handle, int unitNumber, bool isDosis, char* unit);
};
#endif