// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/
#include <nx/sdk/analytics/helpers/consuming_device_agent.h>
#include <nx/sdk/helpers/settings_response.h>

#include "stub_analytics_plugin_TrafficPlate_ini.h"

#define NX_DEBUG_ENABLE_OUTPUT true
#include "device_agent.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <sstream>
#include "../Logger.h"
// #include <boost/version.hpp>

using namespace nx::sdk;


using namespace nx::sdk::analytics;

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {


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
						const std::string deviceId = deviceInfo->id();
						const std::string ip = extractHostFromURL(deviceInfo->url());
						const std::string port = extractPortFromURL_string(deviceInfo->url());
						camera = CameraManager::getInstance("Oryza" + deviceId, ip);

						//Logger::instance()->writeLogServer("________________m_deviceId", m_deviceId);

						camera->initialize();
						camera->setHost(ip);
						camera->setPort(port);
						camera->setDeviceId(deviceId);

						#ifdef WIN32
						createImageStorage("StoreImageTraffic\\");
						#else
						createImageStorage("/StoreImageTraffic/");
						#endif

						// Logger::instance()->writeLogServer("Hello, Boost! Version ", BOOST_VERSION);
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
            "id": ")json" + kNewTrafficPlateEventType + R"json(",
            "name": "Traffic Plate"
        }
    ],
    "objectTypes": [
		{
			"id": ")json" + kNewTrafficPlateObjectType + R"json(",
            "name": "Traffic Plate"
		}
    ]
}
)json";
					}

					nx::sdk::Result<const nx::sdk::ISettingsResponse*> DeviceAgent::settingsReceived()
					{
						camera->setUsername(settingValue(kUsernameSetting));
						camera->setPassword(settingValue(kPasswordSetting));
						camera->login();
						camera->subscribe();

						return nullptr;
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

						camera->setCallBack(false);
						*metadataPackets = generateMetdataWithBestShot();
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

						eventMetadata->setTypeId(kNewTrafficPlateEventType);

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
						// ObjectMetadata contains information about an object on the frame.
						const auto objectMetadata = makePtr<ObjectMetadata>();
						// Set all required fields.
						LOG("METADATA 2");

						objectMetadata->setTypeId(kNewTrafficPlateObjectType);
						LOG("METADATA 3");
						std::shared_ptr<TrafficPlateObj> trafficPlateObject_ptr(new TrafficPlateObj(camera->trafficInfo()));
						LOG("METADATA 4");
						objectMetadata->setTrackId(m_trackId);
						LOG("METADATA 5");
						objectMetadata->addAttributes(trafficPlateObject_ptr->attributes());
						LOG("METADATA 6");
						objectMetadataPacket->addItem(objectMetadata.get());
						LOG("METADATA 6");
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
						auto bestShot = generateImageBestShot(m_trackId).releasePtr();
						auto objectMetadata = generateObjectMetadataPacket().releasePtr();

						m_trackId = nx::sdk::UuidHelper::randomUuid();

						return { objectMetadata, bestShot };
					}


				} // namespace sample
			}
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx
