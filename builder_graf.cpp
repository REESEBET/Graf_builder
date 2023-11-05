#include <windows.h>
#include <math.h>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib> // для rand() и srand() 
#include <ctime> // для time()


std::vector<double> coordinate_X;
std::vector<double> coordinate_Y;
std::vector<std::string> name_Plot;
int count_Plot;
bool flag = 0;


const wchar_t windowClass[] = L"win32app";
const wchar_t windowTitle[] = L"Win32API - topology of the OR2 element";




// Генерируем рандомное число между min и max 
int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);

    // Равномерно распределяем генерацию значения из диапазона 
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}


void Romb(HDC hdc, double x, double y)
{
    MoveToEx(hdc, x + 5, y, nullptr);
    LineTo(hdc, x, y + 5);
    LineTo(hdc, x + 5, y + 10);
    LineTo(hdc, x + 10, y + 5);
    LineTo(hdc, x + 5, y);
}

void Line(HDC hdc, int x1, int y1, int x2, int y2) //адекватная функция рисования линий
{
    MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
    LineTo(hdc, x2, y2);
}

void DrawGrid(HDC hdc, HWND hWnd)
{
    HPEN pen, old_pen;
    HBRUSH brush, old_brush;

    RECT rect;
    GetClientRect(hWnd, &rect);

    //-----------------------background--------------------------//
    //-----------------------------------------------------------//
    brush = CreateSolidBrush(RGB(128, 128, 128));
    old_brush = (HBRUSH)SelectObject(hdc, brush);

    pen = CreatePen(PS_USERSTYLE, 1, RGB(255, 255, 255));
    old_pen = (HPEN)SelectObject(hdc, pen);

    Rectangle(hdc, rect.left + 10, rect.top + 10, rect.right - 10, rect.bottom - 20);

    SelectObject(hdc, old_pen);
    DeleteObject(pen);

    SelectObject(hdc, old_brush);
    DeleteObject(brush);

    pen = CreatePen(PS_USERSTYLE, 1, RGB(255, 255, 255));
    old_pen = (HPEN)SelectObject(hdc, pen);

    for (int i = rect.left + 10; i < rect.right - 10; i += 20)
    {
        MoveToEx(hdc, i, rect.top + 10, nullptr);
        LineTo(hdc, i, rect.bottom - 20);
    }
    for (int j = rect.top + 10; j < rect.bottom - 20; j += 20)
    {
        MoveToEx(hdc, rect.left + 10, j, nullptr);
        LineTo(hdc, rect.right - 10, j);
    }

    SelectObject(hdc, old_pen);
    DeleteObject(pen);
}

void DrawPlot(HDC hdc, HWND hWnd)
{
    HPEN pen, old_pen;
    HBRUSH brush, old_brush;

    RECT rect;
    GetClientRect(hWnd, &rect);

    int width = ((rect.right - 10) - (rect.left + 10));
    int height = ((rect.bottom - 20) - (rect.top + 10));

    if (flag)
    {
        /*область рисовки*/
        double max_x_elem = *max_element(coordinate_X.begin(), coordinate_X.end());
        double min_x_elem = *min_element(coordinate_X.begin(), coordinate_X.end());

        double max_y_elem = *max_element(coordinate_Y.begin(), coordinate_Y.end());
        double min_y_elem = *min_element(coordinate_Y.begin(), coordinate_Y.end());


        double kx = width / (max_x_elem - min_x_elem);
        double ky = height / (max_y_elem - min_y_elem);

        double start_x = 10;
        double start_y = 20;
        if (min_y_elem < 0) { start_y = (20 + abs(min_y_elem) * ky); }


        brush = CreateSolidBrush(RGB(255, 255, 255));
        old_brush = (HBRUSH)SelectObject(hdc, brush);

        pen = CreatePen(PS_USERSTYLE, 2, RGB(255, 0, 0));
        old_pen = (HPEN)SelectObject(hdc, pen);

        bool flag_romb = true;

        for (int j = 0; j < count_Plot; j++) {

            pen = CreatePen(PS_USERSTYLE, 2, RGB(getRandomNumber(0, 255), getRandomNumber(0, 255), getRandomNumber(0, 255)));
            old_pen = (HPEN)SelectObject(hdc, pen);


            for (int i = j; i < coordinate_X.size() - count_Plot; i += count_Plot) {


                if (i + 2 * count_Plot < coordinate_X.size())
                {
                    Line(hdc, start_x + coordinate_X[i] * kx, rect.bottom - (start_y + coordinate_Y[i] * ky), start_x + coordinate_X[i + count_Plot] * kx, rect.bottom - (start_y + coordinate_Y[i + count_Plot] * ky));
                }


                if (flag_romb) {
                    Romb(hdc, -5 + start_x + coordinate_X[i] * kx, -5 + rect.bottom - (start_y + coordinate_Y[i] * ky));
                    flag_romb = 0;
                }


                if (i % 15 == 0)
                {
                    Romb(hdc, -5 + start_x + coordinate_X[i] * kx, -5 + rect.bottom - (start_y + coordinate_Y[i] * ky));
                }

                if (i + count_Plot >= coordinate_X.size() - count_Plot)
                {
                    Romb(hdc, -5 + start_x + coordinate_X[i] * kx, -5 + rect.bottom - (start_y + coordinate_Y[i] * ky));
                }

            }


            SelectObject(hdc, old_pen);
            DeleteObject(pen);

        }


        SelectObject(hdc, old_pen);
        DeleteObject(pen);
        SelectObject(hdc, old_brush);
        DeleteObject(brush);


        for (int i = 0; i < name_Plot.size(); i++)
        {
            TextOutA(hdc, rect.left + 40 + i * 40, rect.bottom - 15, name_Plot[i].c_str(), std::strlen(name_Plot[i].c_str()));

        }
    }

}


long __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {


    PAINTSTRUCT ps;
    HDC hdc;

    HPEN pen, old_pen;
    HBRUSH brush, old_brush;

    RECT rect;
    GetClientRect(hWnd, &rect);

    int width = ((rect.right - 10) - (rect.left + 10));
    int height = ((rect.bottom - 20) - (rect.top + 10));

    switch (message)
    {


    case WM_KEYDOWN:
    {
        if (wParam == VK_UP)
        {
            OPENFILENAME ofn;

            wchar_t szFileName[MAX_PATH] = L"";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = L"txt";

            if (GetOpenFileName(&ofn))
                MessageBox(hWnd, ofn.lpstrFile, L"имя файла", MB_OK);

            std::ifstream file;
            file.open(ofn.lpstrFile);
            if (file.is_open())
                MessageBox(hWnd, L"FILE OPEN!", L"Сообщение", MB_OK);

            coordinate_X.clear();
            coordinate_Y.clear();
            name_Plot.clear();

            file >> count_Plot; // считываем число графиков из первой строки файла

            std::string str = "";

            for (int i = 0; i < count_Plot; ++i)
            {
                file >> str;
                name_Plot.push_back(str);
            }

            while (!file.eof()) {

                double x, y;

                file >> x;
                for (int i = 0; i < count_Plot; i++) {
                    coordinate_X.push_back(x);

                }

                for (int i = 0; i < count_Plot; i++) {
                    file >> y;
                    coordinate_Y.push_back(y);
                }

            }
            flag = 1;
            file.close();
        }

        InvalidateRect(hWnd, nullptr, true);
    }
    break;

    case WM_DESTROY:
        //KillTimer(hWnd, 34);
       //KillTimer(hWnd, 35);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        DrawGrid(hdc, hWnd);
        DrawPlot(hdc, hWnd);

        EndPaint(hWnd, &ps);
    }
    break;



    case WM_LBUTTONDOWN:
    {

    }
    break;

    case WM_RBUTTONDOWN:
    {
    }
    break;

    case WM_CLOSE:
    {
    }
    break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1 + 2);     //color background
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = windowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, L"CanвЂ™t register window class!", L"Win32 API Test", NULL);
        return 1;
    }

    HWND hWnd = CreateWindow(windowClass, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, L"CanвЂ™t create window!", L"Win32 API Test", NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}