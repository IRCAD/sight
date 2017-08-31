/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_SCONFIGCONTROLLER_HPP__
#define __FWSERVICES_SCONFIGCONTROLLER_HPP__

#include "fwServices/IController.hpp"
#include "fwServices/config.hpp"
#include "fwServices/helper/ConfigLauncher.hpp"

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwTools/Failed.hpp>

namespace fwServices
{

/**
 * @brief   This service starts/stops a template configuration.
 *
 *  The parameters of the template configuration <param name="..." /> are filled according to the
 *  <inout> and <parameter> tags. Using <inout> is especially useful to wait for deferred objects, but it is strongly
 * recommended to use it to pass any object.
 * Note that <in> is not supported. This would have no meaning, because we can't ensure the object won't be modified
 * in the configuration. <out> is also not supported because if we assume that the target configuration produces the
 * object, thus we would not get a valid id for the matching parameter.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::fwServices::SConfigController" >
            <appConfig id="IdOfConfig" />
            <inout key="object" uid="..." />
            <parameter replace="channel" by="changeValueChannel"  />
            <parameter replace="service" by="serviceUid" />
        </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b object [::fwData::Object]: \b key specifies the name of the parameter in the target configuration and \b uid
 * identifies the object whose uid is passed as value of the parameter.
 * @subsection Configuration Configuration:
 * - \b parameter: \b replace specifies the name of the parameter in the target configuration and \b by the value of
 * this parameter. The variable GENERIC_UID can be used as unique identifier when the configuration is launched.
 */
class FWSERVICES_CLASS_API SConfigController : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SConfigController)(::fwServices::IController) );

    /// Constructor. Does nothing.
    FWSERVICES_API SConfigController() noexcept;

    /// Destructor. Does nothing.
    FWSERVICES_API virtual ~SConfigController() noexcept;

protected:

    /// Starts the config
    virtual void starting();

    /// Stops the config
    virtual void stopping();

    /// Does nothing
    virtual void updating();


    /**
     * @brief Declare the configuration to associate with an operator type and a view config
     *
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @code{.xml}
       <service impl="::fwServices::SConfigController" type="::fwServices::IController">
            <config>
                <appConfig id="IdOfConfig" >
                    <parameters>
                        <parameter replace="SERIESDB" by="medicalData"  />
                        <parameter replace="IMAGE" by="@values.image"  />
                    </parameters>
                </appConfig>
            </config>
       </service>
        @endcode
     * It MUST have at least one key node and at least one replace node.
     */
    virtual void configuring();

    /// Swaps the config
    virtual void swapping();

    /// Overrides
    virtual void info( std::ostream& _sstream );

private:

    /// AppConfig manager
    ::fwServices::helper::ConfigLauncher::uptr m_configLauncher;

};

} // fwServices


#endif // __FWSERVICES_SCONFIGCONTROLLER_HPP__


