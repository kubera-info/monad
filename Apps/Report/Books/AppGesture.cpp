// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppGesture.h"
// Monad
#include "AppSingleton.h"
#include "Input/Cursor.h"

using namespace std;

namespace Monad
{
	using namespace Audio;
	using namespace Input;
	using namespace Kernel;

	namespace App
	{
		void IsMouseDownStub(
			const bool leftButtonDown
		)
		{

			if (CURSOR_TYPES::POINTER == g_cursorType
				&& !Exceptions::InAnyCloseReason()
				&& Files::FILE_MAN_STATE_READY == Files::g_fileManagerState)
				if (leftButtonDown)
					FireEvent(
						EVENT_IDS::POINTER_DOWN
					);
				else
					FireEvent(
						EVENT_IDS::POINTER_UP
					);
		}
	}
}