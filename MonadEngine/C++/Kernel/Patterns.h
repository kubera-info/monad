// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>
#include <vector>
#include <string>

// Boost
#include "boost/serialization/strong_typedef.hpp"

// Monad
#include "Exceptions/Exceptions.h"
#include "Math/Random.h"

namespace Monad::Kernel
{
	/// <summary>
	/// Vector of range-to-value mappings.
	/// Each entry maps a range threshold to a destination value.
	/// </summary>
	using VectorResPair = std::vector<std::pair<uint32_t, uint32_t>>;

	/// <summary>
	/// Selects an optimal value based on the given dimension size.
	/// </summary>
	/// <param name="resolutions">List of size-to-value mappings.</param>
	/// <param name="dimensionSize">Input value to be matched against ranges.</param>
	/// <param name="defaultValue">Value used if no range matches.</param>
	/// <returns>Selected value based on the range mapping.</returns>
	uint32_t GetOptimalMapSize(
		const VectorResPair& resolutions,
		const uint32_t dimensionSize,
		const uint32_t defaultValue
	) noexcept;

	using VectorPath = std::vector<std::filesystem::path>;

	/// <summary>
	/// Container for random string selection.
	/// </summary>
	template<typename S>
	struct VectorRandom : std::vector<S>
	{
		using std::vector<S>::vector;

		/// <summary>
		/// Returns a randomly selected string from the vector.
		/// </summary>
		const S& GetRandomString() const
		{
			THROW_EXC_IFFALSE(Exceptions::NoDataFound, !this->empty(), L"Random String");
			return std::vector<S>::operator[](Math::g_random.GetRandomInt(this->size() - 1));
		}

		/// <summary>
		/// Returns a randomly selected string and removes it from the vector.
		/// </summary>
		S GetRandomStringOnce()
		{
			auto ret = GetRandomString();
			erase_if(*this, [&ret](const auto& name) { return name == ret; });
			return ret;
		}
	};
}

#define MONAD_STRONG_TYPEDEF(T, D)                                                                               \
struct D                                                                                                         \
    : boost::totally_ordered1< D                                                                                 \
    , boost::totally_ordered2< D, T                                                                              \
    > >                                                                                                          \
{                                                                                                                \
    T t;                                                                                                         \
    explicit D(const T& t_) BOOST_NOEXCEPT_IF(boost::has_nothrow_copy_constructor<T>::value) : t(t_) {}          \
    D() BOOST_NOEXCEPT_IF(boost::has_nothrow_default_constructor<T>::value) : t() {}                             \
    D(const D & t_) BOOST_NOEXCEPT_IF(boost::has_nothrow_copy_constructor<T>::value) : t(t_.t) {}                \
    D& operator=(const D& rhs) BOOST_NOEXCEPT_IF(boost::has_nothrow_assign<T>::value) {t = rhs.t; return *this;} \
    D& operator=(const T& rhs) BOOST_NOEXCEPT_IF(boost::has_nothrow_assign<T>::value) {t = rhs; return *this;}   \
    operator const T&() const {return t;}                                                                        \
    operator T&() {return t;}                                                                                    \
    bool operator==(const D& rhs) const {return t == rhs.t;}                                                     \
	auto operator<=>(const D& rhs) const {return t <=> rhs.t;}\
};