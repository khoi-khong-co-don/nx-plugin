#pragma once
#include <dhnetsdk.h>
#ifdef WIN64
#pragma comment(lib, "dhnetsdk.lib")
#endif
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <map>

#include "../i_event_controller.h"

namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {

					class DahuaEventController : public IEventController
					{
					public:
						DahuaEventController();
						 ~DahuaEventController();

					public:
						void capture(DEV_EVENT_TRAFFICJUNCTION_INFO* pstInfo, BYTE* pBuffer, DWORD dwBufSize, LDWORD dwUser);

						void saveTrafficPlateDetection(DEV_EVENT_TRAFFICJUNCTION_INFO* pstInfo);

						void saveFaceDetection(DEV_EVENT_FACERECOGNITION_INFO* pstInfo);

						void saveFaceRecognition(CANDIDATE_INFOEX* pstCandidate);

					};

				} // namespace sample
			}
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx