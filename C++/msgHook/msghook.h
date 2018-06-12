#ifndef MSGHOOK_H
#define MSGHOOK_H

#include "msghook_global.h"
#include <Windows.h>

//class MSGHOOKSHARED_EXPORT MsgHook
//{

//public:
//    MsgHook();
//};

extern "C"
{

MSGHOOKSHARED_EXPORT int InstallCbtHook(HWND hwnd);

MSGHOOKSHARED_EXPORT int UnInstallCbtHook();

MSGHOOKSHARED_EXPORT LRESULT CALLBACK CBTProc(
  _In_ int    nCode,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
);

}

#endif // MSGHOOK_H
