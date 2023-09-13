#pragma once
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <thread>

#define ID_TIMER2 3

HHOOK hKeyboardHook = NULL;
HHOOK hMouseHook = NULL;
HWND hMainWnd;
DWORD BlackoutTime = 0;

void UnBlockInputs() {
    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}

void WakePC() {
    mouse_event(MOUSEEVENTF_MOVE, 0, 1, 0, NULL);
    Sleep(40);
    mouse_event(MOUSEEVENTF_MOVE, 0, -1, 0, NULL);
    MessageBeep(MB_OK);
    OutputDebugStringA("\nWakePC");
    KillTimer(hMainWnd, ID_TIMER2);
}

bool SetBlackoutTime(DWORD time) {
    BlackoutTime = time * 1000 * 60 ;//Millisec To Minutes
    return true;
}

void CALLBACK TimeProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    UnBlockInputs();
    WakePC();
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    OutputDebugStringA("KeyboardProc MESSAGE!!\n");
    SendMessage(hMainWnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
    return 1;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    PostMessage(hMainWnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
    return 1;
}

int ScreenBlocker() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    SetBlackoutTime(1);
    MessageBoxA(NULL, std::to_string(BlackoutTime/1000).c_str(),"Blackout Timming" , MB_OK);
    SetTimer(hMainWnd, ID_TIMER2, BlackoutTime, TimeProc);// In Minutes
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
    PostMessage(hMainWnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);

    if (hKeyboardHook == NULL || hMouseHook == NULL) {
        OutputDebugStringA("Error!!");
        return 1;
    }

    return 0;
}
