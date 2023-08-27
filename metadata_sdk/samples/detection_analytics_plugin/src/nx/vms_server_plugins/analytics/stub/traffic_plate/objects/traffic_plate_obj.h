#pragma once
#include "abstract_obj.h"

#include "../define_var.h"

using namespace nx::vms_server_plugins::analytics::stub::TrafficPlate;
namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {
					const std::string kTrafficPlateObjectType = "nx.stub.TrafficPlate.TrafficPlateObjectType";

					class TrafficPlateObj : public AbstractObj
					{
						using base_type = AbstractObj;
					public:
						TrafficPlateObj(std::vector<std::string> trafficplateDataInfo);
					};
				}
			} // namespace stub
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx