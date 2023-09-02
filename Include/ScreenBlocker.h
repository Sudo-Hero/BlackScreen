#pragma once
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <thread>

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
int ScreenBlocker();
void UnBlockInputs();
void WakePC();
