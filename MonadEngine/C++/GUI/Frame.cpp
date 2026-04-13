// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Frame.h"

// Monad
#include "Kernel/Format.h"
#include "Math/Math.h"
#include "Renderer/MeshCore.h"
#include "Repository/Repo.h"
#include "Renderer/MeshOpt.h"

using namespace DirectX;
using namespace std::literals::string_literals;

namespace Monad::GUI
{
	namespace
	{
		struct MeshCreatorPipe : Renderer::InputLayout::MeshBuilderPlain
		{
			inline MeshCreatorPipe(
				const XMFLOAT4& pipe
			) :
				c_pipe(pipe)
			{
				VERTEX_TYPE buildLeftUpper, buildRightUpper;
				VERTEX_TYPE buildRightLower, buildLeftLower;
				VERTEX_TYPE buildLeftUpper2, buildRightUpper2;
				VERTEX_TYPE buildRightLower2, buildLeftLower2;
				const Math::Ellipse pipeEllipse(
					8,
					c_scale,
					c_scale / 5,
					XM_PI
				);

				for (float iR = 0; pipeEllipse.GetSidesCount() > iR; ++iR)
				{
					float const move = c_scale * iR / (pipeEllipse.GetSidesCount() * 2);
					float const move2 = c_scale * (iR + 1) / (pipeEllipse.GetSidesCount() * 2);

					buildLeftUpper.m_position.z = buildRightUpper.m_position.z = -pipeEllipse[static_cast<size_t>(iR)].y;
					buildLeftLower.m_position.z = buildRightLower.m_position.z = -pipeEllipse[static_cast<size_t>(iR) + 1].y;

					buildLeftUpper.m_position.x = c_pipe.x + move;
					buildRightUpper.m_position.x = c_pipe.z - move;
					buildLeftUpper.m_position.y = buildRightUpper.m_position.y = c_pipe.w * Kernel::LANDSCAPE_RATIO - move;

					buildLeftLower.m_position.x = c_pipe.x + move2;
					buildRightLower.m_position.x = c_pipe.z - move2;
					buildLeftLower.m_position.y = buildRightLower.m_position.y = c_pipe.w * Kernel::LANDSCAPE_RATIO - move2;

					Up(buildLeftUpper, buildLeftLower, buildRightUpper, buildRightLower);

					buildLeftUpper2 = buildLeftUpper; buildRightUpper2 = buildRightUpper;
					buildRightLower2 = buildRightLower; buildLeftLower2 = buildLeftLower;

					buildLeftUpper2.m_position.y = buildRightUpper2.m_position.y = c_pipe.y * Kernel::LANDSCAPE_RATIO + move;
					buildLeftLower2.m_position.y = buildRightLower2.m_position.y = c_pipe.y * Kernel::LANDSCAPE_RATIO + move2;

					Down(buildLeftUpper2, buildRightUpper2, buildLeftLower2, buildRightLower2);

					Left(buildLeftUpper, buildLeftLower2, buildLeftLower, buildLeftUpper2);

					Right(buildRightUpper, buildRightLower, buildRightLower2, buildRightUpper2);
				}

				ComputeNormals();
			}

			/// <summary>
			/// Facade providing a simplified interface for invoking AddTriangle.
			/// </summary>
			/// <param name="a">First vertex</param>
			/// <param name="b">Second vertex</param>
			/// <param name="c">Third vertex</param>
			/// <param name="normalID">ID of group of vertices, this id forces, that vertex normal will be computed only within its group. It makes sharp edges between groups.</param>
			void AddTriangleMetallic(
				VERTEX_TYPE a,
				VERTEX_TYPE b,
				VERTEX_TYPE c,
				float normalID
			)
			{
				a.m_normal = b.m_normal = c.m_normal = { normalID, 0, 0 };
				AddTriangle(a, b, c);
			}

			inline void Up(
				const VERTEX_TYPE& leftUpper,
				const VERTEX_TYPE& leftLower,
				const VERTEX_TYPE& rightUpper,
				const VERTEX_TYPE& rightLower
			)
			{
				for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += c_lerpStep)
				{
					VERTEX_TYPE buildLeftUpper(leftUpper), buildLeftLower(leftLower), buildRightUpper(rightUpper), buildRightLower(rightLower);
					buildLeftUpper.m_position.x = std::lerp(leftUpper.m_position.x, rightUpper.m_position.x, lerpVert);
					buildRightUpper.m_position.x = std::lerp(leftUpper.m_position.x, rightUpper.m_position.x, lerpVert + c_lerpStep);
					buildLeftLower.m_position.x = std::lerp(leftLower.m_position.x, rightLower.m_position.x, lerpVert);
					buildRightLower.m_position.x = std::lerp(leftLower.m_position.x, rightLower.m_position.x, lerpVert + c_lerpStep);
					AddTriangleMetallic(
						buildLeftUpper,
						buildLeftLower,
						buildRightUpper,
						1.0f
					);
					AddTriangleMetallic(
						buildLeftLower,
						buildRightLower,
						buildRightUpper,
						1.0f
					);
				}
			}

			inline void Down(
				const VERTEX_TYPE& leftUpper2,
				const VERTEX_TYPE& rightUpper2,
				const VERTEX_TYPE& leftLower2,
				const VERTEX_TYPE& rightLower2
			)
			{
				for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += c_lerpStep)
				{
					VERTEX_TYPE buildLeftUpper2(leftUpper2), buildRightUpper2(rightUpper2), buildLeftLower2(leftLower2), buildRightLower2(rightLower2);
					buildLeftUpper2.m_position.x = std::lerp(leftUpper2.m_position.x, rightUpper2.m_position.x, lerpVert);
					buildRightUpper2.m_position.x = std::lerp(leftUpper2.m_position.x, rightUpper2.m_position.x, lerpVert + c_lerpStep);
					buildLeftLower2.m_position.x = std::lerp(leftLower2.m_position.x, rightLower2.m_position.x, lerpVert);
					buildRightLower2.m_position.x = std::lerp(leftLower2.m_position.x, rightLower2.m_position.x, lerpVert + c_lerpStep);
					AddTriangleMetallic(
						buildLeftUpper2,
						buildRightUpper2,
						buildLeftLower2,
						.5f);
					AddTriangleMetallic(
						buildLeftLower2,
						buildRightUpper2,
						buildRightLower2,
						.5f
					);
				}
			}

			inline void Left(
				const VERTEX_TYPE& leftUpper,
				const VERTEX_TYPE& leftLower2,
				const VERTEX_TYPE& leftLower,
				const VERTEX_TYPE& leftUpper2
			)
			{
				for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += c_lerpStep)
				{
					VERTEX_TYPE buildLeftUpper(leftUpper), buildLeftLower2(leftLower2), buildLeftLower(leftLower), buildLeftUpper2(leftUpper2);
					buildLeftUpper.m_position.y = std::lerp(leftUpper.m_position.y, leftLower.m_position.y, lerpVert);
					buildLeftLower.m_position.y = std::lerp(leftUpper.m_position.y, leftLower.m_position.y, lerpVert + c_lerpStep);
					buildLeftUpper2.m_position.y = std::lerp(leftUpper2.m_position.y, leftLower2.m_position.y, lerpVert);
					buildLeftLower2.m_position.y = std::lerp(leftUpper2.m_position.y, leftLower2.m_position.y, lerpVert + c_lerpStep);
					AddTriangleMetallic(
						buildLeftUpper,
						buildLeftLower2,
						buildLeftLower,
						.25f
					);
					AddTriangleMetallic(
						buildLeftLower2,
						buildLeftUpper,
						buildLeftUpper2,
						.25f
					);
				}
			}

			inline void Right(
				const VERTEX_TYPE& rightUpper,
				const VERTEX_TYPE& rightLower,
				const VERTEX_TYPE& rightLower2,
				const VERTEX_TYPE& rightUpper2
			)
			{
				for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += c_lerpStep)
				{
					VERTEX_TYPE buildRightUpper(rightUpper), buildRightLower(rightLower), buildRightLower2(rightLower2), buildRightUpper2(rightUpper2);
					buildRightUpper.m_position.y = std::lerp(buildRightUpper.m_position.y, rightLower.m_position.y, lerpVert);
					buildRightLower.m_position.y = std::lerp(buildRightLower.m_position.y, rightLower.m_position.y, lerpVert + c_lerpStep);
					buildRightUpper2.m_position.y = std::lerp(buildRightUpper2.m_position.y, rightLower2.m_position.y, lerpVert);
					buildRightLower2.m_position.y = std::lerp(buildRightLower2.m_position.y, rightLower2.m_position.y, lerpVert + c_lerpStep);
					AddTriangleMetallic(
						buildRightUpper,
						buildRightLower,
						buildRightLower2,
						.125f
					);
					AddTriangleMetallic(
						buildRightLower2,
						buildRightUpper2,
						buildRightUpper,
						.125f
					);
				}
			}

			const DirectX::XMFLOAT4 c_pipe;
			static constexpr float
				c_scale = 0.065f,
				c_lerpStep = 0.0625f;
		};
	}

	Renderer::Mesh DialogBorderMesh(
		const DirectX::XMFLOAT4& pipe
	)
	{
		// ":" means it is a system resource, not from OS file. 
		// It is used to distinguish meshes created on the fly from meshes loaded from files,
		// because they are stored in the same repository.
		// There is no the short filename with ":" prefix, so it is safe to use it as a marker.
		const std::string dialogRectName =
			":"s.append(std::to_string(pipe.x)).
			append(","s).
			append(std::to_string(pipe.y)).
			append(","s).
			append(std::to_string(pipe.z)).
			append(","s).
			append(std::to_string(pipe.w));
		// try_emplace does not work here, because the value is a complete mesh and the leazy evaluation cannot be done

		if (auto iterMsh = Repositories::g_repositoryGeneric->m_meshes.find(dialogRectName);
			Repositories::g_repositoryGeneric->m_meshes.cend() != iterMsh
			)
			return iterMsh->second;
		else
		{
			MeshCreatorPipe frame{ pipe };
			auto retMesh = Renderer::OptimizeMesh(
				frame.m_indices,
				{ 
					frame.m_vertices.data(),
					sizeof(MeshCreatorPipe::VERTEX_TYPE),
					frame.m_vertices.size()
				}
			);	

			return Repositories::g_repositoryGeneric->m_meshes.emplace(
				dialogRectName,
				Renderer::Mesh::MeshHelper{ retMesh.m_indices, retMesh.m_vertices }
			).first->second;
		}
	}
}
