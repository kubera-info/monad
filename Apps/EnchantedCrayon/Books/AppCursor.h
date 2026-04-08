// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Renderer/Combine.h"
#include "Renderer/Metallic.h"
#include "Time/TimeLerp.h"

namespace Monad::App
{
	/// <summary>
	/// Returns the currently selected crayon color.
	/// This value is used by the cursor and coloring logic.
	/// </summary>
	const DirectX::XMFLOAT3& GetCrayonColor() noexcept;

	/// <summary>
	/// Crayon-shaped cursor used for drawing and coloring.
	/// Handles animation, color changes and interaction logic.
	/// </summary>
	struct CombineCursorCrayon final : Renderer::Combine
	{
		CombineCursorCrayon();

		/// <summary>
		/// Starts the process of applying a new color to the cursor.
		/// Also triggers smooth color transition.
		/// </summary>
		/// <param name="cursorColor">New RGB color value.</param>
		void SetColor(
			const DirectX::XMFLOAT3& cursorColor
		);

		/// <summary>
		/// Called when the cursor is pressed
		/// (mouse button, touch, or controller input).
		/// </summary>
		void Push();

		/// <summary>
		/// Called when the cursor is released
		/// (mouse button, touch, or controller input).
		/// </summary>
		void Pop();

	private:
		/// <summary>
		/// Updates the transformation matrix of the cursor.
		/// Handles position and animation offsets.
		/// </summary>
		void OnNewMatrix(
			DirectX::XMFLOAT4X4& matrixToAlter
		) const noexcept final;

		/// <summary>
		/// Determines whether the cursor should be visible.
		/// </summary>
		bool IsVisible() const noexcept final;

		/// <summary>
		/// Applies the given color to all cursor components.
		/// </summary>
		void InternalSetColor(
			const DirectX::XMFLOAT3& cursorColor
		) noexcept;

		/// <summary>
		/// Animation controller for cursor push/pop movement.
		/// </summary>
		Time::TimeLerp m_timeLerpPushPop;

		/// <summary>
		/// Handles smooth color interpolation for the cursor.
		/// </summary>
		struct TimeLerpColorChange final : Time::TimeLerp
		{
			TimeLerpColorChange(
				CombineCursorCrayon& parent
			) noexcept;

		private:
			/// <summary>
			/// Called when color interpolation starts.
			/// </summary>
			void OnFlush() noexcept final;

			/// <summary>
			/// Called every frame during color transition.
			/// </summary>
			void OnFrameMove() noexcept final;

			CombineCursorCrayon& m_parent;
		} m_colorChange;

		/// <summary>
		/// Shader responsible for rendering the crayon body.
		/// </summary>
		struct ShaderCrayon final : Renderer::ShaderMetallic2
		{
			inline ShaderCrayon() noexcept;

			/// <summary>
			/// Sets the crayon color.
			/// </summary>
			void SetColor(
				const DirectX::XMFLOAT3& cursorColor
			) noexcept;

		private:
			void OnComputePerWorldFrame(
				const DirectX::XMFLOAT4X4& modelSpaceMatrix
			) final;

			DirectX::XMFLOAT3 m_rgb;
		} m_crayon;

		/// <summary>
		/// Shader responsible for rendering the paper label
		/// on the crayon.
		/// </summary>
		struct ShaderPaper final : Renderer::Shader
		{
			inline ShaderPaper();

			/// <summary>
			/// Sets the paper tint color.
			/// </summary>
			void SetColor(
				const DirectX::XMFLOAT3& cursorColor
			) noexcept;

		private:
			void OnComputePerWorldFrame(
				const DirectX::XMFLOAT4X4& modelSpaceMatrix
			) final;

			DirectX::XMFLOAT3 m_rgb;
			Renderer::CB::ForecolorInstance m_forecolor;
		} m_paper;
	};

	/// <summary>
	/// Animated hourglass cursor (torus).
	/// Used to indicate busy or waiting state.
	/// </summary>
	struct ShaderCursorTorus final : Renderer::ShaderMetallic2
	{
		ShaderCursorTorus();

	private:
		void OnNewMatrix(
			DirectX::XMFLOAT4X4& matrixToAlter
		) const noexcept final;

		bool IsVisible() const noexcept final;
	};
}