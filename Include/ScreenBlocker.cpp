#pragma once
#include "ScreenBlocker.h"
HHOOK hKeyboardHook = NULL;
HHOOK hMouseHook = NULL;

void UnBlockInputs() {
    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}

void WakePC() {
    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
    mouse_event(MOUSEEVENTF_MOVE, 0, 1, 0, NULL);
    Sleep(40);
    mouse_event(MOUSEEVENTF_MOVE, 0, -1, 0, NULL);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    OutputDebugStringA("KEYPRESS!!");
    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
    return 1;

}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
    OutputDebugStringA("MOUSEMOVED!!");
    return 1;
}

int ScreenBlocker() {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);

    if (hKeyboardHook == NULL || hMouseHook == NULL) {
        std::cerr << "Hook installation failed" << std::endl;
        return 1;
    }
    
    return 0;
}
