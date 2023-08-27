#include <nx/kit/debug.h>

#include "face_detection_renovation/plugin.h"
#include "traffic_plate/plugin.h"

extern "C" NX_PLUGIN_API nx::sdk::IPlugin * createNxPluginByIndex(int instanceIndex)
{
	using namespace nx::vms_server_plugins::analytics::stub;

	switch (instanceIndex)
	{
	case 0: return new TrafficPlate::Plugin();
	case 1: return new FaceDetection::Plugin();


	default: return nullptr;
	}
}
