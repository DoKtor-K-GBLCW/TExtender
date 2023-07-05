/*
	Plugin-SDK (Grand Theft Auto) SHARED header file
	Authors: GTA Community. See more here
	https://github.com/DK22Pac/plugin-sdk
	Do not delete this comment block. Respect others' work!
*/

#include "Screen.h"
#include "common.h"

// Screen Class
namespace TExtender {
#define SCREEN_RESOLUTION_DEFAULT 1080.0f
	float gScreenResolution = SCREEN_RESOLUTION_DEFAULT;

	void CScreen::SetBaseResolution(float res) {
		gScreenResolution = res;
	}

	float CScreen::GetBaseResolution() {
		return gScreenResolution;
	}

	float CScreen::GetCoord(float a) {
		float base = GetScreenWidth() > GetScreenHeight() ? static_cast<float>(GetScreenHeight()) : static_cast<float>(GetScreenWidth());
		return static_cast<float>(static_cast<int>(a * base / gScreenResolution));
	}

	float CScreen::GetCoord(float a, eScreenCoordTranslationSide side) {
		switch (side) {
		case SIDE_ANY:
			return GetCoord(a);
		case SIDE_LEFT:
			return GetCoordLeft(a);
		case SIDE_RIGHT:
			return GetCoordRight(a);
		case SIDE_TOP:
			return GetCoordTop(a);
		case SIDE_BOTTOM:
			return GetCoordBottom(a);
		case SIDE_CENTER_LEFT:
			return GetCoordCenterLeft(a);
		case SIDE_CENTER_RIGHT:
			return GetCoordCenterRight(a);
		case SIDE_CENTER_UP:
			return GetCoordCenterUp(a);
		case SIDE_CENTER_DOWN:
			return GetCoordCenterDown(a);
		}
		return 0.0f;
	}

	float CScreen::GetCoordLeft(float a) {
		return GetCoord(a);
	}

	float CScreen::GetCoordRight(float a) {
		return GetScreenWidth() - GetCoord(a);
	}

	float CScreen::GetCoordTop(float a) {
		return GetCoord(a);
	}

	float CScreen::GetCoordBottom(float a) {
		return GetScreenHeight() - GetCoord(a);
	}

	float CScreen::GetCoordCenterLeft(float a) {
		return GetScreenCenterX() - GetCoord(a);
	}

	float CScreen::GetCoordCenterRight(float a) {
		return GetScreenCenterX() + GetCoord(a);
	}

	float CScreen::GetCoordCenterUp(float a) {
		return GetScreenCenterY() - GetCoord(a);
	}

	float CScreen::GetCoordCenterDown(float a) {
		return GetScreenCenterY() + GetCoord(a);
	}

	float CScreen::GetMultiplier(float a) {
		float base = GetScreenWidth() > GetScreenHeight() ? static_cast<float>(GetScreenHeight()) : static_cast<float>(GetScreenWidth());
		return a * base / gScreenResolution;
	}

	float CScreen::GetScreenHeight() {
		return static_cast<float>(RsGlobal.maximumHeight);
	}

	float CScreen::GetScreenWidth() {
		return static_cast<float>(RsGlobal.maximumWidth);
	}

	float CScreen::GetScreenCenterX() {
		return GetScreenWidth() / 2.0f;
	}

	float CScreen::GetScreenCenterY() {
		return GetScreenHeight() / 2.0f;
	}

	float CScreen::GetScreenMaxX() {
		return GetScreenWidth();
	}

	float CScreen::GetScreenMaxY() {
		return GetScreenHeight();
	}
}