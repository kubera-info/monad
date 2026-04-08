// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Audio/Jingle.h"
#include "Kernel/Events.h"

namespace Monad::App
{
	/// <summary>
	/// Indicates whether a swipe gesture is currently in progress.
	/// </summary>
	extern bool g_amIInGesture;

	/// <summary>
	/// Notifies the system about pointer press or release events
	/// (e.g. mouse button, touch input, controller trigger).
	/// </summary>
	/// <param name="leftButtonDown">
	/// True if the pointer is currently pressed, false if released.
	/// </param>
	void IsMouseDownStub(
		const bool leftButtonDown
	);

	/// <summary>
	/// Handles swipe gestures used for navigation on menu screens.
	/// Detects horizontal movement while the pointer is pressed.
	/// </summary>
	struct Swipe final :
		Kernel::EventFrameMoveStub,
		Kernel::EventPointerDownStub,
		Kernel::EventPointerUpStub
	{
		Swipe();

	private:
		/// <summary>
		/// Called every frame while a gesture is active.
		/// Tracks pointer movement to determine gesture direction.
		/// </summary>
		bool OnFrameMoveStub() noexcept final;

		/// <summary>
		/// Called when the pointer is pressed.
		/// Initializes gesture tracking.
		/// </summary>
		/// <returns>Standard event handling result.</returns>
		bool OnPointerDownStub() noexcept final;

		/// <summary>
		/// Called when the pointer is released.
		/// Evaluates the gesture and triggers an action if needed.
		/// </summary>
		/// <returns>Standard event handling result.</returns>
		bool OnPointerUpStub() final;

		/// <summary>
		/// Bounding values of the gesture movement.
		/// Used to detect horizontal swipe direction.
		/// </summary>
		float m_startX = 0.0f;
		float m_minX = 0.0f;
		float m_maxX = 0.0f;
		float m_minY = 0.0f;
		float m_maxY = 0.0f;

		/// <summary>
		/// Randomized click sound played after a successful swipe.
		/// Adds subtle feedback variation.
		/// </summary>
		const Audio::RandomJingle c_clicksSounds;
	};
}