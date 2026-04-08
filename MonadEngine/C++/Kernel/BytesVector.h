// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <wrl.h>
// STD
#include <span>
#include <vector>

namespace Monad::Kernel
{
	using PtrIStream = Microsoft::WRL::ComPtr<IStream>;
	using SpanBytes = std::span<uint8_t>;

	/// <summary>
	/// Immutable view over a block of binary data.
	/// Typically represents a fragment of a byte vector.
	/// </summary>
	struct SpanConstBytes final : public std::span<const uint8_t>
	{
		using span::span;

		template<typename V>
			requires (!(std::is_same_v<V, std::wstring> ||
		std::is_same_v<V, std::wstring_view>))
			SpanConstBytes(const V& buffer) noexcept
			: span{
				reinterpret_cast<const uint8_t*>(buffer.data()),
				buffer.size() * sizeof(typename V::value_type)
			}
		{}

		/// <summary>
		/// Creates a temporary COM IStream backed by this data.
		/// </summary>
		PtrIStream GetComIStreamTemp() const;

		/// <summary>
		/// Creates a COM IStream backed by a copy of this data.
		/// </summary>
		PtrIStream GetComIStreamOnCopy() const;
	};

	/// <summary>
	/// Allocator adaptor that prevents value-initialization
	/// and instead performs default initialization.
	///
	/// See:
	/// https://stackoverflow.com/questions/21028299/
	/// </summary>
	template<typename T, typename A = std::allocator<T>>
	struct default_init_allocator : A
	{
		using traits = std::allocator_traits<A>;

		template<typename U>
		struct rebind
		{
			using other =
				default_init_allocator<U,
				typename traits::template rebind_alloc<U>>;
		};

		using A::A;

		template<typename U>
		void construct([[maybe_unused]] U* ptr) noexcept
		{
			// Intentionally does nothing (no value initialization)
		}
	};

	/// <summary>
	/// Vector implementation that avoids default construction
	/// of elements for performance reasons.
	/// </summary>
	template<typename POD>
	struct VectorNoCtor : std::vector<POD, default_init_allocator<POD>>
	{
		using std::vector<POD, default_init_allocator<POD>>::vector;

		/// <summary>
		/// Clears the vector and releases allocated memory.
		/// </summary>
		void ClearAndShrink() noexcept
		{
			this->clear();
			this->shrink_to_fit();
		}
	};

	/// <summary>
	/// Byte buffer used for binary data such as streams,
	/// compiled shaders, or serialized assets.
	/// </summary>
	struct VectorBytes : VectorNoCtor<uint8_t>
	{
		using VectorNoCtor::VectorNoCtor;

		/// <summary>
		/// Constructs a byte buffer from a constant byte span.
		/// </summary>
		VectorBytes(const SpanConstBytes sourceBytes);
	};
}
