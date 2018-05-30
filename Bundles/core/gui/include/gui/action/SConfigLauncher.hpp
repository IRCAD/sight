/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "gui/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/helper/ConfigLauncher.hpp>

#include <fwTools/Failed.hpp>

namespace gui
{
namespace action
{

/**
 * @brief   This action starts/stops a template configuration.
 *
 *  The parameters of the template configuration <param name="..." /> are filled according to the
 *  <inout> and <parameter> tags. Using <inout> is especially useful to wait for deferred objects, but it is strongly
 * recommended to use it to pass any object.
 * Note that <in> is not supported. This would have no meaning, because we can't ensure the object won't be modified
 * in the configuration. <out> is also not supported because if we assume that the target configuration produces the
 * object, thus we would not get a valid id for the matching parameter.
 *
 * @section Signals Signals
 * - \b launched(): emitted when the configuration is launched
 *
 * @section Slots Slots
 * - \b stopConfig(): called to stop the running configuration
 *
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
class GUI_CLASS_API SConfigLauncher : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SConfigLauncher)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    GUI_API SConfigLauncher() noexcept;

    /// Destructor. Do nothing.
    GUI_API virtual ~SConfigLauncher() noexcept;

    /// Set the action service is activated/disable.
    GUI_API virtual void setIsActive(bool isActive) override;

    // Launched signal key
    GUI_API static const ::fwCom::Signals::SignalKeyType s_LAUNCHED_SIG;

protected:

    /**
     * @name Defines signal triggered when config is started
     * @{ */
    typedef ::fwCom::Signal< void () > LaunchedSignalType;
    LaunchedSignalType::sptr m_sigLaunched;
    /**  @} */

    ///This method launches the IAction::starting method.
    virtual void starting() override;

    ///This method launches the IAction::stopping method.
    virtual void stopping() override;

    /**
     * @brief Starts the view and initialize the operator.
     */
    virtual void updating() override;

    /**
     * @brief Declare the configuration to associate with an operator type and a view config
     *
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @code{.xml}
       <service impl="::gui::action::SConfigLauncher" type="::fwGui::IActionSrv">
           <config>
                <appConfig id="Visu2DID" >
                    <parameters>
                        <parameter replace="SERIESDB" by="medicalData"  />
                        <parameter replace="IMAGE" by="@values.image"  />
                    </parameters>
                </appConfig>
            </config>
       </service>
        @endcode
     */
    virtual void configuring() override;

    /**
     * @name Slots
     * @}
     */
    /// Slot: stop the config.
    void stopConfig();
    /**
     * @}
     */

    ::fwServices::helper::ConfigLauncher::uptr m_configLauncher;
    std::string m_proxychannel; ///< Name of the channel used to connect stopConfig slot to the config frame closing.
};

} // action
} // gui
