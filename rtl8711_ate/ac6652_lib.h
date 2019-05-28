
extern "C" long __declspec(dllimport) __stdcall AC6652_DI(HANDLE hHandle,long ionum);
extern "C" long __declspec(dllimport) __stdcall AC6652_DO(HANDLE hHandle,long ionum,long iodata);

extern "C" HANDLE __declspec(dllimport) __stdcall AC6652_OpenDevice(long DeviceNum);
extern "C" long __declspec(dllimport) __stdcall AC6652_CloseDevice(HANDLE hHandle);

