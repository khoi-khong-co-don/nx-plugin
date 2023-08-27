// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/sdk/analytics/helpers/plugin.h>
#include <nx/sdk/analytics/helpers/engine.h>
#include <nx/sdk/analytics/i_uncompressed_video_frame.h>

namespace nx {
namespace vms_server_plugins {
namespace analytics {
    namespace stub {
        namespace TrafficPlate {

            class Engine : public nx::sdk::analytics::Engine
            {
            public:
                Engine();
                 ~Engine() override;

            protected:
                 std::string manifestString() const override;

            protected:
                 void doObtainDeviceAgent(
                    nx::sdk::Result<nx::sdk::analytics::IDeviceAgent*>* outResult,
                    const nx::sdk::IDeviceInfo* deviceInfo) override;
            };

        } // namespace sample
    }
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
