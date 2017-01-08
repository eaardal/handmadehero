// BITMAPINFO: https://msdn.microsoft.com/en-us/library/windows/desktop/dd183375(v=vs.85).aspx
// BITMAPINFOHEADER: https://msdn.microsoft.com/en-us/library/windows/desktop/dd183376(v=vs.85).aspx
// CreateDIBSection: https://msdn.microsoft.com/en-us/library/windows/desktop/dd183494(v=vs.85).aspx
// DIB (Device Independent Bitmaps): https://msdn.microsoft.com/en-us/library/windows/desktop/dd183562(v=vs.85).aspx

#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;

// Minnet som vi kan skrive vår eget bitmap til
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;

internal void Win32ResizeDIBSection(int Width, int Height)
{
  // TODO: Free our DIBSection

  if (BitmapHandle)
  {
    DeleteObject(BitmapHandle);
  }

  if (!BitmapDeviceContext)
  {
    BitmapDeviceContext = CreateCompatibleDC(0);
  }

  // Sett bitmap info/egenskaper
  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = Width;
  BitmapInfo.bmiHeader.biHeight = Height;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;

  // Opprett et område i minnet for Device Independent Bitmap som vi kan styre selv
  BitmapHandle = CreateDIBSection(
    BitmapDeviceContext,
    &BitmapInfo,
    DIB_RGB_COLORS,
    &BitmapMemory,
    0, 0);
}

internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
  // Kopier vår bitmap buffer/rektangel til vinduets bitmap buffer/rektangel
  StretchDIBits(DeviceContext,
    X, Y, Width, Height,
    X, Y, Width, Height,
    BitmapMemory,
    &BitmapInfo,
    DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
  LRESULT Result = 0;

  switch (Message) {

    // Når vinduet får en resize event
    case WM_SIZE:
    {
      RECT ClientRect;
      GetClientRect(Window, &ClientRect);

      int Width = ClientRect.right - ClientRect.left;
      int Height = ClientRect.bottom - ClientRect.top;

      Win32ResizeDIBSection(Width, Height);

      OutputDebugStringA("WM_SIZE\n");
      break;
    }

    // Når vinduet får en destroy event
    case WM_DESTROY:
    {
      Running = false;
      OutputDebugStringA("WM_DESTROY\n");
      break;
    }

    // Når vinduet får en close event
    case WM_CLOSE:
    {
      Running = false;
      OutputDebugStringA("WM_CLOSE\n");
      break;
    }

    // Når vinduet blir aktivert
    case WM_ACTIVATEAPP:
    {
      OutputDebugStringA("WM_ACTIVATEAPP\n");
      break;
    }

    // Når vinduet blir oppdatert av Windows
    case WM_PAINT:
    {
      // Få en DeviceContext og et område å tegne bildet i
      PAINTSTRUCT Paint;
      HDC DeviceContext = BeginPaint(Window, &Paint);

      // Kalkuler hvor på skjermen vi kan tegne
      int X = Paint.rcPaint.left;
      int Y = Paint.rcPaint.top;
      int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
      int Width = Paint.rcPaint.right - Paint.rcPaint.left;

      // Oppdater vinduet med vår egen backbuffer, dvs med vår egen kalkulerte bitmap
      Win32UpdateWindow(DeviceContext, X, Y, Width, Height);

      // Avslutt oppdateringen av vinduet
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
  // Lag en definisjon av vinduet og sett egenskaper på det
  WNDCLASS WindowClass = {};

  WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";

  if (RegisterClassA(&WindowClass))
  {
    // Be Windows om å opprette vinduet og gi oss en handle til det
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
      // Sett vinduet vårt som aktivt
      Running = true;

      // Lytt på meldinger fra Windows's message loop så lenge vinduet er aktivt
      while(Running)
      {
        // Hent ny melding fra Windows
        MSG Message;
        BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);

        // Om det var en melding fra Windows, håndter meldingen videre (usikker!)
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
