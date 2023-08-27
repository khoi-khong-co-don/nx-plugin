#include "oryza_camera.h"

using namespace nx::vms_server_plugins::analytics::stub;
namespace nx {
    namespace vms_server_plugins {
        namespace analytics {
            namespace stub {
                namespace TrafficPlate {

                    OryzaCamera::OryzaCamera() {

                    }

                    OryzaCamera::~OryzaCamera() {

                    }
                    void OryzaCamera::initialize() {
                        LOG("Oryza camera initialized.");
                       
                        p_ev_cntrl.reset(new OryzaEventController());
                        p_connection.reset(new OryzaSDKConnection());
                    }

                    void OryzaCamera::configure() {
                       LOG( "Oryza camera configured.");
                        /*auto myid = std::this_thread::get_id();

                        std::stringstream ss;
                        ss << this;
                        std::string Address = ss.str();
                        LOG("ID thread Oryza cam: -- " + Address);*/
                    }

                    bool OryzaCamera::login() {
                        LOG("Oryza camera log in.");
                        p_connection->login((LDWORD)this);
                        LOG("Oryza camera login success.");
                        return p_connection->loginSuccess();
                    }

                    bool OryzaCamera::logout() {
                        LOG("Oryza camera logged out.");
                        p_connection->logout((LDWORD)this);
                        return true;
                    }

                    bool OryzaCamera::subscribe() {
                        LOG("Oryza camera subscribed.");
                        p_connection->subscribe((LDWORD)this);

                        std::stringstream ss;
                        ss << this;
                        std::string Address = ss.str();
                        LOG("ID thread Oryza cam: -- " + Address);

                        return p_connection->subscribeSuccess();
                    }

                    bool OryzaCamera::unsubscribe() {
                        LOG("Oryza camera unsubscribed.");;
                        p_connection->unsubscribe((LDWORD)this);
                        return true;
                    }

                    void OryzaCamera::doAction() {
                        std::cout << "Oryza camera performing an action." << std::endl;
                    }

                    void OryzaCamera::capture(BYTE* pBuffer, DWORD dwBufSize, LDWORD User) {
                       LOG("Capturing image with Oryza camera.");
                        p_ev_cntrl->capture(pBuffer, dwBufSize, User);
                        LOG("Capturing image with Oryza camera success.");
                    }

                    void OryzaCamera::setCallBack(const bool& callback)
                    {
                        p_ev_cntrl->setCallback(callback);
                    }

                    bool OryzaCamera::callback() const
                    {
                        return p_ev_cntrl->callback();
                    }

                    void OryzaCamera::setIsRecognized(const bool& isRecognized)
                    {
                        p_ev_cntrl->setIsRecognized(isRecognized);
                    }

                    bool OryzaCamera::isRecognized() const
                    {
                        return p_ev_cntrl->isRecognized();
                    }

                    void OryzaCamera::setImgpath(const std::string& imgpath)
                    {
                        p_ev_cntrl->setImgpath(imgpath);
                    }

                    std::string OryzaCamera::imgpath() const
                    {
                        return p_ev_cntrl->imgpath();
                    }

                    std::string OryzaCamera::host() const
                    {
                        return p_ev_cntrl->host();
                    }

                    void OryzaCamera::setHost(const std::string& host)
                    {
                        p_ev_cntrl->setHost(host);
                    }

                    std::string OryzaCamera::port() const
                    {
                        return p_ev_cntrl->port();;
                    }

                    void OryzaCamera::setPort(const std::string& post)
                    {
                        p_ev_cntrl->setPort(post);
                    }

                    std::string OryzaCamera::username() const
                    {
                        return p_ev_cntrl->username();
                    }

                    void OryzaCamera::setUsername(const std::string& username)
                    {
                        p_ev_cntrl->setUsername(username);
                    }

                    std::string OryzaCamera::password() const
                    {
                        return p_ev_cntrl->password();
                    }

                    void OryzaCamera::setPassword(const std::string& password)
                    {
                        p_ev_cntrl->setPassword(password);
                    }

                    std::string OryzaCamera::deviceId() const
                    {
                        return p_ev_cntrl->deviceId();
                    }

                    void OryzaCamera::setDeviceId(const std::string deviceId)
                    {
                        p_ev_cntrl->setdeviceId(deviceId);
                    }

                
                    std::vector<std::string> OryzaCamera::faceDataInfo() const
                    {
                        return p_ev_cntrl->faceDataInfo();
                    }

                    void OryzaCamera::setFaceDataInfo(const std::vector<std::string> faceDataInfo)
                    {
                        p_ev_cntrl->setFaceDataInfo(faceDataInfo);
                    }

                    std::vector<std::string> OryzaCamera::recognitionDataInfo() const
                    {
                        return p_ev_cntrl->recognitionDataInfo();
                    }

                    void OryzaCamera::setRecognitionDataInfo(const std::vector<std::string> faceRecognitionInfo)
                    {
                        p_ev_cntrl->setRecognitionDataInfo(faceRecognitionInfo);
                    }
                    std::vector<std::string> OryzaCamera::trafficInfo() const
                    {
                        LOG("LAY DATA");
                        return p_ev_cntrl->TrafficplateDataInfos();
                    }

                    void OryzaCamera::setTrafficInfo(const std::vector<std::string> trafficInfo)
                    {
                        p_ev_cntrl->setTrafficplateDataInfos(trafficInfo);
                    }

                    void OryzaCamera::saveTrafficPlate(std::vector<std::string> pstInfo)
                    {
                        LOG("SAVE DATA 1");
                        p_ev_cntrl->saveTrafficPlate(pstInfo);
                    }

                }
            }
        }
    }
}