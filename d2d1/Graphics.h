#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <tlhelp32.h>
#include "Math.h"
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "Kernel32")

class Graphics {

	ID2D1Factory* factory;
	ID2D1HwndRenderTarget * renderTarget;
	ID2D1SolidColorBrush* brush;

	IDWriteTextFormat* m_pTextFormat;
	IDWriteFactory* m_pDWriteFactory;

public:
	Graphics();
	~Graphics();

	bool Init(HWND windowHandle);

	D2D1_SIZE_F renderTargetSize;
	int width;
	int height;

	void BeginDraw() { renderTarget->BeginDraw(); }
	void EndDraw() { renderTarget->EndDraw(); }
	void Render();
	void GetInfo();

	wchar_t* NumberToString(int number) {
		wchar_t character[256];
		swprintf_s(character, L"%d", number);
		wprintf(L"%s\n", character);

		return character;
	}

	bool WorldToScreen(Vector3 In, Vector3& Out, float * ViewMatrix);
	void ClearScreen(float r, float g, float b, float a);
	void DrawCircle(float x, float y, float radius, D2D1::ColorF colour);
	void DrawText(wchar_t* text, int x, int y, int width, int height, D2D1::ColorF colour);
	void DrawLine(float StartX, float StartY, float EndX, float EndY, float size, D2D1::ColorF colour);
	void DrawFilledRect(float x, float y, float width, float height, D2D1::ColorF colour);
	void DrawRect(float x, float y, float width, float height, D2D1::ColorF colour);
	void DrawGrid(int box_size, float thickness, D2D1::ColorF colour);
	void CornerBox(int x, int y, int w, int h, int Size, D2D1::ColorF colour);
	void CornerBoxOutline(int x, int y, int w, int h, int Size, D2D1::ColorF colour);
	void Box3D(int HeadX, int HeadY, int bottomX, int bottomY, float Distance_Player, float bounding_width, float size, D2D1::ColorF colour);
	void DrawHorizontalBar(int x, int y, int value, float width);
	void DrawVerticalBar(Vector3 Head, Vector3 Foot, int health);
	void DrawShadowText(wchar_t* text, int x, int y, D2D1::ColorF colour);
};