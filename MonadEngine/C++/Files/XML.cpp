// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "XML.h"
// Platform
#include <wrl.h>
#include <xmllite.h>
#pragma comment (lib, "XmlLite.lib")
// Monad
#include "Exceptions/Exceptions.h"
#include "Tools/Convert.h"

using namespace std;
namespace fs = std::filesystem;

namespace Monad
{
	using namespace Kernel;

	namespace Files
	{
		wstring
			MONAD_TAG = L"monad"s,
			XML_ATTR_NAME = L"name"s,
			XML_ATTR_ID = L"id"s;

		namespace
		{
			using PairTransformChar = pair<const wstring_view&, const wstring_view>;
			constexpr wstring_view
				CHAR_R{ L"\r" },
				CHAR_AMP{ L"&" },
				CHAR_NL{ L"\n" },
				CHAR_LT{ L"<" },
				CHAR_GT{ L">" },
				CHAR_SPC{ L" " },
				CHAR_TAB{ L"\t" },
				CHAR_QUO{ L"\"" },

				CHAR_R_2{ L"" },
				CHAR_AMP_2{ L"&amp;" },
				CHAR_NL_2{ L"&#13;&#10;" },
				CHAR_LT_2{ L"&lt;" },
				CHAR_GT_2{ L"&gt;" },
				CHAR_SPC_2{ L"&#32;" },
				CHAR_TAB_2{ L"&#9;" },
				CHAR_QUO_2{ L"&#34;" };

			inline constexpr PairTransformChar
				CHAR_PAIR_R{ CHAR_R, CHAR_R_2 },
				CHAR_PAIR_AMP{ CHAR_AMP, CHAR_AMP_2 },
				CHAR_PAIR_NL{ CHAR_NL, CHAR_NL_2 },
				CHAR_PAIR_LT{ CHAR_LT, CHAR_LT_2 },
				CHAR_PAIR_GT{ CHAR_GT, CHAR_GT_2 },
				CHAR_PAIR_SPC{ CHAR_SPC, CHAR_SPC_2 },
				CHAR_PAIR_TAB{ CHAR_TAB, CHAR_TAB_2 },
				CHAR_PAIR_QUO{ CHAR_QUO, CHAR_QUO_2 };

			using VectorTransform = const vector<PairTransformChar>;

			VectorTransform g_attrPairs =
			{
				CHAR_PAIR_R,
				CHAR_PAIR_AMP,
				CHAR_PAIR_NL,
				CHAR_PAIR_QUO,
				CHAR_PAIR_LT
			};

			/*VectorTransform g_valuePairs =
			{
				CHAR_PAIR_R,
				CHAR_PAIR_AMP,
				CHAR_PAIR_NL,
				CHAR_PAIR_LT,
				CHAR_PAIR_SPC,
				CHAR_PAIR_TAB
			};*/
			void TransformToQuotedPrintable(
				wstring& quoteString,
				VectorTransform characterToQuotedPrintablePairs
			)
			{
				for (auto const& quotedPair : characterToQuotedPrintablePairs)
				{
					ReplaceAll(
						quoteString,
						quotedPair.first,
						quotedPair.second
					);
				}
			}
		}

#pragma region XMLManager
		XMLManager::XMLManager(
			IStream* const fileStream,
			InitializerListFns semantics
		) :
			c_semantics{ semantics }
		{
			Microsoft::WRL::ComPtr<IXmlReader> xmlReader;
			THROW_EXC_IFFAILED_DB(
				CreateXmlReader(
					__uuidof(IXmlReader),
					reinterpret_cast<void**>(xmlReader.GetAddressOf()), nullptr),
				L"Create Xml Reader");
			THROW_EXC_IFFAILED_DB(
				xmlReader->SetProperty(
					XmlReaderProperty_DtdProcessing,
					DtdProcessing_Prohibit),
				L"Prohibit Validation"
			);
			THROW_EXC_IFFAILED_DB(
				xmlReader->SetInput(fileStream),
				L"Set Input");

			XmlNodeType nodeType;

			while (!xmlReader->IsEOF())
			{
				THROW_EXC_IFFAILED_DB(
					xmlReader->Read(&nodeType),
					L"Read Node");

				if (XmlNodeType_Element == nodeType)
				{
					m_rootOfDoc = xmlReader.Get();
					break;
				}
			}
		}

		XMLManager::XMLManager(
			XMLElement root
		) :
			m_rootOfDoc{ root }
		{}

		XMLManager::XMLManager(
			XMLElement::InitializerListAttrs attrs,
			XMLElement::VectorNestedElements nestedElements
		) :
			XMLManager{ { MONAD_TAG, attrs, nestedElements } }
		{}

		bool XMLManager::HasNoNestedElements() const noexcept
		{
			return m_rootOfDoc.m_nestedElements.empty();
		}

		void XMLManager::Processor()
		{
			for (const auto& element : m_rootOfDoc.m_nestedElements)
			{
				if (const auto fun(c_semantics.find(element.GetElementName())); fun != c_semantics.cend())
					fun->second(element, this);
				else
				{
#if defined _DEBUG
					System::MndOutputDebugString(L"\n Element: \"");
					System::MndOutputDebugString(element.GetElementName());
					System::MndOutputDebugString(L"\" has no implemenatation\n");
#endif
				}
			}
		}
		const std::string XMLManager::BuildXML() const
		{
			wstringstream buffer;
			m_rootOfDoc.SaveXML(buffer);
			return Tools::UnicodeToUTF8(buffer.str());
		}
#pragma endregion

#pragma region XMLElement
		void XMLManager::XMLElement::SaveXML(
			wstringstream& buffer
		) const
		{
			SaveOpenAndAttrs(buffer);
			if (m_nestedElements.empty()
#if defined EXTREME_XML
				&& m_value.empty()
#endif
				)
				SaveEmptyElement(buffer);
			else
				SaveNonEmptyElement(buffer);
		}

		void XMLManager::XMLElement::SaveNonEmptyElement(
			wstringstream& buffer
		) const
		{
			buffer << L'>';
			SaveDescendants(buffer);
#if defined EXTREME_XML
			SaveValue(buffer);
#endif
			buffer << L"</" + m_elementName + L'>';
		}

		void XMLManager::XMLElement::SaveEmptyElement(
			wstringstream& buffer
		) const
		{
			buffer << L"/>";
		}

		void XMLManager::XMLElement::SaveOpenAndAttrs(
			wstringstream& buffer
		) const
		{
			buffer << L'<' << m_elementName;
			for (const auto& attr : m_attrs)
			{
				wstring attrValue = attr.second;
				TransformToQuotedPrintable(attrValue, g_attrPairs);
				buffer << L' ' << attr.first << L"=\"" << attrValue << L'"';
			}
		}

		void XMLManager::XMLElement::SaveDescendants(
			wstringstream& buffer
		) const
		{
			for (const auto& element : m_nestedElements)
				element.SaveXML(buffer);
		}
#if defined EXTREME_XML
		void XMLManager::XMLElement::SaveValue(
			wstringstream& buffer
		) const
		{
			//wstring value = m_value;
			//QuotedPrintable(value, g_valuePairs);
			buffer < << m_value;
		}
#endif
		XMLManager::XMLElement::XMLElement(
			const wstring& elementName,
			InitializerListAttrs attrs,
			VectorNestedElements nestedElements
		) :
			m_elementName{ elementName },
			m_attrs{ attrs },
			m_nestedElements{ nestedElements }
		{}

		XMLManager::XMLElement::XMLElement(
			IXmlReader* reader
		)
		{
			const wchar_t* pwszValue = nullptr;
			uint32_t countOfValue = 0;
			THROW_EXC_IFFAILED_DB(
				reader->GetLocalName(
					&pwszValue,
					&countOfValue),
				L"Get Element");
			m_elementName = { pwszValue, countOfValue };
#if defined MONAD_DUMP_XML
			OutputDebugString(L"<");
			OutputDebugString(pwszValue);
#endif
			const auto bRet = reader->IsEmptyElement();

#pragma region MapAttrs
			const wchar_t* pwszLocalName = nullptr;
			uint32_t countOfLocalName = 0;
			const HRESULT hr = reader->MoveToFirstAttribute();

			if (S_OK == hr)
				do
				{
					if (!reader->IsDefault())
					{
						THROW_EXC_IFFAILED_DB(reader->GetLocalName(&pwszLocalName, &countOfLocalName), L"Read Attr's Name");
						THROW_EXC_IFFAILED_DB(reader->GetValue(&pwszValue, &countOfValue), L"Read Attr's Value #2");

						m_attrs.emplace(
							piecewise_construct,
							forward_as_tuple(pwszLocalName, countOfLocalName),
							forward_as_tuple(pwszValue, countOfValue));
					}
				} while (S_OK == reader->MoveToNextAttribute());
			else if (S_FALSE != hr)
				THROW_EXC(Exceptions::InvalidXML, hr, L"Get First Attribute");
#pragma endregion

			if (bRet)
			{
#if defined MONAD_DUMP_XML
				OutputDebugString(L"/>");
#endif
				return;
			}

			XmlNodeType nodeType{};

			while (!reader->IsEOF())
			{
				THROW_EXC_IFFAILED_DB(reader->Read(&nodeType), L"Read Node Type");
				switch (nodeType)
				{
				case XmlNodeType_EndElement:
				{
#if defined MONAD_DUMP_XML
					OutputDebugString(m_elementName.c_str());
					OutputDebugString(L">");
#endif
					return;
				}
#if defined EXTREME_XML
				case XmlNodeType_Text:
				case XmlNodeType_Whitespace:
				case XmlNodeType_CDATA:
					THROW_EXC_IFFAILED_DB(reader->GetValue(&pwszValue, NULL), L"Get Value");
					m_value += pwszValue;
#if defined MONAD_DUMP_XML
					OutputDebugString(m_value.c_str());
#endif
					break;
#endif
				case XmlNodeType_Element:
					m_nestedElements.emplace_back(reader);
					break;
				}
			}
		}

		const wstring& XMLManager::XMLElement::GetElementName() const noexcept
		{
			return m_elementName;
		}

#if defined EXTREME_XML
		const wstring& XMLManager::XMLElement::GetValue() const noexcept
		{
			return m_value;
		}
#endif

		const wstring& XMLManager::XMLElement::GetAttr(
			const wstring& name
		) const
		{
			const auto itSearch = m_attrs.find(name);
			THROW_EXC_ONEND(m_attrs, itSearch, L"Attribute");
			return itSearch->second;
		}

		float XMLManager::XMLElement::GetFloatAttr(
			const wstring& name
		) const
		{
			return stof(GetAttr(name));
		}

		unsigned long long XMLManager::XMLElement::GetULongLongAttr(
			const wstring& name
		) const
		{
			return stoull(GetAttr(name));
		}

		wchar_t XMLManager::XMLElement::GetCharAttr(
			const wstring& name
		) const
		{
			return static_cast<wchar_t>(stoul(GetAttr(name)));
		}

		bool XMLManager::XMLElement::GetBoolAttr(
			const wstring& name
		) const
		{
			return GetAttr(name) == L"1";
		}

		const wstring& XMLManager::XMLElement::GetNameAttr() const
		{
			return GetAttr(XML_ATTR_NAME);
		}

		const wstring& XMLManager::XMLElement::GetIDAttr() const
		{
			return GetAttr(XML_ATTR_ID);
		}
		/*void XMLManager::XMLElement::SetValue(const wstring& value)
		{
#if defined MONAD_DUMP_XML
			OutputDebugString(m_value.c_str());
#endif
			m_value = value;
		}*/
#pragma endregion

		static_assert(std::is_nothrow_move_constructible<XMLManager::XMLElement>::value, "Move Ctor.");
		static_assert(std::is_nothrow_move_assignable<XMLManager::XMLElement>::value, "Move Assign.");
		static_assert(std::is_copy_constructible<XMLManager::XMLElement>::value, "copy Ctor.");
		static_assert(std::is_copy_assignable<XMLManager::XMLElement>::value, "copy Assign.");
	}
}