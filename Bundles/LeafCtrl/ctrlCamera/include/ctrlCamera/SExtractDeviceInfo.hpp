#ifndef __CTRLCAMERA_SExtractDeviceInfo_HPP__
#define __CTRLCAMERA_SExtractDeviceInfo_HPP__

#include "ctrlCamera/config.hpp"

#include <arData/Camera.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IController.hpp>



namespace ctrlCamera
{
/**
 * @class SExtractDeviceInfo
 * @brief This service extract camera informations in configuration using android device name.
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
    CTRLCAMERA_API virtual void starting() throw( ::fwTools::Failed );

    /**
     * @brief Configure the service
     *
     * @verbatim
        <service uid="..." type="::fwServices::IController" impl="::ctrlCamera::SExtractDeviceInfo" autoConnect="no">
            <cameraUid>cameraUid</cameraUid>

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
       @endverbatim
     * - cameraUid: uid of the ::arData::Camera to configure
     * - One of devices or configId tag is needed:
     *  - \b devices: configuration of devices
     *  - \b configId: id of a ServiceConfig containing devices configuration.
     */
    CTRLCAMERA_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMERA_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMERA_API virtual void updating() throw( ::fwTools::Failed );

private:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, ConfigurationType > DeviceConfigType;
    typedef ::fwRuntime::ConfigurationElementContainer::Container CfgContainer;

    std::string m_cameraUid;
    ::arData::Camera::sptr m_camera;

    DeviceConfigType m_devicesConfig;

};

} // namespace ctrlCamera


#endif /* __CTRLCAMERA_SExtractDeviceInfo_HPP__ */
