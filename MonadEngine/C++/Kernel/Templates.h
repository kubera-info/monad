// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// STD
#include <set>
#include <unordered_map>
#include <unordered_set>
// Monad
#include "__MonadKernel.h"
#include "Renderer/__MonadRenderer.h"

namespace Monad::Kernel
{
	/// <summary>
	/// Trait used to detect whether a class provides a static Get() method.
	/// </summary>
	template<typename T, typename = void>
	struct HasMethodGet : std::false_type {};

	/// <summary>
	/// Specialization for types that define a static Get() method.
	/// </summary>
	template<typename T>
	struct HasMethodGet<T, decltype(static_cast<void>(T::Get), 0)>
		: std::true_type {};

	/// <summary>
	/// Checks whether a value lies within a given range (inclusive).
	/// </summary>
	/// <typeparam name="N">Numeric type.</typeparam>
	/// <param name="minimum">Lower bound.</param>
	/// <param name="maximum">Upper bound.</param>
	/// <param name="value">Value to test.</param>
	/// <returns>True if value ∈ [minimum, maximum].</returns>
	template<typename N>
		requires (std::is_integral_v<N> || std::is_floating_point_v<N>)
	inline bool between(
		const N minimum,
		const N maximum,
		const N value
	) noexcept
	{
		return minimum <= value && value <= maximum;
	}

	/// <summary>
	/// Hash and equality functor for POD-like objects.
	/// Designed for use with unordered containers.
	/// </summary>
	/// <typeparam name="POD">Type to hash (must be trivially copyable).</typeparam>
	template<typename POD>
	struct ObjectHash
	{
		/// <summary>
		/// Hash function.
		/// </summary>
		size_t operator()(const POD& v) const noexcept
		{
			return std::_Hash_array_representation(
				reinterpret_cast<const uint8_t*>(&v),
				sizeof(POD));
		}

		/// <summary>
		/// Equality comparison.
		/// </summary>
		bool operator()(const POD& lhs, const POD& rhs) const noexcept
		{
			return std::strong_ordering::equivalent == (lhs <=> rhs);
		}
	};

	template<typename V>
	using UnorderedSet = std::unordered_set<V, ObjectHash<V>, ObjectHash<V>>;

	template<typename K, typename V>
	using UnorderedMap = std::unordered_map<K, V, ObjectHash<K>, ObjectHash<K>>;

	template<typename V>
	using UnorderedMapWString = std::unordered_map<std::wstring, V>;

	template<typename V>
	using UnorderedMapString = std::unordered_map<std::string, V>;

	/// <summary>
	/// Replaces all occurrences of a substring in a string.
	/// </summary>
	/// <typeparam name="S">Replacement type (wstring, wstring_view, wchar_t*).</typeparam>
	/// <param name="str">Target string.</param>
	/// <param name="from">Substring to replace.</param>
	/// <param name="to">Replacement text.</param>
	template<typename S = std::wstring>
	void ReplaceAll(
		std::wstring& str,
		const S& from,
		const S& to = L""
	)
	{
		for (size_t pos = 0;
			(pos = str.find(from, pos)) != std::wstring::npos;)
		{
			str.replace(pos, from.length(), to);
			pos += to.length();
		}
	}

	/// <summary>
	/// Helper that assigns a global pointer to the owning object.
	/// Must be declared as the first member of a class.
	/// </summary>
	/// <typeparam name="T">Owning class type.</typeparam>
	template<typename T>
		requires std::is_class_v<T>
	struct Me final
	{
		explicit Me(
			T* parentThis,
			T*& globalPtr
		) noexcept
			: m_globalPtr(&globalPtr)
		{
			*m_globalPtr = parentThis;
		}

		explicit Me(T*& globalPtr) noexcept
			: Me(nullptr, globalPtr)
		{}

		void operator=(T* parentThis) noexcept
		{
			*m_globalPtr = parentThis;
		}

		~Me()
		{
			// Prevent dangling pointer
			*m_globalPtr = nullptr;
		}

	private:
		T** m_globalPtr;
	};

	/// <summary>
	/// Frame-indexed container (front/back buffering).
	/// </summary>
	template<typename R>
	struct ArrayFrames : std::array<R, Renderer::FRAME_COUNT>
	{
		auto operator->() noexcept
		{
			return &(*this)[Renderer::g_frameIndex];
		}
	};
}
