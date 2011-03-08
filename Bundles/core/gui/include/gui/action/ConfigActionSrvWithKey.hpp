#ifndef _GUI_ACTION_CONFIGACTIONSRVWITHKEY_HPP_
#define _GUI_ACTION_CONFIGACTIONSRVWITHKEY_HPP_

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
 * @class   ConfigActionSrvWithKey
 * @brief   To manage configuration file defines in xml extension.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class GUI_CLASS_API ConfigActionSrvWithKey : public ::gui::action::ConfigActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ConfigActionSrvWithKey)(::gui::action::ConfigActionSrv) ) ;

    /// Constructor. Do nothing.
    GUI_API ConfigActionSrvWithKey() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~ConfigActionSrvWithKey() throw() ;

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
       <service implementation="::gui::action::ConfigActionSrvWithKey" type="::fwGui::IActionSrv">
           <config id="IdOfExtension" />
           <replace val="VALUE" pattern ="PATTERN_TO_REPLACE_BY_VALUE" />
           <key id= "KEY" pattern = "PATIENT_TO_REPLACE_BY_KEY" />
       </service>
        @endverbatim
      * It MUST have at least one key node and at least one replace node.
      */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

private:

    /**
     * @brief Read the configuration and show the parameters view.
     */
    void startConfig();

    /**
     * @brief Close the parameters view.
     */
    void stopConfig();

    /**
     * @brief keep the association between the PATTERN and the associated key as m_keyAdaptors[PATTERN] = AssociatedKey.
     */
    std::map< std::string, std::string > m_keyAdaptors;

};

} //action
} // GUI


#endif // _GUI_ACTION_CONFIGACTIONSRVWITHKEY_HPP_


