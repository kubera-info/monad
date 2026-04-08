// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>
#include <sstream>
#include <unordered_map>
// Monad
#include "Kernel/Del.h"
#include "Kernel/Templates.h"

interface IXmlReader;

namespace Monad::Files
{
	// Typical XML tags and attributes in these projects:
	// The root element is named "monad". It can have attribute or nested elements.
	// At the moment the elements do not have text values.

	extern std::wstring MONAD_TAG, XML_ATTR_NAME, XML_ATTR_ID;

	/// <summary>
	/// A base XML reader, maps XML → STL containers
	/// It reads the tags semantics from a deriving class
	/// And XML writter
	/// </summary>
	struct XMLManager
	{
		/// <summary>
		/// One Tag and its contents
		/// </summary>
		struct XMLElement
		{
			using MapAttrs = Kernel::UnorderedMapWString<const std::wstring>;
			using InitializerListAttrs = std::initializer_list<MapAttrs::value_type>;
			using VectorNestedElements = std::vector<XMLElement>;
			using XMLAllocatorElement = std::allocator<XMLElement>;
			friend XMLManager;
			friend XMLAllocatorElement;
			XMLElement(
				IXmlReader* reader
			);
			XMLElement(
				const std::wstring& elementName,
				InitializerListAttrs attrs,
				VectorNestedElements nestedElements
			);
			XMLElement() noexcept = default;
			~XMLElement() = default;
			OPER_DEF_NO_DEF_CTOR(XMLElement)

				// Accessors
				const std::wstring& GetElementName() const noexcept;
#if defined EXTREME_XML
			const std::wstring& GetValue() const noexcept;
#endif
			const std::wstring& GetAttr(
				const std::wstring& name
			) const;
			float GetFloatAttr(
				const std::wstring& name
			) const;
			unsigned long long GetULongLongAttr(
				const std::wstring& name
			) const;
			wchar_t GetCharAttr(
				const std::wstring& name
			) const;
			bool GetBoolAttr(
				const std::wstring& name
			) const;
			const std::wstring& GetNameAttr() const;
			const std::wstring& GetIDAttr() const;

		private:
			void SaveXML(
				std::wstringstream& buffer
			) const;
			void SaveNonEmptyElement(
				std::wstringstream& buffer
			) const;
			void SaveEmptyElement(
				std::wstringstream& buffer
			) const;
			void SaveOpenAndAttrs(
				std::wstringstream& buffer
			) const;
			void SaveDescendants(
				std::wstringstream& buffer
			) const;
#if defined EXTREME_XML
			void SaveValue(
				std::wstringstream& buffer
			) const;
#endif

			/// <summary>
			/// Tag name of an XML element
			/// </summary>
			std::wstring m_elementName;
#if defined EXTREME_XML
			/// <summary>
			/// Text of a element
			/// </summary>
			std::wstring m_value;
#endif

		public:
			/// <summary>
			/// Element's Attributes
			/// </summary>
			MapAttrs m_attrs;
			/// <summary>
			/// Descendant elements
			/// </summary>
			VectorNestedElements m_nestedElements;
		};

		/// <summary>
		/// Ctor
		/// </summary>
		/// <param name="attrs"> - attributes of the XML root element</param>
		/// <param name="nestedElements"> - nested elements of the XML root element</param>
		XMLManager(
			XMLElement root);
		XMLManager(
			XMLElement::InitializerListAttrs attrs,
			XMLElement::VectorNestedElements nestedElements);
		virtual ~XMLManager() = default;
		/// Save current state to XML file
		/// </summary>
		bool HasNoNestedElements() const noexcept;
		const std::string BuildXML() const;

	protected:
		/// <summary>
		/// Processes the buffored XML contents using lambdas
		/// </summary>
		void Processor();

	private:
		using FnSemantics = void(*)(
			const XMLElement& element,
			XMLManager*);
		using MapSemantics = Kernel::UnorderedMapWString<const FnSemantics>;
		const MapSemantics c_semantics;

	public:
		/// <summary>
		/// XML processing lambdas
		/// </summary>
		using InitializerListFns = std::initializer_list<MapSemantics::value_type>;
		/// <summary>
		/// Ctor, parses from the stream (file?) (does not call Processor()). Only prepares the struct
		/// </summary>
		/// <param name="fileStream">XML input stream (UTF8)</param>
		/// <param name="pPairs">Functions called while parsing tags</param>
		XMLManager(
			IStream* const fileStream,
			InitializerListFns semantics);	// For reading

	protected:
		XMLElement m_rootOfDoc;
	};
}
#define XML_NO_ELEMENTS {}