/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_MOVELAYOUTACTIONSERVICE_HPP_
#define _GUI_ACTION_MOVELAYOUTACTIONSERVICE_HPP_

#include <boost/logic/tribool.hpp>

#include <vector>
#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{

/**
 * @class   ModifyLayoutActionSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class GUI_CLASS_API ModifyLayoutActionSrv : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ModifyLayoutActionSrv)(::fwGui::IActionSrv) ) ;
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
    * @brief Constructor. Do nothing.
    */
    GUI_API ModifyLayoutActionSrv() throw();

    /**
    * @brief Destructor. Do nothing.
    */
    GUI_API virtual ~ModifyLayoutActionSrv() throw();

protected:

    enum ActionType {
        MOVE,
        SHOW,
        HIDE,
        SHOW_OR_HIDE,
        ENABLE,
        DISABLE,
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

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or stopped
     */
    GUI_API void configuring() throw( ::fwTools::Failed );

    GUI_API virtual void starting() throw(::fwTools::Failed);

    GUI_API virtual void stopping() throw(::fwTools::Failed);

private:

    // < fwID, WIDContainer>
    typedef std::vector< std::pair< std::string, std::string > > MoveSrvVectType;
    // map representing fwID's services and new associated wid parent container
    MoveSrvVectType m_moveSrv;

    // < wid, showState>
    typedef std::vector< std::pair< std::string, ::boost::logic::tribool > > ShowSrvVectType;
    // map representing wid container and show state
    ShowSrvVectType m_showSrv;

    // < fwID, enableState>
    typedef std::vector< std::pair< std::string, bool > > EnableSrvVectType;
    // map representing fwID's services and enable state
    EnableSrvVectType m_enableSrv;
};


} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_MOVELAYOUTACTIONSERVICE_HPP_*/
