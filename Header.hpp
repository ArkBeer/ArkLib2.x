#pragma once
#include<iostream>
#include<string>
#include<sstream>
#include<array>
#include<random>
#include<tuple>
#include<tchar.h>

#include<Windows.h>
#include"Tstring.hpp"
#include"WinMain.hpp"
#include"Key.hpp"
#include"Mouse.hpp"
#include"Random.hpp"
#include"Frame_Rate.hpp"
#include"Shape.hpp"
#include"2D_Collision.hpp"
#include"Draw_Class.hpp"
#include"D2D1_0.hpp"
#include"D2D1_1.hpp"
#include"D3D11.hpp"
#include"Xaudio2.hpp"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.Boot();

	return 0;
}
