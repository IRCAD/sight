/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_SCONFIGCONTROLLER_HPP_
#define _FWSERVICES_SCONFIGCONTROLLER_HPP_

#include <map>

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include "fwServices/config.hpp"
#include "fwServices/IController.hpp"
#include "fwServices/AppConfigManager.hpp"

namespace fwServices
{

/**
 * @class   SConfigController
 * @brief   To manage configuration file defines in xml extension.
 * @author  IRCAD (Research and Development Team).
 * @date    2010-2012.
 *
 * This action works on a ::fwData::Composite. It action starts/stops a template configuration given by its identifier in this action configuration.
 *  - You can specified pattern to replace in the template configuration by the tag 'replace'.
 *  - You can specified pattern to replace by the uid of the object contained by the composite with the given key
 *  - The pattern GENERIC_UID is replaced by a generated unique identifier when the configuration is launch.
 *  This assure that the created object and services in the configuration have a unique uid even if this
 *  configuration is launch several times.
 *
 * Example of this service configuration
 * @verbatim
   <service implementation="::gui::action::SConfigController" type="::fwGui::IActionSrv">
       <config id="IdOfTemplateConfig" />
       <replace val="VALUE" pattern="PATTERN_TO_REPLACE_BY_VALUE" />
       <key id="KEY" pattern="PATTERN_TO_REPLACE_BY_UID_OF_KEY" />
   </service>
   @endverbatim
 */
class FWSERVICES_CLASS_API SConfigController : public ::fwServices::IController
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SConfigController)(::fwServices::IController) ) ;

    /// Constructor. Does nothing.
    FWSERVICES_API SConfigController() throw() ;

    /// Destructor. Does nothing.
    FWSERVICES_API virtual ~SConfigController() throw() ;

protected:

    /// Starts the config
    virtual void starting() throw(::fwTools::Failed);

    /// Stops the config
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating() throw(::fwTools::Failed);


    /**
     * @brief Declare the configuration to associate with an operator type and a view config
     *
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @verbatim
       <service implementation="::fwServices::SConfigController" type="::fwServices::IController">
           <config id="IdOfTemplateConfig" />
           <replace val="VALUE" pattern="PATTERN_TO_REPLACE_BY_VALUE" />
           <key id="KEY" pattern="PATTERN_TO_REPLACE_BY_UID_OF_KEY" />
       </service>
        @endverbatim
     * It MUST have at least one key node and at least one replace node.
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

private:

    /// Starts config
    void startConfig();

    /// Stops config
    void stopConfig();

    /// Adds GENERIC_UID to field to adapt
    void addGenericUidToFieldApadtor();

    /// keep the association between the PATTERN and the associated key  as fieldAdaptors[PATTERN] = AssociatedKey.
    std::map< std::string, std::string > m_fieldAdaptors;

    /// keep the association between the PATTERN and the associated key as m_keyAdaptors[PATTERN] = AssociatedKey.
    std::map< std::string, std::string > m_keyAdaptors;

    /// Id of plugin extension where the configuration is defined.
    std::string m_viewConfigId;

    /// Config manager
    ::fwServices::AppConfigManager::sptr m_configTemplateManager;

    /// Defines a special key to defines the associated object him self
    static const std::string s_SELF_KEY;
};

} // fwServices


#endif // _FWSERVICES_SCONFIGCONTROLLER_HPP_


