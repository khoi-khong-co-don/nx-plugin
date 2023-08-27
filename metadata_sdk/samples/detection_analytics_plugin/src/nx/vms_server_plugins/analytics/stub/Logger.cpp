#include "Logger.h"

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
					Logger* Logger::m_instance = nullptr;
					Logger::Logger()
					{
					}

					Logger::~Logger()
					{
					}

					void Logger::writeLogServer(std::string func, std::string logText)
					{
						std::ofstream file;
						time_t now = time(0);
						std::string dt = ctime(&now);
						// convert now to string form

						file.open("test_plugin.log", std::ios_base::app);

						file << dt + "-" + func + ":" + logText;
						file << "\n";
					}

					Logger* Logger::instance()
					{
						if (m_instance == nullptr) {
							m_instance = new Logger();
						}
						return m_instance;
					}

				} // namespace sample
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx