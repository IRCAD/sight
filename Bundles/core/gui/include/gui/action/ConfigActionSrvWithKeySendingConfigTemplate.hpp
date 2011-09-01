/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_CONFIGACTIONSRVWITHKEYSENDINGCONFIGTEMPLATE_HPP_
#define _GUI_ACTION_CONFIGACTIONSRVWITHKEYSENDINGCONFIGTEMPLATE_HPP_

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <gui/action/ConfigActionSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{

/**
 * @class   ConfigActionSrvWithKeySendingConfigTemplate
 * @brief   To manage configuration file defines in xml extension.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * This action works on a ::fwData::Composite. It action sends a message containig
 *  - the identifier of the template configuration given in this action configuration.
 *  - the patterns to replace in the template configuration
 *  - the title
 *
 *  This action must be associated with the ::guiQt::editor::DynamicView which will receive this message.
 *  It creates a new tab in the view which manages the configuration.
 *  - You can specified pattern to replace in the template configuration by the tag 'replace'.
 *  - You can specified pattern to replace by the uid of the object contained by the composite with the given key
 *  - The title given by the attribute 'title' can be prefixed by a ::fwData::String contained in the composite with
 *  the key defined by 'titlePrefixKey' attribute.
 *  - The pattern GENERIC_UID is replaced by a generated unique identifier when the configuration is launch.
 *  This assure that the created object and services in the configuration have a unique uid even if this
 *  configuration is launch several times.
 *
 * Example of this service configuration
 * @verbatim
   <service implementation="::gui::action::ConfigActionSrvWithKeySendingConfigTemplate" type="::fwGui::IActionSrv">
       <config id="IdOfTemplateConfig" title="Visu 3D" titlePrefixKey="tabPrefix" tooltipKey="tabInfo" />
       <replace val="VALUE" pattern="PATTERN_TO_REPLACE_BY_VALUE" />
       <key id="KEY" pattern="PATTERN_TO_REPLACE_BY_UID_OF_KEY" />
   </service>
   @endverbatim
 * @see ::gui::action::ConfigActionSrv
 */
class GUI_CLASS_API ConfigActionSrvWithKeySendingConfigTemplate : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ConfigActionSrvWithKeySendingConfigTemplate)(::gui::action::ConfigActionSrv) ) ;

    /// Constructor. Do nothing.
    GUI_API ConfigActionSrvWithKeySendingConfigTemplate() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~ConfigActionSrvWithKeySendingConfigTemplate() throw() ;

protected:

    ///This method launches the IAction::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IAction::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Management of observations ( overrides ).
     *
     * Set executable or not the action according to the key specified in configuration
     */
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /**
     * @brief Starts the view and initialize the operator.
     */
    virtual void updating() throw(::fwTools::Failed);


    /**
     * @brief Declare the configuration to associate with an operator type and a view config
     *
     * Call the IAction::configuring()
     *
     *Example of this service configuration
     * @verbatim
       <service implementation="::gui::action::ConfigActionSrvWithKeySendingConfigTemplate" type="::fwGui::IActionSrv">
           <config id="IdOfTemplateConfig" title="Visu 3D" titlePrefixKey="tabPrefix" tooltipKey="tabInfo" />
           <replace val="VALUE" pattern ="PATTERN_TO_REPLACE_BY_VALUE" />
           <key id= "KEY" pattern = "PATIENT_TO_REPLACE_BY_KEY" />
       </service>
        @endverbatim
      */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;



private:

    void sendConfig();

    /// Id of plugin extension where the configuration is defined.
    std::string m_viewConfigId;

    /// Title of the future tab created by this action
    std::string m_viewConfigTitle;

    /// ToolTip of the future tab created by this action
    std::string m_tooltipConfigTitleKey;

    /// Icon of the future tab created by this action
    std::string m_iconConfigId;

    /// Title prefix of the future tab created by this action
    std::string m_viewConfigTitlePrefixKey;

    /// Specify if the config view may be closed
    bool m_closableConfig;

    /**
     * @brief keep the association between the PATTERN and the associated key as m_keyAdaptors[PATTERN] = AssociatedKey.
     */
    std::map< std::string, std::string > m_keyAdaptors;

    /**
     * @brief keep the association between the PATTERN and the associated key  as fieldAdaptors[PATTERN] = AssociatedKey.
     */
    ::fwData::Composite::sptr m_fieldAdaptors;

};

} //action
} // gui


#endif // _GUI_ACTION_CONFIGACTIONSRVWITHKEYSENDINGCONFIGTEMPLATE_HPP_


