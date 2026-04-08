// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <map>
#include <string>
// Monad
#include "AppArtTex.h"
#include "Repository/Repo.h"

namespace Monad::App
{
	/// <summary>
	/// Identifiers of palette textures stored in the repository.
	/// Used for binding palette resources in rendering and profiles.
	/// </summary>
	extern const std::string
		NAME_PALETTE,
		NAME_SECONDARY_PALETTE,
		NAME_PALETTE1,
		NAME_SECONDARY_PALETTE1;

	/// <summary>
	/// Application-level repository.
	/// Stores loaded artworks, palettes and user-related data.
	/// Acts as a bridge between filesystem, GPU resources and UI.
	/// </summary>
	struct Repository final : Repositories::RepositoryGeneric
	{
		Repository();
		OPER_DEL_NO_DEF_CTOR(Repository);

		/// <summary>
		/// Internal map type storing artworks and their GPU memory.
		/// </summary>
		using MapMemberMemories =
			Repositories::MapMember<
			TaskAndMemoryLookup,
			std::map<std::string, TaskAndMemoryLookup>
			>;

		using ArtIterator = MapMemberMemories::iterator;
		using ConstArtIterator = MapMemberMemories::const_iterator;

		/// <summary>
		/// Writes all user-related data:
		/// - palette colors
		/// - selected artwork
		/// </summary>
		void WriteCustomerData();

		/// <summary>
		/// Moves selection to the previous artwork.
		/// Wraps around if needed.
		/// </summary>
		ArtIterator& GetPreviousArt() noexcept;

		/// <summary>
		/// Moves selection to the next artwork.
		/// Wraps around if needed.
		/// </summary>
		ArtIterator& GetNextArt() noexcept;

		/// <summary>
		/// Loads all artwork data and uploads it to GPU memory.
		/// </summary>
		void ImportArts();

		/// <summary>
		/// Finds an artwork by filename and sets it as current.
		/// </summary>
		inline void FindAndSetArtByFilename(
			const std::string& filename
		);

		/// <summary>
		/// Container holding all loaded artworks.
		/// </summary>
		MapMemberMemories m_memArts;

		/// <summary>
		/// Tracks currently selected artwork.
		/// </summary>
		struct CurrentArtProfile
		{
			explicit CurrentArtProfile(
				ArtIterator end
			) noexcept;

			/// <summary>
			/// Reads last used artwork from persistent storage.
			/// </summary>
			void ReadForCurrent();

			ArtIterator m_currentArt;
		} m_currentArtProfile;

		/// <summary>
		/// Returns GPU texture associated with the given artwork.
		/// </summary>
		const Renderer::Texture& GetArtTexture(
			const Repository::ConstArtIterator art
		) const;

	private:
		/// <summary>
		/// Saves palette state into XML profile.
		/// </summary>
		void WriteXMLProfile();

		/// <summary>
		/// Stores current artwork name in system registry.
		/// </summary>
		void WriteCurrentArtNameIntoRegistry() noexcept;
	};
}

/// <summary>
/// Global shortcut to the application repository instance.
/// </summary>
#define g_repository \
	static_cast<Monad::App::Repository*>(Monad::Repositories::g_repositoryGeneric)