#pragma once
#include <string>

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				const std::string kUsernameSetting = "username";

				const std::string kPasswordSetting = "password";

				const std::string kLoginSettingsModel =
					/*suppress newline*/ 1 + (const char*)R"json("
{
    "type": "Settings",
        "items":
        [
            
            {
                "type": "GroupBox",
                "caption": "Login",
                "items":
                [
                    {
                        "type": "TextField",
                        "name": ")json" + kUsernameSetting + R"json(",
                        "caption": "Username",
                        "defaultValue": "admin"
					
                    },
					{
                        "type": "PasswordField",
                        "name": ")json" + kPasswordSetting + R"json(",
                        "caption": "Password",
                        "defaultValue": "Oryza@123"
                    }
                ]
            }
        ]
})json";

				const std::string kLoginSuccessSettingsModel =
					/*suppress newline*/ 1 + (const char*)R"json("
{
    
})json";
			} // namespace stub
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx