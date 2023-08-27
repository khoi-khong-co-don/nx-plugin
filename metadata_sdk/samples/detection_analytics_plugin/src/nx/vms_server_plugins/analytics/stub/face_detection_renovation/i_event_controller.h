#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <ctime>
#include "define_var.h"


namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace FaceDetection {
					class IEventController
					{
					public:
						explicit IEventController();
						 ~IEventController();
					
					public:
						bool callback();

						void setCallback(bool newCallback);

						bool isRecognized();

						void setIsRecognized(bool isRecognized);

						std::vector<std::string> faceDataInfo();

						void setFaceDataInfo(std::vector<std::string> faceDataInfo);

						std::vector<std::string> recognitionDataInfo();

						void setRecognitionDataInfo(std::vector<std::string> recognitionDataInfo);

						std::string username();

						void setUsername(std::string username);

						std::string password();

						void setPassword(std::string passowrd);

						std::string host();

						void setHost(std::string host);

						std::string imgpath();

						void setImgpath(std::string imgpath);

						std::string port();

						void setPort(std::string port);

						std::string deviceId();

						void setdeviceId(std::string deviceId);

					private:
						bool m_callback = 0;
						bool m_isRecognized = 0;
						std::string m_port;
						std::vector<std::string> m_facedataInfo;
						std::vector<std::string> m_recognitionDataInfo;
						std::string m_username;
						std::string m_password;
						std::string m_host;
						std::string m_imgpath;
						std::string m_deviceId;
					};
				} // namespace sample
			}
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx
