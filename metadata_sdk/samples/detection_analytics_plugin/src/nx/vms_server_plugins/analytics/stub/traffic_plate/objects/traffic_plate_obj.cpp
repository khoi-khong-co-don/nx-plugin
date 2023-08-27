#include <vector>

#include "traffic_plate_obj.h"


namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {
					using namespace nx::sdk;
					static Attributes makeAttributes(std::vector<std::string> trafficplateDataInfo)
					{
						return {
								nx::sdk::makePtr<Attribute>(IAttribute::Type::string, "Traffic Plate", trafficplateDataInfo[e_Plate] + "\n"),
								nx::sdk::makePtr<Attribute>(IAttribute::Type::string, "Color", trafficplateDataInfo[e_Color] + "\n"),
								nx::sdk::makePtr<Attribute>(IAttribute::Type::string, "Vehicle", trafficplateDataInfo[e_Vehicle] + "\n"),
								nx::sdk::makePtr<Attribute>(IAttribute::Type::string, "Lane", trafficplateDataInfo[e_Lane] + "\n"),
								nx::sdk::makePtr<Attribute>(IAttribute::Type::string, "Speed", trafficplateDataInfo[e_Speed] + " km/h")
						};
					}

					TrafficPlateObj::TrafficPlateObj(std::vector<std::string> trafficplateDataInfo) :
						base_type(kTrafficPlateObjectType, makeAttributes(trafficplateDataInfo))
					{
					}
				}
			} // namespace stub
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx