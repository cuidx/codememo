#include "msghook.h"


//MsgHook::MsgHook()
//{
//}

#pragma data_seg("msghook_share_data")
HWND g_HwndNotify = NULL;
UINT    g_RegMsg = 0;
#pragma data_seg()

#pragma comment(linker,"/section:msghook_share_data,rws")
#pragma comment(lib,"User32.lib")

HINSTANCE g_hModule = NULL;
HHOOK	g_CbtHook = NULL;
HHOOK   g_CWPHook = NULL;

//UINT    g_RegMsg = RegisterWindowMessage(L"msghook_reg_msg");

LRESULT CALLBACK CallWndProc(
  _In_ int    nCode,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
)
{
    LPCWPSTRUCT pStruct = (LPCWPSTRUCT)lParam;
    char lszMsg[1024] = { 0 };
    if (WM_WINDOWPOSCHANGING == pStruct->message || WM_WINDOWPOSCHANGED == pStruct->message
            || WM_DESTROY == pStruct->message)
    {
        sprintf(lszMsg,"receive CallWndProc window (%llx) message (%x)\n",(long long)pStruct->hwnd,pStruct->message);
        ::OutputDebugStringA(lszMsg);
        if (::IsWindow(g_HwndNotify))
        {
            ::PostMessageA(g_HwndNotify ,g_RegMsg,(WPARAM)pStruct->message,(LPARAM)pStruct->hwnd);
            //DWORD dwInfo = BSM_APPLICATIONS;
            //::BroadcastSystemMessageA(BSF_IGNORECURRENTTASK,&dwInfo,WM_USER + 1000,wParam,lParam);
        }
    }
    return CallNextHookEx(NULL,nCode,wParam,lParam);
}

LRESULT CALLBACK CBTProc(
  _In_ int    nCode,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
)
{
    switch (nCode)
    {
    case HCBT_DESTROYWND:
        //if (g_Hwnd == (HWND)wParam)
        {
            char lszMessage[1024] = { 0 };
            sprintf(lszMessage,"wnd (%llx) destroyed,g_HwndNotify(%llx),g_RegMsg(%x)\n",(long long)wParam,(long long)g_HwndNotify,g_RegMsg);
            ::OutputDebugStringA(lszMessage);
            //MessageBoxA(NULL,lszMessage,"CBTProc",MB_OK);
            if (::IsWindow(g_HwndNotify))
            {
                ::PostMessageA(g_HwndNotify ,g_RegMsg,nCode,wParam);
                //DWORD dwInfo = BSM_APPLICATIONS;
                //::BroadcastSystemMessageA(BSF_IGNORECURRENTTASK,&dwInfo,WM_USER + 1000,wParam,lParam);
            }
        }
        break;
    case HCBT_MOVESIZE:
        //if (g_Hwnd == (HWND)wParam)
        {
            RECT *pRect = (RECT*)lParam;
            char lszMessage[1024] = { 0 };
            sprintf(lszMessage,"wnd (%llx) pos/size change (%d,%d,%d,%d),g_HwndNotify(%llx),g_RegMsg(%x)\n",
                    (long long)wParam,pRect->left,pRect->top,pRect->right,pRect->bottom,(long long)g_HwndNotify,g_RegMsg);
            ::OutputDebugStringA(lszMessage);
            //MessageBoxA(NULL,lszMessage,"CBTProc",MB_OK);
            if (::IsWindow(g_HwndNotify))
            {
                ::PostMessageA(g_HwndNotify ,g_RegMsg,nCode,wParam);
                //DWORD dwInfo = BSM_APPLICATIONS;
                //::BroadcastSystemMessageA(BSF_IGNORECURRENTTASK,&dwInfo,WM_USER + 1001,wParam,lParam);
            }
        }
        break;
    case HCBT_MINMAX:
        {
            char lszMessage[1024] = { 0 };
            sprintf(lszMessage,"wnd (%llx) minmax (%lld),g_HwndNotify(%llx),g_RegMsg(%x)\n",(long long)wParam,(long long)lParam,(long long)g_HwndNotify,g_RegMsg);
            ::OutputDebugStringA(lszMessage);
            //MessageBoxA(NULL,lszMessage,"CBTProc",MB_OK);
            if (::IsWindow(g_HwndNotify))
            {
                ::PostMessageA(g_HwndNotify ,g_RegMsg,nCode,wParam);
                //DWORD dwInfo = BSM_APPLICATIONS;
                //::BroadcastSystemMessageA(BSF_IGNORECURRENTTASK,&dwInfo,WM_USER + 1002,wParam,lParam);
            }
        }
        break;
    default:
        break;
    }
    return CallNextHookEx(NULL,nCode,wParam,lParam);
}

MSGHOOKSHARED_EXPORT int InstallCbtHook(HWND hwnd)
{
    g_HwndNotify = hwnd;
    g_CbtHook = SetWindowsHookEx(WH_CBT,CBTProc,g_hModule,0);
    g_RegMsg = RegisterWindowMessage(L"msghook_reg_msg");
    //if (g_RegMsg == 0)
    {
        DWORD dwError = GetLastError();
        char lszErr[1024] = { 0 };
        sprintf(lszErr,"msgHook RegisterWindowMessage msghook_reg_msg fail,err=%u,g_RegMsg=%x",dwError,g_RegMsg);
        ::OutputDebugStringA(lszErr);
    }
    if (g_CbtHook == NULL)
    {
        char lszErr[1024] = { 0 };
        sprintf(lszErr,"SetWindowsHookEx WH_CBT SetWindowsHookEx fail err=%u",GetLastError());
        MessageBoxA(NULL,lszErr,"error",MB_OK);
    }
    g_CWPHook = SetWindowsHookEx(WH_CALLWNDPROC,CallWndProc,g_hModule,0);
    return g_RegMsg;
}

MSGHOOKSHARED_EXPORT int UnInstallCbtHook()
{
    g_HwndNotify = NULL;
    if (g_CbtHook)
    {
        ::UnhookWindowsHookEx(g_CbtHook);
        g_CbtHook = NULL;
    }
    if (g_CWPHook)
    {
        ::UnhookWindowsHookEx(g_CWPHook);
        g_CWPHook = NULL;
    }
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    //g_hModule = hModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            g_hModule = hModule;
            break;
        }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        {
        }
        break;
    }
    return TRUE;
}
