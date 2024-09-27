#include "X20Import.h"
//#include "framework.h"
X20Import::X20Import()
{
	hDLLGOX20 = NULL;
	handle = -1;
}

X20Import::~X20Import()
{
}

int __stdcall X20Import::init(char* deviceName)
{
	int l_rc = 0;
	if (handle > 0)
		close();
	if (getProcAddresses(&hDLLGOX20, "GOMDX20.dll", 10,
		&GOMDX20_setPassword, "GOMDX20_setPassword",
		&GOMDX20_getHandle, "GOMDX20_getHandle",
		&GOMDX20_releaseHandle, "GOMDX20_releaseHandle",
		&GOMDX20_selectCalibrationEntry, "GOMDX20_selectCalibrationEntry",
		&GOMDX20_getSelectedCalibrationEntry, "GOMDX20_getSelectedCalibrationEntry",
		&GOMDX20_getCalibrationEntryName, "GOMDX20_getCalibrationEntryName",
		&GOMDX20_measureCW, "GOMDX20_measureCW",
		&GOMDX20_getCWValue, "GOMDX20_getCWValue",
		&GOMDX20_getChannelUnitNumber, "GOMDX20_getChannelUnitNumber",
		&GOMDX20_getUnitString, "GOMDX20_getUnitString"
		))
	{
		try {
			l_rc = GOMDX20_setPassword("passw"); //replace passw with the right password
			if (l_rc == 0)
			{
				l_rc = GOMDX20_getHandle(deviceName, &handle);
			}
		}
		catch (...) {
			l_rc = -1;
		}
	}
	else {
		l_rc = -1;
	}
	return l_rc;
}

int __stdcall X20Import::writeCalibrationInfoToConsole()
{
	char calibInfo[100];
	std::cout << "Available calibration entries:" << std::endl;
	for (int i = 0; i < 512; i++)
	{
		int ret = GOMDX20_getCalibrationEntryName(handle, i, calibInfo);
		if (ret == 0)
		{
			std::cout << i << ": " << calibInfo << std::endl;
		}
	}
	return 0;
}

int __stdcall X20Import::setCalibrationEntry(int value)
{
	int l_rc = GOMDX20_selectCalibrationEntry(handle, value);
	return l_rc;
}

int __stdcall X20Import::measure(int channel)
{
	int l_rc = GOMDX20_measureCW(handle, channel);
	return l_rc;
}

int __stdcall X20Import::integralGetValues(int channel, double* value, char* unit)
{
	int l_rc = GOMDX20_getCWValue(handle, channel, value);
	if (l_rc < 0)
		return l_rc;

	int unitNumber;
	l_rc = GOMDX20_getChannelUnitNumber(handle, channel, &unitNumber);
	if (l_rc < 0)
		return l_rc;

	l_rc = GOMDX20_getUnitString(handle, unitNumber, false, unit);
	return l_rc;
}

int __stdcall X20Import::close()
{
	int l_rc = GOMDX20_releaseHandle(handle);
	handle = -1;
	return l_rc;
}

bool __stdcall X20Import::getProcAddresses(HINSTANCE *p_hLibrary,
	const char* p_dllName, INT p_count, ...)
{
	//CString dllstr(p_dllName);
	va_list l_va;
	va_start(l_va, p_count);
	//if ((*p_hLibrary = LoadLibrary(p_dllName)) != NULL)
	if(1)
	{
		FARPROC* l_procFunction = NULL;
		char* l_funcName = NULL;
		int l_idxCount = 0;
		while (l_idxCount < p_count)
		{
			l_procFunction = va_arg(l_va, FARPROC*);
			l_funcName = va_arg(l_va, LPSTR);
			if ((*l_procFunction =
				GetProcAddress(*p_hLibrary, l_funcName)) == NULL)
			{
				l_procFunction = NULL;
				return FALSE;
			}
			l_idxCount++;
		}
	}
	else
	{
		va_end(l_va);
		return false;
	}
	va_end(l_va);
	return true;
}