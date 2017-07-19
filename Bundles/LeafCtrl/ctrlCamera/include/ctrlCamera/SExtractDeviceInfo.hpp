/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMERA_SEXTRACTDEVICEINFO_HPP__
#define __CTRLCAMERA_SEXTRACTDEVICEINFO_HPP__

#include "ctrlCamera/config.hpp"

#include <arData/Camera.hpp>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IController.hpp>



namespace ctrlCamera
{
/**
 * @brief This service extract camera informations in configuration using android device name.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::ctrlCamera::SExtractDeviceInfo">
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
 * - \b camera [::arData::Camera]: Camera to update according to device information.
 * @subsection Configuration Configuration
 * - One of devices or configId tag is needed:
 *  - \b devices: configuration of devices
 *  - \b configId: id of a ServiceConfig containing devices configuration.
 */
class CTRLCAMERA_CLASS_API SExtractDeviceInfo : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SExtractDeviceInfo)(::fwServices::IController) );

    /// Constructor
    CTRLCAMERA_API SExtractDeviceInfo();

    /// Destructor
    CTRLCAMERA_API ~SExtractDeviceInfo();

protected:

    /// Does nothing
    CTRLCAMERA_API virtual void starting();

    /// Configure the service
    CTRLCAMERA_API virtual void configuring();

    /// Does nothing
    CTRLCAMERA_API virtual void stopping();

    /// Does nothing
    CTRLCAMERA_API virtual void updating();

private:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, ConfigurationType > DeviceConfigType;
    typedef ::fwRuntime::ConfigurationElementContainer::Container CfgContainer;

    DeviceConfigType m_devicesConfig;

};

} // namespace ctrlCamera


#endif /* __CTRLCAMERA_SEXTRACTDEVICEINFO_HPP__ */
