// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include "Modules/Xbox-ATG-Samples/Kits/CPUTK/OSLockable.h"
// STD
#include <mutex>
#include <string>
// Monad
#include "Exceptions/Exceptions.h"
#include "Fonts/FontMeta.h"
#include "Kernel/BytesVector.h"
#include "Kernel/Templates.h"
#include "Renderer/__MonadRenderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Tex.h"
#include "Kernel/Events.h"
#include "Tools/Convert.h"

namespace Monad::Repositories
{
	template<typename V, typename M = Kernel::UnorderedMapString<V>>
	struct MapMember : M
	{
		using M::M;

		ATG::CriticalSectionLockable m_lockSafeInsert;

		/// <summary>
		/// Gets element by name
		/// </summary>
		/// <param name="filename">The name of the object</param>
		/// <returns>Refernce to object from repo</returns>
		V& GetMember(
			const std::string& filename
		)
		{
			auto object(this->find(filename));
			THROW_EXC_ONEND(
				*this,
				object,
				L"Map Get Member: "
				+ Tools::UTF8ToUnicode(filename));
			return object->second;
		}

		/// <summary>
		/// Gets element by name
		/// </summary>
		/// <param name="filename">The name of the object</param>
		/// <returns>Refernce to object from repo</returns>
		const V& GetMember(
			const std::string& filename
		) const
		{
			const auto object(this->find(filename));
			THROW_EXC_ONEND(
				*this,
				object,
				L"Map Get Member: "
				+ Tools::UTF8ToUnicode(filename));
			return object->second;
		}

		V& GetMember(
			const std::string_view filename
		)
		{
			return GetMember(std::string(filename));
		}

		const V& GetMember(
			const std::string_view filename
		) const
		{
			return GetMember(std::string(filename));
		}

		template <class... _Mappedty>
		void Insert(
			std::string&& filename,
			_Mappedty&&... internalType
		)
		{
			const std::lock_guard lg(m_lockSafeInsert);
			this->UnsafeInsert(
				std::move(filename),
				std::forward<_Mappedty>(internalType)...
			);
		}

		template <class... _Mappedty>
		void Insert(
			const std::string& filename,
			_Mappedty&&... internalType
		)
		{
			const std::lock_guard lg(m_lockSafeInsert);
			this->UnsafeInsert(
				filename,
				std::forward<_Mappedty>(internalType)...
			);
		}

		template <class... _Mappedty>
		void UnsafeInsert(
			std::string&& filename,
			_Mappedty&&... internalType
		)
		{
			if (!this->try_emplace(
				std::move(filename),
				std::forward<_Mappedty>(internalType)...).second
				)
				THROW_EXC(
					Exceptions::ClassAlreadyExists,
					HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS),
					L"Map Try Emplace: "
					+ Tools::UTF8ToUnicode(filename)
				);
		}

		template <class... _Mappedty>
		void UnsafeInsert(
			const std::string& filename,
			_Mappedty&&... internalType
		)
		{
			if (!this->try_emplace(
				filename,
				std::forward<_Mappedty>(internalType)...).second)
				THROW_EXC(
					Exceptions::ClassAlreadyExists,
					HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS),
					L"Map Try Emplace: "
					+ Tools::UTF8ToUnicode(filename)
				);
		}
	};

	template<typename V, typename M = Kernel::UnorderedMapString<V>>
	struct MapMemberD3DAssets : MapMember<V, M>, Kernel::EventClearD3DAssetsStub
	{
		MapMemberD3DAssets()
		{
			Register();
		}

		bool OnClearD3DAssetsStub() noexcept final
		{
			this->m_lockSafeInsert.lock();
			this->clear();
			this->m_lockSafeInsert.unlock();
			return true;
		}
	};

	struct RepositoryGeneric
	{
		RepositoryGeneric();
		void AddGhostTexture();
		void OnClearD3DAssets() noexcept;

	private:
		const Kernel::Me<RepositoryGeneric> c_me;

	public:
		struct MapMembersTextures : MapMemberD3DAssets<Renderer::Texture>
		{
			Renderer::Texture& LoadEmoji(const std::string& filename);
		} m_shaderResources;
		MapMemberD3DAssets<Renderer::Mesh> m_meshes;
		MapMemberD3DAssets<Renderer::VectorBytesBytecode> m_ps;
		MapMemberD3DAssets<Renderer::ConfigVS> m_vs;
		Fonts::MapFonts m_dialogFonts;
	} extern* g_repositoryGeneric;
}