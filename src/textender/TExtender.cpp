#include "common.h"

#include "TExtender.h"
#include "Sprite2d.h"
//#include "Messages.h"
#include <string>
#include <map>
#include "FileMgr.h"
#include "Utils.h"
#include "Sprite.h"
#include "Screen.h"
#include "postfx.h"
#include "Colors.h"

namespace TExtender {
uint8 CFontNew::MAX_NEW_FONTS;

bool TEManager::bIsLosted = false;
IDirect3DDevice9* TEManager::_RwD3DDevice = NULL;
IDirect3DDevice9* TEManager::GetD3DDevice() {
	if (!_RwD3DDevice)
		CPostFX::getDev(&_RwD3DDevice);
	return _RwD3DDevice;
}

// we should define some D3DSprite methods here :|
CD3DSprite::CD3DSprite() {
	D3DXCreateSprite(TEManager::GetD3DDevice(), &m_pSprite);
}

HRESULT CD3DSprite::Draw(LPDIRECT3DTEXTURE9 pTexture, CONST RECT* pSrcRect, CONST D3DXVECTOR3* pCenter, CONST D3DXVECTOR3* pPosition, D3DCOLOR Color) {
	HRESULT result;
	if (CFontNew::gShadow)
		result = m_pSprite->Draw(pTexture, pSrcRect, pCenter, pPosition, Color);
	else {
		result = m_pSprite->Draw(pTexture, pSrcRect, pCenter, pPosition, D3DCOLOR_RGBA(
			CFontNew::gLetterColors[CFontNew::gNumLetters].r,
			CFontNew::gLetterColors[CFontNew::gNumLetters].g,
			CFontNew::gLetterColors[CFontNew::gNumLetters].b,
			CFontNew::gLetterColors[CFontNew::gNumLetters].a
		));
		CFontNew::gNumLetters++;
	}
	return result;	
}

CD3DSprite *CFontNew::m_pFontSprite;
std::vector<CNewFont> CFontNew::m_aFonts;
eTranslation CFontNew::m_Translation;
int16 CFontNew::m_FontId;
CRGBA CFontNew::gLetterColors[MAX_TEXT_SIZE];
unsigned int CFontNew::gNumLetters;
bool CFontNew::gShadow;

bool CNewFont::SetupFont(char *fontName, char *fileName, unsigned int width, int height, float scaleX, float scaleY, unsigned int weight, bool italic,
	unsigned int charSet, unsigned int outputPrecision, unsigned int quality, unsigned int pitchAndFamily, bool upperCase,
	bool replaceUS)
{
	char f_path[1024];
	sprintf(f_path, "fonts\\%s", fileName);

	if (AddFontResourceExA(f_path, FR_PRIVATE, NULL)) {
		HRESULT h = D3DXCreateFont(TEManager::GetD3DDevice(), width, height, weight, 0, italic, charSet, outputPrecision, quality,
			pitchAndFamily, fontName, &m_pD3DXFont);

		if (h != S_OK) {
			m_pD3DXFont = NULL;
			//XLOGL("[CFontNew] Error initializing font");
			return FALSE;
		}
		//else
			//XLOGL("CFontNew: AddFont %s Done", fileName);
	}
	else {
		m_pD3DXFont = NULL;
		//XLOGL("CFontNew: Font %s Not Found in %s", fileName, f_path);
		return FALSE;
	}

	m_initialised = true;
	strcpy_s(m_fontName, fontName);
	strcpy_s(m_fileName, fileName);
	m_width = width;
	m_height = height;
	m_scaleX = scaleX;
	m_scaleY = scaleY;
	m_weight = weight;
	m_italic = italic;
	m_charSet = charSet;
	m_outputPrecision = outputPrecision;
	m_quality = quality;
	m_pitchAndFamily = pitchAndFamily;
	m_upperCase = upperCase;
	m_replaceUS = replaceUS;
	return true;
}

void CNewFont::PrintString(char* text, CRect const& rect, float scale_x, float scale_y, CRGBA const& color, DWORD format, bool hasBackground,
	CRGBA const* backgroundColor, float shadow, float outline, CRGBA const* dropColor, CRGBA const* shadowColor, CRGBA const* outlineColor)
{

	if (!m_pD3DXFont)
		return;

	if (!shadowColor)
		shadowColor = dropColor;

	if (!outlineColor)
		outlineColor = dropColor;

	RECT d3drect; SetRect(&d3drect, rect.left, rect.top, rect.right, rect.bottom);
	if (hasBackground)
		DrawRect(text, rect, scale_x, scale_y, format, *backgroundColor);
	RECT d3drect2; SetRect(&d3drect2, d3drect.left / (scale_x / 2), d3drect.top / (scale_y / 2), d3drect.right / (scale_x / 2), d3drect.bottom / (scale_y / 2));
	D3DXMATRIX S, P;
	D3DXMatrixScaling(&S, scale_x / 2, scale_y / 2, 1.0f);
	CFontNew::m_pFontSprite->GetTransform(&P);
	CFontNew::m_pFontSprite->SetTransform(&S);
	CFontNew::m_pFontSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	if (dropColor) {
		if (shadow > 0.0f) {
			RECT shadowRect(d3drect2);
			shadowRect.left += shadow;
			shadowRect.right += shadow;
			shadowRect.top += shadow;
			shadowRect.bottom += shadow;
			CFontNew::gShadow = true;
			m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, text, -1, &shadowRect, format, D3DCOLOR_ARGB(dropColor->alpha, dropColor->red, dropColor->green, dropColor->blue));
		}
		else if (outline > 0.0f) {
			RECT outl1(d3drect2);
			outl1.left -= outline;
			outl1.right -= outline;
			CFontNew::gShadow = true;
			m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, text, -1, &outl1, format, D3DCOLOR_ARGB(dropColor->alpha, dropColor->red, dropColor->green, dropColor->blue));
			RECT outl2(d3drect2);
			outl2.left += outline;
			outl2.right += outline;
			CFontNew::gShadow = true;
			m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, text, -1, &outl2, format, D3DCOLOR_ARGB(dropColor->alpha, dropColor->red, dropColor->green, dropColor->blue));
			RECT outl3(d3drect2);
			outl3.top -= outline;
			outl3.bottom -= outline;
			CFontNew::gShadow = true;
			m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, text, -1, &outl3, format, D3DCOLOR_ARGB(dropColor->alpha, dropColor->red, dropColor->green, dropColor->blue));
			RECT outl4(d3drect2);
			outl4.top += outline;
			outl4.bottom += outline;
			CFontNew::gShadow = true;
			m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, text, -1, &outl4, format, D3DCOLOR_ARGB(dropColor->alpha, dropColor->red, dropColor->green, dropColor->blue));
		}
	}
	CFontNew::gNumLetters = 0;
	CFontNew::gShadow = false;
	m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, text, -1, &d3drect2, format, D3DCOLOR_ARGB(color.alpha, color.red, color.green, color.blue));
	CFontNew::m_pFontSprite->SetTransform(&P);
	CFontNew::m_pFontSprite->End();
}

void CNewFont::PrintString(wchar* text, CRect const& rect, float scale_x, float scale_y, CRGBA const& color, DWORD format, bool hasBackground, CRGBA const* backgroundColor, float shadow, float outline, CRGBA const* dropColor, CRGBA const* shadowColor, CRGBA const* outlineColor)
{
	if (!m_pD3DXFont)
		return;

	if (!shadowColor)
		shadowColor = dropColor;

	if (!outlineColor)
		outlineColor = dropColor;

	RECT d3drect; SetRect(&d3drect, rect.left, rect.top, rect.right, rect.bottom);

	if (hasBackground)
		DrawRect(text, rect, scale_x, scale_y, format, *backgroundColor);
	RECT d3drect2; SetRect(&d3drect2, d3drect.left / (scale_x / 2), d3drect.top / (scale_y / 2), d3drect.right / (scale_x / 2), d3drect.bottom / (scale_y / 2));
	D3DXMATRIX S, P;
	D3DXMatrixScaling(&S, scale_x / 2, scale_y / 2, 1.0f);
	CFontNew::m_pFontSprite->GetTransform(&P);
	CFontNew::m_pFontSprite->SetTransform(&S);
	CFontNew::m_pFontSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	auto text_t = (wchar_t*)text;
	if (shadowColor && shadow > 0.0f) {
		RECT shadowRect(d3drect2);
		shadowRect.left += shadow;
		shadowRect.right += shadow;
		shadowRect.top += shadow;
		shadowRect.bottom += shadow;
		CFontNew::gShadow = true;
		m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &shadowRect, format, D3DCOLOR_ARGB(shadowColor->alpha, shadowColor->red, shadowColor->green, shadowColor->blue));
	}
	if (outlineColor && outline > 0.0f) {
		RECT outl1(d3drect2);
		outl1.left -= outline;
		outl1.right -= outline;
		CFontNew::gShadow = true;
		m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &outl1, format, D3DCOLOR_ARGB(outlineColor->alpha, outlineColor->red, outlineColor->green, outlineColor->blue));
		RECT outl2(d3drect2);
		outl2.left += outline;
		outl2.right += outline;
		CFontNew::gShadow = true;
		m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &outl2, format, D3DCOLOR_ARGB(outlineColor->alpha, outlineColor->red, outlineColor->green, outlineColor->blue));
		RECT outl3(d3drect2);
		outl3.top -= outline;
		outl3.bottom -= outline;
		CFontNew::gShadow = true;
		m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &outl3, format, D3DCOLOR_ARGB(outlineColor->alpha, outlineColor->red, outlineColor->green, outlineColor->blue));
		RECT outl4(d3drect2);
		outl4.top += outline;
		outl4.bottom += outline;
		CFontNew::gShadow = true;
		m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &outl4, format, D3DCOLOR_ARGB(outlineColor->alpha, outlineColor->red, outlineColor->green, outlineColor->blue));
	}
	CFontNew::gNumLetters = 0;
	CFontNew::gShadow = false;
	m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &d3drect2, format, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b)); // D3DCOLOR_ARGB(colorTmp.alpha, colorTmp.red, colorTmp.green, colorTmp.blue)
	CFontNew::m_pFontSprite->SetTransform(&P);
	CFontNew::m_pFontSprite->End();
}


void CNewFont::PrintStringWithOutline(wchar* text, CRect const rect, CRGBA const color, CRGBA const dropColor, float outlineWidth)
{

}

void CNewFont::DrawRect(char *text, CRect const& rect, float scale_x, float scale_y, DWORD format, CRGBA const backgroundColor) {
	float x2 = rect.right;
	RECT d3drect; SetRect(&d3drect, rect.left, rect.top, rect.right, rect.bottom);
	RECT d3drect2; SetRect(&d3drect2, d3drect.left / (scale_x / 2), d3drect.top / (scale_y / 2), d3drect.right / (scale_x / 2), d3drect.bottom / (scale_y / 2));
	D3DXMATRIX S, P;
	D3DXMatrixScaling(&S, scale_x / 2, scale_y / 2, 1.0f);
	CFontNew::m_pFontSprite->GetTransform(&P);
	CFontNew::m_pFontSprite->SetTransform(&S);
	CFontNew::m_pFontSprite->Begin(0);
	this->m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, text, -1, &d3drect2, format | DT_CALCRECT, 0xFFFFFFFF);
	CFontNew::m_pFontSprite->SetTransform(&P);
	CFontNew::m_pFontSprite->End();
	CSprite2d::DrawRect(CRect(d3drect2.left*(scale_x / 2) - 8, d3drect2.top*(scale_y / 2) - 8, x2 + 8, d3drect2.bottom*(scale_y / 2) + 8), backgroundColor);
}

void CNewFont::DrawRect(wchar *text, CRect const& rect, float scale_x, float scale_y, DWORD format, CRGBA const backgroundColor) {
	float x2 = rect.right;
	RECT d3drect; SetRect(&d3drect, rect.left, rect.top, rect.right, rect.bottom);
	RECT d3drect2; SetRect(&d3drect2, d3drect.left / (scale_x / 2), d3drect.top / (scale_y / 2), d3drect.right / (scale_x / 2), d3drect.bottom / (scale_y / 2));
	D3DXMATRIX S, P;
	D3DXMatrixScaling(&S, scale_x / 2, scale_y / 2, 1.0f);
	CFontNew::m_pFontSprite->GetTransform(&P);
	CFontNew::m_pFontSprite->SetTransform(&S);
	CFontNew::m_pFontSprite->Begin(0);
	auto text_t = (wchar_t*)text;
	this->m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &d3drect2, format | DT_CALCRECT, 0xFFFFFFFF);
	CFontNew::m_pFontSprite->SetTransform(&P);
	CFontNew::m_pFontSprite->End();
	CSprite2d::DrawRect(CRect(d3drect2.left*(scale_x / 2) - 8, d3drect2.top*(scale_y / 2) - 8, x2 + 8, d3drect2.bottom*(scale_y / 2) + 8), backgroundColor);
}

void CNewFont::ReleaseFont() {
	if (this->m_pD3DXFont) {
		this->m_pD3DXFont->Release();
		this->m_pD3DXFont = NULL;
		char path[1024];
		sprintf(path, "fonts\\%s", this->m_fileName);
		RemoveFontResourceExA(path, FR_PRIVATE, NULL);
	}
}

void CNewFont::OnLost() {
	ReleaseFont();
}

void CNewFont::OnReset() {
	//XLOGL("Hello Reset, thanks to CGame... :)");
	char f_path[1024];
	sprintf(f_path, "fonts\\%s", m_fileName);

	if (AddFontResourceExA(f_path, FR_PRIVATE, NULL)) //{
		D3DXCreateFont(TEManager::GetD3DDevice(), m_width, m_height, m_weight, 0, m_italic, m_charSet, m_outputPrecision, m_quality,
			m_pitchAndFamily, m_fontName, &this->m_pD3DXFont);

		/*
		HRESULT h = D3DXCreateFont
		if (h == S_OK)
			XLOGL("[CFontNew] Re-Initializing Font %s With Height:%d, Width:%d, Italic:%d, CharSet:%d, OutputPrecision:%d, Quality:%d, PitchAndFamily:%d", m_fontName, m_height, m_weight, m_italic, m_charSet, m_outputPrecision, m_quality,
				m_pitchAndFamily);
		else
			XLOGL("[CFontNew] Error reinitializing Font %s After Reset", m_fontName);
	}*/
}

void CFontNew::Initialise() {
	char ini_cat[8];
	char fontName[256];
	char fileName[256];
	char scaleFactorStr[16];
	char translation[16];
	//unsigned int fontId;
	int width;
	unsigned int height;
	float scaleX;
	float scaleY;
	unsigned int weight;
	bool italic;
	unsigned int charSet;
	unsigned int outputPrecision;
	unsigned int quality;
	unsigned int pitchAndFamily;
	bool upperCase;
	bool replaceUS;

	char iniName[2048] = { 0 };
	sprintf(iniName, "%s\\directFont.ini", CFileMgr::GetRootDirName());

	CFontNew::MAX_NEW_FONTS = min(GetPrivateProfileInt("GENERAL", "NumFonts", 6, iniName), 255);

	for (int i = 0; i < MAX_NEW_FONTS; i++) {
		CNewFont thisFont;
		thisFont.m_initialised = false;
		sprintf(ini_cat, "FONT%d", i + 1);
		//XLOGL("#%d $ini_cat: %s", i, ini_cat);

		GetPrivateProfileString(ini_cat, "FontName", "IRANYekanMobile", fontName, 256, iniName);
		GetPrivateProfileString(ini_cat, "FileName", "IRANYekanMobileRegular.ttf", fileName, 256, iniName);
		//XLOGL("#%d $fontName: %s", i, fontName);
		height = GetPrivateProfileInt(ini_cat, "Height", 64, iniName);
		width = GetPrivateProfileInt(ini_cat, "Width", 40, iniName);
		GetPrivateProfileString(ini_cat, "ScaleFactor.x", "0.6", scaleFactorStr, 16, iniName);
		sscanf(scaleFactorStr, "%f", &scaleX);
		GetPrivateProfileString(ini_cat, "ScaleFactor.y", "0.4", scaleFactorStr, 16, iniName);
		sscanf(scaleFactorStr, "%f", &scaleY);
		weight = GetPrivateProfileInt(ini_cat, "Weight", 500, iniName);
		italic = GetPrivateProfileInt(ini_cat, "Italic", 0, iniName);
		charSet = GetPrivateProfileInt(ini_cat, "CharSet", 1, iniName);
		outputPrecision = GetPrivateProfileInt(ini_cat, "OutputPrecision", 0, iniName);
		quality = GetPrivateProfileInt(ini_cat, "Quality", 0, iniName);
		pitchAndFamily = GetPrivateProfileInt(ini_cat, "PitchAndFamily", 0, iniName);
		upperCase = GetPrivateProfileInt(ini_cat, "UpcaseAlways", 0, iniName);
		replaceUS = GetPrivateProfileInt(ini_cat, "ReplaceUnderscoreWithSpace", 0, iniName);

		if (thisFont.SetupFont(fontName, fileName, width, height, scaleX, scaleY, weight, italic, charSet, outputPrecision, quality, pitchAndFamily,
			upperCase, replaceUS)) {
			//XLOGL("SetupFont %d fontName:%s, fileName:%s", i, fontName, fileName);
			m_aFonts.push_back(thisFont);
		}
	}

	GetPrivateProfileString("GENERAL", "UseTranslator", "NONE", translation, 16, iniName);

	if (!strncmp(translation, "NONE", 4))
		m_Translation = TRANSLATION_NONE;
	else if (!strncmp(translation, "SANLTD", 6))
		m_Translation = TRANSLATION_SANLTD;
	else {
		Error("CFontNew::Initialise\nUnknown translation name.");
		m_Translation = TRANSLATION_NONE;
	}

	m_pFontSprite = new CD3DSprite();
}

void CFontNew::Reset() {
	if (!TEManager::bIsLosted)
		return;

	for (int i = 0; i < MAX_NEW_FONTS; i++) {
		if (m_aFonts[i].m_initialised)
			m_aFonts[i].OnReset();
	}
	if (m_pFontSprite)
		m_pFontSprite->OnResetDevice();

	TEManager::bIsLosted = false;
}

void CFontNew::Lost() {
	if (TEManager::bIsLosted)
		return;

	for (int i = 0; i < MAX_NEW_FONTS; i++) {
		if (m_aFonts[i].m_initialised)
			m_aFonts[i].OnLost();
	}
	if (m_pFontSprite)
		m_pFontSprite->OnLostDevice();

	TEManager::bIsLosted = true;
}

void CFontNew::Shutdown() {
	for (int i = 0; i < MAX_NEW_FONTS; i++) {
		if (m_aFonts[i].m_initialised)
			m_aFonts[i].ReleaseFont();
	}
	delete m_pFontSprite;
}

void _UpCase(unsigned char *str) {
	unsigned char *pStr = str;
	while (*pStr) {
		if ((*pStr >= 97 && *pStr <= 122) || (*pStr >= 224 && *pStr <= 255))
			*pStr = *pStr - 32;
		else if (*pStr == 183)
			*pStr = 167;
		pStr++;
	}
}

void ReplaceUsWithSpace(unsigned char *str) {
	unsigned char *pStr = str;
	while (*pStr) {
		if (*pStr == '_')
			*pStr = ' ';
		pStr++;
	}
}

void CFontNew::PrintRTLString(float x, float y, char* text)
{
	PrintString(x, y, text, true);
}

void CFontNew::PrintString(float x, float y, char *text, bool isRTL) {
	// not used in revc
	PrintString(x, y, Utils::ConstCharToWideChar(text));
	return;
}


void CFontNew::PrintRTLString(float x, float y, wchar* text)
{
	PrintString(x, y, text, true);
}

void CFontNew::PrintString(float x, float y, wchar *text, bool isRTL) {
	if (!m_aFonts[m_FontId].m_initialised) {
		// fallback to default font renderer
		CFont::PrintString(x, y, text);
		return;
	}
	static wchar TaggedText[MAX_TEXT_SIZE] = { 0 };

	// FillMemory fills the variable with zeros, preventing GetStringWidth()
	// from detecting old text fragments and causing text alignment problems
	// when returning a larger value
	// (Visible in the subtitles where there are texts that appear higher even
	// having only one line ...) 

	auto text_t = (wchar_t*)TaggedText;
	wmemset(text_t, 0, MAX_TEXT_SIZE);
	//FillMemory(TaggedText, MAX_TEXT_SIZE, 0);

	CRect rect;
	unsigned int flag;

	if (Details.alignment == ALIGN_CENTER || Details.centre) {
		rect = CRect(x - Details.centreSize / 2.0f, y, x + Details.centreSize / 2.0f, SCREEN_HEIGHT);
		flag = DT_CENTER;
	}
	else if (Details.alignment == ALIGN_RIGHT || Details.rightJustify) {
		rect = CRect(Details.rightJustifyWrap, y, x, SCREEN_HEIGHT);
		flag = DT_RIGHT;
	}
	else {
		rect = CRect(x, y, CFont::Details.wrapX, SCREEN_HEIGHT);
		flag = DT_LEFT;
	}

	if (isRTL)
		flag |= DT_RTLREADING;


	ProcessTags(TaggedText, text);

	auto bgColor = CFont::Details.background ? CFont::Details.backgroundColor : CRGBA(0, 0, 0, 0);
	m_aFonts[m_FontId].PrintString(TaggedText,
		rect,
		CFont::Details.scaleX * m_aFonts[m_FontId].m_scaleX,
		CFont::Details.scaleY * m_aFonts[m_FontId].m_scaleY,
		CFont::Details.color,
		DT_TOP | DT_NOCLIP | DT_WORDBREAK | flag,

		CFont::Details.background,
		&bgColor,

		(float)(CFont::Details.shadow * 2),
		(float)(CFont::Details.outline * 2),
		&CFont::Details.dropColor,
		nil, nil
	);
}

void CFontNew::ProcessTags(char *dest, char *src, CFontDetails* _details) {
	// not used in revc
	bool bBreak = false;
	static char text[MAX_TEXT_SIZE];
	char *pDest = dest;
	char *pText = text;
	strcpy_s(text, src);
	//CMessages::InsertPlayerControlKeysInString(text);

	if (!_details)
		_details = &CFont::Details;

	CRGBA currColor(_details->color);
	unsigned int numLetters = 0;
	while (true) {
		// if string was finished
		if (!*pText)
			break;
		// if we found a tag
		if (*pText == '~') {
			// process tag
			pText++;
			// if string was finished
			if (!*pText)
				break;
			// if new line tag
			switch (*pText) {
			case 'N':
			case 'n':
				*pDest = '\n';
				pDest++;
				pText++;
				break;
			case 'R':
			case 'r':
				currColor = CRGBA(224, 50, 50, _details->color.alpha); // HUD_COLOUR_RED
				pText++;
				break;
			case 'G':
			case 'g':
				currColor = CRGBA(114, 204, 114, _details->color.alpha);
				pText++;
				break;
			case 'B':
			case 'b':
				currColor = CRGBA(93, 182, 229, _details->color.alpha);
				pText++;
				break;
			case 'W':
			case 'w':
				currColor = CRGBA(240, 240, 240, _details->color.alpha); // HUD_COLOUR_WHITE
				pText++;
				break;
			case 'Y':
			case 'y':
				currColor = CRGBA(240, 200, 80, _details->color.alpha); // HUD_COLOUR_YELLOW
				pText++;
				break;
			case 'P':
			case 'p':
				currColor = CRGBA(132, 102, 226, _details->color.alpha);
				pText++;
				break;
			case 'l':
				currColor = CRGBA(0, 0, 0, _details->color.alpha);
				pText++;
				break;
			case 'S':
			case 's':
				currColor = CRGBA(240, 240, 240, _details->color.alpha); // HUD_COLOUR_WHITE
				pText++;
				break;
			case 'H':
			case 'h':
				currColor = CRGBA(
					min((float)_details->color.red * 1.5f, 255.0f),
					min((float)_details->color.green * 1.5f, 255.0f),
					min((float)_details->color.blue * 1.5f, 255.0f),
					_details->color.alpha
				);
				pText++;
				break;
			}
			// close tag
			while (*pText != '~') {
				if (!pText) {
					*pDest = '\0';
					return;
				}
				pText++;
			}
			pText++;
		}
		else {
			*pDest = *pText;
			if (*pText != ' ') {
				gLetterColors[numLetters] = currColor;
				numLetters++;
			}
			pDest++;
			pText++;
		}
	}
	*pDest = '\0';
}

void CFontNew::ProcessTags(wchar* dest, wchar* src, CFontDetails* _details) {
	static wchar_t text[MAX_TEXT_SIZE] = { 0 };

	// FillMemory fills the variable with zeros, preventing GetStringWidth()
	// from detecting old text fragments and causing text alignment problems
	// when returning a larger value
	// (Visible in the subtitles where there are texts that appear higher even
	// having only one line ...) 

	wmemset(text, 0, MAX_TEXT_SIZE);
	//FillMemory(text, MAX_TEXT_SIZE, 0);

	auto src_t = (wchar_t*)src; // I Love This Part :D
	unsigned int s_len = min(wcslen(src_t), MAX_TEXT_SIZE);

	wcsncpy_s(text, src_t, s_len);

	//CMessages::InsertPlayerControlKeysInString(dest);

	if (!_details)
		_details = &CFont::Details;

	wchar* outText = dest;
	CRGBA currColor(_details->color);
	unsigned int numLetters = 0, txtidx = 0;

	for (int j = 0; j < MAX_TEXT_SIZE; j++)
		gLetterColors[j] = Colors::Transparent;

	for (size_t i = 0; i <= s_len; i++)
	{
		if (i == s_len)
		{
			*outText = L'\0';
			break;
		}
		else if (text[i] == L'~' && text[i + 2] == L'~')
		{
			i++;
			switch (text[i])
			{
			case L'N': // NewLine
			case L'n':
				*outText = L'\x0A';
				outText++;
				i++;
				break;

			// To-Do: Use Colors Class
			case L'R':
			case L'r':
				currColor = CRGBA(224, 50, 50, _details->color.alpha); // HUD_COLOUR_RED
				i++;
				break;
			case L'G':
			case L'g':
				currColor = CRGBA(114, 204, 114, _details->color.alpha);
				i++;
				break;
			case L'B':
			case L'b':
				currColor = CRGBA(93, 182, 229, _details->color.alpha);
				i++;
				break;
			case L'W':
			case L'w':
				currColor = CRGBA(240, 240, 240, _details->color.alpha); // HUD_COLOUR_WHITE
				i++;
				break;
			case L'H':
			case L'h':
				currColor = CRGBA(
					(unsigned char)(min((float)_details->color.r * 1.5f, 255.0f)),
					(unsigned char)(min((float)_details->color.g * 1.5f, 255.0f)),
					(unsigned char)(min((float)_details->color.b * 1.5f, 255.0f)),
					_details->color.alpha
				);
				i++;
				break;
			case L'Y':
			case L'y':
				currColor = CRGBA(240, 200, 80, _details->color.alpha); // HUD_COLOUR_YELLOW
				i++;
				break;
			case L'P':
			case L'p':
				currColor = CRGBA(132, 102, 226, _details->color.alpha);
				i++;
				break;
			case L'L':
			case L'l':
				currColor = CRGBA(0, 0, 0, _details->color.alpha);
				i++;
				break;
			case L'S':
			case L's':
				currColor = CRGBA(240, 240, 240, _details->color.alpha); // HUD_COLOUR_WHITE
				i++;
				break;
			default:
				i++;
				break;
			}
		}
		else if (text[i] == L'~' && text[i + 5] == L'~' && (text[i + 1] == L'a' || text[i + 1] == L'A'))
		{
			bool forceAlpha = text[i + 1] == L'A';
			i+=2;
			static wchar_t color_alpha[5];
			std::wmemcpy(color_alpha, &src_t[i], 3);
			color_alpha[3] = '\0';
			int iAlpha;
			if (sscanf(Utils::WideCharToConstChar((wchar*)color_alpha), "%03d", &iAlpha) == 1)
				currColor = CRGBA(currColor.r, currColor.g, currColor.b, forceAlpha ? iAlpha : (int)((iAlpha / 255.0f) * _details->color.alpha));
			i += 3;
		}
		else if (text[i] == L'~' && text[i + 7] == L'~')
		{
			i++;
			static wchar_t color_6[8];
			std::wmemcpy(color_6, &src_t[i], 6);
			color_6[6] = '\0';
			int r, g, b;
			if (sscanf(Utils::WideCharToConstChar((wchar*)color_6), "%02x%02x%02x", &r, &g, &b) == 3)
				currColor = CRGBA(r, g, b, _details->color.alpha);
			i+= 6;
		}
		else if (text[i] == L'~' && text[i + 9] == L'~')
		{
			i++;
			static wchar_t color_8[10];
			std::wmemcpy(color_8, &src_t[i], 8);
			color_8[8] = '\0';
			int r, g, b, a;
			if (sscanf(Utils::WideCharToConstChar((wchar*)color_8), "%02x%02x%02x%02x", &r, &g, &b, &a) == 4)
				currColor = CRGBA(r, g, b, (int)((a / 255.0f) * _details->color.alpha));
			i += 8;
		}
		else if (text[i] == L'~' && text[i + 10] == L'~')
		{
			i++;
			static wchar_t color_9[11];
			std::wmemcpy(color_9, &src_t[i], 9);
			color_9[9] = '\0';
			int r, g, b, a;
			if (sscanf(Utils::WideCharToConstChar((wchar*)color_9), "%02x%02x%02x0%02x", &r, &g, &b, &a) == 4)
				currColor = CRGBA(r, g, b, a);
			i += 9;
		}
		else {
			*outText = text[i];
			if (text[i] != L' ') {
				gLetterColors[numLetters] = currColor;
				numLetters++;
			}
			outText++;
		}
	}
}

float CFontNew::GetStringWidth(char *str, char bFull, char bScriptText) {
	if (m_aFonts[m_FontId].m_initialised) {
		static char text[MAX_TEXT_SIZE];
		static char taggedText[MAX_TEXT_SIZE];
		char* pText = taggedText;
		strncpy(text, str, strlen(str)); // CMessages::GetStringLength(str)
		CFontNew::ProcessTags(taggedText, text);
		if (!bFull)
		{
			while (*pText && *pText != ' ')
				pText++;
			*pText = '\0';
		}
		return CFontNew::m_aFonts[m_FontId].GetStringWidth(taggedText);
	}
	else
		return 0.0f; // CFont::GetStringWidth(str, bFull);
}

float CNewFont::GetStringWidth(char *str) {
	float scale_x = CFont::Details.scaleX * m_scaleX;
	float scale_y = CFont::Details.scaleY * m_scaleY;
	RECT d3drect; SetRect(&d3drect, 0, 0, 0, 0);
	D3DXMATRIX S, P;
	D3DXMatrixScaling(&S, scale_x / 2, scale_y / 2, 1.0f);
	CFontNew::m_pFontSprite->GetTransform(&P);
	CFontNew::m_pFontSprite->SetTransform(&S);
	CFontNew::m_pFontSprite->Begin(0);
	this->m_pD3DXFont->DrawText(CFontNew::m_pFontSprite, str, -1, &d3drect, DT_TOP | DT_NOCLIP | DT_SINGLELINE | DT_CALCRECT, 0xFFFFFFFF);
	CFontNew::m_pFontSprite->SetTransform(&P);
	CFontNew::m_pFontSprite->End();
	return (d3drect.right - d3drect.left) * (scale_x / 2);
}

float CNewFont::GetStringWidth(wchar *str) {
	float scale_x = CFont::Details.scaleX * m_scaleX;
	float scale_y = CFont::Details.scaleY * m_scaleY;
	RECT d3drect; SetRect(&d3drect, 0, 0, 0, 0);
	D3DXMATRIX S, P;
	D3DXMatrixScaling(&S, scale_x / 2.0f, scale_y / 2.0f, 1.0f);
	CFontNew::m_pFontSprite->GetTransform(&P);
	CFontNew::m_pFontSprite->SetTransform(&S);
	CFontNew::m_pFontSprite->Begin(0);
	auto text_t = (wchar_t*)str;
	this->m_pD3DXFont->DrawTextW(CFontNew::m_pFontSprite, text_t, -1, &d3drect, DT_TOP | DT_NOCLIP | DT_SINGLELINE | DT_CALCRECT, 0xFFFFFFFF);
	CFontNew::m_pFontSprite->SetTransform(&P);
	CFontNew::m_pFontSprite->End();

	// Values are enclosed between "(" and ")" to prevent the function from
	// returning the wrong value...
	return (((float)d3drect.right - (float)d3drect.left) * scale_x) / 2.0f;
}


void CFontNew::SetFontStyle(int16 style) {
	m_FontId = style;
}

float CFontNew::GetHeightScale(float h, int iFontId) {
	if (iFontId == -1)
		iFontId = m_FontId;
	return m_aFonts[iFontId].m_height * (h * FONT_HEIGHT_MULT);
}

void CFontNew::PrintRTLStringFromBottom(float x, float y, wchar* text)
{
	PrintStringFromBottom(x, y, text, true);
}

void CFontNew::PrintStringFromBottom(float x, float y, wchar *str, bool isRTL)
{
	y -= GetHeightScale(Details.scaleY) * (GetNumberLines(x, y, str) - 1);
	PrintString(x, y, str, isRTL);
}

void CNewFont::GetTextRect(CRect* rect, float x, float y, CFontDetails _fd, wchar* text) {
	if (_fd.centre) {
		rect->left = x - (_fd.centreSize / 2.0f + 4.0f);
		rect->right = _fd.centreSize / 2.0f + x + 4.0f;
	}
	else if (_fd.rightJustify) {
		rect->left = _fd.rightJustifyWrap - 4.0f;
		rect->right = x;
	}
	else {
		rect->left = x - 4.0f;
		rect->right = _fd.wrapX + 4.0f;
	}

	rect->top = y - 4.0f;
	rect->bottom = y + 4.0f + (CFontNew::GetHeightScale(_fd.scaleY, _fd.fId) * ((float)CFontNew::GetNumberLines(x, y, text)));
}

/// TextPrinter Class
std::unordered_map<std::string, CFontDetails> TextPrinter::structures;

TextPrinter::TextPrinter()
{
	Reset();
}

TextPrinter::TextPrinter(TextPrinter const& lhs)
{
	Reset();
	CopyFrom(lhs);
}

TextPrinter &
TextPrinter::CopyFrom(TextPrinter const& tp)
{
	SetScale(tp._details.scaleX, tp._details.scaleY);
	SetSlantRefPoint(tp._details.slantRefX, tp._details.slantRefY);
	SetSlant(tp._details.slant);
	SetColor(tp._details.color);
	SetJustify(tp._details.justify);
	Centred(tp._details.centre);
	SetWrapx(tp._details.wrapX);
	SetCentreSize(tp._details.centreSize);
	SetBackground(tp._details.background);
	SetBackgroundColor(tp._details.backgroundColor);
	BackGroundOnlyText(tp._details.backgroundOnlyText);
	SetProportional(tp._details.proportional);
	SetFontStyle(tp.m_FontId);
	SetRightJustifyWrap(tp._details.rightJustifyWrap);
	SetAlphaFade(tp._details.alphaFade);
	SetDropShadowPosition(tp._details.dropShadowPosition);
	SetAlignment(tp._details.alignment);
	return *this;
}

TextPrinter::~TextPrinter()
{
	//XLOGL("TextPrinter Died!");
}

CFontDetails &
TextPrinter::GetDetails()
{
	return _details;
}

TextPrinter& TextPrinter::SaveAs(std::string name)
{
	CFontDetails structure = CFontDetails(_details);
	structures[name] = structure;
	return *this;
}

bool TextPrinter::HasStructure(std::string name)
{
	return structures.find(name) != structures.end();
}

TextPrinter& TextPrinter::Use(std::string name)
{
	auto iter = structures.find(name);
	if (iter != structures.end()) {
		_details = CFontDetails(iter->second);
		m_FontId = _details.fId;
	}
	return *this;
}

void TextPrinter::ClearStructures()
{
	structures.clear();
}

TextPrinter&
TextPrinter::Reset()
{
	SetScale(1.0f, 1.0f);
	SetSlantRefPoint(SCREEN_WIDTH, 0.0f);
	SetSlant(0.0f);
	SetColor(CRGBA(255, 255, 255, 0));
	SetJustifyOff();
	SetCentreOff();
	SetWrapx(SCREEN_WIDTH);
	SetCentreSize(SCREEN_WIDTH);
	SetBackgroundOff();
	SetBackgroundColor(CRGBA(128, 128, 128, 128));
	SetBackGroundOnlyTextOff();
	SetPropOn();
	SetFontStyle(0);
	SetRightJustifyWrap(0.0f);
	SetAlphaFade(255.0f);
	SetDropShadowPosition(0);
	SetAlignment(ALIGN_LEFT);
	return *this;
}

// Legacy Setters
TextPrinter &
TextPrinter::SetProportional(bool on)
{
	_details.proportional = on;
	return *this;
}

TextPrinter &
TextPrinter::SetBackground(bool enable) // , bool includeWrap
{
	_details.background = enable;
	return *this;
}

TextPrinter&
TextPrinter::SetBackground(bool on, CRGBA col)
{
	return SetBackground(on).SetBackgroundColor(col);
}

TextPrinter &
TextPrinter::SetOrientation(short alignment) // eFontAlignment alignment
{
	_details.centre = alignment == ALIGN_CENTER;
	_details.rightJustify = alignment == ALIGN_RIGHT;
	return *this; // Fake IT Until Make It :D, // Update: We Made It ;)
}

TextPrinter &
TextPrinter::ClearOrientation()
{
	_details.centre = false;
	_details.rightJustify = false;
	return *this;
}

TextPrinter &
TextPrinter::SetScale(float x, float y)
{
#ifdef MORE_LANGUAGES
	/*if (IsJapanese()) {
		x *= 1.35f;
		y *= 1.25f;
	}*/
#endif
	_details.scaleX = x;
	_details.scaleY = y;
	return *this;
}

TextPrinter &
TextPrinter::SetSlantRefPoint(float x, float y)
{
	_details.slantRefX = x;
	_details.slantRefY = y;
	return *this;
}

TextPrinter &
TextPrinter::SetSlant(float s)
{
	_details.slant = s;
	return *this;
}

TextPrinter &
TextPrinter::SetColor(CRGBA col)
{
	_details.color = col;
	if (_details.alphaFade < 255.0f)
		_details.color.a *= _details.alphaFade / 255.0f;
	return *this;
}

TextPrinter &
TextPrinter::SetJustifyOn(void)
{
	_details.justify = true;
	_details.centre = false;
	_details.rightJustify = false;
	return *this;
}

TextPrinter &
TextPrinter::SetJustifyOff(void)
{
	_details.justify = false;
	_details.rightJustify = false;
	return *this;
}

TextPrinter&
TextPrinter::SetJustify(bool on)
{
	return on ? SetJustifyOn() : SetJustifyOff();
}

TextPrinter &
TextPrinter::SetCentreOn(void)
{
	_details.centre = true;
	_details.justify = false;
	_details.rightJustify = false;
	return *this;
}

TextPrinter &
TextPrinter::SetCentreOff(void)
{
	_details.centre = false;
	return *this;
}

TextPrinter &
TextPrinter::SetWrapx(float x)
{
	_details.wrapX = x;
	return *this;
}

TextPrinter &
TextPrinter::SetCentreSize(float s)
{
	_details.centreSize = s;
	return *this;
}

TextPrinter &
TextPrinter::SetBackgroundOn(void)
{
	_details.background = true;
	return *this;
}

TextPrinter &
TextPrinter::SetBackgroundOff(void)
{
	_details.background = false;
	return *this;
}

TextPrinter &
TextPrinter::SetBackgroundColor(CRGBA col)
{
	_details.backgroundColor = col;
	return *this;
}

TextPrinter &
TextPrinter::SetBackgroundBorder(CRect const& rect)
{
	_details.backgroundBorder = rect;
	return *this;
}

TextPrinter &
TextPrinter::SetBackGroundOnlyTextOn(void)
{
	_details.backgroundOnlyText = true;
	return *this;
}

TextPrinter &
TextPrinter::SetBackGroundOnlyTextOff(void)
{
	_details.backgroundOnlyText = false;
	return *this;
}

TextPrinter &
TextPrinter::SetAlignment(int align)
{
	_details.alignment = align;
	return *this;
}

TextPrinter &
TextPrinter::SetRightJustifyOn(void)
{
	_details.rightJustify = true;
	_details.justify = false;
	_details.centre = false;
	return *this;
}

TextPrinter &
TextPrinter::SetRightJustifyOff(void)
{
	_details.rightJustify = false;
	_details.justify = false;
	_details.centre = false;
	return *this;
}

TextPrinter &
TextPrinter::SetPropOff(void)
{
	_details.proportional = false;
	return *this;
}

TextPrinter &
TextPrinter::SetPropOn(void)
{
	_details.proportional = true;
	return *this;
}

TextPrinter &
TextPrinter::SetFontStyle(uint8 style)
{
	m_FontId = style;
	_details.fId = style;
	return *this;
	if (style == FONT_HEADING) {
		_details.style = FONT_STANDARD;
		_details.bFontHalfTexture = true;
	}
	else {
		_details.style = style;
		_details.bFontHalfTexture = false;
	}
	return *this;
}

TextPrinter &
TextPrinter::SetRightJustifyWrap(float wrap)
{
	_details.rightJustifyWrap = wrap;
	return *this;
}

TextPrinter&
TextPrinter::SetHorizantalWrap(float wrap)
{
	if (_details.rightJustify)
		_details.rightJustifyWrap = wrap;
	else if (_details.centre)
		_details.centreSize = wrap;
	else
		_details.wrapX = wrap;
	return *this;
}

TextPrinter &
TextPrinter::SetAlphaFade(float fade)
{
	_details.alphaFade = fade;
	return *this;
}

TextPrinter &
TextPrinter::SetDropColor(CRGBA col)
{
	_details.dropColor = col;
	if (_details.alphaFade < 255.0f)
		_details.dropColor.a *= _details.alphaFade / 255.0f;
	return *this;
}

TextPrinter &
TextPrinter::SetDropShadowPosition(int16 pos)
{
	_details.dropShadowPosition = pos;
	return *this;
}

TextPrinter&
TextPrinter::SetShadowWidth(float s)
{
	_details.shadow = s;
	return *this;
}

TextPrinter&
TextPrinter::SetShadowColor(CRGBA col)
{
	_details.shadowColor = col;
	if (_details.alphaFade < 255.0f)
		_details.shadowColor.a *= _details.alphaFade / 255.0f;
	return *this;
}

TextPrinter&
TextPrinter::SetShadow(float s, CRGBA col)
{
	return SetShadowWidth(s).SetShadowColor(col);
}

TextPrinter&
TextPrinter::SetOutlineWidth(float s)
{
	_details.outline = s;
	return *this;
}

TextPrinter&
TextPrinter::SetOutlineColor(CRGBA col)
{
	_details.outlineColor = col;
	if (_details.alphaFade < 255.0f)
		_details.outlineColor.a *= _details.alphaFade / 255.0f;
	return *this;
}

TextPrinter&
TextPrinter::SetOutline(float s, CRGBA col)
{
	return SetOutlineWidth(s).SetOutlineColor(col);
}

TextPrinter&
TextPrinter::SetEdge(int8 s)
{
	return SetOutlineWidth(s);
}

TextPrinter&
TextPrinter::SetEdge(int8 s, CRGBA col)
{
	return SetOutline(s, col);
}

// New-Gen Setters
TextPrinter &
TextPrinter::Proportional(bool on)
{
	return SetProportional(on);
}

TextPrinter &
TextPrinter::Background(bool on) // , bool includeWrap
{
	return on ? SetBackgroundOn() : SetBackgroundOff();
}

TextPrinter&
TextPrinter::Background(bool on, CRGBA col)
{
	return SetBackground(on, col);
}

TextPrinter &
TextPrinter::Orientation(short alignment) // eFontAlignment alignment
{
	return SetOrientation(alignment);
}

TextPrinter &
TextPrinter::Scale(float x, float y)
{
	return SetScale(x, y);
}

TextPrinter &
TextPrinter::Scale(CVector2D const& vec)
{
	return SetScale(vec.x, vec.y);
}

TextPrinter &
TextPrinter::SlantRefPoint(float x, float y)
{
	return SetSlantRefPoint(x, y);
}

TextPrinter &
TextPrinter::SlantRefPoint(CVector2D const& vec)
{
	return SetSlantRefPoint(vec.x, vec.y);
}

TextPrinter &
TextPrinter::Slant(float s)
{
	return SetSlant(s);
}

TextPrinter &
TextPrinter::ShadowWidth(float s)
{
	return SetShadowWidth(s);
}

TextPrinter &
TextPrinter::OutlineWidth(float s)
{
	return SetOutlineWidth(s);
}

TextPrinter&
TextPrinter::ShadowColor(CRGBA col)
{
	return SetShadowColor(col);
}

TextPrinter&
TextPrinter::OutlineColor(CRGBA col)
{
	return SetOutlineColor(col);
}

TextPrinter&
TextPrinter::Shadow(float s, CRGBA col)
{
	return SetShadow(s, col);
}

TextPrinter&
TextPrinter::Outline(float s, CRGBA col)
{
	return SetOutline(s, col);
}

TextPrinter &
TextPrinter::Color(CRGBA col)
{
	return SetColor(col);
}

TextPrinter&
TextPrinter::TextColor(CRGBA col)
{
	return SetColor(col);
}

TextPrinter &
TextPrinter::Justify(bool on)
{
	return on ? SetJustifyOn() : SetJustifyOff();
}

TextPrinter&
TextPrinter::WrapX(float x)
{
	return SetWrapx(x);
}

TextPrinter&
TextPrinter::JustifyWrap(bool on, float wrap)
{
	return Justify(on).SetWrapx(wrap);
}

TextPrinter &
TextPrinter::Centred(bool on)
{
	return on ? SetCentreOn() : SetCentreOff();
}

TextPrinter &
TextPrinter::CentreSize(float s)
{
	return SetCentreSize(s);
}

TextPrinter&
TextPrinter::Centred(bool on, float s)
{
	return Centred(on).SetCentreSize(s);
}

TextPrinter &
TextPrinter::BackgroundColor(CRGBA col)
{
	return SetBackgroundColor(col);
}

TextPrinter &
TextPrinter::BackgroundBorder(CRect const& rect)
{
	return SetBackgroundBorder(rect);
}

TextPrinter &
TextPrinter::BackGroundOnlyText(bool on)
{
	return on ? SetBackGroundOnlyTextOn() : SetBackGroundOnlyTextOff();
}

TextPrinter &
TextPrinter::Alignment(int align)
{
	return SetAlignment(align);
}

TextPrinter &
TextPrinter::RightJustify(bool on)
{
	return on ? SetRightJustifyOn() : SetRightJustifyOff();
}

TextPrinter&
TextPrinter::RightJustify(bool on, float wrap)
{
	return RightJustify(on).RightJustifyWrap(wrap);
}

TextPrinter &
TextPrinter::Prop(bool on)
{
	return on ? SetPropOn() : SetPropOff();
}

TextPrinter &
TextPrinter::FontStyle(uint8 style)
{
	return SetFontStyle(style);
}

TextPrinter &
TextPrinter::RightJustifyWrap(float wrap)
{
	return SetRightJustifyWrap(wrap);
}

TextPrinter&
TextPrinter::HorizantalWrap(float wrap)
{
	return SetHorizantalWrap(wrap);
}

TextPrinter &
TextPrinter::AlphaFade(float fade)
{
	return SetAlphaFade(fade);
}

TextPrinter&
TextPrinter::Alpha(float fade)
{
	return SetAlphaFade(fade);
}

TextPrinter &
TextPrinter::DropColor(CRGBA col)
{
	return SetDropColor(col);
}

TextPrinter &
TextPrinter::DropShadowPosition(int16 pos)
{
	return SetDropShadowPosition(pos);
}

// Getters
bool
TextPrinter::BackGroundOnlyText()
{
	return _details.backgroundOnlyText;
}

int
TextPrinter::Alignment()
{
	return _details.alignment;
}

bool
TextPrinter::RightJustify()
{
	return _details.rightJustify;
}

float
TextPrinter::RightJustifyWrap()
{
	return _details.rightJustifyWrap;
}

float
TextPrinter::AlphaFade()
{
	return _details.alphaFade;
}

float
TextPrinter::Alpha()
{
	return _details.alphaFade;
}

CRGBA const&
TextPrinter::DropColor()
{
	return _details.dropColor;
}

int16
TextPrinter::DropShadowPosition()
{
	return _details.dropShadowPosition;
}

bool
TextPrinter::Proportional()
{
	return _details.proportional;
}

bool
TextPrinter::Background() // , bool includeWrap
{
	return _details.background;
}

short
TextPrinter::Orientation() // eFontAlignment alignment
{
	return _details.centre ? ALIGN_CENTER : (_details.rightJustify ? ALIGN_RIGHT : ALIGN_LEFT); // Fake IT Until Make It :D
}

CVector2D
TextPrinter::Scale()
{
	return CVector2D(_details.scaleX, _details.scaleY);
}

CVector2D
TextPrinter::SlantRefPoint()
{
	return CVector2D(_details.slantRefX, _details.slantRefY);
}

float
TextPrinter::Slant()
{
	return _details.slant;
}

float
TextPrinter::ShadowWidth()
{
	return _details.shadow;
}

float
TextPrinter::OutlineWidth()
{
	return _details.outline;
}

CRGBA const&
TextPrinter::ShadowColor()
{
	return _details.shadowColor;
}

CRGBA const&
TextPrinter::OutlineColor()
{
	return _details.outlineColor;
}

CRGBA const&
TextPrinter::Color()
{
	return _details.color;
}

CRGBA const&
TextPrinter::TextColor()
{
	return _details.color;
}

bool
TextPrinter::Justify()
{
	return _details.justify;
}

bool
TextPrinter::Centred()
{
	return _details.centre;
}

float
TextPrinter::WrapX()
{
	return _details.wrapX;
}

float
TextPrinter::CentreSize()
{
	return _details.centreSize;
}

float
TextPrinter::HorizantalWrap()
{
	if (_details.rightJustify)
		return _details.rightJustifyWrap;
	else if (_details.centre)
		return _details.centreSize;
	else
		return _details.wrapX;
}

CRGBA const&
TextPrinter::BackgroundColor()
{
	return _details.backgroundColor;
}

CRect const&
TextPrinter::BackgroundBorder()
{
	return _details.backgroundBorder;
}

bool
TextPrinter::Prop()
{
	return _details.proportional;
}

uint8
TextPrinter::FontStyle()
{
	return m_FontId;
}

TextPrinter&
TextPrinter::PrintFromBottom(float x, float y, wchar* text, bool isRTL)
{
	y -= CFontNew::GetHeightScale(_details.scaleY) * (CFontNew::GetNumberLines(x, y, text) - 1);
	return Print(x, y, text, isRTL);
}

TextPrinter &
TextPrinter::Print(float x, float y, wchar* text, bool isRTL)
{

	_text = ((wchar_t*)text);
	_x = x;
	_y = y;

	if (!CFontNew::m_aFonts[m_FontId].m_initialised)
		return *this;

	static wchar TaggedText[MAX_TEXT_SIZE] = { 0 };

	// FillMemory fills the variable with zeros, preventing GetStringWidth()
	// from detecting old text fragments and causing text alignment problems
	// when returning a larger value
	// (Visible in the subtitles where there are texts that appear higher even
	// having only one line ...)

	auto text_t = (wchar_t*)TaggedText;
	wmemset(text_t, 0, MAX_TEXT_SIZE);

	CRect rect;
	unsigned int flag;

	if (_details.alignment == ALIGN_CENTER || _details.centre) {
		rect = CRect(x - _details.centreSize / 2.0f, y, x + _details.centreSize / 2.0f, SCREEN_HEIGHT);
		flag = DT_CENTER;
	}
	else if (_details.alignment == ALIGN_RIGHT || _details.rightJustify) {
		rect = CRect(_details.rightJustifyWrap, y, x, SCREEN_HEIGHT);
		flag = DT_RIGHT;
	}
	else {
		rect = CRect(x, y, _details.wrapX, SCREEN_HEIGHT);
		flag = DT_LEFT;
	}

	if (isRTL)
		flag |= DT_RTLREADING;

	//CFontNew::Translate((char*)text, Text);

	CFontNew::ProcessTags(TaggedText, text, &_details);
	//CFont::FilterOutTokensFromString(text);

	/*if (CFont::Details.background) {
		//x += CFont::Details.backgroundBorder.left;
		//y += CFont::Details.backgroundBorder.top;
		CFont::GetTextRect(&rect, x, y, text);
		CSprite2d::DrawRect(rect, CFont::Details.backgroundColor);
	}*/

	auto bgColor = _details.background ? _details.backgroundColor : CRGBA(0, 0, 0, 0);
	CFontNew::m_aFonts[m_FontId].PrintString(TaggedText,
		rect,
		_details.scaleX * CFontNew::m_aFonts[m_FontId].m_scaleX,
		_details.scaleY * CFontNew::m_aFonts[m_FontId].m_scaleY,
		_details.color,
		DT_TOP | DT_NOCLIP | DT_WORDBREAK | flag,

		_details.background,
		&bgColor,

		(_details.shadow * 2.0f),
		(_details.outline * 2.0f),
		&_details.dropColor,
		&_details.shadowColor,
		&_details.outlineColor
	);

	return *this;
}

// Plugin-SDK's FontPrint
#define SCALEW 0.8f
#define SCALEH 1.6f
#define SPACING 15.0f

float TextPrinter::ScreenInteger(float value) {
	return static_cast<float>(static_cast<int>(value));
}

bool TextPrinter::Get3dTo2d(CVector const& posn, CVector& out) {
	float outw, outh;
	return CSprite::CalcScreenCoors(posn, &out, &outw, &outh, false);
}

float TextPrinter::CalcScaling(float z) {
	float scale = 1.0f / z * 5.0f;
	if (scale > 1.0f)
		scale = 1.0f;
	else if (scale < 0.01f)
		scale = 0.0f;
	else if (scale < 0.25f)
		scale = 0.25f;
	return scale;
}

void TextPrinter::PrintOneAt3d(wchar* const& line, CVector const& posn2d, float offset_x, float offset_y, float w, float h,
	CRGBA const& color, unsigned char style, FontAlignment alignment, unsigned char dropPosition,
	CRGBA const& dropColor, bool shadow, float lineSize, bool proportional, bool justify)
{
	PrintUnscaled(line,
		posn2d.x + CScreen::GetCoord(offset_x),
		posn2d.y + CScreen::GetCoord(offset_y),
		style,
		CScreen::GetMultiplier(w),
		CScreen::GetMultiplier(h),
		color, alignment, dropPosition, dropColor, shadow,
		CScreen::GetCoord(lineSize),
		proportional, justify);
}

void TextPrinter::PrintUnscaled(wchar* const& line, float x, float y, unsigned char style, float w, float h,
	CRGBA const& color, FontAlignment alignment, unsigned char dropPosition, CRGBA const& dropColor, bool shadow, float lineSize,
	bool proportional, bool justify)
{
	if (!line)
		return;
	if(!wcslen((wchar_t*)line))
		return;

	TextPrinter tPrinter;
	tPrinter
		.SetFontStyle(style)
		.SetScale(w * SCALEW, h * SCALEH)
		.SetColor(color)
		.SetAlphaFade(255.0f)
		.SetSlant(0.0f)
		.SetDropColor(dropColor);
	switch (alignment) {
	case AlignCenter:
		tPrinter
			.Centred(true, ScreenInteger(lineSize));
		break;
	case AlignLeft:
		tPrinter
			.JustifyWrap(true, ScreenInteger(x + lineSize));
		break;
	case AlignRight:
		tPrinter
			.RightJustify(true, ScreenInteger(x - lineSize));
		break;
	}

	if (shadow)
		tPrinter
			.ShadowWidth(dropPosition);
	else
		tPrinter
			.OutlineWidth(dropPosition);

	tPrinter
		.Proportional(proportional)
		.Background(false)
		.BackGroundOnlyText(false);

	// To-Do: Detect isRTL Here
	/*auto firstChar = Utils::WideCharToConstChar(line)[0];
	bool notIsEnglish = !((firstChar >= 'a' && firstChar <= 'z') || (firstChar >= 'A' && firstChar <= 'Z'));*/
	tPrinter.Print(ScreenInteger(x), ScreenInteger(y), line);
}

void TextPrinter::SPrint(wchar* const& line, float x, float y, unsigned char style, float w, float h,
	CRGBA const& color, FontAlignment alignment, unsigned char dropPosition, CRGBA const& dropColor, bool shadow, float lineSize,
	bool proportional, bool justify, ScreenSide screenSide)
{
	switch (screenSide) {
	case LeftBottom:
		x = CScreen::GetCoord(x, CScreen::SIDE_LEFT);
		y = CScreen::GetCoord(y, CScreen::SIDE_BOTTOM);
		break;
	case RightTop:
		x = CScreen::GetCoord(x, CScreen::SIDE_RIGHT);
		y = CScreen::GetCoord(y, CScreen::SIDE_TOP);
		break;
	case RightBottom:
		x = CScreen::GetCoord(x, CScreen::SIDE_RIGHT);
		y = CScreen::GetCoord(y, CScreen::SIDE_BOTTOM);
		break;
	default:
		x = CScreen::GetCoord(x, CScreen::SIDE_LEFT);
		y = CScreen::GetCoord(y, CScreen::SIDE_TOP);
		break;
	}
	PrintUnscaled(line,
		x,
		y,
		style,
		CScreen::GetMultiplier(w),
		CScreen::GetMultiplier(h),
		color, alignment, dropPosition, dropColor, shadow,
		CScreen::GetCoord(lineSize),
		proportional, justify);
}

void TextPrinter::SPrint(ScreenSide screenSide, FontAlignment alignment,
	wchar* const& line, float x, float y, unsigned char style, float w, float h,
	CRGBA const& color, unsigned char dropPosition, CRGBA const& dropColor, bool shadow, float lineSize,
	bool proportional, bool justify)
{
	SPrint(line, x, y, style, w, h, color, alignment, dropPosition, dropColor, shadow, lineSize, proportional, justify, screenSide);
}

void TextPrinter::SPrint(std::vector<std::wstring> const& lines, float x, float y, float spacing, unsigned char style,
	float w, float h, CRGBA const& color, FontAlignment alignment, unsigned char dropPosition, CRGBA const& dropColor,
	bool shadow, float lineSize, bool proportional, bool justify, ScreenSide screenSide)
{
	spacing *= h * SCALEH * SPACING;
	for (size_t i = 0; i < lines.size(); i++) {
		SPrint(((wchar*)lines[i].c_str()), x, y + spacing * i, style, w, h, color, alignment, dropPosition,
			dropColor, shadow, lineSize, proportional, justify, screenSide);
	}
}

void TextPrinter::SPrint(ScreenSide screenSide, FontAlignment alignment,
	std::vector<std::wstring> const& lines, float x, float y, float spacing, unsigned char style, float w, float h,
	CRGBA const& color, unsigned char dropPosition, CRGBA const& dropColor, bool shadow, float lineSize,
	bool proportional, bool justify)
{
	spacing *= h * SCALEH * SPACING;
	for (size_t i = 0; i < lines.size(); i++) {
		SPrint(screenSide, alignment, ((wchar*)lines[i].c_str()), x, y + spacing * i, style, w, h, color,
			dropPosition, dropColor, shadow, lineSize, proportional, justify);
	}
}

bool TextPrinter::PrintAt3d(CVector const& posn, wchar* const& line, float offset_x, float offset_y,
	unsigned char style, float w, float h, CRGBA const& color, bool scaleOnDistance, FontAlignment alignment,
	unsigned char dropPosition, CRGBA const& dropColor, bool shadow, float lineSize, bool proportional, bool justify)
{
	CVector out;

	if (!Get3dTo2d(posn, out))
		return false;

	float s = 1.0f;
	if (scaleOnDistance)
		s = CalcScaling(out.z);
	if (s != 0.0f) {
		PrintOneAt3d(line, out, offset_x * s, offset_y * s, w * s, h * s, color, style,
			alignment, dropPosition, dropColor, shadow, lineSize, proportional, justify);
	}
	return true;
}

bool TextPrinter::PrintAt3d(CVector const& posn, std::vector<std::wstring> const& lines, float spacing, float offset_x,
	float offset_y, unsigned char style, float w, float h, CRGBA const& color, bool scaleOnDistance, FontAlignment alignment,
	unsigned char dropPosition, CRGBA const& dropColor, bool shadow, float lineSize, bool proportional, bool justify)
{
	CVector out;
	if (!Get3dTo2d(posn, out))
		return false;

	if (scaleOnDistance) {
		float s = CalcScaling(out.z);
		if (s == 0.0f)
			return false;
		w *= s;
		h *= s;
		offset_x *= s;
		offset_y *= s;
	}
	spacing *= h * SCALEH * SPACING;
	for (size_t i = 0; i < lines.size(); i++) {
		PrintOneAt3d(((wchar*)lines[i].c_str()), out, offset_x, offset_y + spacing * i, w, h, color, style,
			alignment, dropPosition, dropColor, shadow, lineSize, proportional, justify);
	}
	return true;
}
}