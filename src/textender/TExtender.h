//#pragma once
#define FONT_WIDTH_MULT (0.256f)
#define FONT_HEIGHT_MULT (0.296f)

#ifndef MAX_TEXT_SIZE
#define MAX_TEXT_SIZE 2048
#endif

#include "Font.h"
#include "D3DSprite.h"
#include "Pad.h"

#include <vector>
#include <d3dx9.h>

#include <string>

#define FONT_DEFAULT FONT_BANK

namespace TExtender {
	struct TEManager {

		static bool bIsLosted;
		//static bool bReset;

		static IDirect3DDevice9* _RwD3DDevice;
		static IDirect3DDevice9* GetD3DDevice();

	};

	enum eTranslation {
		TRANSLATION_NONE = 0,
		TRANSLATION_SANLTD = 1
	};

	class CNewFont {
		ID3DXFont* m_pD3DXFont;
	public:
		char m_fontName[128];
		char m_fileName[128];
		int m_width;
		unsigned int m_height;
		unsigned int m_weight;
		float m_scaleX;
		float m_scaleY;
		bool m_italic;
		unsigned int m_charSet;
		unsigned int m_outputPrecision;
		unsigned int m_quality;
		unsigned int m_pitchAndFamily;
		bool m_upperCase; // unused yet
		bool m_replaceUS; // unused yet
		bool m_initialised;
		bool SetupFont(char* fontName, char* fileName, unsigned int width, int height, float scaleX, float scaleY, unsigned int weight, bool italic, unsigned int charSet, unsigned int outputPrecision, unsigned int quality, unsigned int pitchAndFamily, bool upperCase, bool replaceUS);
		void PrintString(char* text, CRect const& rect, float scale_x, float scale_y, CRGBA const& color, DWORD format, bool hasBackground, CRGBA const* backgroundColor, float shadow, float outline, CRGBA const* dropColor, CRGBA const* shadowColor, CRGBA const* outlineColor);
		void PrintString(wchar* text, CRect const& rect, float scale_x, float scale_y, CRGBA const& color, DWORD format, bool hasBackground, CRGBA const* backgroundColor, float shadow, float outline, CRGBA const* dropColor, CRGBA const* shadowColor, CRGBA const* outlineColor);
		void PrintStringWithOutline(wchar* text, CRect const rect, CRGBA const color, CRGBA const dropColor, float outlineWidth);
		void DrawRect(char* text, CRect const& rect, float scale_x, float scale_y, DWORD format, CRGBA const backgroundColor);
		void DrawRect(wchar* text, CRect const& rect, float scale_x, float scale_y, DWORD format, CRGBA const backgroundColor);
		void ReleaseFont();
		void OnLost();
		void OnReset();
		float GetStringWidth(char* str);
		float GetStringWidth(wchar* str);
		static void GetTextRect(CRect* rect, float x, float y, CFontDetails _fd, wchar* text);
	};

	class CFontNew : public CFont {
	public:
		static uint8 MAX_NEW_FONTS;

		static int16 m_FontId;
		static CD3DSprite* m_pFontSprite;
		static std::vector<CNewFont> m_aFonts;
		static eTranslation m_Translation; // not implemented
		static CRGBA gLetterColors[MAX_TEXT_SIZE];
		static unsigned int gNumLetters;
		static bool gShadow;

		static void Initialise();
		static void Shutdown();
		static void Lost();
		static void Reset();
		static void SetFontStyle(int16 style);
		static void PrintString(float x, float y, char* text, bool isRTL = false);
		static void PrintRTLString(float x, float y, char* text);
		static void PrintString(float x, float y, wchar* text, bool isRTL = false);
		static void PrintRTLString(float x, float y, wchar* text);
		static void PrintStringFromBottom(float x, float y, wchar* str, bool isRTL = false);
		static void PrintRTLStringFromBottom(float x, float y, wchar* str);
		static void ProcessTags(char* dest, char* src);
		static void ProcessTags(wchar* dest, wchar* src);
		static float GetHeightScale(float h, int iFontId = -1);
		static float GetStringWidth(char* str, char bFull, char bScriptText);
	};

	class TextPrinter {
	protected:
		std::wstring _text;
		float _x;
		float _y;
		int16 m_FontId;
		CFontDetails _details;

	public:

		enum ScreenSide : uint8 {
			LeftTop,
			LeftBottom,
			RightTop,
			RightBottom
		};

		enum FontAlignment : uint8 {
			AlignLeft = ALIGN_LEFT,
			AlignCenter = ALIGN_CENTER,
			AlignRight = ALIGN_RIGHT
		};

	public:
		TextPrinter& Reset();
		TextPrinter& CopyFrom(TextPrinter const& tp);

		TextPrinter();
		TextPrinter(TextPrinter const& lhs);
		~TextPrinter();

		CFontDetails& GetDetails();

		// Legacy Setters
		TextPrinter& SetScale(float x, float y);
		TextPrinter& SetSlantRefPoint(float x, float y);
		TextPrinter& SetSlant(float s);
		TextPrinter& SetJustifyOn(void);
		TextPrinter& SetJustifyOff(void);
		TextPrinter& SetJustify(bool on);
		TextPrinter& SetRightJustifyOn(void);
		TextPrinter& SetRightJustifyOff(void);
		TextPrinter& SetCentreOn(void);
		TextPrinter& SetCentreOff(void);
		TextPrinter& SetWrapx(float x);
		TextPrinter& SetCentreSize(float s);
		TextPrinter& SetBackgroundOn(void);
		TextPrinter& SetBackgroundOff(void);
		TextPrinter& SetBackGroundOnlyTextOn(void);
		TextPrinter& SetBackGroundOnlyTextOff(void);
		TextPrinter& SetPropOn(void);
		TextPrinter& SetPropOff(void);
		// set text style
		TextPrinter& SetFontStyle(uint8 style);
		TextPrinter& SetRightJustifyWrap(float wrap);
		TextPrinter& SetHorizantalWrap(float wrap);
		TextPrinter& SetAlphaFade(float fade);
		TextPrinter& SetDropShadowPosition(int16 pos);
		TextPrinter& SetBackgroundColor(CRGBA col);
		TextPrinter& SetBackgroundBorder(CRect const& rect);
		TextPrinter& SetColor(CRGBA col);
		TextPrinter& SetDropColor(CRGBA col);
		// Added From Plugin-SDK
		TextPrinter& SetAlignment(int align);
		TextPrinter& SetProportional(bool on);
		// setups text background
		TextPrinter& SetBackground(bool enable); // , bool includeWrap
		TextPrinter& SetOrientation(short alignment); // eFontAlignment alignment

		TextPrinter& SetShadowColor(CRGBA col);
		TextPrinter& SetShadowWidth(float s);
		TextPrinter& SetShadow(float s, CRGBA col);

		TextPrinter& SetOutlineColor(CRGBA col);
		TextPrinter& SetOutlineWidth(float s);
		TextPrinter& SetOutline(float s, CRGBA col);
		TextPrinter& SetEdge(int8 s);
		TextPrinter& SetEdge(int8 s, CRGBA col);

		// New-Gen Setters
		TextPrinter& Scale(float x, float y);
		TextPrinter& Scale(CVector2D const& vec);
		TextPrinter& SlantRefPoint(float x, float y);
		TextPrinter& SlantRefPoint(CVector2D const& vec);
		TextPrinter& Slant(float s);
		TextPrinter& Justify(bool on);
		TextPrinter& JustifyWrap(bool on, float wrap);
		TextPrinter& WrapX(float x);
		TextPrinter& RightJustify(bool on);
		TextPrinter& RightJustify(bool on, float wrap);
		TextPrinter& Centred(bool on);
		TextPrinter& CentreSize(float s);
		TextPrinter& Centred(bool on, float s);
		TextPrinter& Background(bool on); // , bool includeWrap
		TextPrinter& BackGroundOnlyText(bool on);
		TextPrinter& Prop(bool on);
		//  text style
		TextPrinter& FontStyle(uint8 style);
		TextPrinter& RightJustifyWrap(float wrap);
		TextPrinter& HorizantalWrap(float wrap);
		TextPrinter& AlphaFade(float fade);
		TextPrinter& DropShadowPosition(int16 pos);
		TextPrinter& BackgroundColor(CRGBA col);
		TextPrinter& BackgroundBorder(CRect const& rect);
		TextPrinter& Color(CRGBA col);
		TextPrinter& DropColor(CRGBA col);
		// Added From Plugin-SDK
		TextPrinter& Alignment(int align);
		TextPrinter& Proportional(bool on);
		// ups text background
		TextPrinter& Orientation(short alignment); // eFontAlignment alignment

		TextPrinter& ShadowColor(CRGBA col);
		TextPrinter& ShadowWidth(float s);
		TextPrinter& Shadow(float s, CRGBA col);

		TextPrinter& OutlineColor(CRGBA col);
		TextPrinter& OutlineWidth(float s);
		TextPrinter& Outline(float s, CRGBA col);

		// New-Gen Getters
		CVector2D Scale();
		CVector2D SlantRefPoint();
		float Slant();
		bool Justify();
		bool RightJustify();
		bool Centred();
		float WrapX();
		float CentreSize();
		// ups text background
		bool Background();
		bool BackGroundOnlyText();
		bool Prop();
		//  text style
		uint8 FontStyle();
		float RightJustifyWrap();
		float AlphaFade();
		int16 DropShadowPosition();
		CRGBA const& BackgroundColor();
		CRect const& BackgroundBorder();
		CRGBA const& Color();
		CRGBA const& DropColor();
		// Added From Plugin-SDK
		int Alignment();
		bool Proportional();
		short Orientation(); // eFontAlignment alignment

		float ShadowWidth();
		CRGBA const& ShadowColor();
		float OutlineWidth();
		CRGBA const& OutlineColor();

		TextPrinter& Print(float x, float y, wchar* text, bool isRTL = false);
		TextPrinter& PrintFromBottom(float x, float y, wchar* text, bool isRTL = false);

		// Plugin-SDK's FontPrint
		static float ScreenInteger(float value);
		static bool Get3dTo2d(CVector const& posn, CVector& out);
		static float CalcScaling(float z);
		static void PrintOneAt3d(wchar* const& line, CVector const& posn2d, float offset_x, float offset_y, float w, float h,
			CRGBA const& color, unsigned char style, FontAlignment alignment, unsigned char dropPosition,
			CRGBA const& dropColor, bool shadow, float lineSize, bool proportional, bool justify);
		static void PrintUnscaled(wchar* const& line, float x, float y, unsigned char style = FONT_DEFAULT,
			float w = 1.0f, float h = 1.0f, CRGBA const& color = CRGBA(255, 255, 255, 255), FontAlignment alignment = AlignLeft,
			unsigned char dropPosition = 1, CRGBA const& dropColor = CRGBA(0, 0, 0, 255), bool shadow = false,
			float lineSize = 9999.0f, bool proportional = true, bool justify = false);
		static void SPrint(wchar* const& line, float x, float y, unsigned char style = FONT_DEFAULT, float w = 1.0f, float h = 1.0f,
			CRGBA const& color = CRGBA(255, 255, 255, 255), FontAlignment alignment = AlignLeft, unsigned char dropPosition = 1,
			CRGBA const& dropColor = CRGBA(0, 0, 0, 255), bool shadow = false, float lineSize = 9999.0f,
			bool proportional = true, bool justify = false, ScreenSide screenSide = LeftTop);
		static void SPrint(ScreenSide screenSide, FontAlignment alignment,
			wchar* const& line, float x, float y, unsigned char style = FONT_DEFAULT, float w = 1.0f, float h = 1.0f,
			CRGBA const& color = CRGBA(255, 255, 255, 255), unsigned char dropPosition = 1,
			CRGBA const& dropColor = CRGBA(0, 0, 0, 255), bool shadow = false, float lineSize = 9999.0f,
			bool proportional = true, bool justify = false);
		static void SPrint(std::vector<std::wstring> const& lines, float x, float y, float spacing = 1.0f, unsigned char style = FONT_DEFAULT,
			float w = 1.0f, float h = 1.0f,
			CRGBA const& color = CRGBA(255, 255, 255, 255), FontAlignment alignment = AlignLeft, unsigned char dropPosition = 1,
			CRGBA const& dropColor = CRGBA(0, 0, 0, 255), bool shadow = false, float lineSize = 9999.0f,
			bool proportional = true, bool justify = false, ScreenSide screenSide = LeftTop);
		static void SPrint(ScreenSide screenSide, FontAlignment alignment,
			std::vector<std::wstring> const& lines, float x, float y, float spacing = 1.0f, unsigned char style = FONT_DEFAULT,
			float w = 1.0f, float h = 1.0f,
			CRGBA const& color = CRGBA(255, 255, 255, 255), unsigned char dropPosition = 1,
			CRGBA const& dropColor = CRGBA(0, 0, 0, 255), bool shadow = false, float lineSize = 9999.0f,
			bool proportional = true, bool justify = false);
		bool PrintAt3d(CVector const& posn, wchar* const& line, float offset_x = 0.0f, float offset_y = 0.0f,
			unsigned char style = FONT_DEFAULT, float w = 1.0f, float h = 1.0f, CRGBA const& color = CRGBA(255, 255, 255, 255),
			bool scaleOnDistance = true, FontAlignment alignment = AlignLeft, unsigned char dropPosition = 1,
			CRGBA const& dropColor = CRGBA(0, 0, 0, 255), bool shadow = false, float lineSize = 9999.0f,
			bool proportional = true, bool justify = false);
		bool PrintAt3d(CVector const& posn, std::vector<std::wstring> const& lines, float spacing = 1.0f,
			float offset_x = 0.0f, float offset_y = 0.0f,
			unsigned char style = FONT_DEFAULT, float w = 1.0f, float h = 1.0f, CRGBA const& color = CRGBA(255, 255, 255, 255),
			bool scaleOnDistance = true, FontAlignment alignment = AlignLeft, unsigned char dropPosition = 1,
			CRGBA const& dropColor = CRGBA(0, 0, 0, 255), bool shadow = false, float lineSize = 9999.0f,
			bool proportional = true, bool justify = false);
	};
}