#include "oryza_SDK_connection.h"
#include "oryza_camera.h"
#include "oryza_event_controller.h"
#include "WorkerThread.h"


namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {
					OryzaSDKConnection::OryzaSDKConnection() {
					workerthread = new WorkerThread();
					socketThread = new WorkerThread();
						//workerthread(new WorkerThread());
					}

					OryzaSDKConnection::~OryzaSDKConnection() {
						CLIENT_Cleanup();
					}

					void OryzaSDKConnection::login(LDWORD dwUser = NULL) {
						OryzaCamera* oryza_cam = reinterpret_cast<OryzaCamera*>(dwUser);

						LOG(oryza_cam->host());
						LOG(oryza_cam->port());
						LOG(oryza_cam->username());
						LOG(oryza_cam->password());
						LOG(oryza_cam->deviceId());
						std::stringstream ss_this;
						ss_this << oryza_cam;
						std::string Address_this = ss_this.str();
						LOG("ID thread device Agent: -- " + Address_this);
						std::string cmd;
						if (oryza_cam->port() == "")
						{ 
							json11::Json my_json = json11::Json::object{
							{ "ip", oryza_cam->host()},
							{ "username", oryza_cam->username() },
							{ "pass",  oryza_cam->password() },
							{ "status", 1 },
							{ "id_device", oryza_cam->deviceId()},
							{ "ai_module", 1 },
							{ "address", Address_this}
								};
							cmd = my_json.dump();
						}
						else
						{
							json11::Json my_json = json11::Json::object{ 
							{ "ip", oryza_cam->host()},
							{ "port", oryza_cam->port() },
							{ "username", oryza_cam->username() },
							{ "pass",  oryza_cam->password() },
							{ "status", 1 },
							{ "id_device", oryza_cam->deviceId()},
							{ "ai_module", 1 },
							{ "address", Address_this}
								};
							cmd = my_json.dump();
						}
						

						//Pust RTSP to Websocket
						// workerthread->CreateThread();
						// std::shared_ptr<UserData> sendData(new UserData());
						// sendData->msg = cmd;
						// workerthread->PostMsg(sendData);
						// workerthread->ExitThread();
					}

					void OryzaSDKConnection::logout(LDWORD dwUser = NULL) {
						m_lLoginID = 0;
					}

					void OryzaSDKConnection::subscribe(LDWORD dwUser = NULL) {
						if (!m_lLoginID) return;
						LOG("OPEN SOCKET");
						socketThread->CreateThread();
						socketThread->CreateSocket();
						if (socketThread->socketExiss())
						{
							LOG("SOCKET IS LIVE - CLOSE THREAD");
							//socketThread->ExitThread();
							delete socketThread;
							LOG("SOCKET IS LIVE - CLOSE THREAD SUCCESS");
						}
					}

					void OryzaSDKConnection::unsubscribe(LDWORD dwUser = NULL) {
						OryzaCamera* oryza_cam_close = reinterpret_cast<OryzaCamera*>(dwUser);
						
						LOG(oryza_cam_close->host());
						LOG(oryza_cam_close->port());
						LOG(oryza_cam_close->username());
						LOG(oryza_cam_close->password());
						LOG(oryza_cam_close->deviceId());
						std::stringstream ss_this;
						ss_this << oryza_cam_close;
						std::string Address_this = ss_this.str();
						LOG("ID thread device Agent: -- " + Address_this);
						std::string cmd_close;
						if (oryza_cam_close->port() == "")
						{
							json11::Json my_json_close = json11::Json::object{
							{ "ip", oryza_cam_close->host()},
							{ "username", oryza_cam_close->username() },
							{ "pass",  oryza_cam_close->password() },
							{ "status", 0 },
							{ "id_device", oryza_cam_close->deviceId()},
							{ "ai_module", 1 },
							{ "address", Address_this}
							};
							cmd_close = my_json_close.dump();
						}
						else
						{
							json11::Json my_json_close = json11::Json::object{
							{ "ip", oryza_cam_close->host()},
							{ "port", oryza_cam_close->port() },
							{ "username", oryza_cam_close->username() },
							{ "pass",  oryza_cam_close->password() },
							{ "status", 0 },
							{ "id_device", oryza_cam_close->deviceId()},
							{ "ai_module", 1 },
							{ "address", Address_this}
							};
							cmd_close = my_json_close.dump();
						}
						
						LOG("CLOSE WEB");
						// workerthread->CreateThread();
						// std::shared_ptr<UserData> sendData_close(new UserData());
						// sendData_close->msg = cmd_close;
						// workerthread->PostMsg(sendData_close);
						// workerthread->ExitThread();

						m_lRealPicHandle = 0;
						
						delete workerthread;
						//delete socketThread;
						LOG("CLOSE THREAD SUCCESS");

					}

					bool OryzaSDKConnection::loginSuccess()
					{
						return m_lLoginID != 0;
					}

					bool OryzaSDKConnection::subscribeSuccess()
					{
						return m_lRealPicHandle != 0;
					}
				};

			} // namespace sample
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx