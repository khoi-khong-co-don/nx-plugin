// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/sdk/analytics/helpers/consuming_device_agent.h>
#include <nx/sdk/helpers/uuid_helper.h>
#include <nx/sdk/analytics/helpers/event_metadata.h>
#include <nx/sdk/analytics/helpers/event_metadata_packet.h>
#include <nx/sdk/analytics/helpers/object_metadata.h>
#include <nx/sdk/analytics/helpers/object_metadata_packet.h>
#include <nx/sdk/analytics/i_object_track_best_shot_packet.h>
#include <nx/sdk/analytics/helpers/object_track_best_shot_packet.h>

#include "dahua_camera/dahua_camera.h"
#include "camera_manager.h"

#include "objects/face_detection_obj.h"
#include "objects/face_recognition_obj.h"

#include "../utils.h" 

#include "engine.h"

#include "settings_model.h"


#include <memory>

#include <dhnetsdk.h>
#ifdef WIN64
#pragma comment(lib, "dhnetsdk.lib")
#endif

#include <mutex>

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace FaceDetection {

					class DeviceAgent : public nx::sdk::analytics::ConsumingDeviceAgent
					{
					public:
						DeviceAgent(const nx::sdk::IDeviceInfo* deviceInfo);
						 ~DeviceAgent() override;

					protected:
						 std::string manifestString() const override;

						 bool pushUncompressedVideoFrame(
							const nx::sdk::analytics::IUncompressedVideoFrame* videoFrame) override;

						 bool pullMetadataPackets(
							std::vector<nx::sdk::analytics::IMetadataPacket*>* metadataPackets) override;

						 void doSetNeededMetadataTypes(
							nx::sdk::Result<void>* outValue,
							const nx::sdk::analytics::IMetadataTypes* neededMetadataTypes) override;

						 nx::sdk::Result<const nx::sdk::ISettingsResponse*> settingsReceived() override;
					private:
						nx::sdk::Ptr<nx::sdk::analytics::IMetadataPacket> generateEventMetadataPacket();
						nx::sdk::Ptr<nx::sdk::analytics::IMetadataPacket> generateObjectMetadataPacket();
						std::vector<nx::sdk::analytics::IMetadataPacket*> generateMetdataWithBestShot();

					private:
						const std::string kNewFaceDetectionObjectType = "nx.stub.FaceDetection.faceDetectionObjectType";
						const std::string kNewFaceRecognitionObjectType = "nx.stub.FaceDetection.faceRecognitionObjectType";

						const std::string kNewFaceDetectionEventType = "nx.stub.FaceDetection.faceDetectionEventType";

					private:
						nx::sdk::Uuid m_trackId = nx::sdk::UuidHelper::randomUuid();
						int m_frameIndex = 0; /**< Used for generating the detection in the right place. */
						int m_trackIndex = 0; /**< Used in the description of the events. */
						/** Used for binding object and event metadata to the particular video frame. */
						int64_t m_lastVideoFrameTimestampUs = 0;

					private:
						nx::sdk::Ptr<nx::sdk::analytics::IObjectTrackBestShotPacket> generateImageBestShot(nx::sdk::Uuid trackId);
					private:
						std::string m_deviceId;
						std::shared_ptr<ICamera> camera;

						std::mutex mutex;
					};

				} // namespace sample
			}
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx
