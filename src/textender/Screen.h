/*
	Plugin-SDK (Grand Theft Auto) SHARED header file
	Authors: GTA Community. See more here
	https://github.com/DK22Pac/plugin-sdk
	Do not delete this comment block. Respect others' work!
*/

#pragma once

namespace TExtender {
	class CScreen {
	public:
		enum eScreenCoordTranslationSide {
			SIDE_ANY,
			SIDE_LEFT,
			SIDE_RIGHT,
			SIDE_TOP,
			SIDE_BOTTOM,
			SIDE_CENTER_LEFT,
			SIDE_CENTER_RIGHT,
			SIDE_CENTER_UP,
			SIDE_CENTER_DOWN
		};

		static void SetBaseResolution(float res);
		static float GetBaseResolution();
		static float GetMultiplier(float a);

		static float GetCoord(float a, eScreenCoordTranslationSide side);
		static float GetCoord(float a);
		static float GetCoordLeft(float a);
		static float GetCoordRight(float a);
		static float GetCoordTop(float a);
		static float GetCoordBottom(float a);
		static float GetCoordCenterLeft(float a);
		static float GetCoordCenterRight(float a);
		static float GetCoordCenterUp(float a);
		static float GetCoordCenterDown(float a);
		static float GetScreenHeight();
		static float GetScreenWidth();
		static float GetScreenCenterX();
		static float GetScreenCenterY();
		static float GetScreenMaxX();
		static float GetScreenMaxY();
	};
}