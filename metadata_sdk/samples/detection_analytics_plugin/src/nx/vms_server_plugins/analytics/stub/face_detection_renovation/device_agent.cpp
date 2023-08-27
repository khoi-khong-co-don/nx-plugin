// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/
#include <nx/sdk/analytics/helpers/consuming_device_agent.h>
#include <nx/sdk/helpers/settings_response.h>

#include "stub_analytics_plugin_FaceDetection_ini.h"

#define NX_DEBUG_ENABLE_OUTPUT true
#include "device_agent.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <sstream>
#include "../Logger.h"

using namespace nx::sdk;


using namespace nx::sdk::analytics;

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace FaceDetection {


					using namespace nx::sdk;
					using namespace nx::sdk::analytics;

					Ptr<IMetadataPacket> metadataPacket = nullptr;


					/**
					 * @param deviceInfo Various information about the related device, such as its id, vendor, model,
					 *     etc.
					 */
					DeviceAgent::DeviceAgent(const nx::sdk::IDeviceInfo* deviceInfo) :
						// Call the DeviceAgent helper class constructor telling it to verbosely report to stderr.
						ConsumingDeviceAgent(deviceInfo, /*enableOutput*/ true)
					{
						m_deviceId = deviceInfo->logicalId();
						const std::string ip = extractHostFromURL(deviceInfo->url());
						const std::string port = extractPortFromURL_string(deviceInfo->url());
						std::string deviceId = deviceInfo->id();

						camera = CameraManager::getInstance("Oryza" + deviceId, ip);

						camera->initialize();

						camera->setHost(ip);

						camera->setPort(port);

						camera->setDeviceId(deviceId);

						#ifdef WIN32
						createImageStorage("StoreImageFace\\");
						#else
						createImageStorage("/StoreImageFace/");	
						#endif
					}

					DeviceAgent::~DeviceAgent()
					{
						//Logger::instance()->writeLogServer(__func__, "DeviceAgent is destroyed");
						camera->unsubscribe();
						camera->logout();

					}

					/**
					 *  @return JSON with the particular structure. Note that it is possible to fill in the values
					 * that are not known at compile time, but should not depend on the DeviceAgent settings.
					 */
					std::string DeviceAgent::manifestString() const
					{
						// Tell the Server that the plugin can generate the events and objects of certain types.
						// Id values are strings and should be unique. Format of ids:
						// `{vendor_id}.{plugin_id}.{event_type_id/object_type_id}`.
						//
						// See the plugin manifest for the explanation of vendor_id and plugin_id.
						return /*suppress newline*/ 1 + (const char*)R"json(
{
    "eventTypes": [
        {
            "id": ")json" + kNewFaceDetectionEventType + R"json(",
            "name": "Person Detect"
        }
    ],
    "objectTypes": [
        {
            "id": ")json" + kNewFaceDetectionObjectType + R"json(",
            "name": "Person detected"
        },
		{
			"id": ")json" + kNewFaceRecognitionObjectType + R"json(",
            "name": "Person recognized"
		}
    ]
}
)json";
					}

					nx::sdk::Result<const nx::sdk::ISettingsResponse*> DeviceAgent::settingsReceived()
					{
						std::string username = settingValue(kUsernameSetting);
						std::string password = settingValue(kPasswordSetting);

						if (username == camera->username() && password == camera->password())
							return nullptr;

						const auto settingsResponse = new sdk::SettingsResponse();

						camera->setUsername(username);
						camera->setPassword(password);

						if (camera->login() && camera->subscribe()) {
							settingsResponse->setModel(kLoginSuccessSettingsModel);
						}
						else {
							settingsResponse->setModel(kLoginSettingsModel);
						}

						pushManifest(manifestString());

						return settingsResponse;
					}
					/**
					 * Called when the Server sends a new uncompressed frame from a camera.
					 */
					bool DeviceAgent::pushUncompressedVideoFrame(const IUncompressedVideoFrame* videoFrame)
					{
						++m_frameIndex;
						m_lastVideoFrameTimestampUs = videoFrame->timestampUs();

						/*			if (EV_CNTRL(m_deviceId)->callback())
										return true;*/

						return true; //< There were no errors while processing the video frame.
					}

					/**
					 * Serves the similar purpose as pushMetadataPacket(). The differences are:
					 * - pushMetadataPacket() is called by the plugin, while pullMetadataPackets() is called by Server.
					 * - pushMetadataPacket() expects one metadata packet, while pullMetadataPacket expects the
					 *     std::vector of them.
					 *
					 * There are no strict rules for deciding which method is "better". A rule of thumb is to use
					 * pushMetadataPacket() when you generate one metadata packet and do not want to store it in the
					 * class field, and use pullMetadataPackets otherwise.
					 */
					bool DeviceAgent::pullMetadataPackets(std::vector<IMetadataPacket*>* metadataPackets)
					{
						if (!camera->callback())
							return true;
						mutex.lock();
						camera->setCallBack(false);
						LOG("PULL 1");
						*metadataPackets = generateMetdataWithBestShot();
						LOG("PULL 2");
						mutex.unlock();
						return true; //< There were no errors while filling metadataPackets.
					}

					void DeviceAgent::doSetNeededMetadataTypes(
						nx::sdk::Result<void>* /*outValue*/,
						const nx::sdk::analytics::IMetadataTypes* /*neededMetadataTypes*/)
					{

					}

					//-------------------------------------------------------------------------------------------------
					// private

					Ptr<IMetadataPacket> DeviceAgent::generateEventMetadataPacket()
					{
						// Generate event every kTrackFrameCount'th frame.


						// EventMetadataPacket contains arbitrary number of EventMetadata.
						const auto eventMetadataPacket = makePtr<EventMetadataPacket>();
						// Bind event metadata packet to the last video frame using a timestamp.

						//eventMetadataPacket->setTimestampUs(m_lastVideoFrameTimestampUs);
						eventMetadataPacket->setTimestampUs(m_lastVideoFrameTimestampUs);

						// Zero duration means that the event is not sustained, but momental.
						eventMetadataPacket->setDurationUs(0);

						// EventMetadata contains an information about event.
						const auto eventMetadata = makePtr<EventMetadata>();
						// Set all required fields.

						eventMetadata->setTypeId(kNewFaceDetectionEventType);

						eventMetadata->setIsActive(true);

						eventMetadata->setCaption("Person Detect");

						//eventMetadata->setDescription(personRecognized.fullData);

						eventMetadataPacket->addItem(eventMetadata.get());

						// Generate index and track id for the next track.
						++m_trackIndex;
						m_trackId = nx::sdk::UuidHelper::randomUuid();

						return eventMetadataPacket;
					}

					Ptr<IMetadataPacket> DeviceAgent::generateObjectMetadataPacket()
					{
						// ObjectMetadataPacket contains arbitrary number of ObjectMetadata.
						const auto objectMetadataPacket = makePtr<ObjectMetadataPacket>();

						// Bind the object metadata to the last video frame using a timestamp.

						objectMetadataPacket->setTimestampUs(m_lastVideoFrameTimestampUs);
						objectMetadataPacket->setDurationUs(0);
						LOG("METADATA 1");
						// ObjectMetadata contains information about an object on the frame.
						const auto objectMetadata = makePtr<ObjectMetadata>();
						// Set all required fields.
						LOG("METADATA 2");
						if (camera->isRecognized()) {
							objectMetadata->setTypeId(kNewFaceRecognitionObjectType);
							LOG("METADATA 3");
							std::shared_ptr<FaceRecognitionObj> faceRecognitionObject_ptr(new FaceRecognitionObj(camera->recognitionDataInfo()));
							LOG("METADATA 4");
							objectMetadata->setTrackId(m_trackId);
							LOG("METADATA 5");
							objectMetadata->addAttributes(faceRecognitionObject_ptr->attributes());
							LOG("METADATA 6");
							camera->setIsRecognized(false);
						}
						else {
							objectMetadata->setTypeId(kNewFaceDetectionObjectType);

							std::shared_ptr<FaceDetectionObj> faceDetectionObject_ptr(new FaceDetectionObj(camera->faceDataInfo()));

							objectMetadata->setTrackId(m_trackId);

							objectMetadata->addAttributes(faceDetectionObject_ptr->attributes());

							LOG("Face is detected, but not recognized!!!");
						}
						LOG("METADATA 7");
						objectMetadataPacket->addItem(objectMetadata.get());
						LOG("METADATA 8");
						return objectMetadataPacket;
					}

					Ptr<IObjectTrackBestShotPacket> DeviceAgent::generateImageBestShot(Uuid trackId)
					{
						LOG("IMAGE 1");
						auto bestShotPacket = makePtr<ObjectTrackBestShotPacket>(trackId, m_lastVideoFrameTimestampUs);
						LOG("IMAGE 2");
						bestShotPacket->setImageDataFormat(imageFormatFromPath(camera->imgpath()));
						LOG("IMAGE 3");
						bestShotPacket->setImageData(loadFile(camera->imgpath()));
						LOG("IMAGE 4");
						return bestShotPacket;
					}

					std::vector<IMetadataPacket*> DeviceAgent::generateMetdataWithBestShot() {
						LOG(":genera 1");
						auto bestShot = generateImageBestShot(m_trackId).releasePtr();
						LOG(":genera 2");

						auto objectMetadata = generateObjectMetadataPacket().releasePtr();
						LOG(":genera 3");

						m_trackId = nx::sdk::UuidHelper::randomUuid();

						return { objectMetadata, bestShot };
					}


				} // namespace sample
			}
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx
