// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "XBOXControler.h"
// Platform
#include <windows.gaming.input.h>
// Monad
#include "Core/DXSample.h"
#include "Core/Win32ApplicationGeneric.h"
#include "Exceptions/Exceptions.h"
#include "Math.h"
#include "Cursor.h"
#include "Threads/__MonadThread.h"

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Gaming::Input;
using namespace Microsoft::WRL::Wrappers;
using Microsoft::WRL::ComPtr;

namespace Monad::Input
{
	using namespace GUI;

	namespace
	{
		// Helper function for deadzone filtering
		double ApplyDeadzone(
			double value,
			double speed
		) noexcept
		{
			constexpr auto DOUBLE_XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE = (8125.0 / 32167.0);
			return value < DOUBLE_XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && value > -DOUBLE_XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? 0.0 : value * speed;
		}

		void MoveCursor(
			long deltaX,
			long deltaY
		) noexcept
		{
			if (deltaX == 0 && deltaY == 0)
				return;

			POINT curPos;
			if (GetCursorPos(&curPos))
			{
				const auto newX = curPos.x + deltaX;
				const auto newY = curPos.y - deltaY;
				::SetCursorPos(newX, newY);
				POINT pt{};
				if (GetCursorPos(&pt))
					SetCursorPosGeneric(pt.x, pt.y);
			}
		}
	}

	ThreadXboxController::ThreadXboxController() noexcept :
		Thread(Threads::THREAD_NAME_XBOX_CONTROLLER)
	{}

	void ThreadXboxController::Routine()
	{
		constexpr DWORD tickTime = 2, inactiveTime = 512;

		ComPtr<IGamepadStatics> gamepadStatics;
		THROW_EXC_IFFAILED(Exceptions::InvalidClass,
			GetActivationFactory(
				HStringReference(
					RuntimeClass_Windows_Gaming_Input_Gamepad).Get(),
				&gamepadStatics),
			L"Runtime Class Windows Gaming Input Gamepad");

		while (!m_stopToken.stop_requested())
		{
			if (g_active)
			{
				const double
					w = DXSample::GetWidth(),
					h = DXSample::GetHeight(),
					stepInto = GetNormalizedAspectRatio2(w, h) * 1080.0 / h;
				constexpr double
					smallStep = 8.0,
					bigStep = 16.0;
				if (ComPtr<IVectorView<Gamepad*>> gamepads; SUCCEEDED(gamepadStatics->get_Gamepads(&gamepads)))
					if (uint32_t gamepadCount; SUCCEEDED(gamepads->get_Size(&gamepadCount)))
						if (gamepadCount)
						{
							for (uint32_t i = 0; i < gamepadCount; ++i)
								if (ComPtr<IGamepad> gamepad; SUCCEEDED(gamepads->GetAt(i, &gamepad)))
									if (GamepadReading gamepadReading; SUCCEEDED(gamepad->GetCurrentReading(&gamepadReading)))
									{
										const auto
											speedLeft = (gamepadReading.Buttons & GamepadButtons::GamepadButtons_LeftThumbstick) != 0 ? bigStep : smallStep,
											speedRight = (gamepadReading.Buttons & GamepadButtons::GamepadButtons_RightThumbstick) != 0 ? bigStep : smallStep;
										const auto
											xLeft = ApplyDeadzone(gamepadReading.LeftThumbstickX, speedLeft),
											yLeft = ApplyDeadzone(gamepadReading.LeftThumbstickY, speedLeft),
											xRight = ApplyDeadzone(gamepadReading.RightThumbstickX, speedRight),
											yRight = ApplyDeadzone(gamepadReading.RightThumbstickY, speedRight);
										constexpr auto divFactor = 1.0 / 1600.0;
										const auto speedFactor = w * divFactor * stepInto;
										const auto
											x = static_cast<long>((xLeft + xRight) * speedFactor),
											y = static_cast<long>((yLeft + yRight) * speedFactor);
										MoveCursor(x, y);
										static auto xboxClickOld = false;
										const auto xboxClick = gamepadReading.LeftTrigger || gamepadReading.RightTrigger
											|| (gamepadReading.Buttons & GamepadButtons::GamepadButtons_A) != 0;
										if (xboxClick != xboxClickOld)
											Win32ApplicationGeneric::PostMessage((xboxClickOld = xboxClick) ? WM_APP_CLICK : WM_APP_RELEASE);
									}
						}
						else
							Sleep(inactiveTime);
			}
			Sleep(tickTime);
		}
	}
}