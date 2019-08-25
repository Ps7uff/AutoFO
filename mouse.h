void Abutton (int &press_time) {
  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.time = 0;
  input.ki.dwExtraInfo = 0;
  input.ki.wVk = 0;
  input.ki.dwFlags =  KEYEVENTF_SCANCODE; // there is no KEYEVENTF_KEYDOWN
  input.ki.wScan = 0x1E;
  ::SendInput(1, &input, sizeof(INPUT));
  Sleep(press_time);
  input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
  ::SendInput(1, &input, sizeof(INPUT));
}
void Abutton_press () {
  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.time = 0;
  input.ki.dwExtraInfo = 0;
  input.ki.wVk = 0;
  input.ki.dwFlags =  KEYEVENTF_SCANCODE; // there is no KEYEVENTF_KEYDOWN
  input.ki.wScan = 0x1E;
  ::SendInput(1, &input, sizeof(INPUT));
}
void Abutton_release () {
  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.time = 0;
  input.ki.dwExtraInfo = 0;
  input.ki.wVk = 0;
  input.ki.dwFlags =  KEYEVENTF_SCANCODE; // there is no KEYEVENTF_KEYDOWN
  input.ki.wScan = 0x1E;
  input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
  ::SendInput(1, &input, sizeof(INPUT));
}

void LeftClick ( )
{ 
  INPUT    Input={0};
  // left down 
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
  ::SendInput(1,&Input,sizeof(INPUT));
  Sleep(100); 
  // left up
  ::ZeroMemory(&Input,sizeof(INPUT));
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
  ::SendInput(1,&Input,sizeof(INPUT));
}
void Ebutton () {
  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.time = 0;
  input.ki.dwExtraInfo = 0;
  input.ki.wVk = 0;
  input.ki.dwFlags =  KEYEVENTF_SCANCODE; // there is no KEYEVENTF_KEYDOWN
  input.ki.wScan = 0x12;
  ::SendInput(1, &input, sizeof(INPUT));
  Sleep(100);
  input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
  ::SendInput(1, &input, sizeof(INPUT));
}
void minusbutton () {
  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.time = 0;
  input.ki.dwExtraInfo = 0;
  input.ki.wVk = 0;
  input.ki.dwFlags =  KEYEVENTF_SCANCODE; // there is no KEYEVENTF_KEYDOWN
  input.ki.wScan = 0x0C;
  ::SendInput(1, &input, sizeof(INPUT));
  Sleep(100);
  input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
  ::SendInput(1, &input, sizeof(INPUT));
}
void zerobutton () {
  INPUT input;
  input.type = INPUT_KEYBOARD;
  input.ki.time = 0;
  input.ki.dwExtraInfo = 0;
  input.ki.wVk = 0;
  input.ki.dwFlags =  KEYEVENTF_SCANCODE; // there is no KEYEVENTF_KEYDOWN
  input.ki.wScan = 0x0B;
  ::SendInput(1, &input, sizeof(INPUT));
  Sleep(100);
  input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
  ::SendInput(1, &input, sizeof(INPUT));
}
void MouseMove (double x, double y )
{  
  double fScreenWidth    = ::GetSystemMetrics( SM_CXSCREEN )-1; 
  double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1; 
  double fx = x*(65535.0f/fScreenWidth);
  double fy = y*(65535.0f/fScreenHeight);
  INPUT  Input={0};
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_MOVE;
  Input.mi.dx = fx;
  Input.mi.dy = fy;
  ::SendInput(1,&Input,sizeof(INPUT));
}

