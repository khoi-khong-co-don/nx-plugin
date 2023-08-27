#include "Rece_data_socket.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <map>
#include <chrono>

#define UNKNOWN "Unknown"
#define TRAFFIC_PLATE_PLUGIN 0
#define FACE_DETECTION_PLUGIN 1

namespace nx {
    namespace vms_server_plugins {
        namespace analytics {
            namespace stub {
                void DataSocket::dataSocket(int ai_module, std::string dataSocket, std::string User)
                {

                    switch (ai_module)
                    {

                    case TRAFFIC_PLATE_PLUGIN:
                    {
                        LOG("AI MODULE TRAFFIC");

                        std::string err;
                        const auto msgJson = json11::Json::parse(dataSocket, err);
                        LOG("TRAFFIC 0");
                        std::string traffic = "";
                        std::string color = "";
                        std::string vehicle = "";
                        std::string lane = "";
                        std::string speed = "";
                        std::string base64 = "";
                        std::string user = "";
                        LOG("TRAFFIC 1");
                        traffic = (!msgJson["traffic"].string_value().empty() ? msgJson["traffic"].string_value() : UNKNOWN);
                        color = (!msgJson["color"].string_value().empty() ? msgJson["color"].string_value() : UNKNOWN);
                        vehicle = (!msgJson["vehicle"].string_value().empty() ? msgJson["vehicle"].string_value() : UNKNOWN);
                        lane = (!std::to_string(msgJson["lane"].int_value()).empty() ? std::to_string(msgJson["lane"].int_value()) : UNKNOWN);
                        speed = (!std::to_string(msgJson["speed"].int_value()).empty() ? std::to_string(msgJson["speed"].int_value()) : UNKNOWN);

                        base64 = msgJson["base64"].string_value();
                        user = msgJson["address"].string_value();
                        LOG("TRAFFIC 2");
                        if (user != "")
                        {
                            LOG("TRAFFIC 3");
                            const char* user_str = user.c_str();
                            std::uint64_t address_value = std::strtoull(user_str, nullptr, 16);
                            ICamera* oryza_cam_traffic = reinterpret_cast<ICamera*>(address_value);
                            LOG("TRAFFIC 4");
                            std::vector<std::string> data = { traffic , color , vehicle , lane , speed };
                            //std::vector<std::string> data = { name , birthday , city , country };

                            oryza_cam_traffic->setTrafficInfo(data);
                            LOG("TRAFFIC 5");
                            std::string base64String = base64; // Replace with your Base64 string

                            std::vector<unsigned char> bytes = this->base64_decode_to_bytes(base64String);
                            LOG("TRAFFIC 6");
                            DWORD result_dword = this->get_buffer_size(base64String);

                             this->capture(bytes.data(), result_dword, address_value);
                             LOG("TRAFFIC 7");
                             oryza_cam_traffic->setCallBack(TRUE);
                             LOG("TRAFFIC 8");
                             oryza_cam_traffic->setIsRecognized(TRUE);
                             LOG("TRAFFIC 9");

                        }
                        break;
                    }

                    case FACE_DETECTION_PLUGIN:
                    {
                        LOG("AI MODULE FACE");
                        std::string err;
                        const auto msgJson = json11::Json::parse(dataSocket, err);

                        std::string name = "";
                        std::string birthday = "";
                        std::string city = "";
                        std::string country = "";
                        std::string base64 = "";
                        std::string user = "";

                        name = (!msgJson["name"].string_value().empty() ? msgJson["name"].string_value() : UNKNOWN);
                        birthday = (!msgJson["birthday"].string_value().empty() ? msgJson["birthday"].string_value() : UNKNOWN);
                        city = (!msgJson["city"].string_value().empty() ? msgJson["city"].string_value() : UNKNOWN);
                        country = (!msgJson["country"].string_value().empty() ? msgJson["country"].string_value() : UNKNOWN);
                        base64 = msgJson["base64"].string_value();
                        user = msgJson["address"].string_value();

                        if (user != "")
                        {
                            const char* user_str = user.c_str();
                            std::uint64_t address_value = std::strtoull(user_str, nullptr, 16);

                            //FaceDetection::Oryza::OryzaCamera* oryza_cam = reinterpret_cast<FaceDetection::Oryza::OryzaCamera*>(address_value);
                            ICamera* oryza_cam = reinterpret_cast<ICamera*>(address_value);

                            std::vector<std::string> data = { name , birthday , city , country };

                            oryza_cam->setRecognitionDataInfo(data);

                            std::string base64String = base64; // Replace with your Base64 string

                            std::vector<unsigned char> bytes = this->base64_decode_to_bytes(base64String);

                            DWORD result_dword = this->get_buffer_size(base64String);

                            this->capture(bytes.data(), result_dword, address_value);

                            oryza_cam->setCallBack(TRUE);
                            oryza_cam->setIsRecognized(TRUE);
                        }
                        break;
                    }
                    }
                }

                //----------------------------------------------------------------------------
                // Base64 to byte
                //---------------------------------------------------------------------------- 
                std::vector<unsigned char> DataSocket::base64_decode_to_bytes(const std::string& base64_string) {
                    typedef boost::archive::iterators::transform_width<
                        boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> base64_dec;

                    // Calculate the size of the decoded binary data
                    size_t padding = std::count(base64_string.begin(), base64_string.end(), '=');
                    size_t decoded_size = (base64_string.size() * 6) / 8 - padding;

                    // Resize the output vector to the proper size
                    std::vector<unsigned char> decoded_data;
                    decoded_data.reserve(decoded_size);

                    try {
                        std::copy(base64_dec(base64_string.begin()), base64_dec(base64_string.end()), std::back_inserter(decoded_data));
                    }
                    catch (std::exception& e) {
                        // Catch any exceptions during decoding (invalid Base64 string)
                    }

                    return decoded_data;
                };


                //----------------------------------------------------------------------------
                // Base64 to dword
                //---------------------------------------------------------------------------- 
                DWORD DataSocket::get_buffer_size(const std::string& base64_string) {
                    size_t padding = std::count(base64_string.begin(), base64_string.end(), '=');
                    return static_cast<DWORD>((base64_string.size() * 6) / 8 - padding);
                };


                void DataSocket::capture(BYTE* pBuffer, DWORD dwBufSize, LDWORD User) {
                    std::string strStoreImgPath;
                    int lastSlashPos;
                    std::string folderImage;
                    LOG("CREATE IMAGE!!!!!!!!!");
#ifdef WIN32
                    HMODULE hModule = NULL;

                    hModule = ::GetModuleHandle("detection_analytics_plugin.dll");
                    LOG("IMAGE1");
                    char szFullDllPath[_MAX_PATH] = { 0 };
                    char szModuleDir[_MAX_PATH] = { 0 };

                    ::GetModuleFileName(hModule, szFullDllPath, _MAX_PATH);
                    char* lastBackslash = strrchr(szFullDllPath, '\\');
                    size_t lastBackslashPos = lastBackslash - szFullDllPath + 1;
                    strncpy(szModuleDir, szFullDllPath, lastBackslashPos);
                    strStoreImgPath = std::string(szModuleDir);
                    strStoreImgPath.append("StoreImageFace\\");
                    LOG(strStoreImgPath);
#else
                    char buffer[4096];
                    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
                        std::string currentDirectory(buffer);
                        memset(buffer, 0, sizeof(buffer));
                        strStoreImgPath = currentDirectory.append("/StoreImageFace/");
                    }
#endif
                    LOG("IMAGE4");
                    auto currentTime = std::chrono::system_clock::now();
                    auto timePoint = std::chrono::system_clock::to_time_t(currentTime);
                    auto timeInfo = std::localtime(&timePoint);

                    int day = timeInfo->tm_mday;
                    int month = timeInfo->tm_mon + 1;
                    int year = timeInfo->tm_year + 1900;

                    int hours = timeInfo->tm_hour;
                    int minutes = timeInfo->tm_min;
                    int seconds = timeInfo->tm_sec;
                    auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                        currentTime.time_since_epoch()
                    ).count();
                    miliseconds %= 1000;

                    std::string currentTimeStr = std::to_string(year) + std::to_string(month) + std::to_string(day) + std::to_string(hours) + std::to_string(minutes) + std::to_string(seconds) + std::to_string(miliseconds);
                    std::string strJpgFile = strStoreImgPath + std::string(currentTimeStr) + std::string(".jpg");
                    LOG("IMAGE5");
                    reinterpret_cast<ICamera*>(User)->setImgpath(strJpgFile);
                    LOG("IMAGE6");
                    LOG(strJpgFile);
                    FILE* fp = fopen(strJpgFile.c_str(), "wb");

                    if (fp == NULL)
                    {
                        Logger::instance()->writeLogServer(__func__, "Save picture failed!");
                        return;
                    }
                    LOG("IMAGE7");
                    fwrite(pBuffer, dwBufSize, 1, fp);
                    fclose(fp);
                    Logger::instance()->writeLogServer(__func__, "create image success : " + strJpgFile);
                }


            }
        }
    }
}