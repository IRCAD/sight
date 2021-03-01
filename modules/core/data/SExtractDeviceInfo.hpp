/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/data/config.hpp"

#include <core/base.hpp>

#include <data/Camera.hpp>

#include <service/IController.hpp>
#include <service/macros.hpp>

namespace sight::module::data
{
/**
 * @brief This service extract camera informations in configuration using android device name.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::sight::module::data::SExtractDeviceInfo">
            <inout key="camera" uid="..." />
            <devices>
                <device>
                    <name>Nexus10</name>
                    <intrinsic>
                        <width>.</width>
                        <height>.<height>
                        <fx>.</fx>
                        <fy>.</fy>
                        <cx>.</cx>
                        <cy>.</cy>
                        <k1>.</k1>
                        <k2>.</k2>
                        <p1>.</p1>
                        <p2>.</p2>
                        <k3>.</k3>
                        <skew>.</skew>
                    </intrinsic>
                </device>
                    ...
                <device>
                    ...
                </device>
            </devices>
            <!-- or -->
            <configId>my config</configId>
           </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b camera [sight::data::Camera]: Camera to update according to device information.
 * @subsection Configuration Configuration
 * - One of devices or configId tag is needed:
 *  - \b devices: configuration of devices
 *  - \b configId: id of a Config containing devices configuration.
 */
class MODULE_DATA_CLASS_API SExtractDeviceInfo : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SExtractDeviceInfo, service::IController)

    /// Constructor
    MODULE_DATA_API SExtractDeviceInfo();

    /// Destructor
    MODULE_DATA_API ~SExtractDeviceInfo();

protected:

    /// Does nothing
    MODULE_DATA_API virtual void starting() override;

    /// Configure the service
    MODULE_DATA_API virtual void configuring() override;

    /// Does nothing
    MODULE_DATA_API virtual void stopping() override;

    /// Does nothing
    MODULE_DATA_API virtual void updating() override;

private:

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, ConfigurationType > DeviceConfigType;
    typedef core::runtime::ConfigurationElementContainer::Container CfgContainer;

    DeviceConfigType m_devicesConfig;

};

} // namespace sight::module::data
