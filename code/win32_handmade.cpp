#include <windows.h>

static bool Running;

LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
  LRESULT Result = 0;

  switch (Message) {
    case WM_SIZE:
    {
      OutputDebugStringA("WM_SIZE\n");
      break;
    }
    case WM_DESTROY:
    {
      Running = false;
      OutputDebugStringA("WM_DESTROY\n");
      break;
    }
    case WM_CLOSE:
    {
      Running = false;
      OutputDebugStringA("WM_CLOSE\n");
      break;
    }
    case WM_ACTIVATEAPP:
    {
      OutputDebugStringA("WM_ACTIVATEAPP\n");
      break;
    }
    case WM_PAINT:
    {
      PAINTSTRUCT Paint;
      HDC DeviceContext = BeginPaint(Window, &Paint);

      int X = Paint.rcPaint.left;
      int Y = Paint.rcPaint.top;
      int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
      int Width = Paint.rcPaint.right - Paint.rcPaint.left;
      PatBlt(DeviceContext, X, Y, Width, Height, WHITENESS);

      EndPaint(Window, &Paint);

      break;
    }
    default:
    {
      OutputDebugStringA("WM_SIZE\n");
      Result = DefWindowProc(Window, Message, WParam, LParam);
      break;
    }
  }

  return(Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
  WNDCLASS WindowClass = {};

  WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc = MainWindowCallback;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";

  if (RegisterClassA(&WindowClass))
  {
    HWND WindowHandle = CreateWindowExA(
      0,
      WindowClass.lpszClassName,
      "Handmade Hero",
      WS_OVERLAPPEDWINDOW|WS_VISIBLE,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      0,
      0,
      Instance,
      0
    );

    if (WindowHandle)
    {
      while(Running)
      {
        MSG Message;
        BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
        if (MessageResult > 0)
        {
          TranslateMessage(&Message);
          DispatchMessageA(&Message);
        }
        else
        {
          break;
        }
      }
    }
  }

  return(0);
}
