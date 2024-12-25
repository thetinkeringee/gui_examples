#include <windows.h>
#include <wingdi.h>
#include <winnt.h>

#ifndef UNICODE
static_assert(0,"UNICODE is not defined");
#endif

LRESULT CALLBACK wndProc(HWND,UINT,WPARAM,LPARAM);

int wWinMain(HINSTANCE hInstance, // Handle to the current instance of the program
             HINSTANCE, // hPrevInstance -- Allways NULL
             LPWSTR lpCmdLine, // Caommand line excluding the program name
             int nShowCmd      // Controls how the window is displayed
             )
{
  HBRUSH bkgroundBrush = CreateSolidBrush(RGB(0x40,0x40,0x50));

  WCHAR* className = L"WindowCLass";

  WNDCLASSEXW wc = {
      .cbSize = sizeof(WNDCLASSEXW),
      .style = 0,
      .lpfnWndProc = wndProc, // Poointer to Function that handles messages
                              // sent to thewindow
      .cbClsExtra = 0,  // Zero extra bytes  
      .cbWndExtra = 0,  // Zero extra bytes 
      .hInstance = hInstance, 
      .hIcon = LoadIcon(NULL, IDI_APPLICATION), // Handle to icon for the window 
      .hCursor = LoadCursor(NULL, IDC_ARROW), // Specify the mouse cursor 
      .hbrBackground = bkgroundBrush,  
      .lpszMenuName = NULL,  
      .lpszClassName = className, 
      .hIconSm = LoadIcon(NULL, IDI_APPLICATION) 
  };

  if (!RegisterClassExW(&wc)) {
    MessageBoxW(NULL, L"Error when registering window class!", L"Error", MB_OK | MB_ICONERROR);
    return 0;
  }

  WCHAR* windowName = L"Window 01";

  HWND win = CreateWindowExW(WS_EX_CLIENTEDGE, className, windowName,
                             WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL,
                             hInstance, NULL);

  if (nullptr == win) {
    MessageBoxW(NULL, L"Error when creating window!", L"Error", MB_OK | MB_ICONERROR);
    return 0;
  }

  ShowWindow(win, nShowCmd);

  MSG msg;
  while(GetMessageW(&msg, NULL, 0,0) > 0 ) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
  }

  return 0;
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) {
        PostMessageW(hwnd, WM_DESTROY, wParam, lParam);
    }

  default:
    break;
  }
  return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
