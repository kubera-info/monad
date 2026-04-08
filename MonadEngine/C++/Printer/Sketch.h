// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Templates.h"
#include "Renderer/Shader.h"

namespace Monad::Print
{
	struct TaskTexturePrinter;
	/// <summary>
	/// This class is a shader that creates a picture for further printing
	/// </summary>
	interface ShaderSketchGeneric abstract : Renderer::ShaderTextured<Renderer::ShaderGeneric>
	{
		friend TaskTexturePrinter;

		ShaderSketchGeneric();
		using ShaderTextured::ShaderTextured;
		/// <summary>
		/// Initializes a printing process of a current art
		/// </summary>
		virtual void OnPrint() = 0;
		virtual void ScreenshotRT() noexcept = 0;
		/// <summary>
		/// Sets status for the printing initialization in one scene frame
		/// </summary>
		void PrintOnce() noexcept;
		/// <summary>
		/// Assigns the pointer to global variable for easy calling (a shortcut)
		/// </summary>
		inline void CreateSketchVar() noexcept { m_me = this; }
		/// <summary>
		/// Releases temporary data and changes status 
		/// </summary>
		void ReleasePrintingLock() noexcept;

	private:
		bool IsSystem() const noexcept final;
		bool IsVisible() const noexcept final;

		Kernel::Me<ShaderSketchGeneric> m_me;

	protected:
		enum class PRINT_STATUSES : size_t
		{
			READY,
			START,
			PRINT_ONCE
		};
		std::atomic<PRINT_STATUSES> m_printOnce = PRINT_STATUSES::READY;
	} extern* g_shaderSketchGeneric;
}