#include "i_event_controller.h"

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace FaceDetection {

					IEventController::IEventController() {

					}

					IEventController::~IEventController() {

					}

					bool IEventController::callback()
					{
						return m_callback;
					}

					void IEventController::setCallback(bool callback)
					{
						m_callback = callback;
					}

					bool IEventController::isRecognized()
					{
						return m_isRecognized;
					}

					void IEventController::setIsRecognized(bool isRecognized)
					{
						m_isRecognized = isRecognized;
					}

					void IEventController::setFaceDataInfo(std::vector<std::string> faceDataInfo)
					{
						m_facedataInfo = faceDataInfo;
					}

					std::vector<std::string> IEventController::faceDataInfo()
					{
						return m_facedataInfo;
					}

					void IEventController::setRecognitionDataInfo(std::vector<std::string> recognitionDataInfo)
					{
						m_recognitionDataInfo = recognitionDataInfo;
					}

					std::vector<std::string> IEventController::recognitionDataInfo()
					{
						return m_recognitionDataInfo;
					}

					std::string IEventController::username() {
						return m_username;
					}

					void IEventController::setUsername(std::string username) {
						m_username = username;
					}

					std::string IEventController::password() {
						return m_password;
					}

					void IEventController::setPassword(std::string password) {
						m_password = password;
					}

					std::string IEventController::host() {
						return m_host;
					}

					void IEventController::setHost(std::string host) {
						m_host = host;
					}

					std::string IEventController::imgpath() {
						return m_imgpath;
					}

					void IEventController::setImgpath(std::string imgpath) {
						m_imgpath = imgpath;
					}

					std::string IEventController::port() {
						return m_port;
					}

					void IEventController::setPort(std::string port) {
						m_port = port;
					}

					std::string IEventController::deviceId() {
						return m_deviceId;
					}

					void IEventController::setdeviceId(const std::string deviceId) {
						m_deviceId = deviceId;
					}

				} // namespace sample
			}
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx