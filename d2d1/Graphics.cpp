#include "stdafx.h"
#include "d2d1.h"
#include "Process.h"
#include "Math.h"
#include "Graphics.h"
#include "Player.h"


Graphics::Graphics() {
	factory = NULL;
	renderTarget = NULL;
	brush = NULL;
	m_pTextFormat = NULL;
	m_pDWriteFactory = NULL;
}

Graphics::~Graphics() {
	if (factory) factory->Release();
	if (renderTarget) renderTarget->Release();
	if (brush) brush->Release();
	if (m_pTextFormat) m_pTextFormat->Release();
	if (m_pDWriteFactory) m_pDWriteFactory->Release();
}

bool Graphics::Init(HWND windowHandle) {
	HRESULT hr;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (hr != S_OK) return false;

	RECT rect;
	GetClientRect(windowHandle, &rect);

	factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
		D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom)),
		&renderTarget);

	if (hr != S_OK) return false;

	hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);
	if (hr != S_OK) return false;

	static const WCHAR msc_fontName[] = L"Century Gothic";
	static const FLOAT msc_fontSize = 15;
	
	// Turn anti alias mode on
	renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
			);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&m_pTextFormat
			);
	}
	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		//m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		//m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	}

	return true;
}

void Graphics::ClearScreen(float r, float g, float b, float a) {
	renderTarget->Clear(D2D1::ColorF(r, g, b, a));
}

void Graphics::DrawCircle(float x, float y, float radius, D2D1::ColorF colour) {
	brush->SetColor(colour);
	renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush, 3.0f);
}

void Graphics::DrawText(wchar_t* text, int x, int y, int width, int height, D2D1::ColorF colour) {
	brush->SetColor(colour);
	renderTarget->DrawText(text, wcslen(text), m_pTextFormat, D2D1::RectF(x, y, width, height), brush);
}

void Graphics::DrawShadowText(wchar_t* text, int x, int y, D2D1::ColorF colour)
{
	brush->SetColor(colour);
	DrawText(text, x, y - 1, 10, 10, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.f));
	DrawText(text, x, y + 1, 10, 10, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.f));
	DrawText(text, x - 1, y, 10, 10, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.f));
	DrawText(text, x + 1, y, 10, 10, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.f));
	DrawText(text, x, y, 10, 10, colour);
}

void Graphics::DrawLine(float StartX, float StartY, float EndX, float EndY, float size, D2D1::ColorF colour)
{
	brush->SetColor(colour);
	renderTarget->DrawLine(D2D1::Point2F(StartX, StartY), D2D1::Point2F(EndX, EndY), brush, size);
}

void Graphics::DrawFilledRect(float x, float y, float width, float height, D2D1::ColorF colour) {
	brush->SetColor(colour);
	D2D1_RECT_F rectangle = D2D1::RectF(x, y, x + width, y + height);
	renderTarget->FillRectangle(&rectangle, brush);
}

void Graphics::DrawRect(float x, float y, float width, float height, D2D1::ColorF colour) {
	brush->SetColor(colour);
	D2D1_RECT_F rectangle = D2D1::RectF(x, y, x + width, y + height);
	renderTarget->DrawRectangle(&rectangle, brush);
}

void Graphics::DrawGrid(int box_size = 25, float thickness = 0.5f, D2D1::ColorF colour = D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f)) {
	D2D1_SIZE_F rtSize = renderTarget->GetSize();

	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);

	for (int x = 0; x < width; x += box_size)
	{
		DrawLine(static_cast<FLOAT>(x), 0.0f, static_cast<FLOAT>(x), rtSize.height, thickness, colour);
	}

	for (int y = 0; y < height; y += box_size)
	{
		DrawLine(0.0f, static_cast<FLOAT>(y), rtSize.width, static_cast<FLOAT>(y), thickness, colour);
	}
}

bool Graphics::WorldToScreen(Vector3 In, Vector3& Out, float * ViewMatrix) {
	Out.x = In.x * ViewMatrix[0] + In.y * ViewMatrix[4] + In.z * ViewMatrix[8] + ViewMatrix[12];
	Out.y = In.x * ViewMatrix[1] + In.y * ViewMatrix[5] + In.z * ViewMatrix[9] + ViewMatrix[13];
	Out.z = In.x * ViewMatrix[2] + In.y * ViewMatrix[6] + In.z * ViewMatrix[10] + ViewMatrix[14];
	float w = In.x * ViewMatrix[3] + In.y * ViewMatrix[7] + In.z * ViewMatrix[11] + ViewMatrix[15];

	if (w < 0.0)
		return false;

	Out.x /= w;
	Out.y /= w;
	Out.z /= w;

	Out.x *= width / 2.0f;
	Out.x += width / 2.0f;

	Out.y *= -height / 2.0f;
	Out.y += height / 2.0f;

	return true;
}

void Graphics::CornerBox(int x, int y, int w, int h, int Size, D2D1::ColorF colour) {
	brush->SetColor(colour);
	int bWidth = w; int bHeight = h; 
	DrawLine(x, y, x, y + (bHeight / 5), Size, colour);
	DrawLine(x, y, x + (bWidth / 5), y, Size, colour);
	DrawLine(x + bWidth, y, x + bWidth - (bWidth / 5), y, Size, colour);
	DrawLine(x + bWidth, y, x + bWidth, y + (bHeight / 5), Size, colour);
	DrawLine(x, y + bHeight, x + (bWidth / 5), y + bHeight, Size, colour);
	DrawLine(x, y + bHeight, x, y + bHeight - (bHeight / 5), Size, colour);
	DrawLine(x + bWidth, y + bHeight, x + bWidth - (bWidth / 5), y + bHeight, Size, colour);
	DrawLine(x + bWidth, y + bHeight, x + bWidth, y + bHeight - (bHeight / 5), Size, colour);
}     
	
void Graphics::CornerBoxOutline(int x, int y, int w, int h, int Size, D2D1::ColorF colour) {
	CornerBox(x - 1, y - 1, w + 2, h + 2, Size, colour);
	CornerBox(x, y, w, h, Size, colour);
}

void Graphics::Box3D(int HeadX, int HeadY, int bottomX, int bottomY, float Distance_Player, float bounding_width, float size, D2D1::ColorF colour)
{
	float drawW = 245 / Distance_Player;
	float drawW2 = 555 / Distance_Player;
	float drawW3 = 55 / Distance_Player;

	DrawLine(bottomX - drawW, bottomY, bottomX + drawW, bottomY, size, colour);
	DrawLine(HeadX - drawW, HeadY, HeadX + drawW, HeadY, size, colour);
	DrawLine(HeadX - drawW, HeadY, bottomX - drawW, bottomY, size, colour);
	DrawLine(HeadX + drawW, HeadY, bottomX + drawW, bottomY, size, colour);
	DrawLine(HeadX - drawW2, HeadY - drawW3, bottomX - drawW2, bottomY - drawW3, size, colour);
	DrawLine(bottomX - drawW, bottomY, bottomX - drawW2, bottomY - drawW3, size, colour);
	DrawLine(HeadX - drawW, HeadY, HeadX - drawW2, HeadY - drawW3, size, colour);
	DrawLine((HeadX + drawW) + drawW2, HeadY - drawW3, (bottomX + drawW) + drawW2, bottomY - drawW3, size, colour);
	DrawLine(bottomX + drawW, bottomY, (bottomX + drawW) + drawW2, bottomY - drawW3, size, colour);
	DrawLine(HeadX + drawW, HeadY, (HeadX + drawW) + drawW2, HeadY - drawW3, size, colour);
	DrawLine(HeadX - drawW2, HeadY - drawW3, (HeadX + drawW) + drawW2, HeadY - drawW3, size, colour);
	DrawLine(bottomX - drawW2, bottomY - drawW3, (bottomX + drawW) + drawW2, bottomY - drawW3, size, colour);
}

void Graphics::DrawHorizontalBar(int x, int y, int health, float width)
{
	if (health <= 0) { return; }
	float l, r, g;

	g = health * 2.55;
	r = 255 - g;

	DrawFilledRect(x - (width / 2) -1, y, width + 2, 8, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
	//DrawFilledRect(x - (width / 2), y, width, 3, D2D1::ColorF(r / 255, g / 255, 0 / 255, 1.0f));
	DrawFilledRect(x - (width / 2), y + 2, (width / 100) * health, 4, D2D1::ColorF(r / 255, g / 255, 0 / 255, 1.0f));
}

void Graphics::DrawVerticalBar(Vector3 Head, Vector3 Foot, int health)
{
	if (health <= 0) { return; }
	float l, r, g;

	g = health * 2.55;
	r = 255 - g;

	Vector3 Box = Head - Foot;
	if (Box.y < 0)
		Box.y *= -1;

	int BoxWidth = (int)Box.y / 2;
	int DrawX = (int)Head.x - (BoxWidth / 2);
	int DrawY = (int)Head.y;

	DWORD dwDrawWidth = ((Foot.y - Head.y) / 100) * health;

	DrawFilledRect(DrawX - 6, DrawY - 1, 5, (int)Box.y + 2, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
	DrawFilledRect(DrawX - 5, DrawY, 3, dwDrawWidth, D2D1::ColorF(r / 255, g / 255, 0 / 255, 1.0f));
}

void Graphics::Render() {
	BeginDraw();

	ClearScreen(0.0f, 0.0f, 0.0f, 0.0f);
	
	MyPlayer.ReadInformation();

	DrawLine(width / 2 - 5, height / 2, width / 2 + 5, height / 2, 2.0f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f));
	DrawLine(width / 2, height / 2 - 5, width / 2, height / 2 + 5, 2.0f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f));

	int TotalPlayerCount = process->Read<int>(0x50F500);

	for (int i = 0; i < TotalPlayerCount; i++)
	{
		PlayerList[i].ReadInformation(i);

		// Skip loop if they are not even alive
		if (!PlayerList[i].isAlive()) {
			continue;
		}

		Vector3 ScreenHeadPosition;
		bool head_result = WorldToScreen(PlayerList[i].HeadPosition, ScreenHeadPosition, PlayerList[i].viewMatrix);
		Vector3 ScreenFootPosition;
		bool foot_result = WorldToScreen(PlayerList[i].FootPosition, ScreenFootPosition, PlayerList[i].viewMatrix);

		if (head_result && foot_result) {
			DrawLine(width / 2, height, ScreenFootPosition.x, ScreenFootPosition.y, 1.0f, D2D1::ColorF(1.f, 0.f, 0.f, 1.f));

			float DistanceToEnemy = MyPlayer.Position.DistTo(PlayerList[i].HeadPosition);
			
			//Get the height of our box by taking the W2S'd feet position minus the W2S'd head position
			float bounding_height = (ScreenFootPosition.y - ScreenHeadPosition.y);
			float bounding_width = bounding_height / 2;
			
			wchar_t playerNameBuffer[256];
			wsprintfW(playerNameBuffer, L"%S", PlayerList[i].Name);
			DrawShadowText(playerNameBuffer, ScreenHeadPosition.x + (bounding_width / 2), ScreenHeadPosition.y, D2D1::ColorF(1.f, 0.f, 0.f, 1.f));

			wchar_t playerDistanceBuffer[256];
			wsprintfW(playerDistanceBuffer, L"[%im]", int(DistanceToEnemy));
			DrawShadowText(playerDistanceBuffer, ScreenHeadPosition.x + (bounding_width / 2), ScreenFootPosition.y, D2D1::ColorF(1.f, 0.f, 0.f, 1.f));

			//DrawRect(ScreenHeadPosition.x - 55, ScreenHeadPosition.y - 55, 55, 55, D2D1::ColorF(1.f, 0.f, 0.f, 1.f));
			//DrawBar(ScreenHeadPosition.x, ScreenFootPosition.y, PlayerList[i].Health, bounding_width);

			DrawVerticalBar(ScreenHeadPosition, ScreenFootPosition, PlayerList[i].Health);

			//CornerBoxOutline(ScreenHeadPosition.x - (bounding_width / 2), ScreenHeadPosition.y, bounding_width, bounding_height, 2, D2D1::ColorF(1.f, 0.f, 0.f, 1.f));
			DrawRect(ScreenHeadPosition.x - (bounding_width / 2), ScreenHeadPosition.y, bounding_width, bounding_height, D2D1::ColorF(1.f, 0.f, 0.f, 1.f));

		}
	}
		
	EndDraw();
}
