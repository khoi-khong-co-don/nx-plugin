// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/sdk/analytics/helpers/plugin.h>
#include <nx/sdk/analytics/i_engine.h>

namespace nx {
namespace vms_server_plugins {
namespace analytics {
    namespace stub {
        namespace FaceDetection {

            class Plugin : public nx::sdk::analytics::Plugin
            {
            protected:
                 nx::sdk::Result<nx::sdk::analytics::IEngine*> doObtainEngine() override;
                 std::string manifestString() const override;
            };

        } // namespace sample
    }
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
