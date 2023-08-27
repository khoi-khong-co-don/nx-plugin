#include "socket.h"
#include "oryza_camera.h"
#include "../../Rece_data_socket.h"
//#include "WorkerThread.h"
#ifdef _WIN32
#else
#include <future>
#endif

namespace nx {
    namespace vms_server_plugins {
        namespace analytics {
            namespace stub {
                namespace TrafficPlate {

                    #define UNKNOWN "Unknown" 
                   // using nx::kit::Json;

                    //----------------------------------------------------------------------------
                   // Get message from socket
                   //----------------------------------------------------------------------------
                    #ifdef _WIN64
					void Socket::GetMsgSocket(SOCKET client)
                    {
                        LOG("GET MSG START");
                        std::string dataSocket = "";

                        int iResult;
                        char buffer[10000];

                        LOG("RECEIVE DATA");
                        iResult = recv(client, buffer, sizeof(buffer), 0);

                        if (iResult > 10)
                        {
                            for (int i = 0; i < iResult; i++)
                            {
                                dataSocket += buffer[i];
                            }
                            memset(buffer, 0, sizeof(buffer));

                            std::string err;
                            const auto msgJson = json11::Json::parse(dataSocket, err);
                            DataSocket sendData;
                            std::string user = "";
                            std::string ai_module = "";

                            user = msgJson["address"].string_value();
                            ai_module = msgJson["ai_module"].string_value();

                            sendData.dataSocket(std::stoi(ai_module), dataSocket, user);
                        }
						closesocket(client);                   
                        LOG("Receive data success!!!!!!!");
                    }
					#else
					void Socket::GetMsgSocket(int new_socket)
					{
                        LOG("GET MSG START");
                        std::string dataSocket = "";

                        int iResult;
                        char buffer[10000];

                        LOG("RECEIVE DATA");
                        
                        iResult = read(new_socket, buffer, sizeof(buffer));

                        if (iResult > 10)
                        {
                            for (int i = 0; i < iResult; i++)
                            {
                                dataSocket += buffer[i];
                            }
                            memset(buffer, 0, sizeof(buffer));

                            std::string err;
                            const auto msgJson = json11::Json::parse(dataSocket, err);
                            DataSocket sendData;
                            std::string user = "";
                            std::string ai_module = "";

                            user = msgJson["address"].string_value();
                            ai_module = msgJson["ai_module"].string_value();

                            sendData.dataSocket(std::stoi(ai_module), dataSocket, user);
                        }


                        //if (iResult > 10)
                        //{
                        //    for (int i = 0; i < iResult; i++)
                        //    {
                        //        dataSocket += buffer[i];
                        //    }
                        //    memset(buffer, 0, sizeof(buffer));

                        //    std::string err;
                        //    const auto msgJson = json11::Json::parse(dataSocket, err);
                        //    
                        //    std::string traffic = "";
                        //    std::string color = "";
                        //    std::string vehicle = "";
                        //    std::string lane = "";
                        //    std::string speed = "";
                        //    std::string base64 = "";
                        //    std::string user = "";

                        //    traffic = (!msgJson["traffic"].string_value().empty() ? msgJson["traffic"].string_value() : UNKNOWN);
                        //    color = (!msgJson["color"].string_value().empty() ? msgJson["color"].string_value() : UNKNOWN);
                        //    vehicle = (!msgJson["vehicle"].string_value().empty() ? msgJson["vehicle"].string_value() : UNKNOWN);
                        //    lane = (!msgJson["lane"].string_value().empty() ? msgJson["lane"].string_value() : UNKNOWN);
                        //    speed = (!msgJson["speed"].string_value().empty() ? msgJson["speed"].string_value() : UNKNOWN);

                        //    base64 = msgJson["base64"].string_value();
                        //    user = msgJson["address"].string_value();

                        //    if (user != "")
                        //    {
                        //        const char* user_str = user.c_str();
                        //        std::uint64_t address_value = std::strtoull(user_str, nullptr, 16);
                        //        OryzaCamera* oryza_cam = reinterpret_cast<OryzaCamera*>(address_value);

                        //        LOG("GET MESS 1");
                        //        std::vector<std::string> data = { traffic , color , vehicle , lane , speed };
                        //        //std::vector<std::string> data = { name , birthday , city , country };
                        //        LOG("GET MESS 2");
                        //        oryza_cam->saveTrafficPlate(data);
                        //        LOG("GET MESS 3");
                        //        std::string base64String = base64; // Replace with your Base64 string

                        //        std::vector<unsigned char> bytes = base64_decode_to_bytes(base64String);

                        //        DWORD result_dword = get_buffer_size(base64String);
                        //        LOG("GET MESS 4");
                        //        oryza_cam->capture(bytes.data(), result_dword, address_value);
                        //        LOG("GET MESS 5");
                        //        oryza_cam->setCallBack(TRUE);
                        //        oryza_cam->setIsRecognized(TRUE);
                        //        LOG("GET MESS 6");
                        //    }
                        //}
						close(new_socket);
                        LOG("Receive data success!!!!!!!");
                    };
                    #endif
                    //----------------------------------------------------------------------------
                    // Create Socket
                    //----------------------------------------------------------------------------
                    void Socket::createSocket(){
                        LOG("CREATE SOCKET SUCESS");
                        #ifdef _WIN32
                        WSADATA wsa_data;
                        SOCKADDR_IN server_addr, client_addr;
                        WSAStartup(MAKEWORD(2, 2), &wsa_data);


                        SOCKET server;
                        int client_addr_size;
                        server = socket(AF_INET, SOCK_STREAM, 0);

                        server_addr.sin_addr.s_addr = INADDR_ANY;
                        server_addr.sin_family = AF_INET;
                        server_addr.sin_port = htons(5555);

                        ::bind(server, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr));
                        listen(server, 0);


                        client_addr_size = sizeof(client_addr);
                        Logger::instance()->writeLogServer(__func__, "Listening for incoming connections...");

                        while (1)
                        {
                            SOCKET client;

                            if ((client = accept(server, reinterpret_cast<SOCKADDR*>(&client_addr), &client_addr_size)) != INVALID_SOCKET)
                            {
                                c_mutex_socket.lock();
                                LOG("HAVE DATA TO RECEIVE");
                                    
                                //auto fut = async(launch::async, &WorkerThread::GetMsgSocket, this, client, idcam);
                                std::thread thread_rece;
                                thread_rece = std::thread(&Socket::GetMsgSocket, this, client);

                                thread_rece.join();
                                c_mutex_socket.unlock();
                                LOG("LISTEN SUCCESS!!!!!!");
                            }

                            const auto last_error = WSAGetLastError();

                            if (last_error > 0)
                            {
                            }
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

                        // Forcefully attaching socket to the port 8080
                        if (setsockopt(server_fd, SOL_SOCKET,
                                    SO_REUSEADDR | SO_REUSEPORT, &opt,
                                    sizeof(opt))) {
                            exit(EXIT_FAILURE);
                        }
                        address.sin_family = AF_INET;
                        address.sin_addr.s_addr = INADDR_ANY;
                        address.sin_port = htons(5555);


                        // Forcefully attaching socket to the port 8080
                        if (bind(server_fd, (struct sockaddr*)&address,
                                sizeof(address))
                            < 0) {

                            exit(EXIT_FAILURE);
                        }
                        if (listen(server_fd, 3) < 0) {
                            exit(EXIT_FAILURE);
                        }
                        LOG("LISTENING...");
                        while(1)
                        {
                            if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) >= 0) {
                                LOG("HAVE DATA TO RECEIVE");
                                auto fut = std::async(std::launch::async, &Socket::GetMsgSocket, this, new_socket);
                            }
                        }

						#endif
                    };
                }
            }
        }
    }
}