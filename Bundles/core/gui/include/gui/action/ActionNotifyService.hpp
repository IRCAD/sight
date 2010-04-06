/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_ACTIONNOTIFYSERVICE_HPP_
#define _GUI_ACTION_ACTIONNOTIFYSERVICE_HPP_

#include <vector>
#include <fwServices/IService.hpp>

#include "gui/action/IAction.hpp"
#include "gui/export.hpp"


namespace gui
{

namespace action
{

/**
 * @brief   This action reset root object. All services are eliminated as well as objects composing the root object.
 * @class   CloseAction.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class GUI_CLASS_API ActionNotifyService : public ::gui::action::IAction
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ActionNotifyService)(::gui::action::IAction::Baseclass) ) ;

    /**
    * @brief Constructor. Do nothing.
    */
    GUI_API ActionNotifyService() throw();

    /**
    * @brief Destructor. Do nothing.
    */
    GUI_API virtual ~ActionNotifyService() throw();

protected:

    enum MessageType {
        REMOVE,
        ADD,
        ADD_OR_REMOVE,
        DO_NOTHING
    };

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

    /**
     * @brief This method starts-updates or stops the specified services
     */
    GUI_API void updating()throw (fwTools::Failed);    
    
    GUI_API void updating(::fwServices::ObjectMsg::csptr _msg )throw (fwTools::Failed);

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or stopped
     */
    GUI_API void configuring() throw( ::fwTools::Failed );

private:
    // vector representing uuid's services that must be started (true) or stopped (false)
    std::vector< std::pair< std::string, MessageType > > m_uuidServices;
    
    std::string m_onevent;
};


} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_ACTIONNOTIFYSERVICE_HPP_*/
