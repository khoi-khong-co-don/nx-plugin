#pragma once
#include "abstract_obj.h"


using namespace nx::vms_server_plugins::analytics::stub::FaceDetection;
namespace nx {
    namespace vms_server_plugins {
        namespace analytics {
            namespace stub {

                const std::string kNewFaceDetectionObjectType = "nx.stub.FaceDetection.faceDetectionObjectType";

                class FaceDetectionObj : public AbstractObj
                {
                    using base_type = AbstractObj;
                public:
                    FaceDetectionObj(std::vector<std::string> faceDataInfo);
                };

            } // namespace stub
        } // namespace analytics
    } // namespace vms_server_plugins
} // namespace nx