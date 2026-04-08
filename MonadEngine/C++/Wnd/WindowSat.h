// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Files/Folder.h"
#include "Kernel/Patterns.h"
#include "Window.h"
#include "Renderer2D/2D.h"

namespace Monad::Wnd
{
	/// <summary>
	/// Manages satellite windows displayed on secondary monitors.
	/// These windows are independent from the main rendering surface
	/// and are typically used for auxiliary or decorative rendering.
	/// </summary>
	struct ManagerSatellites final
	{
		/// <summary>
		/// Window class used for satellite windows.
		/// 
		/// Each satellite window is created on a non-primary monitor.
		/// The purpose is usually visual decoration or background rendering
		/// without interfering with the main application window.
		/// </summary>
		struct WindowClassSatellite final : WindowClass
		{
			/// <summary>
			/// Registers the window class for satellite windows.
			/// </summary>
			WindowClassSatellite();

			/// <summary>
			/// Creates a satellite window.
			/// </summary>
			/// <param name="wndRect">Target monitor rectangle.</param>
			/// <param name="createWindowParameter">User-defined creation parameter.</param>
			/// <param name="appTitle">Window title.</param>
			/// <returns>Created window handle.</returns>
			HWND WindowFactory(
				const RECT& wndRect,
				LPVOID createWindowParameter,
				const I18N::I18NString& appTitle
			) const;
		} m_satelliteWindowClass;

		/// <summary>
		/// Synchronizes satellite windows with currently available monitors.
		/// Creates or removes windows as needed.
		/// </summary>
		void SetSatelliteWindows();

		/// <summary>
		/// Updates window titles of all satellite windows.
		/// </summary>
		void SetTitle() const noexcept;

		/// <summary>
		/// Constructs the satellite manager.
		/// </summary>
		ManagerSatellites(
			Renderer2D::Factory2D& factory2D
		);

		/// <summary>
		/// Destructor. Cleans up satellite resources.
		/// </summary>
		~ManagerSatellites();

		OPER_DEL_NO_DEF_CTOR(ManagerSatellites);

		/// <summary>
		/// Self-reference helper used by the framework.
		/// </summary>
		const Kernel::Me<ManagerSatellites> c_me;

		/// <summary>
		/// Folder containing satellite assets (images, resources).
		/// </summary>
		const Files::Path m_folderSat;

		/// <summary>
		/// Pool of random image names used for satellite backgrounds.
		/// </summary>
		const Kernel::VectorRandomString m_blobJpgNames;

		/// <summary>
		/// Reference to 2D renderer factory.
		/// </summary>
		Renderer2D::Factory2D& m_2D;

		/// <summary>
		/// Internal container holding active satellite windows.
		/// </summary>
		struct SetSatellites;
		std::unique_ptr<SetSatellites> m_satellites;
	} extern* g_managerSatellites;
}