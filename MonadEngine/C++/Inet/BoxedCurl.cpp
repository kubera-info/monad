// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "BoxedCurl.h"

// STL / system
#include <ranges>

// Monad
#include "System/Library.h"
#include "Tools/Convert.h"
#include "Exceptions/Exceptions.h"
#include "System/System.h"

using namespace std;

namespace Monad::Internet
{
#pragma region CurlINet::LocalLibraryCurl

	/// <summary>
	/// Runtime-loaded wrapper over libcurl.
	/// This class resolves required libcurl symbols dynamically
	/// to avoid static dependency and allow late binding.
	/// </summary>
	struct CurlINet::LocalLibraryCurl : System::LocalLibrary
	{
		LocalLibraryCurl() :
			LocalLibrary(
#if defined(_M_ARM64)
				L"libcurl-arm64.dll"
#else
				L"libcurl-x64.dll"
#endif
			)
		{}

#define DLL_TO_APP(FUN_NAME,RES,ARGS)\
		using LP##FUN_NAME = RES(WINAPI*)ARGS;\
		const LP##FUN_NAME c_fn_##FUN_NAME = reinterpret_cast<LP##FUN_NAME>(GetProcAddress(#FUN_NAME));

		// Core libcurl entry points
		DLL_TO_APP(curl_global_init, CURLcode, (long flags));
		DLL_TO_APP(curl_global_cleanup, void, (void));
		DLL_TO_APP(curl_easy_init, CURL*, (void));
		DLL_TO_APP(curl_easy_setopt, CURLcode, (CURL* curl, CURLoption option, ...));
		DLL_TO_APP(curl_easy_perform, CURLcode, (CURL* curl));
		DLL_TO_APP(curl_easy_cleanup, void, (CURL* curl));
		DLL_TO_APP(curl_slist_free_all, void, (struct curl_slist* list));
		DLL_TO_APP(curl_slist_append, struct curl_slist*, (struct curl_slist* list, const char* data));
		DLL_TO_APP(curl_easy_escape, char*, (CURL* handle, const char* string, int length));
		DLL_TO_APP(curl_free, void, (void* p));
		DLL_TO_APP(curl_easy_getinfo, CURLcode, (CURL* curl, CURLINFO info, ...));
		DLL_TO_APP(curl_version_info, curl_version_info_data*, (CURLversion));
	};
#pragma endregion

	namespace
	{
		/// <summary>
		/// HTTP response wrapper used internally.
		/// Stores both CURL status and HTTP status code.
		/// </summary>
		struct Responce final
		{
			operator bool() const noexcept;

			CURLcode   m_curlCode = CURLE_OK;
			long       m_httpCode = 0;
			std::string m_responseText;
		};

		/// <summary>
		/// CURL write callback – appends incoming bytes to std::string.
		/// </summary>
		size_t CallbackWriteToString(
			void* contents,
			[[maybe_unused]] size_t size,
			size_t nmemb,
			string* s
		) noexcept
		{
			assert(contents);
			assert(size == 1);

			try
			{
				s->insert(s->size(), static_cast<const char*>(contents), nmemb);
			}
			catch (bad_alloc&)
			{
				return 0;
			}
			return nmemb;
		}

#pragma region Easy

		/// <summary>
		/// RAII wrapper over CURL easy handle.
		/// Handles initialization, configuration, execution and cleanup.
		/// </summary>
		struct Easy final
		{
			Easy(const CurlINet::LocalLibraryCurl& libCurl)
				: c_libCurl{ libCurl },
				m_curl{ c_libCurl.c_fn_curl_easy_init() }
			{
				THROW_EXC_IFFALSE(Exceptions::NotInitialized, m_curl, L"Easy Init");
			}

			~Easy()
			{
				c_libCurl.c_fn_curl_easy_cleanup(m_curl);
			}
			OPER_DEL_NO_DEF_CTOR(Easy);

			/// <summary>
			/// Generic wrapper for curl_easy_setopt
			/// </summary>
			template<typename A>
			void SetOpt(CURLoption option, A arg)
			{
				THROW_EXC_IFFALSE(
					Exceptions::NotInitialized,
					CURLE_OK == c_libCurl.c_fn_curl_easy_setopt(m_curl, option, arg),
					L"Easy"
				);
			}

			void SetOpt(CURLoption option, const wstring& arg)
			{
				SetOpt(option, Tools::UnicodeToUTF8(arg));
			}

			void SetOpt(CURLoption option, const string& arg)
			{
				SetOpt(option, arg.data());
			}

			/// <summary>
			/// Performs HTTP request with configured parameters.
			/// </summary>
			Responce SetOptsAndCallFunction(
				const string& functionName,
				CurlINet::InitializerListQueryParameters parameters
			)
			{
				Responce apiCallResponce;

				// Standard HTTP options
				SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
				SetOpt(CURLOPT_MAXREDIRS, 2L);
				SetOpt(CURLOPT_TIMEOUT, 5L);
				curl_version_info_data* ver = c_libCurl.c_fn_curl_version_info(CURLVERSION_NOW);
				const auto verStr =
					to_wstring((ver->version_num >> 16) & 0xff)
					+ L'.'
					+ to_wstring((ver->version_num >> 8) & 0xff)
					+ L'.'
					+ to_wstring(ver->version_num & 0xff);

				SetOpt(CURLOPT_USERAGENT,
					L"Monad(a) Engine, libCURL("s
					+ verStr
					+ L"), Reporting tool("s
					+ System::GetAppVersionAndArch()
					+ L")");
				SetOpt(CURLOPT_WRITEFUNCTION, CallbackWriteToString);
				SetOpt(CURLOPT_SSL_OPTIONS, static_cast<long>(CURLSSLOPT_NO_REVOKE));
				SetOpt(CURLOPT_HTTP_VERSION, static_cast<long>(CURL_HTTP_VERSION_3));

				// HTTP headers
				auto monadLocale = I18N::GetLocaleName();
				struct SList final
				{
					using PairHTTPHeader = pair<string, string>;

					SList(const CurlINet::LocalLibraryCurl& libCurl,
						initializer_list<PairHTTPHeader> headers)
						: c_libCurl(libCurl)
					{
						for (const auto& header : headers)
							AppendHeader(header);
					}

					~SList()
					{
						c_libCurl.c_fn_curl_slist_free_all(m_chunk);
					}

					void AppendHeader(const PairHTTPHeader& header)
					{
						const auto old_chunk = m_chunk;
						m_chunk = c_libCurl.c_fn_curl_slist_append(
							m_chunk,
							(header.first + ": " + header.second).data());

						if (!m_chunk)
						{
							if (old_chunk)
								c_libCurl.c_fn_curl_slist_free_all(old_chunk);
							THROW_EXC_IFNULL(Exceptions::NullValue, m_chunk, L"Append");
						}
					}

					struct curl_slist* m_chunk = nullptr;
					const CurlINet::LocalLibraryCurl& c_libCurl;
				} chunk{
					c_libCurl,
					{
						{ "Accept-Language"s, monadLocale },
						{ "Charset"s, "utf-8"s }
					}
				};

				SetOpt(CURLOPT_HTTPHEADER, chunk.m_chunk);
				SetOpt(CURLOPT_URL, Parse(functionName, parameters));
				SetOpt(CURLOPT_WRITEDATA, &apiCallResponce.m_responseText);
				SetOpt(CURLOPT_SSL_VERIFYPEER, FALSE);

				apiCallResponce.m_curlCode = Perform();
				THROW_EXC_IFFALSE(Exceptions::NullValue, apiCallResponce, L"Easy Resp");

				GetInfo(apiCallResponce);
				return apiCallResponce;
			}

			/// <summary>
			/// URL-encodes a string using curl escaping.
			/// </summary>
			string Escape(const string& value) const
			{
				struct OutputStr
				{
					OutputStr(const CurlINet::LocalLibraryCurl& libCurl, char* str) noexcept
						: c_libCurl(libCurl), m_str(str) {}
					~OutputStr() { c_libCurl.c_fn_curl_free(m_str); }

					const CurlINet::LocalLibraryCurl& c_libCurl;
					char* m_str = nullptr;
				};

				return OutputStr{
					c_libCurl,
					c_libCurl.c_fn_curl_easy_escape(m_curl, value.c_str(), (int)value.length())
				}.m_str;
			}

			inline CURLcode Perform() noexcept
			{
				return c_libCurl.c_fn_curl_easy_perform(m_curl);
			}

			inline void GetInfo(Responce& httpCode) const noexcept
			{
				httpCode.m_curlCode =
					c_libCurl.c_fn_curl_easy_getinfo(
						m_curl,
						CURLINFO_RESPONSE_CODE,
						&httpCode.m_httpCode);
			}

			/// <summary>
			/// Builds URL with query parameters.
			/// </summary>
			string Parse(
				const string& functionName,
				CurlINet::InitializerListQueryParameters parameters
			)
			{
				string urlBuilder;

				for (const auto& parameter :
					parameters | views::filter([](const auto& p) noexcept
						{ return !p.c_value.empty(); }))
				{
					urlBuilder +=
						(urlBuilder.empty() ? "/?"s : "&"s) +
						parameter.c_name + '=' +
						Escape(Tools::UnicodeToUTF8(parameter.c_value));
				}

				return functionName + urlBuilder;
			}

		private:
			const CurlINet::LocalLibraryCurl& c_libCurl;
			CURL* const m_curl;
		};
#pragma endregion
	}

#pragma region CurlINet

	CurlINet::ExpectedResponce CurlINet::Get(
		const string& apiURL,
		const string& functionName,
		InitializerListQueryParameters parameters
	) const
	{
		auto apiCallResponce =
			Easy(*c_libCurl).SetOptsAndCallFunction(apiURL + functionName, parameters);

		if (apiCallResponce && apiCallResponce.m_httpCode == 200)
			return apiCallResponce.m_responseText;
		else
			return std::unexpected(apiCallResponce.m_httpCode);
	}

	CurlINet::CurlINet()
		: c_libCurl(make_unique<LocalLibraryCurl>())
	{
		THROW_EXC_IFFALSE(
			Exceptions::NotInitialized,
			CURLE_OK == c_libCurl->c_fn_curl_global_init(CURL_GLOBAL_DEFAULT),
			L"Global");
	}

	CurlINet::~CurlINet()
	{
		c_libCurl->c_fn_curl_global_cleanup();
	}
#pragma endregion

#pragma region CurlINet::Responce

	Responce::operator bool() const noexcept
	{
		return CURLE_OK == m_curlCode;
	}

#pragma endregion
}