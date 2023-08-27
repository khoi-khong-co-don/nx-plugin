#include "WorkerThread.h"
#include <ctime>
#include <chrono>
#include "oryza_camera.h"
#define MSG_EXIT_THREAD			1
#define MSG_POST_USER_DATA		2
#define MSG_TIMER				3
#define MSG_CREATE_SOCKET       5
#define MSG_CALL_HTTP			6


namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {
					//using json = nlohmann::json;

					int count_data = 0;

					struct ThreadMsg
					{
						ThreadMsg(int i, std::shared_ptr<void> m) { id = i; msg = m; }
						int id;
						std::shared_ptr<void> msg;
					};

					//----------------------------------------------------------------------------
					// WorkerThread
					//----------------------------------------------------------------------------
					WorkerThread::WorkerThread() : m_thread(nullptr), m_timerExit(false)
					{
						
					}

					//----------------------------------------------------------------------------
					// ~WorkerThread
					//----------------------------------------------------------------------------
					WorkerThread::~WorkerThread()
					{
						LOG("CLOSE THREAD START");
						ExitThread();
						m_thread = nullptr;
						LOG("CLOSE THREAD END");
					}

					//----------------------------------------------------------------------------
					// CreateThread
					//----------------------------------------------------------------------------
					bool WorkerThread::CreateThread()
					{
						if (!m_thread)
						{
							m_thread = std::unique_ptr<std::thread>(new std::thread(&WorkerThread::Process, this));

						}
						return true;
					}


					//----------------------------------------------------------------------------
					// ExitThread
					//----------------------------------------------------------------------------
					void WorkerThread::ExitThread()
					{
						if (!m_thread)
							return;

						// Create a new ThreadMsg
						std::shared_ptr<ThreadMsg> threadMsg(new ThreadMsg(MSG_EXIT_THREAD, 0));

						// Put exit thread message into the queue
						{
							std::lock_guard<std::mutex> lock(m_mutex);
							m_queue.push(threadMsg);
							m_cv.notify_one();
						}

						m_thread->join();
						m_thread = nullptr;
					}
					//----------------------------------------------------------------------------
					// Close Thread
					//----------------------------------------------------------------------------
					void WorkerThread::setSocketExiss(bool socketExiss)
					{
						m_socketExiss = socketExiss;
					}

					bool WorkerThread::socketExiss()
					{
						return m_socketExiss;
					}

					//----------------------------------------------------------------------------
					// PostMsg
					//----------------------------------------------------------------------------
					void WorkerThread::PostMsg(std::shared_ptr<UserData> data)
					{
						if (m_thread);

						// Create a new ThreadMsg
						std::shared_ptr<ThreadMsg> threadMsg(new ThreadMsg(MSG_POST_USER_DATA, data));

						// Add user data msg to queue and notify worker thread
						std::unique_lock<std::mutex> lk(m_mutex);
						m_queue.push(threadMsg);
						m_cv.notify_one();
					}

					//----------------------------------------------------------------------------
					// call Http
					//----------------------------------------------------------------------------
					void WorkerThread::callHttp(std::shared_ptr<UserData> data)
					{
						if (m_thread);

						// Create a new ThreadMsg
						std::shared_ptr<ThreadMsg> threadMsg(new ThreadMsg(MSG_CALL_HTTP, data));

						// Add user data msg to queue and notify worker thread
						std::unique_lock<std::mutex> lk(m_mutex);
						m_queue.push(threadMsg);
						m_cv.notify_one();
					}

					//----------------------------------------------------------------------------
					// CreateSocket
					//----------------------------------------------------------------------------
					void WorkerThread::CreateSocket()
					{
						if (!m_thread)
							return;
						// Create a new ThreadMsg
						std::shared_ptr<ThreadMsg> threadMsg(new ThreadMsg(MSG_CREATE_SOCKET, 0));

						// Put create socket thread
						{
							std::lock_guard<std::mutex> lock(m_mutex);
							m_queue.push(threadMsg);
							m_cv.notify_one();
						}
					}


					//----------------------------------------------------------------------------
					// TimerThread
					//----------------------------------------------------------------------------
					void WorkerThread::TimerThread()
					{
						while (!m_timerExit)
						{
							// Sleep for 250mS then put a MSG_TIMER into the message queue
							std::this_thread::sleep_for(std::chrono::milliseconds(100));

							std::shared_ptr<ThreadMsg> threadMsg(new ThreadMsg(MSG_TIMER, 0));

							// Add timer msg to queue and notify worker thread
							std::unique_lock<std::mutex> lk(m_mutex);
							m_queue.push(threadMsg);
							m_cv.notify_one();
						}
					}

					//----------------------------------------------------------------------------
					// Check socket live
					//----------------------------------------------------------------------------
					bool WorkerThread::isSocketServerAlive(std::string serverIP, int serverPort) {
						#ifdef _WIN32
						WSADATA wsaData;
						if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
							std::cerr << "WSAStartup failed." << std::endl;
							return false;
						}

						SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						if (clientSocket == INVALID_SOCKET) {
							WSACleanup();
							return false;
						}

						std::string address;

						boost::asio::io_service io_service;
						boost::asio::ip::tcp::resolver resolver(io_service);
						boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");

						boost::system::error_code ec;
						boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query, ec);

						if (!ec) {
							while (it != boost::asio::ip::tcp::resolver::iterator()) {
								boost::asio::ip::address addr = (it++)->endpoint().address();
								if (addr.is_v4()) {
									address = addr.to_string();
								}
							}
						}


						sockaddr_in serverAddr;
						//serverAddr.sin_addr.s_addr = INADDR_ANY;
						serverAddr.sin_family = AF_INET;
						//serverAddr.sin_family = AF_INET;
						//serverAddr.sin_addr.s_addr = INADDR_ANY;
						serverAddr.sin_port = htons(serverPort);
						inet_pton(AF_INET, address.c_str(), &(serverAddr.sin_addr));

						if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == 0) {
							// Connection successful
							closesocket(clientSocket);
							WSACleanup();
							return true;
						}
						else {
							// Connection failed
							closesocket(clientSocket);
							WSACleanup();
							return false;
						}
						#else
						int server_fd, new_socket, valread;
						struct sockaddr_in address;
						int opt = 1;
						int addrlen = sizeof(address);
						char buffer[10000] = { 0 };

						// Creating socket file descriptor
						if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
							exit(EXIT_FAILURE);
						}
						
						address.sin_family = AF_INET;
						address.sin_addr.s_addr = INADDR_ANY;
						address.sin_port = htons(serverPort);

						int result =connect(server_fd, (struct sockaddr*)&address, sizeof(address));
						if (result == 0) 
						{
							close(server_fd);
							return true;
							
						}
						else 
						{
							close(server_fd);	
							return false;		
							
						}		
						#endif
					}

					//----------------------------------------------------------------------------
					// Process
					//----------------------------------------------------------------------------
					void WorkerThread::Process()
					{
						m_timerExit = false;
						std::thread timerThread(&WorkerThread::TimerThread, this);

						while (1)
						{
							std::shared_ptr<ThreadMsg> msg;
							{
								// Wait for a message to be added to the queue
								std::unique_lock<std::mutex> lk(m_mutex);
								while (m_queue.empty())
									m_cv.wait(lk);

								if (m_queue.empty())
									continue;

								msg = m_queue.front();
								m_queue.pop();
							}

							switch (msg->id)
							{
							case  MSG_CREATE_SOCKET:
							{
								if (isSocketServerAlive("192.168.56.1", 5555)) {
									Logger::instance()->writeLogServer(__func__, "server is live");
									setSocketExiss(true);
								}
								else
								{
									p_socket.reset(new Socket());
									LOG("CREATE SOCKET SUCESS");
									p_socket->createSocket();
									LOG("CREATE SOCKET");
								}
								break;
							}

							case MSG_POST_USER_DATA:
							{
								auto userData = std::static_pointer_cast<UserData>(msg->msg);
								boost::asio::io_context ioc;
								auto client_send = std::make_shared<CWebSocket_Sync>(ioc);
								auto const host_send = "192.168.111.65";
								auto const port_send = "8000";

								client_send->connectWeb(host_send, port_send);
								client_send->send_data(userData->msg);
								client_send->CloseConnection();
								LOG("POST MSG");

								break;
							}

							case MSG_TIMER:
								//Logger::instance()->writeLogServer(__func__, "Timer expired on ");
								break;

							case MSG_EXIT_THREAD:
							{
								LOG("CLOSING THREAD");
								m_timerExit = true;
								timerThread.join();
								return;
							}

							case MSG_CALL_HTTP:
							{
								try {
									boost::asio::io_service io_service;

									// Resolve the server address and port
									tcp::resolver resolver(io_service);
									tcp::resolver::query query("192.168.111.59", "8000");
									tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

									// Establish a connection to the server
									tcp::socket socket(io_service);
									boost::asio::connect(socket, endpoint_iterator);

									// Load the image data
									auto userData = std::static_pointer_cast<UserData>(msg->msg);
									std::string err;
									const auto msgJson = json11::Json::parse(userData->msg, err);

									std::string path = msgJson["file"].string_value();

									std::ifstream image_file(path, std::ios::binary | std::ios::ate);
									std::streamsize image_size = image_file.tellg();
									image_file.seekg(0);
									std::vector<char> image_data(image_size);

									// JSON data
									std::string color = "red";
									int lane = 1;
									int speed = 1;
									std::string traffic_plate = "325565";
									std::string vehicle = "car";
									std::string id_device = "54a5s4das";

									// Create the POST request body (multipart/form-data)
									std::string boundary = "------WebKitFormBoundary7MA4YWxkTrZu0gW";
									std::string body = "--" + boundary + "\r\n"
										"Content-Disposition: form-data; name=\"Color\"\r\n\r\n" + color + "\r\n"
										"--" + boundary + "\r\n"
										"Content-Disposition: form-data; name=\"Lane\"\r\n\r\n" + std::to_string(lane) + "\r\n"
										"--" + boundary + "\r\n"
										"Content-Disposition: form-data; name=\"Speed\"\r\n\r\n" + std::to_string(speed) + "\r\n"
										"--" + boundary + "\r\n"
										"Content-Disposition: form-data; name=\"Traffic_plate\"\r\n\r\n" + traffic_plate + "\r\n"
										"--" + boundary + "\r\n"
										"Content-Disposition: form-data; name=\"Vehicle\"\r\n\r\n" + vehicle + "\r\n"
										"--" + boundary + "\r\n"
										"Content-Disposition: form-data; name=\"file\"; filename=\"image.jpg\"\r\n"
										"Content-Type: image/jpeg\r\n\r\n";
									body += std::string(image_data.begin(), image_data.end()) + "\r\n";
									body += "--" + boundary + "\r\n"
										"Content-Disposition: form-data; name=\"id_device\"\r\n\r\n" + id_device + "\r\n";
									body += "--" + boundary + "--\r\n";

									// Create the HTTP POST request
									std::string request = "POST /api/v1/vms-ai HTTP/1.1\r\n"
										"Host: 192.168.111.59:8000\r\n"
										"Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
										"Content-Length: " + std::to_string(body.length()) + "\r\n"
										"Connection: close\r\n\r\n" + body;
									// std::string request = body;
									 // Send the POST request
									boost::asio::write(socket, boost::asio::buffer(request));

									// Read and print the response from the server
									boost::asio::streambuf response;
									boost::asio::read_until(socket, response, "\r\n");

									// Convert response streambuf to string
									std::string response_str;
									std::istream response_stream(&response);
									while (!response_stream.eof()) {
										char buffer[10000];
										response_stream.read(buffer, sizeof(buffer));
										response_str.append(buffer, response_stream.gcount());
										memset(buffer, 0, sizeof(buffer));
									}

									LOG("HTTP Response:\n");
									LOG(response_str);

								}
								catch (std::exception& e) {
									LOG("Exception: " + std::string(e.what()) + "\n");
								}

							}
							default:
							{
								break;
							}
							}
						}
					}
				}
			}
		}
	}
}
