#pragma once
#include <iostream>
#include <unordered_map>
#include <dhnetsdk.h>
#ifdef _WIN64
#pragma comment(lib, "dhnetsdk.lib")
#endif

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {
					
					const std::string kUsernameSetting = "username";

					const std::string kPasswordSetting = "password";

					enum recognitionDataEnum {
						e_Plate,
						e_Color,
						e_Vehicle,
						e_Lane,
						e_Speed
					};

				}
			}
		}
	}
}