#ifndef _GUI_ACTION_CONFIGACTIONSRV_HPP_
#define _GUI_ACTION_CONFIGACTIONSRV_HPP_

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/ConfigTemplateManager.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{

/**
 * @class   ConfigActionSrv
 * @brief   To manage configuration file defines in xml extension.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class GUI_CLASS_API ConfigActionSrv : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ConfigActionSrv)(::fwGui::IActionSrv) ) ;

    /// Constructor. Do nothing.
    GUI_API ConfigActionSrv() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~ConfigActionSrv() throw() ;

protected:


    ///This method launches the IAction::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IAction::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Management of observations ( overrides ).
     *
     * Stop configuration when it receives "WINDOW_CLOSED" event (ie. close the param view).
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
       <service implementation="::gui::action::ConfigActionSrv" type="::fwGui::IActionSrv">
           <config id="IdOfExtension" />
           <replace val="VALUE" pattern ="PATTERN_TO_REPLACE_BY_VALUE" />
       </service>
        @endverbatim
      * It MUST have at least one replace node.
      */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;
    /**
     * @brief Read the configuration and show the parameters view.
     */
    virtual void startConfig();

    /**
     * @brief Close the parameters view.
     */
    virtual void stopConfig();

    /**
     * @brief Add GENERIC_UID to field to adapt.
     */
    void AddGenericUidToFieldApadtor();


    /// Id of plugin extension where the configuration is defined.
    std::string m_viewConfigId;

    /// to know if config is running
    bool m_configIsRunning;

    // config manager
    ::fwServices::ConfigTemplateManager::sptr m_configTemplateManager;
    /**
     * @brief keep the association between the PATTERN and the associated key  as fieldAdaptors[PATTERN] = AssociatedKey.
     */
    std::map< std::string, std::string > m_fieldAdaptors;

};

} //action
} // GUI


#endif // _GUI_ACTION_CONFIGACTIONSRV_HPP_


