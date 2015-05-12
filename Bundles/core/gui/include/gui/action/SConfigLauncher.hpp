/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SCONFIGLAUNCHER_HPP__
#define __GUI_ACTION_SCONFIGLAUNCHER_HPP__

#include <fwCom/Signal.hpp>

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwServices/helper/ConfigLauncher.hpp>

#include "gui/config.hpp"

namespace gui
{
namespace action
{


/**
 * @class   SConfigLauncher
 * @brief   To manage configuration file defines in xml extension.
 * @date    2013.
 *
 * This action works on a ::fwData::Composite. It action starts/stops an AppConfig given by its identifier in this action configuration.
 *  - You can specified pattern to replace in the template configuration by the tag 'replace'.
 *  - You can specified pattern to replace by the uid of the object contained by the composite with the given key
 *  - The pattern GENERIC_UID is replaced by a generated unique identifier when the configuration is launch.
 *  This assure that the created object and services in the configuration have a unique uid even if this
 *  configuration is launch several times.
 *
 * Example of this service configuration
 * @verbatim
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
   @endverbatim
 */
class GUI_CLASS_API SConfigLauncher : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SConfigLauncher)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    GUI_API SConfigLauncher() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~SConfigLauncher() throw();

    /// Set the action service is activated/disable.
    GUI_API virtual void setIsActive(bool isActive);

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
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IAction::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Management of observations ( overrides ).
     *
     * Stop configuration when it receives "WINDOW_CLOSED" event (ie. close the param view).
     */
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /**
     * @brief Starts the view and initialize the operator.
     */
    virtual void updating() throw(::fwTools::Failed);


    /**
     * @brief Declare the configuration to associate with an operator type and a view config
     *
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @verbatim
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
        @endverbatim
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

    ::fwServices::helper::ConfigLauncher::sptr m_configLauncher;
};

} // action
} // gui


#endif // __GUI_ACTION_SCONFIGLAUNCHER_HPP__


