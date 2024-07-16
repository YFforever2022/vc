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




int Vnc_Connect(std::string ip, int port); // vnc_����
int Vnc_Disconnect(); // vnc_�Ͽ�
std::vector<unsigned char> Vnc_GetScreenData(); // vnc_��ͼ
std::vector<unsigned char> Vnc_GetMouseImg(); // vnc_���_ȡͼƬ    �������ݻ�������Ҫע����߳��ظ���д������
std::string Vnc_GetMouseCode(); // vnc_���_ȡ������
int Mouse_Move(int x, int y); // vnc_���_�ƶ�
int Mouse_MoveR(int x, int y); // vnc_���_����ƶ�
int Mouse_GetLastPos(int* x, int* y); // vnc_���_ȡ���λ��
int Mouse_Up(); // vnc_���_�������а���
int Mouse_LeftDown(); // vnc_���_�������
int Mouse_MiddleDown(); // vnc_���_�м�����
int Mouse_RightDown(); // vnc_���_�Ҽ�����
int Mouse_LeftClick(); // vnc_���_�������
int Mouse_MiddleClick(); // vnc_���_�м�����
int Mouse_RightClick(); // vnc_���_�Ҽ�����
int Mouse_WheelUp(); // vnc_���_�����ϻ�
int Mouse_WheelDown(); // vnc_���_�����»�
int Mouse_SetDelay(int minInterval, int maxInterval); // vnc_���_���������� ������ʱ�ڲ�Ĭ��Ϊ30-60ms֮�����
int Keyboard_Down(uint32_t key); // vnc_����_����
int Keyboard_Up(uint32_t key); // vnc_����_����
int Keyboard_Click(uint32_t key); // vnc_����_����
int Keyboard_SetDelay(int minInterval, int maxInterval); // vnc_����_���������� ������ʱ�ڲ�Ĭ��Ϊ30-60ms֮�����

