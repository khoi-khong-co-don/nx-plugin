#ifndef ORYZACAMERA_H
#define ORYZACAMERA_H
#include <dhnetsdk.h>
#ifdef WIN64
#pragma comment(lib, "dhnetsdk.lib")
#endif
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>

#include "../../i_camera.h"
#include "oryza_event_controller.h"
#include "oryza_SDK_connection.h"

using namespace nx::vms_server_plugins::analytics::stub;
namespace nx {
    namespace vms_server_plugins {
        namespace analytics {
            namespace stub {
                namespace TrafficPlate {

                    class OryzaCamera : public ICamera {

                    public:
                        OryzaCamera();
                        ~OryzaCamera();
                    
                    public:
                        void initialize() override;
                        void configure() override;
                        bool login() override;
                        bool logout() override;
                        bool subscribe() override;
                        bool unsubscribe() override;
                        void doAction() override;
                        void capture(BYTE* pBuffer, DWORD dwBufSize, LDWORD User);

                        void setCallBack(const bool& callback) override;
                        bool callback() const override;

                        void setIsRecognized(const bool& isRecognized)override;
                        bool isRecognized() const override;

                        void setImgpath(const std::string& imgpath) override;
                        std::string imgpath() const override;

                        std::string host() const override;
                        void setHost(const std::string& host) override;

                        std::string port() const override;
                        void setPort(const std::string& port) override;

                        std::string username() const override;
                        void setUsername(const std::string& username) override;

                        std::string password() const override;
                        void setPassword(const std::string& password) override;

                        std::string deviceId() const override;
                        void setDeviceId(const std::string deviceId) override;

                        std::vector<std::string> faceDataInfo() const override;
                        void setFaceDataInfo(const std::vector<std::string> faceDataInfo) override;

                        std::vector<std::string> recognitionDataInfo() const override;
                        void setRecognitionDataInfo(const std::vector<std::string> faceRecognitionInfo) override;
                        
                        std::vector<std::string> trafficInfo() const override;
                        void setTrafficInfo(const std::vector<std::string> trafficInfo) override;

                        void saveTrafficPlate(std::vector<std::string> pstInfo);

                    protected:
                
                        std::map<std::string, std::string> m_facedataInfos;
                        std::map<std::string, std::string> m_faceRecognitionInfo;
                        std::map<std::string, std::string> m_trafficdataInfo;

                    private:
                        std::shared_ptr<OryzaEventController> p_ev_cntrl;
                        std::shared_ptr<OryzaSDKConnection> p_connection;
                    };
                }
            }
        }
    }
}
#endif // ORYZACAMERA_H
