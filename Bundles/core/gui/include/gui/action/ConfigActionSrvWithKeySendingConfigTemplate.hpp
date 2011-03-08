#ifndef _GUI_ACTION_ConfigActionSrvWithKeySendingConfigTemplate_HPP_
#define _GUI_ACTION_ConfigActionSrvWithKeySendingConfigTemplate_HPP_

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

//#include <fwServices/AppConfigManager.hpp>

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
           <config id="IdOfExtension" />
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
} // GUI


#endif // _GUI_ACTION_ConfigActionSrvWithKeySendingConfigTemplate_HPP_


