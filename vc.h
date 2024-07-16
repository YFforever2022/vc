#pragma once
#include <rfb/rfbclient.h>
#include <iostream>
#include <mutex>
#include <cstring>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <opencv2/opencv.hpp>

/*
 * File Name: vc.h
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




int Vnc_Connect(std::string ip, int port); // vnc_连接
int Vnc_Disconnect(); // vnc_断开
std::vector<unsigned char> Vnc_GetScreenData(); // vnc_截图
std::vector<unsigned char> Vnc_GetMouseImg(); // vnc_鼠标_取图片    返回数据缓冲区需要注意多线程重复读写的问题
std::string Vnc_GetMouseCode(); // vnc_鼠标_取特征码
int Mouse_Move(int x, int y); // vnc_鼠标_移动
int Mouse_MoveR(int x, int y); // vnc_鼠标_相对移动
int Mouse_GetLastPos(int* x, int* y); // vnc_鼠标_取最后位置
int Mouse_Up(); // vnc_鼠标_弹起所有按键
int Mouse_LeftDown(); // vnc_鼠标_左键按下
int Mouse_MiddleDown(); // vnc_鼠标_中键按下
int Mouse_RightDown(); // vnc_鼠标_右键按下
int Mouse_LeftClick(); // vnc_鼠标_左键单击
int Mouse_MiddleClick(); // vnc_鼠标_中键单击
int Mouse_RightClick(); // vnc_鼠标_右键单击
int Mouse_WheelUp(); // vnc_鼠标_滚轮上滑
int Mouse_WheelDown(); // vnc_鼠标_滚轮下滑
int Mouse_SetDelay(int minInterval, int maxInterval); // vnc_鼠标_设置随机间隔 不设置时内部默认为30-60ms之间随机
int Keyboard_Down(uint32_t key); // vnc_键盘_按下
int Keyboard_Up(uint32_t key); // vnc_键盘_弹起
int Keyboard_Click(uint32_t key); // vnc_键盘_单击
int Keyboard_SetDelay(int minInterval, int maxInterval); // vnc_键盘_设置随机间隔 不设置时内部默认为30-60ms之间随机

