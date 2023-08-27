// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "utils.h"
#include "Logger.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include <regex>
#include <dhnetsdk.h>
#include "face_detection_renovation/i_event_controller.h"

#ifdef WIN32
#pragma comment(lib, "dhnetsdk.lib")
#elif __linux__
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				using namespace nx::vms_server_plugins::analytics::stub::FaceDetection;
				using namespace nx::sdk;

				bool toBool(std::string str)
				{
					std::transform(str.begin(), str.begin(), str.end(), ::tolower);
					return str == "true" || str == "1";
				}

				bool startsWith(const std::string& str, const std::string& prefix)
				{
					return str.rfind(prefix, 0) == 0;
				}

				std::vector<char> loadFile(const std::string& path)
				{
					std::ifstream file(path, std::ios::binary);
					if (!file.is_open())
						return {};

					return std::vector<char>(std::istreambuf_iterator<char>(file), {});
				}

				std::string imageFormatFromPath(const std::string& path)
				{
					auto endsWith =
						[](const std::string& str, const std::string& suffix)
					{
						if (suffix.size() > str.size())
							return false;

						return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
					};

					if (endsWith(path, ".jpg") || endsWith(path, ".jpeg"))
						return "image/jpeg";

					if (endsWith(path, ".png"))
						return "image/png";

					if (endsWith(path, ".tiff"))
						return "image/tiff";

					return "";
				}

				bool isHttpOrHttpsUrl(const std::string& path)
				{
					auto startsWith =
						[](const std::string& str, const std::string& prefix)
					{
						if (prefix.size() > str.size())
							return false;

						return std::equal(prefix.begin(), prefix.end(), str.begin());
					};

					return startsWith(path, "http://") || startsWith(path, "https://");
				}

				std::string join(
					const std::vector<std::string>& strings,
					const std::string& delimiter,
					const std::string& itemPrefix,
					const std::string& itemPostfix)
				{
					std::string result;
					for (int i = 0; i < strings.size(); ++i)
					{
						result += itemPrefix + strings[i] + itemPostfix;
						if (i != strings.size() - 1)
							result += delimiter;
					}

					return result;
				}

				std::map<std::string, std::string> toStdMap(const Ptr<const IStringMap>& sdkMap)
				{
					std::map<std::string, std::string> result;
					if (!sdkMap)
						return result;

					for (int i = 0; i < sdkMap->count(); ++i)
						result[sdkMap->key(i)] = sdkMap->value(i);

					return result;
				}

				std::string extractHostFromURL(const char* url) {
					std::regex hostRegex("(http|https|rtsp)://([^:/]+)(:\\d+)?");
					std::cmatch match;

					if (std::regex_search(url, match, hostRegex)) {
						return std::string(match[2].first, match[2].second);
					}
					else {
						return "";
					}
				}

				int extractPortFromURL(const char* url) {
					std::regex portRegex(R"((http|https|rtsp)://[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+:([0-9]+)/)");
					std::cmatch match;
					int res = 80;
					if (std::regex_search(url, match, portRegex)) {
						res = stoi(std::string(match[1].first, match[1].second));
					}
					
					return res;
				}

				std::string extractPortFromURL_string(const char* url) {
					std::regex portRegex(R"(http://[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+:([0-9]+)/)");
					std::cmatch match;

					if (regex_search(url, match, portRegex)) {
						return std::string(match[1].first, match[1].second);
					}
					else {
						return "80";
					}
				}

				void createImageStorage(std::string name) {
					std::string strStoreImgPath;

				#ifdef WIN32
					HMODULE hModule = NULL;

					hModule = ::GetModuleHandle("detection_analytics_plugin.dll");

					char szFullDllPath[_MAX_PATH] = { 0 };
					char szModuleDir[_MAX_PATH] = { 0 };

					::GetModuleFileName(hModule, szFullDllPath, _MAX_PATH);
					char* lastBackslash = strrchr(szFullDllPath, '\\');
					size_t lastBackslashPos = lastBackslash - szFullDllPath + 1;
					strncpy(szModuleDir, szFullDllPath, lastBackslashPos);

					strStoreImgPath = std::string(szModuleDir);
					strStoreImgPath.append(name);
					CreateDirectory((strStoreImgPath).c_str(), NULL);
				#else
					IEventController* f_path; 
					f_path = new IEventController();
					
					void* symbolAddr = reinterpret_cast<void*>(&createImageStorage); // Example symbol (you can use any symbol from your library)
					Dl_info info;

					if (dladdr(symbolAddr, &info)) {
					} 
					else {
						LOG("Address is error");
					}
					strStoreImgPath = std::string(info.dli_fname);

					// Find the position of the second-to-last occurrence of '/'
    				size_t secondToLastSlashPos = strStoreImgPath.find_last_of('/', strStoreImgPath.find_last_of('/') - 1);
					
					if (secondToLastSlashPos != std::string::npos) {
 						strStoreImgPath = strStoreImgPath.substr(0, secondToLastSlashPos);
						LOG(strStoreImgPath);
					}
					
					std::string folder_image = strStoreImgPath.append(name);

					if (mkdir(folder_image.c_str(), 0777) == 0) {
						LOG("Create Folder");
					} 
					else 
					{
						LOG("False create folder");
					}

					delete f_path;
				#endif
				}
			} // namespace stub
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx
