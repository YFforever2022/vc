#include "vc.h"
#include <random>
#include <windows.h>

/*
 * File Name: main.cpp
 * Description: The program was compiled using the original libvncclient library for learning purposes only.
 * Copyright: (C) 2024 YFforever2022
 * Author: YFforever2022
 * License: GNU General Public License v2.0 or later
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Disclaimer:
 * The program compiled using the original libvncclient library is for learning purposes only.
 * Any consequences caused by using this code to compile the program will be borne by the user.
 *
 * Creation Date: July 12, 2024
 * Modification History:
 * Date        Author          Modification
 * ------      ------          ------------
 * 2024-07-12  YFforever2022   1.0
 *
 * Dependencies:
 * This project uses the OpenCV and the LibVncClient library, which can be found at:
 * https://github.com/opencv/opencv/tree/4.6.0
 * https://github.com/LibVNC/libvncserver/tree/master
 */




BOOL InitShareMemOK = FALSE;
const int bufferSize = 1024 * 1024 * 20;
HANDLE hMapFile0001 = NULL;
LPVOID lpMapAddress0001 = nullptr;
HANDLE hMapFile0002 = NULL;
LPVOID lpMapAddress0002 = nullptr;
HANDLE hMapFile0003 = NULL;
LPVOID lpMapAddress0003 = nullptr;
DWORD global_parent_PID = 0;


#define MY_MSG_InitShareMem 4999
#define MY_MSG_Connet 5000
#define MY_MSG_Disconnect 5001
#define MY_MSG_GetScreenData 5002
#define MY_MSG_GetMouseImg 5003
#define MY_MSG_GetMouseCode 5004
#define MY_MSG_Mouse_Move 5005
#define MY_MSG_Mouse_MoveR 5006
#define MY_MSG_Mouse_GetLastPos 5007
#define MY_MSG_Mouse_Up 5008
#define MY_MSG_Mouse_LeftDown 5009
#define MY_MSG_Mouse_MiddleDown 5010
#define MY_MSG_Mouse_RightDown 5011
#define MY_MSG_Mouse_LeftClick 5012
#define MY_MSG_Mouse_MiddleClick 5013
#define MY_MSG_Mouse_RightClick 5014
#define MY_MSG_Mouse_WheelUp 5015
#define MY_MSG_Mouse_WheelDown 5016
#define MY_MSG_Mouse_SetDelay 5017
#define MY_MSG_Keyboard_Down 5018
#define MY_MSG_Keyboard_Up 5019
#define MY_MSG_Keyboard_Click 5020
#define MY_MSG_Keyboard_SetDelay 5021



#pragma pack(push, 1)
struct Init_Struct {
    wchar_t name[256];
    int name_size;
    DWORD parent_PID;
};
#pragma pack(pop)




#include <psapi.h>
bool IsProcessRunning(DWORD pid) {
    DWORD processes[1024];
    DWORD cbNeeded;
    if (EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        DWORD numProcesses = cbNeeded / sizeof(DWORD);
        for (DWORD i = 0; i < numProcesses; i++) {
            if (processes[i] == pid) {
                return true;
            }
        }
    }
    return false;
}

void KillSelf2() {
    while (true)
    {
        if (IsProcessRunning(global_parent_PID) == false) {
            ExitProcess(0);
        }
        Sleep(5000);
    }

}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
    {
        UnmapViewOfFile(lpMapAddress0001);
        CloseHandle(hMapFile0001);
        UnmapViewOfFile(lpMapAddress0002);
        CloseHandle(hMapFile0002);
        UnmapViewOfFile(lpMapAddress0003);
        CloseHandle(hMapFile0003);

        PostQuitMessage(0);
        return 0;
    }
    case WM_PAINT:
        return 0;
    case WM_KEYDOWN:
        return 0;
    case WM_COPYDATA:
    {
        if (wParam == MY_MSG_Connet) {
            int ret = 0;
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr && cds->cbData > 0) {
                std::vector<char> m_ip(cds->cbData + 1);
                memcpy(m_ip.data(), cds->lpData, cds->cbData);
                m_ip[cds->cbData] = '\0';
                ret = Vnc_Connect(m_ip.data(), cds->dwData);
                std::cout << "ip:" << m_ip.data() << "  port:" << cds->dwData << std::endl;
            }
            return ret;
        }
        else if (wParam == MY_MSG_Disconnect)
        {
            int ret = Vnc_Disconnect();
            return ret;
        }
        else if (wParam == MY_MSG_GetScreenData)
        {
            memset(static_cast<char*>(lpMapAddress0001), 0, bufferSize);
            std::vector<uchar> img = Vnc_GetScreenData(); // 截图
            if (img.size() > 54)
            {
                memcpy(static_cast<char*>(lpMapAddress0001), img.data(), img.size());
                return img.size();
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_GetMouseImg)
        {
            memset(static_cast<char*>(lpMapAddress0002), 0, bufferSize);
            std::vector<uchar> img = Vnc_GetMouseImg(); // 鼠标图像
            if (img.size() > 54)
            {
                memcpy(static_cast<char*>(lpMapAddress0002), img.data(), img.size());
                return img.size();
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_GetMouseCode)
        {
            memset(static_cast<char*>(lpMapAddress0003), 0, bufferSize);
            std::string code = Vnc_GetMouseCode(); // 鼠标特征码
            if (code.size() > 0)
            {
                memcpy(static_cast<char*>(lpMapAddress0003), code.data(), code.size());
                return code.size();
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_Mouse_Move)
        {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr && cds->cbData > 0) {
                int x = cds->dwData;
                int y;
                memcpy(&y, cds->lpData, cds->cbData);
                return Mouse_Move(x, y);
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_Mouse_MoveR)
        {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr && cds->cbData > 0) {
                int x = cds->dwData;
                int y;
                memcpy(&y, cds->lpData, cds->cbData);
                return Mouse_MoveR(x, y);
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_Mouse_GetLastPos)
        {
            int x;
            int y;
            Mouse_GetLastPos(&x, &y);
            int ret = y * 65536 + x;
            return ret;
        }
        else if (wParam == MY_MSG_Mouse_Up)
        {
            return Mouse_Up();
        }
        else if (wParam == MY_MSG_Mouse_LeftDown)
        {
            return Mouse_LeftDown();
        }
        else if (wParam == MY_MSG_Mouse_MiddleDown)
        {
            return Mouse_MiddleDown();
        }
        else if (wParam == MY_MSG_Mouse_RightDown)
        {
            return Mouse_RightDown();
        }
        else if (wParam == MY_MSG_Mouse_LeftClick)
        {
            return Mouse_LeftClick();
        }
        else if (wParam == MY_MSG_Mouse_MiddleClick)
        {
            return Mouse_MiddleClick();
        }
        else if (wParam == MY_MSG_Mouse_RightClick)
        {
            return Mouse_RightClick();
        }
        else if (wParam == MY_MSG_Mouse_WheelUp)
        {
            return Mouse_WheelUp();
        }
        else if (wParam == MY_MSG_Mouse_WheelDown)
        {
            return Mouse_WheelDown();
        }
        else if (wParam == MY_MSG_Mouse_SetDelay)
        {
            int minInterval = 30, maxInterval = 60;
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr) {
                minInterval = cds->dwData;
                maxInterval = cds->cbData;
                return Mouse_SetDelay(minInterval, maxInterval);
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_Keyboard_Down)
        {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr) {
                uint32_t key = (uint32_t)cds->dwData;
                return Keyboard_Down(key);
            }
            else {
                return -1;
            }

        }
        else if (wParam == MY_MSG_Keyboard_Up)
        {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr)
            {
                uint32_t key = (uint32_t)cds->dwData;
                return Keyboard_Up(key);
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_Keyboard_Click)
        {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr)
            {
                uint32_t key = (uint32_t)cds->dwData;
                return Keyboard_Click(key);
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_Keyboard_SetDelay)
        {
            int minInterval = 30, maxInterval = 60;
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds != nullptr) {
                minInterval = cds->dwData;
                maxInterval = cds->cbData;
                return Keyboard_SetDelay(minInterval, maxInterval);
            }
            else {
                return -1;
            }
        }
        else if (wParam == MY_MSG_InitShareMem)
        {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
            if (cds == nullptr || cds->lpData == nullptr) {
                return -7;
            }

            Init_Struct* data = (Init_Struct*)cds->lpData;
            global_parent_PID = data->parent_PID;
            std::thread t(KillSelf2);
            t.detach();


            wchar_t name[256];
            wcsncpy(name, data->name, data->name_size);
            name[data->name_size] = '\0';
            std::wstring name1 = std::wstring(name, data->name_size);

            hMapFile0001 = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name1.c_str());
            if (hMapFile0001 == NULL) {
                std::cerr << "Failed to create file mapping object: " << GetLastError() << std::endl;
                return -1;
            }
            lpMapAddress0001 = MapViewOfFile(hMapFile0001, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
            if (lpMapAddress0001 == NULL) {
                std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
                CloseHandle(hMapFile0001);
                return -2;
            }

            std::wstring name2 = name1 + L"-2";
            //MessageBoxW(NULL, name2.c_str(), name1.c_str(), MB_OK);
            
            hMapFile0002 = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name2.c_str());
            if (hMapFile0002 == NULL) {
                std::cerr << "Failed to create file mapping object: " << GetLastError() << std::endl;
                return -3;
            }
            lpMapAddress0002 = MapViewOfFile(hMapFile0002, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
            if (lpMapAddress0002 == NULL) {
                std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
                CloseHandle(hMapFile0002);
                return -4;
            }

            std::wstring name3 = name1 + L"-3";
            hMapFile0003 = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name3.c_str());
            if (hMapFile0003 == NULL) {
                std::cerr << "Failed to create file mapping object: " << GetLastError() << std::endl;
                return -5;
            }
            lpMapAddress0003 = MapViewOfFile(hMapFile0003, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
            if (lpMapAddress0003 == NULL) {
                std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
                CloseHandle(hMapFile0003);
                return -6;
            }

            InitShareMemOK = TRUE;
            return 1;
        }



    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


std::wstring StringToWString(const std::string& narrowStr) {
    int wideStrLen = MultiByteToWideChar(CP_UTF8, 0, narrowStr.c_str(), -1, nullptr, 0);
    if (wideStrLen == 0) {
        return L"";
    }

    std::wstring wideStr(wideStrLen, L'\0');
    if (MultiByteToWideChar(CP_UTF8, 0, narrowStr.c_str(), -1, &wideStr[0], wideStrLen) == 0) {
        return L"";
    }

    return wideStr;
}


std::wstring GenerateRandomName() {
    std::random_device rd;
    std::mt19937 gen(rd());

    const wchar_t* characters = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int minLength = 5;
    int maxLength = 20;
    int length = minLength + (gen() % (maxLength - minLength + 1));
    std::wstring Name;

    for (int i = 0; i < length; i++) {
        Name += characters[gen() % (wcslen(characters))];
    }

    return Name;
}


void KillSelf() {
    for (size_t i = 0; i < 10; i++)
    {
        Sleep(1000);
        if (InitShareMemOK == false) {
            if (i > 3) {
                ExitProcess(0);
            }
        }
        else {
            break;
        }
    }

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    std::wstring className = GenerateRandomName();

    WNDCLASS wndClass;//WNDCLASS是一个由系统支持的结构，CreateWindow只是将某个WNDCLASS定义的窗体变成实例
    ZeroMemory(&wndClass, sizeof(WNDCLASS));//其作用是用0来填充一块内存区域
    //------定义窗口属性
    wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;//创建的窗口的回调函数
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = GetModuleHandle(NULL);//获取句柄，NULL则返回调用进程本身的句柄
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = className.c_str();
    //------END

    RegisterClass(&wndClass);

    // 生成随机窗口名
    std::wstring windowName = GenerateRandomName();

    // 创建窗口
    HWND hWnd = CreateWindow(wndClass.lpszClassName, windowName.c_str(), WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_CLIPCHILDREN, 50, 50, 500, 200, 0, 0, wndClass.hInstance, 0);
    //类名，窗口标题，窗口样式，x,y,宽度，高度，hWndParent，hMenu，hlnstance，lpParam
    if (hWnd == NULL) {
        return 1;
    }


    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);
    //MessageBoxA(NULL, std::to_string((int)hWnd).c_str(), "w窗口句柄：", MB_OK);


    std::thread t(KillSelf);
    t.detach();


    //if (AllocConsole())
    //{
    //    freopen("CONIN$", "r", stdin);
    //    freopen("CONOUT$", "w", stdout);
    //    freopen("CONOUT$", "w", stderr);
    //    printf("调试控制台已打开。\n");
    //    printf("窗口句柄：%i\n", hWnd);
    //}



    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return (int)msg.wParam;
}

