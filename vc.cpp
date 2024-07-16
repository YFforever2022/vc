#include "vc.h"
#include <random>
#include <windows.h>

/*
 * File Name: vc.cpp
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





rfbClient* client = nullptr;


std::mutex globalImgLock;
std::vector<unsigned char> globalBuffer;
int screenWidth = 0;
int screenHeight = 0;

std::mutex globalMouseLock;
std::vector<unsigned char> globalMouse;
std::string globalMouseCode = "";

std::mutex globalRunningLock;
BOOL globalRunning = FALSE;

std::atomic<int> global_M_minInterval = 30, global_M_maxInterval = 60;
std::atomic<int> global_K_minInterval = 30, global_K_maxInterval = 60;



int generateRandomNumber(int minInterval, int maxInterval) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<int> distr(minInterval, maxInterval);
    return distr(rng);
}


void replaceTransparentWithColor(cv::Mat& image) {
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            cv::Vec4b& pixel = image.at<cv::Vec4b>(y, x);
            if (pixel[3] == 0) {
                pixel[0] = 255; // B
                pixel[1] = 0;   // G
                pixel[2] = 255; // R
                pixel[3] = 255; // A
            }
        }
    }
}


void myGotCursorShape_callback(_rfbClient* client, int xhot, int yhot, int width, int height, int bytesPerPixel)
{
    size_t size = width * height * 4;

    unsigned long hash = 0;
    for (int i = 0; i < size; ++i) {
        hash = (hash * 31) ^ client->rcSource[i];
    }
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << hash;
    {
        std::lock_guard<std::mutex> lock(globalMouseLock);
        globalMouseCode = oss.str();
    }



    size = width * height * 4;
    cv::Mat image(height, width, CV_8UC4, client->rcSource);
    if (!image.empty()) {
        replaceTransparentWithColor(image);
        cv::cvtColor(image, image, cv::COLOR_RGBA2RGB);

        int width = image.cols;
        int height = image.rows;
        int row_stride = (width * 3 + 3) & ~3;

        BITMAPFILEHEADER fileHeader = { 0 };
        BITMAPINFOHEADER infoHeader = { 0 };

        fileHeader.bfType = 0x4D42; // 'BM'
        fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        fileHeader.bfSize = fileHeader.bfOffBits + row_stride * height;
        fileHeader.bfReserved1 = 0;
        fileHeader.bfReserved2 = 0;

        infoHeader.biSize = sizeof(BITMAPINFOHEADER);
        infoHeader.biWidth = width;
        infoHeader.biHeight = height; // 从上到下的行顺序
        infoHeader.biPlanes = 1;
        infoHeader.biBitCount = 24;
        infoHeader.biCompression = 0;
        infoHeader.biSizeImage = row_stride * height;

        std::vector<unsigned char> temp;
        temp.resize(fileHeader.bfSize);
        memcpy(temp.data(), &fileHeader, sizeof(BITMAPFILEHEADER));
        memcpy(temp.data() + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(BITMAPINFOHEADER));

        for (int y = 0; y < height; y++) {
            const uint8_t* srcRow = image.ptr<uint8_t>(y);
            uint8_t* dstRow = temp.data() + 54 + (height - y - 1) * row_stride;
            memcpy(dstRow, srcRow, width * 3);
        }

        {
            std::lock_guard<std::mutex> lock(globalMouseLock);
            globalMouse = temp;
        }


    }



}



void myFinishedFrameBufferUpdate_callback(_rfbClient* client)
{
    std::lock_guard<std::mutex> lock(globalImgLock);
    if (screenWidth != client->width || screenHeight != client->height || globalBuffer.size() == 0) {
        // 重建缓冲区大小的
        screenWidth = client->width;
        screenHeight = client->height;
        globalBuffer.resize(54 + screenWidth * screenHeight * 3);
    }

    size_t row_stride = (screenWidth * 3 + 3) & ~3;
    size_t small_pic_row_pixels = client->width * 4;

    BITMAPFILEHEADER fileHeader = { 0 };
    BITMAPINFOHEADER infoHeader = { 0 };

    fileHeader.bfType = 0x4D42; // 'BM'
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    fileHeader.bfSize = fileHeader.bfOffBits + row_stride * screenHeight;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;

    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = screenWidth;
    infoHeader.biHeight = screenHeight; // 从上到下的行顺序
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = row_stride * screenHeight;

    memcpy(globalBuffer.data(), &fileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(globalBuffer.data() + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(BITMAPINFOHEADER));

    for (size_t r = 0; r < client->height; r++)
    {
        for (size_t c = 0; c < client->width; c++)
        {
            size_t dstOffset = (client->height - r - 1) * row_stride + c * 3;
            size_t srcOffset = r * small_pic_row_pixels + c * 4;
            memcpy(globalBuffer.data() + 54 + dstOffset, client->frameBuffer + srcOffset, 3);
        }
    }
}

// 接收VNC服务器的消息并处理
void HandlingVncEvents()
{
    while (1)
    {
        {
            Sleep(1);
            std::lock_guard<std::mutex> lock(globalRunningLock);
            if (globalRunning == FALSE) {
                return;
            }

            if (WaitForMessage(client, 1000) > 0)
            {
                if (!HandleRFBServerMessage(client))
                {
                    {
                        std::lock_guard<std::mutex> lock(globalImgLock);
                        globalBuffer.clear();
                    }
                    {
                        std::lock_guard<std::mutex> lock(globalMouseLock);
                        globalMouseCode = "";
                        globalMouse.clear();
                    }

                    std::cout << "reconnect" << std::endl;
                    rfbInitClient(client, nullptr, nullptr);
                    Sleep(100);
                }

            }
        }
    }
}


// vnc_连接
int Vnc_Connect(std::string ip, int port)
{
    std::lock_guard<std::mutex> lock(globalRunningLock);
    if (globalRunning == TRUE)
    {
        return -10;
    }
    client = rfbGetClient(8, 3, 4);
    char* _ip = nullptr;
    _ip = (char*)malloc(ip.length() + 1);
    memcpy(_ip, ip.c_str(), ip.length());
    _ip[ip.length()] = '\0';

    client->serverHost = _ip;
    client->serverPort = port;
    client->connectTimeout = 3;
    client->GotCursorShape = myGotCursorShape_callback;
    client->FinishedFrameBufferUpdate = myFinishedFrameBufferUpdate_callback;


    client->appData.useRemoteCursor = TRUE;
    client->appData.compressLevel = 0;
    client->appData.qualityLevel = 100;
    client->appData.encodingsString = "raw";
    client->appData.palmVNC = FALSE;

    if (!rfbInitClient(client, nullptr, nullptr)) {
        std::cerr << "Error connecting to VNC server" << std::endl;
        return -11;
    }

    SetFormatAndEncodings(client);


    globalRunning = TRUE;
    std::thread t(HandlingVncEvents);
    t.detach();

    return 1;
}

// vnc_断开
int Vnc_Disconnect()
{
    std::lock_guard<std::mutex> lock(globalRunningLock);
    if (globalRunning == TRUE) {
        {
            std::lock_guard<std::mutex> lock(globalMouseLock);
            globalMouseCode = "";
            globalMouse.clear();
        }

        {
            std::lock_guard<std::mutex> lock(globalImgLock);
            globalBuffer.clear();
        }

        globalRunning = FALSE;
        rfbClientCleanup(client);
        return 1;
    }
    return -1;
}

// vnc_鼠标_取特征码
std::string Vnc_GetMouseCode()
{
    std::lock_guard<std::mutex> lock(globalMouseLock);
    return globalMouseCode;
}

// vnc_鼠标_取图片
std::vector<unsigned char> Vnc_GetMouseImg()
{
    std::lock_guard<std::mutex> lock(globalMouseLock);
    return globalMouse;
}

// vnc_截图
std::vector<unsigned char> Vnc_GetScreenData()
{
    std::lock_guard<std::mutex> lock(globalImgLock);
    return globalBuffer;
}

std::atomic<int> global_mouse_x = 0;
std::atomic<int> global_mouse_y = 0;
// vnc_鼠标_移动
int Mouse_Move(int x, int y)
{
    if (client == nullptr) {
        return -1;
    }
    global_mouse_x = x;
    global_mouse_y = y;
    return SendPointerEvent(client, x, y, 0);
}

// vnc_鼠标_取最后位置
int Mouse_GetLastPos(int* x, int* y)
{
    *x = global_mouse_x;
    *y = global_mouse_y;
    return 1;
}

// vnc_鼠标_相对移动
int Mouse_MoveR(int x, int y)
{
    if (client == nullptr) {
        return -1;
    }
    global_mouse_x += x;
    global_mouse_y += y;
    return SendPointerEvent(client, global_mouse_x, global_mouse_y, 0);
}

// vnc_鼠标_弹起所有按键
int Mouse_Up()
{
    if (client == nullptr) {
        return -1;
    }
    return SendPointerEvent(client, global_mouse_x, global_mouse_y, 0);
}

// vnc_鼠标_左键按下
int Mouse_LeftDown()
{
    if (client == nullptr) {
        return -1;
    }
    return SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbButton1Mask);
}

// vnc_鼠标_中键按下
int Mouse_MiddleDown()
{
    if (client == nullptr) {
        return -1;
    }
    return SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbButton2Mask);
}

// vnc_鼠标_右键按下
int Mouse_RightDown()
{
    if (client == nullptr) {
        return -1;
    }
    return SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbButton3Mask);
}

int Mouse_LeftClick()
{
    if (client == nullptr) {
        return -1;
    }
    SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbButton1Mask);
    Sleep(generateRandomNumber(global_M_minInterval.load(), global_M_maxInterval.load()));
    int ret = SendPointerEvent(client, global_mouse_x, global_mouse_y, 0);
    return ret;
}

int Mouse_MiddleClick()
{
    if (client == nullptr) {
        return -1;
    }
    SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbButton2Mask);
    Sleep(generateRandomNumber(global_M_minInterval.load(), global_M_maxInterval.load()));
    int ret = SendPointerEvent(client, global_mouse_x, global_mouse_y, 0);
    return ret;
}

int Mouse_RightClick()
{
    if (client == nullptr) {
        return -1;
    }
    SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbButton3Mask);
    Sleep(generateRandomNumber(global_M_minInterval.load(), global_M_maxInterval.load()));
    int ret = SendPointerEvent(client, global_mouse_x, global_mouse_y, 0);
    return ret;
}

// vnc_鼠标_滚轮上滑
int Mouse_WheelUp()
{
    if (client == nullptr) {
        return -1;
    }
    return SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbWheelUpMask);
}

// vnc_鼠标_滚轮下滑
int Mouse_WheelDown()
{
    if (client == nullptr) {
        return -1;
    }
    return SendPointerEvent(client, global_mouse_x, global_mouse_y, rfbWheelDownMask);
}

// vnc_键盘_按下
int Keyboard_Down(uint32_t key)
{
    if (client == nullptr) {
        return -1;
    }
    return SendKeyEvent(client, key, TRUE);
}

// vnc_键盘_弹起
int Keyboard_Up(uint32_t key)
{
    if (client == nullptr) {
        return -1;
    }
    return SendKeyEvent(client, key, FALSE);
}

int Keyboard_Click(uint32_t key)
{
    if (client == nullptr) {
        return -1;
    }
    SendKeyEvent(client, key, TRUE);
    Sleep(generateRandomNumber(global_K_minInterval.load(), global_K_maxInterval.load()));
    int ret = SendKeyEvent(client, key, FALSE);
    return ret;
}

int Mouse_SetDelay(int minInterval, int maxInterval)
{
    global_M_minInterval.store(minInterval);
    global_M_maxInterval.store(maxInterval);
    return 1;
}

int Keyboard_SetDelay(int minInterval, int maxInterval)
{
    global_K_minInterval.store(minInterval);
    global_K_maxInterval.store(maxInterval);
    return 1;
}

